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


#ifndef INCL_PLATFORMMNGT_UNMANAGED_PROCESS_PROXY_HPP
#define INCL_PLATFORMMNGT_UNMANAGED_PROCESS_PROXY_HPP

#include "platformapplicationlibrary/UnmanagedProcessStates.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

/**
 *Purpose:
 *<p> The process impl managing an unmanaged process.
 */
class UnmanagedProcess_impl : public Process_impl
{

private:
    /**
     * The associated state machine.
     */
    UnmanagedProcessStateMachine* m_stateMachine;


    /**
     * The states of an unmanaged process.
     */
    static UnmanagedProcessInitial* M_initial;
    static UnmanagedProcessInitialising* M_initialising;
    static UnmanagedProcessRunning* M_running;
    static UnmanagedProcessStopped* M_stopped;
    static UnmanagedProcessEnded* M_ended;
    static UnmanagedProcessFailedDeath* M_failedDeath;
    static UnmanagedProcessFailedInvalid* M_failedInvalid;

private:
    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    UnmanagedProcess_impl(
        const UnmanagedProcess_impl& rhs );

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    UnmanagedProcess_impl& operator=(
        const UnmanagedProcess_impl& rhs );
        
    /**
     *Purpose:
     *<p> Get the current state name of state machine
     *
     */    
    const char* get_state_machine_state_name();
    
    /**
     *Purpose:
     *<p> Set the current state name of state machine
     *
     */
    void set_state_machine_state_name(const char* state_name);
        

public:
    /**
     *Purpose:
     *<p> constructor.
     */
    UnmanagedProcess_impl(
        PortableServer::POA_ptr poa,
        CdmwPlatformMngt::ProcessDef* process_def,
        const char* process_key, const char* process_path,
        Application_impl* application,
        CdmwPlatformMngt::RWElementRepository_ptr element_repository)
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~UnmanagedProcess_impl();
    
    
    /**
     *Purpose:
     *<p> Ends the creation of the process servant
     *
     *@exception HostNotFound if the host where the process will be run is
     * not found.
     *@exception DuplicateEntity if there is a duplicate entity name among
     * the entities of the process
     *@exception DuplicateService if there is a duplicate service name among
     * the service definitions of the process
     *@exception DuplicateStep if there is a duplicate step label among
     * the steps of the process
     */
    void end_creation(CdmwPlatformMngt::ProcessDef* process_def)
            throw (CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   OutOfMemoryException,
    		   CosPropertyService::MultipleExceptions,
                   CORBA::SystemException);
                   
    /**
     *Purpose:
     *<p> Synchronise the processing to initial state
     *
     */
    void synchro_processing_to_initial();
    
    /**
     *Purpose:
     *<p> Synchronise the processing to initialised state
     *
     */
    void synchro_processing_to_initialised();
    
    /**
     *Purpose:
     *<p> Synchronise the processing to running state
     *
     */
    void synchro_processing_to_running();
    
    
    /**
     *Purpose:
     *<p> Get the internal status from state machine.
     */
     CdmwPlatformMngt::ProcessStatus get_internal_status();
    
    
      
protected:
    /**
     *Purpose:
     *<p> Gets the process's states.
     * This method must exclusivaly be used by the state machine.
     */
    CdmwPlatformMngt::ProcessStatus getStatus()
        throw( CORBA::SystemException );

public:
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_status:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessStatus get_status(
        CORBA::String_out status_info )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/status_info:1.0
     * operation
     */
    char* status_info();

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_auto_ending:1.0
     * operation
     */
    void set_auto_ending(CORBA::Boolean auto_ending)
        throw( CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_host:1.0
     * operation
     */
    void set_host(
        const char* host_name)
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/set_process_info:1.0
     * operation
     */
    void set_process_info(
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus stop(
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );
      

    /**
     *Purpose:
     *<p> Called when an error has occured.
     */
    void error_event(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info );

    /**
     *Purpose:
     *<p> Called when the process has ended its running step.
     */
    void ending_event(
        const CdmwPlatformMngtBase::EventHeader& header );

    /**
     *Purpose:
     *<p> Called when a technical error has occured.
     *
     *@param error_info the information about the error.
     */
    void invalidate_event(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );

    /**
     *Purpose:
     *<p> Creates all the state instances (states are singleton).
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    static void createStates()
        throw( OutOfMemoryException );

    /**
     *Purpose:
     *<p> Destroys all the states.
     */
    static void destroyStates();

    /**
     * Moves the state machine to specified state.
     */
    void setState(
        const char* stateName );

    /**
     *Purpose:
     *<p> Creates a process status change event. This method
     * is called within the state machine.
     */
    void notifyStatusChangeEvent(
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer );
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_UNMANAGED_PROCESS_PROXY_HPP

