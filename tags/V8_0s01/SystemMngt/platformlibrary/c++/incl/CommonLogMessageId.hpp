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


#ifndef INCL_PLATFORMMNGT_COMMONLOGMESSAGEID_HPP 
#define INCL_PLATFORMMNGT_COMMONLOGMESSAGEID_HPP


namespace Cdmw
{
namespace PlatformMngt
{

const long MSG_ID_COMMON_START  = 1000;

/**
* "Event notifier is running"
*/
const long MSG_ID_EVENT_NOTIF_RUNNING = MSG_ID_COMMON_START + 0;

/**
* "Successful notification to observer '%s' of event \"%s\""
*/
const long MSG_ID_EVENT_NOTIF_SUCCESS = MSG_ID_COMMON_START + 1;

/**
* "Timeout expired for notification to observer '%s' of event \"%s\""
*/
const long MSG_ID_EVENT_NOTIF_TIMEOUT = MSG_ID_COMMON_START + 2;

/**
* "Exception \"%s\" raised by notification to observer '%s' of event \"%s\""
*/
const long MSG_ID_EVENT_NOTIF_EXCEPTION = MSG_ID_COMMON_START + 3;

/**
* Loss of a notification due to a memory exhaustion.
*
* "Loss of notification to observer '%s' of event \"%s\" due to memory exhaustion"
*/
const long MSG_ID_EVENT_NOTIF_NOTIF_LOST = MSG_ID_COMMON_START + 4;

/**
* Loss of an notification event due to a memory exhaustion.
*
* "Loss of notification event due to memory exhaustion"
*/
const long MSG_ID_EVENT_NOTIF_EVENT_LOST = MSG_ID_COMMON_START + 5;

/**
* Loss of an event due to an unexpected exception
*
* "Loss of notification event due to unexpected exception"
*/
const long MSG_ID_EVENT_NOTIF_UNEXP_EXCEPT = MSG_ID_COMMON_START + 6;

/**
* Unexpected exception in Event Handler event execution
*
* "Unexpected exception in Event Handler event execution"
*/
const long MSG_ID_EVENT_HANDLER_EXEC_EXCEPT = MSG_ID_COMMON_START + 7;

/**
* Queue exception in Event Handler
*
* "Queue exception in Event Handler"
*/
const long MSG_ID_EVENT_HANDLER_QUEUE_EXCEPT = MSG_ID_COMMON_START + 8;

/**
* Unexpected exception in Event Handler
*
* "Unexpected exception in Event Handler"
*/
const long MSG_ID_EVENT_HANDLER_UNEXP_EXCEPT = MSG_ID_COMMON_START + 9;

/**
* Loss of a monitoring cycle due to a memory exhaustion.
*
* "Monitoring cycle lost due to memory exhaustion"
*/
const long MSG_ID_MONITORING_NO_MEMORY = MSG_ID_COMMON_START + 10;

/**
* "Monitoring signaled"
*/
const long MSG_ID_MONITORING_SIGNAL = MSG_ID_COMMON_START + 11;


/**
* "Invalid Task identification"
*/
const long MSG_ID_INVALID_TASK = MSG_ID_COMMON_START + 12;



const long MSG_ID_COMMON_END = MSG_ID_COMMON_START + 13;



class CommonLogMessages
{
public:
    static const char** M_commonMessages;
};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_COMMONLOGMESSAGEID_HPP

