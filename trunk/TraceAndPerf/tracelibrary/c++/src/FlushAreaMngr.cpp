/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <locale.h>

#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/System.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/osthreads/ThreadObserver.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <TraceAndPerf/tracelibrary/FilterMngr.hpp>
#include <TraceAndPerf/tracelibrary/FlushAreaMngr.hpp>
#include <TraceAndPerf/tracelibrary/StreamMngr.hpp>
#include <TraceAndPerf/idllib/CdmwTraceCommon.stub.hpp>

#include "tracelibrary/AreaFlusher.hpp"
#include "tracelibrary/FlushArea.hpp"
#include "tracelibrary/TraceProducer_impl.hpp"
#include "tracelibrary/TraceTools.hpp"

#include <Foundation/logging/LogManager.hpp> // ECR-0169
#include "tracelibrary/LogManagerTraceDelegate.hpp" // ECR-0169


namespace
{
    void print_exception(const CORBA::SystemException & ex)
    {
        std::cerr << "CORBA system Exception : \n" << ex << std::endl;
    }

    void print_exception(const CORBA::Exception & ex)
    {
        std::cerr << "CORBA Exception : \n" << ex << std::endl;
    }

    void print_exception(const Cdmw::Exception & ex)
    {
        std::cerr << "CDMW Exception : \n" << ex.what() << std::endl;
    }
}; // End anonymous namespace



namespace Cdmw
{

// Default internal Cdmw trace levels
const char* CDMW_NREP = "CDMW_NREP";
const char* CDMW_NINT = "CDMW_NINT";
const char* CDMW_LFCL = "CDMW_LFCL";
const char* CDMW_INIT = "CDMW_INIT";

namespace Trace
{

/**
 *Implementation notes:  none
 *<p>
 *Portability issues: none
 */


static const char* p_outFlushAreaMsg =
    "[FlushAreaMngr] running out of flush areas";


/**
 * This class handle automatic registration of all thread
 * created by the OsSupport thread library
 */
class TraceAutomaticRegisterer : public OsSupport::ThreadObserver
{
    public:
        virtual void
        onEnterRun()
        {
            Cdmw::Trace::StreamMngr::instance().register_thread();
        }


        virtual void
        onLeaveRun()
        {
            Cdmw::Trace::StreamMngr::instance().unregister_thread();

        }
};


TraceAutomaticRegisterer traceAutomaticRegisterer;


/**
 * This variable is set to true, when the FlushAreaMngr::init() function
 * has been called.
 */
bool FlushAreaMngr::M_initDone = false;


/**
 * Contains the FlushAreaMngr singleton initialised by the
 * FlushAreaMngr::init() function.
 */
FlushAreaMngr* FlushAreaMngr::M_pSingleton = NULL;

/**
 * Contains the LogManagerDelegate singleton initialised by the
 * FlushAreaMngr::init() function.
 */
Cdmw::Logging::LogManagerDelegate* FlushAreaMngr::M_LogManagerDelegate = NULL;

//
// Ctor
//
FlushAreaMngr::FlushAreaMngr(PortableServer::POA_ptr tracePOA,
                             std::list<CdmwTrace::TraceProducer::CollectorData>& collectorList,
                             const std::string& domain,
                             long level,
                             const std::string& applicationName,
                             const std::string& processName,
                             size_t flushingTime,
                             size_t nbFlushArea,
                             size_t sizeFlushArea,
                             size_t areaNbMessage,
                             size_t msgThreshold)
    throw(OutOfMemoryException,
          BadParameterException,
          InternalErrorException)

    : m_spAreaFlusher(NULL),
      m_flushModeThread(ASYNCHRONOUS),
      m_flushModeBackup(LINEAR),
      m_barrier(2),
      m_flushingTime(flushingTime),
      m_msgThreshold(msgThreshold),
      m_lastMsgCount(0),
      m_lastMsgHeader(),
      m_lastMsgBody(""),
      m_flush_active(false),
      m_flushingTimeout_cond(m_currentFlushArea_mtx),
      m_thread_must_terminate(false),
      m_pFilterMngr(NULL),
      m_applicationName(applicationName),
      m_processName(processName)
{
    try
    {
        // get and store the process id
        m_processId = OsSupport::OS::get_processId();

        // We create the required number of FlushArea
        create_FlushAreas(nbFlushArea, sizeFlushArea,
                          std::strlen(p_outFlushAreaMsg) + 1,
                          areaNbMessage);

        // We ask to be notify each time a new thread is created
        OsSupport::Thread::register_observer(&traceAutomaticRegisterer);

        // We create the thread in charge of flushing Area as a background task
#       ifdef _MSC_VER
        // The auto_ptr implementation shipped with visual is not compliant
        // and doesn't provide reset(), but it broken as it used an assignement
        // operator with const auto_ptr&, we use this deviation

        // We create the thread in charge of communicating with collectors
        m_spAreaFlusher = std::auto_ptr<AreaFlusher>(new AreaFlusher(m_barrier));
#       else
        m_spAreaFlusher.reset(new AreaFlusher(m_barrier));
#       endif

        // We create the object in charge of determining, if a specific
        // level/domain must be traced
        m_pFilterMngr = new FilterMngr();

        // initialize the domain and level to trace if defined
        if (domain != "") {
            // ECR-0123
            activate_level(CdmwTrace::ALL_COMPONENT_NAMES, domain, level);
        }

        // We must create the Servant to handle registration request
        // (ref count is incremented)
        TraceProducer_impl* pTraceProducerServant =
                new TraceProducer_impl(m_spAreaFlusher.get(),
                                       m_pFilterMngr);

        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the
        // method end)
        PortableServer::ServantBase_var servant_var = pTraceProducerServant;

        try {
            // activate servant on POA (ref count is incremented)
            PortableServer::ObjectId_var oid =
                tracePOA->activate_object(pTraceProducerServant);
            CORBA::Object_var object = tracePOA->id_to_reference(oid.in());
            m_traceProducer = CdmwTrace::TraceProducer::_narrow(object.in());
        } catch (const PortableServer::POA::WrongPolicy&) {
            CDMW_THROW2(Cdmw::BadParameterException,
                        "Wrong POA Policy for activate_object",
                        "NULL");
        } catch (const CORBA::Exception& ex) {
            std::cerr << "Trace Producer servant activation error" << "\n";
            print_exception(ex);
            CDMW_ASSERT(0);
        }

        // notify each collector of birth of TraceProducer
        std::list<CdmwTrace::TraceProducer::CollectorData>::iterator
            itCollectorList = collectorList.begin();

        for (; itCollectorList != collectorList.end() ; itCollectorList++)
        {
            // collector object name
            std::string collector_obj_name;

            try {
                // get collector object from list (the_collector is a _var)
                CdmwTrace::Collector_var collector =
                    itCollectorList->the_collector;
                // get collector object name from list
                collector_obj_name =
                    itCollectorList->the_collectorObjName.in();
                // get collector mnemonic name from list
                std::string collector_mnemonic =
                    itCollectorList->the_collectorMnemoName.in();

                if (! CORBA::is_nil(collector.in())) {
                    // call collector update to register the trace producer
                    collector->update(m_traceProducer.in());
                }

                // register collector in areaFlusher
                m_spAreaFlusher->register_collector(collector.in(),
                                                    collector_obj_name.c_str(),
                                                    collector_mnemonic.c_str());
            } catch (const CORBA::Exception& ex) {
                std::cerr << "Trace Collector access error : "
                          << collector_obj_name.c_str() << "\n";
                print_exception(ex);
            }
        }

        try {
            // Ok, we start the background thread in charge of fluhsing data
            m_spAreaFlusher->start();

            // Before returning, we wait it declares itself as operational
            m_barrier.wait();
        } catch (const OsSupport::ThreadSchedulingException&) {
            // We have not chosen to assign specific right to the thread
            // we are corrupted
            CDMW_THROW(InternalErrorException);
        }
    } catch (const std::bad_alloc&) {
        CDMW_THROW(OutOfMemoryException);
    }
}


/**
 * Constructor.
 */
// ECR-0123
FlushAreaMngr::FlushAreaMngr(PortableServer::POA_ptr tracePOA,
                             std::list<CdmwTrace::TraceProducer::CollectorData>& collectorList,
                             const std::string& componentName,
                             const std::string& domain,
                             long level,
                             const std::string& applicationName,
                             const std::string& processName,
                             size_t flushingTime,
                             size_t nbFlushArea,
                             size_t sizeFlushArea,
                             size_t areaNbMessage,
                             size_t msgThreshold)
    throw(OutOfMemoryException,
          BadParameterException,
          InternalErrorException)

    : m_spAreaFlusher(NULL),
      m_flushModeThread(ASYNCHRONOUS),
      m_flushModeBackup(LINEAR),
      m_barrier(2),
      m_flushingTime(flushingTime),
      m_msgThreshold(msgThreshold),
      m_lastMsgCount(0),
      m_lastMsgHeader(),
      m_lastMsgBody(""),
      m_flush_active(false),
      m_flushingTimeout_cond(m_currentFlushArea_mtx),
      m_thread_must_terminate(false),
      m_pFilterMngr(NULL),
      m_applicationName(applicationName),
      m_processName(processName)
{
    try
    {
        // get and store the process id
        m_processId = OsSupport::OS::get_processId();

        // We create the required number of FlushArea
        create_FlushAreas(nbFlushArea, sizeFlushArea,
                          std::strlen(p_outFlushAreaMsg) + 1,
                          areaNbMessage);

        // We ask to be notify each time a new thread is created
        OsSupport::Thread::register_observer(&traceAutomaticRegisterer);

        // We create the thread in charge of flushing Area as a background task
#       ifdef _MSC_VER
        // The auto_ptr implementation shipped with visual is not compliant
        // and doesn't provide reset(), but it broken as it used an assignement
        // operator with const auto_ptr&, we use this deviation

        // We create the thread in charge of communicating with collectors
        m_spAreaFlusher = std::auto_ptr<AreaFlusher>(new AreaFlusher(m_barrier));
#       else
        m_spAreaFlusher.reset(new AreaFlusher(m_barrier));
#       endif

        // We create the object in charge of determining, if a specific
        // level/domain must be traced
        m_pFilterMngr = new FilterMngr();

        // initialize the domain and level to trace if defined
        if (domain != "") {
            // ECR-0123
            activate_level(componentName, domain, level);
        }

        // We must create the Servant to handle registration request
        // (ref count is incremented)
        TraceProducer_impl* pTraceProducerServant =
                new TraceProducer_impl(m_spAreaFlusher.get(),
                                       m_pFilterMngr);

        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the
        // method end)
        PortableServer::ServantBase_var servant_var = pTraceProducerServant;

        try {
            // activate servant on POA (ref count is incremented)
            PortableServer::ObjectId_var oid =
                tracePOA->activate_object(pTraceProducerServant);
            CORBA::Object_var object = tracePOA->id_to_reference(oid.in());
            m_traceProducer = CdmwTrace::TraceProducer::_narrow(object.in());
        } catch (const PortableServer::POA::WrongPolicy&) {
            CDMW_THROW2(Cdmw::BadParameterException,
                        "Wrong POA Policy for activate_object",
                        "NULL");
        } catch (const CORBA::Exception& ex) {
            std::cerr << "Trace Producer servant activation error" << "\n";
            print_exception(ex);
            CDMW_ASSERT(0);
        }

        // notify each collector of birth of TraceProducer
        std::list<CdmwTrace::TraceProducer::CollectorData>::iterator
            itCollectorList = collectorList.begin();

        for (; itCollectorList != collectorList.end() ; itCollectorList++)
        {
            // collector object name
            std::string collector_obj_name;

            try {
                // get collector object from list (the_collector is a _var)
                CdmwTrace::Collector_var collector =
                    itCollectorList->the_collector;
                // get collector object name from list
                collector_obj_name =
                    itCollectorList->the_collectorObjName.in();
                // get collector mnemonic name from list
                std::string collector_mnemonic =
                    itCollectorList->the_collectorMnemoName.in();

                if (! CORBA::is_nil(collector.in())) {
                    // call collector update to register the trace producer
                    collector->update(m_traceProducer.in());
                }

                // register collector in areaFlusher
                m_spAreaFlusher->register_collector(collector.in(),
                                                    collector_obj_name.c_str(),
                                                    collector_mnemonic.c_str());
            } catch (const CORBA::Exception& ex) {
                std::cerr << "Trace Collector access error : "
                          << collector_obj_name.c_str() << "\n";
                print_exception(ex);
            }
        }

        try {
            // Ok, we start the background thread in charge of fluhsing data
            m_spAreaFlusher->start();

            // Before returning, we wait it declares itself as operational
            m_barrier.wait();
        } catch (const OsSupport::ThreadSchedulingException&) {
            // We have not chosen to assign specific right to the thread
            // we are corrupted
            CDMW_THROW(InternalErrorException);
        }
    } catch (const std::bad_alloc&) {
        CDMW_THROW(OutOfMemoryException);
    }
}


//
// Initialise the trace library
//
CdmwTrace::TraceProducer_ptr
FlushAreaMngr::init(PortableServer::POA_ptr tracePOA,
                    std::list<CdmwTrace::TraceProducer::CollectorData>& collectorList,
                    const std::string& domain,
                    long level,
                    const std::string& applicationName,
                    const std::string& processName,
                    size_t flushingTime,
                    size_t nbFlushArea,
                    size_t sizeFlushArea,
                    size_t areaNbMessage,
                    size_t msgThreshold)

    throw(OutOfMemoryException,
          BadParameterException,
          InternalErrorException)
{
    // The user try to initialise twice the tracelibrary
    CDMW_ASSERT(! M_initDone);

    // for international char management
    // set for all locale categories the traditional unix system behavior
    ::setlocale (LC_ALL,"C");

    try {
        M_pSingleton = new FlushAreaMngr(tracePOA,
                                         collectorList,
                                         CdmwTrace::ALL_COMPONENT_NAMES, // ECR-0123
                                         domain,
                                         level,
                                         applicationName,
                                         processName,
                                         flushingTime,
                                         nbFlushArea,
                                         sizeFlushArea,
                                         areaNbMessage,
                                         msgThreshold);

        // We start the thread in charge of flushing in case of timeout
        M_pSingleton->start();
    } catch (const std::bad_alloc&) {
        CDMW_THROW(OutOfMemoryException);
    }

    M_initDone = true;

    // ECR-0169
    // register the log manager delegate.
    M_LogManagerDelegate=new Cdmw::Trace::LogManagerTraceDelegate(*M_pSingleton);
    Cdmw::Logging::LogManager* logMngr = Cdmw::Logging::LogManager::Instance();
    logMngr->register_alternate_delegate(M_LogManagerDelegate);

    // We return the reference on the object managing communication with
    // collector, it will be registered in the repository by the caller
    return CdmwTrace::TraceProducer::_duplicate(
               M_pSingleton->m_traceProducer.in());
}


/**
 * Initialize the Trace library.
 */
// ECR-0123
CdmwTrace::TraceProducer_ptr
FlushAreaMngr::init(PortableServer::POA_ptr tracePOA,
                    std::list<CdmwTrace::TraceProducer::CollectorData>& collectorList,
                    const std::string& componentName,
                    const std::string& domain,
                    long level,
                    const std::string& applicationName,
                    const std::string& processName,
                    size_t flushingTime,
                    size_t nbFlushArea,
                    size_t sizeFlushArea,
                    size_t areaNbMessage,
                    size_t msgThreshold)
    throw(OutOfMemoryException,
          BadParameterException,
          InternalErrorException)
{
    // The user try to initialise twice the tracelibrary
    CDMW_ASSERT(! M_initDone);

    // for international char management
    // set for all locale categories the traditional unix system behavior
    ::setlocale (LC_ALL,"C");

    try {
        M_pSingleton = new FlushAreaMngr(tracePOA,
                                         collectorList,
                                         componentName, // ECR-0123
                                         domain,
                                         level,
                                         applicationName,
                                         processName,
                                         flushingTime,
                                         nbFlushArea,
                                         sizeFlushArea,
                                         areaNbMessage,
                                         msgThreshold);

        // We start the thread in charge of flushing in case of timeout
        M_pSingleton->start();
    } catch (const std::bad_alloc&) {
        CDMW_THROW(OutOfMemoryException);
    }

    M_initDone = true;

    // ECR-0169
    // register the log manager delegate.
    M_LogManagerDelegate=new Cdmw::Trace::LogManagerTraceDelegate(*M_pSingleton);
    Cdmw::Logging::LogManager* logMngr = Cdmw::Logging::LogManager::Instance();
    logMngr->register_alternate_delegate(M_LogManagerDelegate);


    // We return the reference on the object managing communication with
    // collector, it will be registered in the repository by the caller
    return CdmwTrace::TraceProducer::_duplicate(
               M_pSingleton->m_traceProducer.in());
}


//
// Free memory and terminate the Trace service
//
void
FlushAreaMngr::cleanup()
    throw(InternalErrorException)
{
    // if library is initialised
    if (M_initDone)
    { 
        // ECR-0169
        // unregister the log manager delegate.
        Cdmw::Logging::LogManager* logMngr = Cdmw::Logging::LogManager::Instance();
        logMngr->unregister_alternate_delegate(M_LogManagerDelegate);

        // We stop our internal thread managing the timeout
        M_pSingleton->m_thread_must_terminate = true;
        M_pSingleton->m_flushingTimeout_cond.broadcast();

        // We wait for the completion of the internal thread
        M_pSingleton->join();

        // We stop the thread in charge of flushing data
        M_pSingleton->m_spAreaFlusher->stop_thread();

        // And delete all used Area
        M_pSingleton->delete_FlushAreas();

        M_initDone = false;

        delete M_LogManagerDelegate;
        M_LogManagerDelegate = NULL;

        delete M_pSingleton;

        M_pSingleton = NULL;
    }
}


//
// Dtor
//
FlushAreaMngr::~FlushAreaMngr()
    throw()
{
    if (m_pFilterMngr != NULL)
    {
        delete m_pFilterMngr;
    }

    // The user has not called the cleanup function
    CDMW_ASSERT(M_initDone == false);

    // We ask to be no more notified on the Thread event
    OsSupport::Thread::unregister_observer(&traceAutomaticRegisterer);
}


//
// Run
//
void
FlushAreaMngr::run()
    throw()
{
    using namespace OsSupport;

    try
    {
        CDMW_MUTEX_GUARD(m_currentFlushArea_mtx);

        Condition::WaitReturn status;

        while (! m_thread_must_terminate) {
            // arm timer and wait it
            status = m_flushingTimeout_cond.wait (m_flushingTime);

            // The time has expired, we must flush the area
            if (status == Condition::TIMEDOUT) {
                try {
                    flush_area();

                    if (m_lastMsgCount > 1) {
                        insert_info_message();
                    }
                } catch(...) {
                    std::cerr << "Exception raised in trace library"
                              << std::endl;
                }

                // We have been signaled, maybe we must exit, or
                // just re-arm the timeout
            }
        }
    }

    // catch any exception from Mutex and assert it
    catch (...) {
      CDMW_ASSERT(false == true);
    }
}


//
// Return the singleton.
//
FlushAreaMngr&
FlushAreaMngr::instance()
    throw()
{
    // The user has not called the init() function
    CDMW_ASSERT(M_initDone == true);

    return *M_pSingleton;
}


//
// Flush the current FlushArea
//
void
FlushAreaMngr::flush_area()
    throw(OutOfMemoryException,
          InternalErrorException)
{
    CDMW_TRACE_DEBUG("FlushAreaMngr::flush_area()");

    // BE CAREFUL: m_currentFlushArea_mtx mutex must have been
    // acquired

    switch (m_flushModeBackup)
    {
        case LINEAR:
            // Deliver the current flush area to the area flusher
            deliver_flushArea();
            break;

        case CIRCULAR:
            // In circular mode, the area is never flushed.
            // The contents of this area may be inspected
            // only with a debugger.

            // The current area is simply reset
            m_pCurrentFlushArea->reset();
            break;

        default:
           CDMW_NEVER_HERE();
           break;
    };
}


//
// Add a message to the flush area.
//
bool
FlushAreaMngr::add_message(const Message::MessageHeader& header,
                           const std::string& body)
    throw(InternalErrorException)
{
    bool ret = false;

    try
    {
        CDMW_ASSERT(m_spAreaFlusher.get() != NULL);

        CDMW_MUTEX_GUARD (m_currentFlushArea_mtx);

        bool loop = false;
        bool is_sameMsg = false;

        if (m_lastMsgCount == 0) {
            // we do not have a last message
            // (possibly because it is the first message or
            // because we have just flushed the area)
            // so do not waste time comparing anything

            // make a copy so that we can compare it with the next message
            m_lastMsgHeader = header;
            m_lastMsgBody = body;

            // actually, we have a "last" message now
            m_lastMsgCount = 1;
        } else {
            // we use the Message::headercmp() static method instead
            // of redefining the operator== in MessageHeader because
            // the time stamp is ignored here while using the operator==
            // would mean that even the time stamps shall be equal
            is_sameMsg = ((Message::headercmp(m_lastMsgHeader, header)) &&
                          (m_lastMsgBody == body));

            if (is_sameMsg) {
                // same message, let us increment the counter
                m_lastMsgCount++;

                // need to flush?
                if (m_lastMsgCount == m_msgThreshold) {
                    ret = insert_info_message();
                } else {
                    ret = true;
                }
            } else {
                // new message

                if ((m_lastMsgCount == 1) ||
                    ((m_lastMsgCount > 1) && insert_info_message()))
                {
                    // make a copy
                    m_lastMsgHeader = header;
                    m_lastMsgBody = body;

                    // adjust the counter
                    m_lastMsgCount = 1;
                } else {
                    ret = false;
                }
            }
        }

        if (m_lastMsgCount == 1)
        {
            do
            {
                loop = false;

                if (m_pCurrentFlushArea == NULL) {
                    CDMW_TRACE_DEBUG(
                        "Warning : Cdmw::Trace::FlushAreaMngr :");
                    CDMW_TRACE_DEBUG(
                        "No more flush area available at the moment"
                        << std::endl);

                    // clean last message
                    m_lastMsgBody = "";

                    break;
                }

                FlushArea::AreaStatus status =
                    m_pCurrentFlushArea->add_message(header, body);

                switch (status)
                {
                    case FlushArea::OK:
                        {
                            ret = true;
                        }
                        break;

                    case FlushArea::AREA_FULL:
                        {
                            // flush immediately the current area
                            flush_area();

                            // reset the timeout
                            m_flushingTimeout_cond.broadcast();

                            ret = true;
                        }
                        break;

                    case FlushArea::SIZE_TOO_SMALL:
                        {
                            // flush immediately the current area
                            flush_area();

                            // try to use the next current flush area
                            loop = true;
                        }
                        break;

                    case FlushArea::OUT_OF_MEMORY:
                        {
                            CDMW_TRACE_DEBUG(
                                "Warning : Cdmw::Trace::FlushAreaMngr :");
                            CDMW_TRACE_DEBUG(
                                "Out of memory." << std::endl);
                        }
                        break;

                    case FlushArea::MESSAGE_TOO_BIG:
                        {
                            CDMW_TRACE_DEBUG(
                                "Warning : Cdmw::Trace::FlushAreaMngr :");
                            CDMW_TRACE_DEBUG(
                                "Inappropriate area size. Cannot manage message with body length of "
                                << body.length()
                                << " characters."
                                << std::endl);
                        }
                        break;
                }
            } while (loop);
        }
    }

    catch(InternalErrorException&)
    {
        throw;
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
        CDMW_ASSERT(false == true);
    }

    return ret;
}


/**
 * Insert an info message about the number of times
 * a message has been added to the flush area.
 *
 * This method assumes that the m_currentFlushArea_mtx
 * mutex has been acquired by the caller and therefore
 * will not make any attempt to acquire the mutex.
 */
bool
FlushAreaMngr::insert_info_message()
    throw(InternalErrorException)
{
    bool retval = false;

    if (m_lastMsgCount > 1)
    {
        try
        {
            CDMW_ASSERT(m_spAreaFlusher.get() != NULL);

            bool loop = false;

            // update the time stamp in the header of the last message
            Message::MessageHeader infoMsgHeader = m_lastMsgHeader;
            infoMsgHeader.m_timestamping = Cdmw::OsSupport::OS::get_time();

            // info message
            std::ostringstream oss;
            oss << "Last message repeated "
                << m_lastMsgCount
                << " times." << std::ends;
            std::string infoMsgBody = oss.str();

            do
            {
                loop = false;

                if (m_pCurrentFlushArea == NULL) {
                    CDMW_TRACE_DEBUG("Warning : Cdmw::Trace::FlushAreaMngr :");
                    CDMW_TRACE_DEBUG(
                        "No more flush area available at the moment"
                        << std::endl);
                    break;
                }

                FlushArea::AreaStatus status =
                    m_pCurrentFlushArea->add_message(infoMsgHeader,
                                                     infoMsgBody);

                switch (status)
                {
                    case FlushArea::OK:
                        {
                            retval = true;
                        }
                        break;

                    case FlushArea::AREA_FULL:
                        {
                            // flush immediately the current area
                            flush_area();

                            // reset the timeout
                            m_flushingTimeout_cond.broadcast();

                            retval = true;
                        }
                        break;

                    case FlushArea::SIZE_TOO_SMALL:
                        {
                            // flush immediately the current area
                            flush_area();

                            // try to use the next current flush area
                            loop = true;
                        }
                        break;

                    case FlushArea::OUT_OF_MEMORY:
                        {
                            CDMW_TRACE_DEBUG(
                                "Warning : Cdmw::Trace::FlushAreaMngr :");
                            CDMW_TRACE_DEBUG("Out of memory." << std::endl);
                        }
                        break;

                    case FlushArea::MESSAGE_TOO_BIG:
                        {
                            CDMW_TRACE_DEBUG(
                                "Warning : Cdmw::Trace::FlushAreaMngr :");
                            CDMW_TRACE_DEBUG(
                                "Inappropriate area size. Cannot manage message with body length of "
                                << infoMsgBody.length()
                                << " characters."
                                << std::endl);
                        }
                        break;
                }
            } while (loop);
        }

        catch(InternalErrorException&)
        {
            throw;
        }

        // catch any exception from Mutex and assert it
        catch (...)
        {
            CDMW_ASSERT(false == true);
        }
    }

    if (retval) {
        m_lastMsgCount = 0;
        m_lastMsgBody = "";
    }

    return retval;
}


//
//
//
void
FlushAreaMngr::deliver_flushArea()
    throw(OutOfMemoryException,
          InternalErrorException)
{
    // BE CAREFUL: m_currentFlushArea_mtx mutex must have been
    // acquired

    CDMW_ASSERT(m_spAreaFlusher.get() != NULL);

    if (m_pCurrentFlushArea != NULL) {
        if (!m_pCurrentFlushArea->is_empty()) {
            // save current flush area pointer
            FlushArea* p_flushAreaToDeliver = m_pCurrentFlushArea;

            // update the current flush area
            update_currentFlushArea();

            // if no more flush area available,
            // store out of flush area error message
            if (m_pCurrentFlushArea == NULL)
            {
                std::string outFlushAreaMsg = p_outFlushAreaMsg;
                p_flushAreaToDeliver->store_outFlushAreaMsg(outFlushAreaMsg);
            }

            // actually deliver to the area flusher
            m_spAreaFlusher->flush_area (p_flushAreaToDeliver);
        }
    }
}


//
//
//
void
FlushAreaMngr::update_currentFlushArea()
    throw()
{
    try
    {
        CDMW_MUTEX_GUARD(m_flushAreaMngr_mtx);

        if (! m_vpFlushArea.empty()) {
            // updates the current flush area with the first
            // element of the available flush areas.
            // The latter is removed from the vector.
            m_pCurrentFlushArea = m_vpFlushArea.front();
            m_vpFlushArea.erase (m_vpFlushArea.begin());
        } else {
            m_pCurrentFlushArea = NULL;
        }
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
        CDMW_ASSERT(false == true);
    }
}


//
//
//
void
FlushAreaMngr::put_flushedArea(FlushArea* pFlushedArea)
    throw(OutOfMemoryException)
{
    try
    {
        CDMW_ASSERT(pFlushedArea != NULL);

        if (m_pCurrentFlushArea == NULL) {
            // Reuse immediately the flushed area
            CDMW_MUTEX_GUARD(m_currentFlushArea_mtx);
            m_pCurrentFlushArea = pFlushedArea;

            // reset the timeout
            m_flushingTimeout_cond.broadcast();
        } else {
            // add the flushed area to the list of available areas
            CDMW_MUTEX_GUARD(m_flushAreaMngr_mtx);
            m_vpFlushArea.push_back (pFlushedArea);
        }
    }

    catch (const std::bad_alloc&)
    {
        CDMW_THROW(OutOfMemoryException);
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
        CDMW_ASSERT(false == true);
    }
}


//
// Create the number of FlushArea specified
//
void
FlushAreaMngr::create_FlushAreas(size_t nbFlushArea,
                                 size_t sizeFlushArea,
                                 size_t sizeOutFlushAreaMsg,
                                 size_t areaNbMessage)
    throw(OutOfMemoryException)
{
    try
    {
        CDMW_MUTEX_GUARD(m_flushAreaMngr_mtx);

        m_pCurrentFlushArea = new FlushArea(areaNbMessage,
                                            sizeFlushArea,
                                            sizeOutFlushAreaMsg,
                                            0);

        for (unsigned int iArea = 1 ; iArea < nbFlushArea ; ++iArea) {
            m_vpFlushArea.push_back(new FlushArea(areaNbMessage,
                                                  sizeFlushArea,
                                                  sizeOutFlushAreaMsg,
                                                  iArea));
        }
    }

    catch (const std::bad_alloc&)
    {
        CDMW_THROW(OutOfMemoryException);
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
        CDMW_ASSERT(false == true);
    }
}


//
// Delete the FlushArea create by create_FlushAreas()
//
void
FlushAreaMngr::delete_FlushAreas()
    throw()
{
    try
    {
        CDMW_MUTEX_GUARD(m_flushAreaMngr_mtx);

        if (m_pCurrentFlushArea != NULL) {
            delete m_pCurrentFlushArea;
        }

        for (size_t iArea=0 ; iArea < m_vpFlushArea.size() ; ++iArea) {
            delete m_vpFlushArea[iArea];
        }

        // We delete the pointer of the old one
        m_vpFlushArea.erase(m_vpFlushArea.begin(), m_vpFlushArea.end());
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
        CDMW_ASSERT(false == true);
    }
}


//
// Activate message flushing
//
void
FlushAreaMngr::activate_flushing()
    throw()
{
    m_flush_active = true;
    m_spAreaFlusher->activate_flushing();
}


//
// Deactivate message flushing
//
void
FlushAreaMngr::deactivate_flushing()
    throw()
{
    m_flush_active = false;
    m_spAreaFlusher->deactivate_flushing();
}


//
// Change the current flushing mode
//
void
FlushAreaMngr::set_flushMode(FlushMode flushMode)
    throw()
{
    try
    {
        CDMW_MUTEX_GUARD(m_flushAreaMngr_mtx);

        switch (flushMode)
        {
            case CIRCULAR:
                m_flushModeBackup = CIRCULAR;
                m_flushModeThread = SEMI_SYNCHRONOUS;
                break;

            case LINEAR:
                m_flushModeBackup = LINEAR;
                break;

            case ASYNCHRONOUS:
                m_flushModeBackup = LINEAR;
                m_flushModeThread = ASYNCHRONOUS;

                m_spAreaFlusher->set_flushState (AreaFlusher::ASYNCHRONOUS);
                break;


            case SEMI_SYNCHRONOUS:
                m_flushModeThread = SEMI_SYNCHRONOUS;
                m_spAreaFlusher->set_flushState (AreaFlusher::SYNCHRONOUS);
                break;

            default:
                CDMW_NEVER_HERE();
                break;
        }
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
        CDMW_ASSERT(false == true);
    }
}


//
// Define the specified level as activated
//
void
FlushAreaMngr::activate_level(const std::string& domain, long level)
    throw(OutOfMemoryException)
{
    // ECR-0123
    // m_pFilterMngr->activate_level(domain, level);
    activate_level(CdmwTrace::ALL_COMPONENT_NAMES, domain, level);
}


/**
 * Activate the specified level.
 */
// ECR-0123
void
FlushAreaMngr::activate_level(const std::string& componentName,
                              const std::string& domain,
                              long level)
    throw(OutOfMemoryException)
{
    m_pFilterMngr->activate_level(componentName, domain, level);
}


//
// Define the specified level as de-activated
//
void
FlushAreaMngr::deactivate_level(const std::string& domain, long level)
    throw(OutOfMemoryException)
{
    // ECR-0123
    // m_pFilterMngr->deactivate_level(domain, level);
    deactivate_level(CdmwTrace::ALL_COMPONENT_NAMES, domain, level);
}


/**
 * Deactivate the specified level.
 */
// ECR-0123
void
FlushAreaMngr::deactivate_level(const std::string& componentName,
                                const std::string& domain,
                                long level)
    throw(OutOfMemoryException)
{
    m_pFilterMngr->deactivate_level(componentName, domain, level);
}


//
// returns true if the specified level is to be traced
//
bool
FlushAreaMngr::is_to_be_traced(const std::string& domain, long level)
    throw(OutOfMemoryException)
{
    // ECR-0123
    return is_to_be_traced(CdmwTrace::ALL_COMPONENT_NAMES, domain, level);
}


/**
 * Check if the specified level is active.
 */
// ECR-0123
bool
FlushAreaMngr::is_to_be_traced(const std::string& componentName,
                               const std::string& domain,
                               long level)
    throw(OutOfMemoryException)
{
    bool msg_to_trace = false;

    // The user has not called FlushAreaMngr::init() before
    // message is not traced in this case
    if (M_initDone) {
        msg_to_trace =
            M_pSingleton->m_pFilterMngr->is_activated(componentName,
                                                      domain,
                                                      level);
    }

    return msg_to_trace;
}


} // End namespace Trace
} // End namespace Cdmw
