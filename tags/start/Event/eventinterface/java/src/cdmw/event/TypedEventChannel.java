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

import cdmw.namingandrepository.NamingInterface;
import cdmw.orbsupport.ExceptionMinorCodes;

import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.SystemException;

/**
 * This class provides a simplified interface to connect and get push 
 * consumer to/from a typed event channel. It internally uses ORBACUS
 * Event which is compliant with OMG CORBA COS Event service. In this 
 * interface, event channel are defined by a name and a naming context.
 *
 * <br> See <a href="ftp://www.omg.org/pub/docs/formal/98-12-09.pdf">
 *    Object Management Group. 1998. CORBAservices: 
 *    Common Object Service Specification.</a>
 *
 */

public class TypedEventChannel {

    /**
     * reference on the Cos EventChannel.
     */  
    private TypedEventChannelInterface typedEventChannelInterface; 

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
     * @exception org.omg.CORBA.BAD_PARAM if channel name 
     * is invalid or not found or context is nil.
     * org.omg.CORBA.INTERNAL for internal error and any other system
     * exception.
     */ 
    public TypedEventChannel(String channelName, 
                      org.omg.CosNaming.NamingContext ctx)
        throws SystemException {
        
        this.channelName = channelName;
        this.namingInterface = new NamingInterface(ctx);
        
        // Get a reference on the Cos EventChannel
        org.omg.CosTypedEventChannelAdmin.TypedEventChannel cosTypedEventChannel;
        try {
            org.omg.CORBA.Object obj = namingInterface.resolve(channelName);
            cosTypedEventChannel = 
                org.omg.CosTypedEventChannelAdmin.TypedEventChannelHelper.narrow(obj);
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

        this.typedEventChannelInterface = 
            new TypedEventChannelInterface(cosTypedEventChannel);

        // initialisation done: everything is OK
        
    }

    /**
     * Copy constructor
     * 
     * @param rhs TypedEventChannel object to be copied.
     */ 
    public TypedEventChannel(TypedEventChannel rhs) {
        
        this.channelName = rhs.channelName;
        this.namingInterface = rhs.namingInterface;
        this.typedEventChannelInterface = 
            new TypedEventChannelInterface(
                rhs.typedEventChannelInterface.getCosTypedEventChannel());
        
    }

    /**
     * Default Constructor is protected to avoid
     * needless checking of validity of internal event channel before
     * every single non-static function call
     */ 
    protected TypedEventChannel() throws SystemException {
        this.channelName = null;
        this.namingInterface = null;
        this.typedEventChannelInterface = null;
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
        return this.typedEventChannelInterface.getCosTypedEventChannel();
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
        this.typedEventChannelInterface.destroy();
    }

    /**
     * This operation returns a reference on the ProxyPushConsumer of this
     * typed event channel. This operation is used by a push supplier
     * before sending an event.
     *
     * @param interfaceRef interface of the typed event channel
     * @param pushSupplier The push supplier sending event to the channel.
     * A nil object reference may be passed to the connect_push_supplier
     * operation; if so a channel cannot invoke the disconnect_push_supplier
     * operation on the supplier; the supplier may be disconnected from the
     * channel without being informed.
     * @return Reference on the typed proxy push consumer for the given interfaceRef.
     * @exception org.omg.CosTypedEventChannelAdmin.InterfaceNotSupported 
     * if interfaceRef is not supported by the typed event service 
     * (IDL definition missing in the interface repository).
     * @exception SystemException any CORBA system exception.
     */
    public org.omg.CORBA.Object getTypedPushConsumer(String interfaceRef, 
        org.omg.CosEventComm.PushSupplier pushSupplier)  
        throws org.omg.CosTypedEventChannelAdmin.InterfaceNotSupported,
        SystemException {
        
        return typedEventChannelInterface.getTypedPushConsumer(interfaceRef, 
                                                                pushSupplier);
    }

    /**
     * This operation declares a push consumer to an event channel. After
     * this, the consumer is able to receive event from the event channel.
     *
     * @param consumer The push consumer to be connected. 
     * @exception org.omg.CosTypedEventChannelAdmin.NoSuchImplementation raised 
     * if no proxyPushSupplier exist able to call operations 
     * in interface interfaceRef.
     * @exception org.omg.CosEventChannelAdmin.TypeError raised 
     * if interfaceRef is not supported by the push consumer. 
     * @exception SystemException any CORBA system exception.
     */
    public void
    connectTypedPushConsumer(String interfaceRef, 
        org.omg.CosEventComm.PushConsumer consumer) 
        throws org.omg.CosTypedEventChannelAdmin.NoSuchImplementation, 
              org.omg.CosEventChannelAdmin.TypeError,
              SystemException {
               
        this.typedEventChannelInterface.
                connectTypedPushConsumer(interfaceRef, consumer);
        
    }

    /**
     * Connect the channel to another channel. This operation connect the
     * channel to the specified channel. The specified channel becomes 
     * the supplier of event of the current one.
     *
     * @param interfaceRef The interface. 
     * @param channelName The name of the channel to be connected to. 
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
    public void connectToChannel(String interfaceRef, String channelName)
    throws org.omg.CosTypedEventChannelAdmin.InterfaceNotSupported,
           org.omg.CosTypedEventChannelAdmin.NoSuchImplementation, 
           org.omg.CosEventChannelAdmin.AlreadyConnected, 
           org.omg.CosEventChannelAdmin.TypeError,
           SystemException {

        // Get the channel object to which we want to connect.
        org.omg.CosTypedEventChannelAdmin.TypedEventChannel supplierEventChannel;
        try {
            org.omg.CORBA.Object obj = namingInterface.resolve(channelName);
            supplierEventChannel = 
                org.omg.CosTypedEventChannelAdmin.
                    TypedEventChannelHelper.narrow(obj);
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

        typedEventChannelInterface.connectToChannel(interfaceRef,
                                                 supplierEventChannel);
           
    }

}


