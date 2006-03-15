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


package cdmw.platformmngt.test;

import cdmw.common.Options;
import cdmw.common.BadOrderException;

import cdmw.ossupport.OS;
import cdmw.orbsupport.StrategyList;

import cdmw.platformmngt.PlatformInterface;

/**
 * The main test.
 *
 */
public class MainTest {

    public static final int EXIT_FAILURE = -1;

    public static final String UNMANAGED_OPTION = "--unmanaged";
    public static final String AUTO_END_OPTION = "--auto-end";
    public static final String NB_STEPS_OPTION = "--nb-steps";
    public static final String SERVICE_REQUEST_OPTION = "--service-request";
    public static final String HOST_ENTITY_SET_OPTION = "--host-entity-set";
    public static final String SYS_ENTITY_SET_OPTION = "--sys-entity-set";
    public static final String APP_ENTITY_SET_OPTION = "--app-entity-set";
    public static final String PROC_ENTITY_SET_OPTION = "--proc-entity-set";
    public static final String ENTITY_UPDATING_OPTION = "--entity-update";
    public static final String PROCESS_TIMEOUT_OPTION = "--process-timeout";
    public static final String SLEEP_OPTION = "--Sleep";
    public static final String MONITORING_REQUEST_OPTION = "--Request";
    public static final String MONITORING_EXCEPT_OPTION = "--Exception";

    public static void usage() {
        System.out.println("Usage:\n"
           + "cdmw.platformmngt.test.MainTest options\n"
           + "\n"
           + "Options:\n"
           + "--help                             Display this information.\n"
           + "[--unmanaged]                      Process is not managed.\n"
           + "[--auto-end=<time-out>]            Process will auto end after time out in msec.\n"
           + "[--nb-steps=<value>]               Number of init stepsName.\n"
           + "[--service-request=<name>]         Service to request.\n"
           + "[--host-entity-set=<name>]          Host entity to set.\n"
           + "[--sys-entity-set=<name>]          System entity to set.\n"
           + "[--app-entity-set=<name>]          Application entity to set.\n"
           + "[--proc-entity-set=<name>]         Process entity to set.\n"
           + "[--entity-update=<interval-time]   Entity updating interval in msec. \n"
           + "[--process-timeout=<time-out>]     Process will initialises, run and stop after\n"
           + "                                   a time out in msec.\n");
    }

    public static void main(String[] args) {

        if (run(args) == EXIT_FAILURE) {
            System.out.println("Test failed.");
        } else {
            System.out.println("Test successful.");
        }

    }

    public static int run(String[] args) {

        String applicationName;
        String processName =  "cdmw.platformmngt.test.MainTest";

        // the return code
        int retCode = 0;
        // the ORB
        org.omg.CORBA.ORB orb = null;

        //
        // help argument
        //
        if (args.length > 1) {
            if(args[0].equals("--help")) {
                usage();
                return retCode;
            }
        }


        try {
            StrategyList strategy = new StrategyList();
            strategy.addORBThreaded();
            strategy.addPOAThreadPerRequest();

            String unmanagedOption = OS.getOptionValue(args, UNMANAGED_OPTION);
            boolean managedProcess = true;
            if (unmanagedOption.equals("yes")) {
                managedProcess = false;
            }

            String autoEndOption = OS.getOptionValue(args, AUTO_END_OPTION);
            int autoEndTimeOut = -1;

            if (autoEndOption.equals("no")) {
            } else if (autoEndOption.equals("yes")) {
                System.out.println("bad value for "
                    + AUTO_END_OPTION + "=<time-out> option\n");
                return EXIT_FAILURE;
            } else if (!autoEndOption.equals("no")) {
                try {
                    autoEndTimeOut = Integer.valueOf(autoEndOption).intValue();
                } catch(NumberFormatException nfe) {
                    System.out.println("bad value for "
                        + AUTO_END_OPTION + "=<time-out> option\n");
                    return EXIT_FAILURE;
                }
            }

            String nbStepsOption = OS.getOptionValue(args, NB_STEPS_OPTION);
            int nbStepsValue;
            if (nbStepsOption.equals("no")) {
                nbStepsValue = 1;
            } else if (nbStepsOption.equals("yes")) {
                System.out.println("bad value for "
                    + NB_STEPS_OPTION + "=<value> option\n");
                return EXIT_FAILURE;
            } else {
                try {
                    nbStepsValue = Integer.valueOf(nbStepsOption).intValue();
                } catch(NumberFormatException nfe) {
                    System.out.println("bad value for "
                        + NB_STEPS_OPTION + "=<value> option\n");
                    return EXIT_FAILURE;
                }
            }

            String serviceRequestName
                = OS.getOptionValue(args, SERVICE_REQUEST_OPTION);
            if (serviceRequestName.equals("yes")) {
                System.out.println("bad value for "
                    + SERVICE_REQUEST_OPTION + "=<name> option\n");
                return EXIT_FAILURE;
            } else if (serviceRequestName.equals("no")) {
                serviceRequestName = "";
            }

            String hostEntityName = OS.getOptionValue (args, HOST_ENTITY_SET_OPTION);
            if (hostEntityName.equals("yes")) {
                System.out.println("bad value for "
                    + HOST_ENTITY_SET_OPTION + "=<name> option\n");
                return EXIT_FAILURE;
            } else if (hostEntityName.equals("no")) {
                hostEntityName = "";
            }

            String sysEntityName = OS.getOptionValue (args, SYS_ENTITY_SET_OPTION);
            if (sysEntityName.equals("yes")) {
                System.out.println("bad value for "
                    + SYS_ENTITY_SET_OPTION + "=<name> option\n");
                return EXIT_FAILURE;
            } else if (sysEntityName.equals("no")) {
                sysEntityName = "";
            }

            String appEntityName = OS.getOptionValue(args, APP_ENTITY_SET_OPTION);
            if (appEntityName.equals("yes")) {
                System.out.println("bad value for "
                    + APP_ENTITY_SET_OPTION + "=<name> option\n");
                return EXIT_FAILURE;
            }  else if (appEntityName.equals("no")) {
                appEntityName = "";
            }

            String procEntityName = OS.getOptionValue(args, PROC_ENTITY_SET_OPTION);
            if (procEntityName.equals("yes")) {
                System.out.println("bad value for "
                    + PROC_ENTITY_SET_OPTION + "=<name> option\n");
                return EXIT_FAILURE;
            } else if (procEntityName.equals("no")) {
                procEntityName = "";
            }

            String entityUpdtOption = OS.getOptionValue(args, ENTITY_UPDATING_OPTION);
            int entityUpdating = 5000;

            if (entityUpdtOption.equals("no")) {
            } else if (entityUpdtOption.equals("yes")) {
                System.out.println("bad value for "
                    + ENTITY_UPDATING_OPTION + "=<interval-time> option\n");
                return EXIT_FAILURE;
            } else if (!entityUpdtOption.equals("no")) {
                try {
                    entityUpdating = Integer.valueOf(entityUpdtOption).intValue();
                } catch(NumberFormatException nfe) {
                    System.out.println("bad value for "
                        + NB_STEPS_OPTION + "=<interval-time> option\n");
                    return EXIT_FAILURE;
                }
            }

            String processTimeOutOption = OS.getOptionValue(args, PROCESS_TIMEOUT_OPTION);
            short processTimeOut = 0;

            if (processTimeOutOption.equals("no")) {
            } else if (processTimeOutOption.equals("yes")) {
                System.out.println("bad value for "
                    + PROCESS_TIMEOUT_OPTION + "=<time-out> option\n");
                return EXIT_FAILURE;
            } else if (processTimeOutOption != "no") {
                try {
                    processTimeOut = Short.valueOf(processTimeOutOption).shortValue();
                } catch(NumberFormatException nfe) {
                    System.out.println("bad value for "
                        + PROCESS_TIMEOUT_OPTION + "=<time-out> option\n");
                    return EXIT_FAILURE;
                }
            }

            String sleepTimeOption = OS.getOptionValue(args, SLEEP_OPTION);
            int sleepTime = 0;
            if (sleepTimeOption.equals("no")) {
                sleepTime = 100;
            } else if (sleepTimeOption.equals("yes")) {
                System.out.println("bad value for "
                    + SLEEP_OPTION + "=<value> option\n");
                return EXIT_FAILURE;
            } else if (sleepTimeOption != "no") {
                try {
                    sleepTime = Short.valueOf(sleepTimeOption).intValue();
                } catch(NumberFormatException nfe) {
                    System.out.println("bad value for "
                        + SLEEP_OPTION + "=<value> option\n");
                    return EXIT_FAILURE;
                }
            }

            String monitoringRequestOption = OS.getOptionValue(args, MONITORING_REQUEST_OPTION);
            boolean monitoringRequest = true;
            if (monitoringRequestOption.equals("no")) {
                monitoringRequest = true;
            } else if (monitoringRequestOption.equals("true")) {
                monitoringRequest = true;
            } else if (monitoringRequestOption.equals("false")) {
                monitoringRequest = false;
            } else monitoringRequest = true;

            String monitoringExceptOption = OS.getOptionValue(args, MONITORING_EXCEPT_OPTION);
            boolean monitoringExcept = true;
            if (monitoringExceptOption.equals("no")) {
                monitoringExcept = false;
            } else if (monitoringExceptOption.equals("true")) {
                monitoringExcept = true;
            } else if (monitoringExceptOption.equals("false")) {
                monitoringExcept = false;
            } else monitoringExcept = false;

            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, strategy);

            org.omg.CORBA.Object obj = orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA poa = org.omg.PortableServer.POAHelper.narrow(obj);

            poa.the_POAManager().activate();

            if (managedProcess) {
                try {
                    boolean platformManaged =
                         PlatformInterface.isLaunchedByPlatformManagement(args);

                    if (platformManaged) {
                        // initialise the platform interface
                        PlatformInterface.setup(orb, args);
                    }

                    AutoEnding autoEnding = new AutoEnding(autoEndTimeOut);

                    // creates the process behaviour
                    MyProcessBehaviour myProcess = new MyProcessBehaviour(
                        orb, platformManaged, "cdmw.platformmngt.test.MainTest",
                        autoEnding, autoEndTimeOut, entityUpdating, nbStepsValue,
                        sleepTime, monitoringRequest, monitoringExcept);


                    if (serviceRequestName.length() != 0) {
                        // set service name to request
                        myProcess.setServiceToRequest(serviceRequestName);
                    }

                    if (hostEntityName.length() != 0) {
                        // set system entity name to set
                        myProcess.setHostEntityToSet(hostEntityName);
                    }

                    if (sysEntityName.length() != 0) {
                        // set system entity name to set
                        myProcess.setSysEntityToSet(sysEntityName);
                    }

                    if (appEntityName.length() != 0) {
                        // set application entity name to set
                        myProcess.setAppEntityToSet(appEntityName);
                    }

                    if (procEntityName.length() != 0) {
                        // set process entity name to set
                        myProcess.setProcEntityToSet(procEntityName);
                    }

                    if (platformManaged) {
                        // acknowledge the creation of the process
                        PlatformInterface.acknowledgeCreation(myProcess);

                        // sample usage of the platform interface
                        applicationName = PlatformInterface.getApplicationName();
                        processName = PlatformInterface.getProcessName();

                        System.out.println("Application name : " + applicationName);
                        System.out.println("Process name : " + processName);
                    } else {
                        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind
                            = new com.thalesgroup.CdmwPlatformMngtBase.StartupKind();
                        startupKind.state_retrieval
                            = com.thalesgroup.CdmwPlatformMngtBase.StateRetrieval.LAST_STATE;
                        startupKind.startup_mode = processTimeOut;
                        startupKind.specified_state_id = "";

                        myProcess.initialise(startupKind);
                        myProcess.run();
                    }

                    // start orb
                    orb.run();
                } catch(Exception e) {
                    // cleanup the ORB to exit completely
                    orb.shutdown(false);
                    orb.destroy();
                    throw e;
                }
            
            } else {
                System.out.println("Start Unmanaged Process : "
                    + "cdmw.platformmngt.test.MainTest");

                // start orb
                orb.run();
            }

            //orb.shutdown(false) is done by the ProcessBehaviour
            System.out.println("Destroying the orb...");
            orb.destroy();

        } catch(Exception e) {
            System.err.println("FAILURE : Unexpected exception");
            retCode = EXIT_FAILURE;
        }

        return retCode;

    }

}

