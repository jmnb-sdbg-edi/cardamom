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


#ifndef INCL_MYPROCESSBEHAVIOUR_HPP 
#define INCL_MYPROCESSBEHAVIOUR_HPP 

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

#include "TraceClientThread.hpp"


#include <string>
#include <sstream>
#include <iostream>
#include <fstream>



using namespace Cdmw::PlatformMngt;
using namespace Cdmw::OsSupport;



/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class MyProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
{

public:

    /**
    * Purpose:
    * <p> The constructor.
    */ 
    MyProcessBehaviour(CORBA::ORB_ptr orb, 
                       std::string& traceMessage,
                       unsigned long traceCount)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
        m_traceMessage = traceMessage;
        m_traceCount = traceCount;
        m_trace_client_thread = NULL;
        m_init_done = false;
        
        // Creating a Condition associated with the member Mutex
        m_condition = new Cdmw::OsSupport::Condition(m_mutex);
    }


    /**
    * Purpose:
    * <p> The destructor.
    */ 
    virtual ~MyProcessBehaviour() throw ()
    {  
        // ==================================================
        // if client thread is defined call stop
        //     to stop it if still running
        // ==================================================
        if (m_trace_client_thread != NULL) 
        {
	        this->stop();
	    }
	    
	    delete m_condition;    
    }

    
    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/nb_steps:1.0
    * attribute
    */
    virtual CORBA::ULong nb_steps() throw(CORBA::SystemException)
    {
        return 1;
    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
	* operation
	*/
    virtual CORBA::Object_ptr get_service() throw(CORBA::SystemException)
    {
        // example of using the PlatformInterface for notifying a message
        PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
            m_processName.c_str(), "Service requested->NIL returned");
            
        return CORBA::Object::_nil();
    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
	* operation
	*/
    virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException)
    {
        // first lock the mutex
        m_mutex.lock();
        
        // loop and wait the initialisation from main module
        while (! m_init_done)
        {
            // Wait for init end
            m_condition->wait();
        }
        
        m_mutex.unlock();
        
        // get application and process names                    
        m_applicationName = PlatformInterface::Get_application_name();
        m_processName = PlatformInterface::Get_process_name();
                    
        // example of using the PlatformInterface for notifying a message
        PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                     m_processName.c_str(), "Initialisation requested");
                                         
        
        // ==================================================                                
        // create the hello client thread
        // ==================================================
        m_trace_client_thread =
            new TraceClientThread (m_orb.in(),
                                   m_applicationName,
                                   m_processName,
                                   m_traceMessage,
                                   m_traceCount);
               
    }


    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
	* operation
	*/
    virtual void next_step()
        throw(CdmwPlatformMngt::ProcessDelegate::InvalidStep, CORBA::SystemException)
    {
               
    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
	* operation
	*/
    virtual void run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
              CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
              CORBA::SystemException)
    {          
        // example of using the PlatformInterface for notifying a message
        PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                     m_processName.c_str(), "Run requested");
                                         
        // ======================================                                
        // start the client thread
        // ======================================
        if (m_trace_client_thread != NULL) 
        {
	        m_trace_client_thread->start();
	    }
    }

    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
	* operation
	*/
    virtual void stop() throw(CORBA::SystemException)
    {
        TraceClientThread* client_thread = m_trace_client_thread;
        m_trace_client_thread = NULL;
            
        // example of using the PlatformInterface for notifying a message
        PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                     m_processName.c_str(), "Stop requested");
                                         
        
        if (client_thread != NULL) 
        {
	        // First signal termination to hello client thread if it was 
	        // at least requested to run
	    
            Cdmw::OsSupport::ThreadStatus status = client_thread->get_status();
        
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


    void set_init_done() 
    {
        m_mutex.lock();
        m_init_done = true;
        m_mutex.unlock();
    }
    
    Cdmw::OsSupport::Condition& get_condition()
    {
        return *m_condition;
    }
    
private:

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;

    
    /**
    * The application name.
    */
    std::string m_applicationName;
      
    /**
    * The process name.
    */
    std::string m_processName;
    
    /**
    * The trace message.
    */
    std::string m_traceMessage;
    
    /**
    * The number of trace message.
    */
    unsigned long m_traceCount;
    
    /**
    * The client thread reference
    */
    TraceClientThread* m_trace_client_thread;
    
    /**
    * init done flag
    */
    bool  m_init_done;
    
    /**
    * Condition and Mutex
    */
    Cdmw::OsSupport::Condition*   m_condition;
    Cdmw::OsSupport::Mutex        m_mutex;
     

};


#endif // INCL_MYPROCESSBEHAVIOUR_HPP

