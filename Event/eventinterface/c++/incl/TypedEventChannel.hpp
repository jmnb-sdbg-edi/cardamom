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


#ifndef INCL_EVENT_TYPEDEVENTCHANNEL_HPP 
#define INCL_EVENT_TYPEDEVENTCHANNEL_HPP 

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ObjectTraits.hpp>
#include <Foundation/common/Exception.hpp> 
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

#include <Event/eventinterface/TypedEventChannelInterface.hpp>

/**
* Root namespace for Cdmw runtime.
*/

namespace Cdmw
{

/**
* Namespace for Cdmw Event types and data.
*/

namespace Event
{



/**
*Purpose:
*<p>   This class provides a simplified interface to connect and get push 
*      consumer to/from a typed event channel. It internally uses ORBACUS
*      Event which is compliant with OMG CORBA COS Event service. In this 
*      interface, event channel are defined by a name and a naming context.
*
*Features:
*<p> 
*    None
*
*@see  Object Management Group. 1998. CORBAservices: Common Object Services
*      Specification. 
*@link ftp://www.omg.org/pub/docs/formal/98-12-09.pdf. 
*
*/
class TypedEventChannel
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        *
        *@param channelName Name of the event channel.
        *@param ctx NamingContext where channelName is registered.
        *@exception CORBA::SystemException CORBA::BAD_PARAM if 
        * channel name is invalid or not found or context is nil.
        * CORBA::INTERNAL for internal error and any other system
        * exception.
        * 
        */ 
        TypedEventChannel(const char* channelName, 
                          CosNaming::NamingContext_ptr ctx)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        *@param rhs TypedEventChannel object to be copied.
        *
        *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
        * may throw CORBA system exceptions.
        */ 
        TypedEventChannel(const TypedEventChannel& rhs)
                throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> Assignement operator. 
        *
        *@param rhs A TypedEventChannel object to assign to current object.
        *
        *@return A reference to the current object.
        *
        *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
        * may throw CORBA system exceptions.
        */         
        TypedEventChannel&
        operator=(const TypedEventChannel& rhs)
                throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> Destructor 
        * 
        */ 
        virtual 
        ~TypedEventChannel()
                throw();

        /**
        * Purpose:
        * <p> This operation returns a reference on a COS Typed event channel.
        *     This allows the user to easy get a reference of the Cos Typed event
        *     channel.
        *
        *@return Reference on the CosTypedEventChannel
        */ 
        CosTypedEventChannelAdmin::TypedEventChannel_ptr
        get_CosTypedEventChannel()
            throw ();


        /**
        * Purpose:
        * <p> This operation destroys the current Eventchannel.
        *     It unregisters event channel and calls destroy operation
        *     on the Cos Event channel.
        *
        *@exception CORBA::SystemException any CORBA system exception.
        *
        */
        void 
        destroy()
        throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> This operation returns a reference on the ProxyPushConsumer of this
        *     typed event channel. This operation is used by a push supplier
        *     before sending an event.
        *
        *@param interfaceRef interface of the typed event channel
        *@param pushSupplier The push supplier sending event to the channel.
        * A nil object reference may be passed to the connect_push_supplier
        * operation; if so a channel cannot invoke the disconnect_push_supplier
        * operation on the supplier; the supplier may be disconnected from the
        * channel without being informed.
        *@return Reference on the typed proxy push consumer for the given interfaceRef.
        *@exception CosTypedEventChannelAdmin::InterfaceNotSupported if interfaceRef
        * is not supported by the typed event service (IDL definition missing in the 
        * interface repository).
        *@exception CORBA::SystemException any CORBA system exception.
        *
        */
        template <class T> typename Cdmw::OrbSupport::object_traits<T>::_ptr_type
        get_typedPushConsumer(const char* interfaceRef, 
        CosEventComm::PushSupplier_ptr pushSupplier = CosEventComm::PushSupplier::_nil())  
        throw (CosTypedEventChannelAdmin::InterfaceNotSupported,
               CORBA::SystemException)
        {            
            return m_typedEventChannelInterface->get_typedPushConsumer<T>(interfaceRef, 
                                                                          pushSupplier);
        }

        /**
        * Purpose:
        * <p> This operation declares a push consumer to an event channel. After
        *     this, the consumer is able to receive event from the event channel.
        *
        *@param consumer The push consumer to be connected. 
        *@exception CosTypedEventChannelAdmin::NoSuchImplementation raised if no proxyPushSupplier
        * exist able to call operations in interface interfaceRef.
        *@exception CosEventChannelAdmin::TypeError raised if interfaceRef is not supported
        * by the push consumer. 
        *@exception CORBA::SystemException any CORBA system exception.
        *
        */
        void
        connect_typedPushConsumer(const char* interfaceRef, CosEventComm::PushConsumer_ptr consumer) 
            throw (CosTypedEventChannelAdmin::NoSuchImplementation, 
                   CosEventChannelAdmin::TypeError,
                   CORBA::SystemException);

        /**
        * Purpose:
        * <p> Connect the channel to another channel. This operation connect the
        *     channel to the specified channel. The specified channel becomes 
        *     the supplier of event of the current one.
        *
        *@param interfaceRef The interface. 
        *@param channelName The name of the channel to be connected to. 
        *
        *@exception CosTypedEventChannelAdmin::InterfaceNotSupported if interfaceRef
        * is not supported by the typed event service (IDL definition missing in the 
        * interface repository).
        *@exception CosTypedEventChannelAdmin::NoSuchImplementation raised if no proxyPushSupplier
        * exist able to call operations in interface interfaceRef.
        *@exception AlreadyConnectedException raised if the event channels are already connected.
        *@exception CosEventChannelAdmin::TypeError raised if interfaceRef is not supported
        * by the push consumer. 
        *@exception CORBA::SystemException any CORBA system exception.
        *
        */
        void
        connect_to_channel (const char* interfaceRef, const char* channelName)
        throw (CosTypedEventChannelAdmin::InterfaceNotSupported,
               CosTypedEventChannelAdmin::NoSuchImplementation, 
               CosEventChannelAdmin::AlreadyConnected, 
               CosEventChannelAdmin::TypeError,
               CORBA::SystemException);



    protected:

        /**
        * Purpose:
        * <p> Default Constructor is protected to avoid
        * needless checking of validity of internal event channel before
        * every single non-static function call
        * 
        *@exception CORBA::SystemException Doesn't throw but added for harmonisation
        * with other constructors
        */ 
        TypedEventChannel()
            throw(CORBA::SystemException);

    private:

        /**
        *[Attribute description]
        */ 
        // reference on the Cos EventChannel.
        TypedEventChannelInterface*  m_typedEventChannelInterface; 

        // channel name
        std::string m_channelName;

        // root context
        CommonSvcs::Naming::NamingInterface m_NamingInterface;


}; // End class TypedEventChannel 

}; // End namespace Event
}; // End namespace Cdmw
#endif // INCL_EVENT_TYPEDEVENTCHANNEL_HPP


