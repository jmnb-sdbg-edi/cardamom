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


package cdmw.ccm.container.test;

import cdmw.testutils.Testable;
import cdmw.ossupport.OS;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;

public class MainTest {

    public final static int POA_THREAD_POOL_SIZE = 5;

    public static void main(String[] args) {

        org.omg.CORBA.ORB orb = null;

        try {
            // Initialize the ORB
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            orb = ORBSupport.ORBInit(args, orbStrategies);

            CdmwInterface.cdmwInit(orb, args);

            //
            // Get the root POA
            //
            org.omg.CORBA.Object obj =
                orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(obj);

            //
            // Run implementation (Activate the POA manager)
            //
            rootPOA.the_POAManager().activate();


            TestThread thr = new TestThread(orb, rootPOA);
            thr.start();
            orb.run();
            thr.join();
        } catch (Exception e) {
            e.printStackTrace();
        }

        if(orb != null) {
            try {
                CdmwInterface.cdmwCleanup(orb);
                orb.destroy();
            } catch(Exception e) {
                e.printStackTrace();
            }
        }

    }

    public static class TestThread extends Thread {

        private org.omg.CORBA.ORB orb;

        private org.omg.PortableServer.POA poa;

        public TestThread(
            org.omg.CORBA.ORB orb,
            org.omg.PortableServer.POA poa) {
            this.orb = orb;
            this.poa= poa;
        }

        public void run() {

            try {
                OS.sleep(Testable.getTimescale() * 2000);
            } catch(InterruptedException ie) {}
            SessionContainerTest session =
                new SessionContainerTest(this.orb, this.poa);
            session.start();
            orb.shutdown(false);

        }

    }

}

