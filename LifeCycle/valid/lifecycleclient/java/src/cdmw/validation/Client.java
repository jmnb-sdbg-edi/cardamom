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

package cdmw.validation;

public class Client {

    public static final int POA_THREAD_POOL_SIZE = 5;

    public static void main(String[] args) {

        String allTests =
            cdmw.ossupport.OS.getOptionValue(args, "--interactive");

        // ---- Does it use interactive mode? ----
        boolean interactiveMode = false;
        if ( !allTests.equals("no") ) {
            interactiveMode = true;
        }

        // Command for all the tests
        String commands = "verbose \n"
            + "sleep 6000\n"
            + "entity\n"
            + "unmanaged\n"
            + "session\n"
            + "service\n"
            + "stop Resource.Factories/Name Process Agent.factory\n"
            + "stop Resource.Factories/Anonymous Process Agent.factory\n"
            + "exit \n";

        org.omg.CORBA.ORB orb = null;

        try {
            // Initialize the ORB
            cdmw.orbsupport.StrategyList orbStrategies =
                new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies);

            java.io.BufferedReader in = null;
            if (interactiveMode) {
                // Enter interactive mode
                in = new java.io.BufferedReader(
                    new java.io.InputStreamReader(System.in));
            } else {
                in = new java.io.BufferedReader(
                    new java.io.StringReader(commands));
            }

            cdmw.cdmwinit.ProcessControl ctrl
                = new cdmw.lifecycle.MyProcessControl(orb, in, System.out);

            cdmw.cdmwinit.CdmwInterface.cdmwInit(orb, args, ctrl,
                "cdmw.generated.ClientCdmwInterface");

            // Get the root POA
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));

            // Run implementation (Activate the POA manager)
            rootPOA.the_POAManager().activate();

            orb.run();
        } catch (Exception ex) {
            ex.printStackTrace();
        }

        if (orb != null) {
            try {
                cdmw.cdmwinit.CdmwInterface.cdmwCleanup(orb,
                    "cdmw.generated.ClientCdmwInterface");
                orb.destroy();
            } catch(Exception ex) {
                ex.printStackTrace();
            }
        }

    }

}

