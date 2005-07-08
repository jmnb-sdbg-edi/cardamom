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


#ifndef INCL_EVENT_TYPEDEVENTCHANNELINTERFACE_HPP 
#define INCL_EVENT_TYPEDEVENTCHANNELINTERFACE_HPP 

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/ObjectTraits.hpp>

#include <Event/idllib/CosTypedEventComm.stub.hpp>
#include <Event/idllib/CosTypedEventChannelAdmin.stub.hpp>


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
*      Event which is compliant with OMG CORBA COS Event service. This
*      interface directly uses the CORBA COS typed event channel.
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
class TypedEventChannelInterface
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param nc An object reference of a Cos typed event channel.
        *
        *@exception CORBA::BAD_PARAM Using a nil event channel will raise a 
        * CORBA::BAD_PARAM system exception.
        *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
        * may throw CORBA system exceptions.
        */ 
        TypedEventChannelInterface(CosTypedEventChannelAdmin::TypedEventChannel_ptr eventChannel)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        *@param rhs TypedEventChannelInterface object to be copied.
        *
        *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
        * may throw CORBA system exceptions.
        */ 
        TypedEventChannelInterface(const TypedEventChannelInterface& rhs)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> Assignement operator. 
        *
        *@param rhs A TypedEventChannelInterface object to assign to current object.
        *
        *@return A reference to the current object.
        *
        *@exception CORBA::SystemException Because of a call to <I>_duplicate</I> that
        * may throw CORBA system exceptions.
        */         
        TypedEventChannelInterface&
        operator=(const TypedEventChannelInterface& rhs)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> Destructor. It releases internal reference of the Cos typed event channel.
        * 
        */ 
        virtual 
        ~TypedEventChannelInterface()
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
        * <p> This operation returns a reference on the ProxyPushConsumer of this
        *     typed event channel. This operation is used by a push supplier
        *     before sending an event.
        *
        *@param interfaceRef Interface repository id of the typed event channel
        *@param pushSupplier The push supplier sending event to the channel.
        * A nil object reference may be passed to the connect_push_supplier
        * operation; if so a channel cannot invoke the disconnect_push_supplier
        * operation on the supplier; the supplier may be disconnected from the
        * channel without being informed.
        *@return Reference on the typed proxy push consumer for the given interfaceRef.
        *@exception CosTypedEventChannelAdmin::InterfaceNotSupported if interfaceRef
        * is nor supported by the typed event service (IDL definition missing in the 
        * interface repository).
        *@exception CORBA::SystemException any CORBA system exception.
        *
        */
        template <class T> 
        typename Cdmw::OrbSupport::object_traits<T>::_ptr_type
        get_typedPushConsumer(const char* interfaceRef, 
                              CosEventComm::PushSupplier_ptr pushSupplier 
                                 = CosEventComm::PushSupplier::_nil())  
        throw (CosTypedEventChannelAdmin::InterfaceNotSupported,
               CORBA::SystemException);


        /**
        * Purpose:
        * <p> This operation declares a push consumer to an event channel. After
        *     this, the consumer is able to receive event from the event channel.
        *
        *@param interfaceRef Interface repository id of the typed event channel
        *@param consumer The push consumer to be connected. 
        *
        *@exception CosTypedEventChannelAdmin::NoSuchImplementation raised if no proxyPushSupplier
        * exist able to call operations in interface interfaceRef.
        *@exception CosEventChannelAdmin::TypeError raised if interfaceRef is not supported
        * by the push consumer. 
        *@exception CORBA::SystemException any CORBA system exception.
        *
        */
        void
        connect_typedPushConsumer(const char* interfaceRef, 
                                  CosEventComm::PushConsumer_ptr consumer) 
            throw (CosTypedEventChannelAdmin::NoSuchImplementation, 
                   CosEventChannelAdmin::TypeError,
                   CORBA::SystemException);

        /**
        * Purpose:
        * <p> Connect the channel to another channel. This operation connect the
        *     channel to the specified channel. The specified channel becomes 
        *     the supplier of event of the current one.
        *
        *@param interfaceRef Interface repository id of the typed event channel
        *@param eventChannelSupplier The reference of the channel to be connected to. 
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
        connect_to_channel(const char* interfaceRef,
                           CosTypedEventChannelAdmin::TypedEventChannel_ptr eventChannelSupplier)
        throw (CosTypedEventChannelAdmin::InterfaceNotSupported,
               CosTypedEventChannelAdmin::NoSuchImplementation, 
               CosEventChannelAdmin::AlreadyConnected, 
               CosEventChannelAdmin::TypeError,
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
        TypedEventChannelInterface() throw(CORBA::SystemException);


    private:

        /**
        * internal reference on the Cos typed event channel
        */ 
        CosTypedEventChannelAdmin::TypedEventChannel_var m_CosTypedEventChannel;


}; // End class TypedEventChannelInterface 

template <class T> 
typename Cdmw::OrbSupport::object_traits<T>::_ptr_type
TypedEventChannelInterface::get_typedPushConsumer(const char* interfaceRef, 
                                CosEventComm::PushSupplier_ptr pushSupplier)  
   throw (CosTypedEventChannelAdmin::InterfaceNotSupported,
          CORBA::SystemException)        
{
    using namespace Cdmw::OrbSupport;

    typedef Cdmw::OrbSupport::object_traits<T> OT;
    
    /**
    * An alias to a CORBA _var type of the object
    */
    typedef typename OT::_var_type _var_type;
    
    // proxy for consumer
    CosTypedEventChannelAdmin::TypedProxyPushConsumer_var proxyConsumer;
    CosTypedEventChannelAdmin::TypedSupplierAdmin_var supplierAdmin;
    CORBA::Object_var obj;
    _var_type typedConsumer;
    
    try 
    {
        supplierAdmin = m_CosTypedEventChannel->for_suppliers();
        
        if (CORBA::is_nil(supplierAdmin.in()))
        {
            throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
        }
        proxyConsumer = supplierAdmin->obtain_typed_push_consumer(interfaceRef);
        
        if (CORBA::is_nil(proxyConsumer.in()))
        {
            throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
        }
        proxyConsumer->connect_push_supplier(pushSupplier);
        
        obj = proxyConsumer->get_typed_consumer();
        
        typedConsumer = OT::_narrow(obj.in());
    }
    catch (const CosTypedEventChannelAdmin::InterfaceNotSupported& )
    {
        throw;
    }
    catch (const CosEventChannelAdmin::AlreadyConnected& )
    {
        throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
    }
    
    return typedConsumer._retn();
} // end template get_typedPushConsumer

}; // End namespace Event
}; // End namespace Cdmw
#endif // INCL_EVENT_TYPEDEVENTCHANNELINTERFACE_HPP


