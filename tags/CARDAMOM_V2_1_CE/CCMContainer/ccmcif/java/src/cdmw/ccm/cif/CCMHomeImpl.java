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


package cdmw.ccm.cif;

import cdmw.common.Assert;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.lifecycle.LifeCycleSingleObject;

//
// IDL:thalesgroup.com/CdmwCcmCif/CdmwCCMHome:1.0
//
public abstract class CCMHomeImpl
    extends com.thalesgroup.CdmwCcmCif.CCMHomePOA
    implements 
        cdmw.lifecycle.ServantProviderBaseInterface,
        cdmw.ccm.container.ComponentServantProviderInterface, 
        cdmw.lifecycle.LifeCycleObjectBaseInterface {

    /**
     * Component map with key = stringified oid.
     * 
     */
    protected class ComponentInfo {
        
        /**
         * component reference
         */
        public org.omg.CORBA.Object compRef = null;
        
        /**
         * component servant
         */
        public org.omg.PortableServer.Servant compServant = null;
        
        /**
         * component executor
         */
        public org.omg.Components.EnterpriseComponent compExec = null;
        
        public ComponentInfo() {}
        
    }
    
    /**
     * This inner class is used to delegate protected methods
     * from LifeCycleSingleObject
     * 
     */
    protected class LifeCycleSingleObjectDelegate 
        extends LifeCycleSingleObject {

        public LifeCycleSingleObjectDelegate(
            cdmw.lifecycle.ObjectDeactivator deactivator) {
            super(deactivator);
        }

        public org.omg.PortableServer.POA _getPOA() 
            throws org.omg.PortableServer.CurrentPackage.NoContext {
            return this.getPOA();
        }
        
        public byte[] _getObjectId() 
            throws org.omg.PortableServer.CurrentPackage.NoContext {
            return this.getObjectId();
        }
        
        public void _deactivateObject() 
            throws org.omg.PortableServer.CurrentPackage.NoContext {
            this.deactivateObject();
        }
    
        public void _registerObjectAsRemoved(byte[] oid) {
            this.registerObjectAsRemoved(oid);
        }

    }
    
    /**
     * This inner class is used to delegate protected methods
     * from ServantProviderBase
     * 
     */
    protected class ServantProviderBaseDelegate
        extends cdmw.lifecycle.ServantProviderBase {
            
        public ServantProviderBaseDelegate() {}
        
        public void _cleanupObject(byte[] oid) {
            super.cleanupObject(oid);
        }
    
        // dummy method
        protected String[] getIds() { return null;}
    
        // dummy method
        protected int getNumberOfIds() { return 0;}
        
        // dummy method
        public boolean mayIncarnateMultipleObjects() { return false;}
        
        // dummy method
        public org.omg.PortableServer.Servant lookupServant(byte[] oid)
            throws org.omg.PortableServer.ForwardRequest { return null;}
        
        // dummy method
        public void releaseServant(
            byte[] oid,
            org.omg.PortableServer.POA poa,
            org.omg.PortableServer.Servant servant) {}

        // dummy method
        public String getSupportedInterface(byte[] oid)
            throws org.omg.CORBA.OBJECT_NOT_EXIST { return null;}

    }

    /**
     * home Context
     */
    protected com.thalesgroup.CdmwCcmContainer.CCM2Context context;
    
    /**
     * home id
     */
    protected byte[] oid;
    
    /**
     * home repository id
     */
    protected String repId;
    
    /**
     * component rep id
     */
    protected String compRepId;
    
    /**
     * list of created component
     */
    protected java.util.Hashtable components = new java.util.Hashtable();
    
    /**
     * this attribute is used to delegate LifeCycleSingleObject methods
     */
    protected LifeCycleSingleObjectDelegate lifecycleDelegate;
    
    /**
     * this attribute is used to delegate ServantProviderBase methods
     */
    protected ServantProviderBaseDelegate spbDelegate;

    /**
     * Constructor
     * 
     * @param repId repId home repository ID
     * @param compRepId
     * @param deactivator
     */
    public CCMHomeImpl(String repId, String compRepId, 
        cdmw.lifecycle.ObjectDeactivator deactivator) {
        this.context = null;
        this.lifecycleDelegate = 
            new LifeCycleSingleObjectDelegate(deactivator);
        this.spbDelegate = new ServantProviderBaseDelegate();
        try{
        	this.oid = deactivator.getObjectId();
        }catch (org.omg.PortableServer.CurrentPackage.NoContext nc)
        {
        	System.err.println("System Exception raised in the CCMHome constructor");
        }
        this.repId = repId;
        this.compRepId = compRepId;
    }

    /**
     * Returns an object reference that supports 
     * the CORBA::ComponentIR::ComponentDef interface, 
     * describing the component type associated with the home object.
     * 
     * @return CORBA::ComponentIR::ComponentDef describing 
     * the component type associated with the home object.
     */
    //
    // IDL:omg.org/Components/CCMHome/get_component_def:1.0
    //
    //public org.omg.CORBA.IRObject get_component_def() {
    public org.omg.Components.IRObject get_component_def() {
		if (isRemoved(oid))
			throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        Assert.check(context != null);
        // TODO: Implementation
        return null;
    }

    /**
     * This returns an object reference that supports 
     * the CORBA::ComponentIR::HomeDef interface describing the home type.
     * 
     * @return CORBA::ComponentIR::HomeDef interface describing the home type.
     */
    //
    // IDL:omg.org/Components/CCMHome/get_home_def:1.0
    //
    //public org.omg.CORBA.IRObject get_home_def() {
    public org.omg.Components.IRObject get_home_def() {
    	if (isRemoved(oid))
			throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        Assert.check(context != null);
        // TODO: Implementation
        return (org.omg.Components.IRObject) null;
    }


    /**
     * This causes the component denoted by the reference to cease to exist.
     * Subsequent invocations on the reference will cause an OBJECT_NOT_EXIST
     * system exception to be raised.
     * 
     * @param comp
     * @exception org.omg.CORBA.BAD_PARAM system exception raised 
     * if the component denoted by the parameter does not exist 
     * in the container associated with target home object.
     * @exception org.omg.Components.RemoveFailure exception raised 
     * if application errors.
     */
    //
    // IDL:omg.org/Components/CCMHome/remove_component:1.0
    //
    public void remove_component(org.omg.Components.CCMObject comp) 
        throws org.omg.Components.RemoveFailure {
        
        if (isRemoved(oid))
			throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);	

        Assert.check(this.context != null);

        if (comp == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        
        // check component context is a Session2Context one
        org.omg.Components.Session2Context s2ctx 
            = org.omg.Components.Session2ContextHelper.narrow(this.context);
        
        if (s2ctx != null) {
            try {
                byte[] compOid = s2ctx.get_oid_from_ref(comp);
                String compOidStr = new String(compOid);
                
                ComponentInfo compInfo = 
                    (ComponentInfo) components.get(compOidStr);
                
                if (compInfo != null) {
                    try {
                        // ask to the component its removal
                        comp.remove();
                    } catch (org.omg.Components.RemoveFailure rf) {
                        System.err.println(
                            "Exception raised when removing component");
                        throw rf;
                	} catch (org.omg.CORBA.OBJECT_NOT_EXIST one) {
                        System.err.println(
                            "This component is being removed");
                        throw new org.omg.Components.RemoveFailure(
                    		com.thalesgroup.CdmwDeployment.BAD_COMPONENT_REFERENCE.value);
                    } catch (org.omg.CORBA.SystemException se) {
                        System.err.println(
                            "System exception raised when removing component");
                        throw se;
                    }
                    
                    // remove component from the created component map
                    this.components.remove(compOidStr);
                } else {
                    // this component has not been created by this home
                    System.err.println("This component has not been created " +
                        "by this home or has already been removed!");
                    throw new org.omg.CORBA.BAD_PARAM(
                        ExceptionMinorCodes.BAD_PARAMUnknownComponentReference,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }
            } catch (org.omg.Components.IllegalState is) {
                System.err.println("IllegalState exception raised!");
                throw new org.omg.Components.RemoveFailure(
                    com.thalesgroup.CdmwDeployment.ILLEGAL_STATE.value);
            } catch (org.omg.Components.BadComponentReference bcr) {
                System.err.println("BadComponentReference exception raised!");
                throw new org.omg.Components.RemoveFailure(
                    com.thalesgroup.CdmwDeployment.BAD_COMPONENT_REFERENCE.value);
            }
        } else {
            System.err.println("Context is not a Session2Context!");
            throw new org.omg.Components.RemoveFailure(
                com.thalesgroup.CdmwDeployment.WRONG_CONTEXT_KIND.value);
        }
    
    }

    /**
     * Remove instructs the object to cease to exist. The object reference 
     * for the target is no longer valid after remove successfully completes.
     * <B>NOTA:</B> Any further call to <I>Remove</I> for this object will raise
     * a org.omg.CORBA.OBJECT_NOT_EXIST system exception.
     *
     * @exception org.omg.CosLifeCycle.NotRemovable An implementation 
     * that refuses to remove itself should raise this exception
     */ 
    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
    //
    public void remove() throws org.omg.CosLifeCycle.NotRemovable {
    	
    	if (isRemoved(oid))
			throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    
        Assert.check(this.context != null);

        byte[] oid = (new String("")).getBytes();
        if (!isRemoved(oid)) {
            java.util.Enumeration e = this.components.elements();
            while(e.hasMoreElements()) {
                ComponentInfo compInfo = (ComponentInfo) e.nextElement();
                try {
                    org.omg.Components.CCMObject ccmObj = 
                        org.omg.Components.CCMObjectHelper.narrow(compInfo.compRef);
                    ccmObj.remove();
                } catch (org.omg.Components.RemoveFailure rf) {
                    // Ignore
                    System.err.println("CCMHomeImpl destructor: " + 
                        "error when trying to remove a component !");
                }
            }
        }

        // clean component list
        this.components.clear();
    
        this.lifecycleDelegate.remove();
    
        // release context
        this.context = null;
    
    }

    /**
     * This operation returns whether the CDMW factory supports the 
     * creation of objects supporting the specified repository Id.
     *
     * @param repositoryId The Repository Id of the desired interface.
     * @return <B>true</B> if the factory supports/creates objects of 
     * <I>repositoryId</I> type or of derived types. Otherwise it returns
     * <B>false</B>.
     */
    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    public boolean supports(String repositoryId) {
    	if (isRemoved(oid))
			throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        Assert.check(this.context != null);
        return this.spbDelegate.supports(repositoryId);
    }

    /**
     * This operation returns a sequence of CORBA Repository IDs of CDMW 
     * objects that can be created by the factory.
     *
     * @return A sequence of CORBA Repository IDs of CDMW objects that 
     * can be created by the factory.
     */
    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //
    public String[] get_supported_objects() {
    	if (isRemoved(oid))
			throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        Assert.check(this.context != null);
        return this.spbDelegate.getSupportedObjects();
    }
    
    /**
     * This operation returns a sequence of CORBA Repository IDs of CDMW 
     * objects that can be created by the factory.
     *
     * @return A sequence of CORBA Repository IDs of CDMW objects that 
     * can be created by the factory.
     */
    public String[] getSupportedObjects() {
        return get_supported_objects();
    }
    
    /**
     * This method returns the RepositoryID of the object associated 
     * to the <I>oid</I>.
     * 
     * @param oid ObjectId of the object
     * @return The RepositoryId of the object
     * @exception org.omg.CORBA.OBJECT_NOT_EXIST 
     * The object does not exist any more.
     */
    public String getSupportedInterface(byte[] oid) 
        throws org.omg.CORBA.OBJECT_NOT_EXIST {
        if (isRemoved(oid))
			throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        Assert.check(this.context != null);

        try {
            String name = new String(oid);
            ComponentInfo compInfo = 
                (ComponentInfo) this.components.get(name);
        
            if (compInfo == null) {
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
            else {
                return this.compRepId;
            }
        // Pb with ObjectId
        } catch (org.omg.CORBA.BAD_PARAM bp) {
            throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                ExceptionMinorCodes.OBJECT_NOT_EXIST,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    }
       
    /**
     * This method provides a servant for the CORBA object with
     * the specified ObjectId. It is called by the Container
     * whenever a servant is needed to incarnate an object.
     * 
     * @param oid ObjectId of the object to be incarnated
     * @return A servant incarnating the object 
     * @exception org.omg.PortableServer.ForwardRequest Indicates 
     * to the Life Cycle Framework ORB that the SevantProvider cannot handle 
     * this object and requests can be handled to the returned IOR. 
     * If this exception is translated/forwarded to the client-side ORB, 
     * the request invocation will be transparently re-transmitted 
     * to the returned object.
     */
    public org.omg.PortableServer.Servant 
        lookupServant(byte[] oid) 
        throws org.omg.PortableServer.ForwardRequest {
        if (isRemoved(oid))
			throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        Assert.check(this.context != null);
    
        System.out.println("Entering in lookupServant...");
    
        org.omg.PortableServer.Servant servant = null;
    
        try {
            // check if it is a component oid or a facet/consumer oid or 
            // a local push consumer oid.
            String objectId = new String(oid);
            java.util.Vector names = CCMUtils.readOid(objectId);
    
            String compOid = (String) names.elementAt(0);
            System.out.println("oid = " + objectId);
            System.out.println("compOid = " + compOid);
            
            // check a component with this oid has been created in this home
            ComponentInfo compInfo = 
                (ComponentInfo) this.components.get(compOid);
            
            if (compInfo == null) {
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO); 
            }

            if (compInfo.compServant != null) {
                CCMObjectImpl _comp = getCCMObjectImpl(compInfo.compServant);

                if (_comp.is_removed()) {
                    // because the call to release servant comes from POA,
                    // it may occurs that the object is deactivated but
                    // release_servant not called and so the component has
                    // not been deleted from the map.
                    throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                        ExceptionMinorCodes.OBJECT_NOT_EXIST,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }
            }
            
            switch (names.size()) {
            case 1: // oid is a component oid
                // Now we can create the component servant and store servant
                // while facet servant are part of component servant
                compInfo.compServant = create_component_servant(
                    compOid, this.context, compInfo.compExec);
                Assert.check(compInfo.compServant != null);
                
                // NOTA: comp executor is not null if an executor has been created at ref 
                // creation that is a factory operation has been used to create ref.
                
                servant = compInfo.compServant;
                break;
    
            case 2: // oid is a facet/consumer oid
                String name = (String) names.elementAt(1);
                System.out.println("name = " + name);

                Assert.check(compInfo.compServant != null);
                CCMObjectImpl comp = getCCMObjectImpl(compInfo.compServant);
                
                servant = comp.get_facet_servant(name);
                break;
                
            case 3:  // oid is a local push consumer
                Assert.check(compInfo.compServant != null);
                comp = getCCMObjectImpl(compInfo.compServant);
                
                name = (String) names.elementAt(1);
                String ckValue = (String) names.elementAt(2);
                System.out.println("name = " + name);
                System.out.println("ckValue = " + ckValue);
                
                servant = comp.get_local_push_consumer_servant(name, ckValue);
                break;
    
            default:
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
            
        // Pb with ObjectId
        } catch (org.omg.CORBA.BAD_PARAM bp) {
            System.err.println("lookupServant: Pb with ObjectId!");
            throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                ExceptionMinorCodes.OBJECT_NOT_EXIST,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (org.omg.Components.CreateFailure cf) {
            System.err.println("lookupServant: Executor create failure!");
            throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                ExceptionMinorCodes.OBJECT_NOT_EXIST,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CORBA.OBJECT_NOT_EXIST one) {
            System.err.println("lookupServant: Object not exist!");
            throw one;
        }
    
        return servant;

    }
    
    /**
     * This method is called by Container whenever it is done with 
     * the specified servant. The ServantProvider is free to handle it at will.
     * 
     * @param oid ObjectId of the object incarnated by the servant
     * @param poa The POA where the object is activated
     * @param servant The servant in question
     */
    public void releaseServant(byte[] oid,
        org.omg.PortableServer.POA poa, 
        org.omg.PortableServer.Servant servant) {


        System.out.println("Entering in releaseServant...");
    
        try {
            // check if it is a component oid or a facet oid
            String objectId = new String(oid);
            java.util.Vector names = CCMUtils.readOid(objectId);
    
            String compOid = (String) names.elementAt(0);
            System.out.println("oid= " + objectId);
            System.out.println("compOid= " + compOid);
    
            // check a component with this oid has been created in this home
            ComponentInfo compInfo = 
                (ComponentInfo) this.components.get(compOid);
            
            if (compInfo  == null) {
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
    
            switch (names.size()) {
            case 1: // oid is a component oid
                // check if operation removed has been requested on component
                Assert.check(servant != null);
                CCMObjectImpl component = getCCMObjectImpl(servant);
                
                Assert.check(component != null);
                if (component.is_removed()) {
                    System.out.println(
                        "Component is removed, delete it from map");
                    // if so remove component from the map
                    this.components.remove(compOid);            
                }
                // release servant, nothing to do in Java
                break;
    
            case 2: // oid is a facet/consumer oid
                String name = (String) names.elementAt(1);
                System.out.println("name= " + name);
                // release servant, nothing to do in Java
                break;
                
            case 3:  // oid is a local push consumer
                name = (String) names.elementAt(1);
                String ckValue = (String) names.elementAt(2);
                System.out.println("name= " + name);
                System.out.println("ckValue= " + ckValue);
                // release servant, nothing to do in Java
                break;
    
            default:
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch (org.omg.CORBA.BAD_PARAM bp) {
            // Pb with ObjectId
            System.err.println("releaseServant: Pb with ObjectId!");
            throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                ExceptionMinorCodes.OBJECT_NOT_EXIST,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

    }

    /**
     * This method is called by Container after Home creation.
     * 
     * @param ctxt Component context given by container.
     * @exception org.omg.Components.CCMException Any Component exception
     */
    public void set_context(
        com.thalesgroup.CdmwCcmContainer.CCM2Context ctxt)
        throws org.omg.Components.CCMException {
        if (isRemoved(oid))
			throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        this.context = ctxt;
    }
            
    /**
     * This creates new component reference.
     * 
     * @return A reference on the new component.
     * @exception org.omg.Components.CreateFailure if application errors.
     */ 
    protected org.omg.CORBA.Object create_component_ref(
        org.omg.Components.EnterpriseComponent compExec) 
        throws org.omg.Components.CreateFailure {
        	
        if (isRemoved(oid))
			throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

        Assert.check(this.context != null);

        // check component context is a Session2Context one
        org.omg.Components.Session2Context s2ctx 
            = org.omg.Components.Session2ContextHelper.narrow(this.context);
        ComponentInfo compInfo = new ComponentInfo();
        
        if (s2ctx != null) {
            try {
                compInfo.compRef = s2ctx.create_ref(this.compRepId);
    
                if (compInfo.compRef == null) {
                    System.err.println("Create_ref has failed!");
                    throw new org.omg.CORBA.INTERNAL(
                        ExceptionMinorCodes.INTERNAL, 
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }
        
                // set executor => executor is not nil if factory operation is used!
                compInfo.compExec = compExec;
        
                byte[] compOid = s2ctx.get_oid_from_ref(compInfo.compRef);
                String compOidStr = new String(compOid);
        
                this.components.put(compOidStr, compInfo);
            } catch (org.omg.Components.IllegalState is) {
                System.err.println("IllegalState exception raised!");
                throw new org.omg.Components.CreateFailure(
                    com.thalesgroup.CdmwDeployment.ILLEGAL_STATE.value);
            } catch (org.omg.Components.BadComponentReference bcr) {
                System.err.println("BadComponentReference exception raised!");
                throw new org.omg.Components.CreateFailure(
                    com.thalesgroup.CdmwDeployment.BAD_COMPONENT_REFERENCE.value);
            } catch (org.omg.CORBA.SystemException se) {
                System.err.println(se.toString());
                throw se;
            }
        } else {
            System.err.println("Context is not a Session2Context!");
            throw new org.omg.Components.CreateFailure(
                com.thalesgroup.CdmwDeployment.WRONG_CONTEXT_KIND.value);
        }
        
        return compInfo.compRef;
    }
    
    /**
     * This creates new component reference.
     * 
     * @return A reference on the new component.
     * @exception org.omg.Components.CreateFailure if application errors.
     */ 
    protected org.omg.CORBA.Object create_component_ref() 
        throws org.omg.Components.CreateFailure {
        return create_component_ref(null);
    }

    /**
     * This creates new component servant.
     * This shall be implemented by the well typed home servant.
     *
     * @param compOid 
     * @param ctx The component context
     * @param executor The component executor
     * @return The new component servant.
     * @exception org.omg.Components.CreateFailure
     */ 
    protected abstract org.omg.PortableServer.Servant create_component_servant(
        String compOid,
        com.thalesgroup.CdmwCcmContainer.CCM2Context ctx,
        org.omg.Components.EnterpriseComponent executor)
        throws org.omg.Components.CreateFailure;

    protected abstract CCMObjectImpl 
        getCCMObjectImpl(org.omg.PortableServer.Servant servant);

    /**
     * the following methods delegate LifeCycleSingleObject methods
     * 
     */

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/copy:1.0
    //
    public org.omg.CosLifeCycle.LifeCycleObject copy(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws
            org.omg.CosLifeCycle.NoFactory,
            org.omg.CosLifeCycle.NotCopyable,
            org.omg.CosLifeCycle.InvalidCriteria,
            org.omg.CosLifeCycle.CannotMeetCriteria {
        return this.lifecycleDelegate.copy(there, the_criteria);
    }
    
    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/move:1.0
    //
    public void move(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws
            org.omg.CosLifeCycle.NoFactory,
            org.omg.CosLifeCycle.NotMovable,
            org.omg.CosLifeCycle.InvalidCriteria,
            org.omg.CosLifeCycle.CannotMeetCriteria {
        this.lifecycleDelegate.move(there, the_criteria);
    }
    
    protected final org.omg.PortableServer.POA getPOA() 
        throws org.omg.PortableServer.CurrentPackage.NoContext {
        return this.lifecycleDelegate._getPOA();
    }
    
    protected final byte[] getObjectId() 
        throws org.omg.PortableServer.CurrentPackage.NoContext {
        return this.lifecycleDelegate._getObjectId();
    }
    
    protected final void deactivateObject() 
        throws org.omg.PortableServer.CurrentPackage.NoContext {
        this.lifecycleDelegate._deactivateObject();
    }
    
    public boolean isRemoved(byte[] oid) {
        return this.lifecycleDelegate.isRemoved(oid);
    }

    protected void registerObjectAsRemoved(byte[] oid) {
        this.lifecycleDelegate._registerObjectAsRemoved(oid);
    }
    
    /**
     * the following methods delegate ServantProviderBase methods
     * 
     */
    
    public void setCleanupNotifier(cdmw.lifecycle.CleanupNotifier notifier) {
        this.spbDelegate.setCleanupNotifier(notifier);
    }
    
    protected void cleanupObject(byte[] oid) {
        this.spbDelegate._cleanupObject(oid);
    }
    
    protected abstract String[] getIds();
    
    protected abstract int getNumberOfIds();

}

