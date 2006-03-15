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


#ifndef INCL_EVENT_EVENTCHANNELINTERFACE_HPP 
#define INCL_EVENT_EVENTCHANNELINTERFACE_HPP 

#include <Foundation/orbsupport/CORBA.hpp>

#include <Event/idllib/CosEventComm.stub.hpp>
#include <Event/idllib/CosEventChannelAdmin.stub.hpp>


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
*      Event which is compliant with OMG CORBA COS Event service. This
*      interface directly uses the CORBA COS event channel.
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
class EventChannelInterface 
{

    public:
        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param nc An object reference of a Cos event channel.
        *
        *@exception CORBA::BAD_PARAM Using a nil event channel will raise a 
        * CORBA::BAD_PARAM system exception.
        *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
        * may throw CORBA system exceptions.
        */ 
        EventChannelInterface(CosEventChannelAdmin::EventChannel_ptr eventChannel)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        *@param rhs EventChannelInterface object to be copied.
        *
        *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
        * may throw CORBA system exceptions.
        */ 
        EventChannelInterface(const EventChannelInterface& rhs)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> Assignement operator. 
        *
        *@param rhs A EventChannelInterface object to assign to current object.
        *
        *@return A reference to the current object.
        *
        *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
        * may throw CORBA system exceptions.
        */         
        EventChannelInterface&
        operator=(const EventChannelInterface& rhs)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> Destructor. It releases internal reference of the Cos event channel.
        * 
        */ 
        virtual 
        ~EventChannelInterface()
            throw();

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
        * <p> This operation returns a reference on a COS event channel.
        *     This allows the user to easy get a reference of the Cos event
        *     channel.
        *
        *@return Reference on the CosEventChannel
        */ 
        CosEventChannelAdmin::EventChannel_ptr
        get_CosEventChannel()
            throw ();

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
        *@return CosEventComm::PushSupplier The proxy push suppplier needed to ask for disconnection
        *@exception CORBA::SystemException any CORBA system exception.
        *
        */
        CosEventComm::PushSupplier_ptr
        connect_pushConsumer (CosEventComm::PushConsumer_ptr  consumer)
            throw (CORBA::SystemException);

        /**
        * Purpose:
        * <p> Connect the channel to another channel. This operation connect the
        *     channel to the specified channel. The specified channel becomes 
        *     the supplier of event of the current one.
        *
        *@param eventChannelSupplier The reference of the channel to be connected to. 
        *@return void
        *@exception CosEventChannelAdmin::AlreadyConnected raised if event channels are already
        * connected.
        *@exception CORBA::SystemException any CORBA system exception.
        */
        void
        connect_to_channel(CosEventChannelAdmin::EventChannel_ptr eventChannelSupplier)
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
        EventChannelInterface() throw(CORBA::SystemException);



    private:

        // internal reference on the Cos event channel
        CosEventChannelAdmin::EventChannel_var m_CosEventChannel;
            
}; // End class EventChannelInterface

}; // End namespace Event
}; // End namespace Cdmw
#endif // INCL_EVENT_EVENTCHANNELINTERFACE_HPP


