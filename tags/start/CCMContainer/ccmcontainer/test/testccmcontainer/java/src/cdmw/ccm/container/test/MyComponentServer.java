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


package cdmw.ccm.container.test;

import cdmw.common.Assert;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.ccm.container.ContainerImpl;
import cdmw.ccm.container.HomesServantActivator;

public class MyComponentServer
    extends org.omg.Components.Deployment.ComponentServerPOA {

    private org.omg.CORBA.ORB orb;

    private org.omg.PortableServer.POA poa;

    private org.omg.Components.Deployment.Container container;

    private org.omg.PortableServer.Current current;

    private com.thalesgroup.CdmwDeployment.ComponentInstallation
        componentInstallation;

    private boolean isRemoved;

    public MyComponentServer(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA poa,
        com.thalesgroup.CdmwDeployment.ComponentInstallation
            componentInstallation) {

        this.orb = orb;
        this.poa = poa;
        this.container = null;
        this.current = null;
        this.componentInstallation = componentInstallation;
        this.isRemoved= false;

        Assert.check(orb != null);
        try {
            org.omg.CORBA.Object obj =
                orb.resolve_initial_references("POACurrent");
            current = org.omg.PortableServer.CurrentHelper.narrow(obj);
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {}

    }

    public void destroy() {
        if (isRemoved && this.container != null) {
            try {
                this.container.remove();
                this.container = null;
            } catch (Exception e) {
                // ignore ...
            }
        }
    }

    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/configuration:1.0
    //
    public org.omg.Components.ConfigValue[] configuration() {
        return null;
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/get_server_activator:1.0
    //
    public org.omg.Components.Deployment.ServerActivator get_server_activator() {
        return null;
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/create_container:1.0
    //
    public org.omg.Components.Deployment.Container create_container(
        org.omg.Components.ConfigValue[] config)
        throws
            org.omg.Components.CreateFailure,
            org.omg.Components.Deployment.InvalidConfiguration {

        org.omg.Components.HomeRegistration homeRegistration = null;
        org.omg.Components.Deployment.ComponentServer componentServer = null;

        try {
            org.omg.PortableServer.POA poa = this.current.get_POA();
            byte[] oid = this.current.get_object_id();

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

        org.omg.PortableServer.POA homesSerializePoa;
        org.omg.PortableServer.POA homesMultithreadPoa;
        // Create a SingleThread POA for single threaded homes and a Multithread POA
        // for multithread homes.
        try {
            org.omg.PortableServer.POAManager poaManager = poa.the_POAManager();
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
            policies[6] = poa.create_thread_policy(
                org.omg.PortableServer.ThreadPolicyValue.SINGLE_THREAD_MODEL);


            StrategyList poaStrategies = new StrategyList();

            String SINGLE_THREAD_HOMES_POA_NAME = "HomeSerializePOA";
            String MULTI_THREAD_HOMES_POA_NAME  = "HomeMultithreadPOA";

            homesSerializePoa = ORBSupport.createPOA(
                this.poa,
                SINGLE_THREAD_HOMES_POA_NAME,
                poaManager,
                policies,
                poaStrategies);

            //
            // Create and install servant activator
            //
            org.omg.PortableServer.ServantActivator theActivator =
                new HomesServantActivator();
            try {
                homesSerializePoa.set_servant_manager(theActivator);
            } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {}


            // Create a multi thread poa
            policies[6] = poa.create_thread_policy(
                org.omg.PortableServer.ThreadPolicyValue.ORB_CTRL_MODEL);
            homesMultithreadPoa = ORBSupport.createPOA(
                this.poa,
                MULTI_THREAD_HOMES_POA_NAME,
                poaManager,
                policies,
                poaStrategies);

            //
            // Create and install servant activator
            //
            theActivator = new HomesServantActivator();
            try {
                homesMultithreadPoa.set_servant_manager(theActivator);
            } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {}

        } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        }

        com.thalesgroup.CdmwEvent.EventChannelFactory
            noEventChannelFactory = null;
        String processName = "DummyProcessName";
        String applicationName = "DummyApplicationName";
        // Create a container instance
        ContainerImpl cont = new ContainerImpl(
            this.orb,
            this.poa,
            homesSerializePoa,
            homesMultithreadPoa,
            this.componentInstallation,
            componentServer,
            homeRegistration,
            noEventChannelFactory,
            processName,
            applicationName,
            config);

        this.container = cont._this(this.orb);
        return this.container;

    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/remove_container:1.0
    //
    public void remove_container(org.omg.Components.Deployment.Container cref)
        throws org.omg.Components.RemoveFailure {
        if (this.container != null && this.container._is_equivalent(cref)) {
            this.container.remove();
            this.container = null;
        } else {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMUnknownContainerReference,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/get_containers:1.0
    //
    public org.omg.Components.Deployment.Container[] get_containers() {
        org.omg.Components.Deployment.Container[] containers = null;
        if (this.container == null) {
            containers = new org.omg.Components.Deployment.Container[0];
        } else {
            containers = new org.omg.Components.Deployment.Container[1];
            containers[0]= this.container;
        }
        return containers;
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/remove:1.0
    //
    public void remove() throws org.omg.Components.RemoveFailure {
        if (isRemoved) {
            throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                ExceptionMinorCodes.OBJECT_NOT_EXIST,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        try {
            byte[] oid = poa.servant_to_id(this);
            poa.deactivate_object(oid);

            isRemoved = true;
        } catch (Exception ex) {
            throw new org.omg.Components.RemoveFailure(
                com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
        }
    }

}

