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

#ifndef CARDAMOM_CLOCK_TIMESPAN_HPP
#define CARDAMOM_CLOCK_TIMESPAN_HPP

/**
 *@author Fabrizio Morciano <fmorciano@amjsv.it>
 *@author Raffaele Mele <rmele@progesi.it>
 */
#include "Foundation/orbsupport/RefCountServantBase.hpp"

#include <Time/clockservice/CosClockService.stub.hpp>
#include <Time/clockservice/config.hpp>

namespace Cdmw {    namespace clock {

/**
 *@brief TimeSpan Valuetype implementation.
 */
class CLOCK_API  TimeSpan_impl 
:   public virtual OBV_CosClockService::TimeSpan
,   public virtual CORBA::DefaultValueRefCountBase
{
public:
    TimeSpan_impl();

    /**
     * This method returns an IntervalT structure with the values of 
     * its fields filled in with the corresponding values from the TimeSpan.
     */
    TimeBase::IntervalT time_interval ();

    /**
     * This operation returns a value of type OverlapType depending on how 
     * the interval in the object and the time range represented by the 
     * parameter time overlap. The interval in the object is interval A and 
     * the interval in the parameter UTC is interval B. If OverlapType is not
     * OTNoOverlap, then the out parameter overlap contains the overlap 
     * interval; otherwise, the out parameter contains the gap between the two
     * intervals. The exception CORBA::BAD_PARAM is raised if the UTC passed 
     * in is invalid.
     */
    CosClockService::OverlapType spans(CosClockService::UTC *,
                                       CosClockService::TimeSpan_out) ;

    /**
     * This operation returns a value of type OverlapType depending on how 
     * the interval in the object and interval in the parameter other overlap.
     * The interval in the object is interval A and the interval in the 
     * parameter other is interval B. If OverlapType is not OTNoOverlap, then 
     * the out parameter overlap contains the overlap interval; otherwise, 
     * the out parameter contains the gap between the two intervals. The
     * exception CORBA::BAD_PARAM is raised if the TimeSpan passed in is invalid.
     */
    CosClockService::OverlapType overlaps(CosClockService::TimeSpan *, 
                                          CosClockService::TimeSpan_out);

    /**
     * Returns a UTC in which the inaccuracy interval is equal to the time 
     * interval in the TimeSpan and time value is the midpoint of the interval.
     */
    CosClockService::UTC * time();

private:
    TimeSpan_impl(const TimeSpan_impl& );
    TimeSpan_impl& operator=(const TimeSpan_impl& );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Concrete implementation of valutype TimeSpan
 * this class just need to mix-in RefCounter.
 */
class CLOCK_API  TimeSpan_impl_init
:   public CosClockService::TimeSpan_init
{
public:
    TimeSpan_impl_init();

    /**
     * Creates a TimeSpan from a TimeBase::IntervalT.
     */
    CosClockService::TimeSpan* init (const TimeBase::IntervalT & from)
        throw (CORBA::SystemException);
    
    /**
     * Composes a TimeSpan from an upper and lower bound.
     */
    CosClockService::TimeSpan* compose (TimeBase::TimeT lower_bound,
                                        TimeBase::TimeT upper_bound)
        throw (CORBA::SystemException);
      
private:  // needed to CORBA
    TimeSpan_impl_init(const TimeSpan_impl_init& );
    TimeSpan_impl_init& operator=(const TimeSpan_impl_init& );

    CORBA::ValueBase * create_for_unmarshal (void);
};

}   /* clock */ }   /* Cdmw */

#endif /* CARDAMOM_CLOCK_TIMESPAN_HPP */
