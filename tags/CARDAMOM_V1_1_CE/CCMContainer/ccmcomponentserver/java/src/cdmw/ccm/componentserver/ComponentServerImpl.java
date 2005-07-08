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


package cdmw.ccm.componentserver;

import cdmw.common.Assert;
import cdmw.common.Options;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.PolicyList;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.ccm.container.ContainerImpl;
import cdmw.ccm.container.HomesServantActivator;

import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ComponentKindValueHelper;

/**
 * The ComponentServer acts as a singleton factory for the creation
 * of Container objects and is used by an Assembly object
 * during the deployment process.
 */
public class ComponentServerImpl
    extends com.thalesgroup.CdmwCcmComponentServer.ComponentServerPOA {

    public static final String SINGLE_THREAD_HOMES_POA_NAME =
        "HomeSerializePOA";

    public static final String MULTI_THREAD_HOMES_POA_NAME =
        "HomeMultithreadPOA";

    public static final String[] componentServerIds = {
      "IDL:thalesgroup.com/CdmwCcmContainer/Container:1.0",
      "IDL:omg.org/Components/Deployment/Container:1.0",
      "IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase:1.0"
    };

    //
    // Attributes
    //

    /**
     * A reference to the ORB pseudo-object
     */
    private org.omg.CORBA.ORB orb;

    /**
     * A parent POA used for the component server
     */
    private org.omg.PortableServer.POA poa;

    /**
     * A POA used for single thread component homes
     */
    private org.omg.PortableServer.POA homesSerializePoa = null;

    /**
     * A POA used for thread-safe component homes
     */
    private org.omg.PortableServer.POA homesMultithreadPoa = null;

    /**
     * A POA Current object
     */
    private org.omg.PortableServer.Current current = null;

    /**
     * A sequence of ConfigValue objects passed in to the
     * <I>create_component_server</I> operation at the Container creation.
     */
    private org.omg.Components.ConfigValue[] configValues;

    /**
     * A reference to the ServerActivator object
     */
    private
        org.omg.Components.Deployment.ServerActivator serverActivator = null;

    /**
     * A reference to the ComponentInstallation object
     */
    private
        com.thalesgroup.CdmwDeployment.ComponentInstallation cpntInstallation
            = null;

    /**
     * A reference to a HomeRegistration for use to register component homes
     * so they may be located by the HomeFinder.
     */
    private org.omg.Components.HomeRegistration homeRegistration;

    /**
     * The CDMW Event Channel Factory.
     */
    private com.thalesgroup.CdmwEvent.EventChannelFactory eventChannelFactory;

    /**
     * The CDMW Process name.
     */
    private String processName;

    /**
     * The CDMW Application name.
     */
    private String applicationName;

    /**
     * A list of all the containers created by the component server.
     */
    private java.util.Vector containers = new java.util.Vector();

    /**
     * Indicates whether the remove operation has been called or not.
     */
    private boolean isRemoved = false;

    public ComponentServerImpl(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA poa,
        org.omg.Components.HomeRegistration homeRegistration,
        com.thalesgroup.CdmwEvent.EventChannelFactory eventChannelFactory,
        String processName,
        String applicationBame,
        org.omg.Components.ConfigValue[] config) {

        this.orb = orb;
        this.poa = poa;
        this.configValues = config;
        this.homeRegistration = homeRegistration;
        this.eventChannelFactory = eventChannelFactory;
        this.processName = processName;
        this.applicationName = applicationName;

        Assert.check(orb != null);
        try {
            org.omg.CORBA.Object obj =
                orb.resolve_initial_references("POACurrent");
            current = org.omg.PortableServer.CurrentHelper.narrow(obj);

            // Create a SingleThread POA for single threaded homes
            // and a Multithread POA for multithread homes.
            org.omg.PortableServer.POAManager poaManager =
                poa.the_POAManager();
            org.omg.CORBA.Policy[] policies = new org.omg.CORBA.Policy[7];
            policies[0] = poa.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID);
            policies[1] = poa.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.PERSISTENT);
            policies[2] = poa.create_servant_retention_policy(
                org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN);
            policies[3] = poa.create_id_uniqueness_policy(
                org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID);
            policies[4] = poa.create_request_processing_policy(
                org.omg.PortableServer.RequestProcessingPolicyValue.USE_SERVANT_MANAGER);
            policies[5] = poa.create_implicit_activation_policy(
                org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);
            // Create a single thread poa
            policies[6] = poa.create_thread_policy(
                org.omg.PortableServer.ThreadPolicyValue.SINGLE_THREAD_MODEL);

            StrategyList poaStrategies = new StrategyList();

            homesSerializePoa = ORBSupport.createPOA(
                poa,
                SINGLE_THREAD_HOMES_POA_NAME,
                poaManager,
                new PolicyList(policies),
                poaStrategies);

            //
            // Create and install servant activator
            //
            org.omg.PortableServer.ServantActivator theActivator =
                new HomesServantActivator();
            homesSerializePoa.set_servant_manager(theActivator);

            // Create a multi thread poa
            policies[6] = poa.create_thread_policy(
                org.omg.PortableServer.ThreadPolicyValue.ORB_CTRL_MODEL);

            homesMultithreadPoa = ORBSupport.createPOA(
                poa,
                MULTI_THREAD_HOMES_POA_NAME,
                poaManager,
                new PolicyList(policies),
                poaStrategies);
            //
            // Create and install servant activator
            //
            theActivator = new HomesServantActivator();
            homesMultithreadPoa.set_servant_manager(theActivator);

        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        }

    }

    public void destroy() {

        if (isRemoved) {
            try {
                removeAllContainers();
            } catch (Exception e) {}
        }
        containers.clear();

    }

    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    /**
     * This operation returns whether the ComponentServer supports the
     * creation of Containers with the specified repository Id.
     *
     * @param repositoryId The Repository Id of the desired Container interface.
     *
     * @return <B>true</B> if the component server supports/creates containers
     * of <I>repository_id</I> type or of derived types. Otherwise it returns
     * <B>false</B>.
     */
    public boolean supports(String repositoryId) {
        boolean result = false;
        for (int i=0; i<componentServerIds.length; i++) {
            if (componentServerIds[i].equals(repositoryId)) {
                result = true;
                break;
            }
        }
        return result;
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //
    /**
     * This operation returns a sequence of CORBA Repository IDs of Containers
     * that can be created by the componentserver.
     *
     * @return A sequence of CORBA Repository IDs of containers that
     * can be created by the component server.
     */
    public String[] get_supported_objects() {
        return componentServerIds;
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/configuration:1.0
    //
    /**
     * Gives access to the ConfigValue objects passed in
     * to the create_component_server operation at the ComponentServer creation.
     *
     * @return A sequence of ConfigValue objects passed in
     * to the create_component_server operation at the ComponentServer creation.
     */
    public org.omg.Components.ConfigValue[] configuration() {
        return this.configValues;
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/get_server_activator:1.0
    //
    /**
     * Gives access to the ServerActivator object
     * that created this ComponentServer.
     *
     * @return Object reference to the ServerActivator object
     * that created this ComponentServer.
     */
    public org.omg.Components.Deployment.ServerActivator
        get_server_activator() {
        return this.serverActivator;
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/create_container:1.0
    //
    /**
     * The create_container operation creates a new Container object within the
     * component server process.
     *
     * @param config Sequence of ConfigValue objects that provide
     * name value pairs used to configure the new Container.
     *
     * <p/> <B>NOTA:</B> These are the configuration values understood
     * by the component server for container creation.
     * <TABLE>
     *   <TR>
     *      <TD><U><B>Config Name</B></U></TD><TD><U><B>Type</B></U></TD>
     *   </TR>
     *   <TR>
     *      <TD>COMPONENT_KIND (*)</TD><TD>ComponentKindValue</TD>
     *   </TR>
     * </TABLE>
     * <p/>
     * <TABLE>
     *   <TR>
     *      <TD>(*)</TD><TD>mandatory</TD>
     *   </TR>
     * </TABLE>
     *
     * @return Object reference to the newly created container object.
     *
     * @exception org.omg.Components.Deployment.InvalidConfiguration
     * Raised if the configuration values are not valid.
     * @exception org.omg.Components.CreateFailure Raised if a container could
     * not be created for internal reasons (such as insufficient resources).
     */
    public org.omg.Components.Deployment.Container
        create_container(org.omg.Components.ConfigValue[] config)
        throws
            org.omg.Components.CreateFailure,
            org.omg.Components.Deployment.InvalidConfiguration {

        // NOTA: These are the configuration values understood
        // by the component server for container creation
        //
        // +====================================+=============================+
        // | Config Name                        | Type                        |
        // +====================================+=============================+
        // | COMPONENT_KIND (*)                 | ComponentKindValue          |
        // +------------------------------------+-----------------------------+
        //
        // (*) mandatory
        //
        // - Get Component kind (compulsory)
        //
        ComponentKindValue componentKind = null;
        boolean found = false;
        String configName =
            com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value;
        for (int count = 0; count < config.length; ++count) {
            if (configName.equals(config[count].name)) {
                componentKind =
                    ComponentKindValueHelper.extract(config[count].value);
                found = true;
                break;
            }
        }
        if (!found) {
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InvalidConfiguration();
        }

        // Only Session container type is supported!
        if (componentKind.value() != ComponentKindValue.SERVICE.value() &&
            componentKind.value() != ComponentKindValue.SESSION.value()) {
            throw new org.omg.CORBA.NO_IMPLEMENT(
                ExceptionMinorCodes.NO_IMPLEMENTComponentKindNotSupported,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        org.omg.Components.Deployment.ComponentServer componentServer = null;
        try {
            org.omg.PortableServer.POA poa = current.get_POA();
            byte[] oid = current.get_object_id();

            org.omg.CORBA.Object obj = poa.id_to_reference(oid);
            componentServer =
                org.omg.Components.Deployment.ComponentServerHelper.narrow(obj);
            if (componentServer == null) {
                throw new org.omg.Components.CreateFailure(
                    com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
            }
        } catch (Exception e) {
            throw new org.omg.Components.CreateFailure(
                com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
        }

        // Create a container instance
        ContainerImpl cont = new ContainerImpl(
            this.orb,
            this.poa,
            this.homesSerializePoa,
            this.homesMultithreadPoa,
            this.cpntInstallation,
            componentServer,
            this.homeRegistration,
            this.eventChannelFactory,
            this.processName,
            this.applicationName,
            config);

        org.omg.Components.Deployment.Container container = cont._this(this.orb);
        containers.add(container);

        return container;

    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/remove_container:1.0
    //
    /**
     * The remove_container operation removes a previously created container
     * and all homes and components that were created in the container.
     *
     * @param cref Reference to the container that should be removed.
     *
     * @exception org.omg.CORBA.BAD_PARAM Raised if the <I>cref</I> parameter
     * was not created by this ComponentServer.
     * @exception org.omg.Components.RemoveFailure Raised if the container
     * or any of its homes could not be removed for any internal reasons.
     */
    public void remove_container(org.omg.Components.Deployment.Container cref)
        throws org.omg.Components.RemoveFailure, org.omg.CORBA.SystemException {

        java.util.Enumeration e = this.containers.elements() ;
        for (; e.hasMoreElements() ;) {
            org.omg.Components.Deployment.Container c =
                (org.omg.Components.Deployment.Container) e.nextElement();
            if (cref._is_equivalent(c)) {
                containers.remove(c);
                return;
            }
        }
        throw new org.omg.CORBA.BAD_PARAM(
            ExceptionMinorCodes.BAD_PARAMUnknownContainerReference,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);

    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/get_containers:1.0
    //
    /**
     * The get_containers operation returns a sequence of all Container objects
     * created by this ComponentServer.
     *
     * @return all Container objects created by this ComponentServer.
     */
    public org.omg.Components.Deployment.Container[] get_containers() {

        org.omg.Components.Deployment.Container[] result =
            new org.omg.Components.Deployment.Container[this.containers.size()];
        java.util.Enumeration e = this.containers.elements();
        int i = 0;
        for( ; e.hasMoreElements(); ) {
            result[i++] =
                (org.omg.Components.Deployment.Container) e.nextElement();
        }
        return result;

    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/remove:1.0
    //
    /**
     * The remove operation removes the component server
     * and all containers, homes, and components
     * that were created in the component server.
     *
     * @exception org.omg.Components.RemoveFailure Raised
     * if the component server or any of its containers
     * could not be removed for any internal reasons.
     */
    public void remove() throws org.omg.Components.RemoveFailure {

        if (isRemoved) {
            throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                ExceptionMinorCodes.OBJECT_NOT_EXIST,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        try {
            byte[] oid = poa.servant_to_id(this);
            poa.deactivate_object(oid);

            // removeAllContainers();

            this.isRemoved = true;
        } catch (Exception ex) {
            throw new org.omg.Components.RemoveFailure(
                com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
        }

    }

    //
    // IDL:thalesgroup.com/CdmwCcmComponentServer/ComponentServer/set_component_installation:1.0
    //
    public void set_component_installation(
        com.thalesgroup.CdmwDeployment.ComponentInstallation cpntInstall) {
        this.cpntInstallation = cpntInstall;
    }

    //
    // IDL:thalesgroup.com/CdmwCcmComponentServer/ComponentServer/set_server_activator:1.0
    //
    public void set_server_activator(
        org.omg.Components.Deployment.ServerActivator serverActivator) {
        this.serverActivator = serverActivator;
    }

    private void removeAllContainers()
        throws org.omg.Components.RemoveFailure {

        java.util.Enumeration e = this.containers.elements();
        for( ; e.hasMoreElements(); ) {
            org.omg.Components.Deployment.Container c =
                (org.omg.Components.Deployment.Container) e.nextElement();
            c.remove();
        }

    }

}

