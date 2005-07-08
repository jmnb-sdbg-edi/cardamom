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

#ifndef CARDAMOM_CLOCK_UTC_HPP
#define CARDAMOM_CLOCK_UTC_HPP

#include <Time/clockservice/CosClockService.stub.hpp>
#include <Time/clockservice/config.hpp>

namespace Cdmw {    namespace clock {

/**
 *
 * @brief Implementation file for UTC valuetype.
 *
 * @author Fabrizio Morciano
 */

class CLOCK_API  UTC_impl :
    public virtual OBV_CosClockService::UTC, 
    public virtual CORBA::DefaultValueRefCountBase
{
public:
    UTC_impl();

    /**
     * @brief Retrieve inaccuracy value present in UTC.
     * @return inaccuracy composed from inacclo and inacclhi.
     */
    TimeBase::InaccuracyT inaccuracy();

    /**
     * @return time expressed as a TimeBase::UtcT. 
     */
    TimeBase::UtcT  utc_time();

    /**
     * Compare the time contained in the value with the time given in the input
     * parameter <code>with_utc</code> using the comparison type in 
     * <code>comparison_type</code>.
     */
    CosClockService::TimeComparison 
    compare_time(CosClockService::ComparisonType comparison_type,
                 CosClockService::UTC* with_utc);

    /**
     * @brief   Return a <code>TimeSpan</code> value representing the error 
     *          around the time value in the <code>UTC</code> time interval.
     * <code>TimeSpan.upper_bound = UTC.time + UTC.inaccuracy</code>
     * <code>TimeSpan.lower_bound = UTC.time - UTC.inaccuracy</code>
     */
    CosClockService::TimeSpan*  interval();

private:
    UTC_impl(const UTC_impl&);
    UTC_impl operator=(const UTC_impl&);
};

///////////////////////////////////////////////////////////////////////////////

/**
 *
 * @brief Implementation file for UTC valuetype.
 *
 * @author Fabrizio Morciano
 */

class CLOCK_API  UTC_impl_init :
    public CosClockService::UTC_init
{
public:
    UTC_impl_init();
    
    /**
     * @brief Build an UTC from an Utc type.
     * @param from is the UtcT value.
     * @exception CORBA::SystemException if it is not possible to build 
     *            an UTC.
     */
    CosClockService::UTC* init(const TimeBase::UtcT & from) 
        throw (CORBA::SystemException);

    /**
     * @brief Compose an UTC from its parts.
     * @param time to set in UTC.
     * @param inacclo low part of an UTC inaccuracy.
     * @param inacchi high part of an UTC inaccuracy.
     * @param tdf time displacement factor from Greenwich meridian.
     * @exception CORBA::SystemException if it is not possible to build 
     *            an UTC.
     */
    CosClockService::UTC* compose(TimeBase::TimeT time,
                                  CORBA::ULong inacclo,
                                  CORBA::UShort inacchi,
                                  TimeBase::TdfT tdf  ) 
        throw (CORBA::SystemException); 

private:  // needed to CORBA

    UTC_impl_init(const UTC_impl_init& );
    UTC_impl_init& operator=(const UTC_impl_init& );

    CORBA::ValueBase * create_for_unmarshal (void);
};

} /* clock */   } /* Cdmw */

#endif /* CARDAMOM_CLOCK_UTC_HPP */
