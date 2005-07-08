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


package cdmw.ccm.cif;

import cdmw.common.InternalErrorException;
import cdmw.common.AlreadyDoneException;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.ccm.common.CookieImpl;

/**
 * This class implements the org.omg.Components.CCMObject interface.
 * It will be inherited by all components servant implementation.
 *
 */
//
// IDL:omg.org/Components/CCMObject:1.0
//
public abstract class CCMObjectImpl extends org.omg.Components.CCMObjectPOA {
    
    public static final String PUSH_CONSUMER_ID = 
        "IDL:CosEventComm::PushConsumer:1.0";

    public static final String PSEUDO_EMITTER_CK_VALUE = "Emitter";
    
    ////////////////////////
    //                    //
    //    Attributes      //
    //                    //
    ////////////////////////

    /**
     * Component Context for CDMW component
     */
    protected com.thalesgroup.CdmwCcmContainer.CCM2Context context;

    /**
     * component rep id
     */
    protected String repId;

    /**
     * set to true when configuration completed
     */
    protected boolean configured = false;

    /**
     * object Id of the current component
     */
    protected String componentOid;

    /**
     * Facet Map: find facet info by name
     */
    private class FacetInfo {
        /**
         * facet repository id
         */
        public String repId;
        /**
         *  facet reference
         */
        public org.omg.CORBA.Object objRef;
    };

    private class ReceptacleInfo {
        /**
         * receptacle repository id
         */
        public String repId;
        /**
         * true if receptacle is multiplex
         */
        public boolean isMultiplex;
        /**
         * connection info
         */
        public java.util.Hashtable connections = new java.util.Hashtable();
    };

    /**
     * Consumer Map
     */
    private class ConsumerInfo {
        /**
         * consumer repository id
         */
        public String repId;
        /**
         * consumer reference
         */
        public org.omg.CORBA.Object consumer;
    };

    private class EmitterInfo {
        /**
         * consumer repository id
         */
        public String repId;
        /**
         * event channel
         */
        public cdmw.event.EventChannelInterface channel;
        /**
         * proxy push consumer used to push event in event channel
         */
        public org.omg.CosEventComm.PushConsumer proxyConsumer;
        /**
         * proxy push supplier used to connect consumer
         */
        public org.omg.CosEventComm.PushSupplier proxySupplier;
        /**
         * component consumer reference
         */
        public org.omg.Components.EventConsumerBase ccmConsumer;
        /**
         * consumer reference implemented org.omg.CosEventComm.PushConsumer
         */
        public org.omg.CosEventComm.PushConsumer pushConsumer;
        /**
         * local consumer servant implementing org.omg.CosEventComm.PushConsumer
         */
        public org.omg.PortableServer.Servant pushConsumerServant;
    };

    private class SubscriberInfo {
        /**
         * proxy push supplier used to connect consumer
         */
        public org.omg.CosEventComm.PushSupplier proxySupplier;
        /**
         * component consumer reference
         */
        public org.omg.Components.EventConsumerBase ccmConsumer;
        /**
         * consumer reference implementing org.omg.CosEventComm.PushConsumer
         */
        public org.omg.CosEventComm.PushConsumer pushConsumer;
        /**
         * local consumer servant implementing org.omg.CosEventComm.PushConsumer
         */
        public org.omg.PortableServer.Servant pushConsumerServant;
    };

    private class PublisherInfo {
        /**
         * consumer repository id
         */
        public String repId;
        /**
         * event channel
         */
        public cdmw.event.EventChannelInterface channel;
        /**
         * proxy push consumer used to push event in event channel
         */
        public org.omg.CosEventComm.PushConsumer proxyConsumer;
        /**
         * connection info
         */
        public java.util.Hashtable subscriptions = new java.util.Hashtable();
    };

    /**
     *  map of component facet
     */
    private java.util.Hashtable facets = new java.util.Hashtable();

    /**
     * map of component receptacle
     */
    private java.util.Hashtable receptacles = new java.util.Hashtable();

    /**
     * map of component event sink
     */
    private java.util.Hashtable consumers = new java.util.Hashtable();

    /**
     * map of component source (emitter)
     */
    private java.util.Hashtable emitters = new java.util.Hashtable();

    /**
     * map of component source (publisher)
     */
    private java.util.Hashtable publishers = new java.util.Hashtable();

    /**
     * true if remove operation has been called
     */
    private boolean isRemoved = false;

    public CCMObjectImpl(
        String compOid,
        com.thalesgroup.CdmwCcmContainer.CCM2Context ctx,
        String repId) {

        this.context = ctx;
        this.repId = repId;
        this.componentOid = compOid;

    }

    //
    // IDL:omg.org/Components/CCMObject/get_component_def:1.0
    //
    //public org.omg.CORBA.IRObject get_component_def() {
    public org.omg.Components.IRObject get_component_def() {
    	if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        org.omg.Components.CCMHome home = this.context.get_CCM_home();
        return home.get_component_def();
    }

    //
    // IDL:omg.org/Components/CCMObject/get_ccm_home:1.0
    //
    public org.omg.Components.CCMHome get_ccm_home() {
    	if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        return this.context.get_CCM_home();
    }

    //
    // IDL:omg.org/Components/CCMObject/get_primary_key:1.0
    //
    public org.omg.Components.PrimaryKeyBase get_primary_key()
        throws org.omg.Components.NoKeyAvailable {
        
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        // Check that there is an associated primary key.
        // If so, context is an entity context
        try {
            org.omg.Components.EntityContext ectx = 
                org.omg.Components.EntityContextHelper.narrow(this.context);
            return ectx.get_primary_key();
        } catch(org.omg.CORBA.BAD_PARAM bp) {
            throw new org.omg.Components.NoKeyAvailable();
        } catch (org.omg.Components.IllegalState is) {
            throw new org.omg.Components.NoKeyAvailable();
        }
        
    }

    //
    // IDL:omg.org/Components/CCMObject/configuration_complete:1.0
    //
    public void configuration_complete()
        throws org.omg.Components.InvalidConfiguration {
        
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        // Check receptacle are connected
        java.util.Enumeration e = this.receptacles.elements();
        
        while(e.hasMoreElements()) {
            ReceptacleInfo rInfo = (ReceptacleInfo) e.nextElement();
            
            if (rInfo.connections.size() == 0) {
                throw new org.omg.Components.InvalidConfiguration();
            }
        }
    
        this.configured = true;

    }

    //
    // IDL:omg.org/Components/CCMObject/remove:1.0
    //
    public void remove() throws org.omg.Components.RemoveFailure {
    	
    	if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    
        // check component context is a Session2Context one
        org.omg.Components.Session2Context s2ctx 
            = org.omg.Components.Session2ContextHelper.narrow(this.context);
        
        if (s2ctx != null) {
            // advise executor of its remove
            this.advise_executor_of_remove_component();
        
            // remove all facets
            java.util.Enumeration e = this.facets.elements();
        
            while(e.hasMoreElements()) {
                FacetInfo fInfo = (FacetInfo) e.nextElement();
                
                org.omg.CosLifeCycle.LifeCycleObject facetBase 
                    = org.omg.CosLifeCycle.LifeCycleObjectHelper.narrow(fInfo.objRef);
                try {
                    facetBase.remove();
                } catch(org.omg.CosLifeCycle.NotRemovable nr) {
                    System.err.println(nr.toString());
                    throw new org.omg.Components.RemoveFailure(
                        com.thalesgroup.CdmwDeployment.REMOVE_FAILURE_BASE.value);
                }
            }

            this.facets.clear();

            // remove all consumers
            e = this.consumers.elements();
            
            while (e.hasMoreElements()) {
                ConsumerInfo cInfo = (ConsumerInfo) e.nextElement();
                
                org.omg.CosLifeCycle.LifeCycleObject consumerBase 
                    = org.omg.CosLifeCycle.LifeCycleObjectHelper.narrow(cInfo.consumer);
                try {
                    consumerBase.remove();
                } catch(org.omg.CosLifeCycle.NotRemovable nr) {
                    System.err.println(nr.toString());
                    throw new org.omg.Components.RemoveFailure(
                        com.thalesgroup.CdmwDeployment.REMOVE_FAILURE_BASE.value);
                }
            }

            this.consumers.clear();

            // remove all publishers
            e = this.publishers.elements();
            
            while (e.hasMoreElements()) {
                PublisherInfo publisher = (PublisherInfo) e.nextElement();

		if (publisher.proxyConsumer != null)
                    publisher.proxyConsumer.disconnect_push_consumer();

                if (publisher.channel != null) {
                    publisher.channel.destroy();
                }
            }

            // remove all emitters
            e = this.emitters.elements();
            
            while (e.hasMoreElements()) {
                EmitterInfo emitter = (EmitterInfo) e.nextElement();

                if (emitter.proxyConsumer != null) {
                    emitter.proxyConsumer.disconnect_push_consumer();
                }
            }

            // register object as removed
            this.isRemoved = true;

            // deactivate servant
            try {
                this.context.req_passivate();
            } catch(org.omg.Components.PolicyMismatch pm) {
                System.err.println(pm.toString());
                throw new org.omg.Components.RemoveFailure(
                    com.thalesgroup.CdmwDeployment.REMOVE_FAILURE_BASE.value);
            }
            
        } else {
            System.err.println("Context is not a Session2Context!");
            throw new org.omg.Components.RemoveFailure(
                com.thalesgroup.CdmwDeployment.WRONG_CONTEXT_KIND.value);
        }
    
    }

    //
    // IDL:omg.org/Components/CCMObject/get_all_ports:1.0
    //
    public org.omg.Components.ComponentPortDescription get_all_ports() {
      
      if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    
      org.omg.Components.ComponentPortDescription componentPortDesc =
          new ComponentPortDescriptionImpl();
    
      componentPortDesc.facets = get_all_facets();
      componentPortDesc.receptacles = get_all_receptacles();
      componentPortDesc.consumers = get_all_consumers();
      componentPortDesc.emitters = get_all_emitters();
      componentPortDesc.publishers = get_all_publishers();
    
      return componentPortDesc;
    
    }

    //
    // IDL:omg.org/Components/Navigation/provide_facet:1.0
    //
    public org.omg.CORBA.Object provide_facet(String name)
        throws org.omg.Components.InvalidName {
        
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        if (name == null) {
            throw new org.omg.Components.InvalidName();
        }
    
        String[] nameList = new String[1];
        nameList[0] = name;
    
        org.omg.Components.FacetDescription[] fDesc = 
            get_named_facets(nameList);
    
        if (fDesc.length != 1) {
            throw new org.omg.Components.InvalidName();
        }
    
        return fDesc[0].facet_ref;

    }

    //
    // IDL:omg.org/Components/Navigation/get_all_facets:1.0
    //
    public org.omg.Components.FacetDescription[] get_all_facets() {
    	
    	if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        
        String[] nameList = new String[this.facets.size()];
        java.util.Set keyset = this.facets.keySet();
        java.util.Iterator iter = keyset.iterator();
        int i = 0;
    
        while (iter.hasNext()) {
            nameList[i] = (String) iter.next();
            i++;
        }
    
        try {
            return get_named_facets(nameList);
        } catch (org.omg.Components.InvalidName in) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    }

    //
    // IDL:omg.org/Components/Navigation/get_named_facets:1.0
    //
    public org.omg.Components.FacetDescription[] get_named_facets(
        String[] names)
        throws org.omg.Components.InvalidName {
        
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        org.omg.Components.FacetDescription[] fDesc = 
            new org.omg.Components.FacetDescription[names.length];
        
        for (int i=0; i<names.length; i++) {
            String name = names[i];
            FacetInfo fInfo = (FacetInfo) this.facets.get(name);
    
            if (fInfo != null) {
                // receptacle_name exists
                fDesc[i] = new FacetDescriptionImpl();
    
                fDesc[i].name = name;
                fDesc[i].type_id = fInfo.repId;
                fDesc[i].facet_ref = fInfo.objRef;
            } else {
                // name not found => error
                throw new org.omg.Components.InvalidName();
            }
        }
        
        return fDesc;

    }

    //
    // IDL:omg.org/Components/Navigation/same_component:1.0
    //
    public boolean same_component(org.omg.CORBA.Object objectRef) {
    	
    	if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    
        if (objectRef == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        // First case: object_ref is facet
        // Check object reference is equivalent with a facet of the component.
        java.util.Enumeration e = this.facets.elements();
    
        while (e.hasMoreElements()) {
            FacetInfo fInfo = (FacetInfo) e.nextElement();
    
            if (objectRef._is_equivalent(fInfo.objRef)) {
                return true;
            }
        }
    
        // Second case: object_ref is a component ref
        /*
        // Check facets of this component is equivalent with the current ones
        try {
            org.omg.Components.CCMObject componentRef 
                = org.omg.Components.CCMObjectHelper.narrow(objectRef);
    
            if (componentRef == null) {
                return false;
            }
            org.omg.Components.FacetDescription[] remoteFacets = 
                componentRef.get_all_facets();
            org.omg.Components.FacetDescription[] myFacets = get_all_facets();
    
            int nbFacet = myFacets.length;
    
            if (nbFacet != remoteFacets.length) {
                // components have not the same number of facet
                return false;
            }
    
            for (int i=0; i<nbFacet; i++) {
                org.omg.CORBA.Object objRef = myFacets[i].facet_ref;
                org.omg.CORBA.Object objRef2 = remoteFacets[i].facet_ref;
                if ( !objRef._is_equivalent(objRef2) ) {
                    return false;
                }
            }
        } catch (Exception e) {
            // exception raised
            // object ref is propably not a component ref
            return false;
        }
        */
    
        try {
            // check component context is a Session2Context one
            org.omg.Components.SessionContext sctx 
                = org.omg.Components.Session2ContextHelper.narrow(this.context);
            
            if (sctx != null) {
                org.omg.CORBA.Object compRef = sctx.get_CCM_object();
                return compRef._is_equivalent(objectRef);
            }
        } catch(org.omg.Components.IllegalState is) {
            System.err.println("IllegalState exception raised!");
            return false;
        } catch (org.omg.CORBA.SystemException se) {
            // exception raised
            return false;
        }
    
        return true;
    
    }

    //
    // IDL:omg.org/Components/Receptacles/connect:1.0
    //
    /**
     * connects the object reference specified by the connection parameter to
     * the receptacle specified by the name parameter on the component.
     * 
     * @param name        The receptacle name.
     * @param connection  The connection object reference.
     *
     * @return a cookie if receptacle is multiplex, nil otherwise.
     * @exception org.omg.Components.InvalidName The name parameter does not specify a valid receptacle name.
     * @exception org.omg.Components.InvalidConnection The object reference in the connection parameter does
     * not support the interface declared in the receptacle’s uses statement.
     * @exception org.omg.Components.AlreadyConnected The receptacle is a simplex receptacle and it is already connected.
     * @exception org.omg.Components.ExceededConnectionLimit The receptacle is a multiplex receptacle and the 
     * implementation-defined limit to the number of connections is exceeded.
     * @exception CORBA::SystemException  Any Corba system exception.
     */
    public org.omg.Components.Cookie connect(
        String name,
        org.omg.CORBA.Object connection)
        throws
            org.omg.Components.InvalidName,
            org.omg.Components.InvalidConnection,
            org.omg.Components.AlreadyConnected,
            org.omg.Components.ExceededConnectionLimit {
            	
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        org.omg.Components.Cookie cookieValue = null;

        if (name == null) {
            throw new org.omg.Components.InvalidName();
        }
    
        if (connection == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    
        // find receptacle
        ReceptacleInfo rInfo = (ReceptacleInfo) this.receptacles.get(name);
        
        if (rInfo != null) {
            // receptacle_name exists
    
            if (!rInfo.isMultiplex) {
                // simplex receptacle: check if a receptacle is not already connected!
                if (rInfo.connections.size() > 0) {
                    throw new org.omg.Components.AlreadyConnected();
                }                   
            } else {
                // multiplex receptacle
                // No limit defined for connection : exception ExceededConnectionLimit not raised!
            }
    
            // Check connection supports the interface declared in uses statement
            if ( !connection._is_a(rInfo.repId) ) {
                throw new org.omg.Components.InvalidConnection();
            }
    
            // object has the right interface, insert new connection in the list
            // Cookie is created even for simplex receptacle for homogeneity
            cookieValue = CookieImpl.createCookie();
            rInfo.connections.put(cookieValue, connection);
    
            if (!rInfo.isMultiplex) {
                cookieValue = null;
            }
        } else {
            // no receptacle named name exist!
            throw new org.omg.Components.InvalidName();
        }
    
        return cookieValue;

    }

    //
    // IDL:omg.org/Components/Receptacles/disconnect:1.0
    //
    /**
     * disconnects the object reference specified by ck parameter to
     * the receptacle specified by the name parameter on the component.
     * 
     * @param name   The receptacle name.
     * @param ck     The connection cookie value.
     *
     * @return void.
     * @exception org.omg.Components.InvalidName The name parameter does not specify a valid receptacle name.
     * @exception org.omg.Components.InvalidConnection The receptacle is a multiplex receptacle and the cookie
     *           value in the ck parameter does not denote an existing connection on the receptacle.
     * @exception org.omg.Components.CookieRequired The receptacle is a multiplex receptacle and a null value
     *           is specified in the ck parameter.
     * @exception org.omg.Components.NoConnection The receptacle is a simplex receptacle there is no current connection.
     * @exception CORBA::SystemException  Any Corba system exception.
     *
     */
    public void disconnect(String name, org.omg.Components.Cookie ck)
        throws
            org.omg.Components.InvalidName,
            org.omg.Components.InvalidConnection,
            org.omg.Components.CookieRequired,
            org.omg.Components.NoConnection {
            	
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        if (name == null) {
            throw new org.omg.Components.InvalidName();
        }
    
        // find receptacle
        ReceptacleInfo rInfo = (ReceptacleInfo) this.receptacles.get(name);
        
        if (rInfo != null) {
            // receptacleName exists
    
            if (rInfo.isMultiplex) {
                if (ck == null) {
                    throw new org.omg.Components.CookieRequired();
                }
                
                // try to remove this connection from the list
                if (rInfo.connections.remove(ck) == null) {
                    // no connection found 
                    throw new org.omg.Components.InvalidConnection();
                }
            } else {
                // simplex receptacle
                if (rInfo.connections.size() == 0) {
                    throw new org.omg.Components.NoConnection();
                }            
                
                // erase all element in map (there is only one!)
                rInfo.connections.clear();
            }
        } else {
            // no receptacle named name exist!
            throw new org.omg.Components.InvalidName();
        }

    }

    //
    // IDL:omg.org/Components/Receptacles/get_connections:1.0
    //
    public org.omg.Components.ConnectionDescription[] get_connections(
        String name)
        throws org.omg.Components.InvalidName {
        
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        if (name == null) {
            throw new org.omg.Components.InvalidName();
        }
        
        String[] nameList = new String[1];
        nameList[0] = name;
        
        org.omg.Components.ReceptacleDescription[] rDesc = 
            get_named_receptacles(nameList);
        return rDesc[0].connections;

    }

    //
    // IDL:omg.org/Components/Receptacles/get_all_receptacles:1.0
    //
    public org.omg.Components.ReceptacleDescription[] get_all_receptacles() {
    	
    	if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    
        String[] nameList = new String[this.receptacles.size()];
        java.util.Set keyset = this.receptacles.keySet();
        java.util.Iterator iter = keyset.iterator();
        int i = 0;
    
        while (iter.hasNext()) {
            nameList[i] = (String) iter.next();
            i++;
        }
        try {
            org.omg.Components.ReceptacleDescription[] rDesc 
                = get_named_receptacles(nameList);
            return rDesc;
        } catch (org.omg.Components.InvalidName in) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    
    }

    //
    // IDL:omg.org/Components/Receptacles/get_named_receptacles:1.0
    //
    public org.omg.Components.ReceptacleDescription[] get_named_receptacles(
        String[] names)
        throws org.omg.Components.InvalidName {
        
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        org.omg.Components.ReceptacleDescription[] rDesc = 
            new org.omg.Components.ReceptacleDescription[names.length];
        
        for (int i=0; i<names.length; i++) {
            String name = names[i];
            ReceptacleInfo rInfo = (ReceptacleInfo) this.receptacles.get(name);
    
            if (rInfo != null) {
                // receptacle_name exists
    
                rDesc[i] = new ReceptacleDescriptionImpl();
                rDesc[i].name = name;
                rDesc[i].type_id = rInfo.repId;
                rDesc[i].is_multiple = rInfo.isMultiplex;
    
                org.omg.Components.ConnectionDescription[] connections = 
                    new org.omg.Components.ConnectionDescription[rInfo.connections.size()];
                
                java.util.Set keySet = rInfo.connections.keySet();
                java.util.Iterator iter = keySet.iterator();
                int j = 0;

                while(iter.hasNext()) {
                    org.omg.Components.Cookie cookie = 
                        (org.omg.Components.Cookie) iter.next();
                    org.omg.CORBA.Object objectRef = 
                        (org.omg.CORBA.Object) rInfo.connections.get(cookie);
                    
                    connections[j] = new ConnectionDescriptionImpl();
                    connections[j].ck = cookie;
                    connections[j].objref = objectRef;
                    
                    j++;
                }
                rDesc[i].connections = connections;
            
            } else {
                // name not found => error
                throw new org.omg.Components.InvalidName();
            }
        }
        
        return rDesc;

    }

    //
    // IDL:omg.org/Components/Events/get_consumer:1.0
    //
    public org.omg.Components.EventConsumerBase get_consumer(String sinkName)
        throws org.omg.Components.InvalidName {
        
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        org.omg.Components.EventConsumerBase result = null;

        if (sinkName == null) {
            throw new org.omg.Components.InvalidName();
        }
    
        ConsumerInfo cInfo = (ConsumerInfo) this.consumers.get(sinkName);
        
        if (cInfo != null) {
            // sink exist
            return org.omg.Components.EventConsumerBaseHelper.narrow(cInfo.consumer);
        } else {
            // no event sink defined with this name for this component
            throw new org.omg.Components.InvalidName();
        }

    }

    //
    // IDL:omg.org/Components/Events/subscribe:1.0
    //
    public org.omg.Components.Cookie subscribe(
        String publisherName,
        org.omg.Components.EventConsumerBase subscriber)
        throws
            org.omg.Components.InvalidName,
            org.omg.Components.InvalidConnection,
            org.omg.Components.ExceededConnectionLimit {
        
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        org.omg.Components.Cookie cookieValue = null;

        if (publisherName == null) {
            throw new org.omg.Components.InvalidName();
        }
    
        if (subscriber == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    
        // find publisher
        PublisherInfo pInfo = 
            (PublisherInfo) this.publishers.get(publisherName);
        
        if (pInfo != null) {
            // publisher_name exists
    
            // Check connection supports the interface declared in uses statement
            if ( !subscriber._is_a(pInfo.repId) ) {
                throw new org.omg.Components.InvalidConnection();
            }
    
            // object has the right interface, insert new subscription in the list
            cookieValue = CookieImpl.createCookie();
    
            // Subscribe consumer to the right event channel
            SubscriberInfo sInfo = new SubscriberInfo();
    
            sInfo.ccmConsumer = subscriber;
            
            // insert subscription now because we need to find it 
            // in lookupServant when narrow operation is called!
            pInfo.subscriptions.put(cookieValue, sInfo);
            
            // test if subscriber is a cdmw subscriber or not 
            // (that is implement CosEventComm::PushConsumer)
            try {
                sInfo.pushConsumer  
                    = org.omg.CosEventComm.PushConsumerHelper.narrow(subscriber);
            } catch(org.omg.CORBA.BAD_PARAM bp1) {
                sInfo.pushConsumerServant = 
                    new PushConsumerImpl(this.context, subscriber);
            
                // event_sink is not a CDMW one
                // Create my local push consumer to receive event 
                // this consumer will send to the EventConsumerBase.
                // check component context is a Session2Context one
                try {
                    org.omg.Components.Session2Context s2ctx 
                        = org.omg.Components.Session2ContextHelper.narrow(this.context);
                    String cookieStr = 
                        ((CookieImpl)cookieValue).getCookieString();
                    System.out.println("cookieStr = " + cookieStr);

                    byte[] pushConsumerOid = 
                        CCMUtils.createOid(this.componentOid, publisherName, cookieStr);

                    org.omg.CORBA.Object obj
                        = s2ctx.create_ref_from_oid(pushConsumerOid, PUSH_CONSUMER_ID);

                    sInfo.pushConsumer = org.omg.CosEventComm.PushConsumerHelper.narrow(obj);
                } catch(org.omg.CORBA.BAD_PARAM bp2) {
                    pInfo.subscriptions.remove(cookieValue);
                    System.err.println("Context is not a Session2Context!");
                    throw new org.omg.CORBA.INTERNAL(
                        ExceptionMinorCodes.INTERNAL, 
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }

            }
    
            try {
                sInfo.proxySupplier 
                    = pInfo.channel.connectPushConsumer(sInfo.pushConsumer);
            } catch (org.omg.CORBA.SystemException se) {
                pInfo.subscriptions.remove(cookieValue);
                System.err.println("Exception raised when connecting consumer to channel");
                throw se;
            }
        } else {
            // no publisher named name exist!
            throw new org.omg.Components.InvalidName();
        }
    
        return cookieValue;

    }

    //
    // IDL:omg.org/Components/Events/unsubscribe:1.0
    //
    public void unsubscribe(
        String publisherName,
        org.omg.Components.Cookie ck)
        throws org.omg.Components.InvalidName, 
            org.omg.Components.InvalidConnection {
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        if (publisherName == null) {
            throw new org.omg.Components.InvalidName();
        }
    
        if (ck == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    
        // find publisher
        PublisherInfo pInfo = 
            (PublisherInfo) this.publishers.get(publisherName);
        
        if (pInfo != null) {
            // publisher_name exists
    
            SubscriberInfo sInfo = (SubscriberInfo) pInfo.subscriptions.get(ck);
    
            if (sInfo != null) {
                // Unsubscribe consumer from the event channel
                sInfo.proxySupplier.disconnect_push_supplier();
    
                try {
                    com.thalesgroup.CdmwCcmCif.PushConsumer localPushConsumer
                        = com.thalesgroup.CdmwCcmCif.PushConsumerHelper.narrow(sInfo.pushConsumer);
                    // localConsumer created, remove it!
                    localPushConsumer.remove();
                } catch(org.omg.CORBA.BAD_PARAM bp) {
                    // localConsumer not created, do nothing
                } catch(org.omg.CosLifeCycle.NotRemovable nr) {
                    System.err.println(nr.toString());
                    throw new org.omg.Components.InvalidName();
                }
    
                // remove subscription
                pInfo.subscriptions.remove(ck);
            } else {
                // no connection found 
                throw new org.omg.Components.InvalidConnection();
            }
        } else {
            // no publisher named publisher_name exist!
            throw new org.omg.Components.InvalidName();
        }

    }

    //
    // IDL:omg.org/Components/Events/connect_consumer:1.0
    //
    public void connect_consumer(
        String emitterName,
        org.omg.Components.EventConsumerBase consumer)
        throws
            org.omg.Components.InvalidName,
            org.omg.Components.AlreadyConnected,
            org.omg.Components.InvalidConnection {
		
		if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        if (emitterName == null) {
            throw new org.omg.Components.InvalidName();
        }
    
        if (consumer == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        // find emitter
        EmitterInfo eInfo = (EmitterInfo) this.emitters.get(emitterName);
        
        if (eInfo != null) {
            // emitterName exists
    
            // Check connection supports the interface declared in uses statement
            if ( !consumer._is_a(eInfo.repId) ) {
                throw new org.omg.Components.InvalidConnection();
            }
    
            if (eInfo.ccmConsumer == null) {
                // No consumer connected
    
                eInfo.ccmConsumer = consumer;
    
                // test if subscriber is a cdmw subscriber or not (that is implement CosEventComm::PushConsumer)
                try {
                    eInfo.pushConsumer  
                        = org.omg.CosEventComm.PushConsumerHelper.narrow(consumer);
                } catch(org.omg.CORBA.BAD_PARAM bp1) {
                    // event_sink is not a CDMW one
                    // Create my local push consumer to receive event 
                    // this consumer will send to the EventConsumerBase.
    
                    eInfo.pushConsumerServant = 
                        new PushConsumerImpl(this.context, consumer);

                    try {
                        // check component context is a Session2Context one
                        org.omg.Components.Session2Context s2ctx 
                            = org.omg.Components.Session2ContextHelper.narrow(this.context);
                        
                        byte[] pushConsumerOid = CCMUtils.createOid(
                            this.componentOid, emitterName, PSEUDO_EMITTER_CK_VALUE);

                        org.omg.CORBA.Object obj = 
                            s2ctx.create_ref_from_oid(pushConsumerOid, PUSH_CONSUMER_ID);

                        eInfo.pushConsumer = 
                            org.omg.CosEventComm.PushConsumerHelper.narrow(obj);
                    } catch(org.omg.CORBA.BAD_PARAM bp2) {
                        System.err.println("Context is not a Session2Context!");
                        throw new org.omg.CORBA.INTERNAL(
                            ExceptionMinorCodes.INTERNAL, 
                            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                    }
                }
                              
                try {
                    eInfo.proxySupplier 
                        = eInfo.channel.connectPushConsumer(eInfo.pushConsumer);
                } catch (org.omg.CORBA.SystemException se) {
                    System.err.println("Exception raised when connecting consumer to channel");
                    throw se;
                }
            } else {
                throw new org.omg.Components.AlreadyConnected();
            }
        } else {
            // no emitter named emitterName exist!
            throw new org.omg.Components.InvalidName();
        }

    }

    //
    // IDL:omg.org/Components/Events/disconnect_consumer:1.0
    //
    public org.omg.Components.EventConsumerBase disconnect_consumer(
        String sourceName)
        throws org.omg.Components.InvalidName, org.omg.Components.NoConnection {
        
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        org.omg.Components.EventConsumerBase result = null;

        if (sourceName == null) {
            throw new org.omg.Components.InvalidName();
        }
    
        // find emitter
        EmitterInfo eInfo = (EmitterInfo) this.emitters.get(sourceName);
        
        if (eInfo != null) {
            // emitter_name exists
    
            if (eInfo.ccmConsumer == null) {
                throw new org.omg.Components.NoConnection();
            } else {
                // disconnect consumer from event channel
                eInfo.proxySupplier.disconnect_push_supplier();
    
                try {
                    com.thalesgroup.CdmwCcmCif.PushConsumer localPushConsumer
                        = com.thalesgroup.CdmwCcmCif.PushConsumerHelper.narrow(eInfo.pushConsumer);
                    // localConsumer created, remove it!
                    localPushConsumer.remove();
                } catch(org.omg.CORBA.BAD_PARAM bp) {
                    // localConsumer not created, do nothing
                } catch(org.omg.CosLifeCycle.NotRemovable nr) {
                    System.err.println(nr.toString());
                    throw new org.omg.Components.InvalidName();
                }
    
                // remove consumer
                result = eInfo.ccmConsumer;
                eInfo.ccmConsumer = null;
                eInfo.pushConsumer = null;
                eInfo.pushConsumerServant = null;
    
            }
        } else {
            // no emitter named emitter_name exist!
            throw new org.omg.Components.InvalidName();
        }
    
        return result;

    }

    //
    // IDL:omg.org/Components/Events/get_all_consumers:1.0
    //
    public org.omg.Components.ConsumerDescription[] get_all_consumers() {
    	
    	if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        String[] nameList = new String[this.consumers.size()];
        java.util.Set keyset = this.consumers.keySet();
        java.util.Iterator iter = keyset.iterator();
        int i = 0;

        while (iter.hasNext()) {
            nameList[i] = (String) iter.next();
            i++;
        }
        try {
            return get_named_consumers(nameList);
        } catch (org.omg.Components.InvalidName in) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

    }

    //
    // IDL:omg.org/Components/Events/get_named_consumers:1.0
    //
    public org.omg.Components.ConsumerDescription[] get_named_consumers(
        String[] names)
        throws org.omg.Components.InvalidName {
        	
		if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);        	
        org.omg.Components.ConsumerDescription[] cDesc = 
            new org.omg.Components.ConsumerDescription[names.length];
        
        for (int i=0; i<names.length; i++) {
            String name = names[i];
            ConsumerInfo cInfo = (ConsumerInfo) this.consumers.get(name);
    
            if (cInfo != null) {
                // consumer_name exists
    
                cDesc[i] = new ConsumerDescriptionImpl();
    
                cDesc[i].name = name;
                cDesc[i].type_id = cInfo.repId;
                cDesc[i].consumer = 
                    org.omg.Components.EventConsumerBaseHelper.narrow(cInfo.consumer);
            } else {
                // name not found => error
                throw new org.omg.Components.InvalidName();
            }
        }
        
        return cDesc;

    }

    //
    // IDL:omg.org/Components/Events/get_all_emitters:1.0
    //
    public org.omg.Components.EmitterDescription[] get_all_emitters() {
    	
    	if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        
        String[] nameList = new String[this.emitters.size()];
        java.util.Set keyset = this.emitters.keySet();
        java.util.Iterator iter = keyset.iterator();
        int i = 0;
    
        while (iter.hasNext()) {
            nameList[i] = (String) iter.next();
            i++;
        }
        try {
            return get_named_emitters(nameList);
        } catch (org.omg.Components.InvalidName in) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        
    }

    //
    // IDL:omg.org/Components/Events/get_named_emitters:1.0
    //
    public org.omg.Components.EmitterDescription[] get_named_emitters(
        String[] names) throws org.omg.Components.InvalidName {
        
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        org.omg.Components.EmitterDescription[] eDesc = 
            new org.omg.Components.EmitterDescription[names.length];
        
        for (int i=0; i<names.length; i++) {
            String name = names[i];
            EmitterInfo eInfo = (EmitterInfo) this.emitters.get(name);
    
            if (eInfo != null) {
                // emitter_name exists
    
                eDesc[i] = new EmitterDescriptionImpl();
    
                eDesc[i].name= name;
                eDesc[i].type_id = eInfo.repId;
                eDesc[i].consumer = eInfo.ccmConsumer;
            } else {
                // name not found => error
                throw new org.omg.Components.InvalidName();
            }
        }
        
        return eDesc;

    }

    //
    // IDL:omg.org/Components/Events/get_all_publishers:1.0
    //
    public org.omg.Components.PublisherDescription[] get_all_publishers() {
		
		if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        String[] nameList = new String[this.publishers.size()];
        java.util.Set keyset = this.publishers.keySet();
        java.util.Iterator iter = keyset.iterator();
        int i = 0;
    
        while (iter.hasNext()) {
            nameList[i] = (String) iter.next();
            i++;
        }
        try {
            return get_named_publishers(nameList);
        } catch (org.omg.Components.InvalidName in) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

    }

    //
    // IDL:omg.org/Components/Events/get_named_publishers:1.0
    //
    public org.omg.Components.PublisherDescription[] get_named_publishers(
        String[] names)
        throws org.omg.Components.InvalidName {
        	
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        org.omg.Components.PublisherDescription[] pDesc = 
            new org.omg.Components.PublisherDescription[names.length];
    
        for (int i=0; i<names.length; i++) {
            String name = names[i];
            PublisherInfo pInfo = (PublisherInfo) this.publishers.get(name);
    
            if (pInfo != null) {
                // publisher_name exists
    
                pDesc[i] = new PublisherDescriptionImpl();
                pDesc[i].name = name;
                pDesc[i].type_id = pInfo.repId;

                org.omg.Components.SubscriberDescription[] consumers = 
                    new org.omg.Components.SubscriberDescription[pInfo.subscriptions.size()];
                java.util.Set keySet = pInfo.subscriptions.keySet();
                java.util.Iterator iter = keySet.iterator();
                int j = 0;
    
                while (iter.hasNext()) {
                    org.omg.Components.Cookie cookie = 
                        (org.omg.Components.Cookie) iter.next();
                    SubscriberInfo sInfo = 
                        (SubscriberInfo) pInfo.subscriptions.get(cookie);

                    consumers[j] = new SubscriberDescriptionImpl();
                    consumers[j].ck = cookie;
                    consumers[j].consumer = sInfo.ccmConsumer;
                    j++;
                }
                
                pDesc[i].consumers = consumers;
            } else {
                // name not found => error
                throw new org.omg.Components.InvalidName();
            }
        }
        
        return pDesc;

    }

    /**
     * This checks whether the CORBA object incarnated by the current
     * object (this) has been removed (<I>remove</I> operation has been 
     * called).
     *
     * @return <B>true</B> if the object has been removed, <B>false</B> otherwise.
     */
    public boolean is_removed() {
        return this.isRemoved;
    }

    /**
     * This returns the proxy consumer for the event source specified by
     * source_name. This operation has been created for internal use.
     *
     * @param source_name The event source name (publisher/emitter)
     *
     * @return the proxy consumer of the event service.
     */
    public org.omg.CosEventComm.PushConsumer 
        get_proxy_consumer(String sourceName) {
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        org.omg.CosEventComm.PushConsumer consumer = null;
        
        // find source name in publisher map and emitter map
        PublisherInfo pInfo = (PublisherInfo) this.publishers.get(sourceName);
        
        if (pInfo != null) {
            // publisher_name exists
            consumer = pInfo.proxyConsumer;
        } else {
            // find in emitter map
            EmitterInfo eInfo = (EmitterInfo) this.emitters.get(sourceName);
            
            if (eInfo != null) {
                // emitter_name exists
                consumer = eInfo.proxyConsumer;
            } else {
                // no emitter nor publisher exist!
                // nil is return
            }
        }
        
        return consumer;

    }

    /**
     * This returns the servant for the local push consumer specified by source_name
     * and cookie value.
     *
     * @param source_name The source name
     * @param ck_value    The cookie value
     *
     * @return the local push consumer servant.
     */
    public org.omg.PortableServer.Servant get_local_push_consumer_servant(
        String sourceName,
        String ckValue) {
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        System.out.println("Entering in get_local_push_consumer_servant()");
        System.out.println("source name = " + sourceName);
        System.out.println("ckValue = " + ckValue);
    
        if (!ckValue.equals(PSEUDO_EMITTER_CK_VALUE)) {
            // look in publisher map
            PublisherInfo pInfo = (PublisherInfo) this.publishers.get(sourceName);
            
            if (pInfo != null) {
                // sourceName is a publisher
                
                org.omg.Components.Cookie ck = new CookieImpl(ckValue);
                SubscriberInfo sInfo = (SubscriberInfo) pInfo.subscriptions.get(ck);
                
                if (sInfo != null) {
                    
                    if (sInfo.pushConsumerServant != null) {
                        return sInfo.pushConsumerServant;
                    } else {
                        // No local push consumer with this name exist in this component
                        System.err.println("Local push consumer not found: do not exist for this connection.");
                        throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                            ExceptionMinorCodes.OBJECT_NOT_EXIST,
                            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                    }
                } else {
                    // No local push consumer with this name exist in this component
                    System.err.println("Local push consumer not found: no connexion exists in this component");
                    throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                        ExceptionMinorCodes.OBJECT_NOT_EXIST,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }
            } else {
                // No local push consumer with this name exist in this component
                System.err.println("Local push consumer not found: no publisher exists in this component");
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } else {
            // look in emitter map
            EmitterInfo eInfo = (EmitterInfo) this.emitters.get(sourceName);
            
            if (eInfo != null) {
                // sourceName is an emitter
    
                if (eInfo.pushConsumerServant != null) {
                    return eInfo.pushConsumerServant;
                } else {
                    // No local push consumer with this name exist in this component
                    System.err.println("Local push consumer not found: do not exists in this component for this emitter");
                    throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                        ExceptionMinorCodes.OBJECT_NOT_EXIST,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }
            } else {
                // No local push consumer with this name exist in this component
                System.err.println("Local push consumer not found: no emitter exists in this component");
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        }

    }

    /**
     * This returns the servant for the facet specified by facet_name.
     * This operation is implemented by the well typed component servant.
     *
     * @param facet_name The facet name
     *
     * @return the facet servant.
     */
    public abstract org.omg.PortableServer.Servant get_facet_servant(
        String facet_name);

    //////////////////////////
    //                      //
    // Internal Operations  //
    //                      //
    //////////////////////////

    /**
     * Adds a facet in FacetMap
     * 
     * @param comp_oid       The component oid used to create the facet reference.
     * @param facet_name     The facet name.
     * @param repId         The facet repository id.
     *
     * @return  The reference to be exported to clients
     * @exception InternalErrorException
     * @exception AlreadyDoneException
     */
    protected org.omg.CORBA.Object declare_facet(
        String compOid,
        String facetName,
        String repId)
        throws AlreadyDoneException, InternalErrorException {

        FacetInfo facetInfo = new FacetInfo();
        facetInfo.repId = repId;
        facetInfo.objRef = null;
    
        try {
            // check a facet with a same name has not been already declared
            FacetInfo fInfo = (FacetInfo) this.facets.get(facetName);
            if (fInfo == null) {
                // check component context is a Session2Context one
                org.omg.Components.Session2Context s2ctx 
                    = org.omg.Components.Session2ContextHelper.narrow(this.context);
                
                if (s2ctx != null) {
                    byte[] facetOid = CCMUtils.createOid(compOid, facetName);
                    
                    facetInfo.objRef = 
                        s2ctx.create_ref_from_oid(facetOid, repId);
                } else {
                    System.err.println("Context is not a Session2Context!");
                    throw new InternalErrorException();
                }
    
                this.facets.put(facetName, facetInfo);
            } else {
                System.err.println(facetName + " already declared");
                throw new AlreadyDoneException();
            }
        } catch (org.omg.CORBA.SystemException se) {
            System.err.println(se.toString());
            throw new InternalErrorException();
        }
    
        return facetInfo.objRef;

    }

    /**
     * Adds a receptacle in ReceptacleMap
     * 
     * @param receptacleName  The receptacle name.
     * @param repId           The receptacle repository id.
     * @param isMultiplex     This is set to true if receptacle is multiple
     *
     * @exception AlreadyDoneException
     */
    protected void declare_receptacle(
        String receptacleName,
        String repId,
        boolean isMultiplex)
        throws AlreadyDoneException {

        ReceptacleInfo receptacleInfo = new ReceptacleInfo();
        receptacleInfo.repId = repId;
        receptacleInfo.isMultiplex = isMultiplex;
    
        // check a receptacle with a same name has not been already declared
        ReceptacleInfo rInfo = 
            (ReceptacleInfo) this.receptacles.get(receptacleName);
        if (rInfo == null) {
            this.receptacles.put(receptacleName, receptacleInfo);
        } else {
            System.err.println(receptacleName + " already declared");
            throw new AlreadyDoneException();
        }

    }
    
    /**
     * Adds a receptacle in ReceptacleMap if receptacle is simple
     * 
     * @param receptacleName  The receptacle name.
     * @param repId           The receptacle repository id.
     *
     * @exception AlreadyDoneException
     */
    protected void declare_receptacle(
        String receptacleName,
        String repId)
        throws AlreadyDoneException {
    
        declare_receptacle(receptacleName, repId, false);
    
    }

    /**
     * Adds a consumer in ConsumerMap
     * 
     * @param compOid        The component oid used to create the consumer reference.
     * @param consumerName   The consumer name.
     * @param repId          The consumer repository id.
     *
     * @return void
     * @exception AlreadyDoneException
     */
    protected org.omg.CORBA.Object declare_consumer(
        String compOid,
        String consumerName,
        String repId)
        throws AlreadyDoneException, InternalErrorException {

        ConsumerInfo consumerInfo = new ConsumerInfo();
        consumerInfo.repId = repId;
        consumerInfo.consumer = null;
    
        try {
            // check a consumer with a same name has not been already declared
            ConsumerInfo cInfo = 
                (ConsumerInfo) this.consumers.get(consumerName);
            if (cInfo == null) {
                // check component context is a Session2Context one
                org.omg.Components.Session2Context s2ctx 
                    = org.omg.Components.Session2ContextHelper.narrow(this.context);
                
                if (s2ctx != null) {
                    byte[] consumerOid = 
                        CCMUtils.createOid(compOid, consumerName);
                    
                    consumerInfo.consumer
                        = s2ctx.create_ref_from_oid(consumerOid, repId);
                } else {
                    System.err.println("Context is not a Session2Context!");
                    throw new InternalErrorException();
                }
    
                this.consumers.put(consumerName, consumerInfo);
            } else {
                System.err.println(consumerName + " already declared");
                throw new AlreadyDoneException();
            }
        } catch (org.omg.CORBA.SystemException se) {
            System.err.println(se.toString()); 
            throw new InternalErrorException();
        }

        return consumerInfo.consumer;

    }

    /**
     * Adds an emitter in EmitterMap
     * 
     * @param emitterName   The emitter name.
     * @param repId         The emitter repository id.
     *
     * @return void
     * @exception AlreadyDoneException
     */
    protected void declare_emitter(String emitterName, String repId)
        throws AlreadyDoneException {
        	
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        // check a emitter with a same name has not been already declared
        EmitterInfo eInfo = (EmitterInfo) this.emitters.get(emitterName);
        if (eInfo == null) {
            // store information concerning the emitter
            EmitterInfo emitterInfo = new EmitterInfo();
            emitterInfo.repId = repId;
            
            // create an event channel associated to this component instance and 
            // this port name
            try {
                org.omg.Components.PortDescription portDesc 
                    = new PortDescriptionImpl();
                portDesc.name = emitterName;
                portDesc.type_id = repId;
                
                org.omg.CosEventChannelAdmin.EventChannel eventChannel =
                    this.context.obtain_event_channel_for_emitter(portDesc);
                
                emitterInfo.channel = 
                    new cdmw.event.EventChannelInterface(eventChannel);
            } catch (com.thalesgroup.CdmwCcmContainer.NoChannel nc) {
                System.err.println(emitterName + " event channel doesn't exist!");
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL, 
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch (org.omg.CORBA.SystemException se){
                System.err.println("System exception raised during event channel creation!");
                throw se;
            }
            
            try {
                emitterInfo.proxyConsumer = 
                    emitterInfo.channel.getPushConsumer(null);
            } catch (org.omg.CORBA.SystemException se) {
                System.err.println("System exception raised during emitter connection to event channel!");
                throw se;
            }
            
            // emitter info completed, store it in map
            this.emitters.put(emitterName, emitterInfo);
        } else {
            System.err.println(emitterName + " already declared");
            throw new AlreadyDoneException();
        }

    }

    /**
     * Adds a publisher in PublisherMap
     * 
     * @param publisher_name   The publisher name.
     * @param repId           The publisher repository id.
     *
     * @return void
     * @exception AlreadyDoneException if the publisher has already been declared.
     */
    protected void declare_publisher(String publisherName, String repId)
        throws AlreadyDoneException {
        if (is_removed())
    	throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        // check a publisher with a same name has not been already declared
        PublisherInfo pInfo = 
            (PublisherInfo) this.publishers.get(publisherName);
        if (pInfo == null) {
            // store information concerning the pusblisher
            PublisherInfo publisherInfo = new PublisherInfo();
            publisherInfo.repId = repId;
            
            // create an event channel associated to this component instance and 
            // this port name
            try {
                org.omg.Components.PortDescription portDesc = 
                    new PortDescriptionImpl();
                portDesc.name = publisherName;
                portDesc.type_id = repId;
                
                org.omg.CosEventChannelAdmin.EventChannel eventChannel =
                    this.context.create_event_channel_for_publisher(portDesc);
                
                publisherInfo.channel = 
                    new cdmw.event.EventChannelInterface(eventChannel);
            } catch (org.omg.Components.CreateFailure cf) {
                System.err.println(publisherName + " event channel creation failure");
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL, 
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch (org.omg.CORBA.SystemException se) {
                System.err.println("System exception raised during event channel creation!");
                throw se;
            }
            
            try {
                publisherInfo.proxyConsumer = 
                    publisherInfo.channel.getPushConsumer(null);
            } catch (org.omg.CORBA.SystemException se) {
                System.err.println("System exception raised during publisher connection to event channel!");
                throw se;
            }
    
            // publisher info completed, store it in map
            this.publishers.put(publisherName, publisherInfo);
        } else {
            System.err.println(publisherName + " already declared");
            throw new AlreadyDoneException();
        }

    }

    /**
     * Returns an event consumer stored in PublisherMap at ck
     * 
     * @param publisher_name   The publisher name.
     * @param ck               The cookie return at consumer subscription.
     *
     * @return a reference on the target consumer
     * @exception org.omg.Components.InvalidConnection if the connection does'nt exist
     */
    protected org.omg.Components.EventConsumerBase get_consumer(
        String publisherName,
        org.omg.Components.Cookie ck)
        throws org.omg.Components.InvalidName, org.omg.Components.InvalidConnection {

        if (ck == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    
        // find publisher
        org.omg.Components.EventConsumerBase result = null;
        PublisherInfo pInfo = (PublisherInfo) this.publishers.get(publisherName);
        
        if (pInfo != null) {
            // publisherName exists
            
            // find subscription
            SubscriberInfo sInfo = (SubscriberInfo) pInfo.subscriptions.get(ck);
            
            if (sInfo != null) {
                result = sInfo.ccmConsumer;
            } else {
                throw new org.omg.Components.InvalidConnection();
            }
        } else {
            throw new org.omg.Components.InvalidName();
        }
    
        return result;

    }

    /**
     * Calls ccm_passivate and ccm_remove on executor. 
     * This function shall be implemented by the well typed class.
     * 
     * @return void
     * @exception Any CORBA Exception.
     */
    protected abstract void advise_executor_of_remove_component();

}

