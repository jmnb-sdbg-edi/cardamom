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


package cdmw.ccm.serveractivator;


import cdmw.ossupport.OS;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ORBSupport;
//import cdmw.orbsupport.OrbacusUtils;
import cdmw.orbsupport.ORBUtils;
import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import org.omg.Components.ConfigValue;
import org.omg.Components.CreateFailure;
import org.omg.Components.Deployment.ComponentServer;
import org.omg.Components.Deployment.ComponentInstallation;
import org.omg.Components.Deployment.InvalidConfiguration;

import cdmw.ccm.deployment.ConfigValueImpl;

import com.thalesgroup.CdmwDeployment.PROCESS_DESTINATION;


public class ServerActivatorImplTest
    extends cdmw.testutils.Testable
{

    private static final int POA_THREAD_POOL_SIZE = 10;
    
    // Naming Service started by SimRepository with port 5010
    //  (see SimuUtils.startSimu())
    private static final String NAMING_PORT = "5010";
    private static final String NAMING_SERVICE_CORBALOC = 
        "corbaloc::localhost:" + NAMING_PORT + "/NameService";
    
    private static final String APPLI_NAME = "FooAppli";
    private static final String PROCESS_NAME = "FooProcess";
    private static final String DESTINATION_NAME = PROCESS_NAME + "@" + APPLI_NAME;
    
    private static org.omg.CORBA.ORB orb = null;
    private static cdmw.ossupport.ThreadedProcess namingProc = null;
    
    private static ComponentServerStub compServerStub;
    private static ComponentServer compServer;
    private static ComponentInstallationStub compInstStub;
    private static ComponentInstallation compInst;
    private static ServerActivatorImpl activator;
    
    
    
    public ServerActivatorImplTest()
    {
        this.out = System.out;
        this.name = "ServerActivatorImpl tests";
    }

    
    public void doTests()
    {
        // set number of requested successfull tests
        setNbOfRequestedTestOK(8);
        
        // test create_component_server
        println("Test create_component_server()");
        
        // create PROCESS_DESTINATION ConfigValue
        ConfigValueImpl compServConfig = new ConfigValueImpl();
        compServConfig.name = PROCESS_DESTINATION.value;
        compServConfig.value = orb.create_any();
        compServConfig.value.insert_string(DESTINATION_NAME);
        
        // create ConfigValues
        ConfigValue[] config = new ConfigValue[1];
        config[0] = compServConfig;
        try {
            ComponentServer result = activator.create_component_server(config);
        
            if (result._is_equivalent(compServer)) {
                succeed();
            } else {
                fail("Returned ComponentServer is not equivalent to expected ComponentServer");
            }
        
        } catch (InvalidConfiguration e) {
            fail("InvalidConfiguration while create_component_server");
            printInvalidConfiguration(e);
        
        } catch (CreateFailure e) {
            fail("CreateFailure while create_component_server");
            printCreateFailure(e);
        }
        
        
        // test with a bad type for COMPONENT_SERVER_REFERENCE
        compServConfig.value.insert_long(1);
        config[0] = compServConfig;
        try {
            ComponentServer result = activator.create_component_server(config);
            fail("create_component_server succeed with a bad configuration !!");
            
        } catch (InvalidConfiguration e) {
            if (e.reason == org.omg.Components.Deployment.InvalidConfigValueType.value &&
                e.name.equals(PROCESS_DESTINATION.value)) {
                succeed();
            } else {
                fail("Unexpected InvalidConfiguration exception");
                printInvalidConfiguration(e);
            }
        } catch (CreateFailure e) {
            fail("CreateFailure while create_component_server (InvalidConfiguration expected)");
            printCreateFailure(e);
        }
        
        
        // test with a bad reference for COMPONENT_SERVER_REFERENCE
        compServConfig.value.insert_string("foo ior");
        config[0] = compServConfig;
        try {
            ComponentServer result = activator.create_component_server(config);
            fail("create_component_server succeed with a bad configuration !!");
            
        } catch (InvalidConfiguration e) {
            fail("InvalidConfiguration while create_component_server (CreateFailure expected)");
            printInvalidConfiguration(e);
        } catch (CreateFailure e) {
            succeed();
        }
        
        // test with an unknown ConfigValue
        ConfigValueImpl fooConfig = new ConfigValueImpl();
        compServConfig.name = "FOO_CONFIG";
        compServConfig.value = orb.create_any();
        compServConfig.value.insert_string("foo value");
        config = new ConfigValue[2];
        config[0] = compServConfig;
        config[1] = fooConfig;
        try {
            ComponentServer result = activator.create_component_server(config);
            fail("create_component_server succeed without configuration !!");
        
        } catch (InvalidConfiguration e) {
            if (e.reason == org.omg.Components.Deployment.UnknownConfigValueName.value &&
                e.name.equals("FOO_CONFIG")) {
                succeed();
            } else {
                fail("Unexpected InvalidConfiguration exception");
                printInvalidConfiguration(e);
            }
        } catch (CreateFailure e) {
            fail("CreateFailure while create_component_server (InvalidConfiguration expected)");
            printCreateFailure(e);
        }
        
        
        // test with no ConfigValues
        config = new ConfigValue[0];
        try {
            ComponentServer result = activator.create_component_server(config);
            fail("create_component_server succeed without configuration !!");
        
        } catch (InvalidConfiguration e) {
            if (e.reason == org.omg.Components.Deployment.ConfigValueRequired.value &&
                e.name.equals(PROCESS_DESTINATION.value)) {
                succeed();
            } else {
                fail("Unexpected InvalidConfiguration exception");
                printInvalidConfiguration(e);
            }
        } catch (CreateFailure e) {
            fail("CreateFailure while create_component_server (InvalidConfiguration expected)");
            printCreateFailure(e);
        }
        
        
        
        //
        // test get_component_servers
        //
        println("Test get_component_servers()");

        ComponentServer[] servers = activator.get_component_servers();
        if (servers.length != 1) {
            fail("There is " + servers.length + 
                 " ComponentServers, there should be only 1.");
            return;
        }
        if (servers[0]._is_equivalent(compServer)) {
            succeed();
        } else {
            fail("The registred ComponentServer is not equivalent to expected ComponentServer");
        }
        
        //
        // test remove_component_server
        //
        println("Test remove_component_server()");
        
        try {
            activator.remove_component_server(servers[0]);
            if (compServerStub.isRemoved()) {
                succeed();
            } else {
                fail("ComponentServer has not been removed");
            }
            
            ComponentServer[] servers2 = activator.get_component_servers();
            if (servers2.length == servers.length - 1) {
                succeed();
            } else {
                fail("There is " + servers2.length + " registred in ServerActivator. " +
                     "There should be " + (servers.length - 1) + ".");
            }
            
        } catch (org.omg.Components.RemoveFailure e) {
            fail("RemoveFailure " + e.reason + " during remove_component_server().");
        }
    }

    private void printInvalidConfiguration(InvalidConfiguration e) {
        println(e.getMessage());
        if (e.reason == org.omg.Components.Deployment.UnknownConfigValueName.value) {
            println("   UnknownConfigValueName : " + e.name);
        } else if (e.reason == org.omg.Components.Deployment.InvalidConfigValueType.value) {
            println("   InvalidConfigValueType : " + e.name);        
        } else if (e.reason == org.omg.Components.Deployment.ConfigValueRequired.value) {
            println("   ConfigValueRequired : " + e.name);        
        } else if (e.reason == org.omg.Components.Deployment.ConfigValueNotExpected.value) {
            println("   ConfigValueNotExpected : " + e.name);        
        } else {
            println("   Unkwown reason (" + e.reason + ") : " + e.name);        
        }
    }
    
    private void printCreateFailure(CreateFailure e) {
        println(e.getMessage());
        int noMaskReason = 
            e.reason ^ com.thalesgroup.CdmwDeployment.CDMW_VID.value
                     ^ com.thalesgroup.CdmwDeployment.CREATE_FAILURE_BASE.value;
        
        println("Reason: " + e.reason + " => " + noMaskReason + " without mask");
    }
    

    private static org.omg.CosNaming.NamingContext startNamingService() {
        try {
            namingProc = ORBUtils.startNamingService(NAMING_PORT);
            OS.sleep(getTimescale() * 3000);
        } catch(java.io.IOException ioe) {
            System.out.println(ioe.getMessage());
            return null;
        } catch(InterruptedException ie) {
            System.out.println(ie.getMessage());
            return null;
        }

        org.omg.CORBA.Object obj = 
            orb.string_to_object("corbaloc::localhost:" + NAMING_PORT +
                                 "/NameService");
                                             
        return org.omg.CosNaming.NamingContextHelper.narrow(obj);
    }
    
    
    private static void initRepository(org.omg.CosNaming.NamingContext nc) 
        throws Exception
    {
        if (nc == null) {
            System.out.println("`NameService' is not a NamingContextExt object reference");
            throw new org.omg.CORBA.TRANSIENT(
                cdmw.orbsupport.ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        String REPOSITORY_NAME = "CDMW.I/repository.simulated";

        // Get reference to the repository interface
        com.thalesgroup.CdmwNamingAndRepository.Repository rep = null;
        try {
            NamingInterface ni = new NamingInterface(nc);
            org.omg.CORBA.Object obj_ = ni.resolve(REPOSITORY_NAME);
            rep = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj_);
            if (rep == null)  {
                System.out.println("Couldn't retrieve reference of the Repository");
                throw new org.omg.CORBA.TRANSIENT(
                    cdmw.orbsupport.ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            System.out.println("Couldn't retrieve reference of the Repository");
            throw new org.omg.CORBA.TRANSIENT(
                cdmw.orbsupport.ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        try {
            RepositoryInterface.init(cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN, rep);
        } catch (Exception e) {
            System.out.println("Couldn't init Repository");
            throw e;
        }
    }
    
    
    private static org.omg.CosNaming.NamingContext getNamingService() 
        throws org.omg.CORBA.TRANSIENT
    {
        //
        // Get naming service
        //
        org.omg.CORBA.Object obj = null;
//        try {
            obj = orb.string_to_object(NAMING_SERVICE_CORBALOC);
/*        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
             System.out.println("Can't resolve `NameService'");
            throw new org.omg.CORBA.TRANSIENT(
                cdmw.orbsupport.ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
*/
        if(obj==null) {
            System.out.println("`NameService' is a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                cdmw.orbsupport.ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        org.omg.CosNaming.NamingContextExt nc
            = org.omg.CosNaming.NamingContextExtHelper.narrow(obj);


        if (nc==null) {
            System.out.println("`NameService' is not a NamingContextExt object reference");
            throw new org.omg.CORBA.TRANSIENT(
                cdmw.orbsupport.ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        
        return nc;
    }
    
    
    public static void main(String[] args) {
        int timescale = cdmw.testutils.Testable.getTimescale();        

        orb = null;
        try {
            // init ORB
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = ORBSupport.ORBInit(args, orbStrategies);
    
            org.omg.PortableServer.POA rootPOA = 
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();    
    
            // create ComponentServerStub and activate it
            compServerStub = new ComponentServerStub(rootPOA);
            compServer = compServerStub._this(orb);

            // create ComponentInstallationStub and activate it
            compInstStub = new ComponentInstallationStub(rootPOA);
            compInst = compInstStub._this(orb);
 
            // start simrepository
            cdmw.tools.SimuUtils.startSimu(System.out);
            System.out.println("Simrepository started");
            OS.sleep(timescale * 10000);

            // get naming service
            org.omg.CosNaming.NamingContext naming = getNamingService();   



            // init repository
            initRepository(naming);
            System.out.println("Simrepository initialised");

           
/*            // start naming service
            org.omg.CosNaming.NamingContext naming =
                startNamingService();
//            orb.register_initial_reference("NameService", naming);
*/            cdmw.commonsvcs.naming.NamingInterface namingInterface =
                new cdmw.commonsvcs.naming.NamingInterface(naming);
            System.out.println("Naming service started");
            
            // register ComponentServer
            try {
                namingInterface.bind(DESTINATION_NAME + ".ComponentServer",
                                     compServer,
                                     true);
            } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                System.err.println("Error while registring ComponentServer");
                throw e;
            } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                System.err.println("Error while registring ComponentServer");
                throw e;
            } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                System.err.println("Error while registring ComponentServer");
                throw e;
            } catch (org.omg.CosNaming.NamingContextPackage.AlreadyBound e) {
                System.err.println("Error while registring ComponentServer");
                throw e;
            }
                    
                    
            // Get AdminRootContext NamingInterface object from Repository
            cdmw.commonsvcs.naming.NamingInterface adminNaming = null;
            try {
                // Get admin root naming context
                org.omg.CosNaming.NamingContext adminRoot =
                    RepositoryInterface.getRepository().resolve_root_context(
                        cdmw.common.Locations.ADMIN_ROOT_CONTEXT_ID);
    
                // create naming interface on admin root context
                adminNaming = new cdmw.commonsvcs.naming.NamingInterface(adminRoot);
    
            } catch (com.thalesgroup.CdmwNamingAndRepository
                                    .RepositoryPackage.NoRootContext ex) {
                // shouldn't happen
                ex.printStackTrace();
                cdmw.common.Assert.check(false);
            }
            
            // register ComponentInstallation
            try {
                String registerName = cdmw.ossupport.OS.getHostname() +
                                      "/ComponentInstallation";
                adminNaming.bind(registerName,
                                 compInst,
                                 true);
            } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                System.err.println("Error while registring ComponentServer");
                throw e;
            } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                System.err.println("Error while registring ComponentServer");
                throw e;
            } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                System.err.println("Error while registring ComponentServer");
                throw e;
            } catch (org.omg.CosNaming.NamingContextPackage.AlreadyBound e) {
                System.err.println("Error while registring ComponentServer");
                throw e;
            }
                    
            // create ServerActivatorImpl and activate it
            activator = new ServerActivatorImpl(orb, rootPOA, naming);
            activator._this(orb);
            // run test
            ServerActivatorImplTest test = new ServerActivatorImplTest();
            test.start();            

        } catch (Exception e) {
            e.printStackTrace();
        }

        cdmw.tools.SimuUtils.stopSimu(System.out);
        
        
        // stop naming service
        if (namingProc != null) {
            namingProc.killProcess();
        }

        // destroy ORB
        if (orb != null) {
            orb.destroy();
        }            
    }


}




