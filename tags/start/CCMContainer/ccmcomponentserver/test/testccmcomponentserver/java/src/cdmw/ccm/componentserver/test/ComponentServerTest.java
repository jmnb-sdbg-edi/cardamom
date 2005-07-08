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


package cdmw.ccm.componentserver.test;

import cdmw.testutils.Testable;

import cdmw.ccm.deployment.ConfigValueImpl;
import cdmw.ccm.componentserver.ComponentServerImpl;

import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ComponentKindValueHelper;

public class ComponentServerTest extends Testable {

    private org.omg.CORBA.ORB orb;

    private org.omg.PortableServer.POA poa;

    public ComponentServerTest(
        org.omg.CORBA.ORB orb, org.omg.PortableServer.POA poa) {
        this.name = "ComponentServerTest";
        this.out = System.out;
        this.orb = orb;
        this.poa = poa;
    }

    private org.omg.Components.ConfigValue[]
        createConfigValues(ComponentKindValue componentKind) {

        int nbValues = 1;
        org.omg.Components.ConfigValue[] cfg =
            new org.omg.Components.ConfigValue[nbValues];
        int idx = 0;

        // COMPONENT_KIND
        org.omg.CORBA.Any value = orb.create_any();
        ComponentKindValueHelper.insert(value, componentKind);
        cfg[idx++] = new ConfigValueImpl(
            com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value, value);

        return cfg;

    }

    public void doTests() {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(21);

        println("Testing Component Server");
        org.omg.Components.Deployment.ComponentServer theComponentServer = null;

        // 1) Create a ComponentServer
        // Create a set of ConfigValues.
        org.omg.Components.ConfigValue[] cfgCompserver =
            new org.omg.Components.ConfigValue[0];
        com.thalesgroup.CdmwEvent.EventChannelFactory noECF = null;
        String processName = "DummyProcessName";
        String applicationName = "DummyApplicationName";
        org.omg.Components.HomeRegistration homeRegistration = null;
        ComponentServerImpl cs = new ComponentServerImpl(
            this.orb,
            this.poa,
            homeRegistration,
            noECF,
            processName,
            applicationName,
            cfgCompserver);
        org.omg.PortableServer.Servant serv = cs;
        theComponentServer = cs._this(this.orb);

        // Testing theComponentServer.get_containers()
        println("Getting all containers...");
        org.omg.Components.Deployment.Container[] containers =
            theComponentServer.get_containers();
        check(containers.length == 0);

        // 2) Creating a first container
        org.omg.Components.Deployment.Container container1 = null;
        try {
            println("Creating a first session container ...");
            org.omg.Components.ConfigValue[] cfg0 =
                createConfigValues(ComponentKindValue.SESSION);
            container1 = theComponentServer.create_container(cfg0);
            check(container1 != null);
            println("Checking used config values...");
            println("Checking number of config values...");
            org.omg.Components.ConfigValue[] cfg1 = container1.configuration();
            check(cfg1.length == 1);
            println("Checking content of config values...");
            String name = cfg1[0].name;
            check(name.equals(com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value));
            org.omg.CORBA.Any value = cfg1[0].value;
            ComponentKindValue componentKind =
                ComponentKindValueHelper.extract(value);
            check(componentKind.value() == ComponentKindValue.SESSION.value());
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString());
            fail();
        }

        org.omg.Components.Deployment.Container container2 = null;
        try {
            println("Creating a second session container ...");
            org.omg.Components.ConfigValue[] cfg0 =
                createConfigValues(ComponentKindValue.SERVICE);
            container2 = theComponentServer.create_container(cfg0);
            check(container2 != null);
            println("Checking used config values...");
            println("Checking number of config values...");
            org.omg.Components.ConfigValue[] cfg1 = container2.configuration();
            check(cfg1.length == 1);
            println("Checking content of config values...");
            String name = cfg1[0].name;
            check(name.equals(com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value));
            org.omg.CORBA.Any value = cfg1[0].value;
            ComponentKindValue componentKind =
                ComponentKindValueHelper.extract(value);
            check(componentKind.value() == ComponentKindValue.SERVICE.value());
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString());
            fail();
        }

        // Creating an Entity container (NOT supported yet).
        try {
            println("Creating an Entity container (NOT supported yet) ...");
            org.omg.Components.ConfigValue[] cfg0 =
                createConfigValues(ComponentKindValue.PROCESS);
            org.omg.Components.Deployment.Container container =
                theComponentServer.create_container(cfg0);
            fail();
        } catch (Exception ex) {
            System.out.println("ex = " + ex.toString());
            succeed();
        }

        // Testing theComponentServer.get_containers()
        println("Getting all containers...");
        containers = theComponentServer.get_containers();
        check(containers.length == 2);

        // 3) Removing first container
        println("Removing first container...");
        try {
            theComponentServer.remove_container(container1);
            succeed();
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString());
            fail();
        }
        // Testing theComponentServer.get_containers()
        println("Getting all containers...");
        containers = theComponentServer.get_containers();
        check(containers.length == 1);

        println("Removing first container again...");
        try {
            theComponentServer.remove_container(container1);
            fail();
        } catch (Exception ex) {
            System.out.println("ex = " + ex.toString());
            succeed();
        }

        // 4) Testing supported interfaces...
        println("Narrowing to FactoryBase object...");
        com.thalesgroup.CdmwLifeCycle.FactoryBase fact
            = com.thalesgroup.CdmwLifeCycle.FactoryBaseHelper.narrow(
                theComponentServer);
        check(fact != null);
        println("Testing supported interfaces...");
        try {
            String[] seq = fact.get_supported_objects();
            check(seq.length == 3);
            System.out.println("Supported objects:");
            for (int i=0; i < seq.length; ++i) {
                System.out.println("\t" + seq[i]);
            }
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString());
            fail();
        }
        println("Testing support of IDL:thalesgroup.com/CdmwCcmContainer/Container:1.0 ...");
        try {
            String repId = "IDL:thalesgroup.com/CdmwCcmContainer/Container:1.0";
            boolean res = fact.supports(repId);
            check(res == true);
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString());
            fail();
        }
        println("Testing support of IDL:omg.org/Components/Deployment/Container:1.0 ...");
        try {
            String repId = "IDL:omg.org/Components/Deployment/Container:1.0";
            boolean res = fact.supports(repId);
            check(res == true);
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString());
            fail();
        }
        println("Testing support of IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase:1.0 ...");
        try {
            String repId = "IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase:1.0";
            boolean res = fact.supports(repId);
            check(res == true);
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString());
            fail();
        }
        println("Testing support of IDL:dummy:1.0 ...");
        try {
            String repId = "IDL:dummy:1.0";
            boolean res = fact.supports(repId);
            check(res == false);
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString());
            fail();
        }

        // 5) Removing the component server
        println("Removing the component server...");
        try {
            theComponentServer.remove();
            succeed();
        } catch (Exception ex) {
            System.err.println("ex = " + ex.toString());
            fail();
        }

    }

}

