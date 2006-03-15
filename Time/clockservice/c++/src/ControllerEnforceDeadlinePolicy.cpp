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

#include <cstring>
#include "clockservice/ControllerEnforceDeadlinePolicy.hpp"
#include "clockservice/Controller_impl.hpp"
#include "clockservice/ControlledExecutor_impl.hpp"
#include "clockservice/FederatedControlledExecutor_impl.hpp"
#include "Time/clockservice/Util.hpp"
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Guard_T.h>

using namespace Cdmw::clock::util;

////////////////////////////////////////////////////////////////////////////////

namespace 
{
}

////////////////////////////////////////////////////////////////////////////////

Cdmw::clock::ControllerEnforceDeadlinePolicy::ControllerEnforceDeadlinePolicy ()
{
}

////////////////////////////////////////////////////////////////////////////////

Cdmw::clock::ControllerEnforceDeadlinePolicy::~ControllerEnforceDeadlinePolicy ()
{
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @fn void Cdmw::clock::ControllerEnforceDeadlinePolicy::on_set (::Cdmw::clock::ControllerEx_ptr controller)
 * 
 * \f$(x_0,y_0)\f$: is the origin of controlled clock.
 * \f$(x_s,y_s)\f$: when a set operation was performed.
 * \f$(x_n,y_n)\f$: net time base.
 * In this case \f$ x_n = x_s \f$ are the same values.
 * \f$(x_c,y_c)\f$: start of controlled clock.
 * \f$(x_a,y_a)\f$: deadline, before set.
 * \f$(x_{a1},y_{a1})\f$: new deadline, before set.
 * In this case \f$ y_a = y_{a1} \f$ are the same values.
 * 
 * Time equation is:
 * \f$ y_s - y_0 = \alpha(x_s-x_0) \f$.
 * then we can write the system:
 * 
 * \f{eqnarray*}
 * \cases{ 
 * y_a-y_s = \alpha(x_a - x_s) \cr
 * y_{a1}-y_n = \alpha(x_{a1} - x_n) \cr
 * }
 * \f}
 * 
 * But,
 * \f{eqnarray*}
 * \cases{ 
 * y_{a1} = y_a \cr
 * x_n = x_s) \cr
 * }
 * \f}
 * 
 * Then we can write:
 * 
 * \f{eqnarray*}
 * \cases{ 
 * y_a-y_s = \alpha(x_a - x_s) \cr
 * y_a-y_n = \alpha(x_{a1} - x_n) \cr
 * }
 * \f}
 * 
 * and solving:
 * 
 * \f$ x_{a1} = x_a - \frac{y_n-y_s}{\alpha} \f$.
 * where  \f$ x_{a1} \f$ is the new time of deadline in realtime.
 */
void 
Cdmw::clock::ControllerEnforceDeadlinePolicy::on_set (
    ::Cdmw::clock::ControllerEx_ptr controller)
  throw (CORBA::SystemException)
{
     if(!this->isValid(controller))
        return;
    
    Controller_impl* controller_ptr = 
             dynamic_cast<Controller_impl*>(controller);
    CDMW_ASSERT(controller_ptr);

    Cdmw::clock::Executor_impl* iclock_ptr = controller_ptr->retrieve_executor();
    ControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>*          ref2ctrlexec    = 0;
    FederatedControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>* ref2fedctrlexec = 0;
    if (typeid(*iclock_ptr) == typeid(ControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>))
        ref2ctrlexec = dynamic_cast<ControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>*>(iclock_ptr);
    if (typeid(*iclock_ptr) == typeid(FederatedControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>))
        ref2fedctrlexec = dynamic_cast<FederatedControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>*>(iclock_ptr);
    CDMW_ASSERT(ref2ctrlexec || ref2fedctrlexec);

    CORBA::Any     param      = controller_ptr->get_parameter();
    CORBA::ULong   execs      = controller_ptr->execution_limit();
    
    // ref2controlledclock->get_time_origins(x0,y0);
    /* *
     * check controlled status: 
     * if is PAUSED then NOTHING TO DO
     * re-scheduling of timers will be done by resume of
     * controlled clock
     * */
    
    if (controller_ptr->get_status() != Cdmw::clock::PAUSED)  
    {
        if(controller_ptr->executions() ==
           controller_ptr->execution_limit() )
            return; // timer has completed the executions

 	    controller->pause();  

        if(execs!=0)
           execs -= controller_ptr->executions();
 
    	TimeBase::TimeT period = controller_ptr->get_period();
    	double 
            rate = controller_ptr->retrieve_executor()->retrieve_rate();
    
        // start of controller executions
        TimeBase::TimeT 
            xc  = controller_ptr->get_controller_time_origin();
    
        // a.k.a.: ya ( appointment ) 
        // when the timer will be expired in absolute virtual time
    	TimeBase::TimeT 
            expiration_time = xc  +
                             (TimeBase::TimeT)(period*rate)*  
                             (controller_ptr->executions()+1),
            &ya = expiration_time;  // alias

        // time origins before last set
        TimeBase::TimeT
            x0,y0;

        if (ref2ctrlexec)    ref2ctrlexec->get_prev_origins(x0, y0);
        if (ref2fedctrlexec) ref2fedctrlexec->get_prev_origins(x0, y0);

        // new time origins
        TimeBase::TimeT
            xn,yn;

        if (ref2ctrlexec)    ref2ctrlexec->get_time_origins(xn, yn);
        if (ref2fedctrlexec) ref2fedctrlexec->get_time_origins(xn, yn);
  
        TimeBase::TimeT
            xs,ys;
        xs = xn;
        ys = (TimeBase::TimeT)((xs - x0)*rate)+y0;
        
        // xa 
        TimeBase::TimeT
            xa = x0 + (TimeBase::TimeT)((ya - y0)/rate);
     
        // xnew appointement        
        TimeBase::TimeT
            programmed_absolute_virtual_time = 
                xa - (TimeBase::TimeT)((yn - ys)/rate);

    	try 
        {
    		controller_ptr->resume_at(programmed_absolute_virtual_time);     
    	} 
        catch ( CosClockService::PeriodicExecution::Controller::TimePast& e) 
    	{
    		std::cerr << e << std::endl;	
    		throw;
        } catch ( CORBA::SystemException& ex)
        {
    		std::cerr << ex << std::endl;	
    	}
    }
}

////////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ControllerEnforceDeadlinePolicy::on_set_rate (
    ::Cdmw::clock::ControllerEx_ptr controller)
  throw (CORBA::SystemException)
{
     if(!this->isValid(controller))
        return;

    Controller_impl* controller_ptr = 
             dynamic_cast<Controller_impl*>(controller);
    CDMW_ASSERT(controller_ptr);
    if(!controller_ptr)
        return;
    
    CORBA::Any                  param         = controller_ptr->get_parameter();
    TimeBase::TimeT             timer_origin  = controller_ptr->get_controller_time_origin();
    CORBA::ULong                execs         = controller_ptr->execution_limit();
    Cdmw::clock::Executor_impl* clock_ptr     = controller_ptr->retrieve_executor();
    CDMW_ASSERT(clock_ptr);

    ControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>*             ref2ctrlexec = 0;
    FederatedControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>* ref2fedctrlexec = 0;
    if (typeid(*clock_ptr) == typeid(ControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>))
        ref2ctrlexec = dynamic_cast<ControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>*>(clock_ptr);
    if (typeid(*clock_ptr) == typeid(FederatedControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>))
        ref2fedctrlexec = dynamic_cast<FederatedControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>*>(clock_ptr);
    CDMW_ASSERT(ref2ctrlexec || ref2fedctrlexec);

    if (!ref2ctrlexec && !ref2fedctrlexec)
        return;

    TimeBase::TimeT   
        x0 = 0,
        y0 = 0;

    if (ref2ctrlexec)    ref2ctrlexec->get_time_origins(x0, y0);
    if (ref2fedctrlexec) ref2fedctrlexec->get_time_origins(x0, y0);

    y0=0; // just to avoid warning

    double new_rate = 0;
    if (ref2ctrlexec)    new_rate = ref2ctrlexec->get_rate();
    if (ref2fedctrlexec) new_rate = ref2fedctrlexec->get_rate();

    // can not be resumed a controllet that is paused
    if (controller_ptr->get_status() != Cdmw::clock::PAUSED)  
    {
    	controller->pause();
  
	    if(execs!=0)
	        execs -= controller_ptr->executions();
	    if(controller_ptr->executions() ==
	       controller_ptr->execution_limit() )
	        return; // timer has completed the executions
	 
	    TimeBase::TimeT period = controller_ptr->get_period();
	    double previous_rate = clock_ptr->retrieve_rate();
	  
	    TimeBase::TimeT expiration_time = 
	             (timer_origin 
	             +  (static_cast<TimeBase::TimeT>
	                (period*previous_rate))  
	             *  (controller_ptr->executions()+1)
	              );
	
	 //  std::cerr << y0 << std::endl;
	   
	   TimeBase::TimeT 
        resume_at_time = 
        (TimeBase::TimeT)(previous_rate/new_rate) * (expiration_time-x0) + x0;
	   try 
       {  
	      controller_ptr->resume_at(resume_at_time);
	   } 
       catch ( CosClockService::PeriodicExecution::Controller::TimePast& e)
       {
	      std::cerr << e << std::endl;	
	      throw;
	   } 
       catch ( CORBA::SystemException& ex)
       {
	      std::cerr << ex << std::endl;	
          throw;
	   }
    }	   
}

////////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ControllerEnforceDeadlinePolicy::on_pause (
    ::Cdmw::clock::ControllerEx_ptr controller)
  throw (CORBA::SystemException)
{
     if(!this->isValid(controller))
        return;
    controller->pause();
}

////////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ControllerEnforceDeadlinePolicy::on_terminate (
    ::Cdmw::clock::ControllerEx_ptr controller)
  throw (CORBA::SystemException)
{
     if(!this->isValid(controller))
        return;
    controller->terminate();
}

////////////////////////////////////////////////////////////////////////////////

void 
Cdmw::clock::ControllerEnforceDeadlinePolicy::on_resume(
	::Cdmw::clock::ControllerEx_ptr controller)
  throw (CORBA::SystemException)
{
     if(!this->isValid(controller))
        return;
    controller->resume();    
}

////////////////////////////////////////////////////////////////////////////////
// End Of File
////////////////////////////////////////////////////////////////////////////////
