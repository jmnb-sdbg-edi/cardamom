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


public class FTAssemblyLoaderTest
    extends AssemblyLoaderTest
{

    private static final String FT_ASSEMBLY_DIR = "FTAssemblyLoaderTest";

    protected static final String SRV_BKUP_PROC_ID = "ServerProcessBackup";
    protected static final String SRV_BKUP_CONT_ID = "ServerHomeBackup_container";
    protected static final String SRV_BKUP_HOME_ID = "ServerHomeBackup"; 
    protected static final String SRV_BKUP_COMP_ID = "ServerBackup";

    protected static final String COMP_GROUP_ID = "ServerGroup";
    protected static final String COMP_GROUP_DEST = "ServerGroup";
    
    
    public FTAssemblyLoaderTest()
    {
        this.out = System.out;
        this.name = "AssemblyLoader tests (with fault tolerant assembly)";
        this.assemblyDirectory = FT_ASSEMBLY_DIR;
        this.assembly = null;
    }

    public void doTests()
    {
        // do common tests (without FT implications)
        super.doTests();

        println("Now testing Fault Tolerance specifications");
        addNbOfRequestedTestOK(12);
    
        println("check FTComponentServers");    
        
        ComponentServer serverProc = assembly.getComponentServer(SRV_PROC_ID);
        try {
            FTComponentServer ftserverProc = (FTComponentServer) serverProc;
            succeed();
            
            if (ftserverProc.isPrimary()) {
                succeed();
            } else {
                fail("FTComponentServer " + SRV_PROC_ID + " is not primary");
            }
        } catch (ClassCastException e) {
            fail("ComponentServer " + SRV_PROC_ID + " is not a FTComponentServer");
        }
            
        
        ComponentServer serverBackProc = assembly.getComponentServer(SRV_BKUP_PROC_ID);
        try {
            FTComponentServer ftserverBackProc = (FTComponentServer) serverBackProc;
            succeed();
            
            if (ftserverBackProc.isPrimary()) {
                fail("FTComponentServer " + SRV_BKUP_PROC_ID + " is primary");
            } else {
                succeed();
            }
        } catch (ClassCastException e) {
            fail("ComponentServer " + SRV_BKUP_PROC_ID + " is not a FTComponentServer");
        }
    

        println("check FTComponentGroups");    
        
        ComponentGroup compGroup = assembly.getComponentGroup(COMP_GROUP_ID);
        if (compGroup != null) {
            succeed();
        } else {
            fail("ComponentGroup " + compGroup.getId() + " not loaded.");
        }
        
        if (SERVER_REP_ID.equals(compGroup.getRepositoryId())) {
            succeed();
        } else {
            fail("ComponentGroup " + compGroup.getId() + " as RepositoryId " +
                 compGroup.getRepositoryId() + " instead of " +
                 SERVER_REP_ID);
        }
            
        Component server1 = assembly.getComponent(SRV_COMP_ID_1);
        if (server1.equals(compGroup.getComponent(SRV_COMP_ID_1))) {
            succeed();
        } else {
            fail("ComponentGroup " + compGroup.getId() + " doesn't have " +
                 server1.getId() + " as member.");
        }
        
        Component serverBack = assembly.getComponent(SRV_BKUP_COMP_ID);
        if (serverBack.equals(compGroup.getComponent(SRV_BKUP_COMP_ID))) {
            succeed();
        } else {
            fail("ComponentGroup " + compGroup.getId() + " doesn't have " +
                 serverBack.getId() + " as member.");
        }
        
            
        try {
            FTComponentGroup ftcompGroup = (FTComponentGroup) compGroup;
            succeed();
            
            if (assembly.equals(ftcompGroup.getParentAssembly())) {
                succeed();
            } else {
                fail("FTComponentGroup " + ftcompGroup.getId() + 
                     " has a wrong parent assembly."); 
            }
            
            if (COMP_GROUP_DEST.equals(ftcompGroup.getDestination())) {
                succeed();
            } else {
                fail("FTComponentGroup " + ftcompGroup.getId() + 
                     " has for destination " + ftcompGroup.getDestination() +
                     " instead of " + COMP_GROUP_DEST);
            }
            
            if (server1.equals(ftcompGroup.getPrimary())) {
                succeed();
            } else {
                fail("FTComponentGroup " + ftcompGroup.getId() + 
                     " doesn't have " + server1.getId() + 
                     " as primary member (" + 
                     (ftcompGroup.getPrimary() == null ? "null" : ftcompGroup.getPrimary().getId()) +
                     ").");
            }
            
        } catch (ClassCastException e) {
            fail("ComponentGroup " + COMP_GROUP_ID + " is not a FTComponentGroup");
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
            FTAssemblyLoaderTest test = new FTAssemblyLoaderTest();
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




