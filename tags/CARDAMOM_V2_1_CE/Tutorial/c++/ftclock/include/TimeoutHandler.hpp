/* ========================================================================= *
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

#ifndef _DEMO_TIMER_TIMEOUT_HANDLER_H_
#define _DEMO_TIMER_TIMEOUT_HANDLER_H_

#include "CosClockService.skel.hpp"

namespace Cdmw {
    namespace clock {
        namespace valid {
            class TimeoutHandler;
        }
    }
}
   
/**
 * Simple class that implements the Periodic Interface and 
 * is able to get registered for timeouts.
 */ 
class Cdmw::clock::valid::TimeoutHandler : 
    public POA_CosClockService::PeriodicExecution::Periodic
{
public:
    TimeoutHandler(CosClockService::PeriodicExecution::Executor_ptr executor);
    virtual ~TimeoutHandler();

public:
    /**
     * Method invoked by the clock service once the interval
     * that had been scheduled for this obeject elapses.
     */
    CORBA::Boolean do_work (const CORBA::Any& params)
        throw (CORBA::SystemException);
    
private:
    CosClockService::PeriodicExecution::Executor_ptr executor_;
    CosClockService::PeriodicExecution::Controller_ptr ctrl_;
    CORBA::ORB_ptr orb_;
    size_t count_;
    size_t executionLimit_;
};

#endif /* _DEMO_TIMER_TIMEOUT_HANDLER_H_ */
