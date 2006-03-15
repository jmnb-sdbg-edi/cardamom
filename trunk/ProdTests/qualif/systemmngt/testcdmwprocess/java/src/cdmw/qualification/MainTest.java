/* =========================================================================== *
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
 * =========================================================================== */

package cdmw.qualification;

import cdmw.ossupport.OS;

public class MainTest {

    public static final int POA_THREAD_POOL_SIZE = 5;

    public static final int EXIT_FAILURE = -1;

    public static final String UNMANAGED_OPTION = "--unmanaged";
    public static final String SLEEP_OPTION = "--Sleep";
    public static final String MONITORING_REQUEST_OPTION = "--Request";
    public static final String MONITORING_EXCEPT_OPTION = "--Exception";

    public static void usage() {
        System.out.println("Usage:\n"
           + "cdmw.qualification.MainTest options\n"
           + "\n"
           + "Options:\n"
           + "--help                             Display this information.\n"
           + "[--unmanaged]                      Process is not managed.\n"
           + "[--Sleep=<time>]                   Time before is_alive return.\n"
           + "[--Request=<bool>]                 bool returned by is_alive\n"
           + "[--Exception=<bool>]               exception raised by is_alive (after a certain number of times)\n");
    }

    public static void main(String[] args) {

        if (run(args) == EXIT_FAILURE) {
            System.out.println("Test failed.");
        } else {
            System.out.println("Test successful.");
        }

    }

    public static int run(String[] args) {


        // the return code
        int retCode = 0;
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
            String unmanagedOption = OS.getOptionValue(args, UNMANAGED_OPTION);
            boolean managedProcess = true;
            if (unmanagedOption.equals("yes")) {
                managedProcess = false;
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

            System.out.println("test cdmw process, unmanagedOption:" + unmanagedOption + " sleepTimeOption:" + sleepTimeOption + " monitoringRequestOption:" + monitoringRequestOption + " monitoringExceptOption:" + monitoringExceptOption);

            // Initialize the ORB
            cdmw.orbsupport.StrategyList orbStrategies =
                new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies);

            java.io.BufferedReader in = null;

            // Get the root POA
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));

            // Run implementation (Activate the POA manager)
            rootPOA.the_POAManager().activate();

            if (managedProcess) {
                try {
                    cdmw.cdmwinit.ProcessControl ctrl
                      = new cdmw.platformmngt.MyProcessControl(orb, System.out, sleepTime, monitoringRequest, monitoringExcept);

                    cdmw.cdmwinit.CdmwInterface.cdmwInit(orb, args, ctrl,
                                                         "cdmw.generated.CdmwInterface");

                    orb.run();
                } catch(Exception e) {
                    // cleanup the ORB to exit completely
                    orb.shutdown(false);
                    orb.destroy();
                    return EXIT_FAILURE;
                }
            
            } else {
                System.out.println("Start Unmanaged Process : "
                                   + "cdmw.qualification.MainTest");

                // start orb
                orb.run();
            }

        } catch (Exception ex) {
            ex.printStackTrace();
            return EXIT_FAILURE;
        }

        if (orb != null) {
            try {
                cdmw.cdmwinit.CdmwInterface.cdmwCleanup(orb,
                    "cdmw.generated.CdmwInterface");
                System.out.println("Destroying the orb...");
                orb.destroy();
            } catch(Exception ex) {
                ex.printStackTrace();
                return EXIT_FAILURE;
            }
        }
        
        return retCode;
    }

}

