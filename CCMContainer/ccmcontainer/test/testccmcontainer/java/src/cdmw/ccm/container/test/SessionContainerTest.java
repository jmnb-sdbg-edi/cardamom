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

import cdmw.testutils.Testable;

import cdmw.ccm.deployment.ConfigValueImpl;

public class SessionContainerTest extends Testable {

    private org.omg.CORBA.ORB orb;

    private org.omg.PortableServer.POA poa;

    public SessionContainerTest(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA poa) {
        this.name = "Session Container Test";
        this.out = System.out;
        this.orb = orb;
        this.poa = poa;
    }

    public void doTests() {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(14);
        int timescale = cdmw.testutils.Testable.getTimescale();
        
        println("Testing Session Container");

        cdmw.ossupport.ThreadedProcess process = null;
        try {
            process = cdmw.ossupport.OS.createJavaProcess("cdmw.ccm.container.test.MainTest --server");
            try {
                cdmw.ossupport.OS.sleep(timescale*2000);
            } catch (java.lang.InterruptedException e) {
                // ignore
            }
        } catch (java.io.IOException e) {
            println("Error while starting MainTest process: " + e.getMessage());
            e.printStackTrace();
            return;
        }
        
        org.omg.Components.Deployment.ComponentServer componentServer = null;
        println("get the reference of the component_server");
        try {
            org.omg.CORBA.Object obj = orb.string_to_object("file://componentServer.ior");
            componentServer = org.omg.Components.Deployment.ComponentServerHelper.narrow(obj);
            
        } catch (org.omg.CORBA.BAD_PARAM e) {
            println("Error while retrieving component_server reference: " + e.getMessage());
            e.printStackTrace();
            return;
        }
            


        // Create a container instance
        // Create a set of ConfigValues.
        org.omg.Components.ConfigValue[] cfgCompserver =
            new org.omg.Components.ConfigValue[0];
        org.omg.Components.Deployment.Container theContainer = null;
        try {
            theContainer = componentServer.create_container(cfgCompserver);
        } catch(org.omg.Components.CreateFailure cf) {
        } catch(org.omg.Components.Deployment.InvalidConfiguration ic) {}

        // Testing theContainer.configuration() ...

        // Testing theContainer.get_component_server() ...

        // Testing theContainer.install_home() ...
        println("Testing SESSION Component with COMPONENT_LIFETIME policy");
        String ENTRY_PT   = 
            "cdmw.ccm.container.test.CCMHelloHomeImpl.createCCMHelloHomeImpl";
        String UUID_home1 = "SessionHome1";

        com.thalesgroup.HelloModule.HelloHome hellohome1 = null;
        try {
            // Create a set of ConfigValues.
            org.omg.Components.ConfigValue[] cfg = createConfigValues(
                com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION,
                com.thalesgroup.CdmwDeployment.ServantLifetimeValue.COMPONENT_LIFETIME,
                com.thalesgroup.CdmwDeployment.ThreadingPolicyValue.MULTITHREAD,
                false);
            println("Creating " + UUID_home1 + " home...");
            org.omg.Components.CCMHome sessionHome = theContainer.install_home(
                UUID_home1,
                ENTRY_PT,
                cfg);
            hellohome1 =
                com.thalesgroup.HelloModule.HelloHomeHelper.narrow(sessionHome);
            check(hellohome1 != null);
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString());
            fail();
        }

        // Testing the_container.get_homes()
        println("Getting all container's homes...");
        org.omg.Components.CCMHome[] homes =  theContainer.get_homes();
        check(homes.length == 1);

        // Testing component creation ...

        // Testing theContainer.remove_home() ..
        try {
            println("Removing " + UUID_home1 + " home...");
            theContainer.remove_home(hellohome1);
            homes = theContainer.get_homes();
            check(homes.length == 0);
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }

        // Testing  component creation
        try {
            // Create a set of ConfigValues.
            org.omg.Components.ConfigValue[] cfg = createConfigValues(
                com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION,
                com.thalesgroup.CdmwDeployment.ServantLifetimeValue.COMPONENT_LIFETIME,
                com.thalesgroup.CdmwDeployment.ThreadingPolicyValue.MULTITHREAD,
                false);
            println("Creating " + UUID_home1 + " home again ...");
            org.omg.Components.CCMHome sessionHome = theContainer.install_home(
                UUID_home1,
                ENTRY_PT,
                cfg);
            hellohome1 =
                com.thalesgroup.HelloModule.HelloHomeHelper.narrow(sessionHome);
            check(hellohome1 != null);
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }

        // Creating a component
        com.thalesgroup.HelloModule.HelloWorld comp1 = null;
        try {
            println("Creating 1st component using create()...");

            org.omg.Components.CCMObject cmp = hellohome1.create();
            comp1 = com.thalesgroup.HelloModule.HelloWorldHelper.narrow(cmp);
            check(comp1 != null);
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }
        com.thalesgroup.HelloModule.HelloWorld comp2 = null;
        try {
            println("Creating 2nd component using create_component()...");

            org.omg.Components.CCMObject cmp = hellohome1.create_component();
            comp2 = com.thalesgroup.HelloModule.HelloWorldHelper.narrow(cmp);
            check(comp2 != null);
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }

        // Calling say_hello on the 1st and 2nd components
        try {
            println("Calling say_hello() on the 1st component...");
            com.thalesgroup.HelloModule.Message msg = new MessageImpl("Hello!");
            comp1.say_hello(msg);
            succeed();
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }
        try {
            println("Calling say_hello() on the 2nd component...");
            com.thalesgroup.HelloModule.Message msg =
                new MessageImpl("Hello again!");
            comp2.say_hello(msg);
            succeed();
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }

        // Testing valuetype factories
        try {
            println("Calling say_hello() on the 2nd component with no available valuetype factory...");
            com.thalesgroup.HelloModule.TraceMessage msg =
                new TraceMessageImpl("Hello ...", 5);
            comp2.log_message(msg);
            fail();
        } catch (Exception ex) {
            System.out.println("ex = " + ex.toString() );
            succeed();
        }

        String UUID_home2 = "SessionHome2";
        try {
            // Create a set of ConfigValues.
            org.omg.Components.ConfigValue[] cfg = createConfigValues(
                com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION,
                com.thalesgroup.CdmwDeployment.ServantLifetimeValue.COMPONENT_LIFETIME,
                com.thalesgroup.CdmwDeployment.ThreadingPolicyValue.MULTITHREAD,
                true);
            println("Creating " + UUID_home2 +
                " home to install a valuetype factory ...");
            org.omg.Components.CCMHome sessionHome =
                theContainer.install_home(UUID_home2, ENTRY_PT, cfg);
            com.thalesgroup.HelloModule.HelloHome hellohome =
                com.thalesgroup.HelloModule.HelloHomeHelper.narrow(sessionHome);
            check(hellohome != null);
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }

        try {
            println("Calling say_hello() on the 2nd component WITH a valuetype factory installed...");
            com.thalesgroup.HelloModule.TraceMessage msg =
                new TraceMessageImpl("Hello ...", 5);
            comp2.log_message(msg);
            succeed();
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }

        // Deleting 1st component
        try {
            println("Asking 1st component to remove itself...");
            comp1.remove();
            succeed();
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }
        // Deleting 2nd component
        try {
            println("Asking home to remove 2nd component...");
            hellohome1.remove_component(comp2);
            succeed();
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }

        // Tesing valuetype support

        // Testing theContainer.remove() ...
        try {
            println("Removing the container...");
            theContainer.remove();
            succeed();
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString() );
            fail();
        }

        println( "Killing process component_server" );
        cdmw.ossupport.OS.killProcess(process);
        try {
            cdmw.ossupport.OS.sleep(timescale*2000);
        } catch (java.lang.InterruptedException e) {
            // ignore
        }


    }

    private org.omg.Components.ConfigValue[] createConfigValues(
        com.thalesgroup.CdmwDeployment.ComponentKindValue   componentKind,
        com.thalesgroup.CdmwDeployment.ServantLifetimeValue servantLifetime,
        com.thalesgroup.CdmwDeployment.ThreadingPolicyValue threadingPolicy,
        boolean withTraceMessage) {

        int nbValues = 6;
        org.omg.Components.ConfigValue[] cfg =
            new org.omg.Components.ConfigValue[nbValues];
        int idx = 0;
        org.omg.CORBA.Any value;

        // COMPONENT_KIND
        value = orb.create_any();
        com.thalesgroup.CdmwDeployment.ComponentKindValueHelper.insert(
            value,
            componentKind);
        cfg[idx++] = new ConfigValueImpl(
            com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value, value);

        // HOME_REPOSITORY_ID
        String repid= "IDL:thalesgroup.com/Cdmw_HelloModule/HelloHome:1.0";
        value = orb.create_any();
        value.insert_string(repid);
        cfg[idx++] = new ConfigValueImpl(
            com.thalesgroup.CdmwDeployment.HOME_REPOSITORY_ID.value, value);

        // HOME_SERVANT_CLASSNAME
        String classname = "cdmw.ccm.container.test.CdmwHelloHomeImpl";
        value = orb.create_any();
        value.insert_string(classname);
        cfg[idx++] = new ConfigValueImpl(
            com.thalesgroup.CdmwDeployment.HOME_SERVANT_CLASSNAME.value, value);

        // SERVANT_LIFETIME
        value = orb.create_any();
        com.thalesgroup.CdmwDeployment.ServantLifetimeValueHelper.insert(
            value,
            servantLifetime);
        cfg[idx++] = new ConfigValueImpl(
            com.thalesgroup.CdmwDeployment.SERVANT_LIFETIME.value, value);

        // THREADING_POLICY
        value = orb.create_any();
        com.thalesgroup.CdmwDeployment.ThreadingPolicyValueHelper.insert(
            value,
            threadingPolicy);
        cfg[idx++] = new ConfigValueImpl(
            com.thalesgroup.CdmwDeployment.THREADING_POLICY.value, value);

        // VALUETYPE_FACTORY_DEPENDENCIES </TD><TD>sequence< ValuetypeFactoryDescription ></TD>
        String messageRepid =
            "IDL:thalesgroup.com/HelloModule/Message:1.0";
        String messageEntrypoint = 
            "cdmw.ccm.container.test.MessageFactoryImpl.createMessageFactory";
        String traceMessageRepid =
            "IDL:thalesgroup.com/HelloModule/TraceMessage:1.0";
        String traceMessageEntrypoint = 
            "cdmw.ccm.container.test.TraceMessageFactoryImpl.createTraceMessageFactory";

        int lg = 1;
        if (withTraceMessage) lg = 2;
        com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription[] seq =
            new com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription[lg];
        // Message valuetype
        com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription desc =
            new com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription(
                messageRepid,
                messageEntrypoint);
        seq[0] = desc;
        if (withTraceMessage) {
            // TraceMessage valuetype
            desc =
                new com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription(
                    traceMessageRepid,
                    traceMessageEntrypoint);
            seq[1] = desc;
        }

        value = orb.create_any();
        com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescriptionSeqHelper.insert(value, seq);
        cfg[idx++] = new ConfigValueImpl(
            com.thalesgroup.CdmwDeployment.VALUETYPE_FACTORY_DEPENDENCIES.value,
            value);

        return cfg;

    }

}

