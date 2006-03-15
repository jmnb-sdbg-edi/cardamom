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


package cdmw.tools;

import cdmw.testutils.Testable;
import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;
import cdmw.orbsupport.ORBUtils;

/**
 * This class provides a java implementation
 * of the start_simu.sh and stop_simu.sh scripts.
 *
 */
public class SimuUtils {

    /**
     * User defined NameDomains
     */
    public static final String USER_NAMEDOMAINS
            = "System.resources Resource.Factories";

    /**
     * User defined RootContexts
     */
    public static final String USER_ROOTCONTEXTS = "DefaultRootContext";

    public static final String REQUIRED_NAMEDOMAINS =
         "CDMW/SERVICES CDMW/ADMIN CDMW/SERVICES/TRACE/COLLECTORS " +
            "CDMW/SERVICES/EVENT CDMW/SERVICES/EVENT/FACTORIES " +
            "CDMW/SERVICES/EVENT/EVENT_CHANNELS " +
            "CDMW/SERVICES/EVENT/TYPED_EVENT_CHANNELS";

    public static final String REQUIRED_ROOTCONTEXTS = "AdminRootContext";

    public static final String NAMEDOMAINS
            = REQUIRED_NAMEDOMAINS + " " + USER_NAMEDOMAINS;

    public static final String ROOTCONTEXTS
            = REQUIRED_ROOTCONTEXTS + " " + USER_ROOTCONTEXTS;

    public static final String NAME_PORT = "5010";
    public static final String PROP_PORT = "5015";
    public static final String REP_PORT = "5020";

    public static final String NAME_SERVICE_URL
            = "corbaloc::localhost:" + NAME_PORT + "/NameService";

    public static final String REPOSITORY_FILE
        = "SimRepository.IOR";
    public static final String PROCESS_CALLBACK_FILE
        = "SimProcessCallback.IOR";

    public static ThreadedProcess namingp = null;
    public static ThreadedProcess propertyp = null;
    public static ThreadedProcess repositoryp = null;
    public static ThreadedProcess propinstall = null;

    public static void startSimu(java.io.PrintStream out) {

        try {
            out.println("Starting Name Service on port " + NAME_PORT + "...");
            namingp = ORBUtils.startNamingService(NAME_PORT);
            OS.sleep(Testable.getTimescale() * 3000);

            // Building Simulated Repository command arguments
            String namingCorbaloc = ORBUtils.NAMING_PORT_OPTION_SLAVE + "corbaloc::localhost:" + NAME_PORT + "/NameService";
            String simRepositoryArgs
                = " " +ORBUtils.PORT_OPTION_SLAVE + REP_PORT + " "
                + namingCorbaloc;
            java.util.StringTokenizer st
                = new java.util.StringTokenizer(NAMEDOMAINS);
            while(st.hasMoreTokens()) {
                simRepositoryArgs += " -DOM " + st.nextToken();
            }

            st = new java.util.StringTokenizer(ROOTCONTEXTS);
            while(st.hasMoreTokens()) {
                simRepositoryArgs += " -ROOT " + st.nextToken();
            }

            out.println("Starting SimRepository Service on port " + REP_PORT + "...");
            repositoryp = OS.createProcess(
                "cdmw_simulated_repository" + simRepositoryArgs);
            OS.sleep(Testable.getTimescale() * 3000);

            out.println("Starting Property Service on port " + PROP_PORT + "...");
            /*String propertyServerArgs = "--RepPort=" + REP_PORT + 
                " --PropPort=" + PROP_PORT + " --CreatePropertySet";
            propertyp = OS.createJavaProcess(
                "cdmw.tools.PropertyServer " + propertyServerArgs,
                Testable.getProperties());*/
            propertyp = ORBUtils.startPropertyService(PROP_PORT);
            OS.sleep(Testable.getTimescale() * 4000);

        } catch(java.io.IOException ioe) {
            out.println(ioe.getMessage());
        } catch(InterruptedException ie) {
            out.println(ie.getMessage());
        }

    }

    public static void stopSimu(java.io.PrintStream out) {

        if (namingp!=null) {
            out.print("Killing the Name Server...");
            OS.killProcess(namingp);
            out.println(" Done.");
        }

        if (propertyp!=null) {
            out.print("Killing the Property Server...");
            OS.killProcess(propertyp);
            out.println(" Done.");
        }

        if (repositoryp!=null) {
            out.print("Killing the Simulated Repository...");
            OS.killProcess(repositoryp);
            out.println(" Done.");
        }

        if (OS.fileExists(REPOSITORY_FILE)) {
            out.print("Removing " + REPOSITORY_FILE + "...");
            OS.unlink(REPOSITORY_FILE);
            out.println(" Done.");
        }

        if (OS.fileExists(PROCESS_CALLBACK_FILE)) {
            out.print("Removing " + PROCESS_CALLBACK_FILE + "...");
            OS.unlink(PROCESS_CALLBACK_FILE);
            out.println(" Done.");
        }

    }

    public static void main(String[] args) {

        startSimu(System.out);

        try {
            OS.sleep(Testable.getTimescale() * 2000);
        } catch(InterruptedException ie) {
            ie.printStackTrace();
        }

        stopSimu(System.out);

    }

 }

