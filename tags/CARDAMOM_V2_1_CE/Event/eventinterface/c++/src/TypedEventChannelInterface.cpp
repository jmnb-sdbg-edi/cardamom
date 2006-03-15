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


#include "Event/eventinterface/TypedEventChannelInterface.hpp"

#include <string>

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

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

TypedEventChannelInterface::TypedEventChannelInterface(CosTypedEventChannelAdmin::TypedEventChannel_ptr eventChannel)
    throw(CORBA::SystemException)
    : m_CosTypedEventChannel(CosTypedEventChannelAdmin::TypedEventChannel::_duplicate(eventChannel))
{
    using namespace Cdmw::OrbSupport;

    if (CORBA::is_nil(eventChannel)) 
        throw CORBA::BAD_PARAM(BAD_PARAMNilObjectReference,CORBA::COMPLETED_NO);

}
 
TypedEventChannelInterface::TypedEventChannelInterface()
    throw(CORBA::SystemException)
    : m_CosTypedEventChannel(CosTypedEventChannelAdmin::TypedEventChannel::_nil())
{
}
   
/********************************************************************/
TypedEventChannelInterface::TypedEventChannelInterface(const TypedEventChannelInterface& rhs)
    throw(CORBA::SystemException)
  : m_CosTypedEventChannel(CosTypedEventChannelAdmin::TypedEventChannel::_duplicate(rhs.m_CosTypedEventChannel))
{
}
    
/********************************************************************/
TypedEventChannelInterface&
TypedEventChannelInterface::operator=(const TypedEventChannelInterface& rhs)
    throw(CORBA::SystemException)
{
    if (&rhs != this) 
    {
        m_CosTypedEventChannel = CosTypedEventChannelAdmin::TypedEventChannel::_duplicate(rhs.m_CosTypedEventChannel);
    }

    return *this;
}

/********************************************************************/
TypedEventChannelInterface::~TypedEventChannelInterface()
    throw()
{
}

/********************************************************************/
void
TypedEventChannelInterface::destroy()
    throw(CORBA::SystemException)
{
    // ask to Event service to destroy channel
    m_CosTypedEventChannel->destroy();
}

/********************************************************************/

CosTypedEventChannelAdmin::TypedEventChannel_ptr
TypedEventChannelInterface::get_CosTypedEventChannel()
   throw ()
{
    return CosTypedEventChannelAdmin::TypedEventChannel::_duplicate(m_CosTypedEventChannel.in());
}

/********************************************************************/
void
TypedEventChannelInterface::connect_typedPushConsumer(const char* interfaceRef, 
                                                      CosEventComm::PushConsumer_ptr consumer) 
    throw (CosTypedEventChannelAdmin::NoSuchImplementation,
           CosEventChannelAdmin::TypeError,
           CORBA::SystemException)
{
    using namespace Cdmw::OrbSupport;

    // Export proxy for supplier
     CosTypedEventChannelAdmin::TypedConsumerAdmin_var consumerAdmin = 
        m_CosTypedEventChannel -> for_consumers();

    CosEventChannelAdmin::ProxyPushSupplier_var proxySupplier;

     try
    {
        proxySupplier = consumerAdmin -> obtain_typed_push_supplier(interfaceRef);
    }
    catch (const CosTypedEventChannelAdmin::NoSuchImplementation& )
    {
        throw;
    }
    
    if (CORBA::is_nil(consumer))
    {
        throw CORBA::BAD_PARAM(BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);
    }

    try
    {
        proxySupplier->connect_push_consumer(consumer);
    }
    catch (const CosEventChannelAdmin::AlreadyConnected& )
    {
        throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
    }
    catch (const CosEventChannelAdmin::TypeError& )
    {
        throw;
    }

}  // End connect_typedPushConsumer()

/********************************************************************/

void
TypedEventChannelInterface::connect_to_channel(const char* interfaceRef,
                                      CosTypedEventChannelAdmin::TypedEventChannel_ptr eventChannelSupplier)
    throw (CosTypedEventChannelAdmin::InterfaceNotSupported,
           CosTypedEventChannelAdmin::NoSuchImplementation, 
           CosEventChannelAdmin::AlreadyConnected,
           CosEventChannelAdmin::TypeError,
           CORBA::SystemException)
{   
    using namespace Cdmw::OrbSupport;

    // proxy for consumer
    CosTypedEventChannelAdmin::TypedProxyPushConsumer_var proxyConsumer;
    CosTypedEventChannelAdmin::TypedSupplierAdmin_var supplierAdmin = 
        m_CosTypedEventChannel->for_suppliers();
    
    if (CORBA::is_nil(supplierAdmin.in()))
    {
        throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
    }
    
    try 
    {
        proxyConsumer = supplierAdmin->obtain_typed_push_consumer(interfaceRef);
    }
    catch (const CosTypedEventChannelAdmin::InterfaceNotSupported& )
    {
        throw;
    }
    
    if (CORBA::is_nil(proxyConsumer.in()))
    {
        throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
    }
    
    // Export proxy for supplier
    CosTypedEventChannelAdmin::TypedConsumerAdmin_var consumerAdmin = 
        eventChannelSupplier->for_consumers();
    CosEventChannelAdmin::ProxyPushSupplier_var proxySupplier;
    
    if (CORBA::is_nil(consumerAdmin.in()))
    {
        throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
    }
    
    try
    {
        proxySupplier = consumerAdmin->obtain_typed_push_supplier(interfaceRef);
    }
    catch (const CosTypedEventChannelAdmin::NoSuchImplementation& )
    {
        throw;
    }
    
    // Connect the proxy. If we're already connected we'll get an
    // exception. This should only happen if we're reusing the
    // same proxy-object.
    try
    {
        CORBA::Object_var obj = proxyConsumer->get_typed_consumer();
        
        if (CORBA::is_nil(obj.in()))
        {
            throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
        }
        
        CosEventComm::PushConsumer_var typedConsumer=
            CosEventComm::PushConsumer::_narrow(obj.in());
        
        if (CORBA::is_nil(typedConsumer.in()))
        {
            throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
        }
        
        proxySupplier->connect_push_consumer(typedConsumer.in());
        
        // connect consumer to supplier
        proxyConsumer->connect_push_supplier(proxySupplier.in());
    }
    catch(const CosEventChannelAdmin::AlreadyConnected&)
    {
        throw;
    }
    catch (const CosEventChannelAdmin::TypeError& )
    {
        throw;
    }

} // End  connect_to_channel

}; // End namespace Event
}; // End namespace Cdmw


