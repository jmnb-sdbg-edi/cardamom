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


#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


SupervisionEventNotifier::SupervisionEventNotifier(unsigned long callTimeout)
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


SupervisionEventNotifier::~SupervisionEventNotifier() throw()
{

}


SupervisionEventNotifier::SupervisionEventNotifier(const SupervisionEventNotifier& rhs)
{

}


SupervisionEventNotifier& SupervisionEventNotifier::operator=(const SupervisionEventNotifier& rhs)
{
    return *this;
}


std::string SupervisionEventNotifier::eventToString(CdmwPlatformMngt::Event* event)
{
    CORBA::String_var str = event->to_string();

    std::string eventStr(str.in());

    return eventStr;
}


BoundSyncCall* SupervisionEventNotifier::createEventNotificationCall(
    CdmwPlatformMngt::SupervisionObserver_ptr observer,
    CdmwPlatformMngt::Event* event)
    throw(OutOfMemoryException)
{

    BoundSyncCall* notifCall = 
        m_boundSyncCallFactory->createEventNotificationCall(observer, event, m_callTimeout);

    return notifCall;

}


} // End namespace PlatformMngt
} // End namespace Cdmw

