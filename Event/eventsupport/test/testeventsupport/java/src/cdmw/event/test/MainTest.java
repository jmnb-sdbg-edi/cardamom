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


package cdmw.event.test;

import cdmw.common.Locations;
import cdmw.common.Options;
import cdmw.common.Assert;
import cdmw.testutils.Testable;

import cdmw.ossupport.OS;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;
import cdmw.cdmwinit.CdmwInterface;

import org.omg.CORBA.ORB;
import org.omg.CORBA.SystemException;
import org.omg.CORBA.CompletionStatus;

public class MainTest {

    public static final int SUCCESS = 0;
    public static final int FAILURE = 1;

    public static final int POA_THREAD_POOL_SIZE = 5;

    // The naming context of the current Process object within the naming
    public final static String PROC_CTXT
        = "CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_000";

    // 3 seconds before calling initialise
    public final static int INIT_WAIT_DURATION = 3000;

    // 10 seconds processing time before calling stop
    public final static int PROCESSING_DURATION = 10000;

    public static final String FACTORY_SERVER_IOR = "FactoryServerIor";

    public static com.thalesgroup.CdmwPlatformMngt.ProcessCallback
        getProcessCallback(ORB orb) throws org.omg.CORBA.TRANSIENT {

        Assert.check(orb!=null);
        org.omg.CORBA.Object obj = null;

        //
        // Get naming service
        //
        try {
            obj = orb.resolve_initial_references("NameService");
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.out.println("Can't resolve `NameService'");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        if (obj==null) {
            System.out.println("`NameService' is a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        org.omg.CosNaming.NamingContextExt nc =
            org.omg.CosNaming.NamingContextExtHelper.narrow(obj);

        if (nc==null) {
            System.out.println("`NameService' is not a NamingContextExt object reference");
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                        CompletionStatus.COMPLETED_NO);
        }

        String PROCESS_CALLBACK_BINDING_NAME
            = "CDMW.I/process-callback.simulated";

        // Get reference to the ApplicationAgent interface
        com.thalesgroup.CdmwPlatformMngt.ProcessCallback callback = null;
        try {
            NamingInterface ni = new NamingInterface(nc);
            org.omg.CORBA.Object obj_ = ni.resolve(PROCESS_CALLBACK_BINDING_NAME);
            callback = com.thalesgroup.CdmwPlatformMngt.ProcessCallbackHelper.narrow(obj_);
            if (callback==null) {
                System.out.println("Couldn't retrieve reference of the ProcessCallback");
                // TODO: minor cde to be defined
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENT,
                        CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            System.out.println("Couldn't retrieve reference of the ProcessCallback");
            // TODO: minor cde to be defined
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENT,
                    CompletionStatus.COMPLETED_NO);
        }

        return callback;
    }


    public static void initRepository(ORB orb)
        throws SystemException, org.omg.CORBA.ORBPackage.InvalidName,
        com.thalesgroup.CdmwNamingAndRepository.NoNameDomain,
        com.thalesgroup.CdmwNamingAndRepository.InvalidName {

        //
        // Get naming service
        //
        org.omg.CORBA.Object obj = null;

        try {
            obj = orb.resolve_initial_references("NameService");
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.out.println("Can't resolve `NameService'");
            throw in;
        }

        if(obj==null) {
            System.out.println("`NameService' is a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        org.omg.CosNaming.NamingContextExt nc =
            org.omg.CosNaming.NamingContextExtHelper.narrow(obj);
        //org.omg.CosNaming.NamingContext nc =
        //    org.omg.CosNaming.NamingContextHelper.narrow(obj);

        if (nc==null) {
            System.out.println("`NameService' is not a NamingContextExt object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        String REPOSITORY_NAME = "CDMW.I/repository.simulated";

        // Get reference to the repository interface
        com.thalesgroup.CdmwNamingAndRepository.Repository rep = null;

        try {
            NamingInterface ni = new NamingInterface(nc);
            org.omg.CORBA.Object obj_ = ni.resolve(REPOSITORY_NAME);
            rep = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj_);
            if (rep==null) {
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                        CompletionStatus.COMPLETED_NO);
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Couldn't retrieve reference of the Repository");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }

        try {
            RepositoryInterface.init(Locations.CDMW_SERVICES_NAME_DOMAIN, rep);
        } catch(com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            System.out.println("Couldn't init Repository");
            throw nnd;
        } catch(com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            System.out.println("Couldn't init Repository");
            throw in;
        }

    }

    public static int run(org.omg.CORBA.ORB orb, String callbackIOR) {

        ClientThread client = new ClientThread(
            orb, cdmw.tools.SimuUtils.NAME_PORT, callbackIOR);
        client.start();
        try {
            client.join();
            return client.getStatus();
        } catch (InterruptedException ie) {
            return FAILURE;
        }

    }

    public static void main(String[] args) {

        System.out.println("===================================================");
        System.out.println("Starting test EventSupportTest");
        System.out.println("===================================================");

        int status = SUCCESS;
        int timescale = Testable.getTimescale();
        ORB orb = null;

        String callbackIOR
            = OS.getOptionValue( args, Options.CALLBACK_IOR_OPTION);

        boolean unitTests = true;
        // -- Is it launched by platform management for integration tests? --
        if ( !callbackIOR.equals("no") && !callbackIOR.equals("yes") ) {
            unitTests = false;
        }

        try {
            cdmw.orbsupport.StrategyList orbStrategies
                = new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            String[] serverArgs = { "-ORBconfig", "./obtest.conf"};
            orb = cdmw.orbsupport.ORBSupport.ORBInit(serverArgs, orbStrategies);

            if (unitTests) {
                // Starting the simrepository
                cdmw.tools.SimuUtils.startSimu(System.out);
                OS.sleep(timescale * 20000);
                System.out.println("Simrepository started!");

                // init repository
                initRepository(orb);
                OS.sleep(timescale * 3000);
                System.out.println("Simrepository initialised!");

                // Getting Callback IOR
                callbackIOR = orb.object_to_string(getProcessCallback(orb));
            }

            // Get the root POA
            org.omg.CORBA.Object obj
                = orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA
                = org.omg.PortableServer.POAHelper.narrow(obj);

            // Run implementation (Activate the POA manager)
            rootPOA.the_POAManager().activate();

            OS.sleep(timescale * 1000);
            System.out.println("Server initialised");

            // Starting the server containing the EventChannelFactory
            String[] servArgs = {
                Options.CALLBACK_IOR_OPTION + "=" + callbackIOR,
                Options.PROCESS_XML_FILE_OPTION + "=./Server.xml",
                Options.VALIDATE_XML_OPTION
            };
            CdmwInterface.cdmwInit(orb, servArgs,
                null, "cdmw.generated.ServerCdmwInterface");

            status = run(orb, callbackIOR);

        } catch(Exception e) {
            e.printStackTrace();
            status = FAILURE;
        }

        if (orb!=null) {
            try {
                CdmwInterface.cdmwCleanup(orb,
                    "cdmw.generated.ServerCdmwInterface");
                orb.destroy();
            } catch(Exception e) {
                e.printStackTrace();
                status = FAILURE;
            }
        }

        // And we kill launched process
        if (unitTests) cdmw.tools.SimuUtils.stopSimu(System.out);

        System.out.println("===================================================");
        System.out.println("Ending test EventSupportTest");
        System.out.println("===================================================");

        System.out.println("Summary:");
        if (status == SUCCESS) {
            System.out.println("   Main test successful.");
        } else {
            System.out.println("   Main test failed.");
        }
    }

}


