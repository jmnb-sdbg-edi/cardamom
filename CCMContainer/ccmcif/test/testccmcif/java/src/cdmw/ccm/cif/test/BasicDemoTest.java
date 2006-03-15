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
import cdmw.ossupport.OS;

public class BasicDemoTest extends Testable {

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

    public BasicDemoTest(org.omg.CORBA.ORB orb, String containerIORClient, String containerIORServer) {
        this.name = "cdmw.ccm.cif.BasicDemoTest";
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
        setNbOfRequestedTestOK(52);
        
        boolean testOk = false;
        org.omg.Components.CCMHome serverHome = null;
        com.acme.BasicDemo.ServerHome basicServerHome = null;
        org.omg.Components.CCMObject ccmServerComp = null;
        com.acme.BasicDemo.Server basicServerComp = null;
        com.acme.BasicDemo.Server basicServerComp2 = null;
        com.acme.BasicDemo.Server basicServerComp3 = null;
    
        org.omg.Components.CCMHome clientHome = null;
        com.acme.BasicDemo.ClientHome basicClientHome = null;
        org.omg.Components.CCMObject ccmClientComp = null;
        com.acme.BasicDemo.Client basicClientComp = null;
    
        org.omg.CORBA.Object displayFacetObj = null;
    
    
        // This tests CIF classes
        //-------------------------------------------------
        println("CIF classes Tests");
        
        try {
            println("Installing a ServerHome...");
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
                value.insert_string(SERVER_HOME_REP_ID);
                configValues[3] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.HOME_REPOSITORY_ID.value, 
                    value);

                value = orb.create_any();
                value.insert_string(SERVER_HOME_SERVANT_NAME);
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
    
            println("Creating a Server component using create_component() operation...");
            testOk = false;
            try {
                org.omg.Components.KeylessCCMHome keylessServerHome 
                    = org.omg.Components.KeylessCCMHomeHelper.narrow(serverHome);
    
                if (keylessServerHome == null) {
                    println("ERROR: Created home is not a keyless home!");
                } else {
                    ccmServerComp = keylessServerHome.create_component();

                    basicServerComp = 
                        com.acme.BasicDemo.ServerHelper.narrow(ccmServerComp);
    
                    if (basicServerComp == null) {
                        println("ERROR: Created component is not a BasicDemo.Server!");
                    } else {
                        testOk = true;
                    }
                }
            } catch (org.omg.Components.CreateFailure e) {
                println("Unexpected exception raised: CreateFailure");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
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
            
            println("Creating other Server component using create() operation");
            testOk = false;
            try {
                basicServerHome 
                    = com.acme.BasicDemo.ServerHomeHelper.narrow(serverHome);
    
                if (basicServerHome == null) {
                    println("Home is not a BasicDemo.ServerHome!");
                } else {
                    basicServerComp2 = basicServerHome.create();
                    basicServerComp3 = basicServerHome.create();
    
                    if (basicServerComp2 == null) {
                        println("ERROR: basicServerComp2 is nil!");
                    } else {
                        if (basicServerComp3 == null) {
                            println("ERROR: basicServerComp3 is nil!");
                        } else {
                            testOk = true;
                        }
                    }
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Checking Server component is same using same_component() operation...");
            testOk = false;
            try {
                boolean result1 = 
                    basicServerComp2.same_component(basicServerComp3);
                boolean result2
                    = basicServerComp3.same_component(basicServerComp3);
                
                if (result1 == true) {
                    println("ERROR: basicServerComp2 is not the same component!");
                } else {
                    if (result2 == false) {
                        println("ERROR: basicServerComp3 is the same component!");
                    } else {
                        testOk = true;
                    }
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Removing a Server component using remove_component() operation...");
            testOk = false;
            try {
                serverHome.remove_component(basicServerComp2);
                testOk = true;
            } catch (org.omg.Components.RemoveFailure rf) {
                println("Unexpected exception raised: RemoveFailure");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Removing again the same Server component using remove_component() operation...");
            testOk = false;
            try {
                serverHome.remove_component(basicServerComp2);
            } catch (org.omg.Components.RemoveFailure rf) {
                println("Unexpected exception raised: RemoveFailure");
                testOk = false;
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println("Expected exception raised: BAD_PARAM");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Testing Navigation operation on Server component...");
            println("Get_all_facets operation on Server component...");
            testOk = false;
            try {
                org.omg.Components.FacetDescription[] facetDesc
                    = basicServerComp3.get_all_facets();
    
                if (facetDesc.length != 1) {
                    println("ERROR: Unexpected number of facet found for Server component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_all_facets operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.FacetDescription[] facetDesc
                    = basicClientComp.get_all_facets();
    
                if (facetDesc.length != 0) {
                    println("ERROR: Unexpected facet found for Client component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_named_facets operation on Server component...");
            testOk = false;
            try {
                String[] facetNameList = new String[1];
                facetNameList[0] = DISPLAY_FACET_NAME;
                org.omg.Components.FacetDescription[] facetDesc
                    = basicServerComp.get_named_facets(facetNameList);
    
                if (facetDesc.length != 1) {
                    println("ERROR: Unexpected number of facet found for Server component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_named_facets operation on Server component with invalid name...");
            testOk = false;
            try {
                String[] facetNameList = new String[2];
                facetNameList[0] = DISPLAY_FACET_NAME;
                facetNameList[1] = INVALID_FACET_NAME;
    
                org.omg.Components.FacetDescription[] facetDesc
                    = basicServerComp.get_named_facets(facetNameList);
            } catch (org.omg.Components.InvalidName in) {
                println("Expected exception raised: InvalidName");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("provide_facet operation on Server component...");
            testOk = false;
            try {
                displayFacetObj
                    = basicServerComp.provide_facet(DISPLAY_FACET_NAME);
    
                if (displayFacetObj == null) {
                    println("ERROR: No facet found for Server component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("provide_facet operation on Server component with invalid name...");
            testOk = false;
            try {
                org.omg.CORBA.Object facetObj
                    = basicServerComp.provide_facet(INVALID_FACET_NAME);
            } catch (org.omg.Components.InvalidName in) {
                println("Expected exception raised: InvalidName");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);        
    
            println("Testing Receptacle operation on Server component...");
            println("Get_all_receptacles operation on Server component...");
            testOk = false;
            try {
                org.omg.Components.ReceptacleDescription[] recepDesc
                    = basicServerComp3.get_all_receptacles();
    
                if (recepDesc.length != 0) {
                    println("ERROR: Unexpected receptacle found for Server component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_all_receptacles operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.ReceptacleDescription[] recepDesc
                    = basicClientComp.get_all_receptacles();
    
                if (recepDesc.length != 2) {
                    println("ERROR: Unexpected number of receptacle found for Client component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_named_receptacles operation on Client component...");
            testOk = false;
            try {
                String[] recepNameList = new String[2];
                recepNameList[0] = DISPLAY_RECEP_NAME;
                recepNameList[1] = REGISTER_RECEP_NAME;
                org.omg.Components.ReceptacleDescription[] recepDesc
                    = basicClientComp.get_named_receptacles(recepNameList);
    
                if (recepDesc.length != 2) {
                    println("ERROR: Unexpected number of receptacle found for Client component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_named_receptacles operation on Client component with invalid name...");
            testOk = false;
            try {
                String[] recepNameList = new String[2];
                recepNameList[0] = DISPLAY_RECEP_NAME;
                recepNameList[1] = INVALID_RECEP_NAME;
    
                org.omg.Components.ReceptacleDescription[] recepDesc
                    = basicClientComp.get_named_receptacles(recepNameList);
            } catch (org.omg.Components.InvalidName in) {
                println("Expected exception raised: InvalidName");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_connections operation on Client component...");
            testOk=false;
            try {
                org.omg.Components.ConnectionDescription[] conxDesc
                    = basicClientComp.get_connections(DISPLAY_RECEP_NAME);
    
                if (conxDesc.length != 0) {
                    println("ERROR: Unexpected connection found for Client component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_connections operation on Client component with invalid name...");
            testOk = false;
            try {
                org.omg.Components.ConnectionDescription[] conxDesc
                    = basicClientComp.get_connections(INVALID_RECEP_NAME);
            } catch (org.omg.Components.InvalidName in) {
                println("Expected exception raised: InvalidName");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Connect operation on Client component with invalid name...");
            testOk=false;
            try {
                org.omg.Components.Cookie ck
                    = basicClientComp.connect(INVALID_RECEP_NAME, displayFacetObj);
            } catch (org.omg.Components.InvalidName in) {
                println("Expected exception raised: InvalidName");
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Connect operation on Client component with invalid connection...");
            testOk = false;
            try {
                org.omg.Components.Cookie ck
                    = basicClientComp.connect(REGISTER_RECEP_NAME, displayFacetObj);
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Expected exception raised: InvalidConnection");
                testOk = true;
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Connect operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.Cookie ck = basicClientComp.connect(
                    DISPLAY_RECEP_NAME,
                    displayFacetObj);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Expected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Connect operation again on Client component...");
            testOk = false;
            try {
                org.omg.Components.Cookie ck = basicClientComp.connect(
                    DISPLAY_RECEP_NAME,
                    displayFacetObj);
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Expected exception raised: AlreadyConnected");
                testOk = true;
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Disconnect operation on Client component...");
            testOk = false;
            try {
                basicClientComp.disconnect(DISPLAY_RECEP_NAME, null);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.CookieRequired cr) {
                println("Unexpected exception raised: CookieRequired");
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Disconnect operation on Client component with an invalid name...");
            testOk = false;
            try {
                basicClientComp.disconnect(INVALID_RECEP_NAME, null);
            } catch (org.omg.Components.InvalidName in) {
                println("Expected exception raised: InvalidName");
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.CookieRequired cr) {
                println("Unexpected exception raised: CookieRequired");
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
                 basicClientComp.disconnect(DISPLAY_RECEP_NAME, null);
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.CookieRequired cr) {
                println("Unexpected exception raised: CookieRequired");
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
                org.omg.Components.Cookie ck = basicClientComp.connect(
                    DISPLAY_RECEP_NAME,
                    displayFacetObj);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
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
                testOk = false;
            }
            check(testOk);
    
            println("Connect operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.Cookie ck = basicClientComp.connect(
                    REGISTER_RECEP_NAME,
                    basicServerComp);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_connections operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.ConnectionDescription[] conxDesc
                    = basicClientComp.get_connections(REGISTER_RECEP_NAME);
    
                if (conxDesc.length != 1) {
                    println("ERROR: Unexpected number of connection found for Client component => " + conxDesc.length);
                } else {
                    testOk = true;
                }
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_all_publishers operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.PublisherDescription[] publisherDesc
                    = basicClientComp.get_all_publishers();
    
                if (publisherDesc.length != 0) {
                    println("ERROR: Unexpected number of publisher found for Client component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_all_emitters operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.EmitterDescription[] emitterDesc
                    = basicClientComp.get_all_emitters();
    
                if (emitterDesc.length != 0) {
                    println("ERROR: Unexpected number of emitter found for Client component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_all_consumers operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.ConsumerDescription[] consumerDesc
                    = basicClientComp.get_all_consumers();
    
                if (consumerDesc.length != 0) {
                    println("ERROR: Unexpected number of consumers found for Client component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_ccm_home operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.CCMHome cHome
                    = basicClientComp.get_ccm_home();
    
                if (!cHome._is_equivalent(clientHome)) {
                    println("ERROR: Unexpected home for Client component");
                } else {
                    testOk = true;
                }
            }
            catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Get_primary_key operation on Client component...");
            testOk = false;
            try {
                org.omg.Components.PrimaryKeyBase key
                    = basicClientComp.get_primary_key();
            } catch (org.omg.Components.NoKeyAvailable nka) {
                println("Expected exception raised: NoKeyAvailable");
                testOk = true;
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
    
            println("Remove operation on a Server component...");
            testOk = false;
            try {
                basicServerComp3.remove();
                testOk = true;
            } catch (org.omg.Components.RemoveFailure rf) {
                println("Unexpected exception raised: NoKeyAvailable");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
   	    int timescale = Testable.getTimescale();
	    OS.sleep(timescale * 2000);
            println("Remove operation again on the same Server component...");
            testOk = false;
            try {
                basicServerComp3.remove();
            } catch (org.omg.Components.RemoveFailure rf) {
                println("Unexpected exception raised: RemoveFailure");
                testOk = false;
            } catch (org.omg.CORBA.OBJECT_NOT_EXIST one) {
                println("Expected exception raised: OBJECT_NOT_EXIST");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Configuration_complete operation on a removed Server component...");
            testOk = false;
            try {
                basicServerComp3.configuration_complete();
            } catch (org.omg.Components.InvalidConfiguration ic) {
                println("Unexpected exception raised: InvalidConfiguration");
            } catch (org.omg.CORBA.OBJECT_NOT_EXIST one) {
                println("Expected exception raised: OBJECT_NOT_EXIST");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Configuration_complete operation on an other removed Server component...");
            testOk = false;
            try {
                basicServerComp2.configuration_complete();
            } catch (org.omg.Components.InvalidConfiguration ic) {
                println("Unexpected exception raised: InvalidConfiguration");
            } catch (org.omg.CORBA.OBJECT_NOT_EXIST one) {
                println("Expected exception raised: OBJECT_NOT_EXIST");
                testOk=true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);
    
            println("Configuration_complete operation on Server component...");
            testOk = false;
            try {
                basicServerComp.configuration_complete();
            } catch (org.omg.Components.InvalidConfiguration ic) {
                println("Expected exception raised: InvalidConfiguration");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);
    
            println("Init attribute operation on Server component...");
            testOk = false;
            try {
                basicServerComp.name(SERVER_NAME);
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
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
                testOk = false;
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
                basicClientComp.disconnect(DISPLAY_RECEP_NAME, null);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.CookieRequired cr) {
                println("Unexpected exception raised: CookieRequired");
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
                basicClientComp.disconnect(REGISTER_RECEP_NAME, null);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.CookieRequired cr) {
                println("Unexpected exception raised: CookieRequired");
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
        } catch (Exception e)  {
            println("Unexpected exception raised: " + e.toString());
        }
        
    }

}

