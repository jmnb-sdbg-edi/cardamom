/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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


#ifndef INCL_PLATFORMMNGT_EVENT_HEADER_FACTORY_HPP 
#define INCL_PLATFORMMNGT_EVENT_HEADER_FACTORY_HPP 


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include <ctime>

namespace Cdmw
{
namespace PlatformMngt
{


/**
*Purpose:
*<p> 
* The factory of event header.
*/
class EventHeaderFactory
{

    public:

        /**
        * Purpose:
        * Returns the header embedding the specified level and the timestamp
        * of the operation call.
        *
        * @param level The event level
        *
        * @return The event header
        *
        */ 
        static CdmwPlatformMngtBase::EventHeader createHeader(
            CdmwPlatformMngtBase::EventLevel level)
        {

            CdmwPlatformMngtBase::EventHeader eventHeader;

            eventHeader.time_stamp = createTimeStamp();
            eventHeader.level = level;
            
            // initialise the emission event timestamp
            // used for emission
            eventHeader.event_key.primary_key = 0;
            eventHeader.event_key.seconds = 0;
            eventHeader.event_key.microseconds = 0;
            eventHeader.event_key.counter_inx = 0;
            
            return eventHeader;            
        }

        
        /**
        * Purpose:
        * Returns the header embedding the specified level and the timestamp
        * of the operation call.
        *
        * @param time_stamp The event time stamp
        * @param level The event level
        *
        * @return The event header
        *
        */ 
        static CdmwPlatformMngtBase::EventHeader createHeader(
            const CdmwPlatformMngtBase::TimeStamp& time_stamp,
            CdmwPlatformMngtBase::EventLevel level)
        {

            CdmwPlatformMngtBase::EventHeader eventHeader;

            eventHeader.time_stamp = time_stamp;
            eventHeader.level = level;
            
            // initialise the emission event timestamp
            // used for emission
            eventHeader.event_key.primary_key = 0;
            eventHeader.event_key.seconds = 0;
            eventHeader.event_key.microseconds = 0;
            eventHeader.event_key.counter_inx = 0;
            
            return eventHeader;             
        }

        /**
        * Purpose:
        * Returns the timestamp of the operation call.
        */ 
        static CdmwPlatformMngtBase::TimeStamp createTimeStamp()
        {

            CdmwPlatformMngtBase::TimeStamp timeStamp;

            Cdmw::OsSupport::OS::Timeval time_val = Cdmw::OsSupport::OS::get_time();

            time_t ltime = time_val.seconds;
//            if (time(&ltime) != time_t(-1))
            {
                tm now;
                ::localtime_r(&ltime, &now);

                timeStamp.year =  now.tm_year + 1900;
                timeStamp.month = now.tm_mon + 1;
                timeStamp.day =   now.tm_mday;

                timeStamp.hour = now.tm_hour;
                timeStamp.minute = now.tm_min;
                timeStamp.second = now.tm_sec;
                timeStamp.millisecond = (time_val.microseconds)/1000;
                timeStamp.microsecond = (time_val.microseconds)%1000;
            }
            
            return timeStamp; 
            
        }


}; // End class EventHeaderFactory

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_EVENT_HEADER_FACTORY_HPP

