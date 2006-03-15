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


#ifndef INCL_PLATFORMMNGT_SYSTEM_STATES_HPP
#define INCL_PLATFORMMNGT_SYSTEM_STATES_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/platformlibrary/StateMachineTemplate.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class System_impl;
class SystemState;

typedef StateMachine <System_impl, SystemState> SystemStateMachine;



/**
 *Purpose:
 *<p> The abstract state of the state machine.
 *    Provides the default implementation of a system state.
 */ 
class SystemState : public virtual SystemStateMachine::AbstractState
{
	
public:

    virtual ~SystemState() {};

    virtual
    void entryAction (SystemStateMachine* stateMachine)
    {};

    virtual
    void exitAction (SystemStateMachine* stateMachine)
    {};

    virtual
    void doActivity (SystemStateMachine* stateMachine)
    {};

    virtual
    void completionTransition (SystemStateMachine* stateMachine)
    {};

    virtual
    CdmwPlatformMngt::SystemStatus status() = 0;


    void setInfo (SystemStateMachine* stateMachine,
                  const char* messageInfo);
  

    virtual
    void define (SystemStateMachine* stateMachine,
                 const CdmwPlatformMngt::SystemDef& system_def)
            throw (CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateHost,
                   CdmwPlatformMngt::DuplicateHostEntity,
                   CdmwPlatformMngt::HostPropertyExceptions,
                   CdmwPlatformMngt::AlreadyDone,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };
    
    

    virtual
    CdmwPlatformMngt::Application_ptr add_application (
                              SystemStateMachine* stateMachine,
                              const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                              const CdmwPlatformMngt::ApplicationDef& application_def)
            throw (CdmwPlatformMngt::ApplicationAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::ProcessPropertyExceptions,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessCommand, 
                   CdmwPlatformMngt::DuplicateProcessCommandArg,
                   CdmwPlatformMngt::DuplicateEntityCommand, 
                   CdmwPlatformMngt::DuplicateEntityCommandArg,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void remove_application (SystemStateMachine* stateMachine,        
                             const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void set_init_graph (SystemStateMachine* stateMachine,       
                         const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };
    
    
    virtual
    void set_stop_graph (SystemStateMachine* stateMachine,       
                         const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };
    
    
    virtual
    void set_run_set(SystemStateMachine* stateMachine,
                     const CdmwPlatformMngt::RunSetDef& run_set_def)
        throw (CdmwPlatformMngt::DuplicateRunSetElement,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    }
 
    virtual
    void remove_run_set(SystemStateMachine* stateMachine,
                        const char* run_set_name)
        throw (CdmwPlatformMngt::RunSetNotFound,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    }

    virtual
    void remove_all_run_sets(SystemStateMachine* stateMachine)
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    }


    virtual
    CdmwPlatformMngt::Host_ptr 
      add_host(SystemStateMachine* stateMachine,
               const CdmwPlatformMngt::HostDef& host_def)
        throw (CdmwPlatformMngt::HostAlreadyExists,
               CdmwPlatformMngt::DuplicateEntity,
               CosPropertyService::MultipleExceptions,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    }


    virtual
    void remove_host(SystemStateMachine* stateMachine,
                     const char* host_name)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    }                  

    virtual
    void suspension_event (SystemStateMachine* stateMachine)
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();        
    };

    virtual
    void completion_event (SystemStateMachine* stateMachine)
        throw (CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();        
    };


    virtual
    void start (SystemStateMachine* stateMachine,       
                const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };


    virtual
    void resume (SystemStateMachine* stateMachine)       
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void stop (SystemStateMachine* stateMachine,       
               CORBA::Boolean emergency)
             throw (CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException)
    {
    };



protected:

    SystemState() {};


private:

    SystemState (const SystemState& rhs) 
    {};
        

    SystemState& operator= (const SystemState& rhs)
    {
        return *this;
    };
    
};

/**
 *Purpose:
 *<p> The initial state.
 */
class SystemInitial : public virtual SystemState
{

public:

    SystemInitial();

    CdmwPlatformMngt::SystemStatus status();

    void define(SystemStateMachine* stateMachine,
                const CdmwPlatformMngt::SystemDef& system_def)
            throw (CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateHost,
                   CdmwPlatformMngt::DuplicateHostEntity,
                   CdmwPlatformMngt::HostPropertyExceptions,
                   CdmwPlatformMngt::AlreadyDone,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
    
    
    
    CdmwPlatformMngt::Application_ptr add_application (
                              SystemStateMachine* stateMachine,
                              const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                              const CdmwPlatformMngt::ApplicationDef& application_def)
            throw (CdmwPlatformMngt::ApplicationAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::ProcessPropertyExceptions,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessCommand, 
                   CdmwPlatformMngt::DuplicateProcessCommandArg,
                   CdmwPlatformMngt::DuplicateEntityCommand, 
                   CdmwPlatformMngt::DuplicateEntityCommandArg,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);



    void remove_application (SystemStateMachine* stateMachine,        
                             const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);



    void set_init_graph (SystemStateMachine* stateMachine,       
                         const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);


    void set_stop_graph (SystemStateMachine* stateMachine,       
                         const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
 

    void set_run_set(SystemStateMachine* stateMachine,
                     const CdmwPlatformMngt::RunSetDef& run_set_def)
        throw (CdmwPlatformMngt::DuplicateRunSetElement,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);

 

    void remove_run_set(SystemStateMachine* stateMachine,
                        const char* run_set_name)
        throw (CdmwPlatformMngt::RunSetNotFound,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);



    void remove_all_run_sets(SystemStateMachine* stateMachine)
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);




    CdmwPlatformMngt::Host_ptr 
      add_host(SystemStateMachine* stateMachine,
               const CdmwPlatformMngt::HostDef& host_def)
        throw (CdmwPlatformMngt::HostAlreadyExists,
               CdmwPlatformMngt::DuplicateEntity,
               CosPropertyService::MultipleExceptions,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);




    void remove_host(SystemStateMachine* stateMachine,
                     const char* host_name)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);
                 


    void start (SystemStateMachine* stateMachine,      
                const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);
 
 
                   
private:

    SystemInitial (const SystemInitial& rhs); 


    SystemInitial& operator= (const SystemInitial& rhs);       


};

/**
 *Purpose:
 *<p> The starting state.
 */
class SystemStarting : public virtual SystemState
{

public:

    SystemStarting();

    void entryAction (SystemStateMachine* stateMachine);

    void exitAction (SystemStateMachine* stateMachine);

    void doActivity (SystemStateMachine* stateMachine);

    CdmwPlatformMngt::SystemStatus status();

    void stop (SystemStateMachine* stateMachine,      
               CORBA::Boolean emergency)
         throw (CdmwPlatformMngt::IncompatibleStatus,
                CORBA::SystemException);

    void completion_event (SystemStateMachine* stateMachine)
         throw (CdmwPlatformMngt::IncompatibleStatus,
                CORBA::SystemException);

    void suspension_event (SystemStateMachine* stateMachine)
         throw (CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException);
        
        
private:

    SystemStarting (const SystemStarting& rhs);
        

    SystemStarting& operator= (const SystemStarting& rhs);       
};

/**
 *Purpose:
 *<p> The initialisation suspended state.
 */
class SystemStartupSuspended : public virtual SystemState
{

public:

    SystemStartupSuspended();

    void entryAction (SystemStateMachine* stateMachine);

    CdmwPlatformMngt::SystemStatus status();

    void set_init_graph (SystemStateMachine* stateMachine,       
                         const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
                   
    void set_stop_graph (SystemStateMachine* stateMachine,       
                         const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
    
    void resume (SystemStateMachine* stateMachine)      
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    void stop (SystemStateMachine* stateMachine,      
               CORBA::Boolean emergency)
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);


private:

    SystemStartupSuspended (const SystemStartupSuspended& rhs);
        

    SystemStartupSuspended& operator= (const SystemStartupSuspended& rhs);        

};





/**
 *Purpose:
 *<p> The started state.
 */
class SystemStarted : public virtual SystemState
{


public:

    SystemStarted();
    
    void entryAction (SystemStateMachine* stateMachine);
    
    void doActivity (SystemStateMachine* stateMachine);

    CdmwPlatformMngt::SystemStatus status();
 
 
    
    CdmwPlatformMngt::Application_ptr add_application (
                              SystemStateMachine* stateMachine,
                              const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                              const CdmwPlatformMngt::ApplicationDef& application_def)
            throw (CdmwPlatformMngt::ApplicationAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::ProcessPropertyExceptions,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessCommand, 
                   CdmwPlatformMngt::DuplicateProcessCommandArg,
                   CdmwPlatformMngt::DuplicateEntityCommand, 
                   CdmwPlatformMngt::DuplicateEntityCommandArg,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    void remove_application (SystemStateMachine* stateMachine,        
                             const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
                   
                   

    void set_stop_graph (SystemStateMachine* stateMachine,       
                         const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
 
                   
    CdmwPlatformMngt::Host_ptr 
      add_host(SystemStateMachine* stateMachine,
               const CdmwPlatformMngt::HostDef& host_def)
        throw (CdmwPlatformMngt::HostAlreadyExists,
               CdmwPlatformMngt::DuplicateEntity,
               CosPropertyService::MultipleExceptions,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);


    void remove_host(SystemStateMachine* stateMachine,
                     const char* host_name)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);
               
               
                   
    void stop (SystemStateMachine* stateMachine,       
               CORBA::Boolean emergency)
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);



private:

    SystemStarted (const SystemStarted& rhs); 
        

    SystemStarted& operator= (const SystemStarted& rhs);
};





/**
 *Purpose:
 *<p> The stopping state.
 */
class SystemStopping : public virtual SystemState
{

public:

    SystemStopping();
    
    void entryAction (SystemStateMachine* stateMachine);

    void doActivity (SystemStateMachine* stateMachine);

    CdmwPlatformMngt::SystemStatus status();

    void completion_event (SystemStateMachine* stateMachine)
         throw (CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException);
        
        
private:

    SystemStopping (const SystemStopping& rhs); 
        

    SystemStopping& operator= (const SystemStopping& rhs);       
    
};

/**
 *Purpose:
 *<p> The stopped state.
 */
class SystemStopped : public virtual SystemState
{

public:

    SystemStopped();
    
    void entryAction (SystemStateMachine* stateMachine);

    CdmwPlatformMngt::SystemStatus status();
    
    CdmwPlatformMngt::Application_ptr add_application (
                              SystemStateMachine* stateMachine,
                              const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                              const CdmwPlatformMngt::ApplicationDef& application_def)
            throw (CdmwPlatformMngt::ApplicationAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::ProcessPropertyExceptions,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessCommand, 
                   CdmwPlatformMngt::DuplicateProcessCommandArg,
                   CdmwPlatformMngt::DuplicateEntityCommand, 
                   CdmwPlatformMngt::DuplicateEntityCommandArg,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);


    void remove_application (SystemStateMachine* stateMachine,        
                             const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
                   
    
    void set_init_graph (SystemStateMachine* stateMachine,       
                         const CdmwPlatformMngt::GraphElements& application_graph)
            throw (OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
                   
                   
    CdmwPlatformMngt::Host_ptr 
      add_host(SystemStateMachine* stateMachine,
               const CdmwPlatformMngt::HostDef& host_def)
        throw (CdmwPlatformMngt::HostAlreadyExists,
               CdmwPlatformMngt::DuplicateEntity,
               CosPropertyService::MultipleExceptions,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);




    void remove_host(SystemStateMachine* stateMachine,
                     const char* host_name)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);
               
               
               

    void start (SystemStateMachine* stateMachine,        
                const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);


private:

    SystemStopped (const SystemStopped& rhs); 

    SystemStopped& operator= (const SystemStopped& rhs);        

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_SYSTEM_STATES_HPP

