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

#include <Time/clockservice/Executor_impl.hpp>
#include <Time/clockservice/config.hpp>
#include <Time/clockservice/Controller_impl.hpp>

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
    : reactor_(reactor)
{
}

///////////////////////////////////////////////////////////////////////////////

Executor_impl::~Executor_impl ()
{
}

///////////////////////////////////////////////////////////////////////////////

::CosClockService::PeriodicExecution::Controller_ptr
Executor_impl::enable_periodic_execution
(CosClockService::PeriodicExecution::Periodic_ptr on)
    throw (CORBA::SystemException)
{
    CDMW_ASSERT(!CORBA::is_nil(on));
    Cdmw::clock::Controller_impl* ctrl = 
        new Cdmw::clock::Controller_impl(*this, 
                                         on, 
                                         reactor_);
    controllerList_.push_back(ctrl);
    return ctrl->_this();
}

///////////////////////////////////////////////////////////////////////////////
