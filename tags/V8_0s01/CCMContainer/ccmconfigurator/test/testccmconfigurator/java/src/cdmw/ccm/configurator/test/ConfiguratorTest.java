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


package cdmw.ccm.configurator.test;

import cdmw.testutils.Testable;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.xmlutils.XMLHelper;

import cdmw.ccm.deployment.CpfReader;
import cdmw.ccm.deployment.ConfigValues;
import cdmw.ccm.deployment.ConfigValueImpl;
import cdmw.ccm.configurator.StandardConfiguratorFactoryImpl;

import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.XMLParserHelper;

import com.thalesgroup.HelloModule.HelloWorld;

import org.omg.CORBA.Any;
import org.omg.Components.ConfigValue;
import org.omg.Components.StandardConfigurator;
import java.io.*;

public class ConfiguratorTest extends Testable {

    public static final int POA_THREAD_POOL_SIZE = 10;

    public static final String PROPERTIES_FILE
        = new java.io.File("./properties.xml").getAbsolutePath();

    public static final String INTERFACE_ID
        = "IDL:thalesgroup.com/HelloModule/HelloWorld:1.0";

    private static org.omg.CORBA.ORB orb = null;

    private HelloWorld hello = null;

    private StandardConfigurator configurator;

    private XMLParser parser;

    public ConfiguratorTest() {
        this.name = "Configurator Test";
        this.out = System.out;
    }

    public void doTests() {
        
        // set number of requested successfull tests
       // setNbOfRequestedTestOK(7);
	setNbOfRequestedTestOK(6);

        try {
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            //  String[] orbArgs = {"-ORBconfig", "./obtest.conf"};
            String[] orbArgs = {
                "-ORBInitRef", "XMLService=corbaloc::localhost:5060/XMLService",
                "-ORBInitRef", "InterfaceRepository=corbaloc::localhost:21000/DefaultRepository"             
            };
            orb = ORBSupport.ORBInit(orbArgs, orbStrategies);
            XMLHelper.registerValueFactories(orb);

            parser = XMLParserHelper.narrow(
                orb.resolve_initial_references("XMLService"));

            // Install value type factory
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                org.omg.Components.ConfigValueHelper.id(),
                    new cdmw.ccm.deployment.ConfigValueFactoryImpl());

            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
            orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();

            StandardConfiguratorFactoryImpl confFactory
                = new StandardConfiguratorFactoryImpl(orb, rootPOA);
            configurator = confFactory.create_configurator(INTERFACE_ID);

            run();

        } catch(Exception e) {
            e.printStackTrace();
            if (orb != null) {
                orb.shutdown(false);
                orb.destroy();
            }
        }

    }

    public void run() {

        try {
            ServerThread server = new ServerThread();
            server.start();
            println("Sleeping a while to wait for the server...");
            Thread.currentThread().sleep(2000);

            String fileName = "./Hello.ior";
            org.omg.CORBA.Object obj = null;

            try {
                // org.omg.CORBA.Object obj =
                //     orb.string_to_object("relfile:/./Hello.ior");
                // hello = com.thalesgroup.HelloModule.HelloWorldHelper.narrow(obj);

                File f = new File(fileName);
                BufferedReader br = new BufferedReader( new FileReader( f ));

	            // get object reference from command-line argument file
                obj = orb.string_to_object( br.readLine() );

                br.close();
            } catch ( Exception ex )  {}

            if (obj == null) {
                System.out.println("Error opening reference filename :" + fileName);
            }

            hello = com.thalesgroup.HelloModule.HelloWorldHelper.narrow(obj);

            println("Starting basic tests...");
            basicTests();

            println("Starting tests with values read by the CpfReader...");
            cpfReaderTests();

            orb.shutdown(false);
            orb.destroy();
            server.destroy();
        } catch(Exception e) {
            e.printStackTrace();
            if (orb != null) {
                orb.shutdown(false);
                orb.destroy();
            }
        }

    }

    public void basicTests() {

        hello.say_hello();

        println("config values with correct values");
        ConfigValue[] values = new ConfigValue[1];
        ConfigValueImpl cv = new ConfigValueImpl();
        cv.name = "count";
        Any any = orb.create_any();
        any.insert_ulong(20);
        cv.value = any;
        values[0] = cv;

        try {
            println("set_configuration");
            configurator.set_configuration(values);
            println("configure");
            configurator.configure(hello);
            println("say_hello");
            hello.say_hello();
            succeed();
        } catch(org.omg.CORBA.SystemException se) {
            se.printStackTrace();
            fail();
        } catch(org.omg.Components.WrongComponentType wct) {
            wct.printStackTrace();
	    fail();
        }

        println("config values with a readonly attribute");
        values = new ConfigValue[2];
        ConfigValueImpl cv1 = new ConfigValueImpl();
        cv1.name = "coucou";
        any = orb.create_any();
        any.insert_string("hello");
        cv1.value = any;
        ConfigValueImpl cv2 = new ConfigValueImpl();
        cv2.name = "count";
        any = orb.create_any();
        any.insert_long(8);
        cv2.value = any;
        values[0] = cv1;
        values[1] = cv2;

        try {
            configurator.set_configuration(values);
	    configurator.configure(hello);
	    hello.say_hello();
            fail();
        } catch(org.omg.CORBA.SystemException se) {
            println(se.toString());
            fail();
        } catch(org.omg.Components.WrongComponentType wct) {
	    wct.printStackTrace();
            succeed();
        }

        println("config values with non relevant attributes");
        values = new ConfigValue[2];
        cv1 = new ConfigValueImpl();
        cv1.name = "toto";
        any = orb.create_any();
        any.insert_boolean(false);
        cv1.value = any;
        cv2 = new ConfigValueImpl();
        cv2.name = "titi";
        any = orb.create_any();
        any.insert_boolean(false);
        cv2.value = any;
        values[0] = cv1;
        values[1] = cv2;

        try {
            configurator.set_configuration(values);
            configurator.configure(hello);
            hello.say_hello();
            fail();
        } catch(org.omg.CORBA.SystemException se) {
            println(se.toString());
            fail();
        } catch(org.omg.Components.WrongComponentType wct) {
            wct.printStackTrace();
	    succeed();
        }

        println("config values with too many values");
        values = new ConfigValue[3];
        cv1 = new ConfigValueImpl();
        cv1.name = "count";
        any = orb.create_any();
        any.insert_ulong(200);
        cv1.value = any;
        cv2 = new ConfigValueImpl();
        cv2.name = "too";
        any = orb.create_any();
        any.insert_string("much");
        cv2.value = any;
        ConfigValueImpl cv3 = new ConfigValueImpl();
        cv3.name = "test";
        any = orb.create_any();
        any.insert_boolean(false);
        cv3.value = any;
        values[0] = cv1;
        values[1] = cv2;
        values[2] = cv3;

        try {
            configurator.set_configuration(values);
            configurator.configure(hello);
            hello.say_hello();
            fail();
        } catch(org.omg.CORBA.SystemException se) {
            println(se.toString());
            fail();
        } catch(org.omg.Components.WrongComponentType wct) {
            wct.printStackTrace();
	    succeed();
        }
/*
        println("config values with a bad type");
        values = new ConfigValue[1];
        cv = new ConfigValueImpl();
        cv.name = "count";
        any = orb.create_any();
        String s = "ciao";
	any.insert_string(s);
        cv.value = any;
        values[0] = cv;

        try {
            configurator.set_configuration(values);
            configurator.configure(hello);
            hello.say_hello();
            fail();
        } catch(org.omg.CORBA.SystemException se) {
            println(se.toString());
            fail();
        } catch(org.omg.Components.WrongComponentType wct) {
            wct.printStackTrace();
	    succeed();
        }

        hello.say_hello();
*/
    }

    public void cpfReaderTests() {

        try {
            CpfReader reader = new CpfReader(
                parser, PROPERTIES_FILE, orb, INTERFACE_ID);
            println("Reading data from the XML file: "
                + PROPERTIES_FILE + "...");
            ConfigValues values = reader.getConfigValues();
            succeed();

            try {
                configurator.set_configuration(values.getConfigValueArray());
                configurator.configure(hello);
                hello.say_hello();
                succeed();
            } catch(org.omg.CORBA.SystemException se) {
                se.printStackTrace();
                fail();
            } catch(org.omg.Components.WrongComponentType wct) {
                wct.printStackTrace();
                fail();
            }

        } catch(com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException pe) {
            fail(pe.reason);
        } catch(org.omg.CORBA.SystemException se) {
            se.printStackTrace();
            fail();
        }

    }


}

