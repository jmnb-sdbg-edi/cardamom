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
import cdmw.namingandrepository.NamingInterface;

import org.omg.CosEventComm.PushConsumer;
import org.omg.CosEventComm.PushSupplier;
import org.omg.CosNaming.NamingContext;

import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.SystemException;

/**
 * This class provides a simplified interface to connect and get push 
 * consumer to/from an untyped event channel. It internally uses ORBACUS
 * Event which is compliant with OMG CORBA COS Event service. In this 
 * interface, event channel are defined by a name and a naming context.
 *
 * <br> See <a href="ftp://www.omg.org/pub/docs/formal/98-12-09.pdf">
 *    Object Management Group. 1998. CORBAservices: 
 *    Common Object Service Specification.</a>
 *
 */

public class EventChannel {

    /**
     * reference on the Cos EventChannel.
     */
    private EventChannelInterface eventChannelInterface; 

    /**
     * channel name
     */ 
    private String channelName;

    /**
     * root context
     */  
    private NamingInterface namingInterface;

    /**
     * Constructor 
     * 
     * @param channelName Name of the event channel.
     * @param ctx NamingContext where channelName is registered.
     * @exception SystemException org.omg.CORBA.BAD_PARAM if 
     * channel name is invalid or not found or context is nil.
     * org.omg.CORBA.INTERNAL for internal error and any other system
     * exception.
     */ 
    public EventChannel(String channelName, NamingContext ctx)
        throws SystemException {
        
        this.channelName = channelName;
        this.namingInterface = new NamingInterface(ctx);
         // Get a reference on the Cos EventChannel
        org.omg.CosEventChannelAdmin.EventChannel cosEventChannel = null;
        try  {
            org.omg.CORBA.Object obj = namingInterface.resolve(channelName);
            cosEventChannel = 
                org.omg.CosEventChannelAdmin.EventChannelHelper.narrow(obj);
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidChannelName,
                    CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidChannelName,
                    CompletionStatus.COMPLETED_NO);
        }

        if (cosEventChannel==null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidChannelName, 
                    CompletionStatus.COMPLETED_NO);
        }

        this.eventChannelInterface = new EventChannelInterface(cosEventChannel);
           
           // initialisation done: everything is OK
        
        
    }

    /**
     * Copy constructor
     * 
     * @param rhs EventChannel object to be copied.
     */ 
    public EventChannel(EventChannel rhs) {
        
        this.channelName = rhs.channelName;
        this.namingInterface = rhs.namingInterface;
        this.eventChannelInterface = 
            new EventChannelInterface(
                rhs.eventChannelInterface.getCosEventChannel());
        
    }
        
    /**
     * Default Constructor is protected to avoid
     * needless checking of validity of internal Cos event channel before
     * every single non-static function call
     */ 
    protected EventChannel() {
        this.channelName = null;
        this.namingInterface = null;
        this.eventChannelInterface = null;
    }

    /**
     * This operation returns a reference on a COS generic event channel.
     * This allows the user to easy get a reference of the Cos generic event
     * channel.
     *
     * @return Reference on the CosEventChannel
     */ 
    public org.omg.CosEventChannelAdmin.EventChannel getCosEventChannel() {
        return this.eventChannelInterface.getCosEventChannel();
    }

    /**
     * This operation destroys the current Eventchannel.
     * It unregisters event channel and calls destroy operation
     * on the Cos Event channel.
     */
    public void destroy() {    
        // ask to Event service to destroy channel
        eventChannelInterface.destroy();
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
        return this.eventChannelInterface.getPushConsumer(pushSupplier);
    }

    /**
     * This operation declares a push consumer to an event channel. After
     * this, the consumer is able to receive event from the event channel.
     *
     * @param consumer The push consumer which receives event messages. 
     * @exception SystemException any CORBA system exception.
     */
    public void connectPushConsumer (PushConsumer consumer)
        throws SystemException {
        this.eventChannelInterface.connectPushConsumer(consumer);
    }

    /**
     * Connect the channel to another channel. This operation connect the
     * channel to the specified channel. The specified channel becomes 
     * the supplier of event of the current one.
     *
     * @param channelName The name of the channel to be connected to. 
     * @exception org.omg.CosEventChannelAdmin.AlreadyConnected raised 
     * if event channels are already connected.
     * @exception SystemException any CORBA system exception.
     */
    public void connectToChannel(String channelName)
        throws org.omg.CosEventChannelAdmin.AlreadyConnected, SystemException {
        
        // Get the channel object to which we want to connect.
        org.omg.CosEventChannelAdmin.EventChannel supplierEventChannel = null;
        try {
            org.omg.CORBA.Object obj = namingInterface.resolve(channelName);
            supplierEventChannel = 
                org.omg.CosEventChannelAdmin.EventChannelHelper.narrow(obj);
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidChannelName,
                    CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
             throw new org.omg.CORBA.INTERNAL(
                 ExceptionMinorCodes.INTERNAL,CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidChannelName, 
                    CompletionStatus.COMPLETED_NO);
        }

        if (supplierEventChannel==null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidChannelName,
                    CompletionStatus.COMPLETED_NO);
        }
            
        eventChannelInterface.connectToChannel(supplierEventChannel);
        
    }

}


