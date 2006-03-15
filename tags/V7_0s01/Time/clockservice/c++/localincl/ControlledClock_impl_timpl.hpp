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

#include <algorithm>

namespace {

    /**
     * @brief Function Object used with the for_each template function in <algorithm>
     */
    class ClockObserverHandlerTrigger
    {
        public:

            /**
             * @brief the Cdmw::clock::ControlledClockEvent value passed as an input to the constructor is
             *        intended to represent a single event, (i.e. it is an event mask with only one flag set)
             */
            explicit ClockObserverHandlerTrigger(const Cdmw::clock::ControlledClockEvent single_event)
                : event(single_event), num_of_erasables(0) {}

            /**
             * @brief this constant method allows for the retrieval of the number of erasable items,
             *        (i.e. those items associated to observers for which the method _non_existent
             *              has returned true, stating the the observer reference doesn't address
             *              any existing CORBA object anymore).
             * @return the number of erasable items detected so far.
             */
            CORBA::ULong get_num_of_erasables() const { return this->num_of_erasables; }

            /**
             * @brief if the current clock observer has attached with an event mask containing the single event passed as
             *        the constructor's input parameter, this operator triggers the corresponding clock observer handler,
             *        marking for erasure every observer reference addressing an unexistent object.
             */
            void operator()(const std::pair<CORBA::ULong, Cdmw::clock::ClockObserversData_t>& value)
            {
                if (value.second.event & this->event)
                {
                    try
                    {
                        switch (this->event)
                        {
                            case Cdmw::clock::SET_TIME:
                                value.second.observer->handle_set(value.second.argument);
                                break;

                            case Cdmw::clock::SET_RATE:
                                value.second.observer->handle_set_rate(value.second.argument);
                                break;

                            case Cdmw::clock::PAUSE:
                                value.second.observer->handle_pause(value.second.argument);
                                break;

                            case Cdmw::clock::RESUME:
                                value.second.observer->handle_resume(value.second.argument);
                                break;

                            case Cdmw::clock::TERMINATE:
                                value.second.observer->handle_terminate(value.second.argument);
                                break;
                        }
                    }
                    catch (const CORBA::Exception&)
                    {
                        try
                        {
                            if (value.second.observer->_non_existent())
                            {
                                value.second.erasable = 1;
                                this->num_of_erasables++;
                            }
                        }
                        catch (const CORBA::Exception&)
                        {
                            // skip processing of this observer reference
                        }
                    }
                }
            }

        private:

            Cdmw::clock::ControlledClockEvent event;
            CORBA::ULong num_of_erasables;
    };
}

template < typename LOCK, template < typename T > class GUARD>
Cdmw::clock::ControlledClock_impl < LOCK, GUARD >::ControlledClock_impl ()
:m_X0 (0)
,m_Y0 (0)
,m_frozenTime (0)
{
  m_rate = 1.0;
  Cdmw::clock::IClock::m_status = Cdmw::clock::RUNNING;
  property_ = propset_factory_->create_propertyset();
  property_->delete_all_properties();

  {
    // resolution
    CosPropertyService::PropertyName name = "Resolution";
    // in windows is possible to have millisecond resolution
    CORBA::Any value;
    value <<= (CORBA::ULong) (RESOLUTION);
    property_->define_property (name, value);
  }

  {
    // Precision
    CosPropertyService::PropertyName name = "Precision";
    CORBA::Any value;
    value<<= (CORBA::Short)24;
    property_->define_property( name, value );
  }

  {
    // Width
    CosPropertyService::PropertyName name = "Width";
    CORBA::Any value;
    value <<= (CORBA::Short)64;
    property_->define_property (name, value);
  }

  {
    // Stability_Description
    CosPropertyService::PropertyName name = "Stability_Description";
    CORBA::Any value;
    value <<= "Not Available";
    property_->define_property (name, value);
  }

  {
    // Coordination
    CosPropertyService::PropertyName name = "Coordination";
    CORBA::Any value;
    value <<= (CORBA::Short)0;
    property_->define_property (name, value);
  }

  {
    /*
       // Measurement
       CosPropertyService::PropertyName name = "Measurement";
       CORBA::Any value;
       value<<= 0;
       property_->define_property( name, value );
     */
  }

  {
    // TimeScale
    CosPropertyService::PropertyName name = "TimeScale";
    CORBA::Any value;
    value <<= (CORBA::Short)CosClockService::ClockProperty::Local_;
    property_->define_property (name, value);
  }
}

///////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD>
Cdmw::clock::ControlledClock_impl < LOCK, GUARD >::~ControlledClock_impl ()
throw ()
{
  // No-Op.
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
 void
 Cdmw::clock::ControlledClock_impl < LOCK,GUARD >::\
 set_time_origins (TimeBase::TimeT X0,
                   TimeBase::TimeT Y0)
throw ()
{
  Guard_t  guard (m_mutex);
  m_X0 = X0;
  m_Y0 = Y0;
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
void
Cdmw::clock::ControlledClock_impl < LOCK,GUARD >::\
set_frozen_time (TimeBase::TimeT time)
throw ()
{
  Guard_t guard (m_mutex);

  m_frozenTime = time;
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T >  class GUARD>
TimeBase::TimeT
Cdmw::clock::ControlledClock_impl <LOCK,GUARD>::\
adjusted_virtual_time (TimeBase::TimeT time) const
{
  TimeBase::TimeT
    deltaX = time - m_X0;
  float
    abs_rate = fabs (m_rate);
  double
    num = 0.0, den = 0.0;

  if (m_rate >= 0)
  {
    // rest of TimeBase::TimeT interval
    TimeBase::TimeT how_many_to_max = Cdmw::clock::macro::MAX_TIMET - m_Y0;
    num = util::convert (how_many_to_max);
  }
  else
      num = util::convert (m_Y0);

  den = util::convert (deltaX);

  // Overflow/Underflow: rate is too big
  if (num / den < abs_rate)
    throw CORBA::BAD_PARAM ();

  // common case has a rapid solution
  if (m_rate == 1.0)
    return m_Y0 + deltaX;

  TimeBase::TimeT intRatio =
    static_cast < TimeBase::TimeT > (abs_rate * DECIMAL_PRECISION);

  long long
    deltaY = ((deltaX / DECIMAL_PRECISION) * intRatio);

  TimeBase::TimeT out = m_Y0 + (m_rate > 0.0 ? deltaY : -deltaY);

  return out;
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
double
Cdmw::clock::ControlledClock_impl <  LOCK,GUARD >::retrieve_rate ()
{
  return this->get_rate ();
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
void
Cdmw::clock::ControlledClock_impl < LOCK,GUARD >::terminate ()
throw (CORBA::SystemException,
       CosClockService::ControlledClock::NotSupported)
{
  m_status = TERMINATED;

  // notify the event Cdmw::clock::TERMINATE to all clock observers attached on it.
  notifyEventToObserversInMMap(Cdmw::clock::TERMINATE);
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD>
CosPropertyService::PropertySet_ptr
Cdmw::clock::ControlledClock_impl < LOCK, GUARD >::properties ()
throw (CORBA::SystemException)
{
    CosPropertyService::PropertySet_var property_set(CosPropertyService::PropertySet::_duplicate(property_.in()));
    return property_set._retn();
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK,  template < typename T >  class GUARD>
TimeBase::TimeT
Cdmw::clock::ControlledClock_impl < LOCK, GUARD >::current_time ()
throw (CORBA::SystemException,
       CosClockService::TimeUnavailable)
{
  Guard_t guard (m_mutex);

  if (m_status == RUNNING)
  {
    TimeBase::TimeT tmp = Cdmw::clock::compute_current_time ();
    return this->adjusted_virtual_time (tmp);
  }
  return m_frozenTime;
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
void
Cdmw::clock::ControlledClock_impl < LOCK,GUARD >::set (TimeBase::TimeT virtualNow)
throw (CORBA::SystemException,
       CosClockService::ControlledClock::NotSupported)
{
  Guard_t guard (m_mutex);

    save_prev_origins();
    TimeBase::TimeT 
        realNow = Cdmw::clock::compute_current_time ();
  this->update_time_origin (realNow, virtualNow);

  m_frozenTime = virtualNow;

    // notify the event Cdmw::clock::SET_TIME to all clock observers attached on it.
    notifyEventToObserversInMMap(Cdmw::clock::SET_TIME);
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
void
Cdmw::clock::ControlledClock_impl < LOCK,GUARD >::set_rate (CORBA::Float ratio)
throw (CORBA::SystemException,
       CosClockService::ControlledClock::NotSupported)
{
  Guard_t guard (m_mutex);
    if (m_status != PAUSED) 
    {
     TimeBase::TimeT realNow = Cdmw::clock::compute_current_time ();
     TimeBase::TimeT virtualNow = this->adjusted_virtual_time (realNow);
     // update is done with old rate to have a correct
     //  virtual origin for time
     this->update_time_origin (realNow, virtualNow);
   }
  IClock::set_rate (ratio);

    // notify the event Cdmw::clock::SET_RATE to all clock observers attached on it.
    notifyEventToObserversInMMap(Cdmw::clock::SET_RATE);
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
CORBA::Float
Cdmw::clock::ControlledClock_impl < LOCK, GUARD >::get_rate ()
throw (CORBA::SystemException,
       CosClockService::ControlledClock::NotSupported)
{
  Guard_t guard (m_mutex);
  return m_rate;
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
void 
Cdmw::clock::ControlledClock_impl < LOCK, GUARD >::get_time_origins (
    ::TimeBase::TimeT_out real_time,
    ::TimeBase::TimeT_out virtual_time ) 
throw (CORBA::SystemException,
       CosClockService::ControlledClock::NotSupported)
{
	real_time    = m_X0;
	virtual_time = m_Y0;	
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
void
Cdmw::clock::ControlledClock_impl < LOCK,GUARD >::pause ()
throw (CORBA::SystemException,
       CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard (m_mutex);

    if (m_status != RUNNING)
        throw CORBA::BAD_INV_ORDER ();

    TimeBase::TimeT tmp = Cdmw::clock::compute_current_time ();
    m_frozenTime = this->adjusted_virtual_time (tmp);
    m_status = PAUSED;

    // notify the event Cdmw::clock::PAUSE to all clock observers attached on it.
    notifyEventToObserversInMMap(Cdmw::clock::PAUSE);
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
void
Cdmw::clock::ControlledClock_impl < LOCK, GUARD >::resume ()
throw (CORBA::SystemException,
       CosClockService::ControlledClock::NotSupported)
{
    if (m_status != PAUSED)
        throw CORBA::BAD_INV_ORDER ();

    m_status = RUNNING;
    TimeBase::TimeT realNow = Cdmw::clock::compute_current_time ();
    this->update_time_origin (realNow, m_frozenTime);
    m_frozenTime = 0;

    // notify the event Cdmw::clock::RESUME to all clock observers attached on it.
    notifyEventToObserversInMMap(Cdmw::clock::RESUME);
}

template < typename LOCK, template < typename T > class GUARD >
void Cdmw::clock::ControlledClock_impl < LOCK, GUARD >::attach (
      ::Cdmw::clock::ControlledClockEvent id,
      ::Cdmw::clock::ClockObserver_ptr observer,
      const ::CORBA::Any & params)
        throw (CORBA::SystemException,
              Cdmw::clock::ControlledClockEx::AlreadyPresentEntry,
              CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard (m_mutex);

    if (id == Cdmw::clock::NONE)
        throw Cdmw::clock::ControlledClockEx::AlreadyPresentEntry();

    // update event mask for event Cdmw::clock::ALL
    if (id == Cdmw::clock::ALL)
    {
        id = (Cdmw::clock::SET_TIME |
              Cdmw::clock::SET_RATE |
              Cdmw::clock::PAUSE    |
              Cdmw::clock::RESUME   |
              Cdmw::clock::TERMINATE);
    }

    CORBA::ULong hash_value = observer->_hash(std::numeric_limits<CORBA::ULong>::max());
    Cdmw::clock::ClockObserversMMapIterator_t it = findClockObserverInMMap(observer, hash_value);
    if (it != clockObservers.end())
    {
        if (eq(it->second.event, id))
            throw Cdmw::clock::ControlledClockEx::AlreadyPresentEntry();

        if (!less(it->second.event, id))
            throw CosClockService::ControlledClock::NotSupported();

        it->second.event    = id;
        it->second.argument = params;
    }
    else
    {
        Cdmw::clock::ClockObserversData_t data;
        data.observer = Cdmw::clock::ClockObserver::_duplicate(observer);
        data.event    = id;
        data.argument = params;
        data.erasable = 0;

        std::pair<CORBA::ULong, Cdmw::clock::ClockObserversData_t> value(hash_value, data);
        clockObservers.insert(value);
    }
}

////////////////////////////////////////////////////////////////////////////////
  
template < typename LOCK, template < typename T > class GUARD >
void Cdmw::clock::ControlledClock_impl < LOCK, GUARD >::detach (
        ::Cdmw::clock::ControlledClockEvent id,
        ::Cdmw::clock::ClockObserver_ptr observer )
        throw(CORBA::SystemException,
              CosClockService::ControlledClock::NotSupported)
{
    Guard_t guard (m_mutex);

    CORBA::ULong hash_value = observer->_hash(std::numeric_limits<CORBA::ULong>::max());
    Cdmw::clock::ClockObserversMMapIterator_t it = findClockObserverInMMap(observer, hash_value);

    if (it == clockObservers.end())
        throw CosClockService::ControlledClock::NotSupported();

    if (id == Cdmw::clock::NONE)
        throw CosClockService::ControlledClock::NotSupported();

    if (id == Cdmw::clock::ALL)
    {
        CORBA::release(it->second.observer);
        clockObservers.erase(it);
    }
    else
    {
        if (!(less(id, it->second.event) || eq(id, it->second.event)))
            throw CosClockService::ControlledClock::NotSupported();

        it->second.event = it->second.event & ~id;

        if (it->second.event == Cdmw::clock::NONE)
        {
            CORBA::release(it->second.observer);
            clockObservers.erase(it);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
Cdmw::clock::ClockObserversMMapIterator_t Cdmw::clock::ControlledClock_impl<LOCK, GUARD>::
findClockObserverInMMap(Cdmw::clock::ClockObserver_ptr observer, const CORBA::ULong hash_value)
{
    std::pair<Cdmw::clock::ClockObserversMMapIterator_t, Cdmw::clock::ClockObserversMMapIterator_t> p = clockObservers.equal_range(hash_value);
    for (Cdmw::clock::ClockObserversMMapIterator_t result = p.first; result != p.second; ++result)
        if (observer->_is_equivalent(result->second.observer))
            return result;
    return clockObservers.end();
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
void Cdmw::clock::ControlledClock_impl<LOCK, GUARD>::
notifyEventToObserversInMMap(const Cdmw::clock::ControlledClockEvent event)
{
    // Create the function object to use in the for_each algorithm
    ClockObserverHandlerTrigger trigger(event);

    // trigger the appropriate handler on all clock observers attached on the specified event
    for_each(clockObservers.begin(), clockObservers.end(), trigger);

    if (trigger.get_num_of_erasables() > 0)
    {
        // update clock observers map by erasing every erasable item
        Cdmw::clock::ClockObserversMMap_t aux_mmap;
        Cdmw::clock::ClockObserversMMapIterator_t it_beg = clockObservers.begin();
        Cdmw::clock::ClockObserversMMapIterator_t it_end = clockObservers.end();
        for (Cdmw::clock::ClockObserversMMapIterator_t it_cur = it_beg; it_cur != it_end; ++it_cur)
            if (it_cur->second.erasable == 1) CORBA::release(it_cur->second.observer);
            else aux_mmap.insert(*it_cur);

        // assign auxiliary map to clock observers map
        clockObservers = aux_mmap;
    }
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
bool Cdmw::clock::ControlledClock_impl<LOCK, GUARD>::
less(const Cdmw::clock::ControlledClockEvent evt1, const Cdmw::clock::ControlledClockEvent evt2)
{
    if (((evt1 & Cdmw::clock::SET_TIME)  && !(evt2 & Cdmw::clock::SET_TIME)) ||
        ((evt1 & Cdmw::clock::SET_RATE)  && !(evt2 & Cdmw::clock::SET_RATE)) ||
        ((evt1 & Cdmw::clock::PAUSE)     && !(evt2 & Cdmw::clock::PAUSE))    ||
        ((evt1 & Cdmw::clock::RESUME)    && !(evt2 & Cdmw::clock::RESUME))   ||
        ((evt1 & Cdmw::clock::TERMINATE) && !(evt2 & Cdmw::clock::TERMINATE))) return false;

    if ((!(evt1 & Cdmw::clock::SET_TIME)  && (evt2 & Cdmw::clock::SET_TIME)) ||
        (!(evt1 & Cdmw::clock::SET_RATE)  && (evt2 & Cdmw::clock::SET_RATE)) ||
        (!(evt1 & Cdmw::clock::PAUSE)     && (evt2 & Cdmw::clock::PAUSE))    ||
        (!(evt1 & Cdmw::clock::RESUME)    && (evt2 & Cdmw::clock::RESUME))   ||
        (!(evt1 & Cdmw::clock::TERMINATE) && (evt2 & Cdmw::clock::TERMINATE))) return true;

    return false;
}

////////////////////////////////////////////////////////////////////////////////

template < typename LOCK, template < typename T > class GUARD >
bool Cdmw::clock::ControlledClock_impl<LOCK, GUARD>::
eq(const Cdmw::clock::ControlledClockEvent evt1, const Cdmw::clock::ControlledClockEvent evt2)
{
    return (((evt1 & Cdmw::clock::SET_TIME)  == (evt2 & Cdmw::clock::SET_TIME)) &&
            ((evt1 & Cdmw::clock::SET_RATE)  == (evt2 & Cdmw::clock::SET_RATE)) &&
            ((evt1 & Cdmw::clock::PAUSE)     == (evt2 & Cdmw::clock::PAUSE))    &&
            ((evt1 & Cdmw::clock::RESUME)    == (evt2 & Cdmw::clock::RESUME))   &&
            ((evt1 & Cdmw::clock::TERMINATE) == (evt2 & Cdmw::clock::TERMINATE)));
}

////////////////////////////////////////////////////////////////////////////////
//  End Of File
////////////////////////////////////////////////////////////////////////////////
