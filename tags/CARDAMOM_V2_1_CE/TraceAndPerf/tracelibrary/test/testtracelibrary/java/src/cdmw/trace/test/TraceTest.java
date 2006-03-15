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


package cdmw.trace.test;


import org.omg.CosNaming.NamingContext;
import org.omg.CosNaming.NamingContextHelper;

import com.thalesgroup.CdmwNamingAndRepository.Repository;
import com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper;
import com.thalesgroup.CdmwNamingAndRepository.NameDomain;
import com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT;
import com.thalesgroup.CdmwTrace.TraceProducer;
import com.thalesgroup.CdmwTrace.TraceProducerPackage.CollectorData;
import com.thalesgroup.CdmwTrace.Collector;
import com.thalesgroup.CdmwTrace.CollectorHelper;
import com.thalesgroup.CdmwTrace.ALL_COMPONENT_NAMES;
import com.thalesgroup.CdmwTrace.ALL_DOMAINS;
import com.thalesgroup.CdmwTrace.ALL_VALUES;

import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.trace.FlushAreaMngr;



public class TraceTest /*extends cdmw.testutils.Testable*/ {
    
    private static final String DEFAULT_COMMAND_LIST = 
          "verbose\n"
        + "start_flushing\n"
        + "start_stop_targets\n"
        + "sleep_process 10000\n"
        +  "x\n";

        
    private static final int SUCCESS = 0;
    private static final int FAILURE = 1;    
    
    private static final int POA_THREAD_POOL_SIZE = 5;    
    
    private static final String APPLICATION_NAME_OPTION = "--application-name";
    private static final String NAMING_REPOS_URL_OPTION = "--namingRepos-url";    
    private static final String INTERACTIVE_OPTION      = "--interactive";
    
    private static final String COLLECTOR_INI_MNEMONAME = "collector_ini_";

     
    private String  processName;
    private boolean interactiveMode;
    private String  applicationName;
    private String  namingReposURL;   
    
    private org.omg.CORBA.ORB orb;
    private Repository repository;    
    
    
    
    public TraceTest(String  processName,
                     boolean interactiveMode,
                     String  applicationName,
                     String  namingReposURL)
    {
        this.processName     = processName;
        this.interactiveMode = interactiveMode;
        this.applicationName = applicationName;
        this.namingReposURL  = namingReposURL;
        
        orb = null;
        repository = null;
    }    
    
    
    
    private int orbInit(String[] args) {     
        try {
            // Initialize the ORB
            cdmw.orbsupport.StrategyList orbStrategies = 
                new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            
            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies, "");
            return SUCCESS;
            
        } catch (cdmw.common.BadParameterException e) {
            System.err.println("ERROR in strategyList.addPOAThreadPool()\n"
                               + POA_THREAD_POOL_SIZE + " is a bad parameter");
            e.printStackTrace();
            return FAILURE;
        }
    }
          
    private int orbEnd() {
        if (orb != null) {
            try {
                orb.destroy();
            } catch (org.omg.CORBA.SystemException e) {
                e.printStackTrace(System.err);
                return FAILURE;
            }
        }
        return SUCCESS;
    }
          
          
    
    private int processInit() {
        TraceProducer traceProducer;
        
        try {
            // Retrieve Repository object reference
            if (getCdmwRepository() == FAILURE) {
                return FAILURE;   
            }
            
            // set trace collector location under default root context
            // Pattern is :
            //    CDMW/SERVICES/TRACE/COLLECTORS/"
            String collectorDomainStr;
            collectorDomainStr  = cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN;
            collectorDomainStr += "/";
            collectorDomainStr += cdmw.common.Locations.TRACE_COLLECTORS_NAME_DOMAIN;
            
            // Check that collector domain name is really a Name Domain
            NameDomain collectorDomain = 
                repository.resolve_name_domain(collectorDomainStr);
                    
            // get default root context
            NamingContext rootContext = 
                repository.resolve_root_context(DEFAULT_ROOT_CONTEXT.value);
        
            // It is a name domain, so we should be able to get 
            // its naming context
            NamingInterface niRoot = new NamingInterface(rootContext);
            
            // create Collector domain naming context
            org.omg.CORBA.Object obj = niRoot.resolve(collectorDomainStr);
            NamingContext collectorDom = NamingContextHelper.narrow(obj);
                
            // create the initial collector list
            CollectorData[] collectorList = getCollectorList(collectorDom);
                
            // get a reference to the root POA
            org.omg.CORBA.Object rootObj = orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA = 
                org.omg.PortableServer.POAHelper.narrow(rootObj);
        
            // create and initialize the FlushArea manager
            traceProducer = FlushAreaMngr.init(rootPOA,
                                               collectorList,
                                               ALL_COMPONENT_NAMES.value, // ECR-0123
                                               ALL_DOMAINS.value,
                                               ALL_VALUES.value,
                                               applicationName,
                                               processName);
            
            // export the object reference to a file
            String traceProducerIor = orb.object_to_string(traceProducer);
            java.io.FileOutputStream file =
                new java.io.FileOutputStream(ClientThread.TRACE_LIBRARY_IOR_FILE);
            java.io.PrintWriter out = new java.io.PrintWriter(file);
            out.println(traceProducerIor);
            out.flush();
            file.close();          
        
            // activate the POA Manager
            org.omg.PortableServer.POAManager POAMngr = rootPOA.the_POAManager();
            POAMngr.activate();
    
            // Register TraceProducer object
            return registerTraceProducer(traceProducer);
        
        } catch (cdmw.common.BadParameterException e) {
            e.printStackTrace(System.err);
            return FAILURE;
        } catch (cdmw.common.InternalErrorException e) {
            e.printStackTrace(System.err);
            return FAILURE;
            
//        } catch (java.io.FileOutputStream e) {
//            e.printStackTrace(System.err);
//            return FAILURE;
        } catch (java.io.IOException e) {
            e.printStackTrace(System.err);
            return FAILURE;
        } catch (org.omg.CORBA.SystemException e) {
            e.printStackTrace(System.err);
            return FAILURE;
        } catch (org.omg.CORBA.UserException e) {
            e.printStackTrace(System.err);
            return FAILURE;
        }
        
           
    }
    
    
    private void processEnd() {
        // clean up the flush area manager
        FlushAreaMngr.cleanup();
        
        // set TraceProducer location under admin root context
        // Pattern is :
        //   "<host_name>/<application_name>/<process_name>/TRACE/TraceProducer"
        
        // set the name of the Trace Producer context            
        String traceProducerName;
        traceProducerName  = cdmw.ossupport.OS.getHostname();
        traceProducerName += "/";
        traceProducerName += applicationName;
        traceProducerName += "/";
        traceProducerName += processName;
        traceProducerName += "/TRACE/TraceProducer";

        try {

            // get "AdminRootContext" root context and then
            // unregister TraceProducer object
            NamingContext adminRoot = repository.resolve_root_context(
                cdmw.common.Locations.ADMIN_ROOT_CONTEXT_ID);
                
            // create naming interface on admin root context
            NamingInterface ni = new NamingInterface(adminRoot);
            
            // remove the trace producer binding
            ni.unbind(traceProducerName);
        
        } catch (com.thalesgroup.CdmwNamingAndRepository.
                 RepositoryPackage.NoRootContext e) {
            System.err.println("Trace Producer Root Context does not exist");
            e.printStackTrace(System.err);
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
            System.err.println("Trace Producer Name context not found \n" 
                               + traceProducerName);
            e.printStackTrace(System.err);
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
            System.err.println("Trace Producer Name context cannot proceed");
            e.printStackTrace(System.err);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
            System.err.println("Trace Producer Name invalid \n" 
                               + traceProducerName);
            e.printStackTrace(System.err);
        } catch (org.omg.CORBA.SystemException e) {
            e.printStackTrace(System.err);
        } catch (org.omg.CORBA.UserException e) {
            e.printStackTrace(System.err);
        }
        
            
            
                        
    }
    
    
    private int getCdmwRepository() {
        org.omg.CORBA.Object obj;

        obj = orb.string_to_object(namingReposURL);
        if (obj == null) {
            System.err.println("Naming & Repository URL denotes a nil object reference");
            return FAILURE;
        }
        
        repository = com.thalesgroup.CdmwNamingAndRepository.
                     RepositoryHelper.narrow(obj);
        if (repository == null) {
            System.err.println("Couldn't retrieve reference of the Repository");
            return FAILURE;
        }
        
        return SUCCESS;
    }
    

    private CollectorData[] getCollectorList(NamingContext collectorDomain)
    {
        java.util.Vector collectorVector = new java.util.Vector();
        NamingInterface niCollector = new NamingInterface(collectorDomain);
        org.omg.CosNaming.BindingIteratorHolder bindingItr = 
            new org.omg.CosNaming.BindingIteratorHolder();
        org.omg.CosNaming.BindingListHolder bindingList = 
            new org.omg.CosNaming.BindingListHolder();
        
        
        
        // All binding in this list are Collector, so we try to get all
        niCollector.list(NamingInterface.LIST_CHUNK_SIZE,
                         bindingList,
                         bindingItr);
                         
        // We stop when there is no more binding
        boolean isMoreBinding = false;
        int collectorIdx = 0;
        do {
            for (int i = 0; i < bindingList.value.length; i++) {
                org.omg.CosNaming.Binding binding = bindingList.value[i];

                String collectorName = "";                
                try {
                    // We call each Collector, and we notify them of our creation
                    collectorName = niCollector.toString(binding.binding_name);
                    org.omg.CORBA.Object obj = niCollector.resolve(collectorName);
                    com.thalesgroup.CdmwTrace.Collector collector = 
                        com.thalesgroup.CdmwTrace.CollectorHelper.narrow(obj);

                    // create collector mnemonic name
                    String mnemoName = COLLECTOR_INI_MNEMONAME + collectorIdx;
                    collectorIdx++;
                    
                    // create collector data structure
                    com.thalesgroup.CdmwTrace.
                    TraceProducerPackage.CollectorData collectorData =
                        new com.thalesgroup.CdmwTrace.
                            TraceProducerPackage.CollectorData();
                    collectorData.the_collectorMnemoName = mnemoName;
                    collectorData.the_collectorObjName = collectorName;
                    collectorData.the_collector = collector;
                    
                    // insert collector data in vector
                    collectorVector.add(collectorData);


                } catch (cdmw.commonsvcs.naming.InvalidNameException e) {
                    // should not happen!
                    e.printStackTrace(System.err);
                } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                    // Should not happen!
                    System.err.println(  "Trace Collector not found \n"
                                       + collectorName);
                    e.printStackTrace(System.err);
                } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                    // Should not happen!
                    System.err.println("Unexpected Error (CannotProceed exception)!");
                    e.printStackTrace(System.err);
                } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                    System.err.println(  "Trace Producer Name invalid \n"
                                       + collectorName);
                    e.printStackTrace(System.err);
                }
                
                                    
            }
            
            // If the iterator is not NULL, there is more binding to get back
            if (bindingItr.value != null) {
                isMoreBinding = bindingItr.value.next_n(
                    NamingInterface.LIST_CHUNK_SIZE,
                    bindingList);
            }
            
        } while (isMoreBinding);
            
        // transform CollectorData Vector in CollectorData[]
        CollectorData[] collectorList = new CollectorData[collectorVector.size()];
            
        for (int i = 0; i < collectorList.length; i++) {
            collectorList[i] = (CollectorData) collectorVector.elementAt(i);
        }

        return collectorList;
    }



    private int registerTraceProducer(TraceProducer traceProducer) {
        String traceProducerName;

        // set TraceProducer location under admin root context
        // Pattern is :
        //   "<host_name>/<application_name>/<process_name>/TRACE/TraceProducer"
        
        // Build the name of the Trace Producer object
        traceProducerName  = cdmw.ossupport.OS.getHostname();
        traceProducerName += "/";
        traceProducerName += applicationName;
        traceProducerName += "/";
        traceProducerName += processName;
        traceProducerName += "/TRACE/TraceProducer";
        
        // get "AdminRootContext" root context and then
        // register TraceProducer object
        try {
            NamingContext adminRoot = repository.resolve_root_context(
                cdmw.common.Locations.ADMIN_ROOT_CONTEXT_ID);
            
            // create naming interface on admin root context
            NamingInterface ni = new NamingInterface(adminRoot);
            
            // If object already registered, force its registration
            ni.bind(traceProducerName, traceProducer, true);
            
            return SUCCESS;
            
        } catch (com.thalesgroup.CdmwNamingAndRepository.
                 RepositoryPackage.NoRootContext e) {
            System.err.println("Trace Producer Root Context does not exist");
            e.printStackTrace(System.err);
        } catch (org.omg.CosNaming.NamingContextPackage.AlreadyBound e) {
            // Should not happen because force is set to true!
            System.err.println("Trace Producer already bound");
            e.printStackTrace(System.err);            
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
            System.err.println("Trace Producer Name context cannot proceed");
            e.printStackTrace(System.err);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
            System.err.println("Trace Producer Name invalid \n" 
                               + traceProducerName);
            e.printStackTrace(System.err);
        } catch (org.omg.CORBA.SystemException e) {
            e.printStackTrace(System.err);
        } catch (org.omg.CORBA.UserException e) {
            e.printStackTrace(System.err);
        }
    
        return FAILURE;    
    }
    
    private int run() {
        int status = SUCCESS;
        
        ClientThread client;
        if (!interactiveMode) {
            // Do all the tests
            String command = DEFAULT_COMMAND_LIST;
            
            // convert string command to stream
            java.io.ByteArrayInputStream is = 
                new java.io.ByteArrayInputStream(command.getBytes());
            
            // create client thread object
            client = new ClientThread(orb, 
                                      is,
                                      System.out,
                                      repository);

        } else {
            // create client thread object in interactive mode
            client = new ClientThread(orb, 
                                      System.in,
                                      System.out,
                                      repository);
        }
        
        // start client and run orb
        client.start();
        orb.run();
        
        // wait end of client thread after corba loop stopping
        try {
            client.join();
        } catch (InterruptedException e) {
            // ignore
        }
        
        return status;
    }
    
    
    
    
    
    public static void printUsage(java.io.PrintStream os, String progName) {
        os.println(  "Usage:\n"
                   + progName + " options\n\n"
                   + "Options:\n"
                   + "  --help                     Display this information.\n"
                   + "  --interactive              Start in interactive mode.\n"
                   + " \n"
                   + "  --application-name=<name>  Name of application.\n"
                   + "  --namingRepos-url=<URL>    URL of the CDMW Naming and Repository service.\n"
                  );
    }
    
    public static void main (String[] args) { 
        
                      
        // get arguments
        if (args.length == 0) {
            printUsage(System.out, "java cdmw.trace.test.TraceTest");
            return;
        }
        
        // help argument
        if (args[0].compareTo("--help") == 0) {
            printUsage(System.out, "java cdmw.trace.test.TraceTest");
            return;               
        }
        
        // ---- Does it use interactive mode? ----
        boolean interactiveMode;
        String value = cdmw.ossupport.OS.getOptionValue(args, INTERACTIVE_OPTION);
        if (value.compareTo("yes") == 0) {
            interactiveMode = true;
        } else if (value.compareTo("no") == 0) {
            interactiveMode = false;
        } else {
            System.out.println("Error with argument " + INTERACTIVE_OPTION);
            printUsage(System.out, "java cdmw.trace.test.TraceTest");
            return;
        }
        // set Process Name
        String processName = "TraceTest";
        
        // get Application Name
        String applicationName = 
            cdmw.ossupport.OS.getOptionValue(args, APPLICATION_NAME_OPTION);
        if (applicationName.compareTo("no") == 0) {
            System.out.println(APPLICATION_NAME_OPTION +
                               "=<name> option must be defined");
            printUsage(System.out, "java cdmw.trace.test.TraceTest");
            return;            
        }
        
        // get Naming & Repository URL argument
        String namingReposURL = 
            cdmw.ossupport.OS.getOptionValue(args, NAMING_REPOS_URL_OPTION);
        if (namingReposURL.compareTo("no") == 0) {
            System.out.println(NAMING_REPOS_URL_OPTION +
                               "=<URL> option must be defined");
            printUsage(System.out, "java cdmw.trace.test.TraceTest");
            return;            
        }

        // New TraceTest
        TraceTest test = new TraceTest(processName,   
                                       interactiveMode,
                                       applicationName,
                                       namingReposURL);
        int status;

        // Initialize the ORB                                       
        status = test.orbInit(args);
        
        // Initialize processing
        if (status == SUCCESS) {
            status = test.processInit();
        }
                
        // Run processing
        if (status == SUCCESS) {
            status = test.run();
        }

        // End Processing
        if (status == SUCCESS) {
            test.processEnd();
        }

        // end of corba processing
        test.orbEnd();
        
    }
    
    
}



 

