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

#include "Event/eventinterface/EventChannelInterface.hpp"
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

    
EventChannelInterface::EventChannelInterface(CosEventChannelAdmin::EventChannel_ptr eventChannel)
    throw(CORBA::SystemException)
    : m_CosEventChannel(CosEventChannelAdmin::EventChannel::_duplicate(eventChannel))
{
    using namespace Cdmw::OrbSupport;

    if (CORBA::is_nil(eventChannel)) 
        throw CORBA::BAD_PARAM(BAD_PARAMNilObjectReference,CORBA::COMPLETED_NO);

}

EventChannelInterface::EventChannelInterface() 
    throw(CORBA::SystemException)
  : m_CosEventChannel(CosEventChannelAdmin::EventChannel::_nil())
{
}

    
/********************************************************************/
EventChannelInterface::EventChannelInterface(const EventChannelInterface& rhs)
    throw(CORBA::SystemException)
  : m_CosEventChannel(CosEventChannelAdmin::EventChannel::_duplicate(rhs.m_CosEventChannel))
{
}
    
/********************************************************************/
EventChannelInterface&
EventChannelInterface::operator=(const EventChannelInterface& rhs)
    throw(CORBA::SystemException)
{
    if (&rhs != this) 
    {
        m_CosEventChannel 
            = CosEventChannelAdmin::EventChannel::_duplicate(rhs.m_CosEventChannel.in());
    }

    return *this;
}

/********************************************************************/
EventChannelInterface::~EventChannelInterface()
    throw()
{
}

/********************************************************************/

CosEventChannelAdmin::EventChannel_ptr
EventChannelInterface::get_CosEventChannel()
    throw()
{
    return CosEventChannelAdmin::EventChannel::_duplicate(m_CosEventChannel.in());
}


/********************************************************************/
void
EventChannelInterface::destroy()
throw (CORBA::SystemException)
{
    // ask to Event service to destroy channel
    m_CosEventChannel->destroy();

} // End destroy()

/********************************************************************/
CosEventComm::PushConsumer_ptr 
EventChannelInterface::get_pushConsumer(CosEventComm::PushSupplier_ptr pushSupplier)
    throw (CORBA::SystemException)
{
    using namespace Cdmw::OrbSupport;

    // proxy for consumer
    CosEventChannelAdmin::ProxyPushConsumer_var proxyConsumer;
    CosEventChannelAdmin::SupplierAdmin_var supplierAdmin;
    
    try 
    {
        supplierAdmin = m_CosEventChannel->for_suppliers();

        if (CORBA::is_nil(supplierAdmin.in()))
        {
           throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
        }
        proxyConsumer = supplierAdmin->obtain_push_consumer();
        
        if (CORBA::is_nil(proxyConsumer.in()))
        {
           throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
        }
        proxyConsumer->connect_push_supplier(pushSupplier);
    }
    catch (const CosEventChannelAdmin::AlreadyConnected& )
    {
        throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
    }

    return proxyConsumer._retn();
    
} // End get_pushConsumer()

/********************************************************************/

CosEventComm::PushSupplier_ptr
EventChannelInterface::connect_pushConsumer(CosEventComm::PushConsumer_ptr  consumer)
    throw (CORBA::SystemException)
{
    using namespace Cdmw::OrbSupport;

    // Export proxy for supplier
    CosEventChannelAdmin::ConsumerAdmin_var consumerAdmin;
    CosEventChannelAdmin::ProxyPushSupplier_var proxySupplier;
        
    try
    {
        consumerAdmin = m_CosEventChannel->for_consumers();

        if (CORBA::is_nil(consumerAdmin.in()))
        {
            throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
        }

        proxySupplier = consumerAdmin->obtain_push_supplier();

        if (CORBA::is_nil(proxySupplier.in()))
        {
            throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
        }

        proxySupplier->connect_push_consumer(consumer);
    }
    catch (const CosEventChannelAdmin::AlreadyConnected& )
    {
        throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
    }
    catch (const CosEventChannelAdmin::TypeError& )
    {
        throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
    }
    
    return proxySupplier._retn();
} // End connect_pushConsumer()

/********************************************************************/

void
EventChannelInterface::connect_to_channel(CosEventChannelAdmin::EventChannel_ptr eventChannelSupplier)
throw (CosEventChannelAdmin::AlreadyConnected, 
       CORBA::SystemException)
{
    using namespace Cdmw::OrbSupport;

    if (CORBA::is_nil(eventChannelSupplier))
    {
        throw CORBA::BAD_PARAM(BAD_PARAMInvalidChannelName, CORBA::COMPLETED_NO);
    }
    
    // proxy for consumer
    CosEventChannelAdmin::ProxyPushConsumer_var proxyConsumer;
    CosEventChannelAdmin::SupplierAdmin_var supplierAdmin = 
        m_CosEventChannel->for_suppliers();

    if (CORBA::is_nil(supplierAdmin.in()))
    {
        throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
    }

    proxyConsumer = supplierAdmin->obtain_push_consumer();
    
    if (CORBA::is_nil(proxyConsumer.in()))
    {
        throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
    }
    
    // Export proxy for supplier
    CosEventChannelAdmin::ConsumerAdmin_var consumerAdmin = 
        eventChannelSupplier->for_consumers();
    CosEventChannelAdmin::ProxyPushSupplier_var proxySupplier;
    
    proxySupplier = consumerAdmin->obtain_push_supplier();
 
    if (CORBA::is_nil(proxySupplier.in()))
    {
        throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
    }

    // Connect the proxy. If we're already connected we'll get an
    // exception. This should only happen if we're reusing the
    // same proxy-object.
    try
    {
        proxySupplier->connect_push_consumer(proxyConsumer.in());
        
        // connect consumer to supplier
        // 
        // Dead_lock with : proxyConsumer -> connect_push_supplier(proxySupplier);
        proxyConsumer -> connect_push_supplier(CosEventComm::PushSupplier::_nil());
    }
    catch(const CosEventChannelAdmin::AlreadyConnected&)
    {
        throw;
    }
    catch (const CosEventChannelAdmin::TypeError& )
    {
        // exception added in COS EVENT for typed channel only!
        throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
    }

} // End  connect_to_channel

}; // End namespace Event
}; // End namespace Cdmw


