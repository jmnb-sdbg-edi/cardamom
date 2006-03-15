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


package cdmw.ccm.cif.test;


import java.io.File;
import java.util.Vector;
import java.util.StringTokenizer;

import cdmw.common.Assert;
import cdmw.common.Options;
import cdmw.common.Locations;
import cdmw.testutils.Testable;

import cdmw.ossupport.OS;
import cdmw.ossupport.FileUtilities;
import cdmw.ossupport.ThreadedProcess;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ORBUtils;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.ccm.deployment.ConfigValueImpl;
import cdmw.ccm.container.ContainerImpl;
import cdmw.ccm.container.HomeAllocator;
import cdmw.ccm.container.HomeAllocatorRegistry;
import cdmw.ccm.container.HomesServantActivator;

import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

public class MainTest {
    
    public static final String PROCESS_NAME = "DummyProcessName";
    public static final String APPLICATION_NAME = "DummyApplicationName";
    
    public static final String CDMW_RUNTIME_ROOT = 
        System.getProperty("CDMW_RUNTIME_ROOT");

    public static final String NAMING_SERVICE_PORT_OPTION  = "--NamePort";
    public static final String REPOSITORY_PORT_OPTION      = "--RepPort";
    
    public static final int SUCCESS = 0;
    public static final int FAILURE = 1;

    public static final int POA_THREAD_POOL_SIZE = 5;
    
    public static class ComponentInstallationPlugImpl
        extends com.thalesgroup.CdmwDeployment.ComponentInstallationPOA {
    
        private org.omg.PortableServer.POA poa;
    
        public ComponentInstallationPlugImpl(org.omg.PortableServer.POA poa) {
            this.poa = poa;
        }
    
        public org.omg.PortableServer.POA _default_POA() {
            return this.poa;
        }
    
        //
        // IDL:omg.org/Components/Deployment/ComponentInstallation/install:1.0
        //
        public void install(String implUUID, String component_loc)
            throws org.omg.Components.Deployment.InvalidLocation,
            org.omg.Components.Deployment.InstallationFailure {}
    
        //
        // IDL:omg.org/Components/Deployment/ComponentInstallation/replace:1.0
        //
        public void replace(String implUUID, String component_loc)
            throws org.omg.Components.Deployment.InvalidLocation,
            org.omg.Components.Deployment.InstallationFailure {}
    
        //
        // IDL:omg.org/Components/Deployment/ComponentInstallation/remove:1.0
        //
        public void remove(String implUUID)
            throws org.omg.Components.Deployment.UnknownImplId,
            org.omg.Components.RemoveFailure {}
    
        //
        // IDL:omg.org/Components/Deployment/ComponentInstallation/get_implementation:1.0
        //
        public String get_implementation(String implUUID)
            throws org.omg.Components.Deployment.UnknownImplId,
            org.omg.Components.Deployment.InstallationFailure {

            String CLASSPATH=System.getProperty("java.class.path");
            String filePath="";
            StringTokenizer st = new StringTokenizer(CLASSPATH,":");
            while (st.hasMoreTokens()) {
                String token = st.nextToken();
                
                if(token.endsWith("libcdmwPhilo.jar"))
                    {
                        filePath = token;              
                    }
            }  
            System.out.println(filePath);
            return filePath;
        }
    
        //
        // IDL:thalesgroup.com/CdmwDeployment/ComponentInstallation/get_valuetypefactory_location:1.0
        //
        public String get_valuetypefactory_location(
            String implUUID, 
            String repid)
            throws org.omg.Components.Deployment.UnknownImplId,
            com.thalesgroup.CdmwDeployment.UnknownValuetype,
            org.omg.Components.Deployment.InstallationFailure {

            String CLASSPATH=System.getProperty("java.class.path");
            String filePath="";
            StringTokenizer st = new StringTokenizer(CLASSPATH,":");
            while (st.hasMoreTokens()) {
                String token = st.nextToken();
                
                if(token.endsWith("libcdmwPhilo.jar"))
                    {
                        filePath = token;              
                    }
            }  
            System.out.println(filePath);
            return filePath;
        }

    }

    public MainTest() {}
    
    public static org.omg.PortableServer.POA createContainerPOA(
        org.omg.PortableServer.POA parent) throws Exception {
        //
        // Create a POA with Retain and Single-thread policicies
        // to be used to activate the ComponentServer and Container objects.
        // Lifespan policy            = TRANSIENT
        // Id Assignment policy       = SYSTEM_ID
        // Id Uniqueness policy       = UNIQUE_ID
        // Implicit Activation policy = IMPLICIT_ACTIVATION
        // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY
        // Servant Retention policy   = RETAIN;
        // Thread Policy              = SINGLE_THREAD_MODEL
        //        
        org.omg.PortableServer.POAManager poaManager = parent.the_POAManager();
        org.omg.CORBA.Policy[] policies = new org.omg.CORBA.Policy[7];        
        policies[0] = parent.create_id_assignment_policy(
            org.omg.PortableServer.IdAssignmentPolicyValue.SYSTEM_ID);
        policies[1] = parent.create_lifespan_policy(
            org.omg.PortableServer.LifespanPolicyValue.TRANSIENT);
        policies[2] = parent.create_servant_retention_policy(
            org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN);
        policies[3] = parent.create_id_uniqueness_policy(
            org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID);
        policies[4] = parent.create_request_processing_policy(
            org.omg.PortableServer.RequestProcessingPolicyValue.USE_ACTIVE_OBJECT_MAP_ONLY);
        policies[5] = parent.create_implicit_activation_policy(
            org.omg.PortableServer.ImplicitActivationPolicyValue.IMPLICIT_ACTIVATION);
        policies[6] = parent.create_thread_policy(
            org.omg.PortableServer.ThreadPolicyValue.SINGLE_THREAD_MODEL);

        StrategyList poaStrategies = new StrategyList();

        String CONTAINER_POA_NAME = "ContainerPOA";
        return ORBSupport.createPOA(
            parent,
            CONTAINER_POA_NAME,
            poaManager, 
            policies,
            poaStrategies);
    }

    public static int run(
        org.omg.CORBA.ORB orb, 
        String containerIOR, 
        String consumerIOR) {
        ClientThread client = new ClientThread(orb, containerIOR, consumerIOR);
    
        client.start();
        orb.run();
        try {
            client.join();
        } catch(InterruptedException ie) {}
    
        return SUCCESS;
    }
    
    public static com.thalesgroup.CdmwPlatformMngt.ProcessCallback 
        getProcessCallback(org.omg.CORBA.ORB orb) 
        throws org.omg.CORBA.TRANSIENT {
        
        Assert.check(orb != null);
        
        org.omg.CORBA.Object obj = null;
        //
        // Get naming service
        //
        try {
            obj = orb.resolve_initial_references("NameService");
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.err.println("Can't resolve `NameService'");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    
        if (obj == null) {
            System.err.println("`NameService' is a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        
        org.omg.CosNaming.NamingContextExt nc =
            org.omg.CosNaming.NamingContextExtHelper.narrow(obj);
    
        if (nc == null) {
            System.err.println("`NameService' is not a NamingContextExt object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    
        String PROCESS_CALLBACK_BINDING_NAME = "CDMW.I/process-callback.simulated";
    
        // Get reference to the ProcessCallback object
        com.thalesgroup.CdmwPlatformMngt.ProcessCallback callback = null;
        try {
            NamingInterface ni = new NamingInterface(nc);
            org.omg.CORBA.Object obj__ = 
                ni.resolve(PROCESS_CALLBACK_BINDING_NAME);
            callback = 
                com.thalesgroup.CdmwPlatformMngt.ProcessCallbackHelper.narrow(obj__);
            if (callback == null) {
                System.err.println("Couldn't retrieve reference of the ProcessCallback");
                // TODO: Update minor code
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENTCannotReachApplicationAgent,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            System.err.println("Couldn't retrieve reference of the ProcessCallback");
            // TODO: Update minor code
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachApplicationAgent,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        
        return callback;
    }
    
    public static void initRepository(org.omg.CORBA.ORB orb) throws Exception {
    
        //
        // Get naming service
        //
        org.omg.CORBA.Object obj = null;
    
        try {
            obj = orb.resolve_initial_references("NameService");
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.err.println("Can't resolve `NameService'");
            throw in;
        }
    
        if (obj == null) {
            System.err.println("`NameService' is a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        
        org.omg.CosNaming.NamingContextExt nc = null;
        try {
            nc = org.omg.CosNaming.NamingContextExtHelper.narrow(obj);
        } catch (Exception e) {
            System.err.println("Obj is not a NamingContextExt");
            throw e;
        }
    
        if (nc == null) {
            System.err.println("`NameService' is not a NamingContextExt object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    
        String REPOSITORY_NAME = "CDMW.I/repository.simulated";
    
        // Get reference to the repository interface
        com.thalesgroup.CdmwNamingAndRepository.Repository rep = null;
        try {
            NamingInterface ni = new NamingInterface(nc);
            org.omg.CORBA.Object obj__ = ni.resolve(REPOSITORY_NAME);
                
            rep = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj__);
            if (rep == null) {
                System.err.println("Couldn't retrieve reference of the Repository");
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.err.println("Couldn't retrieve reference of the Repository");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        try {
            RepositoryInterface.init(Locations.CDMW_SERVICES_NAME_DOMAIN, rep);
        } catch (Exception e) {
            System.err.println("Couldn't init Repository");
            throw e;
        }
    }
    
    public static void main(String[] args) {

        int status = SUCCESS;
        org.omg.CORBA.ORB orb = null;
        com.thalesgroup.CdmwPlatformMngt.ProcessCallback callback = null;
        ThreadedProcess naming = null;
        ThreadedProcess repository = null; 
        ThreadedProcess eventServer = null;
        ThreadedProcess profileManager = null; 
    
        // help argument
        if (args.length >= 1) {
            if ( args[0].equals("--help") ) {
                System.out.println("Usage:\n"
                    + "java cdmw.ccm.cif.test.MainTest options\n"
                    + "\n"
                    + "Options:\n"
                    + "--help                     Display this information.\n"
                    + "\n"
                    + "[--NamePort=<nbr>          Port of Name Service.\n"
                    + "[--RepPort=<nbr>           Port of Name and Repository.\n");
                return;
            }
        }
        
        String callbackIOR = 
            OS.getOptionValue(args, Options.CALLBACK_IOR_OPTION);
    
        boolean unitTests = true;
        // ---- Is it launched by platform management for integration tests? ----
        if (!callbackIOR.equals("no") && !callbackIOR.equals("yes")) {
            unitTests = false;
        }
    
        // get Name Service port
        String nameServicePort = 
            OS.getOptionValue(args, NAMING_SERVICE_PORT_OPTION);
        
        if (nameServicePort.equals("no"))  {
            nameServicePort = "5000";
        } 
        
        // get Repository port
        String repositoryPort = 
            OS.getOptionValue(args, REPOSITORY_PORT_OPTION);
        
        if (repositoryPort.equals("no")) {
            repositoryPort = "5005";
        } 
    
        int timescale = Testable.getTimescale();
    
        try {
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
    
          //  String[] serverArgs = {"-ORBconfig", "./obtest.conf"};
		String s = ORBUtils.NAMING_PORT_OPTION + "corbaloc::localhost:"
                       + nameServicePort + "/NameService";
            String[] serverArgs = s.split(" ");
            orb = ORBSupport.ORBInit(serverArgs, orbStrategies);

            // PCR-0049
            cdmw.orbsupport.Codec.init(orb);
    
            if (unitTests) {
                // Starting the Name Service
                System.out.println("Starting Name Service on port " + 
                    nameServicePort + "...");
                
                naming = ORBUtils.startNamingService(nameServicePort);
                OS.sleep(timescale * 3000);
    
                // Starting the Repository Simulator :
                String namingCorbaloc = " -ORBInitRef NameService=corbaloc::localhost:" + 
                    nameServicePort + "/NameService";
                
                String simRepositoryArgs = " -ORBEndpoint iiop://localhost: "+ repositoryPort 
                    + namingCorbaloc 
                    + " -DOM " + Locations.CDMW_SERVICES_NAME_DOMAIN
                    + " -DOM " + Locations.CDMW_EVENT_SERVICE_NAME_DOMAIN
                    + " -DOM CDMW/SERVICES/EVENT/FACTORIES"
                    + " -DOM CDMW/SERVICES/EVENT/EVENT_CHANNELS"
                    + " -DOM CDMW/SERVICES/EVENT/TYPED_EVENT_CHANNELS"
                    + " -ROOT " + Locations.ADMIN_ROOT_CONTEXT_ID;
                
                System.out.println("Starting Simulated Repository with the following arguments:");
                System.out.println(simRepositoryArgs);
                
                repository = OS.createProcess(
                    "cdmw_simulated_repository " + simRepositoryArgs);
                OS.sleep(timescale * 10000);
                System.out.println("Simulated Repository started");
    
                // init repository 
                initRepository(orb);
                OS.sleep(timescale * 3000);
                System.out.println("Simrepository initialised");
    
                // Getting callback IOR
                callback = getProcessCallback(orb);
                callbackIOR = orb.object_to_string(callback);
            }
    
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
     
            OS.sleep(timescale * 1000);
            System.out.println("Server initialised");
/*
            // Starting the EventChannelManager
            String profileManagerService = 
                "cdmw.eventadmin.EventChannelManager";
            String managerArgs = " -ORBconfig ./obtest.conf "
                + Options.CALLBACK_IOR_OPTION + "=" + callbackIOR + " " 
                + Options.PROCESS_PORT_OPTION + "=15327 " 
                + Options.LOCALISATION_SERVICE_OPTION+ "=15328 " 
                + " --ProfileXMLFile=./EventChannelConf1.xml " 
                + Options.PROCESS_XML_FILE_OPTION + "=./EventChannelManager.xml "
                + Options.VALIDATE_XML_OPTION;
        
            System.out.println("Starting EventChannelManager " + 
                "with the following arguments: " + managerArgs);
            profileManager = OS.createJavaProcess(
                profileManagerService + managerArgs, Testable.getProperties());
            OS.sleep(timescale * 10000);
            System.out.println("EventChannelManager started");
  */
	    // Starting the EventChannelManager
            String profileManagerService = "cdmw_event_channel_manager";
            String managerArgs =
                " " +
		" " + Options.CALLBACK_IOR_OPTION + "=" + callbackIOR +
                " " + Options.PROCESS_PORT_OPTION + "=15327" +
                " " + Options.LOCALISATION_SERVICE_OPTION + "=15328" +
                " " + Options.PROCESS_XML_FILE_OPTION + "=./EventChannelManager.xml" +
                " --ProfileXMLFile=EventChannelConf1.xml" +
                " " + Options.VALIDATE_XML_OPTION;

            System.out.println("Starting EventChannelManager with the following parameters: ");
            System.out.println(managerArgs);
            profileManager = OS.createProcess(
                profileManagerService + managerArgs);
            //ProcessThread pt = new ProcessThread(managerArgs);
            //pt.start();
            OS.sleep(timescale * 20000);
            System.out.println("EventChannelManager started");

      
            // EventChannelManager Proc URL is CDMW Test Application/PROC_000 
            com.thalesgroup.CdmwPlatformMngt.ProcessDelegate proc = null;
            String procURL = "corbaname::localhost:" + nameServicePort
                + "#CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_000";
            obj = orb.string_to_object(procURL);
        
            if (obj != null) {
                proc = com.thalesgroup.CdmwPlatformMngt.ProcessDelegateHelper.narrow(obj);
            } else {
                System.exit(-1);
            }
        
            cdmw.tools.ProcessAdmin admin = 
                new cdmw.tools.ProcessAdmin(orb, proc);
        
            // send initialise order
            java.io.BufferedReader commands = new java.io.BufferedReader(
                new java.io.StringReader("init LAST 2\n exit \n"));
            admin.run(commands, System.out);
            OS.sleep(timescale * 5000);
        
            // send run order
            commands = new java.io.BufferedReader(
                new java.io.StringReader("run\n exit \n"));
            admin.run(commands, System.out);
            OS.sleep(timescale * 3000);
       /* 
            // Starting Eventservice
            System.out.println("Starting Event Service...");
            eventServer = ORBUtils.startEventService(
                "-ORBconfig ./obtest.conf");
            OS.sleep(timescale * 10000);
            System.out.println("Event Service started");
         */

	    // Starting the cdmw_event_admin which creates event channels
            // This process stops itself
	/*   ThreadedProcess eventAdmin = null;
            String adminArgs =
                " " + ORBUtils.NAMING_PORT_OPTION_SLAVE + "corbaloc::localhost:5000/NameService" +
                " --commandFile-name=ec_creation.cmd";
            System.out.println("Starting EventServiceAdmin with the following parameters: ");
            System.out.println(adminArgs);
            eventAdmin = OS.createProcess(
                "cdmw_event_admin" + adminArgs);
          */  OS.sleep(timescale * 10000);
	
    
            // declare Value factory to the orb
            org.omg.CORBA.portable.ValueFactory vf;
    
            vf = new cdmw.ccm.cif.FacetDescriptionFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/FacetDescription:1.0", vf);
           
            vf = new cdmw.ccm.cif.PortDescriptionFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/PortDescription:1.0", vf);
            
            vf = new cdmw.ccm.common.CookieFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/Cookie:1.0", vf);
            
            vf = new cdmw.ccm.cif.ConnectionDescriptionFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/ConnectionDescription:1.0", vf);
            
            vf = new cdmw.ccm.cif.ReceptacleDescriptionFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/ReceptacleDescription:1.0", vf);
            
            vf = new cdmw.ccm.cif.ConsumerDescriptionFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/ConsumerDescription:1.0", vf);
            
            vf = new cdmw.ccm.cif.EmitterDescriptionFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/EmitterDescription:1.0", vf);
            
            vf = new cdmw.ccm.cif.SubscriberDescriptionFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/SubscriberDescription:1.0", vf);
            
            vf = new cdmw.ccm.cif.PublisherDescriptionFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/PublisherDescription:1.0", vf);
            
            vf = new cdmw.ccm.cif.ComponentPortDescriptionFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/ComponentPortDescription:1.0", vf);
    
            vf = new cdmw.ccm.deployment.ConfigValueFactoryImpl();
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                "IDL:omg.org/Components/ConfigValue:1.0", vf);
    
            // Register HomeAllocator
            HomeAllocator allocator = new HomeAllocator(
                "Cdmw.CCM.CIF.CdmwDinner.SessionPhilosopherHome_impl");
            HomeAllocatorRegistry.register(
                "Cdmw.CCM.CIF.CdmwDinner.SessionPhilosopherHome_impl",
                allocator);
    
            allocator = new HomeAllocator(
                "Cdmw.CCM.CIF.CdmwDinner.SessionObserverHome_impl");
            HomeAllocatorRegistry.register(
                "Cdmw.CCM.CIF.CdmwDinner.SessionObserverHome_impl",
                allocator);
            
            allocator = new HomeAllocator(
                "Cdmw.CCM.CIF.CdmwDinner.SessionForkHome_impl");
            HomeAllocatorRegistry.register(
                "Cdmw.CCM.CIF.CdmwDinner.SessionForkHome_impl",
                allocator);
    
            allocator = new HomeAllocator(
                "Cdmw.CCM.CIF.CdmwDinner.SessionRegistratorHome_impl");
            HomeAllocatorRegistry.register(
                "Cdmw.CCM.CIF.CdmwDinner.SessionRegistratorHome_impl",
                allocator);
    
            /*
            org.omg.CORBA.Policy[] pl = new org.omg.CORBA.Policy[1];
            pl[0] = rootPOA.create_thread_policy(
                org.omg.PortableServer.ThreadPolicyValue.SINGLE_THREAD_MODEL);
    
            // Get parent POA's POA manager
            org.omg.PortableServer.POAManager pmanager
                = rootPOA.the_POAManager();
    
            // Create new POA
            org.omg.PortableServer.POA poa =
                rootPOA.create_POA("poaTest", pmanager, pl);
    
            // Clean up
            for (int i = 0; i < pl.length; ++i) {
                pl[i].destroy();
            }
            */
    
            ComponentInstallationPlugImpl compInstallImpl 
                = new ComponentInstallationPlugImpl(rootPOA);
            com.thalesgroup.CdmwDeployment.ComponentInstallation compInstall
                = compInstallImpl._this(orb);
    
            org.omg.Components.ConfigValue[] configValues = 
                new org.omg.Components.ConfigValue[1];
            org.omg.CORBA.Any value = orb.create_any();
            com.thalesgroup.CdmwDeployment.ComponentKindValueHelper.insert(
                value, 
                com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);
            configValues[0] = new ConfigValueImpl(
                com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value, value);
    
            org.omg.PortableServer.POA homesSerializePOA = null;
            org.omg.PortableServer.POA homesMultithreadPOA = null;
            org.omg.PortableServer.POA containerPOA = 
                createContainerPOA(rootPOA);
            // Create a SingleThread POA for single threaded homes and 
            // a Multithread POA for multithread homes.
            try {
                org.omg.PortableServer.POAManager poaManager = 
                    containerPOA.the_POAManager();
                org.omg.CORBA.Policy[] policies = new org.omg.CORBA.Policy[7];
                policies[0] = containerPOA.create_id_assignment_policy(
                    org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID);
                policies[1] = containerPOA.create_lifespan_policy(
                    org.omg.PortableServer.LifespanPolicyValue.PERSISTENT);
                policies[2] = containerPOA.create_servant_retention_policy(
                    org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN);
                policies[3] = containerPOA.create_id_uniqueness_policy(
                    org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID);
                policies[4] = containerPOA.create_request_processing_policy(
                    org.omg.PortableServer.RequestProcessingPolicyValue.USE_SERVANT_MANAGER);
                policies[5] = containerPOA.create_implicit_activation_policy(
                    org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);
                // Create a single thread poa
                policies[6] = containerPOA.create_thread_policy(
                    org.omg.PortableServer.ThreadPolicyValue.SINGLE_THREAD_MODEL);
                
                StrategyList poaStrategies = new StrategyList();
                
                String SINGLE_THREAD_HOMES_POA_NAME = "HomeSerializePOA";
                String MULTI_THREAD_HOMES_POA_NAME  = "HomeMultithreadPOA";
                
                homesSerializePOA = ORBSupport.createPOA(
                    containerPOA,
                    SINGLE_THREAD_HOMES_POA_NAME,
                    poaManager, 
                    policies,
                    poaStrategies);
                //
                // Create and install servant activator
                //
                org.omg.PortableServer.ServantActivator theActivator =
                    new HomesServantActivator();
                homesSerializePOA.set_servant_manager(theActivator);
            
                
                // Create a multi thread poa
                policies[6] = rootPOA.create_thread_policy(
                    org.omg.PortableServer.ThreadPolicyValue.ORB_CTRL_MODEL);
                
                homesMultithreadPOA = ORBSupport.createPOA(
                    containerPOA,
                    MULTI_THREAD_HOMES_POA_NAME,
                    poaManager, 
                    policies,
                    poaStrategies);
                //
                // Create and install servant activator
                //
                theActivator = new HomesServantActivator();
                homesMultithreadPOA.set_servant_manager(theActivator);
                
            } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_YES);
            } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_YES);
            } catch (org.omg.CORBA.SystemException se) {
                throw se; // rethrow
            } 
            
                  
            // GET EVENT CHANNEL FACTORY
            
            // Get NamingInterface object from RepositoryInterface
            NamingInterface ni = RepositoryInterface.getDomainNamingInterface(
                Locations.CDMW_SERVICES_NAME_DOMAIN);
        
            // Get a reference on the EventChannelFactory
            obj = ni.resolve("EVENT/FACTORIES/DefaultEventChannelFactory");
            com.thalesgroup.CdmwEvent.EventChannelFactory eventChannelFactory
                = com.thalesgroup.CdmwEvent.EventChannelFactoryHelper.narrow(obj);
        
            if (eventChannelFactory == null) {
                System.err.println("defaultEventChannelFactory is a nil object reference");
                System.exit(-1);
            }
    
    
            // Create a container instance
            ContainerImpl containerImpl = new ContainerImpl(
                orb, 
                containerPOA, 
                homesSerializePOA,
                homesMultithreadPOA,
                compInstall, 
                null,
                null,
                eventChannelFactory,
                PROCESS_NAME,
                APPLICATION_NAME,
                configValues);
            
            org.omg.Components.Deployment.Container container = 
                containerImpl._this(orb);
            String containerIOR = orb.object_to_string(container);
    
            // Create a non Cdmw consumer instance
            NonCdmwStatusInfoConsumerImpl consumerImpl 
                = new NonCdmwStatusInfoConsumerImpl(rootPOA);
            com.acme.Dinner.StatusInfoConsumer consumer = 
                consumerImpl._this(orb);
            String consumerIOR = orb.object_to_string(consumer);
            
            OS.sleep(timescale * 1000);
            System.out.println("server initialised");
            
            // Main loop
            status = run(orb, containerIOR, consumerIOR);
            OS.sleep(timescale * 2000);
            
            if (orb != null) {
                try {
                    // send stop order
                    commands = new java.io.BufferedReader(
                        new java.io.StringReader("stop\n exit \n"));
                    System.out.println("Stopping EventChannelManager...");
                    // admin.run(commands, System.out);
                    if (profileManager != null) { OS.killProcess(profileManager);}
                    RepositoryInterface.finish();
                    OS.sleep(timescale * 2000);
                    orb.destroy();
                } catch(Exception ex) {
                    System.err.println(ex.toString());
                } 
            }        
        
            // And we kill launched process
            try  {
                if (eventServer != null) { OS.killProcess(eventServer);}
                if (naming != null) { OS.killProcess(naming);}
                if (repository != null) { OS.killProcess(repository);}
                OS.sleep(timescale * 2000);
            } catch (InterruptedException ie) {
                System.err.println(ie.toString());
            }
        } catch(Exception ex) {
            ex.printStackTrace();
            if (orb != null) { orb.destroy();}
            if (eventServer != null) { OS.killProcess(eventServer);}
            if (naming != null) { OS.killProcess(naming);}
            if (repository != null) { OS.killProcess(repository);}
            System.err.println(ex.toString());
        }
        
    }

}


