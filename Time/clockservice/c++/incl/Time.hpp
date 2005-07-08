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

#ifndef CARDAMOM_CLOCK_TIME_HPP
#define CARDAMOM_CLOCK_TIME_HPP

/**
 * @file
 * @brief  Time service collection header.
 * 
 * @author Fabrizio Morciano <fmorciano@amsjv.it>
 */

#include "tao/corba.h"
#include <string>
#include <Time/clockservice/config.hpp>
#include <Time/clockservice/Clock_impl.hpp>
#include <Time/clockservice/ClockCatalog_impl.hpp>
#include <Time/clockservice/ControlledExecutor_impl.hpp>
#include <Time/clockservice/UTCTimeService_impl.hpp>
#include <Time/clockservice/UTC_impl.hpp>
#include <Time/clockservice/TimeSpan_impl.hpp>
#include <Time/clockservice/Macro.hpp>

/**
 *  @namespace Cdmw::clock
 *  @brief This namespace contains all definitions of clock related objects.
 */
namespace Cdmw  {   namespace   clock {

    /**
     * @fn std::ostream& print(std::ostream& os,CORBA::ULongLong time_in_utc);
     * @brief Print an utc value in a more readable format.
     * @param os destination ostream
     * @param time_in_utc value in utc
     */                      
    std::string print(CORBA::ULongLong time_in_utc);
} /* clock*/  }  /* Cdmw */

#endif /* CARDAMOM_CLOCK_TIME_HPP */
