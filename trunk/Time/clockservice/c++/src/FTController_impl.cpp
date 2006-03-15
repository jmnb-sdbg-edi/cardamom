/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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
 
#include "clockservice/FTController_impl.hpp"
#include "clockservice/Executor_impl.hpp"

Cdmw::clock::FTController_impl::FTController_impl
(Cdmw::clock::Ident                                id,
 Cdmw::clock::Executor_impl&                              clock,
 CosClockService::PeriodicExecution::Periodic_ptr  periodic, 
 Cdmw::clock::ControllerDataStorageHome&           controllerDataStorageHome,
 Cdmw::clock::ControllerDataStore&                 controllerDataStore, 
 ACE_Reactor*                                      reactor)
    : Cdmw::clock::Controller_impl(clock, periodic, reactor),
      m_id(id),
      m_controllerDataStorageHome(controllerDataStorageHome),
      m_controllerDataStore(controllerDataStore)
{
    // No-Op
}

Cdmw::clock::FTController_impl::~FTController_impl()
{
    // No-Op
}

void
Cdmw::clock::FTController_impl::start(TimeBase::TimeT    period, 
                                      TimeBase::TimeT    with_offset,
                                      CORBA::ULong       execution_limit,
                                      const CORBA::Any&  params)
    throw (CORBA::SystemException)
{
    // Call Superclass method
    this->Controller_impl::start(period, with_offset, execution_limit, params);
    // Do the state transfer
    Cdmw::clock::ControllerData data;
    data.id = m_id;
    data.target = this->periodic();
    data.timer_info.period = period;
    data.timer_info.interval = with_offset;
    data.timer_info.next_expiration_time =
        period + with_offset + this->m_clock.current_time();
    data.timer_info.data = params;
    data.timer_info.timer_status = Cdmw::clock::RUNNING;
    
    try {
        ControllerDataStorageObject obj =
            m_controllerDataStorageHome.find_by_oid(m_id);
        obj.set(data);
    } catch (const Cdmw::FT::NotFoundException& e) {
        std::cerr << "[Controller]:>> Unable to Store Timer Data" << std::endl;
    }     
}
                                      
