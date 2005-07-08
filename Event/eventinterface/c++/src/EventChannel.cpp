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


#include <string>
#include "Event/eventinterface/EventChannel.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

namespace Cdmw
{
namespace Event
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


EventChannel::EventChannel(const char* channelName, CosNaming::NamingContext_ptr ctx)
    throw(CORBA::SystemException)
  : m_channelName(channelName),
    m_NamingInterface(ctx)

{
    using namespace Cdmw::OrbSupport;

    // Get a reference on the Cos EventChannel
    CosEventChannelAdmin::EventChannel_var l_CosEventChannel; 
    try 
    {
        CORBA::Object_var obj = m_NamingInterface.resolve(channelName);
        l_CosEventChannel = CosEventChannelAdmin::EventChannel::_narrow(obj.in());
    }
    catch (CosNaming::NamingContext::NotFound& )
    {
        throw CORBA::BAD_PARAM(BAD_PARAMInvalidChannelName, CORBA::COMPLETED_NO);
    }
    catch (CosNaming::NamingContext::CannotProceed& )
    {
        throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
    }
    catch (CosNaming::NamingContext::InvalidName& )
    {
        throw CORBA::BAD_PARAM(BAD_PARAMInvalidChannelName, CORBA::COMPLETED_NO);
    }

    if (CORBA::is_nil(l_CosEventChannel.in()))
    {
        throw CORBA::BAD_PARAM(BAD_PARAMInvalidChannelName, CORBA::COMPLETED_NO);
    }

    m_eventChannelInterface = new EventChannelInterface(l_CosEventChannel.in());
   
    // initialisation done: everything is OK
}

/********************************************************************/
EventChannel::EventChannel(const EventChannel& rhs)
    throw(CORBA::SystemException)
    : m_channelName(rhs.m_channelName),
      m_NamingInterface(rhs.m_NamingInterface)
{
    m_eventChannelInterface = 
        new EventChannelInterface(rhs.m_eventChannelInterface->get_CosEventChannel());
}

/********************************************************************/
EventChannel&
EventChannel::operator=(const EventChannel& rhs)
    throw(CORBA::SystemException)
{
    if (&rhs != this) 
    {
        if (m_eventChannelInterface)
        {
            delete m_eventChannelInterface;
        }
        m_eventChannelInterface = 
            new EventChannelInterface(rhs.m_eventChannelInterface->get_CosEventChannel());
        m_NamingInterface = rhs.m_NamingInterface;
        m_channelName = rhs.m_channelName;
    }
    
    return *this;
}


/********************************************************************/
EventChannel::~EventChannel()
    throw()
{
    delete m_eventChannelInterface;
}

/********************************************************************/

CosEventChannelAdmin::EventChannel_ptr
EventChannel::get_CosEventChannel()
    throw()
{
    return m_eventChannelInterface->get_CosEventChannel();
}


/********************************************************************/
void
EventChannel::destroy()
throw (CORBA::SystemException)
{
    // ask to Event service to destroy channel
    m_eventChannelInterface->destroy();

} // End destroy()

/********************************************************************/
CosEventComm::PushConsumer_ptr 
EventChannel::get_pushConsumer(CosEventComm::PushSupplier_ptr pushSupplier)
    throw (CORBA::SystemException)
{
    return m_eventChannelInterface->get_pushConsumer(pushSupplier);

} // End get_pushConsumer()

/********************************************************************/

void
EventChannel::connect_pushConsumer (CosEventComm::PushConsumer_ptr consumer)
    throw (CORBA::SystemException)
{
    m_eventChannelInterface->connect_pushConsumer(consumer);
    
} // End connect_pushConsumer()

/********************************************************************/

void
EventChannel::connect_to_channel (const char* channelName)
throw (CosEventChannelAdmin::AlreadyConnected, 
       CORBA::SystemException)
{
    using namespace Cdmw::OrbSupport;

    // Get the channel object to which we want to connect.
    CosEventChannelAdmin::EventChannel_ptr supplierEventChannel;
    try 
    {
        CORBA::Object_var obj = m_NamingInterface.resolve(channelName);
        supplierEventChannel = CosEventChannelAdmin::EventChannel::_narrow(obj.in());
    }
    catch (CosNaming::NamingContext::NotFound& )
    {
        throw CORBA::BAD_PARAM(BAD_PARAMInvalidChannelName, CORBA::COMPLETED_NO);
    }
    catch (CosNaming::NamingContext::CannotProceed& )
    {
        throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
    }
    catch (CosNaming::NamingContext::InvalidName& )
    {
        throw CORBA::BAD_PARAM(BAD_PARAMInvalidChannelName, CORBA::COMPLETED_NO);
    }

    if (CORBA::is_nil(supplierEventChannel))
    {
        throw CORBA::BAD_PARAM(BAD_PARAMInvalidChannelName, CORBA::COMPLETED_NO);
    }
    
    m_eventChannelInterface->connect_to_channel(supplierEventChannel);

} // End  connect_to_channel

}; // End namespace Event
}; // End namespace Cdmw


