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

#ifndef CARDAMOM_CLOCK_EXECUTOR_IMPL_HPP
#define CARDAMOM_CLOCK_EXECUTOR_IMPL_HPP

/**
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

#include "Foundation/orbsupport/RefCountServantBase.hpp"

#include <Time/clockservice/CdmwTime.skel.hpp>

// -- EVoT _impl Includes --
#ifndef WIN32
#   if CDMW_ORB_VDR == tao
#   include <orbsvcs/Property/CosPropertyService_i.h>
#   elif CDMW_ORB_VDR==orbacus
#   include <Foundation/idllib/CosPropertyService.stub.hpp>
#   endif
#endif

#include <Time/clockservice/config.hpp>
#include "clockservice/Controller_impl.hpp"
#include "clockservice/ControlledClock_impl.hpp"
#include "clockservice/ControllerUpdateAdmin_impl.hpp"
#include "clockservice/ControllerUpdate_impl.hpp"

// -- STD C++ Includes --
#include <vector>


namespace Cdmw  { namespace clock {

/**
  * @brief Allows registration of an object reference with a clock capable of 
  * performing periodic execution.
  *
  * @author Fabrizio Morciano <fmorciano@selex-si.com>
  */
class CLOCK_API Executor_impl
:   public virtual POA_CosClockService::PeriodicExecution::Executor
,   public virtual Cdmw::clock::Clock_impl
,   public virtual Cdmw::OrbSupport::RefCountServantBase
{
public:
    Executor_impl(ACE_Reactor* reactor);
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
    virtual void detach_terminated_controller(Cdmw::clock::Controller_impl* ctrl);

protected:
    typedef void (Cdmw::clock::policy::ControllerUpdate::* ControllerUpdateMemberFunction_ptr)(Cdmw::clock::ControllerEx_ptr);

    class PolicyFunctorManager : public std::unary_function<Cdmw::clock::Controller_impl*, bool>
    {
        public:
            PolicyFunctorManager(Cdmw::clock::ControlledExecutor_ptr executor,
                                 ControllerUpdateMemberFunction_ptr  function);
            bool operator()(Cdmw::clock::Controller_impl* ctrl);

        private:
            Cdmw::clock::ControlledExecutor_ptr  m_executor;
            ControllerUpdateMemberFunction_ptr   m_function;
    };

protected:
    // -- Methods for controller list management (all methods are lock free)
    void   apply_policy_to_controllers_lock_free(const PolicyFunctorManager& pfm);
    void   attach_controller_lock_free(Cdmw::clock::Controller_impl* ctrl);
    void   detach_controller_lock_free(Cdmw::clock::Controller_impl* ctrl);
    void   detach_all_controllers_lock_free();
    size_t num_of_attached_controllers_lock_free() const;
    void   print_attached_controllers_lock_free() const;

    // -- Methods for reactor management (all methods are lock free)
    ACE_Reactor* get_reactor_lock_free() const;

private:
    Executor_impl(const Executor_impl&);
    Executor_impl& operator=(const Executor_impl&);
    
private:
    typedef std::vector<Cdmw::clock::Controller_impl*> ControllerList_t;
    ControllerList_t m_controller_list;
    ACE_Reactor*     m_reactor;
};

    
} /* clock */ } /* Cdmw */

#endif /* CARDAMOM_CLOCK_EXECUTOR_IMPL_HPP */
