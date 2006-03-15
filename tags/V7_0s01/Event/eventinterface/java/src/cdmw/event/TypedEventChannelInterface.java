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


package cdmw.event;

import cdmw.orbsupport.ExceptionMinorCodes;

import org.omg.CORBA.SystemException;
import org.omg.CORBA.CompletionStatus;

/**
 * This class provides a simplified interface to connect and get push 
 * consumer to/from a typed event channel. It internally uses ORBACUS
 * Event which is compliant with OMG CORBA COS Event service. This
 * interface directly uses the CORBA COS typed event channel.
 *
 * <br> See <a href="ftp://www.omg.org/pub/docs/formal/98-12-09.pdf">
 *    Object Management Group. 1998. CORBAservices: 
 *    Common Object Service Specification.</a>
 *
 */

public class TypedEventChannelInterface {
    
    /**
     * internal reference on the Cos typed event channel
     */ 
    private org.omg.CosTypedEventChannelAdmin.TypedEventChannel cosTypedEventChannel;
    
    /**
     * Constructor
     * 
     * @param nc An object reference of a Cos typed event channel.
     *
     * @exception BAD_PARAM Using a nil event channel will raise a 
     * BAD_PARAM system exception.
     */ 
    public TypedEventChannelInterface(
        org.omg.CosTypedEventChannelAdmin.TypedEventChannel eventChannel)
        throws SystemException {
        
        this.cosTypedEventChannel= eventChannel;
        if (eventChannel==null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference,
                    CompletionStatus.COMPLETED_NO);
        }
        
    }

    /**
     * Copy constructor
     * 
     * @param rhs TypedEventChannelInterface object to be copied.
     */ 
    public TypedEventChannelInterface(TypedEventChannelInterface rhs) {
        this.cosTypedEventChannel = rhs.cosTypedEventChannel;        
    }
    
    /**
     * Default Constructor is protected to avoid
     * needless checking of validity of internal Cos event channel before
     * every single non-static function call
     */ 
    protected TypedEventChannelInterface() {
        this.cosTypedEventChannel = null;
    }

    /**
     * This operation destroys the current Eventchannel.
     * It unregisters event channel and calls destroy operation
     * on the Cos Event channel.
     *
     * @exception SystemException any CORBA system exception.
     */
    public void destroy() {
        // ask to Event service to destroy channel
        cosTypedEventChannel.destroy();
    }

    /**
     * This operation returns a reference on a COS Typed event channel.
     * This allows the user to easy get a reference of the Cos Typed event
     * channel.
     *
     * @return Reference on the CosTypedEventChannel
     */ 
    public org.omg.CosTypedEventChannelAdmin.TypedEventChannel 
        getCosTypedEventChannel() {
        return this.cosTypedEventChannel;
    }

    /**
     * This operation returns a reference on the ProxyPushConsumer of this
     * typed event channel. This operation is used by a push supplier
     * before sending an event.
     *
     * @param interfaceRef Interface repository id of the typed event channel
     * @param pushSupplier The push supplier sending event to the channel.
     * A nil object reference may be passed to the connect_push_supplier
     * operation; if so a channel cannot invoke the disconnect_push_supplier
     * operation on the supplier; the supplier may be disconnected from the
     * channel without being informed.
     * @return Reference on the typed proxy push consumer for the given interfaceRef.
     * @exception org.omg.CosTypedEventChannelAdmin.InterfaceNotSupported 
     * if interfaceRef is nor supported by the typed event service 
     * (IDL definition missing in the interface repository).
     * @exception SystemException any CORBA system exception.
     */
    public org.omg.CORBA.Object    getTypedPushConsumer(
        String interfaceRef, org.omg.CosEventComm.PushSupplier pushSupplier) 
        throws  org.omg.CosTypedEventChannelAdmin.InterfaceNotSupported,
                   SystemException {

        // proxy for consumer
        org.omg.CosTypedEventChannelAdmin.TypedProxyPushConsumer proxyConsumer = null;
        org.omg.CosTypedEventChannelAdmin.TypedSupplierAdmin supplierAdmin = null;
        org.omg.CORBA.Object obj;

        try {
            supplierAdmin = cosTypedEventChannel.for_suppliers();

            if (supplierAdmin==null) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL, 
                        CompletionStatus.COMPLETED_NO);
            }
            proxyConsumer = supplierAdmin.obtain_typed_push_consumer(interfaceRef);

            if (proxyConsumer==null) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL, 
                        CompletionStatus.COMPLETED_NO);
            }
            proxyConsumer.connect_push_supplier(pushSupplier);
            obj = proxyConsumer.get_typed_consumer();

        } catch (org.omg.CosEventChannelAdmin.AlreadyConnected ac) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, 
                    CompletionStatus.COMPLETED_NO);
        }
        
        return obj;
           
    }

    /**
     * This operation declares a push consumer to an event channel. After
     * this, the consumer is able to receive event from the event channel.
     *
     * @param interfaceRef Interface repository id of the typed event channel
     * @param consumer The push consumer to be connected. 
     *
     * @exception org.omg.CosTypedEventChannelAdmin.NoSuchImplementation raised if no proxyPushSupplier
     * exist able to call operations in interface interfaceRef.
     * @exception org.omg.CosEventChannelAdmin.TypeError raised 
     * if interfaceRef is not supported by the push consumer.
     * @exception SystemException any CORBA system exception.
     */
    public void connectTypedPushConsumer(String interfaceRef, 
        org.omg.CosEventComm.PushConsumer consumer) 
        throws org.omg.CosTypedEventChannelAdmin.NoSuchImplementation, 
               org.omg.CosEventChannelAdmin.TypeError,
               SystemException {
        
        // Export proxy for supplier
        org.omg.CosTypedEventChannelAdmin.TypedConsumerAdmin consumerAdmin = 
            cosTypedEventChannel.for_consumers();

        org.omg.CosEventChannelAdmin.ProxyPushSupplier proxySupplier = null;

        proxySupplier = consumerAdmin.obtain_typed_push_supplier(interfaceRef);

        if (consumer==null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference,
                    CompletionStatus.COMPLETED_NO);
        }

        try {
            proxySupplier.connect_push_consumer(consumer);
        } catch (org.omg.CosEventChannelAdmin.AlreadyConnected ac) {
            throw new org.omg.CORBA.INTERNAL(ExceptionMinorCodes.INTERNAL,
                CompletionStatus.COMPLETED_NO);
        }
        
    }

    /**
     * Connect the channel to another channel. This operation connect the
     * channel to the specified channel. The specified channel becomes 
     * the supplier of event of the current one.
     *
     * @param interfaceRef Interface repository id of the typed event channel
     * @param eventChannelSupplier The reference of the channel to be connected to. 
     *
     * @exception org.omg.CosTypedEventChannelAdmin.InterfaceNotSupported 
     * if interfaceRef is not supported by the typed event service 
     * (IDL definition missing in the interface repository).
     * @exception org.omg.CosTypedEventChannelAdmin.NoSuchImplementation raised 
     * if no proxyPushSupplier exist able to call operations 
     * in interface interfaceRef.
     * @exception org.omg.CosEventChannelAdmin.AlreadyConnected raised 
     * if the event channels are already connected.
     * @exception org.omg.CosEventChannelAdmin.TypeError raised 
     * if interfaceRef is not supported by the push consumer.
     * @exception SystemException any CORBA system exception.
     */
    public void connectToChannel(String interfaceRef,
        org.omg.CosTypedEventChannelAdmin.TypedEventChannel eventChannelSupplier)
    throws org.omg.CosTypedEventChannelAdmin.InterfaceNotSupported,
           org.omg.CosTypedEventChannelAdmin.NoSuchImplementation, 
           org.omg.CosEventChannelAdmin.AlreadyConnected, 
           org.omg.CosEventChannelAdmin.TypeError,
           SystemException {

        // proxy for consumer
        org.omg.CosTypedEventChannelAdmin.TypedProxyPushConsumer proxyConsumer = null;
        org.omg.CosTypedEventChannelAdmin.TypedSupplierAdmin supplierAdmin = 
            cosTypedEventChannel.for_suppliers();

        if (supplierAdmin==null) {
            throw new org.omg.CORBA.INTERNAL(ExceptionMinorCodes.INTERNAL,
                CompletionStatus.COMPLETED_NO);
        }

        proxyConsumer = supplierAdmin.obtain_typed_push_consumer(interfaceRef);

        if (proxyConsumer==null) {
            throw new org.omg.CORBA.INTERNAL(ExceptionMinorCodes.INTERNAL,
                CompletionStatus.COMPLETED_NO);
        }

        // Export proxy for supplier
        org.omg.CosTypedEventChannelAdmin.TypedConsumerAdmin consumerAdmin = 
            eventChannelSupplier.for_consumers();
        org.omg.CosEventChannelAdmin.ProxyPushSupplier proxySupplier = null;

        if (consumerAdmin==null) {
            throw new org.omg.CORBA.INTERNAL(ExceptionMinorCodes.INTERNAL,
                CompletionStatus.COMPLETED_NO);
        }

        proxySupplier = consumerAdmin.obtain_typed_push_supplier(interfaceRef);

        // Connect the proxy. If we're already connected we'll get an
        // exception. This should only happen if we're reusing the
        // same proxy-object.
        org.omg.CORBA.Object obj = proxyConsumer.get_typed_consumer();

        if (obj==null) {
            throw new org.omg.CORBA.INTERNAL(ExceptionMinorCodes.INTERNAL,
                CompletionStatus.COMPLETED_NO);
        }

        org.omg.CosEventComm.PushConsumer typedConsumer =
            org.omg.CosEventComm.PushConsumerHelper.narrow(obj);

        if (typedConsumer==null) {
            throw new org.omg.CORBA.INTERNAL(ExceptionMinorCodes.INTERNAL,
                CompletionStatus.COMPLETED_NO);
        }

        proxySupplier.connect_push_consumer(typedConsumer);

        // connect consumer to supplier
        proxyConsumer.connect_push_supplier(proxySupplier);
           
    }

}


