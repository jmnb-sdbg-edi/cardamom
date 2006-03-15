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

#include <Time/clockservice/config.hpp>
#include "clockservice/Executor_impl.hpp"
#include "clockservice/Controller_impl.hpp"

#include <iostream>
#include <Foundation/common/Assert.hpp>

using namespace std;
using namespace Cdmw::clock;
using namespace CosPropertyService;
using namespace CosClockService;

// Only to have an extern  ref for base_time_
extern  const TimeBase::TimeT base_time_;


///////////////////////////////////////////////////////////////////////////////

Executor_impl::Executor_impl(ACE_Reactor* reactor) 
    : m_reactor(reactor)
{
}

///////////////////////////////////////////////////////////////////////////////

Executor_impl::~Executor_impl ()
{
    // memory will be managed form ORB
    detach_all_controllers_lock_free();
}

///////////////////////////////////////////////////////////////////////////////

::CosClockService::PeriodicExecution::Controller_ptr
Executor_impl::enable_periodic_execution
(CosClockService::PeriodicExecution::Periodic_ptr on)
    throw (CORBA::SystemException)
{
    try
    {
        CDMW_ASSERT(!CORBA::is_nil(on));
        Cdmw::clock::Controller_impl* ctrl = 
            new Cdmw::clock::Controller_impl(*this, on, m_reactor);
        attach_controller_lock_free(ctrl);

        /* ----- DEBUG CODE [BEGIN] ----- *
        std::cerr << std::endl << "Attached controller: " << ctrl << std::endl;
        print_attached_controllers_lock_free();
         * ----- DEBUG CODE [ END ] ----- */

        return ctrl->_this();
    }
    catch (std::exception& e)
    {  
       std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        CDMW_ASSERT(!"Unexpected Exception!!!");
    }
    return CosClockService::PeriodicExecution::Controller::_nil();
}

///////////////////////////////////////////////////////////////////////////////

void
Executor_impl::detach_terminated_controller(Cdmw::clock::Controller_impl* ctrl)
{
    assert(ctrl != 0);
    assert(ctrl->get_status() == Cdmw::clock::TERMINATED);
    detach_controller_lock_free(ctrl);

    /* ----- DEBUG CODE [BEGIN] ----- *
    std::cerr << std::endl << "Detached controller: " << ctrl << std::endl;
    print_attached_controllers_lock_free();
     * ----- DEBUG CODE [ END ] ----- */
}
                                                                                                                                                             
///////////////////////////////////////////////////////////////////////////////

void
Executor_impl::apply_policy_to_controllers_lock_free(const PolicyFunctorManager& pfm)
{
    Executor_impl::ControllerList_t aux_controller_list(m_controller_list.begin(), m_controller_list.end());
    std::for_each(aux_controller_list.begin(), aux_controller_list.end(), pfm);
}

///////////////////////////////////////////////////////////////////////////////

void
Executor_impl::attach_controller_lock_free(Cdmw::clock::Controller_impl* ctrl)
{
    assert(ctrl != 0);
    ControllerList_t::iterator it_beg = m_controller_list.begin();
    ControllerList_t::iterator it_end = m_controller_list.end();
    ControllerList_t::iterator it_cur = std::find(it_beg, it_end, ctrl);
    if (it_cur == it_end)
        m_controller_list.push_back(ctrl);
}

///////////////////////////////////////////////////////////////////////////////

void
Executor_impl::detach_controller_lock_free(Cdmw::clock::Controller_impl* ctrl)
{
    ControllerList_t::iterator it_beg = m_controller_list.begin();
    ControllerList_t::iterator it_end = m_controller_list.end();
    ControllerList_t::iterator it_cur = std::find(it_beg, it_end, ctrl);
    if (it_cur != it_end)
    {
        size_t size = m_controller_list.size();
        m_controller_list.erase(it_cur);
        assert(m_controller_list.size() == size - 1);
    }
}

void
Executor_impl::detach_all_controllers_lock_free()
{
    m_controller_list.clear();
}

///////////////////////////////////////////////////////////////////////////////

size_t
Executor_impl::num_of_attached_controllers_lock_free() const
{
    return m_controller_list.size();
}

///////////////////////////////////////////////////////////////////////////////

void
Executor_impl::print_attached_controllers_lock_free() const
{
    size_t ctrl_list_size = m_controller_list.size();
    std::cerr << "List of attached controllers:" << std::endl;
    for (size_t i = 0; i < ctrl_list_size; ++i)
        std::cerr << " - attached controller: " << m_controller_list[i] << std::endl;
    std::cerr << "There are " << ctrl_list_size << " controller(s) attached" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

ACE_Reactor*
Executor_impl::get_reactor_lock_free() const
{
    return m_reactor;
}

///////////////////////////////////////////////////////////////////////////////

Executor_impl::PolicyFunctorManager::PolicyFunctorManager(Cdmw::clock::ControlledExecutor_ptr executor,
                                                          ControllerUpdateMemberFunction_ptr  function)
    : m_executor(executor),
      m_function(function)
{
}

///////////////////////////////////////////////////////////////////////////////

bool
Executor_impl::PolicyFunctorManager::operator()(Cdmw::clock::Controller_impl* ctrl)
{
    try
    {
        assert(ctrl != 0);
        Cdmw::clock::policy::ControllerUpdateID id = ctrl->get_update_policy();
        if (id == Cdmw::clock::policy::UNDEFINED)
            id = m_executor->get_controller_update_policy();
        Cdmw::clock::policy::ControllerUpdate_ptr ptr = 0;
        ptr = Cdmw::clock::policy::ControllerUpdateAdmin_impl::instance()->get_policy(id);
        assert((ptr != 0) && (ptr->*m_function));
        (ptr->*m_function)(ctrl);
    }
    catch (CORBA::Exception& e)
    {
        std::cerr << e << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << " Exception!!!" << std::endl;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////


