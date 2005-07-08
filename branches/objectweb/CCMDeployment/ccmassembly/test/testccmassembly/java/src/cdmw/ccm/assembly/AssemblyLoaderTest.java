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
import cdmw.ccm.deployment.CadReader;

import com.thalesgroup.CdmwDeployment.PackageManager;
import com.thalesgroup.CdmwDeployment._AssemblyPackage;
import com.thalesgroup.CdmwDeployment._ComponentPackage;
import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ServantLifetimeValue;
import com.thalesgroup.CdmwDeployment.ThreadingPolicyValue;

import org.omg.Components.CreateFailure;


public class AssemblyLoaderTest
    extends cdmw.testutils.Testable
{

    protected static final int POA_THREAD_POOL_SIZE = 10;
    protected static final String DTD_DIR = ".";

    protected static final String HOST_ID_1 = "MyHost";
    protected static final String SRV_PROC_ID = "ServerProcess";
    protected static final String SRV_CONT_ID = "ServerHome_container";
    protected static final String SRV_HOME_ID = "ServerHome"; 
    protected static final String SRV_COMP_ID_1 = "Server1";
    protected static final String SRV_COMP_ID_2 = "Server2";
    protected static final String CLT_CONT_ID_1 = "ClientHome1_container";
    protected static final String CLT_HOME_ID_1 = "ClientHome1"; 
    protected static final String CLT_COMP_ID_1 = "Client1";
    
    protected static final String CLT_PROC_ID_1 = "ClientProcess1"; 
    protected static final String CLT_CONT_ID_2 = "ClientHome2_container";
    protected static final String CLT_HOME_ID_2 = "ClientHome2"; 
    protected static final String CLT_COMP_ID_2 = "Client2";
    protected static final String CLT_COMP_ID_3 = "Client3";

    protected static final String HOST_ID_2 = "MyHost2";
    protected static final String CLT_PROC_ID_2 = "ClientProcess2"; 
    protected static final String CLT_CONT_ID_3 = "ClientHome3_container";
    protected static final String CLT_HOME_ID_3 = "ClientHome3"; 
    protected static final String CLT_COMP_ID_4 = "Client4";
    
    protected static final String SERVER_NAME = "Server";
    protected static final String SERVER_VERSION = "1,0,0,0";
    protected static final String SERVER_FILE_ID = "Server";
    protected static final String SERVER_REP_ID = 
        "IDL:thalesgroup.com/BasicDemo/Server:1.0";
    protected static final String SERVER_HOME_REP_ID = 
        "IDL:thalesgroup.com/BasicDemo/ServerHome:1.0";
    protected static final String SERVER_ENTRYPOINT = "createServerHome";
    protected static final String SERVER_UUID =
        "DCE:700dc518-0110-11ce-ac8f-0800090b5d3e";
    protected static final String SERVER_HOME_SERVANT_CLASSNAME = "ServerHome_impl";

    protected static final String CLIENT_NAME = "Client";
    protected static final String CLIENT_VERSION = "1,0,0,0";
    protected static final String CLIENT_FILE_ID = "Client";
    protected static final String CLIENT_REP_ID = 
        "IDL:thalesgroup.com/BasicDemo/Client:1.0";
    protected static final String CLIENT_HOME_REP_ID = 
        "IDL:thalesgroup.com/BasicDemo/ClientHome:1.0";
    protected static final String CLIENT_ENTRYPOINT = "createClientHome";
    protected static final String CLIENT_UUID =
        "DCE:700dc518-0110-11ce-ac8f-0800090531ac";
    protected static final String CLIENT_HOME_SERVANT_CLASSNAME = "ClientHome_impl";



    protected static final String SERVER_PROCESS_DEST =
        "ServerProcess@BasicDemo";
    protected static final String CLIENT_PROCESS1_DEST =
        "ClientProcess1@BasicDemo";
    protected static final String CLIENT_PROCESS2_DEST =
        "ClientProcess2@BasicDemo";

    protected static final String DESTINATION1 =
        "localhost";
    protected static final String DESTINATION2 =
        "remotehost";

    private static final String ASSEMBLY_DIR = "AssemblyLoaderTest";

    protected static org.omg.CORBA.ORB orb = null;
    protected static org.omg.PortableServer.POA rootPOA = null;
    protected static com.thalesgroup.CdmwXML.XMLParser parser = null;
    protected static PackageManagerStub managerStub = null;
    protected static PackageManager manager = null;

    
    protected String assemblyDirectory;
    protected AssemblyImpl assembly;

    
    public AssemblyLoaderTest()
    {
        this.out = System.out;
        this.name = "AssemblyLoader tests";
        this.assemblyDirectory = ASSEMBLY_DIR;
        this.assembly = null;
    }

    public void doTests()
    {
        // set number of requested successfull tests
        setNbOfRequestedTestOK(76);
        
        // create ComponentPackageStub for Server
        ComponentPackageStub componentPackStubServer =
            new ComponentPackageStub(SERVER_NAME, SERVER_VERSION);
        _ComponentPackage componentPackServer =
            componentPackStubServer._this(orb);

        // create ComponentPackageStub for Client
        ComponentPackageStub componentPackStubClient =
            new ComponentPackageStub(CLIENT_NAME, CLIENT_VERSION);
        _ComponentPackage componentPackClient =
            componentPackStubClient._this(orb);

        // create AssemblyPackageStub
        AssemblyPackageStub assemblyPackStub = new AssemblyPackageStub(assemblyDirectory);
        _AssemblyPackage assemblyPack = assemblyPackStub._this(orb);

        // add ComponentPackages to AssemblyPackage
        assemblyPackStub.addComponentPackage(SERVER_FILE_ID, componentPackServer);
        assemblyPackStub.addComponentPackage(CLIENT_FILE_ID, componentPackClient);

        // add AssemblyPackage to PackageManager
        managerStub.addAssemblyPackage(assemblyPack);

        // create CadReader
        CadReader cadReader = null;
        try {
            cadReader = new CadReader(parser, assemblyPack.get_CAD_location());
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwDeployment.LocationCreationException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException e) {
            fail("Error while parsing cad file : " + e.reason);
            return;
        }

        // create AssemblyLoader
        AssemblyLoader assemblyLoader = new AssemblyLoader(rootPOA);


        //
        // test load()
        //
        println("Test load()");

        try {
            assembly = assemblyLoader.load(cadReader);

        } catch (CreateFailure e) {
            fail("Error while loading cad : " + e.getMessage());
            return;
        }

        //
        // test that all elements have been successfully loaded.
        //
        println("Test that all elements have been successfully loaded.");
        
        HostCollocation host1 = assembly.getHostCollocation(HOST_ID_1);
        if (host1 != null) {
            succeed();
        } else {
            fail("HostCollocation " + HOST_ID_1 + " not loaded.");
        }
        
        ComponentServer serverProc = host1.getComponentServer(SRV_PROC_ID);
        if (serverProc != null) {
            succeed();
        } else {
            fail("ComponentServer " + SRV_PROC_ID + " is not child of HostCollocation " + host1.getId());
        }
        
        Container serverCont = serverProc.getContainer(SRV_CONT_ID);
        if (serverCont != null) {
            succeed();
        } else {
            fail("Container " + SRV_CONT_ID + " is not child of ComponentServer " + serverProc.getId());
        }
        
        Home serverHome = serverCont.getHome(SRV_HOME_ID);
        if (serverHome != null) {
            succeed();
        } else {
            fail("Home " + SRV_HOME_ID + " is not child of Container " + serverCont.getId());
        }
        
        Component server1 = serverHome.getComponent(SRV_COMP_ID_1);
        if (server1 != null) {
            succeed();
        } else {
            fail("Component " + SRV_COMP_ID_1 + " is not child of Home " + serverHome.getId());
        }
        
        Component server2 = serverHome.getComponent(SRV_COMP_ID_2);
        if (server2 != null) {
            succeed();
        } else {
            fail("Component " + SRV_COMP_ID_2 + " is not child of Home " + serverHome.getId());
        }
        
        Container clientCont1 = serverProc.getContainer(CLT_CONT_ID_1);
        if (clientCont1 != null) {
            succeed();
        } else {
            fail("Container " + CLT_CONT_ID_1 + " is not child of ComponentServer " + serverProc.getId());
        }
        
        Home clientHome1 = clientCont1.getHome(CLT_HOME_ID_1);
        if (clientHome1 != null) {
            succeed();
        } else {
            fail("Home " + CLT_HOME_ID_1 + " is not child of Container " + clientCont1.getId());
        }
        
        Component client1 = clientHome1.getComponent(CLT_COMP_ID_1);
        if (client1 != null) {
            succeed();
        } else {
            fail("Component " + CLT_COMP_ID_1 + " is not child of Home " + clientHome1.getId());
        }
        
        ComponentServer clientProc1 = host1.getComponentServer(CLT_PROC_ID_1);
        if (clientProc1 != null) {
            succeed();
        } else {
            fail("ComponentServer " + CLT_PROC_ID_1 + " is not child of HostCollocation " + host1.getId());
        }
        
        Container clientCont2 = clientProc1.getContainer(CLT_CONT_ID_2);
        if (clientCont2 != null) {
            succeed();
        } else {
            fail("Container " + CLT_CONT_ID_2 + " is not child of ComponentServer " + clientProc1.getId());
        }
        
        Home clientHome2 = clientCont2.getHome(CLT_HOME_ID_2);
        if (clientHome2 != null) {
            succeed();
        } else {
            fail("Home " + CLT_HOME_ID_2 + " is not child of Container " + clientCont2.getId());
        }
        
        Component client2 = clientHome2.getComponent(CLT_COMP_ID_2);
        if (client2 != null) {
            succeed();
        } else {
            fail("Component " + CLT_COMP_ID_2 + " is not child of Home " + clientHome2.getId());
        }
        
        Component client3 = clientHome2.getComponent(CLT_COMP_ID_3);
        if (client3 != null) {
            succeed();
        } else {
            fail("Component " + CLT_COMP_ID_3 + " is not child of Home " + clientHome2.getId());
        }
        
        HostCollocation host2 = assembly.getHostCollocation(HOST_ID_2);
        if (host2 != null) {
            succeed();
        } else {
            fail("HostCollocation " + HOST_ID_2 + " not loaded.");
        }
        
        ComponentServer clientProc2 = host2.getComponentServer(CLT_PROC_ID_2);
        if (clientProc2 != null) {
            succeed();
        } else {
            fail("ComponentServer " + CLT_PROC_ID_2 + " is not child of HostCollocation " + host2.getId());
        }
        
        Container clientCont3 = clientProc2.getContainer(CLT_CONT_ID_3);
        if (clientCont3 != null) {
            succeed();
        } else {
            fail("Container " + CLT_CONT_ID_3 + " is not child of ComponentServer " + clientProc2.getId());
        }
        
        Home clientHome3 = clientCont3.getHome(CLT_HOME_ID_3);
        if (clientHome3 != null) {
            succeed();
        } else {
            fail("Home " + CLT_HOME_ID_3 + " is not child of Container " + clientCont3.getId());
        }
        
        Component client4 = clientHome3.getComponent(CLT_COMP_ID_4);
        if (client4 != null) {
            succeed();
        } else {
            fail("Component " + CLT_COMP_ID_4 + " is not child of Home " + clientHome3.getId());
        }
        
        //
        // check properties of elements
        //
        println("Check properties of elements");

        println("Check MyHost");
        // check that destination MyHost is DESTINATION1
        if (DESTINATION1.equals(host1.getDestination())) {
            succeed();
        } else {
            fail("Host destination of " + host1.getId() + " is " + 
                 host1.getDestination() + " instead of " + DESTINATION1);
        }

        println("Check ServerProcess");
        // check that destination of ServerProcess is SERVER_PROCESS_DEST
        if (SERVER_PROCESS_DEST.equals(serverProc.getProcessDestination())) {
            succeed();
        } else {
            fail("Process destination of " + serverProc.getId() + " is " + 
                 serverProc.getProcessDestination() + " instead of " + SERVER_PROCESS_DEST);
        }

        println("Check ServerHome_container");
        // check that ComponentKind of ServerHome_container is SESSION
        if (ComponentKindValue.SESSION.equals(serverCont.getComponentKind())) {
            succeed();
        } else {
            fail("ComponentKind of of " + serverCont.getId() + " is not SESSION");
        }
        
        println("Check ServerHome");
        // check that uuid of ServerHome is SERVER_UUID
        if (SERVER_UUID.equals(serverHome.getUUID())) {
            succeed();
        } else {
            fail("UUID for " + serverHome.getId() + " is " + serverHome.getUUID() +
                 " instead of " + SERVER_UUID);
        }

        // check that entrypoint of ServerHome is SERVER_ENTRYPOINT
        if (SERVER_ENTRYPOINT.equals(serverHome.getEntryPoint())) {
            succeed();
        } else {
            fail("Entrypoint of  " + serverHome.getId() + " is " + serverHome.getEntryPoint() +
                 " instead of " + SERVER_ENTRYPOINT);
        }

        // check that configurationcomplete of ServerHome is true
        if (serverHome.getConfigurationComplete()) {
            succeed();
        } else {
            fail("Configurationcomplete of  " + serverHome.getId() + " is set to false");
        }

        // check that ComponentKind of ServerHome is SESSION
        if (ComponentKindValue.SESSION.equals(serverHome.getComponentKind())) {
            succeed();
        } else {
            fail("ComponentKind of of " + serverHome.getId() + " is not SESSION");
        }

        // check that ServantLifetime of ServerHome is COMPONENT_LIFETIME
        if (ServantLifetimeValue.COMPONENT_LIFETIME.equals(serverHome.getServantLifetime())) {
            succeed();
        } else {
            fail("ServantLifetime of of " + serverHome.getId() + " is not COMPONENT_LIFETIME");
        }
        
        // check that ThreadingPolicy of ServerHome is MULTITHREAD
        if (ThreadingPolicyValue.MULTITHREAD.equals(serverHome.getThreadingPolicy())) {
            succeed();
        } else {
            fail("ThreadingPolicy of of " + serverHome.getId() + " is not MULTITHREAD");
        }
      
        // check that HomeRepositoryId of ServerHome is SERVER_HOME_REP_ID
        if (SERVER_HOME_REP_ID.equals(serverHome.getHomeRepositoryId())) {
            succeed();
        } else {
            fail("HomeRepositoryId of of " + serverHome.getId() + " is not " + SERVER_HOME_REP_ID);
        }
      
        // check that HomeServantClassname of ServerHome is SERVER_HOME_SERVANT_CLASSNAME
        if (SERVER_HOME_SERVANT_CLASSNAME.equals(serverHome.getHomeServantClassname())) {
            succeed();
        } else {
            fail("HomeServantClassname of of " + serverHome.getId() + " is not " + SERVER_HOME_SERVANT_CLASSNAME);
        }
      
        println("Check Server1");
        // check that server1 is a SingleComponent
        if (server1 instanceof SingleComponent) {
            succeed();
        } else {
            fail("Component " + server1.getId() + " is not a SingleComponent");
        }

        // check that server1 has SERVER_REP_ID as RepositoryId
        if (SERVER_REP_ID.equals(server1.getRepositoryId())) {
            succeed();
        } else {
            fail("server1 has for RepositoryId " + server1.getRepositoryId() + 
                 " instead of " + SERVER_REP_ID);
        }


        println("Check Server2");
        // check that server2 is a SingleComponent
        if (server2 instanceof SingleComponent) {
            succeed();
        } else {
            fail("Component " + server2.getId() + " is not a SingleComponent");
        }

        // check that server1 has SERVER_REP_ID as RepositoryId
        if (SERVER_REP_ID.equals(server2.getRepositoryId())) {
            succeed();
        } else {
            fail("server1 has for RepositoryId " + server2.getRepositoryId() + 
                 " instead of " + SERVER_REP_ID);
        }

        
        println("Check ClientHome1_container");
        // check that ComponentKind of ClientHome1_container is PROCESS
        if (ComponentKindValue.PROCESS.equals(clientHome1.getComponentKind())) {
            succeed();
        } else {
            fail("ComponentKind of of " + clientHome1.getId() + " is not PROCESS");
        }

        println("Check ClientHome1");
        // check that uuid of ClientHome1 is CLIENT_UUID
        if (CLIENT_UUID.equals(clientHome1.getUUID())) {
            succeed();
        } else {
            fail("UUID for " + clientHome1.getId() + " is " + clientHome1.getUUID() +
                 " instead of " + CLIENT_UUID);
        }

        // check that entrypoint of ClientHome1 is CLIENT_ENTRYPOINT
        if (CLIENT_ENTRYPOINT.equals(clientHome1.getEntryPoint())) {
            succeed();
        } else {
            fail("Entrypoint of  " + clientHome1.getId() + " is " + clientHome1.getEntryPoint() +
                 " instead of " + CLIENT_ENTRYPOINT);
        }

        // check that configurationcomplete of ClientHome1 is false
        if (! clientHome1.getConfigurationComplete()) {
            succeed();
        } else {
            fail("Configurationcomplete of  " + clientHome1.getId() + " is set to true");
        }

        // check that ComponentKind of ClientHome1 is PROCESS
        if (ComponentKindValue.PROCESS.equals(clientHome1.getComponentKind())) {
            succeed();
        } else {
            fail("ComponentKind of of " + clientHome1.getId() + " is not PROCESS");
        }

        // check that ServantLifetime of ClientHome1 is CONTAINER_LIFETIME
        if (ServantLifetimeValue.CONTAINER_LIFETIME.equals(clientHome1.getServantLifetime())) {
            succeed();
        } else {
            fail("ServantLifetime of of " + clientHome1.getId() + " is not CONTAINER_LIFETIME");
        }
        
        // check that ThreadingPolicy of ClientHome1 is SERIALIZE
        if (ThreadingPolicyValue.SERIALIZE.equals(clientHome1.getThreadingPolicy())) {
            succeed();
        } else {
            fail("ThreadingPolicy of of " + clientHome1.getId() + " is not SERIALIZE");
        }
      
        // check that HomeRepositoryId of ClientHome1 is CLIENT_HOME_REP_ID
        if (CLIENT_HOME_REP_ID.equals(clientHome1.getHomeRepositoryId())) {
            succeed();
        } else {
            fail("HomeRepositoryId of of " + clientHome1.getId() + " is not " + CLIENT_HOME_REP_ID);
        }
      
        // check that HomeServantClassname of ClientHome1 is CLIENT_HOME_SERVANT_CLASSNAME
        if (CLIENT_HOME_SERVANT_CLASSNAME.equals(clientHome1.getHomeServantClassname())) {
            succeed();
        } else {
            fail("HomeServantClassname of of " + clientHome1.getId() + " is not " + CLIENT_HOME_SERVANT_CLASSNAME);
        }
        

        println("Check Client1");
        // check that client1 is a SingleComponent
        if (client1 instanceof SingleComponent) {
            succeed();
        } else {
            fail("Component " + client1.getId() + " is not a SingleComponent");
        }

        // check that client1 has SERVER_REP_ID as RepositoryId
        if (CLIENT_REP_ID.equals(client1.getRepositoryId())) {
            succeed();
        } else {
            fail("server1 has for RepositoryId " +client1.getRepositoryId() + 
                 " instead of " + CLIENT_REP_ID);
        }


        println("Check ClientProcess1");
        // check that destination of ClientProcess1 is CLIENT_PROCESS1_DEST
        if (CLIENT_PROCESS1_DEST.equals(clientProc1.getProcessDestination())) {
            succeed();
        } else {
            fail("Process destination of " + clientProc1.getId() + " is " + 
                 clientProc1.getProcessDestination() + " instead of " + CLIENT_PROCESS1_DEST);
        }

        println("Check ClientHome2_container");
        // check that ComponentKind of ClientHome2_container is PROCESS
        if (ComponentKindValue.PROCESS.equals(clientCont2.getComponentKind())) {
            succeed();
        } else {
            fail("ComponentKind of of " + clientCont1.getId() + " is not PROCESS");
        }
        
        println("Check ClientHome2");
        // check that uuid of ClientHome2 is CLIENT_UUID
        if (CLIENT_UUID.equals(clientHome2.getUUID())) {
            succeed();
        } else {
            fail("UUID for " + clientHome2.getId() + " is " + clientHome2.getUUID() +
                 " instead of " + CLIENT_UUID);
        }

        // check that entrypoint of ClientHome2 is CLIENT_ENTRYPOINT
        if (CLIENT_ENTRYPOINT.equals(clientHome2.getEntryPoint())) {
            succeed();
        } else {
            fail("Entrypoint of  " + clientHome2.getId() + " is " + clientHome2.getEntryPoint() +
                 " instead of " + CLIENT_ENTRYPOINT);
        }

        // check that configurationcomplete of ClientHome2 is false
        if (! clientHome2.getConfigurationComplete()) {
            succeed();
        } else {
            fail("Configurationcomplete of  " + clientHome2.getId() + " is set to true");
        }

        // check that ComponentKind of ClientHome2 is PROCESS
        if (ComponentKindValue.PROCESS.equals(clientHome2.getComponentKind())) {
            succeed();
        } else {
            fail("ComponentKind of of " + clientHome2.getId() + " is not PROCESS");
        }

        // check that ServantLifetime of ClientHome2 is CONTAINER_LIFETIME
        if (ServantLifetimeValue.CONTAINER_LIFETIME.equals(clientHome2.getServantLifetime())) {
            succeed();
        } else {
            fail("ServantLifetime of of " + clientHome2.getId() + " is not CONTAINER_LIFETIME");
        }
        
        // check that ThreadingPolicy of ClientHome2 is SERIALIZE
        if (ThreadingPolicyValue.SERIALIZE.equals(clientHome2.getThreadingPolicy())) {
            succeed();
        } else {
            fail("ThreadingPolicy of of " + clientHome2.getId() + " is not SERIALIZE");
        }
      
        // check that HomeRepositoryId of ClientHome2 is CLIENT_HOME_REP_ID
        if (CLIENT_HOME_REP_ID.equals(clientHome2.getHomeRepositoryId())) {
            succeed();
        } else {
            fail("HomeRepositoryId of of " + clientHome2.getId() + " is not " + CLIENT_HOME_REP_ID);
        }
      
        // check that HomeServantClassname of ClientHome2 is CLIENT_HOME_SERVANT_CLASSNAME
        if (CLIENT_HOME_SERVANT_CLASSNAME.equals(clientHome2.getHomeServantClassname())) {
            succeed();
        } else {
            fail("HomeServantClassname of of " + clientHome2.getId() + " is not " + 
                 CLIENT_HOME_SERVANT_CLASSNAME);
        }


        println("Check MyHost2");
        // check that destination MyHost2 is DESTINATION2
        if (DESTINATION2.equals(host2.getDestination())) {
            succeed();
        } else {
            fail("Host destination of " + host2.getId() + " is " + 
                 host2.getDestination() + " instead of " + DESTINATION2);
        }

        println("Check ClientProcess2");
        // check that destination of ClientProcess2 is CLIENT_PROCESS2_DEST
        if (CLIENT_PROCESS2_DEST.equals(clientProc2.getProcessDestination())) {
            succeed();
        } else {
            fail("Process destination of " + clientProc2.getId() + " is " + 
                 clientProc2.getProcessDestination() + " instead of " + CLIENT_PROCESS2_DEST);
        }

        println("Check ClientHome2_container");
        // check that ComponentKind of ClientHome2_container is PROCESS
        if (ComponentKindValue.PROCESS.equals(clientCont2.getComponentKind())) {
            succeed();
        } else {
            fail("ComponentKind of of " + clientCont2.getId() + " is not PROCESS");
        }
        
        println("Check ClientHome2");
        // check that uuid of ClientHome2 is CLIENT_UUID
        if (CLIENT_UUID.equals(clientHome2.getUUID())) {
            succeed();
        } else {
            fail("UUID for " + clientHome2.getId() + " is " + clientHome2.getUUID() +
                 " instead of " + CLIENT_UUID);
        }

        // check that entrypoint of ClientHome2 is CLIENT_ENTRYPOINT
        if (CLIENT_ENTRYPOINT.equals(clientHome2.getEntryPoint())) {
            succeed();
        } else {
            fail("Entrypoint of  " + clientHome2.getId() + " is " + clientHome2.getEntryPoint() +
                 " instead of " + CLIENT_ENTRYPOINT);
        }

        // check that configurationcomplete of ClientHome2 is false
        if (! clientHome2.getConfigurationComplete()) {
            succeed();
        } else {
            fail("Configurationcomplete of  " + clientHome2.getId() + " is set to true");
        }

        // check that ComponentKind of ClientHome2 is PROCESS
        if (ComponentKindValue.PROCESS.equals(clientHome2.getComponentKind())) {
            succeed();
        } else {
            fail("ComponentKind of of " + clientHome2.getId() + " is not PROCESS");
        }

        // check that ServantLifetime of ClientHome2 is CONTAINER_LIFETIME
        if (ServantLifetimeValue.CONTAINER_LIFETIME.equals(clientHome2.getServantLifetime())) {
            succeed();
        } else {
            fail("ServantLifetime of of " + clientHome2.getId() + " is not CONTAINER_LIFETIME");
        }
        
        // check that ThreadingPolicy of ClientHome2 is SERIALIZE
        if (ThreadingPolicyValue.SERIALIZE.equals(clientHome2.getThreadingPolicy())) {
            succeed();
        } else {
            fail("ThreadingPolicy of of " + clientHome2.getId() + " is not SERIALIZE");
        }
      
        // check that HomeRepositoryId of ClientHome2 is CLIENT_HOME_REP_ID
        if (CLIENT_HOME_REP_ID.equals(clientHome2.getHomeRepositoryId())) {
            succeed();
        } else {
            fail("HomeRepositoryId of of " + clientHome2.getId() + " is not " + CLIENT_HOME_REP_ID);
        }
      
        // check that HomeServantClassname of ClientHome2 is CLIENT_HOME_SERVANT_CLASSNAME
        if (CLIENT_HOME_SERVANT_CLASSNAME.equals(clientHome2.getHomeServantClassname())) {
            succeed();
        } else {
            fail("HomeServantClassname of of " + clientHome2.getId() + " is not " + CLIENT_HOME_SERVANT_CLASSNAME);
        }
      
        
        println("Check Client2");
        // check that client2 is a SingleComponent
        if (client2 instanceof SingleComponent) {
            succeed();
        } else {
            fail("Component " + client2.getId() + " is not a SingleComponent");
        }

        // check that client2 has SERVER_REP_ID as RepositoryId
        if (CLIENT_REP_ID.equals(client2.getRepositoryId())) {
            succeed();
        } else {
            fail("server1 has for RepositoryId " +client2.getRepositoryId() + 
                 " instead of " + CLIENT_REP_ID);
        }



        //
        // test if <connections> has been successfully loaded
        //
        println("Test that <connections> has been successfully loaded");

        InterfaceConnection ic1;
        try {
            ic1 = (InterfaceConnection) assembly.getConnection("connect_client1_register");
            if (ic1 != null) {
                succeed();
            } else {
                fail("Connection 'connect_client1_register' doesn't exist in assembly");
            }

        } catch (ClassCastException e) {
            fail("Connection 'connect_client1_register' is not an InterfaceConnection");
        }

        InterfaceConnection ic2;
        try {
            ic2 = (InterfaceConnection) assembly.getConnection("connect_client1_display");
            if (ic2 != null) {
                succeed();
            } else {
                fail("Connection 'connect_client1_display' doesn't exist in assembly");
            }
        } catch (ClassCastException e) {
            fail("Connection 'connect_client1_display' is not an InterfaceConnection");
        }

        InterfaceConnection ic3;
        try {
            ic3 = (InterfaceConnection) assembly.getConnection("connect_client2_register");
            if (ic3 != null) {
                succeed();
            } else {
                fail("Connection 'connect_client2_register' doesn't exist in assembly");
            }
        } catch (ClassCastException e) {
            fail("Connection 'connect_client2_register' is not an InterfaceConnection");
        }

        InterfaceConnection ic4;
        try {
            ic4 = (InterfaceConnection) assembly.getConnection("connect_client2_display");
            if (ic4 != null) {
                succeed();
            } else {
                fail("Connection 'connect_client2_display' doesn't exist in assembly");
            }
        } catch (ClassCastException e) {
            fail("Connection 'connect_client2_display' is not an InterfaceConnection");
        }

        InterfaceConnection ic5;
        try {
            ic5 = (InterfaceConnection) assembly.getConnection("connect_client3_register");
            if (ic5 != null) {
                succeed();
            } else {
                fail("Connection 'connect_client3_register' doesn't exist in assembly");
            }
        } catch (ClassCastException e) {
            fail("Connection 'connect_client3_register' is not an InterfaceConnection");
        }

        InterfaceConnection ic6;
        try {
            ic6 = (InterfaceConnection) assembly.getConnection("connect_client3_display");
            if (ic6 != null) {
                succeed();
            } else {
                fail("Connection 'connect_client3_display' doesn't exist in assembly");
            }
        } catch (ClassCastException e) {
            fail("Connection 'connect_client3_display' is not an InterfaceConnection");
        }

        InterfaceConnection ic7;
        try {
            ic7 = (InterfaceConnection) assembly.getConnection("connect_client4_register");
            if (ic7 != null) {
                succeed();
            } else {
                fail("Connection 'connect_client4_register' doesn't exist in assembly");
            }
        } catch (ClassCastException e) {
            fail("Connection 'connect_client4_register' is not an InterfaceConnection");
        }

        InterfaceConnection ic8;
        try {
            ic8 = (InterfaceConnection) assembly.getConnection("connect_client4_display");
            if (ic8 != null) {
                succeed();
            } else {
                fail("Connection 'connect_client4_display' doesn't exist in assembly");
            }
        } catch (ClassCastException e) {
            fail("Connection 'connect_client4_display' is not an InterfaceConnection");
        }


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
            AssemblyLoaderTest test = new AssemblyLoaderTest();
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




