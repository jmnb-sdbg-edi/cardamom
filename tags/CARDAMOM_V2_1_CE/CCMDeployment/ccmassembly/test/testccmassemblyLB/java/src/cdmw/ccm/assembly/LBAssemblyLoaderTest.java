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


package cdmw.ccm.assembly;

import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ORBSupport;

import cdmw.xmlutils.XMLParserImpl;
import cdmw.xmlutils.XMLHelper;

import org.omg.Components.CreateFailure;


public class LBAssemblyLoaderTest
    extends AssemblyLoaderTest
{

    private static final String LB_ASSEMBLY_DIR = "LBAssemblyLoaderTest";

    protected static final String SRV_BKUP_PROC_ID = "ServerProcessBackup";
    protected static final String SRV_BKUP_COMP_ID = "ServerBackup";

    protected static final String COMP_GROUP_ID = "ServerGroup";
    protected static final String COMP_GROUP_DEST = "ServerGroup";


    /**
     * Constructor.
     */
    public LBAssemblyLoaderTest() {
        this.out = System.out;
        this.name = "AssemblyLoader tests (+ load balanced assembly)";
        this.assemblyDirectory = LB_ASSEMBLY_DIR;
        this.assembly = null;
    }


    /**
     * Runs the tests.
     */
    public void doTests() {
        // Runs the non-LB tests first
        super.doTests();

        println("Now testing Load Balancing specifications");
        addNbOfRequestedTestOK(9);

        println("Check LBComponentServers");

        ComponentServer serverProc = assembly.getComponentServer(SRV_PROC_ID);
        try {
            LBComponentServer lbServerProc = (LBComponentServer) serverProc;
            succeed();
        } catch (ClassCastException e) {
            fail("ComponentServer " + SRV_PROC_ID +
                 " is not a LBComponentServer.");
        }

        ComponentServer serverBackProc =
            assembly.getComponentServer(SRV_BKUP_PROC_ID);
        try {
            LBComponentServer lbServerBackProc =
                (LBComponentServer) serverBackProc;
            succeed();
        } catch (ClassCastException e) {
            fail("ComponentServer " + SRV_BKUP_PROC_ID +
                 " is not a LBComponentServer.");
        }

        println("Check LBComponentGroups");

        ComponentGroup compGroup = assembly.getComponentGroup(COMP_GROUP_ID);
        if (compGroup != null) {
            succeed();
        } else {
            fail("ComponentGroup " + compGroup.getId() + " not loaded.");
        }

        if (SERVER_REP_ID.equals(compGroup.getRepositoryId())) {
            succeed();
        } else {
            fail("ComponentGroup " + compGroup.getId() + " has RepositoryId " +
                 compGroup.getRepositoryId() + " while expected RepositoryId " +
                 "is " + SERVER_REP_ID);
        }

        Component server1 = assembly.getComponent(SRV_COMP_ID_1);
        if (server1.equals(compGroup.getComponent(SRV_COMP_ID_1))) {
            succeed();
        } else {
            fail(server1.getId() + " is not a member of ComponentGroup " +
                 compGroup.getId());
        }

        Component serverBack = assembly.getComponent(SRV_BKUP_COMP_ID);
        if (serverBack.equals(compGroup.getComponent(SRV_BKUP_COMP_ID))) {
            succeed();
        } else {
            fail(serverBack.getId() + " is not a member of ComponentGroup " +
                 compGroup.getId());
        }

        try {
            LBComponentGroup lbCompGroup = (LBComponentGroup) compGroup;
            succeed();

            if (assembly.equals(lbCompGroup.getParentAssembly())) {
                succeed();
            } else {
                fail("LBComponentGroup " + lbCompGroup.getId() +
                     " has a wrong parent assembly.");
            }

            if (COMP_GROUP_DEST.equals(lbCompGroup.getDestination())) {
                succeed();
            } else {
                fail("LBComponentGroup " + lbCompGroup.getId() +
                     " has " + lbCompGroup.getDestination() +
                     " as destination instead of " + COMP_GROUP_DEST);
            }
        } catch (ClassCastException e) {
            fail("ComponentGroup " + COMP_GROUP_ID +
                 " is not a LBComponentGroup");
        }
    }


    /**
     * Main.
     */
    public static void main(String[] args) {
        try {
            // Initialize the ORB.
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = ORBSupport.ORBInit(args, orbStrategies);

            rootPOA = org.omg.PortableServer.POAHelper.narrow(
                orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();

            // Register the XML value factories.
            XMLHelper.registerValueFactories(orb);

            // Create an XML Parser object.
            String[] parserArgs = {"--validate",
                                   "--doctypedir=" + DTD_DIR};
            parser = XMLParserImpl.init(orb, rootPOA, parserArgs);

            // Create a PackageManagerStub object.
            managerStub = new PackageManagerStub();
            manager = managerStub._this(orb);

            // Register the NamingService stub.
            registerNamingServiceStub(orb);

            // Register the HomeFinder stub.
            registerHomeFinderStub(orb);

            // Initialize the AssemblyFactory.
            AssemblyFactoryImpl.init(
                    orb, rootPOA, rootPOA, null, parser, manager);

            // Run the tests.
            LBAssemblyLoaderTest test = new LBAssemblyLoaderTest();
            test.start();
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (orb != null) {
            orb.destroy();
        }
    }


}
