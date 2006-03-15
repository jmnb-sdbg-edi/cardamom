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

import com.thalesgroup.CdmwDeployment.PackageManager;

import org.omg.Components.CreateFailure;


public class FTAssemblyTest
    extends AssemblyTest
{
    
    protected static final String FT_SERVER_1_ID     = "ftserver1";
    protected static final String FT_SERVER_1_DEST   = "proc3@appli1";
    protected static final String FT_CONTAINER_1_ID  = "ftcontainer1";
    protected static final String FT_HOME_1_ID       = "fthome1";
    protected static final String FT_COMP_1_ID       = "ftcomp1";

    protected static final String FT_SERVER_2_ID     = "ftserver2";
    protected static final String FT_SERVER_2_DEST   = "proc4@appli1";
    protected static final String FT_CONTAINER_2_ID  = "ftcontainer2";
    protected static final String FT_HOME_2_ID       = "fthome2";
    protected static final String FT_COMP_2_ID       = "ftcomp2";

    protected static final String FT_SERVER_3_ID     = "ftserver3";
    protected static final String FT_SERVER_3_DEST   = "proc5@appli1";
    protected static final String FT_CONTAINER_3_ID  = "ftcontainer3";
    protected static final String FT_HOME_3_ID       = "fthome3";
    protected static final String FT_COMP_3_ID       = "ftcomp3";

    protected static final String FT_SERVER_4_ID     = "ftserver4";
    protected static final String FT_SERVER_4_DEST   = "proc6@appli1";
    protected static final String FT_CONTAINER_4_ID  = "ftcontainer4";
    protected static final String FT_HOME_4_ID       = "fthome4";
    protected static final String FT_COMP_4_ID       = "ftcomp4";

    protected static final String FT_GROUP_ID        = "ftgroup";
    protected static final String FT_GROUP_DEST      = "ftgroup_dest";
    
    protected static final String FOO_REP_ID         = "IDL:thalesgroup.com/BasicDemo/Server:1.0";

        
    public FTAssemblyTest() {
        this.out = System.out;
        this.name = "Assembly with Fault Tolerance tests";   
    }

    
    public void doTests()
    {
        // do common tests (without FT implications)
        super.doTests();
    
        addNbOfRequestedTestOK(32);
    
        println("test FTComponentServer");
        // create FTComponentServer 1 (primary)
        HostCollocation host1 = assembly.getHostCollocation(HOST_1_ID);
        FTComponentServer ftserver1 = null;
        try {
            ftserver1 = new FTComponentServer(FT_SERVER_1_ID, 
                                              host1, 
                                              FT_SERVER_1_DEST,
                                              true);
            succeed();
        } catch (CreateFailure e) {
            fail("Creation of FTComponentServer 1 with destination (" + FT_SERVER_1_DEST + ") fails");
        }
        // test ftserver1.getId()
        if (FT_SERVER_1_ID.equals(ftserver1.getId())) {
            succeed();
        } else {
            fail("ftserver1 has wrong ID: " + ftserver1.getId());
        }
        // test assembly.getComponentServer()
        if (ftserver1.equals(assembly.getComponentServer(FT_SERVER_1_ID))) {
            succeed();
        } else {
            fail("ftserver1 has not been added to assembly");
        }
        // test host1.getComponentServer()
        if (ftserver1.equals(host1.getComponentServer(FT_SERVER_1_ID))) {
            succeed();
        } else {
            fail("ftserver1 has not been added to host1");
        }
        // test ftserver1.getParentAssembly()
        if (assembly.equals(ftserver1.getParentAssembly())) {
            succeed();
        } else {
            fail("ftserver1 has no parent assembly");
        }
        // test ftserver1.getParentHost()
        if (host1.equals(ftserver1.getParentHost())) {
            succeed();
        } else {
            fail("ftserver1 has no parent host");
        }
        // test ftserver1.getProcessDestination()
        if (FT_SERVER_1_DEST.equals(ftserver1.getProcessDestination())) {
            succeed();
        } else {
            fail("ftserver1 has wrong destination: " + ftserver1.getProcessDestination());
        }
        // test ftserver1.isActivated()
        if (! ftserver1.isActivated()) {
            succeed();
        } else {
            fail("ftserver1 is activated at construction");
        }
        // test ftserver1.isPrimary()
        if (ftserver1.isPrimary()) {
           succeed();
        } else {
           fail("ftserver1 is not primary");
        }
        
        
        // Create another primary FTComponentServer
        FTComponentServer ftserver2 = null;
        try {
            ftserver2 = new FTComponentServer(FT_SERVER_2_ID, 
                                              assembly.getHostCollocation(HOST_2_ID), 
                                              FT_SERVER_2_DEST,
                                              true);
            succeed();
        } catch (CreateFailure e) {
            fail("Creation of FTComponentServer 2 with destination (" + FT_SERVER_2_DEST + ") fails");
        }
        
        // Create a backup FTComponentServer
        FTComponentServer ftserver3 = null;
        try {
            ftserver3 = new FTComponentServer(FT_SERVER_3_ID, 
                                              assembly.getHostCollocation(HOST_2_ID), 
                                              FT_SERVER_3_DEST,
                                              false);
            succeed();
        } catch (CreateFailure e) {
            fail("Creation of FTComponentServer 3 with destination (" + FT_SERVER_3_DEST + ") fails");
        }
        // test ftserver3.isPrimary()
        if (ftserver3.isPrimary()) {
           fail("ftserver3 is primary");
        } else {
           succeed();
        }
        
        // Create another backup FTComponentServer
        FTComponentServer ftserver4 = null;
        try {
            ftserver4 = new FTComponentServer(FT_SERVER_4_ID, 
                                              assembly.getHostCollocation(HOST_2_ID), 
                                              FT_SERVER_4_DEST,
                                              false);
            succeed();
        } catch (CreateFailure e) {
            fail("Creation of FTComponentServer 4 with destination (" + FT_SERVER_4_DEST + ") fails");
        }
        // test ftserver4.isPrimary()
        if (ftserver4.isPrimary()) {
           fail("ftserver4 is primary");
        } else {
           succeed();
        }
        
        // add containers
        Container ftcontainer1 = 
            new Container(FT_CONTAINER_1_ID, 
                          ftserver1, 
                          com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);
        Container ftcontainer2 = 
            new Container(FT_CONTAINER_2_ID, 
                          ftserver2, 
                          com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);
        Container ftcontainer3 = 
            new Container(FT_CONTAINER_3_ID, 
                          ftserver3, 
                          com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);
        Container ftcontainer4 = 
            new Container(FT_CONTAINER_4_ID, 
                          ftserver4, 
                          com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);
        
        // add homes
        Home fthome1 = new Home(FT_HOME_1_ID, ftcontainer1, HOME_UUID, HOME_ENTRYPOINT);
        Home fthome2 = new Home(FT_HOME_2_ID, ftcontainer2, HOME_UUID, HOME_ENTRYPOINT);
        Home fthome3 = new Home(FT_HOME_3_ID, ftcontainer3, HOME_UUID, HOME_ENTRYPOINT);
        Home fthome4 = new Home(FT_HOME_4_ID, ftcontainer4, HOME_UUID, HOME_ENTRYPOINT);
        try {
            fthome1.setHomeRepositoryId(HOME_REP_ID);
            fthome1.setComponentRepositoryId(COMP_REP_ID);
            fthome2.setHomeRepositoryId(HOME_REP_ID);
            fthome2.setComponentRepositoryId(COMP_REP_ID);
            fthome3.setHomeRepositoryId(HOME_REP_ID);
            // fthome 3 has no compatible component RepositoryId
            fthome3.setComponentRepositoryId(FOO_REP_ID);
            fthome4.setHomeRepositoryId(HOME_REP_ID);
            fthome4.setComponentRepositoryId(COMP_REP_ID);
            
        } catch (CreateFailure e) {
            fail("CreateFailure: " + e.getMessage());
            e.printStackTrace(out);
        }
        
        
        // add components 
        SingleComponent ftcomp1 = new SingleComponent(FT_COMP_1_ID, fthome1);
        SingleComponent ftcomp2 = new SingleComponent(FT_COMP_2_ID, fthome2);
        SingleComponent ftcomp3 = new SingleComponent(FT_COMP_3_ID, fthome3);
        SingleComponent ftcomp4 = new SingleComponent(FT_COMP_4_ID, fthome4);
        
        println("test FTComponentGroup");
        // create FTComponentGroup
        FTComponentGroup group = new FTComponentGroup(FT_GROUP_ID,
                                                      assembly,
                                                      FT_GROUP_DEST);
        // test group.getId()
        if (FT_GROUP_ID.equals(group.getId())) {
            succeed();
        } else {
            fail("group has wrong ID: " + group.getId());
        }
        // test group.isActivated()
        if (! group.isActivated()) {
            succeed();
        } else {
            fail("group is activated at construction");
        }
        // test group.getRepositoryId()
        if (group.getRepositoryId() == null) {
            succeed();
        } else {
            fail("group is set at construction: " + group.getRepositoryId());
        }
        // test group.getParentAssembly()
        if (assembly.equals(group.getParentAssembly())) {
            succeed();
        } else {
            fail("group has no parent assembly");
        }
        // test assembly.getComponentGroup()
        if (group.equals(assembly.getComponentGroup(FT_GROUP_ID))) {
            succeed();
        } else {
            fail("group has not been added to assembly");
        }
        // test group.getDestination()
        if (FT_GROUP_DEST.equals(group.getDestination())) {
            succeed();
        } else {
            fail("group has wrong destination: " + group.getDestination());
        }
        // test group.getPrimary()
        if (group.getPrimary() == null) {
            succeed();
        } else {
            fail("group's primary is set at contruction: " + group.getPrimary().getId());
        }
        
        // test group.addComponent()
        try {
            // try to add component with no FT parent ComponentServer
            group.addComponent(assembly.getComponent(COMP_1_ID));
            fail("Succeed to add a non-FT component to group");
        } catch (CreateFailure e) {
            succeed();
        }
        try {
            // add ftcomp1
            group.addComponent(ftcomp1);
            succeed();
        } catch (CreateFailure e) {
            fail("Failed to add ftcomp1 to group: " + e.getMessage());
        }
        // check that home of added component is declared as having FT components
        if (fthome1.hasFTComponents()) {
            succeed();
        } else {
            fail("Home " + fthome1.getId() + " declares to not have FT Components");
        }
        // test group.getPrimary()
        if (ftcomp1.equals(group.getPrimary())) {
            succeed();
        } else {
            if (group.getPrimary() == null) 
                fail("group has no primary");
            else
                fail("group has wrong primary: " + group.getPrimary().getId());
        }
        try {
            // try to add a second primary to the same group
            group.addComponent(ftcomp2);
            fail("Succeed to add 2 primaries to FT group");
        } catch (CreateFailure e) {
            succeed();
        }
        // check that home of not added component is not declared as having FT components
        if (! fthome2.hasFTComponents()) {
            succeed();
        } else {
            fail("Home " + fthome2.getId() + " declares to have FT Components");
        }
        try {
            // try to add a component with not compatible RepositoryId
            group.addComponent(ftcomp3);
            fail("Succeed to add ftcomp3 with wrong RepositoryId to group");
        } catch (CreateFailure e) {
            succeed();
        }
        // check that home of not added component is not declared as having FT components
        if (! fthome3.hasFTComponents()) {
            succeed();
        } else {
            fail("Home " + fthome3.getId() + " declares to have FT Components");
        }
        try {
            // add correct member
            group.addComponent(ftcomp4);
            succeed();
        } catch (CreateFailure e) {
            fail("Failed to add ftcomp4 to group: " + e.getMessage());
        }
        // check that home of added component is declared as having FT components
        if (fthome4.hasFTComponents()) {
            succeed();
        } else {
            fail("Home " + fthome4.getId() + " declares to not have FT Components");
        }
        // test group.getPrimary()
        if (ftcomp1.equals(group.getPrimary())) {
            succeed();
        } else {
            if (group.getPrimary() == null) 
                fail("group has no primary");
            else
                fail("group has wrong primary: " + group.getPrimary().getId());
        }
    }
 


    public static void main(String[] args) {

        try {
            // init ORB
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = ORBSupport.ORBInit(args, orbStrategies);

            rootPOA = org.omg.PortableServer.POAHelper.narrow(
                orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();

            // register XML value factories
            XMLHelper.registerValueFactories(orb);

            // create XML Parser object
            String[] parserArgs = {"--validate",
                                   "--doctypedir=" + DTD_DIR};
            parser = XMLParserImpl.init(orb, rootPOA, parserArgs);

            // create PackageManagerStub object
            managerStub = new PackageManagerStub();
            manager = managerStub._this(orb);

            // register NamingService stub
            registerNamingServiceStub(orb);

            // register HomeFinder stub
            registerHomeFinderStub(orb);

            // init AssemblyFactoryImpl
            AssemblyFactoryImpl.init(orb, rootPOA, rootPOA, null, parser, manager);

            // run test
            FTAssemblyTest test = new FTAssemblyTest();
            test.start();

        } catch (Exception e) {
            e.printStackTrace();
        }

        // destroy ORB
        if (orb != null) {
            orb.destroy();
        }
    }


   
}

