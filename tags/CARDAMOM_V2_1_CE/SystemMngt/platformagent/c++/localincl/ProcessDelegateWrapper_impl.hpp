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


#ifndef INCL_PLATFORMMNGT_PROCESS_DELEGATE_WRAPPER_IMPL_HPP 
#define INCL_PLATFORMMNGT_PROCESS_DELEGATE_WRAPPER_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/ossupport/ProcessCallback.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtProcessDelegateWrapper.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

class ApplicationAgent_impl;

/**
* Purpose:
* <p>
* The wrapper of process delegate.
*/
class ProcessDelegateWrapper_impl : virtual public POA_CdmwPlatformMngt::ProcessDelegateWrapper,
    virtual public PortableServer::RefCountServantBase
{

public:


    /**
    * Purpose:
    * <p> Constructor.
    */ 
    ProcessDelegateWrapper_impl( CdmwPlatformMngt::ProcessDelegate_ptr process,
                                 const std::string processName,
                                 //OsSupport::OS::ProcessId processId );
                                 ApplicationAgent_impl* pAgent );

    /**
    * Purpose:
    * <p> Destructor.
    */ 
    virtual ~ProcessDelegateWrapper_impl() throw ();
    
    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/nb_steps:1.0
    * attribute
    */
    virtual CORBA::ULong nb_steps()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
	* operation
	*/
    virtual CORBA::Object_ptr get_service()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_pull_monitorable:1.0
	* operation
	*/
    virtual FT::PullMonitorable_ptr get_pull_monitorable()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_push_monitorable:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::PushMonitorable_ptr get_push_monitorable()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
	* operation
	*/
    virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
	* operation
	*/
    virtual void next_step()
        throw(CdmwPlatformMngt::ProcessDelegate::BadOrder,
              CdmwPlatformMngt::ProcessDelegate::InvalidStep,
              CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
	* operation
	*/
    virtual void run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
              CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
              CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
	* operation
	*/
    virtual void stop()
        throw(CORBA::SystemException);
    
    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/call_command:1.0
	* operation
	*/
	virtual CdmwPlatformMngt::CommandResponse call_command (
                 const char * command_name,
                 const CdmwPlatformMngt::CmdParameterValues & command_args,
                 CORBA::String_out result_info)
        throw(CdmwPlatformMngt::CommandCallBackNotFound,
              CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/call_entity_command:1.0
	* operation
	*/
	virtual CdmwPlatformMngt::CommandResponse call_entity_command (
                const char * entity_name,
                const char * command_name,
                const CdmwPlatformMngt::CmdParameterValues & command_args,
                CORBA::String_out result_info)
        throw(CdmwPlatformMngt::CommandCallBackNotFound,
              CORBA::SystemException);  
              
              
    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegateWrapper/initialise:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::ProcessCommandResult initialise_process(const CdmwPlatformMngtBase::StartupKind& startup_kind,
                                    CdmwPlatformMngt::Timeout init_timeout)
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegateWrapper/next_step:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::ProcessCommandResult next_step_process(CORBA::ULong step_number,
                                   CdmwPlatformMngt::Timeout next_step_timeout)
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegateWrapper/run:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::ProcessCommandResult run_process(CdmwPlatformMngt::Timeout run_timeout)
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegateWrapper/stop:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::ProcessCommandResult stop_process(CdmwPlatformMngt::Timeout stop_timeout)
        throw(CORBA::SystemException);
        
    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegateWrapper/get_command_result_info:1.0
	* operation
	*/
    virtual char* get_command_result_info()
        throw(CORBA::SystemException);
        


private:

    /**
    * Purpose:
    * <p> Constructor.
    */ 
    ProcessDelegateWrapper_impl();

    // Reference to the corresponding  ProcessDelegate
    CdmwPlatformMngt::ProcessDelegate_var m_process;

    // status to verify there is no duplicated calls 
    CdmwPlatformMngt::ProcessStatus m_status;

    // The mutex protecting concurrent access changing process status
    OsSupport::Mutex* m_pMutex;

    // The last exception name
    std::string m_exceptionName;

    // The current step of the process
    CORBA::ULong m_currentStep;

    //OsSupport::OS::ProcessId m_processID;
    // The process name
    std::string m_processName;
    // The application agent
    ApplicationAgent_impl* m_pAgent;

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PROCESS_DELEGATE_WRAPPER_IMPL_HPP

