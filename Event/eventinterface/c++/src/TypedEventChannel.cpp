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


#include "Event/eventinterface/TypedEventChannel.hpp"
#include <string>

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


TypedEventChannel::TypedEventChannel(const char* channelName, 
                                     CosNaming::NamingContext_ptr ctx)
throw(CORBA::SystemException)
: m_channelName(channelName),
  m_NamingInterface(ctx)

{
    using namespace Cdmw::OrbSupport;

    // Get a reference on the Cos EventChannel
    CosTypedEventChannelAdmin::TypedEventChannel_var l_CosTypedEventChannel;
    try 
    {
        CORBA::Object_var obj = m_NamingInterface.resolve(channelName);
        l_CosTypedEventChannel = 
            CosTypedEventChannelAdmin::TypedEventChannel::_narrow(obj.in());
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

    if (CORBA::is_nil(l_CosTypedEventChannel.in()))
    {
        throw CORBA::BAD_PARAM(BAD_PARAMInvalidChannelName, CORBA::COMPLETED_NO);
    }

    m_typedEventChannelInterface = new TypedEventChannelInterface(l_CosTypedEventChannel.in());
   
    // initialisation done: everything is OK
}

/********************************************************************/
TypedEventChannel::TypedEventChannel(const TypedEventChannel& rhs)
    throw(CORBA::SystemException)
    : m_channelName(rhs.m_channelName),
      m_NamingInterface(rhs.m_NamingInterface)
{
    m_typedEventChannelInterface = 
        new TypedEventChannelInterface(rhs.m_typedEventChannelInterface->get_CosTypedEventChannel());
}

/********************************************************************/
TypedEventChannel&
TypedEventChannel::operator=(const TypedEventChannel& rhs)
    throw(CORBA::SystemException)
{
    if (&rhs != this) 
    {
        if (m_typedEventChannelInterface)
        {
            delete m_typedEventChannelInterface;
        }
        m_typedEventChannelInterface = 
            new TypedEventChannelInterface(rhs.m_typedEventChannelInterface->get_CosTypedEventChannel());
        m_NamingInterface = rhs.m_NamingInterface;
        m_channelName = rhs.m_channelName;
    }
    
    return *this;
}

/********************************************************************/
TypedEventChannel::~TypedEventChannel()
    throw()
{
    delete m_typedEventChannelInterface;
}

/********************************************************************/
void
TypedEventChannel::destroy()
throw (CORBA::SystemException)
{
    // ask to Event service to destroy channel
    m_typedEventChannelInterface->destroy();

} // End destroy()

/********************************************************************/

CosTypedEventChannelAdmin::TypedEventChannel_ptr
TypedEventChannel::get_CosTypedEventChannel()
   throw ()
{
    return m_typedEventChannelInterface->get_CosTypedEventChannel();
}

/********************************************************************/
void
TypedEventChannel::connect_typedPushConsumer(const char* interfaceRef, 
                                             CosEventComm::PushConsumer_ptr consumer) 
    throw (CosTypedEventChannelAdmin::NoSuchImplementation,
           CosEventChannelAdmin::TypeError,
           CORBA::SystemException)
{
    m_typedEventChannelInterface->connect_typedPushConsumer(interfaceRef, consumer);

}  // End connect_typedPushConsumer()

/********************************************************************/

void
TypedEventChannel::connect_to_channel (const char* interfaceRef,
                                       const char* channelName)
    throw (CosTypedEventChannelAdmin::InterfaceNotSupported,
           CosTypedEventChannelAdmin::NoSuchImplementation, 
           CosEventChannelAdmin::AlreadyConnected,
           CosEventChannelAdmin::TypeError,
           CORBA::SystemException)
{
    using namespace Cdmw::OrbSupport;

    // Get the channel object to which we want to connect.
    CosTypedEventChannelAdmin::TypedEventChannel_var supplierEventChannel;
    try 
    {
        CORBA::Object_var obj = m_NamingInterface.resolve(channelName);
        supplierEventChannel = 
            CosTypedEventChannelAdmin::TypedEventChannel::_narrow(obj.in());
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
    
    if (CORBA::is_nil(supplierEventChannel.in()))
    {
        throw CORBA::BAD_PARAM(BAD_PARAMInvalidChannelName, CORBA::COMPLETED_NO);
    }
    
    m_typedEventChannelInterface->connect_to_channel(interfaceRef,
                                                     supplierEventChannel.in());
     
} // End  connect_to_channel

}; // End namespace Event
}; // End namespace Cdmw


