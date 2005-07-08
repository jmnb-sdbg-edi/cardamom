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

#ifndef CARDAMOM_CLOCK_UTC_TIME_SERVICE_HPP
#define CARDAMOM_CLOCK_UTC_TIME_SERVICE_HPP

/**
 *@file
 * 
 *@author Fabrizio Morciano <fmorciano@amjsv.it>
 *@author Lello Mele <lellomele@yahoo.com>
 */

#include <Time/clockservice/CosClockService.skel.hpp>

#ifndef WIN32
#   if CDMW_ORB_VDR == tao
#   include <orbsvcs/Property/CosPropertyService_i.h>
#   elif CDMW_ORB_VDR == orbacus         
#   include <Foundation/idllib/CosProperty.stub.hpp>
#   endif
#endif

#include <Time/clockservice/config.hpp>
#include <Time/clockservice/Clock_impl.hpp>
#include <Time/clockservice/IClock.hpp>

namespace Cdmw  {   namespace clock     {


// forward declaration
class CLOCK_API UTC_impl_init;

/**
 * @brief UtcTimeService_impl is the class who implement the
 *        UtcTimeService interface. 
 * 
 * UtcTimeService interface provides operation for obtaining
 * the current time.
 *
 *@author Fabrizio Morciano <fmorciano@amjsv.it>
 *@author Lello Mele <lellomele@yahoo.com>
 * 
 */
class CLOCK_API  UtcTimeService_impl:
        public POA_CosClockService::UtcTimeService,
        public virtual Cdmw::clock::IClock 
{
public:

    UtcTimeService_impl();
    
    virtual ~UtcTimeService_impl();

    /**
     * @brief Returns the current time and an estimate of inaccuracy
     * in a utc.
     * 
     * @exception CosClockService::TimeUnavailable if is not possible to obtain 
     *            a time. 
     * @attention time returned in UTC from this operation is not guaranteed 
     */
    virtual::CosClockService::UTC *universal_time()
        throw(CORBA::SystemException,
              CosClockService::TimeUnavailable);

    /**
     * @brief Returns the current time in a UTC only if the time can
     * be guaranteed to have been obtained securely.
     * 
     * @exception CosClockService::TimeUnavailable if is not possible to obtain 
     *            a time.
     *
     * @attention Not implemented in present version. 
     */
    virtual::CosClockService::UTC *secure_universal_time()
        throw(CORBA::SystemException,
              CosClockService::TimeUnavailable);

    /**
     * @brief Returns a new UTC containing the absolute time corresponding
     * to the present time offset by the parameter with_offset
     * 
     * @param with_offset UTC containing time offset.
     *            a time. 
     * @exception CosClockService::TimeUnavailable if is not possible to obtain 
     *            a time. 
     */
    virtual::CosClockService::UTC* absolute_time (
            CosClockService::UTC * with_offset)
        throw(CORBA::SystemException,
              CosClockService::TimeUnavailable);

    /**
     * @brief Returns the properties of the UTCTimeService clock.
     */
    virtual::CosPropertyService::PropertySet_ptr  properties()
        throw(CORBA::SystemException);

    /**
     * @brief Returns the clock's current measurement of time.
     * 
     * @attention clock's readings in Gregorian format.
     */
    virtual TimeBase::TimeT current_time()
        throw(CORBA::SystemException,
              CosClockService::TimeUnavailable);
private:
    TAO_PropertySetFactory propset_factory_;
    CosPropertyService::PropertySet_var property_;
    UTC_impl_init *factory_;
};

} /* clock */ } /* Cdmw */

#endif /* CARDAMOM_CLOCK_UTC_TIME_SERVICE_HPP  */
