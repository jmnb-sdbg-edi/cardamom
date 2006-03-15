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

#include "clockservice/ControllerEnforceIntervalPolicy.hpp"
#include "clockservice/Executor_impl.hpp"
#include "clockservice/Controller_impl.hpp"
#include "Time/clockservice/Util.hpp"

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp>

////////////////////////////////////////////////////////////////////////////////

namespace 
{
}

////////////////////////////////////////////////////////////////////////////////

Cdmw::clock::ControllerEnforceIntervalPolicy::ControllerEnforceIntervalPolicy ()
{
}

////////////////////////////////////////////////////////////////////////////////

Cdmw::clock::ControllerEnforceIntervalPolicy::~ControllerEnforceIntervalPolicy ()
{
}

////////////////////////////////////////////////////////////////////////////////

void
Cdmw::clock::ControllerEnforceIntervalPolicy::on_set (
    ::Cdmw::clock::ControllerEx_ptr controller_ptr )
  throw (CORBA::SystemException)
{

    if (!this->isValid(controller_ptr)) return;
    // do nothing

    //Begin Thales
    Controller_impl* controller = dynamic_cast<Controller_impl*>(controller_ptr);
    CORBA::Any param = controller->get_parameter();
    TimeBase::TimeT period = controller->get_period();
    
    if (controller->get_status() == Cdmw::clock::PAUSED)
      {
	// std::cout << ">>Controller Status == PAUSED" << std::endl;
        this->freeze_controller(controller);
        this->update_controller_period(controller, period);
        return;
      }
    
    if (controller->get_status() == Cdmw::clock::STARTED)
      {
        controller->pause();
	
        Executor_impl* ref = controller->retrieve_executor();
        CDMW_ASSERT(ref);
	
	TimeBase::TimeT now = ref->current_time(); 
        TimeBase::TimeT time_origin = controller->get_controller_time_origin();
        TimeBase::TimeT real_delta2finish = Cdmw::clock::compute_delta2end(*ref, period, time_origin);
        long double ld_virt_delta2finish = ((long double) fabs(ref->retrieve_rate())) * real_delta2finish;
        TimeBase::TimeT virt_delta2finish = (TimeBase::TimeT) (ld_virt_delta2finish);
	
        try
	  {
            long double rate = ref->retrieve_rate();
            TimeBase::TimeT time_at = (rate > 0.0) ? now + virt_delta2finish : now - virt_delta2finish;
            controller->resume_at(time_at);
	  }
        catch(CosClockService::PeriodicExecution::Controller::TimePast&)
	  {
            std::cerr << "Ops time in the past!!!" << std::endl;
	  }
        catch(...)
	  {
	  }
      }
    //End Thales
}

////////////////////////////////////////////////////////////////////////////////

void
Cdmw::clock::ControllerEnforceIntervalPolicy::on_set_rate (
    ::Cdmw::clock::ControllerEx_ptr controller_ptr)
  throw (CORBA::SystemException)
{

    if (!this->isValid(controller_ptr)) return;
                
    Controller_impl* controller = dynamic_cast<Controller_impl*>(controller_ptr);
    CORBA::Any param = controller->get_parameter();
    TimeBase::TimeT period = controller->get_period();

    if (controller->get_status() == Cdmw::clock::PAUSED)
    {
       // std::cout << ">>Controller Status == PAUSED" << std::endl;
        this->freeze_controller(controller);
        this->update_controller_period(controller, period);
        return;
    }

    if (controller->get_status() == Cdmw::clock::STARTED)
    {
        controller->pause();
    
        Executor_impl* ref = controller->retrieve_executor();
        CDMW_ASSERT(ref);

        TimeBase::TimeT now = ref->current_time(); 
        TimeBase::TimeT time_origin = controller->get_controller_time_origin();
        TimeBase::TimeT real_delta2finish = Cdmw::clock::compute_delta2end(*ref, period, time_origin);
        long double ld_virt_delta2finish = ((long double) fabs(ref->retrieve_rate())) * real_delta2finish;
        TimeBase::TimeT virt_delta2finish = (TimeBase::TimeT) (ld_virt_delta2finish);

        try
        {
            long double rate = ref->retrieve_rate();
            TimeBase::TimeT time_at = (rate > 0.0) ? now + virt_delta2finish : now - virt_delta2finish;
            controller->resume_at(time_at);
        }
        catch(CosClockService::PeriodicExecution::Controller::TimePast&)
        {
            std::cerr << "Ops time in the past!!!" << std::endl;
        }
        catch(...)
        {
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void
Cdmw::clock::ControllerEnforceIntervalPolicy::on_pause
    (::Cdmw::clock::ControllerEx_ptr controller_ptr)
    throw (CORBA::SystemException)
{
    if (!this->isValid(controller_ptr)) return;

    Controller_impl* controller = dynamic_cast<Controller_impl*>(controller_ptr);

    controller->pause();
    //std::cout << ">> Controller Status == PAUSED" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void
Cdmw::clock::ControllerEnforceIntervalPolicy::on_terminate
    (::Cdmw::clock::ControllerEx_ptr controller)
    throw (CORBA::SystemException)
{
    if (!this->isValid(controller)) return;

    controller->terminate();
}

////////////////////////////////////////////////////////////////////////////////

void
Cdmw::clock::ControllerEnforceIntervalPolicy::on_resume
    (::Cdmw::clock::ControllerEx_ptr controller_ptr)
    throw (CORBA::SystemException)
{
    if (!this->isValid(controller_ptr)) return;

    Controller_impl* controller = dynamic_cast<Controller_impl*>(controller_ptr);

    controller->resume();
    // std::cout << ">> Controller Status == RUNNING" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// End Of File
////////////////////////////////////////////////////////////////////////////////
