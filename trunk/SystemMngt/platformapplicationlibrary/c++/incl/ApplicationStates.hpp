/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#ifndef INCL_PLATFORMMNGT_APPLICATION_STATES_HPP
#define INCL_PLATFORMMNGT_APPLICATION_STATES_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessProxy.stub.hpp"
#include "SystemMngt/platformlibrary/StateMachineTemplate.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class Application_impl;
class ApplicationState;

typedef StateMachine< Application_impl, ApplicationState >
    ApplicationStateMachine;

/**
 *Purpose:
 *<p> The abstract state of the state machine.
 * Provides the default implementation of an application state.
 */ 
class ApplicationState : public virtual ApplicationStateMachine::AbstractState
{
private:
    ApplicationState(
        const ApplicationState& rhs );

    ApplicationState& operator=(
        const ApplicationState& rhs );

protected:
    ApplicationState() {};

public:
    virtual ~ApplicationState() {};

public:
    virtual
    void entryAction( ApplicationStateMachine* stateMachine )
    {};

    virtual
    void exitAction( ApplicationStateMachine* stateMachine )
    {};

    virtual
    void doActivity( ApplicationStateMachine* stateMachine )
    {};

    virtual
    void completionTransition( ApplicationStateMachine* stateMachine )
    {};

    virtual
    CdmwPlatformMngt::ApplicationStatus status() = 0;

    virtual
    void define(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::ApplicationDef& application_def )
            throw( CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessActivityPoint,
                   CdmwPlatformMngt::AlreadyDone,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    CdmwPlatformMngt::ProcessProxy_ptr add_process(
        ApplicationStateMachine* stateMachine,
        CdmwPlatformMngt::ProcessDef* process_def )
            throw( CdmwPlatformMngt::ProcessAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void remove_process(
        ApplicationStateMachine* stateMachine,
        const char* process_name )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::ProcessStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void set_init_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void set_stop_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void initialise(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void resume(
        ApplicationStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void run(
        ApplicationStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void stop(
        ApplicationStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException )
    {
    };

    virtual
    void suspension_event(
        ApplicationStateMachine* stateMachine )
    {
    };

    virtual
    void completion_event(
        ApplicationStateMachine* stateMachine )
    {
    };

    virtual
    void non_completion_event(
        ApplicationStateMachine* stateMachine )
    {
    };
};

/**
 *Purpose:
 *<p> The initial state.
 */
class ApplicationInitial : public virtual ApplicationState
{
private:
    ApplicationInitial(
        const ApplicationInitial& rhs );

    ApplicationInitial& operator=(
        const ApplicationInitial& rhs );

public:
    ApplicationInitial();

    CdmwPlatformMngt::ApplicationStatus status();

    void define(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::ApplicationDef& application_def )
            throw( CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessActivityPoint,
                   CdmwPlatformMngt::AlreadyDone,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::ProcessProxy_ptr add_process(
        ApplicationStateMachine* stateMachine,
        CdmwPlatformMngt::ProcessDef* process_def )
            throw( CdmwPlatformMngt::ProcessAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void remove_process(
        ApplicationStateMachine* stateMachine,
        const char* process_name )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::ProcessStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_init_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_stop_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void initialise(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

};

/**
 *Purpose:
 *<p> The inititalising state.
 */
class ApplicationInitialising : public virtual ApplicationState
{
private:
    ApplicationInitialising(
        const ApplicationInitialising& rhs );

    ApplicationInitialising& operator=(
        const ApplicationInitialising& rhs );

public:
    ApplicationInitialising();

    void entryAction( ApplicationStateMachine* stateMachine );

    void exitAction( ApplicationStateMachine* stateMachine );

    void doActivity( ApplicationStateMachine* stateMachine );

    CdmwPlatformMngt::ApplicationStatus status();

    void set_stop_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void stop(
        ApplicationStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );

    void completion_event(
        ApplicationStateMachine* stateMachine );

    void non_completion_event(
        ApplicationStateMachine* stateMachine );

    void suspension_event(
        ApplicationStateMachine* stateMachine );
};

/**
 *Purpose:
 *<p> The initialisation suspended state.
 */
class ApplicationInitialisationSuspended : public virtual ApplicationState
{
private:
    ApplicationInitialisationSuspended(
        const ApplicationInitialisationSuspended& rhs );

    ApplicationInitialisationSuspended& operator=(
        const ApplicationInitialisationSuspended& rhs );

public:
    ApplicationInitialisationSuspended();

    void entryAction( ApplicationStateMachine* stateMachine );

    CdmwPlatformMngt::ApplicationStatus status();

    void set_init_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_stop_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void resume(
        ApplicationStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void stop(
        ApplicationStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );

};

/**
 *Purpose:
 *<p> The initialising incomplete state.
 */
class ApplicationInitialisationIncomplete : public virtual ApplicationState
{
private:
    ApplicationInitialisationIncomplete(
        const ApplicationInitialisationIncomplete& rhs );

    ApplicationInitialisationIncomplete& operator=(
        const ApplicationInitialisationIncomplete& rhs );

public:
    ApplicationInitialisationIncomplete();
    
    void entryAction( ApplicationStateMachine* stateMachine );

    CdmwPlatformMngt::ApplicationStatus status();

    void set_init_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_stop_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void stop(
        ApplicationStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The initialised state.
 */
class ApplicationInitialised : public virtual ApplicationState
{
private:
    ApplicationInitialised(
        const ApplicationInitialised& rhs );

    ApplicationInitialised& operator=(
        const ApplicationInitialised& rhs );

public:
    ApplicationInitialised();
    
    void entryAction( ApplicationStateMachine* stateMachine );

    CdmwPlatformMngt::ApplicationStatus status();

    CdmwPlatformMngt::ProcessProxy_ptr add_process(
        ApplicationStateMachine* stateMachine,
        CdmwPlatformMngt::ProcessDef* process_def )
            throw( CdmwPlatformMngt::ProcessAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void remove_process(
        ApplicationStateMachine* stateMachine,
        const char* process_name )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::ProcessStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_init_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_stop_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void run(
        ApplicationStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void stop(
        ApplicationStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );

};

/**
 *Purpose:
 *<p> The run request state.
 */
class ApplicationRunRequest : public virtual ApplicationState
{
private:
    ApplicationRunRequest(
        const ApplicationRunRequest& rhs );

    ApplicationRunRequest& operator=(
        const ApplicationRunRequest& rhs );

public:
    ApplicationRunRequest();
    
    void entryAction( ApplicationStateMachine* stateMachine );

    void doActivity( ApplicationStateMachine* stateMachine );

    CdmwPlatformMngt::ApplicationStatus status();

    void set_init_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_stop_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void stop(
        ApplicationStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );

    void completion_event(
        ApplicationStateMachine* stateMachine );
};

/**
 *Purpose:
 *<p> The running state.
 */
class ApplicationRunning : public virtual ApplicationState
{
private:
    ApplicationRunning(
        const ApplicationRunning& rhs );

    ApplicationRunning& operator=(
        const ApplicationRunning& rhs );

public:
    ApplicationRunning();
    
    void entryAction( ApplicationStateMachine* stateMachine );

    CdmwPlatformMngt::ApplicationStatus status();

    CdmwPlatformMngt::ProcessProxy_ptr add_process(
        ApplicationStateMachine* stateMachine,
        CdmwPlatformMngt::ProcessDef* process_def )
            throw( CdmwPlatformMngt::ProcessAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void remove_process(
        ApplicationStateMachine* stateMachine,
        const char* process_name )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::ProcessStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_init_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_stop_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void stop(
        ApplicationStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );

};

/**
 *Purpose:
 *<p> The stopping state.
 */
class ApplicationStopping : public virtual ApplicationState
{
private:
    ApplicationStopping(
        const ApplicationStopping& rhs );

    ApplicationStopping& operator=(
        const ApplicationStopping& rhs );

public:
    ApplicationStopping();
    
    void entryAction( ApplicationStateMachine* stateMachine );

    void doActivity( ApplicationStateMachine* stateMachine );

    CdmwPlatformMngt::ApplicationStatus status();

    void set_init_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void completion_event(
        ApplicationStateMachine* stateMachine );
};

/**
 *Purpose:
 *<p> The stopped state.
 */
class ApplicationStopped : public virtual ApplicationState
{
private:
    ApplicationStopped(
        const ApplicationStopped& rhs );

    ApplicationStopped& operator=(
        const ApplicationStopped& rhs );

public:
    ApplicationStopped();
    
    void entryAction( ApplicationStateMachine* stateMachine );

    CdmwPlatformMngt::ApplicationStatus status();

    CdmwPlatformMngt::ProcessProxy_ptr add_process(
        ApplicationStateMachine* stateMachine,
        CdmwPlatformMngt::ProcessDef* process_def )
            throw( CdmwPlatformMngt::ProcessAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void remove_process(
        ApplicationStateMachine* stateMachine,
        const char* process_name )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::ProcessStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_init_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_stop_graph(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngt::GraphElements& process_graph)
            throw( OutOfMemoryException,
                   CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::StepOutOfBound,
                   CdmwPlatformMngt::StepJump,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void initialise(
        ApplicationStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_STATES_HPP

