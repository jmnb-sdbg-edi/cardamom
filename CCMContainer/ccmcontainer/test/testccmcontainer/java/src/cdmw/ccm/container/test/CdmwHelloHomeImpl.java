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


package cdmw.ccm.container.test;

import cdmw.common.Assert;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.lifecycle.ObjectDeactivator;
import cdmw.lifecycle.CleanupNotifier;

public class CdmwHelloHomeImpl
    extends com.thalesgroup.Cdmw_HelloModule.HelloHomePOA
    implements 
        cdmw.ccm.container.ComponentServantProviderInterface, 
        cdmw.lifecycle.LifeCycleObjectBaseInterface {

    // in C++, this class extends
    // Cdmw::CCM::Container::ComponentServantProvider,
    // Cdmw::LifeCycle::LifeCycleSingleObject,
    // Cdmw::OrbSupport::RefCountServantBase
    // in Java, we use delegation, direct code insertion and implementations
    // the result is quite dirty but that's a test... :)

    public static final String[] ids = {
        "IDL:thalesgroup.com/Cdmw_HelloModule/HelloHome:1.0",
        "IDL:thalesgroup.com/HelloModule/HelloHome:1.0",
        "IDL:thalesgroup.com/HelloModule/HelloHomeImplicit:1.0",
        "IDL:thalesgroup.com/HelloModule/HelloHomeExplicit:1.0",
        "IDL:omg.org/Components/KeylessCCMHome:1.0",
        "IDL:omg.org/Components/CCMHome:1.0",
        "IDL:omg.org/Components/HomeConfiguration:1.0",
        "IDL:thalesgroup.com/CdmwCcmCif/CCMHome:1.0",
        "IDL:omg.org/CosLifeCycle/LifeCycleObject:1.0",
        "IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase:1.0"
    };

    private com.thalesgroup.HelloModule.CCM_HelloHome homeExecutor;

    private com.thalesgroup.CdmwCcmContainer.Session2Context sessionContext;

    private ObjectDeactivator objectDeactivator;

    private boolean removed = false;

    private CleanupNotifier cleanupNotifier = null;

    private java.util.Vector ccmObjects = new java.util.Vector();

    /**
    * Constructor
    *
    * @param homeExecutor
    * @param deactivator
    */
    public CdmwHelloHomeImpl(
        org.omg.Components.HomeExecutorBase homeExecutor,
        ObjectDeactivator deactivator) {

        this.objectDeactivator = deactivator;
        this.homeExecutor =
            com.thalesgroup.HelloModule.CCM_HelloHomeHelper.narrow(homeExecutor);
        this.sessionContext = null;

        System.out.println("CdmwHelloHomeImpl created.");
        Assert.check(this.homeExecutor != null);

    }

    //
    // IDL:omg.org/Components/CCMHome/get_component_def:1.0
    //
    //public org.omg.CORBA.IRObject get_component_def() {
    public org.omg.Components.IRObject get_component_def() {
        return null;
    }

    //
    // IDL:omg.org/Components/CCMHome/get_home_def:1.0
    //
    //public org.omg.CORBA.IRObject get_home_def() {
    public org.omg.Components.IRObject get_home_def() {
        return null;
    }

    //
    // IDL:omg.org/Components/CCMHome/remove_component:1.0
    //
    public void remove_component(org.omg.Components.CCMObject comp)
        throws org.omg.Components.RemoveFailure {

        com.thalesgroup.HelloModule.HelloWorld hello =
            com.thalesgroup.HelloModule.HelloWorldHelper.narrow(comp);
        if (hello == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        java.util.Enumeration e = ccmObjects.elements();
        for( ; e.hasMoreElements(); ) {
            com.thalesgroup.HelloModule.HelloWorld element =
                (com.thalesgroup.HelloModule.HelloWorld) e.nextElement();
            if (element._is_equivalent(hello)) {
                element.remove();
                ccmObjects.remove(element);
                return;
            }
        }
        throw new org.omg.CORBA.BAD_PARAM(
            ExceptionMinorCodes.BAD_PARAMUnknownComponentReference,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);

    }

    //
    // IDL:thalesgroup.com/HelloModule/HelloHomeImplicit/create:1.0
    //
    public com.thalesgroup.HelloModule.HelloWorld create()
        throws org.omg.Components.CreateFailure {

        String repid= "IDL:thalesgroup.com/HelloModule/HelloWorld:1.0";
        org.omg.CORBA.Object obj = sessionContext.create_ref(repid);
        com.thalesgroup.HelloModule.HelloWorld comp =
            com.thalesgroup.HelloModule.HelloWorldHelper.narrow(obj);
        ccmObjects.add(comp);
        return comp;

    }

    //
    // IDL:omg.org/Components/KeylessCCMHome/create_component:1.0
    //
    public org.omg.Components.CCMObject create_component()
        throws org.omg.Components.CreateFailure {
        return create();
    }


    //
    // IDL:omg.org/Components/HomeConfiguration/set_configurator:1.0
    //
    public void set_configurator(org.omg.Components.Configurator cfg) {}

    //
    // IDL:omg.org/Components/HomeConfiguration/set_configuration_values:1.0
    //
    public void
        set_configuration_values(org.omg.Components.ConfigValue[] config) {}

    //
    // IDL:omg.org/Components/HomeConfiguration/complete_component_configuration:1.0
    //
    public void complete_component_configuration(boolean b) {}

    //
    // IDL:omg.org/Components/HomeConfiguration/disable_home_configuration:1.0
    //
    public void disable_home_configuration() {}

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    public boolean supports(String repository_id) {
        boolean result = false;
        String[] ids = getIds();
        for(int i = 0 ; i < ids.length ; ++i) {
          if( repository_id.equals(ids[i]) ) {
              result = true;
              break;
          }
        }
        return result;
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //
    public String[] get_supported_objects() {
        return getIds();
    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/copy:1.0
    //
    public org.omg.CosLifeCycle.LifeCycleObject copy(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws org.omg.CosLifeCycle.NoFactory,
                org.omg.CosLifeCycle.NotCopyable,
                org.omg.CosLifeCycle.InvalidCriteria,
                org.omg.CosLifeCycle.CannotMeetCriteria {

        throw new org.omg.CORBA.NO_IMPLEMENT(
            ExceptionMinorCodes.NO_IMPLEMENT,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);

    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/move:1.0
    //
    public void move(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws org.omg.CosLifeCycle.NoFactory,
                org.omg.CosLifeCycle.NotMovable,
                org.omg.CosLifeCycle.InvalidCriteria,
                org.omg.CosLifeCycle.CannotMeetCriteria {

        throw new org.omg.CORBA.NO_IMPLEMENT(
            ExceptionMinorCodes.NO_IMPLEMENT,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);

    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
    //
    public void remove() throws org.omg.CosLifeCycle.NotRemovable {
        // Remove all components
        java.util.Enumeration e = ccmObjects.elements();
        for( ; e.hasMoreElements(); ) {
            com.thalesgroup.HelloModule.HelloWorld hello =
                (com.thalesgroup.HelloModule.HelloWorld) e.nextElement();
            try {
                hello.remove();
            } catch(org.omg.Components.RemoveFailure rf) {
                throw new org.omg.CosLifeCycle.NotRemovable();
            }
        }
        ccmObjects.clear();
        try {
            byte[] oid = this.objectDeactivator.getObjectId();
            if (isRemoved(oid)) {
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }

            this.objectDeactivator.deactivateObject();
            registerObjectAsRemoved(oid);

        } catch (org.omg.PortableServer.CurrentPackage.NoContext nc) {
            // Not called within the context of an operation invocation
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        // release context
        sessionContext = null;
    }

    // From cdmw.ccm.container.ComponentServantProviderInterface
    public void set_context(com.thalesgroup.CdmwCcmContainer.CCM2Context ctxt)
        throws org.omg.Components.CCMException {

        this.sessionContext =
            com.thalesgroup.CdmwCcmContainer.Session2ContextHelper.narrow(ctxt);

    }

    // From cdmw.lifecycle.ServantProviderBase
    public void setCleanupNotifier(CleanupNotifier notifier) {
        this.cleanupNotifier = notifier;
    }

    public boolean mayIncarnateMultipleObjects() {
        return false;
    }

    public org.omg.PortableServer.Servant lookupServant(byte[] oid)
        throws org.omg.PortableServer.ForwardRequest {

        String s = new String(oid);
        System.out.println("Looking for component <" + s + "> ...");
        System.out.println("Creating component executor...");
        try {
            org.omg.Components.EnterpriseComponent exec = homeExecutor.create();
        } catch(org.omg.Components.CCMException ccme) {
            throw new org.omg.PortableServer.ForwardRequest();
        }

        return new HelloWorldImpl(sessionContext);

    }

    public void releaseServant(
        byte[] oid,
        org.omg.PortableServer.POA poa,
        org.omg.PortableServer.Servant servant) {

        String s = new String(oid);
        System.out.println("Releasing component <" + s + "> ...");
        HelloWorldImpl hello = (HelloWorldImpl) servant;
        if (hello != null && hello.isRemoved()) {
            System.out.println("Component <" + s + "> is removed!");
        }

    }

    public String[] getSupportedObjects() {
        return getIds();
    }

    public String getSupportedInterface(byte[] oid)
        throws org.omg.CORBA.OBJECT_NOT_EXIST {
        return "";
    }

    protected String[] getIds() {
        return ids;
    }

    protected int getNumberOfIds() {
        return ids.length;
    }

    // from cdmw.lifecycle.LifeCycleObjectBaseInterface
    public boolean isRemoved(byte[] oid) {
        return this.removed;
    }

    protected void registerObjectAsRemoved(byte[] oid) {
        this.removed = true;
    }
    
    // the following two methods must be implemented in order to
    // work with the container
    
    public static org.omg.PortableServer.Servant allocate(
        org.omg.Components.HomeExecutorBase homeExecutor,
        cdmw.lifecycle.ObjectDeactivator deactivator) {
        return new CdmwHelloHomeImpl(homeExecutor, deactivator);
    }
     
    public static org.omg.PortableServer.Servant getDelegate(
        org.omg.PortableServer.Servant servant) {
        return servant;
    }

}

