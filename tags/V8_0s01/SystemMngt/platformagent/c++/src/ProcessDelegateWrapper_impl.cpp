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


//#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "platformagent/ProcessDelegateWrapper_impl.hpp"
#include "platformdaemonlibrary/DaemonLogMessageId.hpp"
#include "platformagent/ApplicationAgent_impl.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessDelegate.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/common/Assert.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/nb_steps:1.0
//
CORBA::ULong ProcessDelegateWrapper_impl::nb_steps()
    throw(CORBA::SystemException)
{
    // Should not happen : method not called for unmanaged process or before process creation
    CDMW_ASSERT(!CORBA::is_nil(m_process.in()));
    // ask the Process
    CORBA::ULong nbSteps = m_process->nb_steps();
    
    return nbSteps;
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
//
CORBA::Object_ptr ProcessDelegateWrapper_impl::get_service()
    throw(CORBA::SystemException)
{
    // Should not happen : method not called for unmanaged process or before process creation
    CDMW_ASSERT(!CORBA::is_nil(m_process.in()));
    // ask the Process
    return m_process->get_service();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_pull_monitorable:1.0
//
FT::PullMonitorable_ptr ProcessDelegateWrapper_impl::get_pull_monitorable()
    throw(CORBA::SystemException)
{
    // Should not happen : method not called for unmanaged process or before process creation
    CDMW_ASSERT(!CORBA::is_nil(m_process.in()));
    // ask the Process
    return m_process->get_pull_monitorable();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_push_monitorable:1.0
//
CdmwPlatformMngt::PushMonitorable_ptr ProcessDelegateWrapper_impl::get_push_monitorable()
    throw(CORBA::SystemException)
{
    // Should not happen : method not called for unmanaged process or before process creation
    CDMW_ASSERT(!CORBA::is_nil(m_process.in()));
    // ask the Process
    return m_process->get_push_monitorable();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
//
void ProcessDelegateWrapper_impl::initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException)
{
    // in Wrapper it is initialise_process that is called
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
//
void ProcessDelegateWrapper_impl::next_step()
    throw(CdmwPlatformMngt::ProcessDelegate::BadOrder,
          CdmwPlatformMngt::ProcessDelegate::InvalidStep,
          CORBA::SystemException)
{
    // in Wrapper it is next_step_process that is called
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
//
void ProcessDelegateWrapper_impl::run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
          CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
          CORBA::SystemException)
{
    // in Wrapper it is run_process that is called
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
//
void ProcessDelegateWrapper_impl::stop()
    throw(CORBA::SystemException)
{
    // in Wrapper it is stop_process that is called
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/call_command:1.0
//
CdmwPlatformMngt::CommandResponse ProcessDelegateWrapper_impl::call_command (
                 const char * command_name,
                 const CdmwPlatformMngt::CmdParameterValues & command_args,
                 CORBA::String_out result_info)
     throw(CdmwPlatformMngt::CommandCallBackNotFound,
           CORBA::SystemException)
{
    // Should not happen : method not called for unmanaged process or before process creation
    CDMW_ASSERT(!CORBA::is_nil(m_process.in()));
    // ask the Process
    return m_process->call_command (command_name, command_args, result_info);
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/call_command:1.0
//
CdmwPlatformMngt::CommandResponse ProcessDelegateWrapper_impl::call_entity_command (
                const char * entity_name,
                const char * command_name,
                const CdmwPlatformMngt::CmdParameterValues & command_args,
                CORBA::String_out result_info)
        throw(CdmwPlatformMngt::CommandCallBackNotFound,
              CORBA::SystemException)
{
    // Should not happen : method not called for unmanaged process or before process creation
    CDMW_ASSERT(!CORBA::is_nil(m_process.in()));
    // ask the process
    return m_process->call_entity_command (entity_name, command_name,
                                           command_args, result_info);
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegateWrapper/initialise:1.0
//
CdmwPlatformMngt::ProcessCommandResult ProcessDelegateWrapper_impl::initialise_process(
                 const CdmwPlatformMngtBase::StartupKind& startup_kind,
                 CdmwPlatformMngt::Timeout init_timeout)
    throw(CORBA::SystemException)
{
    // Should not happen : method not called for unmanaged process or before process creation
    CDMW_ASSERT(!CORBA::is_nil(m_process.in()));

    CDMW_MUTEX_GUARD(*m_pMutex);
    CdmwPlatformMngt::ProcessCommandResult cmd_res = CdmwPlatformMngt::PROCESS_INVALID_INFO;
    // At the wrapper level we only check that there is no double calls
    if (m_status != CdmwPlatformMngt::PROCESS_INITIALISED) 
    {
        // Call initialise of Process with timeout
        try
        {
            BoundSyncCallReturn result = EXCEPTION;
            BoundSyncCallFactory* factory = BoundSyncCallFactory::getFactory();

            // Initialise it within the timeout
            std::auto_ptr< ProcessInitCall > initCall(
                factory->createProcessInitCall(
                    m_process.in(),
                    startup_kind,
                    init_timeout ) );

            result = initCall->execute();
            switch (result)
            {
                case EXCEPTION:
                  cmd_res = CdmwPlatformMngt::PROCESS_CMD_EXCEPTION;
                  m_exceptionName = initCall->getExceptionName();
                  LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_EXCEPTION,
                                      m_processName, m_exceptionName);
                  break;

                case TIMEOUT:
                  cmd_res = CdmwPlatformMngt::PROCESS_CMD_TIMEOUT;
                  LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_TIMEOUT,
                                      m_processName);
                  break;

                // In this wrapper INITIALISED does NOT mean that initialisation steps performed.
                // It only means that the first initialise has succeed (used to avoid double call)
                case SUCCEED:
                  cmd_res = CdmwPlatformMngt::PROCESS_CMD_SUCCEED;
                  m_status = CdmwPlatformMngt::PROCESS_INITIALISED;
                  m_currentStep = 0;
                  break;

                  // To be sure that a ProcessStatus type change is taken into account correctly
                default:
                  CDMW_ASSERT(false);
            }
        }
        catch( const OutOfMemoryException& e )
        {
            // Log the error
            LogMngr::logMessage( ERR, "Run out of memory" );
            // TODO: do an assert violation till a policy is decided
            CDMW_ASSERT( false );
        }
    }
    else
    {
        cmd_res = CdmwPlatformMngt::PROCESS_INIT_ALREADY_DONE;
    }
    
            
    return cmd_res;
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegateWrapper/next_step:1.0
//
CdmwPlatformMngt::ProcessCommandResult ProcessDelegateWrapper_impl::next_step_process(
                  CORBA::ULong step_number,
                  CdmwPlatformMngt::Timeout next_step_timeout)
    throw(CORBA::SystemException)
{
    // Should not happen : method not called for unmanaged process or before process creation
    CDMW_ASSERT(!CORBA::is_nil(m_process.in()));

    CDMW_MUTEX_GUARD(*m_pMutex);
    CdmwPlatformMngt::ProcessCommandResult cmd_res = CdmwPlatformMngt::PROCESS_INVALID_INFO;
    // At the wrapper level we only check that there is no double calls
    
    if (m_currentStep != step_number) 
    {
        // Call next_step of Process with timeout
        try
        {
            BoundSyncCallReturn result = EXCEPTION;
            BoundSyncCallFactory* factory = BoundSyncCallFactory::getFactory();

            // Perform one step
            std::auto_ptr< ProcessStepCall > stepCall(
                factory->createProcessStepCall(
                    m_process.in(),
                    next_step_timeout ) );
            result = stepCall->execute();
            switch (result)
            {
                case EXCEPTION:
                  cmd_res = CdmwPlatformMngt::PROCESS_CMD_EXCEPTION;
                  m_exceptionName = stepCall->getExceptionName();
                  
                  LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_EXCEPTION,
                                      m_processName, m_exceptionName);
                  break;

                case TIMEOUT:
                  cmd_res = CdmwPlatformMngt::PROCESS_CMD_TIMEOUT;
                  LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_TIMEOUT,
                                      m_processName);
                  break;

                case SUCCEED:
                 cmd_res = CdmwPlatformMngt::PROCESS_CMD_SUCCEED;
                  m_currentStep++;
                  break;

                  // To be sure that a ProcessStatus type change is taken into account correctly
                default:
                  CDMW_ASSERT(false);
            }
        }
        catch( const OutOfMemoryException& e )
        {
            // Log the error
            LogMngr::logMessage( ERR, "Run out of memory" );
            // TODO: do an assert violation till a policy is decided
            CDMW_ASSERT( false );
        }
    }
    else
    {
        cmd_res = CdmwPlatformMngt::PROCESS_STEP_ALREADY_DONE;
    }
    return cmd_res;
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegateWrapper/run:1.0
//
CdmwPlatformMngt::ProcessCommandResult ProcessDelegateWrapper_impl::run_process(CdmwPlatformMngt::Timeout run_timeout)
    throw(CORBA::SystemException)
{
    // Should not happen : method not called for unmanaged process or before process creation
    CDMW_ASSERT(!CORBA::is_nil(m_process.in()));

    CDMW_MUTEX_GUARD(*m_pMutex);
    CdmwPlatformMngt::ProcessCommandResult cmd_res = CdmwPlatformMngt::PROCESS_INVALID_INFO ;
    // At the wrapper level we only check that there is no double calls
    if (m_status != CdmwPlatformMngt::PROCESS_RUNNING) 
    {
        // Call run of Process with timeout
        try
        {
            BoundSyncCallReturn result = EXCEPTION;
            BoundSyncCallFactory* factory = BoundSyncCallFactory::getFactory();

            // Run it within the timeout
            std::auto_ptr< ProcessRunCall > runCall(
                factory->createProcessRunCall(
                    m_process.in(),
                    run_timeout ) );

            result = runCall->execute();
            CdmwPlatformMngt::ProcessDelegate::NotReadyToRun notReadyToRunException;
            switch (result)
            {
                case EXCEPTION:
                  m_exceptionName = runCall->getExceptionName();
                  
//                  if ( strcmp(m_exceptionName, notReadyToRunException._name() ) == 0 )
                  if ( m_exceptionName == notReadyToRunException._name() )
                  {
                      cmd_res = CdmwPlatformMngt::PROCESS_NOT_READY_TO_RUN;
                      LogMngr::logMessage(ERR,
                                          MSG_ID_AGENT_MANAGED_PROCESS_NOT_READY_TO_RUN,
                                          m_processName);
                  }
                  else
                  {
                      cmd_res = CdmwPlatformMngt::PROCESS_CMD_EXCEPTION;
                      LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_EXCEPTION,
                                          m_processName, m_exceptionName);
                  }
                  break;

                case TIMEOUT:
                  cmd_res = CdmwPlatformMngt::PROCESS_CMD_TIMEOUT;                  
                  LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_TIMEOUT,
                                      m_processName);
                  break;

                // In this wrapper RUNNING state does NOT mean that the process is still running.
                // It only means that the run call has succeed.
                case SUCCEED:
                  cmd_res = CdmwPlatformMngt::PROCESS_CMD_SUCCEED;
                  m_status = CdmwPlatformMngt::PROCESS_RUNNING;
                  break;

                  // To be sure that a ProcessStatus type change is taken into account correctly
                default:
                  CDMW_ASSERT(false);
            }
        }
        catch( const OutOfMemoryException& e )
        {
            // Log the error
            LogMngr::logMessage( ERR, "Run out of memory" );
            // TODO: do an assert violation till a policy is decided
            CDMW_ASSERT( false );
        }
    }
    else
    {
        cmd_res = CdmwPlatformMngt::PROCESS_RUN_ALREADY_DONE;
    }

    return cmd_res;
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegateWrapper/stop:1.0
//
CdmwPlatformMngt::ProcessCommandResult ProcessDelegateWrapper_impl::stop_process(CdmwPlatformMngt::Timeout stop_timeout)
    throw(CORBA::SystemException)
{
    // Should not happen : method not called for unmanaged process or before process creation
    CDMW_ASSERT(!CORBA::is_nil(m_process.in()));

    CDMW_MUTEX_GUARD(*m_pMutex);
    CdmwPlatformMngt::ProcessCommandResult cmd_res = CdmwPlatformMngt::PROCESS_INVALID_INFO;
    // Call stop of Process with timeout
    // At the wrapper level we only check that there is no double calls
    if (m_status != CdmwPlatformMngt::PROCESS_STOPPING) 
    {
        // Call run of Process with timeout
        try
        {
            BoundSyncCallReturn result = EXCEPTION;
            BoundSyncCallFactory* factory = BoundSyncCallFactory::getFactory();

            // Run it within the timeout
            std::auto_ptr< ProcessStopCall > stopCall(
                factory->createProcessStopCall(
                    m_process.in(),
                    stop_timeout ) );

            result = stopCall->execute();
            switch (result)
            {
                case EXCEPTION:
                  cmd_res = CdmwPlatformMngt::PROCESS_CMD_EXCEPTION;
                  m_exceptionName = stopCall->getExceptionName();
                  LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_EXCEPTION,
                                      m_processName, m_exceptionName);
                  break;

                case TIMEOUT:
                  cmd_res = CdmwPlatformMngt::PROCESS_CMD_TIMEOUT;                  
                  LogMngr::logMessage(ERR, MSG_ID_AGENT_MANAGED_PROCESS_TIMEOUT,
                                      m_processName);
                  break;

                // In this wrapper STOPPING state does NOT mean that the process is still stopping.
                // It only means that the stop call has succeed.
                case SUCCEED:
                  cmd_res = CdmwPlatformMngt::PROCESS_CMD_SUCCEED;
                  m_status = CdmwPlatformMngt::PROCESS_STOPPING;
                  break;

                  // To be sure that a ProcessStatus type change is taken into account correctly
                default:
                  CDMW_ASSERT(false);
            }
        }
        catch( const OutOfMemoryException& e )
        {
            // Log the error
            LogMngr::logMessage( ERR, "Run out of memory" );
            // TODO: do an assert violation till a policy is decided
            CDMW_ASSERT( false );
        }
    }    
    else
    {
        cmd_res = CdmwPlatformMngt::PROCESS_STOP_ALREADY_DONE;
    }

    return cmd_res;
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegateWrapper/get_command_result_info:1.0
//
char* ProcessDelegateWrapper_impl::get_command_result_info()
    throw(CORBA::SystemException)
{
    CORBA::String_var cmd_res_info = CORBA::string_dup(m_exceptionName.c_str());
    return cmd_res_info._retn();
}

ProcessDelegateWrapper_impl::ProcessDelegateWrapper_impl
  ( CdmwPlatformMngt::ProcessDelegate_ptr process,
    const std::string processName,
    ApplicationAgent_impl* pAgent ) :
    m_process(CdmwPlatformMngt::ProcessDelegate::_duplicate(process)),
    m_status(CdmwPlatformMngt::PROCESS_INITIAL),
    m_exceptionName(""),
    m_currentStep(0),
    m_processName(processName),
    m_pAgent(pAgent)
{
    m_pMutex = new OsSupport::Mutex();
}

ProcessDelegateWrapper_impl::ProcessDelegateWrapper_impl() :
    m_status(CdmwPlatformMngt::PROCESS_INITIAL),
    m_pMutex(NULL),
    m_exceptionName(""),
    m_currentStep(0),
    m_processName(""),
    m_pAgent(NULL)
{

}

ProcessDelegateWrapper_impl::~ProcessDelegateWrapper_impl()
    throw()
{
    if (m_pMutex != NULL) delete (m_pMutex);
}


} // End namespace PlatformMngt
} // End namespace Cdmw


