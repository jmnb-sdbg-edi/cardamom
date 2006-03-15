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

#ifndef CARDAMOM_CLOCK_CONTROLLED_EXECUTOR_CLOCK_IMPL_HPP
#define CARDAMOM_CLOCK_CONTROLLED_EXECUTOR_CLOCK_IMPL_HPP

#include <Foundation/common/Config.hpp>
#include <Foundation/common/Void.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/osthreads/Guard.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>

// -- EVoT _impl Includes --
#ifndef WIN32
#if CDMW_ORB_VDR == tao
#   include <orbsvcs/Property/CosPropertyService_i.h>
#   elif CDMW_ORB_VDR==orbacus
#   include <Foundation/idllib/CosPropertyService.stub.hpp>
#   endif
#endif

#include <Time/clockservice/CdmwTime.skel.hpp>
#include <Time/clockservice/config.hpp>
#include <Time/clockservice/Util.hpp>

#include "clockservice/Controller_impl.hpp"
#include "clockservice/Executor_impl.hpp"
#include "clockservice/ControlledClock_impl.hpp"
#include "clockservice/ControlledExecutor_impl.hpp"

// -- STD C++ Includes --
#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>


namespace Cdmw  { namespace clock {

/**
 * @brief This class extends Executor controller to manage controlled periodic timer.
 * 
 * @author Francesca Annunziata <fannunziata@selex-si.com>
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */
    CDMW_TEMPLATE_EXPORT
    template <typename LOCK,
              template <typename T> class GUARD = ::Cdmw::OsSupport::Guard>
    class  CLOCK_API  ControlledExecutor_impl \
        :   public virtual Cdmw::clock::ControlledClock_impl< ::Cdmw::Common::Void >
        ,   public virtual Cdmw::clock::Executor_impl
        ,   public virtual POA_Cdmw::clock::ControlledExecutor
        ,   public virtual Cdmw::clock::ControlledExecutor
        ,   public virtual Cdmw::OrbSupport::RefCountServantBase
    {
    public:

        typedef LOCK        Lock_t; 
        typedef GUARD<LOCK> Guard_t;

        ControlledExecutor_impl(ACE_Reactor* reactor);
    
        virtual ~ControlledExecutor_impl () throw();

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


        // -- Methods from ControlledClock Interface --

        /**
         * Set the time for this clock. Extend functionality resetting controller.
         *
         * @param to the time to which the clock will be set.
         */
        virtual void set(TimeBase::TimeT to)
            throw (CORBA::SystemException,
                   CosClockService::ControlledClock::NotSupported);
        
        /**
         * Set the rate for this clock. Extend functionality resetting controller.
         *
         * @param ration the rate of clock will be set.
         */
        virtual void set_rate (CORBA::Float ratio)
            throw (CORBA::SystemException,
                   CosClockService::ControlledClock::NotSupported);

        /**
         * Pause the clock, i.e., the clock stops advancing.
         *
         * @exception CORBA::BAD_INV_ORDER, if the clock is already paused.
         */
        virtual void pause ()
            throw (CORBA::SystemException,
                   CosClockService::ControlledClock::NotSupported);

        /**
         * Resume the clock, i.e., the clock restarts measuring the time
         * that elapses.
         *
         * @exception CORBA::BAD_INV_ORDER, if the clock is not paused.
         */ 
        virtual void resume ()
            throw (CORBA::SystemException,
                   CosClockService::ControlledClock::NotSupported);

        /**
         * Stop the clock, also remove all Controller maintained from
         * this clock.
         */
        virtual void terminate ()
            throw (CORBA::SystemException ,
                   CosClockService::ControlledClock::NotSupported);
    
        /**
         * Return a Controller that uses specifyed policy.
         */
        virtual ::CosClockService::PeriodicExecution::Controller_ptr 
        enable_periodic_execution_with_policy (
            ::CosClockService::PeriodicExecution::Periodic_ptr on,
            ::Cdmw::clock::policy::ControllerUpdateID id)
            throw (CORBA::SystemException,
                   ::Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable);
    
        /**
         * Change the policy that it is used to manage Controller.
         */
        virtual
        void set_controller_update_policy (::Cdmw::clock::policy::ControllerUpdateID id)
            throw (CORBA::SystemException,
                   ::Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable);
    
        /**
         * Return the policy actually used.
         */
        virtual
        Cdmw::clock::policy::ControllerUpdateID get_controller_update_policy ( )
            throw (CORBA::SystemException);

        /**
         * Provides a measure of the current time. The time unit is 100
         * nanosecond i.e. 10e-7 seconds .
         */
        virtual ::TimeBase::TimeT current_time()
            throw (CORBA::SystemException,
                   CosClockService::TimeUnavailable);

    public:
        virtual void detach_terminated_controller(Cdmw::clock::Controller_impl* ctrl);

    protected:
        void set_time_origins(::TimeBase::TimeT X0, ::TimeBase::TimeT Y0) throw();

    private:
        ControlledExecutor_impl(const ControlledExecutor_impl&);
        ControlledExecutor_impl& operator=(const ControlledExecutor_impl&);

    private:
        Lock_t                                  m_mutex;
        Cdmw::clock::policy::ControllerUpdateID m_ID;    
    };

    
} /* clock */ } /* Cdmw */

#if (CDMW_USES_TEMPLATE_INCLUSION_MODEL == 1)
#include "clockservice/ControlledExecutor_impl_timpl.hpp"
#endif /* CDMW_USES_TEMPLATE_INCLUSION_MODEL == 1 */

#endif /* CARDAMOM_CLOCK_CONTROLLED_EXECUTOR_CLOCK_IMPL_HPP */

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
