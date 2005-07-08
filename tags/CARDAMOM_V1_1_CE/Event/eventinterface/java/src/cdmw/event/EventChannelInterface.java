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


package cdmw.event;

import cdmw.orbsupport.ExceptionMinorCodes;

import org.omg.CosEventComm.PushConsumer;
import org.omg.CosEventComm.PushSupplier;
import org.omg.CosEventChannelAdmin.SupplierAdmin;
import org.omg.CosEventChannelAdmin.ConsumerAdmin;
import org.omg.CosEventChannelAdmin.ProxyPushConsumer;
import org.omg.CosEventChannelAdmin.ProxyPushSupplier;

import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.SystemException;

/**
 * This class provides a simplified interface to connect and get push 
 * consumer to/from an untyped event channel. It internally uses ORBACUS
 * Event which is compliant with OMG CORBA COS Event service. This
 * interface directly uses the CORBA COS event channel.
 *
 * <br> See <a href="ftp://www.omg.org/pub/docs/formal/98-12-09.pdf">
 *    Object Management Group. 1998. CORBAservices: 
 *    Common Object Service Specification.</a> 
 */

public class EventChannelInterface {

    /**
     * internal reference on the Cos event channel
     */
    private org.omg.CosEventChannelAdmin.EventChannel cosEventChannel;

    /**
     * Constructor
     * 
     * @param eventChannel An object reference of a Cos event channel.
     *
     * @exception org.omg.CORBA.BAD_PARAM Using a nil event channel will raise 
     * a BAD_PARAM system exception.
     */ 
    public EventChannelInterface(org.omg.CosEventChannelAdmin.EventChannel eventChannel)
        throws SystemException {
        this.cosEventChannel = eventChannel;
        if (eventChannel==null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference,
                    CompletionStatus.COMPLETED_NO);
        }
    }

    /**
     * Copy constructor
     * 
     * @param rhs EventChannelInterface object to be copied.
     */ 
    public EventChannelInterface(EventChannelInterface rhs) {
        this.cosEventChannel = rhs.cosEventChannel;
    }

    /**
     * Default Constructor is protected to avoid
     * needless checking of validity of internal Cos event channel before
     * every single non-static function call
     */ 
    protected EventChannelInterface() {
        this.cosEventChannel = null;
    }

    /**
     * This operation destroys the current Eventchannel.
     * It unregisters event channel and calls destroy operation
     * on the Cos Event channel.
     */
    public void destroy() {
         // ask to Event service to destroy channel
        cosEventChannel.destroy();
    }

    /**
     * This operation returns a reference on a COS event channel.
     * This allows the user to easy get a reference of the Cos event
     * channel.
     *
     * @return Reference on the CosEventChannel
     */ 
    public org.omg.CosEventChannelAdmin.EventChannel getCosEventChannel() {
        return this.cosEventChannel;
    }

    /**
     * This operation returns a reference on the ProxyPushConsumer of this
     * generic event channel. This operation is used by a push supplier
     * before sending an event.
     *
     * @param pushSupplier The push supplier sending event to the channel.
     * A nil object reference may be passed to the connect_push_supplier
     * operation; if so a channel cannot invoke the disconnect_push_supplier
     * operation on the supplier; the supplier may be disconnected from the
     * channel without being informed.
     * @return Reference on the proxy push consumer.
     * @exception SystemException any CORBA system exception.
     */
    public PushConsumer getPushConsumer(PushSupplier pushSupplier)
        throws SystemException {
        
         // proxy for consumer
        ProxyPushConsumer proxyConsumer = null;
        SupplierAdmin supplierAdmin = null;

        try {
            supplierAdmin = this.cosEventChannel.for_suppliers();

            if (supplierAdmin==null) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL,
                        CompletionStatus.COMPLETED_NO);
            }
            proxyConsumer = supplierAdmin.obtain_push_consumer();

            if (proxyConsumer==null) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL,
                        CompletionStatus.COMPLETED_NO);
            }
            proxyConsumer.connect_push_supplier(pushSupplier);
        } catch (org.omg.CosEventChannelAdmin.AlreadyConnected ac) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,
                    CompletionStatus.COMPLETED_NO);
        }

        return proxyConsumer;
        
    }

    /**
     * This operation declares a push consumer to an event channel. After
     * this, the consumer is able to receive event from the event channel.
     *
     * @param consumer The push consumer which receives event messages. 
     * @exception SystemException any CORBA system exception.
     * @return The ProxyPushSupplier the consumer is connected to
     */
    public ProxyPushSupplier connectPushConsumer(PushConsumer consumer)
        throws SystemException {
        
         // Export proxy for supplier
        ConsumerAdmin consumerAdmin = null;
        ProxyPushSupplier proxySupplier = null;

        try {
            consumerAdmin = this.cosEventChannel.for_consumers();

            if (consumerAdmin==null) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL,
                        CompletionStatus.COMPLETED_NO);
            }

            proxySupplier = consumerAdmin.obtain_push_supplier();

            if (proxySupplier==null) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL,
                        CompletionStatus.COMPLETED_NO);
            }

            proxySupplier.connect_push_consumer(consumer);
        } catch (org.omg.CosEventChannelAdmin.AlreadyConnected ar) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CosEventChannelAdmin.TypeError te) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, CompletionStatus.COMPLETED_NO);
        }
        
        return proxySupplier;

    }

    /**
     * Connect the channel to another channel. This operation connect the
     * channel to the specified channel. The specified channel becomes 
     * the supplier of event of the current one.
     *
     * @param eventChannelSupplier The reference of the channel to be connected to.
     * @exception org.omg.CosEventChannelAdmin.AlreadyConnected raised 
     * if event channels are already connected.
     * @exception SystemException any CORBA system exception.
     */
    public void
    connectToChannel(org.omg.CosEventChannelAdmin.EventChannel eventChannelSupplier)
    throws org.omg.CosEventChannelAdmin.AlreadyConnected, SystemException {

        if (eventChannelSupplier==null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidChannelName, 
                    CompletionStatus.COMPLETED_NO);
        }

        // proxy for consumer
        ProxyPushConsumer proxyConsumer = null;
        SupplierAdmin supplierAdmin = this.cosEventChannel.for_suppliers();

        if (supplierAdmin==null) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,CompletionStatus.COMPLETED_NO);
        }

        proxyConsumer = supplierAdmin.obtain_push_consumer();

        if (proxyConsumer==null) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,CompletionStatus.COMPLETED_NO);
        }

        // Export proxy for supplier
        ConsumerAdmin consumerAdmin = eventChannelSupplier.for_consumers();
        ProxyPushSupplier proxySupplier = consumerAdmin.obtain_push_supplier();

        if (proxySupplier==null) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, CompletionStatus.COMPLETED_NO);
        }

        // Connect the proxy. If we're already connected we'll get an
        // exception. This should only happen if we're reusing the
        // same proxy-object.
        try {
            proxySupplier.connect_push_consumer(proxyConsumer);

            // connect consumer to supplier
            // 
            // Dead_lock with : proxyConsumer.connect_push_supplier(proxySupplier);
            proxyConsumer.connect_push_supplier(null);
        } catch (org.omg.CosEventChannelAdmin.TypeError te) {
            // exception added in COS EVENT for typed channel only!
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, CompletionStatus.COMPLETED_NO);
        }
    
    }
            
}


