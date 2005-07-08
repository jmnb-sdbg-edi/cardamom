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

import cdmw.common.Assert;
import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;
import cdmw.event.TypedEventChannelInterface;

import cdmw.lifecycle.POAThreadingPolicy;
import cdmw.lifecycle.ObjectRegistrationGuard;

import com.thalesgroup.CdmwNamingAndRepository.NameDomain;

import org.omg.CORBA.ORB;
import org.omg.CORBA.Any;
import org.omg.CORBA.Policy;
import org.omg.CORBA.SystemException;
import org.omg.PortableServer.POA;

/**
 * Implementation of the TypedEventChannelFactory interface.
 *
 */
public class OrbacusTypedEventChannelFactoryImpl 
    extends com.thalesgroup.CdmwEvent.TypedEventChannelFactoryPOA {
    
    /**
     * Maximum number of retries on receiving TRANSIENT exception
     * for registering objects.
     */
    public static final int MAX_RETRIES = 1;

    /**
     * Name of the default typed event channel factory
     */
    public static final String DEFAULT_TYPED_EVENT_CHANNEL_FACTORY 
        = "DefaultTypedEventChannelFactory";

    /**
     * Supported interface
     */
    public static final String TYPED_EVT_CHANNEL_REP_ID 
        = "IDL:omg.org/CosEventChannelAdmin/TypedEventChannel:1.0";

    /**
     * Repository ID of the PushSupplier object
     */
    public static final String PUSH_SUPPLIER_REP_ID 
        = "IDL:omg.org/CosEventComm/PushSupplier:1.0";

    /**
     * Empty String instance
     */
    public static final String EMPTY_STRING = "";

    /**
     * A reference to the ORB pseudo-object
     */ 
    private ORB orb;
    
    /**
     * Parent POA for the factory
     */
    private POA poa;
    
    /**
     * Object reference of the name domain where the factory will register
     */
    private NameDomain domain;

    /**
     * The fully qualified name of the NameDomain
     */
    private String domainName;
    /**
     * Fully qualified name of the CDMW factory
     */
    private String factoryName;
    
    /**
     * Persistent POA of notifier
     */
    private POA notifierPOA;
    
    // ----------------------------------------------------------------------
    // Event Channel destruction notifier
    // ----------------------------------------------------------------------
    class EventChannelDestructionNotifier 
        extends org.omg.CosEventComm.PushSupplierPOA {
    
        /**
         * the POA
         */
        private POA poa;

        /**
         * domain where event channels are registered
         */
        private NameDomain domain;

        /**
         * channel which has been destroyed
         */
        private String channelName;
    
        public EventChannelDestructionNotifier(POA poa, NameDomain nameDomain,String channelName)
            throws SystemException {
            
            this.poa = poa;
            this.domain = nameDomain;
            this.channelName = channelName;
            System.out.println("EventChannelDestructionNotifier construction");
            
        }

        /**
         * Override _default_POA
         *
         * @return POA
         */
        public POA _default_POA() {
            return this.poa;
        }

        public void disconnect_push_supplier() throws SystemException {
            
            Assert.check(domain!=null);

            try {
                System.out.println("EventChannelDestructionNotifier disconnect order");
                domain.release_name(channelName);
            } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NotRegistered nr) {
                // This should not happen 
                System.out.println("EventChannelDestructionNotifier: NameDomain not registered !");
            } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
                // This should not happen either 
                System.out.println("EventChannelDestructionNotifier: InvalidName !");
            } catch (SystemException se) {
                System.out.println("EventChannelDestructionNotifier: " + se.getMessage() );
            }
        }

    }
    
    // ----------------------------------------------------------------------
    // Resource Servant activator
    // ----------------------------------------------------------------------
    class ServantLocatorImpl 
        extends org.omg.PortableServer.ServantLocatorPOA {
    
        /**
         * domain where event channels are registered
         */
        private NameDomain domain;

        public ServantLocatorImpl(NameDomain nameDomain) {
            this.domain = nameDomain;
        }

        public org.omg.PortableServer.Servant 
        preinvoke(byte[] oid,
                  POA poa,
                  String operation,
                  org.omg.PortableServer.ServantLocatorPackage.CookieHolder theCookie)
            throws SystemException {    
            
            // TODO: Begin for test only
            String s = new String(oid);
            System.out.println("\t[EVENT]===> Pre-invoking " + s);

            return new EventChannelDestructionNotifier(poa, domain, s);
        }

        public void 
        postinvoke(byte[] oid,
                   POA poa,
                   String operation,
                   Object obj,
                   org.omg.PortableServer.Servant theServant)
            throws SystemException {
            
            // TODO: Begin for test only
            String s = new String(oid);
            System.out.println("\t[EVENT]===> Post-invoking " + s);

        }

    }
    

    /**
     * Constructor
     * 
     * @param orb A reference to the ORB pseudo-object
     * @param parent Parent POA for the factory
     * @param nameDomain Object reference of the name domain where the
     * factory will register objects it creates
     * @param nameDomainName The fully qualified name of the NameDomain
     * @param factoryName Fully qualified name of the CDMW factory
     * @param args program arguments
     *
     * @exception BadParameterException Wrong parameter value, such as 
     * threading ... etc.
     * @exception InternalErrorException Internal Error
     * @exception SystemException Any CORBA system exception
     */ 
    public OrbacusTypedEventChannelFactoryImpl(ORB orb, POA parent, NameDomain nameDomain, 
        String nameDomainName, String factoryName, 
        POAThreadingPolicy threadingPolicy, String[] args)
        throws BadParameterException, InternalErrorException, SystemException {
        
        this.orb = orb;
        this.poa = parent;
        this.domain = nameDomain;
        this.domainName = nameDomainName;
        this.factoryName = factoryName;
        
        // Create policy list for simple persistence
        Policy[] pl = new Policy[5];
        pl[0] = poa.create_lifespan_policy(
            org.omg.PortableServer.LifespanPolicyValue.PERSISTENT);

        pl[1] = poa.create_id_assignment_policy(
            org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID);

        pl[2] = poa.create_implicit_activation_policy(
            org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);

        pl[3] = poa.create_servant_retention_policy(
            org.omg.PortableServer.ServantRetentionPolicyValue.NON_RETAIN);

        pl[4] = poa.create_request_processing_policy(
            org.omg.PortableServer.RequestProcessingPolicyValue.USE_SERVANT_MANAGER);

        // Get parent POA's POA manager
        org.omg.PortableServer.POAManager pmanager = poa.the_POAManager();

        try {
            // Create new POA
            String notifierPOAName = factoryName + "POA";
            notifierPOA = poa.create_POA(notifierPOAName, pmanager, pl);

            // Create and register servant locator
            org.omg.PortableServer.Servant servantLocator = 
                new ServantLocatorImpl(nameDomain);
            org.omg.PortableServer.ServantLocator theLocator = 
                (ServantLocatorImpl) servantLocator;
            notifierPOA.set_servant_manager(theLocator);
        } catch(org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            throw new BadParameterException(aae.getMessage());
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new BadParameterException(wp.getMessage());
        } catch(org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            throw new BadParameterException(ip.getMessage());
        }

        // Clean up
        for (int i = 0; i < pl.length; ++i) {
            pl[i].destroy();
        }
        
    }


    /**
     * Return the POA with which the servant is activated
     */
    public POA _default_POA() {
        return this.poa;
    }

    /**
     * Create untyped event channel
     * 
     * @param name The event channel name
     * @return A reference on the created event channel
     * @exception com.thalesgroup.CdmwLifeCycle.AlreadyExistant 
     * Event channel already exists
     * @exception com.thalesgroup.CdmwLifeCycle.InvalidName Name format invalid
     * @exception com.thalesgroup.CdmwEvent.CreationError 
     * Error occurred during event channel creation
     * @exception SystemException System exception
     */ 
    //
    // IDL:thalesgroup.com/CdmwEvent/EventChannelFactory/create_event_channel:1.0
    //
    public org.omg.CosTypedEventChannelAdmin.TypedEventChannel 
        create_event_channel(String name)
        throws com.thalesgroup.CdmwLifeCycle.AlreadyExistant,
               com.thalesgroup.CdmwLifeCycle.InvalidName,
               com.thalesgroup.CdmwEvent.CreationError,
               SystemException {

        org.omg.CosTypedEventChannelAdmin.TypedEventChannel channel;
        com.ooc.OBTypedEventChannelFactory.TypedEventChannelFactory factory = null;
        org.omg.CORBA.Object obj;

        // check supported interface is specified in xml file
        String interfaceKey = OrbacusEventChannelUtils.findInterface(name);
        
        // if no interface Key  specified do not create typed event channel.
        if (interfaceKey.equals(EMPTY_STRING)) {
            throw new com.thalesgroup.CdmwEvent.CreationError(
                "No supported key defined for channel:" + name);
        }
        // Register the name object_key within the NameDomain. This
        // will check validity of the name and whether it is already in use.
        Any regId = OrbacusEventChannelUtils.registerName(domain, name);  

        // Object's name has been registered successfuly. From now on, it should be
        // unregistered if an exception has to be rethrown
        ObjectRegistrationGuard unreg 
            = new ObjectRegistrationGuard(domain, name);

        // find the factory able to create this event channel 
        try {
            obj = OrbacusEventChannelUtils.findOBFactory(orb, name, 
                DEFAULT_TYPED_EVENT_CHANNEL_FACTORY);
            factory = com.ooc.OBTypedEventChannelFactory.
                TypedEventChannelFactoryHelper.narrow(obj);            
        } catch(com.thalesgroup.CdmwEvent.CreationError ce) {
            unreg.destroy();
            throw ce;
        } catch(SystemException se) {
            unreg.destroy();
            throw se;
        }


        if (factory==null) {
            throw new com.thalesgroup.CdmwEvent.CreationError(
                "Invalid factory for channel " + name);
        }

        try {
            channel = factory.create_channel(name);
        } catch (com.ooc.OBEventChannelFactory.ChannelAlreadyExists cae) {
            throw new com.thalesgroup.CdmwLifeCycle.AlreadyExistant(name);
        }

        // Convert channelname to OID
        byte[] oid = name.getBytes();

        // Make a new reference.
        obj = notifierPOA.create_reference_with_id(oid, PUSH_SUPPLIER_REP_ID);
        org.omg.CosEventComm.PushSupplier notifier 
            = org.omg.CosEventComm.PushSupplierHelper.narrow(obj);

        // connect notifier to channel in order to be notified when event channel is destroyed
        try {
            TypedEventChannelInterface typedEventChannelInt 
                = new TypedEventChannelInterface(channel);

            org.omg.CosEventComm.PushConsumer proxyConsumer 
                = org.omg.CosEventComm.PushConsumerHelper.narrow(
                    typedEventChannelInt.getTypedPushConsumer(
                        interfaceKey, notifier));
        } catch(org.omg.CosTypedEventChannelAdmin.InterfaceNotSupported ins) {
            channel.destroy();
            unreg.destroy();
            
            throw new com.thalesgroup.CdmwEvent.CreationError(
                "Interface "+ interfaceKey +" not supported!");
        } catch (SystemException se) {
            channel.destroy();
            unreg.destroy();

            System.out.println("Impossible to connect "
                + "channel destruction notifier: system exception raised!");
            se.printStackTrace();
            throw new com.thalesgroup.CdmwEvent.CreationError(se.getMessage());
        } catch (Exception e) {
            channel.destroy();
            unreg.destroy();

            System.out.println("Impossible to connect "
                + "channel destruction notifier: exception raised!");
            throw new com.thalesgroup.CdmwEvent.CreationError(e.getMessage());
        }

        // Register (name,object) within the repository. In case of an exception
        // ObjectRegistrationGuard will try to unregister the name, even if there a 
        // high probability that this may not work.
        try {
            OrbacusEventChannelUtils.registerObject(domain, regId, channel, MAX_RETRIES);
        } catch(SystemException se) {
            unreg.destroy();
            throw se;
        }

        // Everything is OK, so we "do not rollback" the registration
        unreg.cancel();

        // Return the object reference
        return channel;
               
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    /**
     * Supports the following objects:
     *  IDL:omg.org/CosEventChannelAdmin/EventChannel:1.0
     *
     * @param repositoryId Repository ID of a CDMW object
     * @return true if the object is supported, false otherwise
     */
    public boolean supports(String repositoryId) {
        return repositoryId.equals(TYPED_EVT_CHANNEL_REP_ID);
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supported_objects:1.0
    //
    /**
     * This operation returns a sequence of CORBA Repository IDs of CDMW 
     * objects that can be created by the factory.
     *
     * @return A sequence of CORBA Repository IDs of CDMW objects that 
     * can be created by the factory that is:
     * IDL:omg.org/CosEventChannelAdmin/EventChannel:1.0
     */ 
    public String[] get_supported_objects() {
        String[] repIdSeq = new String[1];
        repIdSeq[0] = TYPED_EVT_CHANNEL_REP_ID;
        return repIdSeq;
    }                                                                             

}


