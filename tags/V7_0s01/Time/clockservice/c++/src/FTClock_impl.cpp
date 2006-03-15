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
 
#include "clockservice/FTClock_impl.hpp"
#include "clockservice/StateTransferConfig.hpp"
#include "clockservice/FTController_impl.hpp"

Cdmw::clock::FTClock_impl::FTClock_impl(CORBA::ORB_ptr orb,
                                        ACE_Reactor* reactor)
    : Cdmw::clock::Executor_impl(reactor),
      m_controllerIds(0)
{
    try {
        // Timeout for cohort
        Cdmw::OsSupport::OS::Timeval cohort_timeout;
        cohort_timeout.seconds = COHORT_TIMEOUT;
        cohort_timeout.microseconds = 0;
        
        // Timeout for coordinator
        Cdmw::OsSupport::OS::Timeval coordinator_timeout;
        coordinator_timeout.seconds = COORDINATOR_TIMEOUT;
        coordinator_timeout.microseconds = 0;
        
        
        CORBA::Object_var obj =
            orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA
            = PortableServer::POA::_narrow(obj.in());
        
//         m_controllerDataStore =
//             new Cdmw::clock::ControllerDataStore(CONTROLLER_DATA_IDENTIFIER,
//                                                  orb,
//                                                  rootPOA.in(),
//                                                  MAX_TRANSACTION_IN_PROGRESS,
//                                                  MAX_TRANSACTION_DONE,
//                                                  cohort_timeout,
//                                                  coordinator_timeout);
        
        m_controllerDataStorageHome =
            new Cdmw::clock::ControllerDataStorageHome(CONTROLLER_DATA_IDENTIFIER);
    } catch (const CORBA::SystemException & e) {
        std::cerr << e << std::endl;
        throw;
    }
}


Cdmw::clock::FTClock_impl::~FTClock_impl()
{
    delete m_controllerDataStorageHome;
//     delete m_controllerDataStore;
}


::CosClockService::PeriodicExecution::Controller_ptr
Cdmw::clock::FTClock_impl::enable_periodic_execution
(CosClockService::PeriodicExecution::Periodic_ptr on)
    throw (CORBA::SystemException)
{
    std::cout << "[Executor]:>> Creating Controller " << std::endl;
    if (CORBA::is_nil(on)) {
        std::cout << "[Executor]:>> Invalid Periodic " << std::endl;
        throw CORBA::BAD_PARAM();
    }
    
    Cdmw::clock::ControllerData data;
    data.id = m_controllerIds;
    data.target =
        CosClockService::PeriodicExecution::Periodic::_duplicate(on);
    data.timer_info.timer_status = Cdmw::clock::CREATED;

    std::cout << "[Executor]:>> Transfering  Controller State" << std::endl;
    try {
        ControllerDataStorageObject obj =
            m_controllerDataStorageHome->create(m_controllerIds, data);
    } catch (...) {
        std::cout << "[Executor]:>> Unable to perform  Controller State Transfer" << std::endl;
    }

    Cdmw::clock::FTController_impl* ctrl = 
        new Cdmw::clock::FTController_impl(m_controllerIds,
                                           *this,
                                           on,
                                           *m_controllerDataStorageHome,
                                           *m_controllerDataStore,
                                           this->get_reactor_lock_free());
    

    m_controllerIds++; 
    std::cout << "[Executor]:>> Created Controller " << std::endl;
    return ctrl->_this();
}


void
Cdmw::clock::FTClock_impl::activate()
{
    std::cout << "############################################################" << std::endl;
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>  Activating  FTClock Replica" << std::endl;
    
    Cdmw::clock::ControllerData_var data;

    try {
        Cdmw::clock::FTController_impl* ctrl;
        while (true) {
            data =
                m_controllerDataStorageHome->find_data_by_oid(m_controllerIds);
            ctrl = 
                new Cdmw::clock::FTController_impl(m_controllerIds,
                                                   *this,
                                                   data->target.in(),
                                                   *m_controllerDataStorageHome,
                                                   *m_controllerDataStore,
                                                   this->get_reactor_lock_free());
            if (data->timer_info.timer_status == Cdmw::clock::RUNNING)
                ctrl->start(data->timer_info.period,
                            data->timer_info.interval,
                            data->timer_info.execution_limit,
                            data->timer_info.data);
            m_controllerIds++;
        }
    } catch (const Cdmw::FT::NotFoundException& e) { }
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>  Complete Executor_impl Initialization" << std::endl;
    std::cout << "############################################################" << std::endl;

}


                                                           
