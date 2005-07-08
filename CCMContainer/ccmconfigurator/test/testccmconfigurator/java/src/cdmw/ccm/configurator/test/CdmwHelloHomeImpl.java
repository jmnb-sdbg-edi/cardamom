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


package cdmw.ccm.configurator.test;


public class CdmwHelloHomeImpl
    extends com.thalesgroup.Cdmw_HelloModule.HelloHomePOA {

    private org.omg.Components.HomeExecutorBase homeExecutor;
    private org.omg.Components.CCM2Context sessionContext;
    private cdmw.lifecycle.ObjectDeactivator deactivator;
    private boolean removed = false;

    private final static String[] ids = {
          "IDL:thalesgroup.com/Cdmw_HelloModule/HelloHome:1.0",
          "IDL:thalesgroup.com/HelloModule/HelloHome:1.0",
          "IDL:thalesgroup.com/HelloModule/HelloHomeImplicit:1.0",
          "IDL:thalesgroup.com/HelloModule/HelloHomeExplicit:1.0",
          "IDL:omg.org/Components/KeylessCCMHome:1.0",
          "IDL:omg.org/Components/CCMHome:1.0",
          "IDL:omg.org/Components/HomeConfiguration:1.0",
          "IDL:thalesgroup.com/CdmwCcmCif/CCMHome:1.0",
          "IDL:omg.org/Coslifecycle/lifecycleObject:1.0",
          "IDL:thalesgroup.com/Cdmwlifecycle/FactoryBase:1.0"
    };

    /**
     * Constructor
     *
     *@param deactivator
     *@param homeExecutor
     */
    public CdmwHelloHomeImpl(org.omg.Components.HomeExecutorBase homeExecutor,
        cdmw.lifecycle.ObjectDeactivator deactivator) {
        this.deactivator = deactivator;
        this.homeExecutor = homeExecutor;
        this.sessionContext = null;
    }

    //
    // IDL:thalesgroup.com/HelloModule/HelloHomeImplicit/create:1.0
    //
    public com.thalesgroup.HelloModule.HelloWorld create()
        throws org.omg.Components.CreateFailure {
        return null;
    }

    //
    // IDL:omg.org/Components/HomeConfiguration/set_configurator:1.0
    //
    public void set_configurator(org.omg.Components.Configurator cfg) {
        // do nothing
    }

    //
    // IDL:omg.org/Components/HomeConfiguration/set_configuration_values:1.0
    //
    public void set_configuration_values(
        org.omg.Components.ConfigValue[] config) {
        // do nothing
    }

    //
    // IDL:omg.org/Components/HomeConfiguration/complete_component_configuration:1.0
    //
    public void complete_component_configuration(boolean b) {
        // do nothing
    }

    //
    // IDL:omg.org/Components/HomeConfiguration/disable_home_configuration:1.0
    //
    public void disable_home_configuration() {
        // do nothing
    }

    //
    // IDL:omg.org/Components/KeylessCCMHome/create_component:1.0
    //
    public org.omg.Components.CCMObject create_component()
        throws org.omg.Components.CreateFailure {
        return null;
    }

    //
    // IDL:omg.org/Components/CCMHome/get_component_def:1.0
    //
    public org.omg.Components.IRObject get_component_def() {
        return null;
    }

    //
    // IDL:omg.org/Components/CCMHome/get_home_def:1.0
    //
    public org.omg.Components.IRObject get_home_def() {
        return null;
    }

    //
    // IDL:omg.org/Components/CCMHome/remove_component:1.0
    //
    public void remove_component(org.omg.Components.CCMObject comp)
        throws org.omg.Components.RemoveFailure {
        // do nothing
    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/copy:1.0
    //
    public org.omg.CosLifeCycle.LifeCycleObject copy(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws  org.omg.CosLifeCycle.NoFactory,
                org.omg.CosLifeCycle.NotCopyable,
                org.omg.CosLifeCycle.InvalidCriteria,
                org.omg.CosLifeCycle.CannotMeetCriteria {

        throw new org.omg.CORBA.NO_IMPLEMENT(
                cdmw.orbsupport.ExceptionMinorCodes.NO_IMPLEMENT,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/move:1.0
    //
    public void move(org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws  org.omg.CosLifeCycle.NoFactory,
                org.omg.CosLifeCycle.NotMovable,
                org.omg.CosLifeCycle.InvalidCriteria,
                org.omg.CosLifeCycle.CannotMeetCriteria {

        throw new org.omg.CORBA.NO_IMPLEMENT(
                cdmw.orbsupport.ExceptionMinorCodes.NO_IMPLEMENT,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);

    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
    //
    public void remove() throws org.omg.CosLifeCycle.NotRemovable {

        try {
            byte[] oid = this.deactivator.getObjectId();
            if (isRemoved(oid)) {
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    cdmw.orbsupport.ExceptionMinorCodes.OBJECT_NOT_EXIST,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }

            this.deactivator.deactivateObject();
            registerObjectAsRemoved(oid);

        } catch (org.omg.PortableServer.CurrentPackage.NoContext nc) {
            // Not called within the context of an operation invocation
            throw new org.omg.CORBA.INTERNAL(
                cdmw.orbsupport.ExceptionMinorCodes.INTERNAL,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

    }

    public boolean isRemoved(byte[] oid) {
        return this.removed;
    }

    protected void registerObjectAsRemoved(byte[] oid) {
        this.removed = true;
    }

    //
    // IDL:thalesgroup.com/Cdmwlifecycle/FactoryBase/supports:1.0
    //
    public boolean supports(String repositoryId) {
        boolean result = false;
        String[] ids = getIds();
        for(int i = 0 ; i < ids.length ; ++i) {
          if( repositoryId.equals(ids[i]) ) {
              result = true;
              break;
          }
        }
        return result;
    }

    //
    // IDL:thalesgroup.com/Cdmwlifecycle/FactoryBase/get_supported_objects:1.0
    //
    public String[] get_supported_objects() {
        return getIds();
    }

    // From Cdmw::CCM::Container::ComponentServantProvider
    public void set_context(org.omg.Components.CCM2Context ctxt) {
        this.sessionContext = ctxt;
    }

    // From Cdmw::lifecycle::ServantProviderBase
    public boolean may_incarnate_multiple_objects() {
        return false;
    }

    public org.omg.PortableServer.Servant lookup_servant(byte[] oid)
        throws org.omg.PortableServer.ForwardRequest {
        return null;
    }

    public void release_servant(byte[] oid, org.omg.PortableServer.POA poa,
        org.omg.PortableServer.Servant servant) {
        // do nothing
    }

    public String get_supported_interface(byte[] oid)
        throws org.omg.CORBA.OBJECT_NOT_EXIST {
        return "";
    }

    protected String[] getIds() {
        return ids;
    }

    protected int getNumberOfIds() {
        return ids.length;
    }

}

