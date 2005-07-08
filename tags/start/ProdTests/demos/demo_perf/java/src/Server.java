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


package cdmw.demoperf;

public class Server {

    public static final int POA_THREAD_POOL_SIZE = 5;

    public static void main(String[] args) {

        System.out.println("Start Server");

        org.omg.CORBA.ORB orb = null;

        try {
            // ===================================================
            // Initialize the ORB
            // ===================================================
            cdmw.orbsupport.StrategyList orbStrategies =
                new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies);

            // ===================================================
            // Get the root POA 
            // ===================================================
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));
        
        
            // ===================================================
            // Activate the root POA manager
            // ===================================================
            rootPOA.the_POAManager().activate();

            // ===================================================
            // create the process control for platform management
            // ===================================================
            cdmw.cdmwinit.ProcessControl ctrl
                = new ServerProcessControl(orb);

            // ===================================================
            // Call generated CdmwInit
            // ===================================================
            cdmw.cdmwinit.CdmwInterface.cdmwInit(orb, args, ctrl,
                "cdmw.generated.ServerCdmwInterface");

            // ===================================================
            // wait for CORBA message
            // ===================================================
            orb.run();

        } catch (Exception ex) {
            ex.printStackTrace();
        }

        // ========================================================
        // program stopping
        // ========================================================
        
        // ===================================================
        // Call generated Cdmw cleanup
        // ===================================================
        cdmw.cdmwinit.CdmwInterface.cdmwCleanup(orb,
           "cdmw.generated.ServerCdmwInterface");

       // ===================================================
       // destroy orb
       // ===================================================
       if (orb != null) {
            try {
                orb.destroy();
            } catch(Exception ex) {
                ex.printStackTrace();
            }
        }

    }

}

