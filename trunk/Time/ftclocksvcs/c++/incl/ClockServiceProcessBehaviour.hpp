/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */
#ifndef _CARDAMOM_EVOT_CLOCKSVC_CLOCK_SERVICE_PROCESS_BEHAVIOUR_H_
#define _CARDAMOM_EVOT_CLOCKSVC_CLOCK_SERVICE_PROCESS_BEHAVIOUR_H_

// -- CDMW Includes --
#include <Foundation/orbsupport/CORBA.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>

// -- STD C++ Include
#include <string>

#include "Time/clockservice/UTCTimeService_impl.hpp"
#include "Time/clockservice/ControlledClock_impl.hpp"
#include "Time/clockservice/ClockCatalog_impl.hpp"
#include "Time/clockservice/Clock_impl.hpp"
#include "Time/clockservice/UTC_impl.hpp"

#include "CosClockServiceS.h"

#include "ace/Get_Opt.h"
#include "ace/Thread.h"
#include "ace/Thread_Manager.h"
#include "ace/TP_Reactor.h"
#include "ace/Get_Opt.h"
#include "ace/Task.h"
#include "ace/Timer_Wheel.h"

#include "orbsvcs/Property/CosPropertyService_i.h"
#include "tao/ORB_Core.h"
#include "tao/PortableServer/ORB_Manager.h"

namespace Cdmw {
    namespace clock {
        namespace svcs {
            class ClockServiceProcessBehaviour;
            class ORBWorker;
            class ReactorWorker;
        }
    }
}


class Cdmw::clock::svcs::ORBWorker : public ACE_Task_Base
{
public:
    ORBWorker (CORBA::ORB_ptr orb);
    virtual ~ORBWorker();
    
public:
    virtual int svc (void);

private:
    CORBA::ORB_var orb_;
};


class Cdmw::clock::svcs::ReactorWorker : public ACE_Task_Base
{
public:
    virtual ~ReactorWorker();
    
public:
    virtual int svc (void);
};


class Cdmw::clock::svcs::ClockServiceProcessBehaviour
    : public virtual Cdmw::CdmwInit::ProcessControl
{
public:
    ClockServiceProcessBehaviour(CORBA::ORB_ptr orb,
                                 size_t         orb_thread_num,
                                 size_t         reactor_thread_num,
                                 size_t         timer_wheel_spokes,
                                 size_t         timer_wheel_resolution, 
                                 std::string    clock_catalog_file_name,
                                 std::string    local_clock_file_name)  
        throw();
    
    ~ClockServiceProcessBehaviour() throw();

public:
    virtual CORBA::ULong nb_steps()
        throw( CORBA::SystemException );
    
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
        throw( CORBA::SystemException );
    
    virtual void on_next_step()
        throw( CdmwPlatformMngt::Process::InvalidStep,
               CORBA::SystemException );

    virtual void on_run()
        throw( CdmwPlatformMngt::Process::NotReadyToRun,
               CORBA::SystemException );

    virtual void on_stop()
        throw( CORBA::SystemException );

protected:
    void initClockCatalog();
    void initLocalClock();
    void initUTCTimeService();
    void initControlledClock();
    void initExecutor();
    void initPropertyService();
    void initValueTypes();
private:
    
    CORBA::ORB_var m_orb;
    
    size_t m_orb_thread_num;
    size_t m_reactor_thread_num;
    size_t m_timer_wheel_spokes;
    size_t m_timer_wheel_resolution;

    std::string m_clock_catalog_file_name;
    std::string m_local_clock_file_name;

    ACE_TP_Reactor  m_tp_reactor;
    ACE_Reactor     m_reactor;
    ACE_Timer_Wheel m_timer_queue;

    Cdmw::clock::svcs::ReactorWorker m_reactor_worker;
    Cdmw::clock::svcs::ORBWorker     m_orb_worker;

    Cdmw::clock::ClockCatalog_impl    m_clockcatalog_impl;
    Cdmw::clock::Clock_impl           m_clock_impl;
    Cdmw::clock::UtcTimeService_impl  m_utc_service_impl;
    Cdmw::clock::ControlledClock_impl m_controlledClock_impl;

    CosClockService::ClockCatalog_var    m_clockcatalog_var;
    CosClockService::UtcTimeService_var  m_utctimeservice_var;
    CosClockService::ControlledClock_var m_controlled_clock_var;
    
    CosClockService::PeriodicExecution::Executor_var m_executor_var;
    
    TAO_PropertySetDef                         m_propset_impl;
    CosPropertyService::PropertySetDef_var     m_propset_var;
    TAO_PropertySetFactory                     m_propset_factory_impl;
    CosPropertyService::PropertySetFactory_var m_propset_factory; 
};

#endif /* _CARDAMOM_EVOT_CLOCKSVC_CLOCK_SERVICE_PROCESS_BEHAVIOUR_H_ */
