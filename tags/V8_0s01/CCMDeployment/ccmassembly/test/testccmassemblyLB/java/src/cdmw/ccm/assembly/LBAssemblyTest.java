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


public class LBAssemblyTest
    extends AssemblyTest
{
    protected static final String LB_SERVER_1_ID    = "lbserver1";
    protected static final String LB_SERVER_1_DEST  = "proc3@appli1";
    protected static final String LB_CONTAINER_1_ID = "lbcontainer1";
    protected static final String LB_HOME_1_ID      = "lbhome1";
    protected static final String LB_COMP_1_ID      = "lbcomp1";

    protected static final String LB_SERVER_2_ID    = "lbserver2";
    protected static final String LB_SERVER_2_DEST  = "proc4@appli1";
    protected static final String LB_CONTAINER_2_ID = "lbcontainer2";
    protected static final String LB_HOME_2_ID      = "lbhome2";
    protected static final String LB_COMP_2_ID      = "lbcomp2";

    protected static final String LB_GROUP_ID       = "lbgroup";
    protected static final String LB_GROUP_DEST     = "lbgroup_dest";

    protected static final String
    FOO_REP_ID = "IDL:thalesgroup.com/BasicDemo/Server:1.0";


    /**
     * Constructor.
     */
    public LBAssemblyTest() {
        this.out = System.out;
        this.name = "Assembly with Load Balancing tests";
    }


    /**
     * Runs the tests.
     */
    public void doTests() {
        // Runs the non-LB tests first
        super.doTests();

        println("Now executing the LB assembly tests");
        addNbOfRequestedTestOK(18);

        println("test LBComponentServer");
        // Create LBComponentServer 1.
        HostCollocation host1 = assembly.getHostCollocation(HOST_1_ID);
        LBComponentServer lbserver1 = null;
        try {
            lbserver1 = new LBComponentServer(LB_SERVER_1_ID,
                                              host1,
                                              LB_SERVER_1_DEST);
            succeed();
        } catch (CreateFailure e) {
            fail("Failed to create LBComponentServer 1 at destination '" +
                 LB_SERVER_1_DEST + "'.");
        }

        // Check its ID.
        if (LB_SERVER_1_ID.equals(lbserver1.getId())) {
            succeed();
        } else {
            fail("lbserver1 has wrong ID: " + lbserver1.getId());
        }

        // Check if it has been added to the assembly.
        if (lbserver1.equals(assembly.getComponentServer(LB_SERVER_1_ID))) {
            succeed();
        } else {
            fail("lbserver1 has not been added to assembly");
        }

        // Check if it has been added to the host.
        if (lbserver1.equals(host1.getComponentServer(LB_SERVER_1_ID))) {
            succeed();
        } else {
            fail("lbserver1 has not been added to host1");
        }

        // Check the LBComponentServer's parent assembly.
        if (assembly.equals(lbserver1.getParentAssembly())) {
            succeed();
        } else {
            fail("lbserver1 has a wrong parent assembly");
        }

        // Check the LBComponentServer's parent host.
        if (host1.equals(lbserver1.getParentHost())) {
            succeed();
        } else {
            fail("lbserver1 has a wrong parent host");
        }

        // Check its destination.
        if (LB_SERVER_1_DEST.equals(lbserver1.getProcessDestination())) {
            succeed();
        } else {
            fail("lbserver1 has a wrong destination: " +
                 lbserver1.getProcessDestination());
        }

        // Check if it's deactived.
        if (!lbserver1.isActivated()) {
            succeed();
        } else {
            fail("lbserver1 has been activated at construction");
        }

        // Create LBComponentServer 2.
        LBComponentServer lbserver2 = null;
        try {
            lbserver2 = new LBComponentServer(
                                LB_SERVER_2_ID,
                                assembly.getHostCollocation(HOST_2_ID),
                                LB_SERVER_2_DEST);
            succeed();
        } catch (CreateFailure e) {
            fail("Failed to create LBComponentServer 2 at destination '" +
                 LB_SERVER_2_DEST + "'.");
        }

        // Add containers.
        Container lbcontainer1 = new Container(
                LB_CONTAINER_1_ID,
                lbserver1,
                com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);

        Container lbcontainer2 = new Container(
                LB_CONTAINER_2_ID,
                lbserver2,
                com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);

        // Add homes.
        Home lbhome1 = new Home(LB_HOME_1_ID,
                                lbcontainer1,
                                HOME_UUID,
                                HOME_ENTRYPOINT);

        Home lbhome2 = new Home(LB_HOME_2_ID,
                                lbcontainer2,
                                HOME_UUID,
                                HOME_ENTRYPOINT);

        try {
            lbhome1.setHomeRepositoryId(HOME_REP_ID);
            lbhome1.setComponentRepositoryId(COMP_REP_ID);
            lbhome2.setHomeRepositoryId(HOME_REP_ID);
            // lbhome2 has an incompatible component RepositoryId.
            lbhome2.setComponentRepositoryId(FOO_REP_ID);
        } catch (CreateFailure e) {
            fail("CreateFailure: " + e.getMessage());
            e.printStackTrace(out);
        }

        // Add components.
        SingleComponent lbcomp1 = new SingleComponent(LB_COMP_1_ID, lbhome1);
        SingleComponent lbcomp2 = new SingleComponent(LB_COMP_2_ID, lbhome2);

        println("test LBComponentGroup");

        // Create a LBComponentGroup.
        LBComponentGroup lbgroup = new LBComponentGroup(LB_GROUP_ID,
                                                        assembly,
                                                        LB_GROUP_DEST);

        // Check the component group id.
        if (LB_GROUP_ID.equals(lbgroup.getId())) {
            succeed();
        } else {
            fail("lbgroup has wrong ID: " + lbgroup.getId());
        }

        // Check if it is deactivated.
        if (!lbgroup.isActivated()) {
            succeed();
        } else {
            fail("lbgroup has been activated at construction");
        }

        // Check the repository id of the group.
        if (lbgroup.getRepositoryId() == null) {
            succeed();
        } else {
            fail("lbgroup was set at construction: " +
                 lbgroup.getRepositoryId());
        }

        // Check the parent assembly.
        if (assembly.equals(lbgroup.getParentAssembly())) {
            succeed();
        } else {
            fail("lbgroup has a wrong parent assembly");
        }

        // Check if the group has been added to assembly.
        if (lbgroup.equals(assembly.getComponentGroup(LB_GROUP_ID))) {
            succeed();
        } else {
            fail("lbgroup has not been added to assembly");
        }

        // Check the destination of the group.
        if (LB_GROUP_DEST.equals(lbgroup.getDestination())) {
            succeed();
        } else {
            fail("lbgroup has a wrong destination: " +
                 lbgroup.getDestination());
        }

        // Testing the addition of components to the LB group.
        try {
            // Try to add a component whose parent is not a LBComponentServer.
            lbgroup.addComponent(assembly.getComponent(COMP_1_ID));
            fail("Successfully added a non LB component to lbgroup");
        } catch (CreateFailure e) {
            succeed();
        }

        try {
            // Add lbcomp1.
            lbgroup.addComponent(lbcomp1);
            succeed();
        } catch (CreateFailure e) {
            fail("Failed to add lbcomp1 to lbgroup: " + e.getMessage());
        }

        try {
            // Try to add a component with incompatible RepositoryId.
            lbgroup.addComponent(lbcomp2);
            fail("Successfully added lbcomp2 (incompatible RepositoryId) " +
                 "to lbgroup");
        } catch (CreateFailure e) {
            succeed();
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
            LBAssemblyTest test = new LBAssemblyTest();
            test.start();
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (orb != null) {
            orb.destroy();
        }
    }

    
}
