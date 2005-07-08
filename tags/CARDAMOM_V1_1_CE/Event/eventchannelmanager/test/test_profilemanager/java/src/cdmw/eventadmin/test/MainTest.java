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


package cdmw.eventadmin.test;

import cdmw.testutils.Testable;
import cdmw.common.Locations;
import cdmw.common.Options;
import cdmw.common.Assert;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.eventadmin.InitUtils;
import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import com.thalesgroup.CdmwPlatformMngt.ProcessCallback;
import com.thalesgroup.CdmwPlatformMngt.ProcessCallbackHelper;
import com.thalesgroup.CdmwNamingAndRepository.Repository;
import com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper;
import com.thalesgroup.CdmwEventAdmin.EventChannelProfileManager;
import com.thalesgroup.CdmwEventAdmin.EventChannelProfileManagerHelper;

public class MainTest {

    public static final int SUCCESS = 0;

    public static final int FAILURE = 1;

    public static final int POA_THREAD_POOL_SIZE = 5;

    // The naming context of the current Process object within the naming
    public final static String PROC_CTXT
        = "CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_000";


    public static ProcessCallback getProcessCallback(org.omg.CORBA.ORB orb)
        throws org.omg.CORBA.TRANSIENT {

        Assert.check(orb != null);
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
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        if(obj==null) {
            System.out.println("`NameService' is a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        org.omg.CosNaming.NamingContextExt nc
            = org.omg.CosNaming.NamingContextExtHelper.narrow(obj);

        if (nc==null) {
            System.out.println("`NameService' is not a NamingContextExt object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        String PROCESS_CALLBACK_BINDING_NAME
            = "CDMW.I/process-callback.simulated";

        // Get reference to the ProcessCallback interface
        ProcessCallback callback = null;
        try {
            NamingInterface ni = new NamingInterface(nc);
            org.omg.CORBA.Object obj_ = ni.resolve(PROCESS_CALLBACK_BINDING_NAME);
            callback = ProcessCallbackHelper.narrow(obj_);
            if (callback==null) {
                System.out.println("Couldn't retrieve reference of the ProcessCallback");
                // TODO: minor cde to be defined
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENT,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            System.out.println("Couldn't retrieve reference of the ProcessCallback");
            // TODO: minor cde to be defined
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENT,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        return callback;
    }


    public static void initRepository(org.omg.CORBA.ORB orb)
        throws org.omg.CORBA.SystemException, 
        org.omg.CORBA.ORBPackage.InvalidName,
        com.thalesgroup.CdmwNamingAndRepository.NoNameDomain,
        com.thalesgroup.CdmwNamingAndRepository.InvalidName {

        //
        // Get naming service
        //
        org.omg.CORBA.Object obj = null;
        org.omg.CosNaming.NamingContextExt nc = null;

        try {
            obj = orb.resolve_initial_references("NameService");
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.out.println("Can't resolve `NameService'");
            throw in;
        }

        if (obj == null) {
            System.out.println("`NameService' is a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        nc = org.omg.CosNaming.NamingContextExtHelper.narrow(obj);

        if (nc == null) {
            System.out.println("`NameService' is not a NamingContextExt object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        String REPOSITORY_NAME = "CDMW.I/repository.simulated";

        // Get reference to the repository interface
        Repository rep = null;
        try {
            NamingInterface ni = new NamingInterface(nc);
            org.omg.CORBA.Object obj_ = ni.resolve(REPOSITORY_NAME);

            rep = RepositoryHelper.narrow(obj_);
            if (rep==null) {
                System.out.println("Couldn't retrieve reference of the Repository");
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Couldn't retrieve reference of the Repository");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
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


    public static int run(org.omg.CORBA.ORB orb) {
        //
        // Get NamingInterface object from RepositoryInterface
        //
        NamingInterface ni = null;
        try {
            ni = RepositoryInterface.getDomainNamingInterface(
                Locations.CDMW_SERVICES_NAME_DOMAIN);
        } catch(com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            nnd.printStackTrace();
            return FAILURE;
        } catch(com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            in.printStackTrace();
            return FAILURE;
        }

        //
        // Get a reference on the EventChannelProfileManager
        //
        EventChannelProfileManager manager = null;
        try {
            org.omg.CORBA.Object obj
                = ni.resolve("EVENT/EventChannelProfileManager");
            manager = EventChannelProfileManagerHelper.narrow(obj);
        } catch(org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            nf.printStackTrace();
            return FAILURE;
        } catch(org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            cp.printStackTrace();
            return FAILURE;
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            in.printStackTrace();
            return FAILURE;
        }

        if (manager==null) {
            System.out.println("EventChannelProfileManager is a nil object reference");
            return FAILURE;
        }

        // Testing TestEventChannelProfileManager
        System.out.println("EventChannelProfileManager reference found!");
        EventChannelProfileManagerTest testProfileManager
            = new EventChannelProfileManagerTest(manager);
        testProfileManager.start();

        return SUCCESS;
    }


    public static void main(String[] args) {

        int status = SUCCESS;
        int timescale = Testable.getTimescale();
        org.omg.CORBA.ORB orb = null;

        String callbackIOR
            = OS.getOptionValue( args, Options.CALLBACK_IOR_OPTION);

        boolean unitTests = true;
        // ---- Is it launched by platform management for integration tests? ----
        if ( !callbackIOR.equals("no") && !callbackIOR.equals("yes") ) {
            unitTests = false;
        }

        ThreadedProcess profileManager = null;

        try {
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            String[] serverArgs = { "-ORBconfig",
                                    "./obtest.conf" };
            orb = ORBSupport.ORBInit(serverArgs, orbStrategies);

            if (unitTests) {
                // Starting the simrepository
                cdmw.tools.SimuUtils.startSimu(System.out);
                OS.sleep(timescale * 20000);
                System.out.println("Simrepository started!");

                // init repository
                initRepository(orb);
                OS.sleep(timescale * 3000);
                System.out.println("Simrepository initialised!");

                // Getting ProcessCallback IOR
                callbackIOR = orb.object_to_string(getProcessCallback(orb));
            }

            // Starting the EventChannelProfileManager
            String profileManagerService = "cdmw.eventadmin.EventChannelManager";
            String managerArgs =
                " " + Options.CALLBACK_IOR_OPTION + "=" + callbackIOR +
                " " + Options.PROCESS_PORT_OPTION + "=15300" +
                " " + Options.PROCESS_XML_FILE_OPTION + "=./Server.xml" +
                " " + InitUtils.PROFILE_XML_FILE_OPTION + "=./EventChannelConf1.xml" +
                " " + Options.VALIDATE_XML_OPTION;

            System.out.println("Launch EventChannelManager" + managerArgs);
            profileManager = OS.createJavaProcess(
                profileManagerService + managerArgs, Testable.getProperties());
            OS.sleep(timescale * 20000);
            System.out.println("EventChannelProfileManager started");

            String commands;
            cdmw.tools.ProcessAdmin admin = null;
            if (unitTests) {
                String managerURL = "corbaname::localhost:5010#" + PROC_CTXT;
                com.thalesgroup.CdmwPlatformMngt.Process proc = null;
                org.omg.CORBA.Object obj = orb.string_to_object(managerURL);

                if (obj!=null) {
                    proc = com.thalesgroup.CdmwPlatformMngt.ProcessHelper.narrow(obj);
                    admin = new cdmw.tools.ProcessAdmin(orb, proc);

                    // send initialise order
                    commands = "verbose \n init LAST 2\n exit \n";
                    admin.run(new java.io.BufferedReader(
                        new java.io.StringReader(commands)), System.out);

                    // send run order
                    commands = "verbose \n run\n exit \n";
                    admin.run(new java.io.BufferedReader(
                        new java.io.StringReader(commands)), System.out);
                }
            }

            status = run(orb);

            if (unitTests) {
                // send stop order
                commands = "verbose \n stop\n exit \n";
                admin.run(new java.io.BufferedReader(
                    new java.io.StringReader(commands)), System.out);
            }

        } catch(Exception ex) {
            ex.printStackTrace();
            status = FAILURE;
        }

        if (orb!=null) {
            try {
                orb.destroy();
             } catch(Exception ex) {
                ex.printStackTrace();
                status = FAILURE;
             }
         }

        // And we kill launched process
        // for the simulated repository, launch process is a script shell
        // which ended when process are launched: no kill of this process
        // to stop sim repository, the stop_simu.sh script shall be called
        try {
            if (unitTests) cdmw.tools.SimuUtils.stopSimu(System.out);
            OS.sleep(timescale * 2000);

            OS.killProcess(profileManager);
        } catch (InterruptedException ie) {
            ie.printStackTrace();
            status = FAILURE;
        }

        if (status == SUCCESS) {
            System.out.println("Main test successful.");
        } else {
            System.out.println("Main test failed.");
        }

    }

}


