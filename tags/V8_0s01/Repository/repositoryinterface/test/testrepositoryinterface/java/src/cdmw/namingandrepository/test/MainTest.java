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


package cdmw.namingandrepository.test;

import cdmw.testutils.Testable;
import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;
import cdmw.orbsupport.ORBUtils;

public class MainTest {

    private final static int SUCCESS = 0;
    private final static int FAILURE = 1;

    public static int run(org.omg.CORBA.ORB orb, String[] args) {

        org.omg.CORBA.Object obj = null;
        try {
            //
            // Resolve the Root POA
            //
            obj = orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA root = 
                org.omg.PortableServer.POAHelper.narrow(obj);

            //
            // Initialize the server so that incoming requests are
            // handled from this point on
            //
            root.the_POAManager().activate();
        } catch(Exception e) {
            System.out.println("Can't resolve root POA");
            return FAILURE;
        }

        //
        // Get naming service
        //
        try {
            obj = orb.resolve_initial_references("NameService");
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.out.println("Can't resolve `NameService'");
            return FAILURE;
        }

        if(obj==null) {
            System.out.println("`NameService' is a nil object reference");
            return FAILURE;
        }

        org.omg.CosNaming.NamingContextExt nc = 
            org.omg.CosNaming.NamingContextExtHelper.narrow(obj);

        if (nc == null) {
            System.out.println("`NameService' is not a NamingContext object "
                + "reference");
            return FAILURE;
        }

        //Testing TestNamingInterfaceResolve
        RepositoryTest rt = new RepositoryTest(nc);
        rt.start();
        
        if (rt.failedTestsCount() == 0) {
            return SUCCESS;
        } else {
            return FAILURE;
        }
    }

    public static void main(String[] args) {

        int status = SUCCESS;
        int timescale = Testable.getTimescale();
        try {

            // We allow the user to specify a different port number for the Name Service
            String option = "--CdmwNamePort";
            String portNumber = OS.getOptionValue(args, option) ;

            if ( portNumber.equals("no") ) {
                portNumber = "5000";
            }
			// Starting the Name Service
            System.out.println("Starting naming service on port " + portNumber + "...");
            ThreadedProcess namingp = ORBUtils.startNamingService(portNumber);
            OS.sleep(timescale * 10000);

            String namingCorbaloc = ORBUtils.NAMING_PORT_OPTION_SLAVE + "corbaloc::localhost:" + portNumber + "/NameService";
            // Starting the Repository Simulator
            String simRepositoryArgs = " " + namingCorbaloc;
            simRepositoryArgs += " -DOM MyDefault.system/MyDefault.domain";
            simRepositoryArgs += " -DOM MyDefault.system/MyDefault.domain/dom1.1/dom2.2";
            simRepositoryArgs += " -DOM MyDefault.system/MyDefault.domain/dom2.1";
            simRepositoryArgs += " -DOM Another.system/Another.domain";
            simRepositoryArgs += " -DOM Another.system/Another.domain/domX.Y ";

            System.out.println("Starting simulated repository...");
            ThreadedProcess repositoryp = OS.createProcess(
                "cdmw_simulated_repository" + simRepositoryArgs);
            OS.sleep(timescale * 10000);

            /*
            String[] strArgs = {
                ORBUtils.NAMING_PORT_OPTION + "corbaloc::localhost:" + portNumber + "/NameService"

            };
            */
            String s = ORBUtils.NAMING_PORT_OPTION + "corbaloc::localhost:" + portNumber + "/NameService";
            String[] strArgs = s.split(" ");

            org.omg.CORBA.ORB orb = 
                org.omg.CORBA.ORB.init(strArgs, ORBUtils.getProperties());
            status = run(orb, strArgs);

            if(orb != null) {
                try {
                    orb.destroy();
                }
                catch(Exception ex) {
                    ex.printStackTrace();
                    status = FAILURE;
                }
            }

            // And we kill the Repository simulator and Name Server
            if (repositoryp != null) OS.killProcess( repositoryp );
            if (namingp != null) OS.killProcess( namingp );

        } catch(Exception ex) {
            ex.printStackTrace();
            status = FAILURE;
        }

        if (status == SUCCESS) {
            System.out.println("Main test successful.");
        } else {
            System.out.println("Main test failed.");
        }

    }

}

