/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
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
*/
/* ===================================================================== */


#include "platformdaemon/PlatformNotifier.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


PlatformNotifier::PlatformNotifier(unsigned long callTimeout)
    throw (InternalErrorException)
    : m_callTimeout(callTimeout)
{

    try
    {
        m_boundSyncCallFactory = BoundSyncCallFactory::getFactory();
    }
    catch(const Cdmw::Exception&)
    {
        CDMW_THROW(InternalErrorException);
    }

}


PlatformNotifier::~PlatformNotifier() throw()
{

}


std::string PlatformNotifier::eventToString(CdmwPlatformMngt::PlatformInfo* event)
{

    std::string eventStr("Process '");

    eventStr += (event->process_name).in();
    eventStr += "' ";

    if (event->process_status == CdmwPlatformMngt::STARTED)
        eventStr += "started";
    else
        eventStr += "ended";

    return eventStr;
}


BoundSyncCall* PlatformNotifier::createEventNotificationCall(
    CdmwPlatformMngt::PlatformObserver_ptr observer,
    CdmwPlatformMngt::PlatformInfo* event)
    throw(OutOfMemoryException)
{

    BoundSyncCall* notifCall = 
        m_boundSyncCallFactory->createPlatformInfoNotificationCall(observer, *event, m_callTimeout);

    return notifCall;

}


} // End namespace PlatformMngt
} // End namespace Cdmw

