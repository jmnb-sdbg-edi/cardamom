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

#ifndef CARDAMOM_CLOCK_CONTROLLED_CLOCK_IMPL_HPP
#define CARDAMOM_CLOCK_CONTROLLED_CLOCK_IMPL_HPP

// -- Clock Service Includes --
#include <Time/clockservice/CdmwTime.skel.hpp>

#ifndef WIN32

#if CDMW_ORB_VDR == tao
#   include <orbsvcs/Property/CosPropertyService_i.h>
#   elif CDMW_ORB_VDR==orbacus
#   include <Foundation/idllib/CosProperty.stub.hpp>
#   endif
#endif

#include <Time/clockservice/config.hpp>
#include <Time/clockservice/Clock_impl.hpp>
#include <Time/clockservice/IClock.hpp>


// -- ACE Includes --
#include "ace/Synch.h"

// -- STD C++ Includes --
#include <iostream>

namespace Cdmw { namespace clock { 

/*
 * TODO: The locking should be strategized so to make it possible
 *       the use of null mutexes. This way we could reduce the
 *       number of mutex that we acquire during the normal operation
 *       of federated clocks. This should be implemented by V2.
 */
 
/**
 * @brief This class provides an implementation for the
 * <code>ControlledClock</code> interface defined by the EVoT
 * specification.
 *
 * This class provides an implementation for the
 * <code>ControlledClock</code> interface defined by the EVoT
 * specification. Controlled clock allows to set the rate at which the
 * time flows as well as the actutal time.
 *
 * @author Angelo Corsaro <acorsaro@amsjv.it>
 * @author Fabrizio Morciano <fmorciano@amsjv.it>
 * @author Raffaele Mele <lellomele@yahoo.com>
 */
class CLOCK_API ControlledClock_impl :
        public virtual POA_CosClockService::ControlledClock,
        public virtual POA_Cdmw::clock::ControlledClockEx,
        public virtual Cdmw::clock::IClock,
        public virtual Cdmw::clock::Clock_impl    
{
public:
    ControlledClock_impl();
    virtual ~ControlledClock_impl() throw();
    
public:

    // -- Methods from the Clock Interface --
    
    /**
     * Returns information on the properties of the available clocks.
     */
    virtual ::CosPropertyService::PropertySet_ptr properties()
        throw (CORBA::SystemException);

    /**
     * Provides a measure of the current time. The time unit is 100
     * nanosecond e.g. 10e-7 seconds .
     */
    virtual ::TimeBase::TimeT current_time()
        throw (CORBA::SystemException,
               CosClockService::TimeUnavailable);

    
    // -- Methods from ControlledClock Interface --

    /**
     * Set the time for this clock.
     *
     * @param to the time to which the clock will be set.
     */
    virtual void set(TimeBase::TimeT to)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);
    
    /**
     * Set the rate at which the clock measures time. The rate is
     * expressed as a ratio of the controlled clock speed relative to
     * the real-time clock speed.
     *
     * @param ratio the ratio for this clock. 
     */
    virtual void set_rate (CORBA::Float ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Extension to retrieve rate.
     *
     */
    virtual CORBA::Float get_rate ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);
    
    /**
     * Pause the clock, e.g., the clock stops advancing.
     *
     * @exception CORBA::BAD_INV_ORDER, if the clock is already paused.
     */
    virtual void pause ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Resume the clock, e.g., the clock restarts measuring the time
     * that elapses.
     *
     * @exception CORBA::BAD_INV_ORDER, if the clock is not paused.
     */ 
    virtual void resume ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Stop the clock.
     */
    virtual void terminate ()
        throw (CORBA::SystemException ,
               CosClockService::ControlledClock::NotSupported);


    virtual double  retrieve_rate();

    virtual ::TimeBase::TimeT virtual_to_real(::TimeBase::TimeT time_val);

private:
    ::TimeBase::TimeT
    adjusted_virtual_time(::TimeBase::TimeT time) const;
    
    inline void updateTimeOrigin(::TimeBase::TimeT realNow,
                                 ::TimeBase::TimeT virtualNow);

private:
    // -- Disable Copy Ctor and assignment operator --
    ControlledClock_impl(const ControlledClock_impl&);
    ControlledClock_impl& operator=(const ControlledClock_impl&);
    
private:
    typedef enum {
        RUNNING,
        PAUSED,
        STOPPED
    } ControlledClockState_t;
    
protected:
    inline CORBA::Float& ratio() throw() { return ratio_; }
    inline ControlledClockState_t& status() throw() { return status_; }
    inline ::TimeBase::TimeT& X0() throw() { return X0_; }
    inline ::TimeBase::TimeT& Y0() throw() { return Y0_; }
    inline ::TimeBase::TimeT& frozen_time() throw() { return frozenTime_; }
    void set_time_origins(::TimeBase::TimeT X0, ::TimeBase::TimeT Y0) throw();
    void set_frozen_time(::TimeBase::TimeT time) throw();
    
private:
    CORBA::Float            ratio_;
    ControlledClockState_t  status_;
    
    ::TimeBase::TimeT       X0_;
    ::TimeBase::TimeT       Y0_; 
    ::TimeBase::TimeT       frozenTime_;

    ACE_Mutex               mutex_;

public:
    typedef std::vector<Cdmw::clock::Controller_impl*> ControllerList_t;

private:
    TAO_PropertySetFactory               propset_factory_;
    CosPropertyService::PropertySet_var  property_;

};

#include "ControlledClock_implinl.hpp"

} /* clock */  } /* Cdmw */

#endif /* CARDAMOM_CLOCK_CONTROLLED_CLOCK_IMPL_HPP*/
