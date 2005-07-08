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


#ifndef INCL_PLATFORMMNGT_UNMANAGED_PROCESS_PROXY_STATES_HPP
#define INCL_PLATFORMMNGT_UNMANAGED_PROCESS_PROXY_STATES_HPP

#include "platformapplicationlibrary/ProcessProxy_impl.hpp"
#include "SystemMngt/platformlibrary/StateMachineTemplate.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class UnmanagedProcessProxy_impl;
class UnmanagedProcessState;

typedef StateMachine< UnmanagedProcessProxy_impl, UnmanagedProcessState >
        UnmanagedProcessStateMachine;

/**
 *Purpose:
 *<p> The abstract state of the state machine.
 * Provides the default implementation of an unmanaged process state.
 */ 
class UnmanagedProcessState
: public virtual UnmanagedProcessStateMachine::AbstractState
{
private:
    UnmanagedProcessState(
        const UnmanagedProcessState& rhs );

    UnmanagedProcessState& operator=(
        const UnmanagedProcessState& rhs );

protected:
    UnmanagedProcessState() {};

public:
    virtual ~UnmanagedProcessState() {};

public:
    virtual
    void entryAction(
        UnmanagedProcessStateMachine* stateMachine )
    {};

    virtual
    void exitAction(
        UnmanagedProcessStateMachine* stateMachine )
    {};

    virtual
    void doActivity(
        UnmanagedProcessStateMachine* stateMachine )
    {};

    virtual
    void completionTransition(
        UnmanagedProcessStateMachine* stateMachine )
    {};

    virtual
    CdmwPlatformMngt::ProcessStatus status() = 0;

    virtual
    char* status_info(
        UnmanagedProcessStateMachine* stateMachine );
    
    virtual
    void set_autoending(
        UnmanagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void set_host(
        UnmanagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    void set_process_info(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    CdmwPlatformMngt::RequestStatus initialise(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException )
    {
        throw CdmwPlatformMngt::IncompatibleStatus();
    };

    virtual
    CdmwPlatformMngt::RequestStatus stop(
        UnmanagedProcessStateMachine* stateMachine )
            throw( CORBA::SystemException )
    {
        return CdmwPlatformMngt::REQUEST_SUCCEED;
    };

    virtual
    void ending_event(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header )
    {};

    virtual
    void invalidate_event(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* reason );
};

/**
 *Purpose:
 *<p> The initial state.
 */
class UnmanagedProcessInitial : public virtual UnmanagedProcessState
{
private:
    UnmanagedProcessInitial(
        const UnmanagedProcessInitial& rhs );

    UnmanagedProcessInitial& operator=(
        const UnmanagedProcessInitial& rhs );

public:
    UnmanagedProcessInitial();

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        UnmanagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        UnmanagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The initialising state.
 */
class UnmanagedProcessInitialising : public virtual UnmanagedProcessState
{
private:
    UnmanagedProcessInitialising(
        const UnmanagedProcessInitialising& rhs );

    UnmanagedProcessInitialising& operator=(
        const UnmanagedProcessInitialising& rhs );

public:
    UnmanagedProcessInitialising();

    void entryAction(
        UnmanagedProcessStateMachine* stateMachine );

    void doActivity(
        UnmanagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();
};

/**
 *Purpose:
 *<p> The running state.
 */
class UnmanagedProcessRunning : public virtual UnmanagedProcessState
{
private:
    UnmanagedProcessRunning(
        const UnmanagedProcessRunning& rhs );

    UnmanagedProcessRunning& operator=(
        const UnmanagedProcessRunning& rhs );

public:
    UnmanagedProcessRunning();

    void entryAction(
        UnmanagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    CdmwPlatformMngt::RequestStatus stop(
        UnmanagedProcessStateMachine* stateMachine )
            throw( CORBA::SystemException );

    void ending_event(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::EventHeader& header );
};

/**
 *Purpose:
 *<p> The stopped state.
 */
class UnmanagedProcessStopped : public virtual UnmanagedProcessState
{
private:
    UnmanagedProcessStopped(
        const UnmanagedProcessStopped& rhs );

    UnmanagedProcessStopped& operator=(
        const UnmanagedProcessStopped& rhs );

public:
    UnmanagedProcessStopped();

    void entryAction(
        UnmanagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        UnmanagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        UnmanagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The ended state.
 */
class UnmanagedProcessEnded : public virtual UnmanagedProcessState
{
private:
    UnmanagedProcessEnded(
        const UnmanagedProcessEnded& rhs );

    UnmanagedProcessEnded& operator=(
        const UnmanagedProcessEnded& rhs );

public:
    UnmanagedProcessEnded();

    void entryAction(
        UnmanagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        UnmanagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        UnmanagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The death state.
 */
class UnmanagedProcessFailedDeath : public virtual UnmanagedProcessState
{
private:
    UnmanagedProcessFailedDeath(
        const UnmanagedProcessFailedDeath& rhs );

    UnmanagedProcessFailedDeath& operator=(
        const UnmanagedProcessFailedDeath& rhs );

public:
    UnmanagedProcessFailedDeath();

    void entryAction(
        UnmanagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        UnmanagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        UnmanagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

/**
 *Purpose:
 *<p> The failed invalid state.
 */
class UnmanagedProcessFailedInvalid : public virtual UnmanagedProcessState
{
private:
    UnmanagedProcessFailedInvalid(
        const UnmanagedProcessFailedInvalid& rhs );

    UnmanagedProcessFailedInvalid& operator=(
        const UnmanagedProcessFailedInvalid& rhs );

public:
    UnmanagedProcessFailedInvalid();

    void entryAction(
        UnmanagedProcessStateMachine* stateMachine );

    CdmwPlatformMngt::ProcessStatus status();

    void set_autoending(
        UnmanagedProcessStateMachine* stateMachine )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_host(
        UnmanagedProcessStateMachine* stateMachine,
        const char* host_name )
            throw( CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    void set_process_info(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngt::ProcessInfo& process_info )
            throw( CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );

    CdmwPlatformMngt::RequestStatus initialise(
        UnmanagedProcessStateMachine* stateMachine,
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
            throw( CdmwPlatformMngt::DuplicateActivityPoint,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException );
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_UNMANAGED_PROCESS_PROXY_STATES_HPP

