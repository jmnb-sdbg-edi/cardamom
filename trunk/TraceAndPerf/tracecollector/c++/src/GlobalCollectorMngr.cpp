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


#include <iostream>
#include <sstream>

#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/System.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/osthreads/ThreadHandle.hpp>

#include <TraceAndPerf/tracelibrary/FilterMngr.hpp>

#include "tracecollector/GlobalCollectorMngr.hpp"
#include "tracecollector/GlobalTraceCollector_impl.hpp"
#include "tracecollector/TraceCircularBuffer.hpp"
#include "tracecollector/TraceFileMngr.hpp"


namespace Cdmw
{
namespace Trace
{


class TraceFileMngr;


/**
 * This class flushes the messages to disk.
 *
 * Features:
 *     - Thread safe, exception safe
 *
 * @see TraceCircularBuffer
 * @see TraceFile
 */
class CollectorFlusher: public OsSupport::Thread
{
    public:
        /**
         * Constructor.
         *
         * @param barrier the barrier is used by the internal thread
         *                to notify its creator it is ready to operate.
         * @param pCircularBuffer CircularBuffer in charge of bufferizing
         *                        the trace messages
         * @param trace_file_name generic name for trace file.
         * @param trace_file_bkp_nbr number of backup of trace files.
         * @param max_trace_file_nbr max number of trace files.
         * @param max_trace_message_nbr max number of message in a trace file.
         * @param traceFormat format of trace (Horizontal or Vertical).
         * @param time_mode gmt or local mode for display time.
         *
         * @exception AssertionFailedException.
         * @exception InternalErrorException.
         * @exception OutOfMemoryException.
         */
        CollectorFlusher(OsSupport::Barrier& barrier,
                         TraceCircularBuffer* pCircularBuffer,
                         const std::string& trace_file_base_name,
                         short trace_file_bkp_nbr,
                         short max_trace_file_nbr,
                         int max_trace_message_nbr,
                         Message::TraceFormat trace_format,
                         OsSupport::OS::TimeMode time_mode)
            throw(OutOfMemoryException,
                  AssertionFailedException,
                  InternalErrorException,
                  BadParameterException)
                : OsSupport::Thread(),
                  m_thread_must_terminate(false),
                  m_barrier(barrier),
                  m_pCircularBuffer(pCircularBuffer),
                  m_pTraceFileMngr(NULL)
        {
            try {
                // get base name for trace file.
                std::string trace_file_name = trace_file_base_name;

                // if backup of trace files is requested
                // and file name is defined
                if (trace_file_bkp_nbr > 1 && !trace_file_base_name.empty()) {
                    // build name of backup info.
                    std::string backUpInfoFileName = trace_file_base_name;
                    backUpInfoFileName += "_bkpinfo";

                    short current_bkp;

                    // open the file for read backup info
                    std::ifstream bkp_file_in_str(backUpInfoFileName.c_str(),
                                                  std::ios::in);

                    // if file can be opened.
                    if (bkp_file_in_str) {
                        // get current backup from info file
                        bkp_file_in_str >> current_bkp;

                        // if current backup not retrieve (empty file)
                        // set the 1st backup
                        if (!bkp_file_in_str.good()) {
                            current_bkp = 1;
                        }
                        // else increment current backup
                        else {
                            current_bkp ++;

                            if (current_bkp > trace_file_bkp_nbr) {
                                current_bkp = 1;
                            }
                        }

                        // close the file
                        bkp_file_in_str.close();
                    }
                    else {
                        current_bkp = 1;
                    }

                    // open the file for write or create file if does not exist.
                    std::ofstream bkp_file_out_str(backUpInfoFileName.c_str(),
                                                   std::ios::out);

                    // if file cannot be opened.
                    if (!bkp_file_out_str) {
                        CDMW_THROW2(BadParameterException,
                                    "Backup Info File cannot be opened",
                                    backUpInfoFileName.c_str());
                    }
                    else {
                        // save current backup nbr in file
                        bkp_file_out_str << current_bkp << std::endl;
                    }

                    // close the file
                    bkp_file_out_str.close();

                    // update the file name base with the backup nbr
                    std::ostringstream name_str;
                    name_str << trace_file_base_name << "_" << current_bkp;

                    trace_file_name = name_str.str();
                }

                // create trace file manager
                m_pTraceFileMngr = new TraceFileMngr(trace_file_name,
                                                     max_trace_file_nbr,
                                                     max_trace_message_nbr,
                                                     trace_format,
                                                     time_mode);
            }
            catch (const std::bad_alloc&) {
                CDMW_THROW (OutOfMemoryException);
            }
        };


        /**
         * Destructor.
         */
        virtual
        ~CollectorFlusher()
            throw()
        {
            // delete trace file manager
            if (m_pTraceFileMngr != NULL) {
                delete m_pTraceFileMngr;
                m_pTraceFileMngr = NULL;
            }
        };


        /**
         * Ask to the internal thread to stop its job, the thread will exit
         * only when all trace messages will have been flushed to disk.
         * When this function return the internal thread have stopped.
         *
         * @exception InternalErrorException.
         */
        void
        stop_thread()
            throw(InternalErrorException)
        {
            // require the thread to terminate.
            m_thread_must_terminate = true;

            // stop the buffer reading -> awake the thread, if it is sleeping.
            m_pCircularBuffer->stopReading();

            try {
                // wait for the termination of our internal thread.
                this->join();
            }
            catch (const OsSupport::DeadlockException&) {
                CDMW_THROW(InternalErrorException);
            }
        };


        /**
         * Print error message in trace file.
         *
         * @param errMesg contains the message to print
         */
        void
        print_error_message(const std::string& errMesg)
            throw()
        {
            m_pTraceFileMngr->print_error_message (errMesg);
        };


    protected:
        /**
         * Call by the underlying thread system in a new thread.
         */
        virtual void
        run()
            throw()
        {
            try {
                // notify the GlobalCollectorMngr that we are ready.
                m_barrier.wait();

                // message header data buffer.
                TraceCircularBuffer::TraceMessageRaw messageData;

                // flag to indicate id data read
                bool dataRead;
                bool loop = true;

                // infinite loop.
                // check if someone's asked the thread to terminate.
                while (loop) {
                    // read trace message.
                    // if no message available thread is set to wait
                    // by CircularBuffer
                    bool success =
                        m_pCircularBuffer->getNextMessage(messageData,
                                                          dataRead);

                    // if no error and data has been read, print message.
                    if (success == true && dataRead == true) {
                        m_pTraceFileMngr->print_message (&messageData);
                    }
                    // else if error or thread to terminate, break the loop.
                    else if (success == false ||
                             m_thread_must_terminate == true)
                    {
                        loop = false;
                    }
                }

                // print end of trace message in the current trace file.
                m_pTraceFileMngr->print_end_of_trace();
            }
            // catch any exception and assert it.
            catch (...) {
                CDMW_ASSERT(false);
            }
        };


    private:
        /**
         * Copy constructor.
         */
        CollectorFlusher(const CollectorFlusher& rhs)
            throw();


        /**
         * Assignement operator.
         */
        CollectorFlusher&
        operator=(const CollectorFlusher& rhs)
            throw();


        /**
         * At each iteration the thread tests this variable, to
         * see if it must stop it's processing.
         * If true the thread terminates.
         */
        bool m_thread_must_terminate;


        /**
         * This barrier is used for synchronization with the GlobalCollectorMngr.
         * By signalling this barrier we are able to notify it, when the
         * thread is operational.
         */
        OsSupport::Barrier& m_barrier;


        /**
         * Hold a circular buffer.
         */
        TraceCircularBuffer* m_pCircularBuffer;

        /**
         * Hold a TraceFileMgr.
         */
        TraceFileMngr* m_pTraceFileMngr;


}; // class CollectorFlusher


/**
 * default trace file name.
 */
const char* GlobalCollectorMngr::DEFAULT_TRACE_FILE="./cdmw_global_trace";


/**
 * This variable is set to true when the GlobalCollectorMngr::Init() function
 * has been called.
 */
bool GlobalCollectorMngr::M_initDone = false;


/**
 * Contains the GlobalCollectorMngr singleton initialised by the
 * GlobalCollectorMngr::Init() function.
 */
GlobalCollectorMngr* GlobalCollectorMngr::M_pSingleton = NULL;


/**
 * Constructor.
 *
 * Note:
 * m_barrier (counter set to 2)
 * The GlobalCollectorMngr constructor and the CollectorFlusher thread have
 * to reach their respective waiting point to continue their processing.
 */
GlobalCollectorMngr::GlobalCollectorMngr(
        PortableServer::POA_ptr tracePOA,
        const std::string& domain,
        long level,
        Message::TraceFormat traceFormat,
        Message::CircularMode queueStrategy,
        size_t queueSize,
        size_t stringSize,
        const std::string& trace_file_name,
        short trace_file_bkp_nbr,
        short max_trace_file_nbr,
        int max_trace_message_nbr,
        OsSupport::OS::TimeMode time_mode)
    throw(OutOfMemoryException,
          BadParameterException,
          InternalErrorException)
        : m_spCollectorFlusher(NULL),
          m_barrier(2),
          m_pFilterMngr(NULL),
          m_pCircularBuffer(NULL)
{
    try {
        // store the size in bytes instead of Kbytes.
        queueSize *= 1024;
        stringSize *= 1024;

        // create the object in charge of determining whether a specific
        // level/domain/component must be traced.
        m_pFilterMngr = new FilterMngr();

        // create a new filter.
        m_pFilterMngr->activate_level(CdmwTrace::ALL_COMPONENT_NAMES,
                                      domain,
                                      level);

        // create the circular buffer.
        m_pCircularBuffer = new TraceCircularBuffer(queueSize,
                                                    stringSize,
                                                    queueStrategy);

        // create the thread in charge of flushing Trace Message as
        // a background task.
#ifdef _MSC_VER
        // The auto_ptr implementation shipped with VC++ is not compliant
        // and doesn't provide reset(), but it's broken as it used an
        // assignement operator with const auto_ptr&, we use this deviation.

        // create the thread in charge of communicating with collectors.
        m_spCollectorFlusher =
            std::auto_ptr <CollectorFlusher>(
                new CollectorFlusher(m_barrier,
                                     m_pCircularBuffer,
                                     trace_file_name,
                                     trace_file_bkp_nbr,
                                     max_trace_file_nbr,
                                     max_trace_message_nbr,
                                     traceFormat,
                                     time_mode));
#else
        m_spCollectorFlusher.reset(
            new CollectorFlusher(m_barrier,
                                 m_pCircularBuffer,
                                 trace_file_name,
                                 trace_file_bkp_nbr,
                                 max_trace_file_nbr,
                                 max_trace_message_nbr,
                                 traceFormat,
                                 time_mode));
#endif

        // create the Servant to handle registration request
        // (ref count is incremented)
        GlobalTraceCollector_impl* pTraceCollectorServant =
            new GlobalTraceCollector_impl(this, m_pFilterMngr);

        // create an object var to take pointer ownership
        // (ref count will be decremented when var is destroyed when exiting
        // the method)
        PortableServer::ServantBase_var servant_var = pTraceCollectorServant;

        // create an object id.
        PortableServer::ObjectId_var oid =
            PortableServer::string_to_ObjectId("GlobalTraceCollector");

        try {
            // activate servant on POA (ref count is incremented)
            tracePOA->activate_object_with_id(oid.in(),
                                              pTraceCollectorServant);
            CORBA::Object_var object = tracePOA->id_to_reference(oid.in());
            m_traceCollector = CdmwTrace::Collector::_narrow(object.in());
        }
        catch (const PortableServer::POA::WrongPolicy&) {
            CDMW_THROW2(Cdmw::BadParameterException,
                        "Wrong POA Policy for activate_object",
                        "NULL");
        }
        catch (const PortableServer::POA::ObjectAlreadyActive&) {
            CDMW_THROW2(BadParameterException,
                        "Global Trace Collector Object already active",
                        "NULL");
        }
        catch (const PortableServer::POA::ServantAlreadyActive&) {
            CDMW_THROW2(BadParameterException,
                        "Global Trace Collector Servant already active",
                        "NULL");
        }
        catch (const CORBA::Exception& ex) {
            std::cerr << "Global Trace Collector servant activation error"
                      << std::endl << "CORBA Exception: "
                      << std::endl << ex << std::endl;
            CDMW_ASSERT(false);
        }

        try {
            // start the background thread in charge of fluhsing data.
            m_spCollectorFlusher->start();

            // wait until it declares itself as operational.
            m_barrier.wait();
        }
        catch (const OsSupport::ThreadSchedulingException&) {
            // forgotten to assign specific rights to the thread
            CDMW_THROW(InternalErrorException);
        }
    }
    catch (const std::bad_alloc&) {
        CDMW_THROW (OutOfMemoryException);
    }
}


/**
 * Constructor.
 *
 * Note:
 * m_barrier (counter set to 2)
 * The GlobalCollectorMngr constructor and the CollectorFlusher thread have
 * to reach their respective waiting point to continue their processing.
 */
GlobalCollectorMngr::GlobalCollectorMngr(
        PortableServer::POA_ptr tracePOA,
        const std::string& componentName,
        const std::string& domain,
        long level,
        Message::TraceFormat traceFormat,
        Message::CircularMode queueStrategy,
        size_t queueSize,
        size_t stringSize,
        const std::string& trace_file_name,
        short trace_file_bkp_nbr,
        short max_trace_file_nbr,
        int max_trace_message_nbr,
        OsSupport::OS::TimeMode time_mode)
    throw(OutOfMemoryException,
          BadParameterException,
          InternalErrorException)
        : m_spCollectorFlusher(NULL),
          m_barrier(2),
          m_pFilterMngr(NULL),
          m_pCircularBuffer(NULL)
{
    try {
        // store the size in bytes instead of Kbytes.
        queueSize *= 1024;
        stringSize *= 1024;

        // create the object in charge of determining whether a specific
        // level/domain/component must be traced.
        m_pFilterMngr = new FilterMngr();

        // create a new filter.
        m_pFilterMngr->activate_level(componentName, domain, level);

        // create the circular buffer.
        m_pCircularBuffer = new TraceCircularBuffer(queueSize,
                                                    stringSize,
                                                    queueStrategy);

        // create the thread in charge of flushing Trace Message as
        // a background task.
#ifdef _MSC_VER
        // The auto_ptr implementation shipped with VC++ is not compliant
        // and doesn't provide reset(), but it's broken as it used an
        // assignement operator with const auto_ptr&, we use this deviation.

        // create the thread in charge of communicating with collectors.
        m_spCollectorFlusher =
            std::auto_ptr <CollectorFlusher>(
                new CollectorFlusher(m_barrier,
                                     m_pCircularBuffer,
                                     trace_file_name,
                                     trace_file_bkp_nbr,
                                     max_trace_file_nbr,
                                     max_trace_message_nbr,
                                     traceFormat,
                                     time_mode));
#else
        m_spCollectorFlusher.reset(
            new CollectorFlusher(m_barrier,
                                 m_pCircularBuffer,
                                 trace_file_name,
                                 trace_file_bkp_nbr,
                                 max_trace_file_nbr,
                                 max_trace_message_nbr,
                                 traceFormat,
                                 time_mode));
#endif

        // create the Servant to handle registration request
        // (ref count is incremented)
        GlobalTraceCollector_impl* pTraceCollectorServant =
            new GlobalTraceCollector_impl(this, m_pFilterMngr);

        // create an object var to take pointer ownership
        // (ref count will be decremented when var is destroyed when exiting
        // the method)
        PortableServer::ServantBase_var servant_var = pTraceCollectorServant;

        // create an object id.
        PortableServer::ObjectId_var oid =
            PortableServer::string_to_ObjectId("GlobalTraceCollector");

        try {
            // activate servant on POA (ref count is incremented)
            tracePOA->activate_object_with_id(oid.in(),
                                              pTraceCollectorServant);
            CORBA::Object_var object = tracePOA->id_to_reference(oid.in());
            m_traceCollector = CdmwTrace::Collector::_narrow(object.in());
        }
        catch (const PortableServer::POA::WrongPolicy&) {
            CDMW_THROW2(Cdmw::BadParameterException,
                        "Wrong POA Policy for activate_object",
                        "NULL");
        }
        catch (const PortableServer::POA::ObjectAlreadyActive&) {
            CDMW_THROW2(BadParameterException,
                        "Global Trace Collector Object already active",
                        "NULL");
        }
        catch (const PortableServer::POA::ServantAlreadyActive&) {
            CDMW_THROW2(BadParameterException,
                        "Global Trace Collector Servant already active",
                        "NULL");
        }
        catch (const CORBA::Exception& ex) {
            std::cerr << "Global Trace Collector servant activation error"
                      << std::endl << "CORBA Exception: "
                      << std::endl << ex << std::endl;
            CDMW_ASSERT(false);
        }

        try {
            // start the background thread in charge of fluhsing data.
            m_spCollectorFlusher->start();

            // wait until it declares itself as operational.
            m_barrier.wait();
        }
        catch (const OsSupport::ThreadSchedulingException&) {
            // forgotten to assign specific rights to the thread
            CDMW_THROW(InternalErrorException);
        }
    }
    catch (const std::bad_alloc&) {
        CDMW_THROW (OutOfMemoryException);
    }
}


/**
 * Initialize the Trace collector.
 */
CdmwTrace::Collector_ptr
GlobalCollectorMngr::Init(PortableServer::POA_ptr tracePOA,
                          const std::string& domain,
                          long level,
                          Message::TraceFormat trace_format,
                          Message::CircularMode queueStrategy,
                          size_t queueSize,
                          size_t stringSize,
                          const std::string& trace_file_name,
                          short trace_file_bkp_nbr,
                          short max_trace_file_nbr,
                          int max_trace_message_nbr,
                          OsSupport::OS::TimeMode time_mode)
    throw(OutOfMemoryException,
          BadParameterException,
          InternalErrorException)
{
    // the user tried to initialise the collector twice.
    CDMW_ASSERT(M_initDone == false);

    try {
        M_pSingleton = new GlobalCollectorMngr(
                                tracePOA,
                                CdmwTrace::ALL_COMPONENT_NAMES,
                                domain,
                                level,
                                trace_format,
                                queueStrategy,
                                queueSize,
                                stringSize,
                                trace_file_name,
                                trace_file_bkp_nbr,
                                max_trace_file_nbr,
                                max_trace_message_nbr,
                                time_mode);
    }
    catch (const std::bad_alloc&) {
        CDMW_THROW (OutOfMemoryException);
    }

    M_initDone = true;

    // return the reference of the object managing the communications with
    // the collector, it will be registered in the repository by the caller.
    return CdmwTrace::Collector::_duplicate(
               M_pSingleton->m_traceCollector.in());
}


/**
 * Initialize the Trace collector.
 */
CdmwTrace::Collector_ptr
GlobalCollectorMngr::Init(PortableServer::POA_ptr tracePOA,
                          const std::string& componentName,
                          const std::string& domain,
                          long level,
                          Message::TraceFormat trace_format,
                          Message::CircularMode queueStrategy,
                          size_t queueSize,
                          size_t stringSize,
                          const std::string& trace_file_name,
                          short trace_file_bkp_nbr,
                          short max_trace_file_nbr,
                          int max_trace_message_nbr,
                          OsSupport::OS::TimeMode time_mode)
    throw(OutOfMemoryException,
          BadParameterException,
          InternalErrorException)
{
    // the user tried to initialise the collector twice.
    CDMW_ASSERT(M_initDone == false);

    try {
        M_pSingleton = new GlobalCollectorMngr(
                                tracePOA,
                                componentName,
                                domain,
                                level,
                                trace_format,
                                queueStrategy,
                                queueSize,
                                stringSize,
                                trace_file_name,
                                trace_file_bkp_nbr,
                                max_trace_file_nbr,
                                max_trace_message_nbr,
                                time_mode);
    }
    catch (const std::bad_alloc&) {
        CDMW_THROW (OutOfMemoryException);
    }

    M_initDone = true;

    // return the reference of the object managing the communications with the
    // collector, it will be registered in the repository by the caller.
    return CdmwTrace::Collector::_duplicate(
               M_pSingleton->m_traceCollector.in());
}


/**
 * Free memory and terminate the Trace service.
 */
void
GlobalCollectorMngr::Cleanup()
    throw(InternalErrorException)
{
    // the library is not initialised.
    CDMW_ASSERT(M_initDone == true);

    // stop the thread in charge of flushing data.
    M_pSingleton->m_spCollectorFlusher->stop_thread();

    M_initDone = false;
    delete M_pSingleton;

    M_pSingleton = NULL;
}


/**
 * Destructor.
 */
GlobalCollectorMngr::~GlobalCollectorMngr()
    throw()
{
    if (m_pFilterMngr != NULL) {
        delete m_pFilterMngr;
        m_pFilterMngr = NULL;
    }

    if (m_pCircularBuffer != NULL) {
        delete m_pCircularBuffer;
        m_pCircularBuffer = NULL;
    }

    // the Cleanup() function must be invoked before the destroying
    // the instance.
    CDMW_ASSERT(M_initDone == false);
}


/**
 * Create an instance of the GlobalCollectorMngr.
 */
GlobalCollectorMngr&
GlobalCollectorMngr::Instance()
    throw()
{
    // the Init() function must be invoked before creating the instance.
    CDMW_ASSERT(M_initDone == true);

    return *M_pSingleton;
}



/**
 * Write a message in the circular buffer.
 */
bool
GlobalCollectorMngr::writeMessage(
        const CdmwTrace::MessagesHeader& messagesHdr,
        const CdmwTrace::FormattedMessage& fmtMessage)
    throw(OutOfMemoryException)
{
    bool ret = true;

    CDMW_ASSERT(m_spCollectorFlusher.get() != NULL);
    CDMW_ASSERT(m_pCircularBuffer != NULL);

    // write out the message only if the message passes at least one filter.
    std::string domain = fmtMessage.header.the_level.the_domain.in();
    std::string componentName =
        fmtMessage.header.the_level.the_component_name.in();

    if (GlobalCollectorMngr::Is_to_be_traced(
                componentName,
                domain,
                fmtMessage.header.the_level.the_value))
    {
        // if not filtered, write message in circular buffer.
        // collector flusher is signaled to flush message on disk.
        TraceCircularBuffer::WriteStatus write_status;
        ret = m_pCircularBuffer->writeMessage(messagesHdr,
                                              fmtMessage,
                                              write_status);

        if (write_status == TraceCircularBuffer::STS_DROP_LATEST) {
            m_spCollectorFlusher->print_error_message(
                "Queue is full : latest trace message has been dropped");
        }
        else if (write_status == TraceCircularBuffer::STS_REMOVE_OLDEST) {
            m_spCollectorFlusher->print_error_message(
                "Queue is full : oldest trace message has been removed");
        }
    }

    return ret;
}


/**
 * Check if the specified level is activated.
 */
bool
GlobalCollectorMngr::Is_to_be_traced(const std::string& domain, long level)
{
    return Is_to_be_traced(CdmwTrace::ALL_COMPONENT_NAMES,
                           domain,
                           level);
}


/**
 * Check if the specified level is activated.
 */
bool
GlobalCollectorMngr::Is_to_be_traced(const std::string& componentName,
                                     const std::string& domain,
                                     long level)
{
    // the user has not called GlobalCollectorMngr::Init() first.
    CDMW_ASSERT(M_initDone == true);

    return M_pSingleton->m_pFilterMngr->is_activated(componentName,
                                                     domain,
                                                     level);
}


}; // namespace Trace
}; // namespace Cdmw
