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

#ifndef CARDAMOM_CLOCK_CLOCK_IMPL_HPP
#define CARDAMOM_CLOCK_CLOCK_IMPL_HPP

/**
 * @file
 * 
 * @author Fabrizio Morciano <fmorciano@amsjv.it>
 * @author Lello Mele <lellomele@yahoo.com>
 * @author Angelo Corsaro <acorsaro@amsjv.it>
 */

#include <Time/clockservice/CosClockService.skel.hpp>

// -- EVoT _impl Includes --
#ifndef WIN32
#   if CDMW_ORB_VDR==tao
#   include <orbsvcs/Property/CosPropertyService_i.h>
#   elif CDMW_ORB_VDR==orbacus
#   include <Foundation/idllib/CosProperty.stub.hpp>
#   endif
#endif

#include <Time/clockservice/config.hpp>
#include <Time/clockservice/Controller_impl.hpp>
#include <Time/clockservice/IClock.hpp>


// -- STD C++ Includes --
#include <vector>


namespace Cdmw  { namespace clock {

/**
 * @brief This class provides an implementation of the Local Clock. 
 *
 *  This clock is not synchronized with any external source, unless the
 *  machine clock is not kept synchronized with a master clock by
 *  using some external mean such as NTP.
 *
 * @author Fabrizio Morciano <fmorciano@amsjv.it>
 * @author Lello Mele <lellomele@libero.it>
 * @author Angelo Corsaro <acorsaro@amsjv.it>
 */
class  CLOCK_API Clock_impl: 
        public virtual POA_CosClockService::Clock,
        public virtual Cdmw::clock::IClock
{
public:
    Clock_impl();
    virtual ~Clock_impl ();
    
    /**
     * @brief The known properties of the clock.
     * @return reference to clock's properties.
     * @exception CORBA::SystemException if error occurs in setting properties
     */
    virtual ::CosPropertyService::PropertySet_ptr properties() 
        throw(CORBA::SystemException);
    

    /**
     * @brief Provides a measure of the current time. The time unit is 100
     * nanosecond e.g. 10e-7 seconds.
     *
     * @return TimeT with current time
     * @exception CORBA::SystemException if error occurs in setting properties
     * @exception CosClockService::TimeUnavailable if time is not available
     *  with required security assurance.
     */
    virtual TimeBase::TimeT current_time()
        throw(CORBA::SystemException, 
              CosClockService::TimeUnavailable);

    
    /**
     * @brief Auxililary function to compute current machine time.
     */
    static  TimeBase::TimeT compute_current_time(); 

private:
    Clock_impl(const Clock_impl&);
    Clock_impl& operator=(const Clock_impl&);
    
protected:
    TAO_PropertySetFactory propset_factory_;
    CosPropertyService::PropertySet_var property_;    
};

    
} /* clock */ } /* Cdmw */

#endif /* CARDAMOM_CLOCK_CLOCK_IMPL_HPP */

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
