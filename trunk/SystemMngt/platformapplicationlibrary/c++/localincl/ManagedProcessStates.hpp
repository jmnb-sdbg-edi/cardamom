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


#ifndef INCL_PLATFORMMNGT_MANAGED_PROCESS_PROXY_STATES_HPP
#define INCL_PLATFORMMNGT_MANAGED_PROCESS_PROXY_STATES_HPP

#include "platformapplicationlibrary/ProcessProxy_impl.hpp"
#include "SystemMngt/platformlibrary/StateMachineTemplate.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class ManagedProcessProxy_impl;
class ManagedProcessState;

typedef StateMachine< ManagedProcessProxy_impl, ManagedProcessState >
    ManagedProcessStateMachine;

/**
 *Purpose:
 *<p> The abstract state of the state machine.
 * Provides the default implementation of a managed process state.
 */ 
class ManagedProcessState : public virtual ManagedProcessStateMachine::AbstractState
{
private:
    ManagedProcessState(
        const ManagedProcessState& rhs );

    ManagedProcessState& operator=(
        const ManagedProcessState& rhs );

protected:
    ManagedProcessState() {};

public:
    virtual ~ManagedProcessState() {};

public:
    virtual
    void entryAction(
        ManagedProcessStateMachine* stateMachine )
    {};

    virtual
    void exitAction(
        ManagedProcessStateMachine* stateMachine )
    {};

    virtual
    void doActivity(
        ManagedProcessStateMachine* stateMachine )
    {};

    virtual
    void completionTransition(
        ManagedProcessStateMachine* stateMachine )
    {};

    virtual
    CdmwPlatformMngt::ProcessStatus status() = 0;

    virtual
    char* status_info(
        ManagedProcessStateMachine* stateMachine );
    
    virtual
    void set_autoending(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void set_host(
        ManagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void set_process_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void set_life_cycle(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void set_monitoring_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void set_activity_point_defs(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ActivityPointDefs& point_defs )
            throw( CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    CdmwPlatformMngt::RequestStatus initialise(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    CdmwPlatformMngt::RequestStatus next_step(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::InvalidStep,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    CdmwPlatformMngt::RequestStatus run(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::NotReadyToRun,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    CdmwPlatformMngt::RequestStatus stop(
        ManagedProcessStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException )
    {
        return CdmwPlatformMngt::REQUEST_SUCCEED;
    };

    virtual
    void error_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info )
    {};

    virtual
    void monitoringFailure_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header )
            throw( CdmwPlatformMngt::IncompatibleType,
                   CORBA::SystemException )
    {};

    virtual
    void ending_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header )
    {};

    virtual
    void invalidate_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason )
    {};
};

/**
 *Purpose:
 *<p> The initial state.
 */
class ManagedProcessInitial : public virtual ManagedProcessState
{
private:
    ManagedProcessInitial(
        const ManagedProcessInitial& rhs );

    ManagedProcessInitial& operator=(
        const ManagedProcessInitial& rhs );

public:
    ManagedProcessInitial();

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        ManagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_life_cycle(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_monitoring_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_activity_point_defs(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ActivityPointDefs& point_defs )
            throw( CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The initialising state.
 */
class ManagedProcessInitialising : public virtual ManagedProcessState
{
private:
    ManagedProcessInitialising(
        const ManagedProcessInitialising& rhs );

    ManagedProcessInitialising& operator=(
        const ManagedProcessInitialising& rhs );

public:
    ManagedProcessInitialising();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    void doActivity(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

     void ending_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header );

    void invalidate_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );
};

/**
 *Purpose:
 *<p> The step performed state.
 */
class ManagedProcessStepPerformed : public virtual ManagedProcessState
{
private:
    ManagedProcessStepPerformed(
        const ManagedProcessStepPerformed& rhs );

    ManagedProcessStepPerformed& operator=(
        const ManagedProcessStepPerformed& rhs );

public:
    ManagedProcessStepPerformed();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    CdmwPlatformMngt::RequestStatus next_step(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::InvalidStep,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus stop(
        ManagedProcessStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );

    void error_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info );

    void ending_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header );

    void invalidate_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );
};

/**
 *Purpose:
 *<p> The initialised state.
 */
class ManagedProcessInitialised : public virtual ManagedProcessState
{
private:
    ManagedProcessInitialised(
        const ManagedProcessInitialised& rhs );

    ManagedProcessInitialised& operator=(
        const ManagedProcessInitialised& rhs );

public:
    ManagedProcessInitialised();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    CdmwPlatformMngt::RequestStatus run(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::NotReadyToRun,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus stop(
        ManagedProcessStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );

    void error_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info );

    void ending_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header );

    void invalidate_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );
};

/**
 *Purpose:
 *<p> The run request state.
 */
class ManagedProcessRunRequest : public virtual ManagedProcessState
{
private:
    ManagedProcessRunRequest(
        const ManagedProcessRunRequest& rhs );

    ManagedProcessRunRequest& operator=(
        const ManagedProcessRunRequest& rhs );

public:
    ManagedProcessRunRequest();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    void doActivity(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void ending_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header );

    void invalidate_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );
};

/**
 *Purpose:
 *<p> The running state.
 */
class ManagedProcessRunning : public virtual ManagedProcessState
{
private:
    ManagedProcessRunning(
        const ManagedProcessRunning& rhs );

    ManagedProcessRunning& operator=(
        const ManagedProcessRunning& rhs );

public:
    ManagedProcessRunning();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    CdmwPlatformMngt::RequestStatus stop(
        ManagedProcessStateMachine* stateMachine,
        CORBA::Boolean emergency )
            throw( CORBA::SystemException );

    void error_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info );

    void monitoringFailure_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header )
            throw( CdmwPlatformMngt::IncompatibleType,
                   CORBA::SystemException );

    void ending_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header );

    void invalidate_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );
};

/**
 *Purpose:
 *<p> The stopping state.
 */
class ManagedProcessStopping : public virtual ManagedProcessState
{
private:
    ManagedProcessStopping(
        const ManagedProcessStopping& rhs );

    ManagedProcessStopping& operator=(
        const ManagedProcessStopping& rhs );

public:
    ManagedProcessStopping();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    void doActivity(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void invalidate_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );
};

/**
 *Purpose:
 *<p> The stopped state.
 */
class ManagedProcessStopped : public virtual ManagedProcessState
{
private:
    ManagedProcessStopped(
        const ManagedProcessStopped& rhs );

    ManagedProcessStopped& operator=(
        const ManagedProcessStopped& rhs );

public:
    ManagedProcessStopped();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        ManagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_life_cycle(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw(CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The ended state.
 */
class ManagedProcessEnded : public virtual ManagedProcessState
{
private:
    ManagedProcessEnded(
        const ManagedProcessEnded& rhs );

    ManagedProcessEnded& operator=(
        const ManagedProcessEnded& rhs );

public:
    ManagedProcessEnded();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        ManagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_life_cycle(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The error state.
 */
class ManagedProcessFailedError : public virtual ManagedProcessState
{
private:
    ManagedProcessFailedError(
        const ManagedProcessFailedError& rhs );

    ManagedProcessFailedError& operator=(
        const ManagedProcessFailedError& rhs );

public:
    ManagedProcessFailedError();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        ManagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_life_cycle(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw(CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void error_event(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* error_info );
};

/**
 *Purpose:
 *<p> The monitoring error state.
 */
class ManagedProcessFailedMonitoringFailure : public virtual ManagedProcessState
{
private:
    ManagedProcessFailedMonitoringFailure(
        const ManagedProcessFailedMonitoringFailure& rhs );

    ManagedProcessFailedMonitoringFailure& operator=(
        const ManagedProcessFailedMonitoringFailure& rhs );

public:
    ManagedProcessFailedMonitoringFailure();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        ManagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_life_cycle(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw(CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The death state.
 */
class ManagedProcessFailedDeath : public virtual ManagedProcessState
{
private:
    ManagedProcessFailedDeath(
        const ManagedProcessFailedDeath& rhs );

    ManagedProcessFailedDeath& operator=(
        const ManagedProcessFailedDeath& rhs );

public:
    ManagedProcessFailedDeath();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        ManagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_life_cycle(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw(CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The discreapancy error state.
 */
class ManagedProcessFailedDiscrepancy : public virtual ManagedProcessState
{
private:
    ManagedProcessFailedDiscrepancy(
        const ManagedProcessFailedDiscrepancy& rhs );

    ManagedProcessFailedDiscrepancy& operator=(
        const ManagedProcessFailedDiscrepancy& rhs );

public:
    ManagedProcessFailedDiscrepancy();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        ManagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_life_cycle(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The timeout expiration state.
 */
class ManagedProcessFailedTimeout : public virtual ManagedProcessState
{
private:
    ManagedProcessFailedTimeout(
        const ManagedProcessFailedTimeout& rhs );

    ManagedProcessFailedTimeout& operator=(
        const ManagedProcessFailedTimeout& rhs );

public:
    ManagedProcessFailedTimeout();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        ManagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_life_cycle(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The failed invalid state.
 */
class ManagedProcessFailedInvalid : public virtual ManagedProcessState
{
private:
    ManagedProcessFailedInvalid(
        const ManagedProcessFailedInvalid& rhs );

    ManagedProcessFailedInvalid& operator=(
        const ManagedProcessFailedInvalid& rhs );

public:
    ManagedProcessFailedInvalid();

    void entryAction(
        ManagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        ManagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        ManagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_life_cycle(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::LifeCycleDef& life_cycle )
            throw( CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        ManagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_MANAGED_PROCESS_PROXY_STATES_HPP

