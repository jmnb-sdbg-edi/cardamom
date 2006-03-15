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
#ifndef _CDMW_CLOCK_CONTROLLED_CLOCK_COMMAND_HANDLER_HPP_
#define _CDMW_CLOCK_CONTROLLED_CLOCK_COMMAND_HANDLER_HPP_

#include <Time/clockservice/CdmwTime.skel.hpp>

namespace Cdmw {
    namespace clock {
        class ControlledClockCommandHandler;
    }
}
/**
 * @brief This class defines the protocol that should be implemented by those
 * classes that whish to handle controlled clock related events.
 * 
 * A controlled clock raises an event each time its state is changed by
 * either setting the time, setting the rate, pausing, etc.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * 
 */
class Cdmw::clock::ControlledClockCommandHandler
{
public:
    virtual ~ControlledClockCommandHandler();

public:
    /**
     * Invoked when a new time is set.
     *
     * @param status new clock status
     * @param X0 new real-time origin
     * @param Y0 new virtual-time origin.
     * @param frozen_time new frozen time.
     * @param ratio the new ratio
     */
    virtual void handle_set(Cdmw::clock::Status status,
                            TimeBase::TimeT     X0,
                            TimeBase::TimeT     Y0,
                            TimeBase::TimeT     frozen_time,
			    CORBA::Float        ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported) = 0;

    /**
     * Invoked when the rate is changed.
     *
     * @param status new clock status
     * @param X0 new real-time origin
     * @param Y0 new virtual-time origin.
     * @param frozen_time new frozen time.
     * @param ratio the new ratio

     */
    virtual void handle_set_rate(Cdmw::clock::Status status,
                                 TimeBase::TimeT     X0,
                                 TimeBase::TimeT     Y0,
                                 TimeBase::TimeT     frozen_time,
                                 CORBA::Float        ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported) = 0;

    /**
     * Invoked when the clock is paused.
     *
     * @param status new clock status
     * @param X0 new real-time origin
     * @param Y0 new virtual-time origin.
     * @param frozen_time new frozen time.
     * @param ratio the new ratio
     */
    virtual void handle_pause(Cdmw::clock::Status status,
                              TimeBase::TimeT     X0,
			      TimeBase::TimeT     Y0,
			      TimeBase::TimeT     frozen_time,
			      CORBA::Float        ratio)
      throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported) = 0;

    /**
     * Invoked when the clock is resumed.
     *
     * @param status new clock status
     * @param X0 new real-time origin
     * @param Y0 new virtual-time origin.
     * @param frozen_time new frozen time.
     * @param ratio the new ratio
     */
    virtual void handle_resume(Cdmw::clock::Status status,
                               TimeBase::TimeT     X0,
			       TimeBase::TimeT     Y0,
			       TimeBase::TimeT     frozen_time,
			       CORBA::Float        ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported) = 0;

    /**
     * Invoked when the clock is terminated.
     */ 
    virtual void handle_terminate()
        throw (CORBA::SystemException ,
               CosClockService::ControlledClock::NotSupported) = 0;

    /**
     * Invoked when the clock receives an align request.
     */ 
    virtual void handle_align_request()
        throw (CORBA::SystemException ,
               CosClockService::ControlledClock::NotSupported) = 0;

    /**
     * Invoked when the clock receives an align response.
     *
     * @param status new clock status
     * @param X0 new real-time origin
     * @param Y0 new virtual-time origin.
     * @param frozen_time new frozen time.
     * @param ratio the new ratio
     */
    virtual void handle_align_response(Cdmw::clock::Status status,
                                       TimeBase::TimeT     X0,
			               TimeBase::TimeT     Y0,
			               TimeBase::TimeT     frozen_time,
			               CORBA::Float        ratio)
        throw (CORBA::SystemException,
               CosClockService::ControlledClock::NotSupported) = 0;

};

#endif /* _CDMW_CLOCK_CONTROLLED_CLOCK_COMMAND_HANDLER_HPP_ */
