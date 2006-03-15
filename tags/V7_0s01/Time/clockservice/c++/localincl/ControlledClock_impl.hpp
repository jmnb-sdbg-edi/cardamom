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

#ifndef CARDAMOM_CLOCK_CONTROLLED_CLOCK_IMPL_HPP
#define CARDAMOM_CLOCK_CONTROLLED_CLOCK_IMPL_HPP

#include <cmath>
#include <limits>

// -- STD C++ Includes --
#include <iostream>

// -- STL C++ Includes --
#include <vector>
#include <map>

#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <Foundation/osthreads/Guard.hpp>
#include <Foundation/common/Assert.hpp>

#ifndef WIN32

#if CDMW_ORB_VDR == tao
#   include <orbsvcs/Property/CosPropertyService_i.h>
#   elif CDMW_ORB_VDR==orbacus
#   include <Foundation/idllib/CosPropertyService.stub.hpp>
#   endif
#endif

// -- Clock Service Includes --
#include <Time/clockservice/CdmwTime.skel.hpp>
#include <Time/clockservice/config.hpp>
#include <Time/clockservice/Macro.hpp>
#include <Time/clockservice/Util.hpp>


#include "clockservice/Clock_impl.hpp"
#include "clockservice/Controller_impl.hpp"

namespace Cdmw { namespace clock {

    /**
     * @brief some typedefs for the mostly used types
 */
    typedef struct
            {
                mutable Cdmw::clock::ClockObserver_ptr    observer;
                mutable Cdmw::clock::ControlledClockEvent event;
                mutable CORBA::Any                        argument;
                mutable CORBA::Boolean                    erasable;
            } ClockObserversData_t;

    typedef std::multimap<CORBA::ULong, ClockObserversData_t>           ClockObserversMMap_t;
    typedef std::multimap<CORBA::ULong, ClockObserversData_t>::iterator ClockObserversMMapIterator_t;
 
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
 * @author Angelo Corsaro <acorsaro@selex-si.com>
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 * @author Raffaele Mele <rmele@progesi.it>
 */
    template <typename LOCK,
          template <typename T> class GUARD = ::Cdmw::OsSupport::Guard>    
    class CLOCK_API ControlledClock_impl 
    :   public virtual POA_CosClockService::ControlledClock
    ,   public virtual CosClockService::ControlledClock
    ,   public virtual POA_Cdmw::clock::ControlledClockEx
    ,   public virtual Cdmw::clock::ControlledClockEx
    ,   public virtual Cdmw::clock::IClock
    ,   public virtual Cdmw::clock::Clock_impl
    ,   public virtual Cdmw::OrbSupport::RefCountServantBase
    {
    public:

    typedef LOCK        Lock_t;
    typedef GUARD<LOCK> Guard_t;
    
    public:
    ControlledClock_impl();
        virtual ~ControlledClock_impl() 
            throw();

    // -- Methods from the Clock Interface --

    /**
     * Returns information on the properties of the available clocks.
     */
    virtual ::CosPropertyService::PropertySet_ptr properties()
        throw (CORBA::SystemException);

    /**
     * Provides a measure of the current time. The time unit is 100
     * nanosecond i.e. 10e-7 seconds .
     */
    virtual ::TimeBase::TimeT current_time()
        throw (CORBA::SystemException,
               CosClockService::TimeUnavailable);


    // -- Methods from ControlledClock Interface --

    /**
     * Set the time for this clock.
         * All subscribed clock observers will be notified.
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
         * All subscribed clock observers will be notified.
     *
     * @param ratio the ratio for this clock.
     */
    virtual void set_rate (CORBA::Float ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Extension to retrieve rate.
         * @sa get_time_origin
     */
    virtual CORBA::Float get_rate ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Extension to retrieve base of time.
     * Return the origin of time for controlled clock,
     * can be used to convert virtual time to real time
     * (i.e.  current_time = virtual_time - get_time_origin / ratio )
     *
         * @param realTime The time when a set method was called.
         * @param virtualTime The <code>realTime<\code> applying clock characteristic.
     */
     virtual   void get_time_origins (
      ::TimeBase::TimeT_out real_time,
      ::TimeBase::TimeT_out virtual_time) 
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);
    /**
     * Pause the clock, i.e., the clock stops advancing.
         * All subscribed clock observers will be notified.
     *
     * @exception CORBA::BAD_INV_ORDER, if the clock is already paused.
     */
    virtual void pause ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Resume the clock, i.e., the clock restarts measuring the time
     * that elapses.
         * All subscribed clock observers will be notified.
     *
     * @exception CORBA::BAD_INV_ORDER, if the clock is not paused.
     */
    virtual void resume ()
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported);

    /**
     * Stop the clock.
         * All subscribed clock observers will be notified.
     */
    virtual void terminate ()
        throw (CORBA::SystemException ,
               CosClockService::ControlledClock::NotSupported);

        /**
         * Add <code>observer<\code> in the list of object that need to have a notify.
         * @param 'observer' The reference to <code>ClockObserver<\code> to remove.
         * @param 'id' The event mask.
         * @param 'params' a CORBA::Any object to be passed as an argument to the clock observer's associated handler.
         * @note It is possible to use different <code>params<\code> for different subscribed observers.
         *       If <code>params<\code> is provided for ALL the last one overwrites the previous.
         * @exception Cdmw::clock::ControlledClockEx::AlreadyPresentEntry
         *            The exception is thrown when:
         *            1. 'id' is Cdmw::clock::NONE
         *            2. 'observer' has already subscribed and its associated event mask is equal to 'id'.
         * @exception CosClockService::ControlledClock::NotSupported
         *            The exception is thrown when:
         *            1. 'observer' has already subscribed and its associated event mask is not properly included in 'id'.
         */
        virtual void attach (
          ::Cdmw::clock::ControlledClockEvent id,
          ::Cdmw::clock::ClockObserver_ptr observer,
          const ::CORBA::Any & params)
            throw (CORBA::SystemException,
                  Cdmw::clock::ControlledClockEx::AlreadyPresentEntry,
                  CosClockService::ControlledClock::NotSupported);
    
        /**
         * Remove a subscribed clock observer from list of object that need to have a notify.
         * @param 'observer' The refernce to the clock observer to remove.
         * @param 'id' The event mask.
         * @exception CosClockService::ControlledClock::NotSupported
         *            The exception is thrown when:
         *            1. 'observer' has not subscribed yet.
         *            2. 'id' is Cdmw::clock::NONE.
         *            3. 'observer' has already subscribed and its associated event mask does not properly include 'id'.
         * @note The special case 'id' equal to Cdmw::clock::ALL is always allowed and systematically
         *       leads to the deletion of 'observer' from the list of subscribed clock observers.
         */
        virtual void detach (
            ::Cdmw::clock::ControlledClockEvent id,
            ::Cdmw::clock::ClockObserver_ptr observer )
            throw(CORBA::SystemException,
                  CosClockService::ControlledClock::NotSupported);
                  
    virtual double  retrieve_rate();

        /**
         * Retrieve value of ControlledClock origins
         * before last set
         */
        void get_prev_origins(::TimeBase::TimeT& X0, 
                              ::TimeBase::TimeT& Y0) 
            throw()
        {
            X0 = m_prev_X0;
            Y0 = m_prev_Y0;
        }
    
    protected:
    // -- Disable Copy Ctor and assignment operator --
    ControlledClock_impl(const ControlledClock_impl&);
    ControlledClock_impl& operator=(const ControlledClock_impl&);

    ::TimeBase::TimeT
    adjusted_virtual_time(::TimeBase::TimeT time) const;

    inline void
    update_time_origin(::TimeBase::TimeT realNow,
                       ::TimeBase::TimeT virtualNow)
    {
        m_X0 = realNow;
        m_Y0 = virtualNow;
    }

    protected:

    inline CORBA::Float& ratio() 
        throw()
    {
        return m_rate;
    }

    inline ControlledClockState_t& status() 
        throw()
    {
        return m_status;
    }


    inline ::TimeBase::TimeT& X0() 
        throw()
    {
        return m_X0;
    }

    inline ::TimeBase::TimeT& Y0() 
        throw()
    {
        return m_Y0;
    }

    inline ::TimeBase::TimeT& frozen_time() 
        throw()
    {
        return m_frozenTime;
    }

        /**
         * Save origins of ControlledClock before
         * to change them in set method.
         */
        void save_prev_origins()
            throw()
        {
            m_prev_X0 = m_X0;
            m_prev_Y0 = m_Y0;
        }
                    
    void set_time_origins(::TimeBase::TimeT X0, 
                          ::TimeBase::TimeT Y0) 
        throw();
        
    void set_frozen_time(::TimeBase::TimeT time) 
        throw();

    protected:
        ::TimeBase::TimeT       
            m_X0,
            m_Y0,
            m_prev_X0,
            m_prev_Y0,
            m_frozenTime;
    
        Lock_t                  
            m_mutex;

    private:

        /**
         * @brief this function finds in the ClockObserversMMap the entry associated
         *        to a specified observer with a specified hash value.
         * @param 'observer'   is a reference to a Cdmw::clock::ClockObserver
         * @param 'hash_value' is the hash value associated to 'observer'
         *        Note: it must be 'hash_value' = 'observer'->_hash('M'), with
                        'M' = std::numeric_limits<CORBA::ULong>::max().
         * @return if a reference to the same clock observer object referred by 'observer'
         *         is already stored in the ClockObserversMMap, the function returns an
         *         iterator pointing to it, otherwise an iterator to the end of the
         *         ClockObserversMMap is returned.
         */
        ClockObserversMMapIterator_t findClockObserverInMMap(Cdmw::clock::ClockObserver_ptr observer, const CORBA::ULong hash_value);

        /**
         * @brief this function traverse the ClockObserversMMap and calls the handler associated to
         *        the specified Cdmw::clock::ControlledClockEvent value on all the stored observers
         *        that have attached subscribing for that specified event. If some stored references
         *        refer to a not yet existing object, they will be released and the associated element
         *        in the ClockObserversMMap erased.
         * @param 'event' is the event that needs to be notified.
         *        Note: 'event' must have one and only one flag set.
         * @return if a reference to s functions returns void.
         */
        void notifyEventToObserversInMMap(const Cdmw::clock::ControlledClockEvent event);

        /**
         * @brief this function compares two Cdmw::clock::ControlledClockEvent values.
         * @param 'evt1' 1-st Cdmw::clock::ControlledClockEvent value to compare
         * @param 'evt2' 2-nd Cdmw::clock::ControlledClockEvent value to compare
         * @return true if and only if 'evt1' < 'evt2':
         *         i.e. all active flags in 'evt1' are also active in 'evt2', but
         *              NOT all active flags in 'evt2' are also active in 'evt1'.
         */
        bool less(const Cdmw::clock::ControlledClockEvent evt1, const Cdmw::clock::ControlledClockEvent evt2);

        /**
         * @brief this function compares two Cdmw::clock::ControlledClockEvent values.
         * @param 'evt1' 1-st Cdmw::clock::ControlledClockEvent value to compare
         * @param 'evt2' 2-nd Cdmw::clock::ControlledClockEvent value to compare
         * @return true if and only if 'evt1' == 'evt2':
         *         i.e. all active flags in 'evt1' are also active in 'evt2', and
         *              all active flags in 'evt2' are also active in 'evt1'.
         */
        bool eq(const Cdmw::clock::ControlledClockEvent evt1, const Cdmw::clock::ControlledClockEvent evt2);

        /**
         * @brief Map containing the association clock <-> clock observers.
         */
        ClockObserversMMap_t clockObservers;
    };
} /*clock*/ } /* Cdmw */

#include "clockservice/ControlledClock_impl_timpl.hpp"

#endif /* CARDAMOM_CLOCK_CONTROLLED_CLOCK_IMPL_HPP*/
