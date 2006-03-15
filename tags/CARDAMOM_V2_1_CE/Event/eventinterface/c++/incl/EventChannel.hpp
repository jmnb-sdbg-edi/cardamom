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


#ifndef INCL_EVENT_EVENTCHANNEL_HPP 
#define INCL_EVENT_EVENTCHANNEL_HPP 

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/common/Exception.hpp> 
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

#include <Event/eventinterface/EventChannelInterface.hpp>

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
*      consumer to/from an untyped event channel. It internally uses ORBACUS
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
class EventChannel 
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
        EventChannel(const char* channelName, CosNaming::NamingContext_ptr ctx)
            throw(CORBA::SystemException);
        
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        *@param rhs EventChannel object to be copied.
        *
        *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
        * may throw CORBA system exceptions.
        */ 
        EventChannel(const EventChannel& rhs)
            throw(CORBA::SystemException);
                
        /**
        * Purpose:
        * <p> Assignement operator. 
        *
        *@param rhs A EventChannel object to assign to current object.
        *
        *@return A reference to the current object.
        *
        *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
        * may throw CORBA system exceptions.
        */         
        EventChannel&
        operator=(const EventChannel& rhs)
            throw(CORBA::SystemException);
        
        
        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~EventChannel()
            throw();

        /**
        * Purpose:
        * <p> This operation returns a reference on a COS generic event channel.
        *     This allows the user to easy get a reference of the Cos generic event
        *     channel.
        *
        *@return Reference on the CosEventChannel
        */ 
        CosEventChannelAdmin::EventChannel_ptr
        get_CosEventChannel()
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
        *     generic event channel. This operation is used by a push supplier
        *     before sending an event.
        *
        *@param pushSupplier The push supplier sending event to the channel.
        * A nil object reference may be passed to the connect_push_supplier
        * operation; if so a channel cannot invoke the disconnect_push_supplier
        * operation on the supplier; the supplier may be disconnected from the
        * channel without being informed.
        *@return Reference on the proxy push consumer.
        *@exception CORBA::SystemException any CORBA system exception.
        */
        CosEventComm::PushConsumer_ptr 
            get_pushConsumer(CosEventComm::PushSupplier_ptr pushSupplier 
                                 = CosEventComm::PushSupplier::_nil())
            throw (CORBA::SystemException);

        /**
        * Purpose:
        * <p> This operation declares a push consumer to an event channel. After
        *     this, the consumer is able to receive event from the event channel.
        *
        *@param consumer The push consumer which receives event messages. 
        *@exception CORBA::SystemException any CORBA system exception.
        *
        */
        void
        connect_pushConsumer (CosEventComm::PushConsumer_ptr  consumer)
            throw (CORBA::SystemException);

        /**
        * Purpose:
        * <p> Connect the channel to another channel. This operation connect the
        *     channel to the specified channel. The specified channel becomes 
        *     the supplier of event of the current one.
        *
        *@param channelName The name of the channel to be connected to. 
        *@exception CosEventChannelAdmin::AlreadyConnected raised if event channels are already
        * connected.
        *@exception CORBA::SystemException any CORBA system exception.
        */
        void
        connect_to_channel (const char* channelName)
        throw (CosEventChannelAdmin::AlreadyConnected, 
               CORBA::SystemException);


    protected:
        /**
        * Purpose:
        * <p> Default Constructor is protected to avoid
        * needless checking of validity of internal Cos event channel before
        * every single non-static function call
        * 
        *@exception CORBA::SystemException Doesn't throw but added for harmonisation
        * with other constructors
        */ 
        EventChannel()
            throw(CORBA::SystemException);

    private:
        
        /**
        *[Attribute description]
        */        
        // reference on the Cos EventChannel.
        EventChannelInterface*  m_eventChannelInterface; 

        // channel name
        std::string m_channelName;

        // root context
        CommonSvcs::Naming::NamingInterface m_NamingInterface;

}; // End class EventChannel 

}; // End namespace Event
}; // End namespace Cdmw
#endif // INCL_EVENT_EVENTCHANNEL_HPP


