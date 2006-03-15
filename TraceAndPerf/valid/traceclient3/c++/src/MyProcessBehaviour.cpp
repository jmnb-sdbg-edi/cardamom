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


#include "SystemMngt/platforminterface/PlatformInterface.hpp"

#include "traceclient3/MyProcessBehaviour.hpp"


/**
 * Constructor.
 */
MyProcessBehaviour::MyProcessBehaviour(CORBA::ORB_ptr orb,
                                       std::string& traceMessage,
                                       unsigned long traceCount)
{
    m_orb = CORBA::ORB::_duplicate(orb);
    m_traceMessage = traceMessage;
    m_traceCount = traceCount;
    m_trace_client_thread = NULL;
    m_init_done = false;

    // create a condition for the mutex
    m_condition = new Cdmw::OsSupport::Condition(m_mutex);
}


/**
 * Destructor.
 */
MyProcessBehaviour::~MyProcessBehaviour()
    throw()
{
    if (m_trace_client_thread != NULL) {
        this->stop();
    }

    delete m_condition;
}


/**
 * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
 */
CORBA::Object_ptr
MyProcessBehaviour::get_service()
    throw(CORBA::SystemException)
{
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(
        CdmwPlatformMngtBase::INF,
        m_processName.c_str(),
        "Service requested -> NIL returned");

    return CORBA::Object::_nil();
}


/**
 * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
 */
void
MyProcessBehaviour::initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CdmwPlatformMngt::ProcessDelegate::BadOrder,
          CORBA::SystemException)
{
    m_mutex.lock();

    // loop and wait for the initialization of main module to finish
    while (!m_init_done) {
        // wait for init end
        m_condition->wait();
    }

    m_mutex.unlock();

    // get the name of the application
    m_applicationName =
        Cdmw::PlatformMngt::PlatformInterface::Get_application_name();

    // get the name of the process
    m_processName =
        Cdmw::PlatformMngt::PlatformInterface::Get_process_name();

    Cdmw::PlatformMngt::PlatformInterface::Notify_message(
        CdmwPlatformMngtBase::INF,
        m_processName.c_str(),
        "Initialisation requested");

    // create the TraceClientThread
    m_trace_client_thread = new TraceClientThread(m_orb.in(),
                                                  m_applicationName,
                                                  m_processName,
                                                  m_traceMessage,
                                                  m_traceCount);
}


/**
 * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
 */
void
MyProcessBehaviour::run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
          CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
          CORBA::SystemException)
{
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(
        CdmwPlatformMngtBase::INF,
        m_processName.c_str(),
        "Run requested");

    // start the TraceClientThread
    if (m_trace_client_thread != NULL) {
        m_trace_client_thread->start();
    }
}


/**
 * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
 */
void
MyProcessBehaviour::stop()
    throw(CORBA::SystemException)
{
    TraceClientThread* client_thread = m_trace_client_thread;
    m_trace_client_thread = NULL;

    Cdmw::PlatformMngt::PlatformInterface::Notify_message(
        CdmwPlatformMngtBase::INF,
        m_processName.c_str(),
        "Stop requested");

    if (client_thread != NULL) {
        // get the status of the client thread
        Cdmw::OsSupport::ThreadStatus status = client_thread->get_status();

        // if it was requested to run or if it is already running
        // then stop it
        if ((status == Cdmw::OsSupport::THREAD_RUN_REQUEST) ||
            (status == Cdmw::OsSupport::THREAD_RUNNING))
        {
            client_thread->stop();
            client_thread->join();
        }

        delete client_thread;
    }

    m_orb->shutdown(false);
}


/**
 *
 */
void
MyProcessBehaviour::set_init_done()
{
    m_mutex.lock();
    m_init_done = true;
    m_mutex.unlock();
}
