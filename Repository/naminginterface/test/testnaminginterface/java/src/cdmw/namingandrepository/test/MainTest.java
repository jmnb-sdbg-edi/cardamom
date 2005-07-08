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


package cdmw.namingandrepository.test;

import cdmw.common.Options;
import cdmw.testutils.Testable;
import cdmw.common.BadParameterException;
import cdmw.common.BadOrderException;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.ORBUtils;

import cdmw.platformmngt.PlatformInterface;

public class MainTest {

    private final static int SUCCESS = 0;
    private final static int FAILURE = 1;

    public static int run(
        org.omg.CORBA.ORB orb, String[] args, String callbackIOR) {

        // Testing NameParser
        ToNameTest toName = new ToNameTest();
        toName.start();

        ToStringTest toString = new ToStringTest();
        toString.start();

        org.omg.PortableServer.POA root = null;
        org.omg.CORBA.Object obj = null;
        try {
            // Resolve the Root POA
            obj = orb.resolve_initial_references("RootPOA");
            root = org.omg.PortableServer.POAHelper.narrow(obj);
            // Initialize the server so that incoming requests are
            // handled from this point on
            root.the_POAManager().activate();
        } catch (org.omg.CORBA.ORBPackage.InvalidName in) {
            System.out.println("Can't resolve `RootPOA'");
            return FAILURE;
        } catch(org.omg.PortableServer.POAManagerPackage.AdapterInactive ai) {
            System.out.println("Can't activate POA Manager");
            return FAILURE;
        }

        // The naming context used for the tests
        org.omg.CosNaming.NamingContext nc = null;

        if ( callbackIOR.length() != 0 ) {

            System.out.println("Getting repository from ProcessCallbak");

            try {
                // setup platforminterface
                PlatformInterface.setup(orb,args);
                // acknowledge the creation of the process
                PlatformInterface.acknowledgeCreation(
                    new MainTestProcessBehaviour(orb));
            } catch(BadOrderException boe) {
                boe.printStackTrace();
                return FAILURE;
            } catch(BadParameterException bpe) {
                bpe.printStackTrace();
                return FAILURE;
            }

            orb.run();

        } else {

            // Get naming service
            try {
                obj = orb.resolve_initial_references("NameService");
            } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
                System.out.println(args[0] + ": Can't resolve `NameService'");
                return FAILURE;
            }

            if (obj == null) {
                System.out.println("`NameService' is a nil object reference");
                return FAILURE;
            }

            nc = org.omg.CosNaming.NamingContextHelper.narrow(obj);
            if (nc == null) {
                System.out.println("`NameService' is not a NamingContext object reference");
                return FAILURE;
            }

            // Create TestObject implementation
            System.out.println("Creating TestObject...");
            TestObjectImpl test = new TestObjectImpl(root);
            System.out.println("Getting test ref...");
            TestObject testRef = test._this(orb);

            // Testing ResolveTest
            ResolveTest resolveTest = new ResolveTest(nc, testRef);
            resolveTest.start();

            // Testing BindContextTest
            BindContextTest bindContextTest = new BindContextTest(nc, testRef);
            bindContextTest.start();

            // Testing BindTest
            BindTest bindTest = new BindTest(nc, testRef);
            bindTest.start();

            // Testing UnbindTest
            UnbindTest unbindTest = new UnbindTest(nc, testRef);
            unbindTest.start();

            // Testing ListTest
            ListTest listTest = new ListTest(nc, testRef);
            listTest.start();

            // Testing TestNamingInterfaceDestroy
            DestroyTest destroyTest = new DestroyTest(nc, testRef);
            destroyTest.start();
        }

        return SUCCESS;
    }

    public static void main(String[] args) {
        // callback IOR
        String callbackIOR = OS.getOptionValue(args, Options.CALLBACK_IOR_OPTION);

        // We allow the user to specify a different port number for the Name Service
        String option = "--CdmwNamePort";
        String portNumber = OS.getOptionValue(args, option);

        if ( portNumber.equals("no") ) {
            portNumber="5000";
        }

        int status = SUCCESS;
        ThreadedProcess namingp = null;

        try {
            namingp = ORBUtils.startNamingService(portNumber);
            OS.sleep(Testable.getTimescale() * 3000);
        } catch(java.io.IOException ioe) {
            System.out.println(ioe.getMessage());
            status = FAILURE;
        } catch(InterruptedException ie) {
            System.out.println(ie.getMessage());
            status = FAILURE;
        }

        org.omg.CORBA.ORB orb = null;

        String s = ORBUtils.NAMING_PORT_OPTION_SLAVE +
                    "corbaloc::localhost:" + portNumber + "/NameService";
        s += (callbackIOR.equals("yes") || callbackIOR.equals("no"))
                ? "" : " " + Options.CALLBACK_IOR_OPTION + "=" + callbackIOR;

        String[] strArgs = s.split(" ");

        orb = org.omg.CORBA.ORB.init(strArgs, ORBUtils.getProperties());
        status = run(orb, strArgs, (callbackIOR.equals("yes") ||
                                    callbackIOR.equals("no")) ? "" : callbackIOR);

        /*
        if (callbackIOR.equals("no") || callbackIOR.equals("yes")) {          
            String[] strArgs = {
                "-ORBnaming",
                "corbaloc::localhost:" + portNumber + "/NameService"
            };
            orb = org.omg.CORBA.ORB.init(strArgs, ORBUtils.getProperties());
            status = run(orb, strArgs, "");
        } else {
            String[] strArgs = {
                "-ORBnaming",
                "corbaloc::localhost:" + portNumber + "/NameService",
                Options.CALLBACK_IOR_OPTION + "=" + callbackIOR
            };
            orb = org.omg.CORBA.ORB.init(strArgs, ORBUtils.getProperties());
            status = run(orb, strArgs, callbackIOR);
        }
        */

        if(orb != null && namingp != null) {
            // We destroy the ORB
            orb.destroy();
            // And we kill the Name Server
            OS.killProcess( namingp );
        }

        if (status == SUCCESS) {
            System.out.println("Main test successful.");
        } else {
            System.out.println("Main test failed.");
        }
    }

}

