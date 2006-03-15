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


package cdmw.ccm.cif.test;

import cdmw.testutils.Testable;
import cdmw.ccm.deployment.ConfigValueImpl;

public class TypedBasicDemoTest extends Testable {
    
    public static final String SERVER_HOME_REP_ID = 
        "IDL:thalesgroup.com/Cdmw_BasicDemo/ServerHome:1.0";
    public static final String SERVER_HOME_SERVANT_NAME = 
        "Cdmw.CCM.CIF.CdmwBasicDemo.SessionServerHome_impl";
    public static final String SERVER_HOME_UID = "ServerHome_1";
    public static final String SERVER_HOME_ENTRY_POINT = 
        "cdmw.ccm.cif.test.CCMServerHomeImpl.createCCMServerHome";
    public static final String SERVER_NAME = "R2D2";

    public static final String CLIENT_HOME_REP_ID = 
        "IDL:thalesgroup.com/Cdmw_BasicDemo/ClientHome:1.0";
    public static final String CLIENT_HOME_SERVANT_NAME = 
        "Cdmw.CCM.CIF.CdmwBasicDemo.SessionClientHome_impl";
    public static final String CLIENT_HOME_UID = "ClientHome_1";
    public static final String CLIENT_HOME_ENTRY_POINT = 
        "cdmw.ccm.cif.test.CCMClientHomeImpl.createCCMClientHome";

    public static final String DISPLAY_FACET_NAME = "for_clients";
    public static final String INVALID_FACET_NAME = "invalid_facet";

    public static final String DISPLAY_RECEP_NAME = "to_server";
    public static final String REGISTER_RECEP_NAME = "to_register";
    public static final String INVALID_RECEP_NAME = "invalid_recep";

    public static final String EVENT_REP_ID = 
        "IDL:acme.com/BasicDemo/Event:1.0";
    public static final String EVENT_ENTRYPOINT = 
        "cdmw.ccm.cif.test.EventFactoryImpl.createEventFactory";

    private org.omg.CORBA.ORB orb;

    private org.omg.Components.Deployment.Container clientContainer;
    private org.omg.Components.Deployment.Container serverContainer;

    public TypedBasicDemoTest(org.omg.CORBA.ORB orb, String containerIORClient, String containerIORServer) {
        this.name = "cdmw.ccm.cif.TypedBasicDemoTest";
        this.out= System.out;
        this.orb = orb;
        try {
            org.omg.CORBA.Object obj = this.orb.string_to_object(containerIORClient);
            if (obj == null) {
                println("TEST FAILED : exception raised during test initialisation!");
            } else {
                this.clientContainer = 
                    org.omg.Components.Deployment.ContainerHelper.narrow(obj);
            }
        } catch (org.omg.CORBA.SystemException se) {
            println("TEST FAILED : system exception raised during test initialisation!");
            throw se;
        } catch (Exception e) {
            println("TEST FAILED : unexpected exception raised during test initialisation!");
        }
        try {
            org.omg.CORBA.Object obj = this.orb.string_to_object(containerIORServer);
            if (obj == null) {
                println("TEST FAILED : exception raised during test initialisation!");
            } else {
                this.serverContainer = 
                    org.omg.Components.Deployment.ContainerHelper.narrow(obj);
            }
        } catch (org.omg.CORBA.SystemException se) {
            println("TEST FAILED : system exception raised during test initialisation!");
            throw se;
        } catch (Exception e) {
            println("TEST FAILED : unexpected exception raised during test initialisation!");
        }
    }
    
    public void doTests() {
    
        // set number of requested successfull tests
        setNbOfRequestedTestOK(23);
        
        boolean testOk = false;
        org.omg.Components.CCMHome serverHome = null;
        com.acme.BasicDemo.ServerHome basicServerHome = null;
        org.omg.Components.CCMObject ccmServerComp = null;
        com.acme.BasicDemo.Server basicServerComp = null;
    
        org.omg.Components.CCMHome clientHome = null;
        com.acme.BasicDemo.ClientHome basicClientHome = null;
        org.omg.Components.CCMObject ccmClientComp = null;
        com.acme.BasicDemo.Client basicClientComp = null;
    
        com.acme.BasicDemo.Display displayFacetObj = null;
    
    
        // This tests CIF classes
        //-------------------------------------------------
        println("CIF classes Tests");
        
        try {
            println("Install a ServerHome");
            testOk = false;
            
            try {
                org.omg.Components.ConfigValue[] configValues = 
                    new org.omg.Components.ConfigValue[5];

                org.omg.CORBA.Any value = orb.create_any();
                com.thalesgroup.CdmwDeployment.ComponentKindValueHelper.insert(
                    value, 
                    com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);
                configValues[0] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value, 
                    value);

                value = orb.create_any();
                com.thalesgroup.CdmwDeployment.ServantLifetimeValueHelper.insert(
                    value, 
                    com.thalesgroup.CdmwDeployment.ServantLifetimeValue.CONTAINER_LIFETIME);
                configValues[1] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.SERVANT_LIFETIME.value, 
                    value);

                value = orb.create_any();
                com.thalesgroup.CdmwDeployment.ThreadingPolicyValueHelper.insert(
                    value, 
                    com.thalesgroup.CdmwDeployment.ThreadingPolicyValue.MULTITHREAD);
                configValues[2] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.THREADING_POLICY.value, 
                    value);

                value = orb.create_any();
                value.insert_string(SERVER_HOME_REP_ID);
                configValues[3] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.HOME_REPOSITORY_ID.value, 
                    value);

                value = orb.create_any();
                value.insert_string(SERVER_HOME_SERVANT_NAME);
                configValues[4] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.HOME_SERVANT_CLASSNAME.value, 
                    value);

                serverHome = this.serverContainer.install_home(
                    SERVER_HOME_UID,
                    SERVER_HOME_ENTRY_POINT,
                    configValues);
                testOk = true;
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
            } catch (org.omg.Components.Deployment.InstallationFailure f) {
                println("Unexpected exception raised: " + 
                    "Components::Deployment::InstallationFailure " + f.reason);
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
            }
            check(testOk);
            
            println("Installing a ClientHome...");
            testOk = false;
    
            try {
                org.omg.Components.ConfigValue[] configValues = 
                    new org.omg.Components.ConfigValue[6];
                
                org.omg.CORBA.Any value = orb.create_any();
                com.thalesgroup.CdmwDeployment.ComponentKindValueHelper.insert(
                    value, 
                    com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);
                configValues[0] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value, 
                    value);

                value = orb.create_any();
                com.thalesgroup.CdmwDeployment.ServantLifetimeValueHelper.insert(
                    value, 
                    com.thalesgroup.CdmwDeployment.ServantLifetimeValue.CONTAINER_LIFETIME);
                configValues[1] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.SERVANT_LIFETIME.value, 
                    value);

                value = orb.create_any();
                com.thalesgroup.CdmwDeployment.ThreadingPolicyValueHelper.insert(
                    value, 
                    com.thalesgroup.CdmwDeployment.ThreadingPolicyValue.MULTITHREAD);
                configValues[2] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.THREADING_POLICY.value, 
                    value);

                value = orb.create_any();
                value.insert_string(CLIENT_HOME_REP_ID);
                configValues[3] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.HOME_REPOSITORY_ID.value, 
                    value);

                value = orb.create_any();
                value.insert_string(CLIENT_HOME_SERVANT_NAME);
                configValues[4] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.HOME_SERVANT_CLASSNAME.value, 
                    value);

                com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription[] seq
                    = new com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription[1];
                // Event valuetype
                com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription desc
                    = new com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription();
                desc.repid = EVENT_REP_ID;
                desc.factory_entrypoint = EVENT_ENTRYPOINT;
                seq[0] = desc;
                
                value = orb.create_any();
                com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescriptionSeqHelper.insert(
                    value, 
                    seq);
                configValues[5] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.VALUETYPE_FACTORY_DEPENDENCIES.value, 
                    value);

                clientHome = this.clientContainer.install_home(
                    CLIENT_HOME_UID,
                    CLIENT_HOME_ENTRY_POINT,
                    configValues);
                testOk = true;
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
            } catch (org.omg.Components.Deployment.InstallationFailure f) {
                println("Unexpected exception raised: " + 
                    "Components::Deployment::InstallationFailure " + f.reason );
            }
            check(testOk);
    
            
            println("Creating a Client component using create_component() operation...");
            testOk = false;
            try {
                org.omg.Components.KeylessCCMHome keylessClientHome 
                    = org.omg.Components.KeylessCCMHomeHelper.narrow(clientHome);
    
                if (keylessClientHome == null) {
                    println("ERROR: Created home is not a keyless home!");
                } else {
                    ccmClientComp = keylessClientHome.create_component();
    
                    basicClientComp
                        = com.acme.BasicDemo.ClientHelper.narrow(ccmClientComp);
    
                    if (basicClientComp == null) {
                        println("ERROR: Created component is not a BasicDemo.Client!");
                    } else {
                        testOk = true;
                    }
                }
            } catch (org.omg.Components.CreateFailure cf) {
                println("Unexpected exception raised: CreateFailure");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);
            
            println("Create a Server component using create_named_component() operation");
            testOk = false;
            try {
                basicServerHome = 
                    com.acme.BasicDemo.ServerHomeHelper.narrow(serverHome);
    
                if (basicServerHome == null) {
                    println("Home is not a BasicDemo::ServerHome!");
                } else {
                    com.acme.BasicDemo.NamedComponent namedComp
                        = basicServerHome.create_named_component(SERVER_NAME);
    
                    basicServerComp = 
                        com.acme.BasicDemo.ServerHelper.narrow(namedComp);
    
                    if (basicServerComp == null) {
                        println("ERROR: basicServerComp is nil!");
                    } else {
                        testOk = true;
                    }
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
    
            println("Testing generated operation on Server component...");
            println("provide_for_clients operation on Server component...");
            testOk = false;
            try {
                displayFacetObj = basicServerComp.provide_for_clients();
    
                if (displayFacetObj == null) {
                    println("ERROR: No facet found for Server component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Connect operation on Client component...");
            testOk = false;
            try {
                basicClientComp.connect_to_server(displayFacetObj);
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Expected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Connect operation again on Client component...");
            testOk = false;
            try {
                basicClientComp.connect_to_server(displayFacetObj);
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Expected exception raised: AlreadyConnected");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Disconnect operation on Client component...");
            testOk = false;
            try {
                basicClientComp.disconnect_to_server();
                testOk = true;
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
    
            println("Disconnect operation again on Client component...");
            testOk = false;
            try {
                basicClientComp.disconnect_to_server();
            } catch (org.omg.Components.NoConnection nc) {
                println("Expected exception raised: NoConnection");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Connect operation again on Client component...");
            testOk = false;
            try {
                basicClientComp.connect_to_server(displayFacetObj);
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Init attribute operation on Client component...");
            testOk = false;
            try {
                com.acme.BasicDemo.Event event = new EventImpl();
                event.text = "Hello World!";
                basicClientComp.event(event);
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);
    
            println("Configuration_complete operation on Client component...");
            testOk = false;
            try {
                basicClientComp.configuration_complete();
            } catch (org.omg.Components.InvalidConfiguration ic) {
                println("Expected exception raised: InvalidConfiguration");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);
    
            println("Connect operation on Client component...");
            testOk = false;
            try {
                basicClientComp.connect_to_register(basicServerComp);
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_connections operation on Client component...");
            testOk = false;
            try {
                com.acme.CommonDemo.Registration registrationConx
                    = basicClientComp.get_connection_to_register();
    
                if (!registrationConx._is_equivalent(basicServerComp)) {
                    println("ERROR: Unexpected connection found for registration for Client component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_all_ports operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.ComponentPortDescription portDesc
                    = basicClientComp.get_all_ports();
    
                org.omg.Components.FacetDescription[] facetDesc
                    = portDesc.facets;
                org.omg.Components.ReceptacleDescription[] recepDesc
                    = portDesc.receptacles;
                org.omg.Components.ConsumerDescription[] consumerDesc
                    = portDesc.consumers;
                org.omg.Components.EmitterDescription[] emitterDesc
                    = portDesc.emitters;
                org.omg.Components.PublisherDescription[] publisherDesc
                    = portDesc.publishers;
    
                if (facetDesc.length != 0) {
                    println("ERROR: unexpected number of facet for Client component");
                } else if (recepDesc.length != 2) {
                    println("ERROR: unexpected number of receptacle for Client component");
                } else if (consumerDesc.length != 0) {
                    println("ERROR: unexpected number of consumer for Client component");
                } else if (publisherDesc.length != 0) {
                    println("ERROR: unexpected number of publisher for Client component");
                } else if (emitterDesc.length != 0) {
                    println("ERROR: unexpected number of emitter for Client component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
    
            println("Configuration_complete operation on Server component...");
            testOk = false;
            try {
                basicServerComp.configuration_complete();
                testOk = true;
            } catch (org.omg.Components.InvalidConfiguration ic) {
                println("Unexpected exception raised: InvalidConfiguration");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);
    
            println("Configuration_complete operation on Client component...");
            testOk = false;
            try {
                basicClientComp.configuration_complete();
                testOk = true;
            } catch (org.omg.Components.InvalidConfiguration ic) {
                println("Unexpected exception raised: InvalidConfiguration");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);
    
            println("Get name of Server component...");
            testOk = false;
            try {
                String serverName = basicServerComp.name();
                println("Server name is " + serverName);
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);
    
            println("Get name of Client component...");
            testOk = false;
            try {
                String clientName = basicClientComp.name();
                println("Server name is " + clientName);
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);
    
            println("Disconnect operation on Client component...");
            testOk = false;
            try {
                basicClientComp.disconnect_to_server();
                testOk = true;
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Disconnect operation on Client component...");
            testOk = false;
            try {
                basicClientComp.disconnect_to_register();
                testOk = true;
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Remove operation on the Server home...");
            testOk = false;
            try {
                this.serverContainer.remove_home(basicServerHome);
                testOk = true;
            } catch (org.omg.Components.RemoveFailure rf) {
                println("Unexpected exception raised: NoKeyAvailable");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
            
            println("Remove operation on the Client home...");
            testOk = false;
            try {
                this.clientContainer.remove_home(clientHome);
                testOk = true;
            } catch (org.omg.Components.RemoveFailure rf) {
                println("Unexpected exception raised: NoKeyAvailable");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
        } catch (Exception e) {
            System.out.println("Unexpected exception raised: " + e.toString());
        }
        
    }

}

