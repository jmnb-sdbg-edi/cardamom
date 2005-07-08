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


package cdmw.tools;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.ORBUtils;
import cdmw.namingandrepository.NamingInterface;
import cdmw.platformmngt.ServiceNames;

/**
 * The simulated repository main class.
 * 
 */
public class SimRepository {

    public static final int SUCCESS = 0;
    public static final int FAILURE = 1;
    
    public static final String REPOSITORY_USER_OBJECT_ID 
        = "repository.simulated";
    public static final String MESSAGE_LOGGER_USER_OBJECT_ID 
        = "message-logger.simulated";
    public static final String PROCESS_CALLBACK_USER_OBJECT_ID 
        = "process-callback.simulated";
    public static final String SERVICE_BROKER_USER_OBJECT_ID 
        = "service-broker.simulated";
    public static final String ENTITY_OBSERVER_USER_OBJECT_ID 
        = "entity-observer.simulated";
    public static final String PROCESS_MESSAGE_BROKER_USER_OBJECT_ID 
        = "process-message-broker.simulated";

    // Binding names
    public static final String REPOSITORY_BINDING_NAME 
        = "CDMW.I/repository.simulated";
    public static final String PROCESS_CALLBACK_BINDING_NAME 
        = "CDMW.I/process-callback.simulated";


    // IOR filenames
    public static final String REPOSITORY_IOR_FILE_NAME 
        = "SimRepository.IOR";
    public static final String PROCESS_CALLBACK_IOR_FILE_NAME 
        = "SimProcessCallback.IOR";

    // Log filenames
    public static final String MESSAGE_LOGGER_LOG_FILE_NAME 
        = "SimPlatformManagement.log";

    // CORBALOC IDs
    public static final String REPOSITORY_CORBALOC_ID 
        = "SimRepository";
    public static final String PROCESS_CALLBACK_CORBALOC_ID 
        = "SimProcessCallback";
    public static final String NAME_SERVICE_ID
        = "NameService";
    
    // NamingContext prefix
    public static final String REPOSITORY_NC_PREFIX 
        = "CDMW.I/NamingAndRepository.simulated";
    public static final String SERVICE_BROKER_NC_PREFIX 
        = "CDMW.I/ServiceBroker.simulated";
    public static final String PROCESS_CALLBACK_NC_PREFIX 
        = "CDMW.I/ProcessCallback.simulated";

    // Default application name
    public static final String DEFAULT_APPLICATION_NAME 
        = "CDMW Test Application";
        
    public static int run(org.omg.CORBA.ORB orb, String[] args) {
            
        try {
            //
            // Get the root POA
            //
            org.omg.CORBA.Object rootObj =
                orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA root 
                = org.omg.PortableServer.POAHelper.narrow(rootObj);
        
            //
            // Create persistent POA
            //
            org.omg.PortableServer.POAManager poaManager = root.the_POAManager();
            org.omg.CORBA.Policy[] policies = new org.omg.CORBA.Policy[2];
            policies[0] = root.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID);
            policies[1] = root.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.PERSISTENT);
            org.omg.PortableServer.POA multithreadPoa 
                = root.create_POA("SimRepository", poaManager, policies);
            
            //
            // Register Repository within the Naming Service
            //
            org.omg.CORBA.Object obj =
                orb.resolve_initial_references("NameService");
            if (obj==null) {
                System.out.println("Could not resolve initial reference \"NameService\"");
                System.exit(1);
            }

            org.omg.CosNaming.NamingContext nc = null;
            try {
                nc = org.omg.CosNaming.NamingContextHelper.narrow(obj);
            } catch (Exception e) {
                System.out.println("Cannot reach the naming service \n"
                    + "Reason: " + e.toString());
                System.exit(1);
            }
            if (nc==null) {
                System.out.println("Invalid reference to a Naming Service");
                System.exit(1);
            }
            
            
            //
            // Create Simulated Repository object
            //
            NamingInterface ni = new NamingInterface(nc);
            NamingInterface repositoryNiBase
                = ni.bindNewContext(REPOSITORY_NC_PREFIX, true);
            org.omg.CosNaming.NamingContext repNc 
                = repositoryNiBase.getNamingContext();

            SimRepositoryImpl rep 
                = new SimRepositoryImpl(orb, multithreadPoa, repNc, args);

            byte[] oid = REPOSITORY_BINDING_NAME.getBytes();
            multithreadPoa.activate_object_with_id(oid, rep);
            com.thalesgroup.CdmwNamingAndRepository.Repository repReference
                = rep._this();

            ni.rebind(REPOSITORY_BINDING_NAME, repReference, false);

            // Bind the Repository to a Corbaloc name
            ORBSupport.bindObjectToCorbaloc( orb,
                REPOSITORY_CORBALOC_ID, repReference );
            
            // Write IOR to file SimRepository.IOR
            String repIor = orb.object_to_string(repReference);
            writeToFile(repIor,REPOSITORY_IOR_FILE_NAME);
            
            // Bind the Default RootContext to corbaloc Id NameService
            // to be accessible by corbaname
            org.omg.CosNaming.NamingContext defaultRootCtxt = rep.resolve_root_context(
                com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);
            ORBSupport.bindObjectToCorbaloc( orb, 
                NAME_SERVICE_ID, defaultRootCtxt );

            // Create Simulated Platform Management Service objects

            //
            // Create a single thread POA for the message logger and the
            // ProcessCallback objects
            //
            policies = new org.omg.CORBA.Policy[3];
            policies[0] = root.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID);
            policies[1] = root.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.PERSISTENT);
            policies[2] = root.create_thread_policy(
                org.omg.PortableServer.ThreadPolicyValue.SINGLE_THREAD_MODEL);
            org.omg.PortableServer.POA singlethreadPoa =
                root.create_POA("SimPlatformManagement", poaManager, policies);

            // Create a MessageLogger object
            MessageLoggerImpl logger = new MessageLoggerImpl(
                singlethreadPoa, MESSAGE_LOGGER_LOG_FILE_NAME);

            oid = MESSAGE_LOGGER_USER_OBJECT_ID.getBytes();
            singlethreadPoa.activate_object_with_id(oid, logger);
            com.thalesgroup.CdmwSimulatedRepository.MessageLogger loggerRef
                = logger._this();

            // Create a ProcessMessageBroker object
            SimProcessMessageBrokerImpl messageBroker 
                = new SimProcessMessageBrokerImpl(multithreadPoa,loggerRef);

            oid = PROCESS_MESSAGE_BROKER_USER_OBJECT_ID.getBytes();
            multithreadPoa.activate_object_with_id(oid, messageBroker);
            com.thalesgroup.CdmwPlatformMngt.ProcessMessageBroker messageBrokerRef
                = messageBroker._this();

            // Create an EntityObserver object
            SimEntityObserverImpl entityObserver 
                = new SimEntityObserverImpl(multithreadPoa,loggerRef);

            oid = ENTITY_OBSERVER_USER_OBJECT_ID.getBytes();
            multithreadPoa.activate_object_with_id(oid, entityObserver);
            com.thalesgroup.CdmwPlatformMngtEntity.EntityObserver entityObserverRef
                = entityObserver._this();
                
            // Create a ServiceBroker object
            NamingInterface serviceBrokerNiBase 
                = ni.bindNewContext(SERVICE_BROKER_NC_PREFIX, true);
            org.omg.CosNaming.NamingContext serviceBrokerNc
                = serviceBrokerNiBase.getNamingContext();

            SimServiceBrokerImpl serviceBroker
                = new SimServiceBrokerImpl(multithreadPoa, serviceBrokerNc, loggerRef);
            
            //
            // Register the Repository as a service within the ServiceBroker
            //
            String sn = ServiceNames.NAMING_AND_REPOSITORY_SERVICE;
            serviceBroker.set_service(sn, repReference);
            
            oid = SERVICE_BROKER_USER_OBJECT_ID.getBytes();
            multithreadPoa.activate_object_with_id(oid, serviceBroker);
            com.thalesgroup.CdmwPlatformMngtService.ServiceBroker serviceBrokerRef 
                = serviceBroker._this();

            // Create a ProcessCallback object
            NamingInterface processCallbackNiBase
                = ni.bindNewContext(PROCESS_CALLBACK_NC_PREFIX, true);
            org.omg.CosNaming.NamingContext processCallbackNc
                = processCallbackNiBase.getNamingContext();
            SimProcessCallbackImpl processCallback
                = new SimProcessCallbackImpl(singlethreadPoa,
                processCallbackNc, messageBrokerRef, entityObserverRef,
                serviceBrokerRef, loggerRef, DEFAULT_APPLICATION_NAME);

            oid = PROCESS_CALLBACK_USER_OBJECT_ID.getBytes();
            singlethreadPoa.activate_object_with_id(oid, processCallback);
            com.thalesgroup.CdmwPlatformMngt.ProcessCallback processCallbackRef
                 = processCallback._this();

            ni.rebind(PROCESS_CALLBACK_BINDING_NAME,processCallbackRef, false);

            // Bind the Repository to a Corbaloc name
            ORBSupport.bindObjectToCorbaloc
                ( orb, PROCESS_CALLBACK_CORBALOC_ID, processCallbackRef );

            // Write IOR to file SimProcessCallback.IOR
            repIor = orb.object_to_string(processCallbackRef);
            writeToFile(repIor,PROCESS_CALLBACK_IOR_FILE_NAME);

            //
            // Run implementation
            //
            poaManager.activate();
            orb.run();

            return SUCCESS;            
            
        } catch (Exception e) {
            e.printStackTrace();
            return FAILURE;
        }
        
    }
    
    public static void main(String[] args) {
    
        int status = SUCCESS;
        org.omg.CORBA.ORB orb = null;

        try {
            orb = org.omg.CORBA.ORB.init(args, ORBUtils.getProperties());
            status = run(orb, args);
        } catch(Exception e) {
            e.printStackTrace();
            status = FAILURE;
        }

        if(orb!=null) {
            try {
                orb.destroy();
            } catch(Exception e) {
                e.printStackTrace();
                status = FAILURE;
            }
        }

        if(status==SUCCESS) {
            // that's good !
        } else {
            System.out.println("SimRepository execution failure.");
        }
    
    }
    
    public static void writeToFile(String ior,String filename) {
    
        java.io.File file = new java.io.File(filename);
        String s;

        try {
            java.io.PrintWriter out = new java.io.PrintWriter(
                new java.io.BufferedWriter( new java.io.FileWriter(file) ) );
            out.println(ior);
            out.close();
        } catch(java.io.IOException e) {}
    
    }
    
}

