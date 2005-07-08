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

package cdmw.ccm.assembly;

import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ORBSupport;

import cdmw.xmlutils.XMLParserImpl;
import cdmw.xmlutils.XMLHelper;

import com.thalesgroup.CdmwDeployment.PackageManager;

import org.omg.Components.CreateFailure;


public class AssemblyTest
    extends cdmw.testutils.Testable
{
    protected static final int POA_THREAD_POOL_SIZE = 10;
    protected static final String DTD_DIR = ".";

    protected static final String HOST_1_ID       = "host1"; 
    protected static final String HOST_1_DEST     = "luke"; 
    protected static final String SERVER_1_ID     = "server1";
    protected static final String SERVER_1_DEST   = "proc1@appli1";
    protected static final String CONTAINER_1_ID  = "container1";
    protected static final String HOME_1_ID       = "home1";
    protected static final String COMP_1_ID       = "comp1";
    
    protected static final String HOST_2_ID     = "host2";
    protected static final String HOST_2_DEST   = "leia"; 
    protected static final String SERVER_2_ID     = "server2";
    protected static final String SERVER_2_DEST   = "proc2@appli1";
    protected static final String CONTAINER_2_ID  = "container2";
    protected static final String HOME_2_ID       = "home2";
    protected static final String COMP_2_ID       = "comp2";

    protected static final String HOME_UUID       = "Home_uuid"; 
    protected static final String HOME_ENTRYPOINT = "Home_entrypoint";
    protected static final String HOME_REP_ID     = "IDL:thalesgroup.com/BasicDemo/ClientHome:1.0";
    protected static final String COMP_REP_ID     = "IDL:thalesgroup.com/BasicDemo/Client:1.0";
    
    
    protected static final String COMP1_USES_ID      = "test_uses"; 
    protected static final String COMP2_FACET_ID     = "test_facet"; 
    protected static final String COMP1_CONSUMER_ID  = "test_consumer"; 
    protected static final String COMP2_EMITS_ID     = "test_emits"; 
    protected static final String COMP2_PUBLISHES_ID = "test_publishes"; 

    protected static final String FINDBY_NAME     = "cdmw/components/test_comp";
    protected static final String FINDBY_STRING   = "corbaloc::localhost:1234/TestObject";
    protected static final String FINDBY_HOME     = "test_home";
    
    protected static final String CONNECTION1_ID  = "connection1";
    protected static final String CONNECTION2_ID  = "connection2";
    protected static final String CONNECTION3_ID  = "connection3";
    protected static final String CONNECTION4_ID  = "connection4";
    protected static final String CONNECTION5_ID  = "connection5";
    protected static final String CONNECTION6_ID  = "connection6";
    protected static final String CONNECTION7_ID  = "connection7";
    
    
    protected static org.omg.CORBA.ORB orb = null;
    protected static org.omg.PortableServer.POA rootPOA = null;
    protected static com.thalesgroup.CdmwXML.XMLParser parser = null;
    protected static PackageManagerStub managerStub = null;
    protected static PackageManager manager = null;

    protected AssemblyImpl assembly;



    public AssemblyTest()
    {
        this.out = System.out;
        this.name = "Assembly tests";
    }

    
    public void doTests()
    {       
        // set number of requested successfull tests
        setNbOfRequestedTestOK(70);
        
        println("create new empty assembly");
        // create new empty assembly
        assembly = new AssemblyImpl(rootPOA);
        // test assembly.isActivated()
        if (org.omg.Components.Deployment.AssemblyState.INACTIVE.equals(assembly.get_state())) {
            succeed();
        } else {
            fail("assembly has wrong state at construction");
        }
        
        println("test HostCollocation");
        // add host1
        HostCollocation host1 = new HostCollocation(HOST_1_ID, assembly, HOST_1_DEST);
        // test host1.getId()
        if (HOST_1_ID.equals(host1.getId())) {
            succeed();
        } else {
            fail("host1 has wrong ID: " + host1.getId());
        }
        // test assembly.getHostCollocation()
        if (host1.equals(assembly.getHostCollocation(HOST_1_ID))) {
            succeed();
        } else {
            fail("host1 has not been added to assembly");
        }
        // test host1.getParentAssembly()
        if (assembly.equals(host1.getParentAssembly())) {
            succeed();
        } else {
            fail("host1 has no parent assembly");
        }
        // test host1.getDestination()
        if (HOST_1_DEST.equals(host1.getDestination())) {
            succeed();
        } else {
            fail("host1 has wrong destination: " + host1.getDestination());
        }
        // test host1.isActivated()
        if (! host1.isActivated()) {
            succeed();
        } else {
            fail("host1 is activated at construction");
        }

        println("test ComponentServer");
        // add server1
        ComponentServer server1 = null;
        try {
            // test with non conform destination
            server1 = new ComponentServer("foo", host1, "fooDest");
            fail("Creation of ComponentServer with non conform destination succeed !!");
        } catch (CreateFailure e) {
            succeed();
        }
        try {
            // test with non conform destination
            server1 = new ComponentServer("foo", host1, "@fooDest");
            fail("Creation of ComponentServer with non conform destination succeed !!");
        } catch (CreateFailure e) {
            succeed();
        }
        try {
            // test with non conform destination
            server1 = new ComponentServer("foo", host1, "fooDest@");
            fail("Creation of ComponentServer with non conform destination succeed !!");
        } catch (CreateFailure e) {
            succeed();
        }
        try {
            // test with conform destination
            server1 = new ComponentServer(SERVER_1_ID, host1, SERVER_1_DEST);
            succeed();
        } catch (CreateFailure e) {
            fail("Creation of ComponentServer with destination (" + SERVER_1_DEST + ") fails");
        }
        // test server1.getId()
        if (SERVER_1_ID.equals(server1.getId())) {
            succeed();
        } else {
            fail("server1 has wrong ID: " + server1.getId());
        }
        // test assembly.getComponentServer()
        if (server1.equals(assembly.getComponentServer(SERVER_1_ID))) {
            succeed();
        } else {
            fail("server1 has not been added to assembly");
        }
        // test host1.getComponentServer()
        if (server1.equals(host1.getComponentServer(SERVER_1_ID))) {
            succeed();
        } else {
            fail("server1 has not been added to host1");
        }
        // test server1.getParentAssembly()
        if (assembly.equals(server1.getParentAssembly())) {
            succeed();
        } else {
            fail("server1 has no parent assembly");
        }
        // test server1.getParentHost()
        if (host1.equals(server1.getParentHost())) {
            succeed();
        } else {
            fail("server1 has no parent host");
        }
        // test server1.getProcessDestination()
        if (SERVER_1_DEST.equals(server1.getProcessDestination())) {
            succeed();
        } else {
            fail("server1 has wrong destination: " + server1.getProcessDestination());
        }
        // test server1.isActivated()
        if (! server1.isActivated()) {
            succeed();
        } else {
            fail("server1 is activated at construction");
        }
        
        println("test Container");
        // add container1
        Container container1 = 
            new Container(CONTAINER_1_ID, 
                          server1, 
                          com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);
        // test container1.getId()
        if (CONTAINER_1_ID.equals(container1.getId())) {
            succeed();
        } else {
            fail("container1 has wrong ID: " + server1.getId());
        }
        // test assembly.getContainer()
        if (container1.equals(assembly.getContainer(CONTAINER_1_ID))) {
            succeed();
        } else {
            fail("container1 has not been added to assembly");
        }
        // test server1.getContainer()
        if (container1.equals(server1.getContainer(CONTAINER_1_ID))) {
            succeed();
        } else {
            fail("container1 has not been added to server1");
        }
        // test container1.getParentAssembly()
        if (assembly.equals(container1.getParentAssembly())) {
            succeed();
        } else {
            fail("container1 has no parent assembly");
        }
        // test container1.getParentComponentServer()
        if (server1.equals(container1.getParentComponentServer())) {
            succeed();
        } else {
            fail("container1 has no parent server");
        }
        // test container1.getComponentKind()
        if (com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION.equals(container1.getComponentKind())) {
            succeed();
        } else {
            fail("container1 has wrong ComponentKind: " + container1.getComponentKind().toString());
        }
        // test container1.isActivated()
        if (! container1.isActivated()) {
            succeed();
        } else {
            fail("container1 is activated at construction");
        }
                                  
        println("test Home");
        // add home1
        Home home1 = new Home(HOME_1_ID, container1, HOME_UUID, HOME_ENTRYPOINT);
        // test home1.getId()
        if (HOME_1_ID.equals(home1.getId())) {
            succeed();
        } else {
            fail("home1 has wrong ID: " + home1.getId());
        }
        // test assembly.getHome()
        if (home1.equals(assembly.getHome(HOME_1_ID))) {
            succeed();
        } else {
            fail("home1 has not been added to assembly");
        }
        // test container1.getHome()
        if (home1.equals(container1.getHome(HOME_1_ID))) {
            succeed();
        } else {
            fail("home1 has not been added to container1");
        }
        // test home1.getParentAssembly()
        if (assembly.equals(home1.getParentAssembly())) {
            succeed();
        } else {
            fail("home1 has no parent assembly");
        }
        // test home1.getParentContainer()
        if (container1.equals(home1.getParentContainer())) {
            succeed();
        } else {
            fail("home1 has no parent container");
        }
        // test home1.getUUID()
        if (HOME_UUID.equals(home1.getUUID())) {
            succeed();
        } else {
            fail("home1 has wrong UUID: " + home1.getUUID());
        }
        // test home1.getEntryPoint()
        if (HOME_ENTRYPOINT.equals(home1.getEntryPoint())) {
            succeed();
        } else {
            fail("home1 has wrong entrpypoint: " + home1.getEntryPoint());
        }
        // test home1.isActivated()
        if (! home1.isActivated()) {
            succeed();
        } else {
            fail("home1 is activated at construction");
        }
        // test home1.setHomeRepositoryId()
        try {
            home1.setHomeRepositoryId(COMP_REP_ID);
            fail("Succeed to set a component RepositoryId instead of a home RepositoryId");
        } catch (CreateFailure e) {
            succeed();
        }
        try {
            home1.setHomeRepositoryId(HOME_REP_ID);
            succeed();
        } catch (CreateFailure e) {
            fail("Failed to set home RepositoryId: " + e.getMessage());
        }
        try {
            home1.setHomeRepositoryId(HOME_REP_ID);
            fail("Succeed to set a home RepositoryId twice");
        } catch (CreateFailure e) {
            succeed();
        }
        // test home1.getHomeRepositoryId()
        if (HOME_REP_ID.equals(home1.getHomeRepositoryId())) {
            succeed();
        } else {
            fail("home1 has wrong home RepositoryId: " + home1.getHomeRepositoryId());
        }
        // test home1.setComponentRepositoryId
        try {
            home1.setComponentRepositoryId(HOME_REP_ID);
            fail("Succeed to set a home RepositoryId instead of a component RepositoryId");
        } catch (CreateFailure e) {
            succeed();
        }
        try {
            home1.setComponentRepositoryId(COMP_REP_ID);
            succeed();
        } catch (CreateFailure e) {
            fail("Failed to set component RepositoryId: " + e.getMessage());
        }
        try {
            home1.setComponentRepositoryId(COMP_REP_ID);
            fail("Succeed to set a component RepositoryId twice");
        } catch (CreateFailure e) {
            succeed();
        }
        // test home1.getComponentRepositoryId()
        if (COMP_REP_ID.equals(home1.getComponentRepositoryId())) {
            succeed();
        } else {
            fail("home1 has wrong component RepositoryId: " + home1.getComponentRepositoryId());
        }
        
                                  
        println("test SingleComponent");
        // add component
        SingleComponent comp1 = new SingleComponent(COMP_1_ID, home1);
        // test comp1.getId()
        if (COMP_1_ID.equals(comp1.getId())) {
            succeed();
        } else {
            fail("comp1 has wrong ID: " + comp1.getId());
        }
        // test assembly.getHome()
        if (comp1.equals(assembly.getComponent(COMP_1_ID))) {
            succeed();
        } else {
            fail("comp1 has not been added to assembly");
        }
        // test home1.getComponent()
        if (comp1.equals(home1.getComponent(COMP_1_ID))) {
            succeed();
        } else {
            fail("comp1 has not been added to home1");
        }
        // test comp1.getParentAssembly()
        if (assembly.equals(comp1.getParentAssembly())) {
            succeed();
        } else {
            fail("comp1 has no parent assembly");
        }
        // test comp1.getParentHome()
        if (home1.equals(comp1.getParentHome())) {
            succeed();
        } else {
            fail("comp1 has no parent home");
        }
        // test comp1.isActivated()
        if (! comp1.isActivated()) {
            succeed();
        } else {
            fail("comp1 is activated at construction");
        }
        // test comp1.getRepositoryId()
        if (COMP_REP_ID.equals(comp1.getRepositoryId())) {
            succeed();
        } else {
            fail("comp1 has wrong component RepositoryId: " + comp1.getRepositoryId());
        }
        
        
        // add another component for connections tests
        HostCollocation host2 = new HostCollocation(HOST_2_ID, assembly, HOST_2_DEST);
        ComponentServer server2 = null;
        try {
            server2 = new ComponentServer(SERVER_2_ID, host2, SERVER_2_DEST);
        } catch (CreateFailure e) {
            fail("Creation of ComponentServer 2 with non conform destination succeed !!");
        }
        Container container2 = 
            new Container(CONTAINER_2_ID, 
                          server2, 
                          com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);
        Home home2 = new Home(HOME_2_ID, container2, HOME_UUID, HOME_ENTRYPOINT);
        SingleComponent comp2 = new SingleComponent(COMP_2_ID, home2);
        
        
        println("test ComponentInstantiationRef");
        // create ComponentInstantiationRef
        ComponentInstantiationRef comp1_ref = new ComponentInstantiationRef(comp1);
        // test comp1_ref.getComponent()
        if (comp1.equals(comp1_ref.getComponent())) {
            succeed();
        } else {
            fail("comp1_ref doesn't reference comp1");
        }

        println("test FacetRef");
        // create FacetRef
        ComponentInstantiationRef comp2_ref = new ComponentInstantiationRef(comp2);
        FacetRef facet_comp2_ref = null;
        try {
            facet_comp2_ref = new FacetRef(COMP2_FACET_ID, comp2_ref);
        } catch (cdmw.common.BadParameterException e) {
            fail("Failed to create FacetRef");
        }
        // test facet_comp2_ref.getProvidesId()
        if (COMP2_FACET_ID.equals(facet_comp2_ref.getProvidesId())) {
            succeed();
        } else {
            fail("facet_comp2_ref's providesId is " + facet_comp2_ref.getProvidesId() +
                 "instead of " + COMP2_FACET_ID);
        }
        // test facet_comp2_ref.getComponentRef()
        if (comp2_ref.equals(facet_comp2_ref.getComponentRef())) {
            succeed();
        } else {
            fail("facet_comp2_ref doesn't reference comp2");
        }
        
        println("test HomePlacementRef");
        // create HomePlacementRef
        HomePlacementRef home2_ref = new HomePlacementRef(home2);
        // test home2_ref.getHome()
        if (home2.equals(home2_ref.getHome())) {
            succeed();
        } else {
            fail("home2_ref doesn't reference home2");
        }
        
        println("test FindbyNamingService");
        // create FindbyNamingService
        org.omg.CORBA.Object namingObj = null;
        try {
            namingObj = orb.resolve_initial_references("NameService");
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            fail("resolve_initial_references(NamingService) failed: " + e.getMessage());
        }
        FindbyNamingService findbyName_ref = 
            new FindbyNamingService(org.omg.CosNaming.NamingContextHelper.narrow(namingObj),
                                    FINDBY_NAME);
        // test findbyName_ref.getName()
        if (FINDBY_NAME.equals(findbyName_ref.getName())) {
            succeed();
        } else {
            fail("findbyName_ref has for name " + findbyName_ref.getName() +
                 " instead of " + FINDBY_NAME);
        }
            
        println("test FindbyStringifiedObject");
        // create FindbyStringifiedObject    
        FindbyStringifiedObject findbyString_ref =
            new FindbyStringifiedObject(FINDBY_STRING,
                                        orb);
        // test findbyString_ref.getStringifiedObject()
        if (FINDBY_STRING.equals(findbyString_ref.getStringifiedObject())) {
            succeed();
        } else {
            fail("findbyString_ref has for string " + findbyString_ref.getStringifiedObject() +
                 " instead of " + FINDBY_STRING);
        }
        
        println("test FindbyHomeFinder");
        // create FindbyHomeFinder
        org.omg.CORBA.Object homeFinderObj = null;
        try {
            homeFinderObj = orb.resolve_initial_references("HomeFinder");
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            fail("resolve_initial_references(HomeFinder) failed: " + e.getMessage());
        }
        FindbyHomeFinder findbyHome_ref =
            new FindbyHomeFinder(org.omg.Components.HomeFinderHelper.narrow(homeFinderObj),
                                 FINDBY_HOME);
        // test findbyHome_ref.getHomeName()
        if (FINDBY_HOME.equals(findbyHome_ref.getHomeName())) {
            succeed();
        } else {
            fail("findbyHome_ref has for home name " + findbyHome_ref.getHomeName() +
                 " instead of " + FINDBY_HOME);
        }
        
        
        
        println("test InterfaceConnection");
        // create InterfaceConnection with facet reference
        InterfaceConnection connection1 = 
            new InterfaceConnection(CONNECTION1_ID,
                                    COMP1_USES_ID,
                                    comp1_ref,
                                    facet_comp2_ref);
        // test connection1.getId()
        if (CONNECTION1_ID.equals(connection1.getId())) {
            succeed();
        } else {
            fail("connection1 has wrong ID: " + connection1.getId());
        }
        // test connection1.isActivated()
        if (! connection1.isActivated()) {
            succeed();
        } else {
            fail("connection1 is activated at construction");
        }
        
        // add connection to assembly
        assembly.addConnection(connection1);
        // test assembly.getConnection()
        if (connection1.equals(assembly.getConnection(CONNECTION1_ID))) {
            succeed();
        } else {
            fail("connection1 has not been added to assembly");
        }
        
        // create InterfaceConnection with home reference
        InterfaceConnection connection2 = 
            new InterfaceConnection(CONNECTION2_ID,
                                    COMP1_USES_ID,
                                    comp1_ref,
                                    home2_ref);
        // test connection2.getId()
        if (CONNECTION2_ID.equals(connection2.getId())) {
            succeed();
        } else {
            fail("connection2 has wrong ID: " + connection2.getId());
        }
        // test connection2.isActivated()
        if (! connection2.isActivated()) {
            succeed();
        } else {
            fail("connection2 is activated at construction");
        }
        
        // create InterfaceConnection with stringified object ref
        InterfaceConnection connection3 = 
            new InterfaceConnection(CONNECTION3_ID,
                                    COMP1_USES_ID,
                                    comp1_ref,
                                    findbyString_ref);
        // test connection3.getId()
        if (CONNECTION3_ID.equals(connection3.getId())) {
            succeed();
        } else {
            fail("connection3 has wrong ID: " + connection2.getId());
        }
        // test connection3.isActivated()
        if (! connection3.isActivated()) {
            succeed();
        } else {
            fail("connection3 is activated at construction");
        }
        
        // create InterfaceConnection with name reference
        InterfaceConnection connection4 = 
            new InterfaceConnection(CONNECTION4_ID,
                                    COMP1_USES_ID,
                                    comp1_ref,
                                    findbyName_ref);
        // test connection4.getId()
        if (CONNECTION4_ID.equals(connection4.getId())) {
            succeed();
        } else {
            fail("connection4 has wrong ID: " + connection4.getId());
        }
        // test connection4.isActivated()
        if (! connection4.isActivated()) {
            succeed();
        } else {
            fail("connection4 is activated at construction");
        }
        
        // create InterfaceConnection with home finder reference
        InterfaceConnection connection5 = 
            new InterfaceConnection(CONNECTION5_ID,
                                    COMP1_USES_ID,
                                    comp1_ref,
                                    findbyHome_ref);
        // test connection5.getId()
        if (CONNECTION5_ID.equals(connection5.getId())) {
            succeed();
        } else {
            fail("connection5 has wrong ID: " + connection5.getId());
        }
        // test connection5.isActivated()
        if (! connection5.isActivated()) {
            succeed();
        } else {
            fail("connection5 is activated at construction");
        }
        
        
        println("test EmitsConnection");
        // create EmitsConnection
        EmitsConnection connection6 =
            new EmitsConnection(CONNECTION6_ID,
                                COMP1_CONSUMER_ID,
                                comp1_ref,
                                COMP2_EMITS_ID,
                                comp2_ref);
        // test connection6.getId()
        if (CONNECTION6_ID.equals(connection6.getId())) {
            succeed();
        } else {
            fail("connection6 has wrong ID: " + connection6.getId());
        }
        // test connection6.isActivated()
        if (! connection6.isActivated()) {
            succeed();
        } else {
            fail("connection6 is activated at construction");
        }

        // add connection to assembly
        assembly.addConnection(connection6);
        // test assembly.getConnection()
        if (connection6.equals(assembly.getConnection(CONNECTION6_ID))) {
            succeed();
        } else {
            fail("connection6 has not been added to assembly");
        }

        println("test PublishesConnection");
        // create PublishesConnection
        PublishesConnection connection7 =
            new PublishesConnection(CONNECTION7_ID,
                                    COMP1_CONSUMER_ID,
                                    comp1_ref,
                                    COMP2_PUBLISHES_ID,
                                    comp2_ref);
        // test connection7.getId()
        if (CONNECTION7_ID.equals(connection7.getId())) {
            succeed();
        } else {
            fail("connection7 has wrong ID: " + connection7.getId());
        }
        // test connection7.isActivated()
        if (! connection7.isActivated()) {
            succeed();
        } else {
            fail("connection7 is activated at construction");
        }

        // add connection to assembly
        assembly.addConnection(connection7);
        
    } 


    
    public static void registerNamingServiceStub(org.omg.CORBA.ORB orb) {
        NamingContextExtStub stub = new NamingContextExtStub();
        org.omg.CosNaming.NamingContextExt naming =
            stub._this(orb);
        ORBSupport.registerInitialReference(orb, "NameService", naming);
    }

    public static void registerHomeFinderStub(org.omg.CORBA.ORB orb) {
        HomeFinderStub stub = new HomeFinderStub();
        org.omg.Components.HomeFinder finder =
            stub._this(orb);
        ORBSupport.registerInitialReference(orb, "HomeFinder", finder);
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
            AssemblyTest test = new AssemblyTest();
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




