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

#ifndef CARDAMOM_CLOCK_EXECUTOR_IMPL_HPP
#define CARDAMOM_CLOCK_EXECUTOR_IMPL_HPP

/**
 * @file
 * 
 * @author Fabrizio Morciano <fmorciano@amsjv.it>
 */

#include <Time/clockservice/CdmwTime.skel.hpp>


// -- EVoT _impl Includes --
#ifndef WIN32
#   if CDMW_ORB_VDR == tao
#   include <orbsvcs/Property/CosPropertyService_i.h>
#   elif CDMW_ORB_VDR==orbacus
#   include <Foundation/idllib/CosProperty.stub.hpp>
#   endif
#endif

#include <Time/clockservice/config.hpp>
#include <Time/clockservice/Controller_impl.hpp>
#include <Time/clockservice/ControlledClock_impl.hpp>


// -- STD C++ Includes --
#include <vector>


namespace Cdmw  { namespace clock {

/**
  * @brief Allows registration of an object reference with a clock capable of 
  * performing periodic execution.
  *
  * @author Fabrizio Morciano <fmorciano@amsjv.it>
  */
class CLOCK_API Executor_impl: 
        public virtual POA_CosClockService::PeriodicExecution::Executor,
        public virtual Cdmw::clock::Clock_impl
{
public:
    Executor_impl(ACE_Reactor* reactor = ACE_Reactor::instance());
    virtual ~Executor_impl ();
    
    // -- Methods from the Executor Interface --
    
    /**
     * Register an instance of the Periodic interface for periodic execution.
     * 
     * @return the <code>Controller</code> to be used to control 
     *         the execution of the <code>Periodic</code> instance.
     */
    virtual ::CosClockService::PeriodicExecution::Controller_ptr
    enable_periodic_execution(CosClockService::PeriodicExecution::Periodic_ptr on)
        throw (CORBA::SystemException);    

public:
    typedef std::vector<Cdmw::clock::Controller_impl*> ControllerList_t;

private:
    Executor_impl(const Executor_impl&);
    Executor_impl& operator=(const Executor_impl&);
    
protected:
    ACE_Reactor* reactor_;
    ControllerList_t controllerList_;
};

    
} /* clock */ } /* Cdmw */

#endif /* CARDAMOM_CLOCK_EXECUTOR_IMPL_HPP */
