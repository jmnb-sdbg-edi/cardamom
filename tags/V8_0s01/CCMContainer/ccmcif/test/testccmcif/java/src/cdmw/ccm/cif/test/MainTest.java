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
import java.io.IOException;

import cdmw.testutils.Testable;
import cdmw.common.Options;
import cdmw.ossupport.OS;
import cdmw.ossupport.FileUtilities;
import cdmw.ossupport.ThreadedProcess;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.ccm.container.HomeAllocator;
import cdmw.ccm.container.HomeAllocatorRegistry;
import cdmw.ccm.componentserver.ComponentServerImpl;
import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ComponentKindValueHelper;

public class MainTest {

    public static final String PROCESS_NAME = "DummyProcessName";
    public static final String APPLICATION_NAME = "DummyApplicationName";
      
    public static final int SUCCESS = 0;
    public static final int FAILURE = 1;  

    public static final int POA_THREAD_POOL_SIZE = 5;

    public static class ComponentInstallationPlugImpl 
        extends com.thalesgroup.CdmwDeployment.ComponentInstallationPOA {

        private org.omg.PortableServer.POA poa;

        public ComponentInstallationPlugImpl(org.omg.PortableServer.POA  poa) {
            this.poa = poa;
        }

        public org.omg.PortableServer.POA _default_POA() {
            return this.poa;
        }

        //
        // IDL:omg.org/Components/Deployment/ComponentInstallation/install:1.0
        //
        public void install(String implUUID, String componentLoc)
            throws org.omg.Components.Deployment.InvalidLocation,
                org.omg.Components.Deployment.InstallationFailure {}
    
        //
        // IDL:omg.org/Components/Deployment/ComponentInstallation/replace:1.0
        //
        public void replace(String implUUID, String componentLoc)
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
                org.omg.Components.Deployment.InstallationFailure  {

            String CLASSPATH=System.getProperty("java.class.path");
            String filePath="";
            StringTokenizer st = new StringTokenizer(CLASSPATH,":");
            while (st.hasMoreTokens()) {
                String token = st.nextToken();
                
                if(token.endsWith("libcdmwBasicDemo.jar"))
                    {
                        filePath = token;              
                    }
            }  
            System.out.println(filePath);
            return filePath;
        };
    
        //
        // IDL:thalesgroup.com/CdmwDeployment/ComponentInstallation/get_valuetypefactory_location:1.0
        //
        public String 
            get_valuetypefactory_location(String implUUID, String repid)
            throws org.omg.Components.Deployment.UnknownImplId,
                com.thalesgroup.CdmwDeployment.UnknownValuetype,
                org.omg.Components.Deployment.InstallationFailure {
            return "";
        };

    }
    
    public static org.omg.PortableServer.POA
        createContainerPOA(org.omg.PortableServer.POA parent) 
        throws org.omg.PortableServer.POAPackage.AdapterAlreadyExists,
            org.omg.PortableServer.POAPackage.InvalidPolicy {
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
    
    public static void writeToFile(String ior,String filename) {
        System.out.println(ior + " " + filename);
        java.io.File file = new java.io.File(filename);
        String s;
        try {
            java.io.PrintWriter out = new java.io.PrintWriter(
                new java.io.BufferedWriter( new java.io.FileWriter(file) ) );
            out.println(ior);
            out.close();
        } catch(java.io.IOException e) {System.err.println("---" + e.toString());}
        
    }

    public static String readFromFile(String file) {

        try {
            java.io.BufferedReader in = new java.io.BufferedReader(
                new java.io.FileReader(file) );
            String result = in.readLine();
            //in.close();
            return result;
        }
        catch(IOException ioe) {
            return null;
        }
    }

    public static int run(org.omg.CORBA.ORB orb, 
                          String containerIORClient, 
                          String containerIORServer, 
                          ThreadedProcess serverp) {
        ClientThread client = new ClientThread(orb, containerIORClient, containerIORServer);
    
        client.start();
        orb.run();
        try {
            client.join();
        } catch(InterruptedException ie) {}
    
        OS.killProcess( serverp );
        return SUCCESS;
    }



    
    public static void main(String[] args) {

        int status = SUCCESS;
        org.omg.CORBA.ORB orb = null;
    
        try {

            boolean client = false;
            if ( OS.getOptionValue( args, "--client").equals("yes") ) 
                client = true;
            else
                client = false;
                

            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
    
            String[] serverArgs = new String[0];
            orb = ORBSupport.ORBInit(serverArgs, orbStrategies);
    
            // Get the root POA 
            org.omg.CORBA.Object obj = 
                orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA = 
                org.omg.PortableServer.POAHelper.narrow(obj);
            
            // Run implementation (Activate the POA manager)
            rootPOA.the_POAManager().activate();
     
    
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
    
            ThreadedProcess serverp = null;
            if (!client)
                {
                    String classname = "cdmw.ccm.cif.test.MainTest --client";
                    try {
                        serverp = OS.createJavaProcess(classname);
                        OS.sleep(3000);
                    } catch(IOException ioe) {
                        throw new InterruptedException("Unable to create server process.");
                    }    
                    
                }
            
            HomeAllocator allocator = null;
            if(!client)
                {
                    // Register HomeAllocator
                    allocator  = new HomeAllocator(
                                                   "Cdmw.CCM.CIF.CdmwBasicDemo.SessionServerHome_impl");
                    HomeAllocatorRegistry.register(
                                                   "Cdmw.CCM.CIF.CdmwBasicDemo.SessionServerHome_impl", 
                                                   allocator);
                }
            else
                {
                    allocator  = new HomeAllocator(
                                                   "Cdmw.CCM.CIF.CdmwBasicDemo.SessionClientHome_impl");
                    HomeAllocatorRegistry.register(
                                                   "Cdmw.CCM.CIF.CdmwBasicDemo.SessionClientHome_impl", 
                                                   allocator);
                }



            ComponentInstallationPlugImpl compInstallPlug
                = new ComponentInstallationPlugImpl(rootPOA);
            com.thalesgroup.CdmwDeployment.ComponentInstallation compInstall
                = compInstallPlug._this(orb);
    
            org.omg.Components.ConfigValue[] configValues = 
                new org.omg.Components.ConfigValue[1];
            
            org.omg.CORBA.Any value = orb.create_any();
            com.thalesgroup.CdmwDeployment.ComponentKindValueHelper.insert(
                value, 
                com.thalesgroup.CdmwDeployment.ComponentKindValue.SESSION);
            configValues[0] = new cdmw.ccm.deployment.ConfigValueImpl(
                com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value, 
                value);
    
            org.omg.PortableServer.POA containerPOA = createContainerPOA(rootPOA);

            
            org.omg.Components.HomeRegistration homeRegistration = null;
            com.thalesgroup.CdmwEvent.EventChannelFactory noECF = null;
            org.omg.Components.ConfigValue[] cfgCompserver =
                new org.omg.Components.ConfigValue[0];
            ComponentServerImpl cs = null;
            if (client)
                {
                    cs = new ComponentServerImpl(orb,
                                                 containerPOA,
                                                 homeRegistration,
                                                 noECF,
                                                 PROCESS_NAME + "1",
                                                 APPLICATION_NAME,
                                                 cfgCompserver);
                }
            else
                {
                    cs = new ComponentServerImpl(orb,
                                                 containerPOA,
                                                 homeRegistration,
                                                 noECF,
                                                 PROCESS_NAME + "2",
                                                 APPLICATION_NAME,
                                                 cfgCompserver);
                }



            org.omg.PortableServer.Servant serv = cs;
            org.omg.Components.Deployment.ComponentServer theComponentServer = cs._this(orb);
            cs.set_component_installation(compInstall);

//             // Create a container instance
            org.omg.Components.Deployment.Container container = 
                theComponentServer.create_container(configValues);
            
            String containerIORClient = new String();
            String containerIORServer = new String();
            if (client)
                {
                    writeToFile(orb.object_to_string(container), "Client.ior");
                }
            else
                {
                    containerIORClient = readFromFile("Client.ior");
                    OS.unlink("Client.ior");
                    containerIORServer = orb.object_to_string(container);
                    System.out.println("client: " + containerIORClient);
                    System.out.println("server: " + containerIORServer);
                }
            
            int timescale = Testable.getTimescale();
            OS.sleep(timescale * 1000);
            if (client)
                {
                    System.out.println("Client initialised");
                    orb.run();
                }
            else
                {
                    System.out.println("Server initialised");
                    status = run(orb, containerIORClient, containerIORServer, serverp);

                }

            OS.sleep(timescale * 2000);
            
        } catch(Exception e) {
            System.err.println(e.toString());
            status = FAILURE;
        }
        
        if (orb != null) {
            try {
                orb.destroy();
            } catch(Exception e) {
                System.err.println(e.toString());
                status = FAILURE;
            } 
        }
        
        if (status == FAILURE) {
            System.out.println("Main test failed.");
        } else {
            System.out.println("Main test succeeded.");
        }
    	System.exit(0);
}

}

