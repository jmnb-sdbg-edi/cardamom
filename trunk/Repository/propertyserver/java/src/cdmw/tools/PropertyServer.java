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


package cdmw.tools;

import cdmw.common.Assert;
import cdmw.common.Options;

import cdmw.testutils.Testable;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ORBUtils;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.platformmngt.ServiceNames;
import cdmw.platformmngt.PlatformInterface;

public class PropertyServer {

    public static final String PROP_SERVICE_ID = "PropertyService";
    public static final String REPOSITORY_PORT_OPTION = "--RepPort";
    public static final String PROPERTY_PORT_OPTION = "--PropPort";
    public static final String PROPERTY_SET_OPTION = "--CreatePropertySet";
    
    public static final int POA_THREAD_POOL_SIZE = 5;
    
    public static void usage() {
        System.out.println("Usage:\n"
            + "java cdmw.tools.PropertyServer options\n"
            +"\n"
            +"Options:\n"
            +"--help                     Display this information.\n"
            +"\n"
            +"[--RepPort=<nbr>]          Port of Name and Repository.\n"
            +"[--PropPort=<nbr>]         Port of Property Service.\n"
            +"[--CreatePropertySet]      Create a PropertySet for LifeCycle test usage");
    }

    public static org.omg.CosPropertyService.PropertySetDefFactory
        getPropertySetDefFactory(org.omg.CORBA.ORB orb, String url)
        throws org.omg.CORBA.TRANSIENT {
        
        Assert.check(orb != null);
    
        org.omg.CORBA.Object obj = null;
        //
        // Get naming service
        //
        try {
            obj = orb.string_to_object(url);
        } catch(Exception e) {
            System.err.println("Invalid PropertyService URL");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        
        if (obj == null) {
            System.err.println("PropertyService URL denotes a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        
        // Get reference to the property service interface
        org.omg.CosPropertyService.PropertySetDefFactory prop = null;
        try {
            prop = org.omg.CosPropertyService.PropertySetDefFactoryHelper.narrow(obj);
            if (prop == null) {
                System.err.println("Couldn't retrieve reference of the PropertyService");
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENT,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            System.err.println("Couldn't retrieve reference of the PropertyService");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENT,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        return prop;

    }

    public static void installPropertySetDef(
        org.omg.CORBA.ORB orb, 
        org.omg.CosPropertyService.PropertySetDefFactory propfac,
        com.thalesgroup.CdmwNamingAndRepository.Repository rep) {
        
        // Create a Propertyset
        org.omg.CosPropertyService.PropertySetDef set = 
            propfac.create_propertysetdef();
        
        // Command is "ln ior CDMW/SERVICESPropertySet.object\n x \n");
        String command = "ln " + orb.object_to_string(set)
            + " CDMW/SERVICES/PropertySet.object\n x \n";
        
        RepositoryAdmin adm = new RepositoryAdmin(orb, rep);
        
        java.io.BufferedReader reader = new java.io.BufferedReader(
            new java.io.StringReader(command));
        adm.run(reader, System.out);
        
    }

    public static class RunThread extends Thread  {
    
        private org.omg.CORBA.ORB orb;
        private org.omg.CosPropertyService.PropertySetDefFactory propsetdefFactory;
        private com.thalesgroup.CdmwNamingAndRepository.Repository repositoryRef;
    
        public RunThread(
            org.omg.CORBA.ORB orb,
            org.omg.CosPropertyService.PropertySetDefFactory propsetdefFactory,
            com.thalesgroup.CdmwNamingAndRepository.Repository repositoryRef) {
            
            this.orb = orb;
            this.propsetdefFactory = propsetdefFactory;
            this.repositoryRef = repositoryRef;
        }
        
        public void run() {
            installPropertySetDef(orb, propsetdefFactory, repositoryRef);
        }
        
    }

    /**
     * Default Process Behaviour
     *
     */
    public static class MyProcessBehaviour 
        extends cdmw.platformmngt.ProcessBehaviour {
        
        private org.omg.CORBA.ORB orb;
        private org.omg.CosPropertyService.PropertySetDefFactory propsetdefFactory;
        private boolean createpropset;

        public MyProcessBehaviour(
            org.omg.CORBA.ORB orb,
            org.omg.CosPropertyService.PropertySetDefFactory propsetdefFactory) {
            
            this(orb, propsetdefFactory, false);
            
        }

        public MyProcessBehaviour(
            org.omg.CORBA.ORB orb,
            org.omg.CosPropertyService.PropertySetDefFactory propsetdefFactory,
            boolean createpropset) {
            
            this.orb = orb;
            this.propsetdefFactory = propsetdefFactory;
            this.createpropset = createpropset;

        }

        public void initialise(com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {
            
            System.out.println("INFO: cdmw_property_server init");

            if (createpropset) {

                com.thalesgroup.CdmwNamingAndRepository.Repository repositoryRef = null;
                
                // get the repository service
                try {
                    org.omg.CORBA.Object narObj = 
                        PlatformInterface.getService(ServiceNames.NAMING_AND_REPOSITORY_SERVICE);
                    repositoryRef = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(narObj);
                } catch (com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound snf) {
                    // Cannot proceed with no CDMW Repository Ior
                    System.err.println("Couldn't retrieve reference of the Repository : Service not found");
                    throw new org.omg.CORBA.BAD_PARAM(
                        ExceptionMinorCodes.BAD_PARAMRepositoryNotFound,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                } catch (com.thalesgroup.CdmwPlatformMngtService.ServiceUnavailable su) {
                    // Cannot proceed with no CDMW Repository Ior
                    System.err.println("Couldn't retrieve reference of the Repository : Service not available");
                    throw new org.omg.CORBA.BAD_PARAM(
                        ExceptionMinorCodes.BAD_PARAMCannotGetRepositoryReference,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                } catch (org.omg.CORBA.SystemException se) {
                    System.err.println("Couldn't retrieve reference of the Repository : System Exception raised");
                    throw se;
                } catch (Exception e) {
                    System.err.println("Couldn't retrieve reference of the Repository");
                    throw new org.omg.CORBA.TRANSIENT(e.toString());
                }
                
                RunThread runThread = new RunThread(this.orb, this.propsetdefFactory, repositoryRef);
                runThread.start();
            }
        }

        public void run()
            throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun {
            System.out.println("INFO: cdmw_property_server run");
        }
        
        public void stop() {
            System.out.println("INFO: cdmw_property_server stop");
            this.orb.shutdown(false);
        }
        
    }

    public static void main(String[] args) {

        org.omg.CORBA.ORB orb = null;
        
        //#if CDMW_ORB_VDR == orbacus
        ThreadedProcess propServer = null;
        //#endif

        String help = OS.getOptionValue(args, "--help");
        if ( help.equals("yes") )  {
            usage();
        }

        // get Repository port
        String repositoryPort = OS.getOptionValue(args, REPOSITORY_PORT_OPTION);
        if ( repositoryPort.equals("no") ) {
            repositoryPort = "5020";
        }

        // get Property service port
        String propServPort = OS.getOptionValue(args, PROPERTY_PORT_OPTION);
        if ( propServPort.equals("no") ) {
            propServPort = "5015";
        }

        // get Property service port
        String createPropSet = OS.getOptionValue(args, PROPERTY_SET_OPTION);

        String rootPoaPortOption = 
            OS.getOptionValue(args, Options.LOCALISATION_SERVICE_OPTION);

        // We must ensure to be listening on a host and a port known to the client
        String[] strOpt = null;

        //#if CDMW_ORB_VDR == orbacus
        if ( rootPoaPortOption.equals("no") ) {
            // no localisation port defined
            // Nothing special to do! Just copy args
            strOpt = new String[args.length];
            for (int i=0; i<args.length; i++) strOpt[i] = args[i];

        } else {
            // a localisation port is defined ! It is becomes the propserv port
            propServPort = rootPoaPortOption;

            // and we should remove the option from the args!
            int j = 0;
            strOpt = new String[args.length - 1];
            for (int i=0; i<args.length; i++) {
                // We check if in the current argument we have the option
                if ( !args[i].startsWith(Options.LOCALISATION_SERVICE_OPTION) ) {
                    // no localisation port, copy argument
                    strOpt[j++] = args[i];
                }
            }
        }
        //#endif

        try {        
            // Initialize the ORB
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            orb = ORBSupport.ORBInit(strOpt, orbStrategies);

            org.omg.CORBA.Object poaObj = orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA poa = org.omg.PortableServer.POAHelper.narrow(poaObj);
            org.omg.PortableServer.POAManager poaManager = poa.the_POAManager();
            poaManager.activate();

            boolean platformManaged 
                = PlatformInterface.isLaunchedByPlatformManagement(args);

            // Starting Orbacus Propserver
            System.out.println("Starting Orbacus propserv on port " + propServPort + "...");

            if (platformManaged) {
                // start the Orbacus Property Server in a different process
                propServer = ORBUtils.startPropertyService(propServPort);
            } else {
                // start the Orbacus Property Server in a separate thread
                String[] arguments = { "-OAport", propServPort };
                ORBUtils.startPropertyService(arguments);
            }

            int timescale = Testable.getTimescale();
            OS.sleep(timescale * 2000);

            System.out.println("Propserv started");
            //#endif

            System.out.println("Property Set Factory's corbaloc name: " + PROP_SERVICE_ID);
            System.out.println("Property Set Factory's corbaloc port: " + propServPort);

            if (platformManaged) {
                // This has been launched by CDMW Plateform Management
                poaManager.activate();

                // initialise the platform interface
                PlatformInterface.setup(orb, args);

                //#if CDMW_ORB_VDR == orbacus        
                String propUrl = "corbaloc::localhost:" + propServPort + "/PropertyService";
                org.omg.CosPropertyService.PropertySetDefFactory propsetdefFactory = 
                    getPropertySetDefFactory(orb, propUrl);
                //#endif
                
                // Create a Process Behaviour
                // acknowledge the creation of the process
                if ( createPropSet.equals("yes") ) {
                    PlatformInterface.acknowledgeCreation(
                        new MyProcessBehaviour(orb, propsetdefFactory, true));
                } else {
                    PlatformInterface.acknowledgeCreation(
                        new MyProcessBehaviour(orb, propsetdefFactory));
                }

                orb.run();
            } else {
                // This is launched for unit test with Simrepository

                String repositoryCorbaloc 
                    = "corbaloc:iiop:localhost:" + repositoryPort + "/SimRepository";

                try {
                    if ( createPropSet.equals("yes") ) {
                        com.thalesgroup.CdmwNamingAndRepository.Repository repositoryRef = null;

                        // get repository from url

                        // Get reference to the repository interface
                        try  {
                            org.omg.CORBA.Object obj = orb.string_to_object(repositoryCorbaloc);

                            if (obj == null) {
                                System.err.println("URL denotes a nil object reference");
                                throw new org.omg.CORBA.TRANSIENT(
                                    ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                            }
                            repositoryRef = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj);

                            if (repositoryRef == null) {
                                System.err.println("Couldn't retrieve reference of the Repository");
                                throw new org.omg.CORBA.TRANSIENT(
                                    ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                            }
                        } catch (Exception e) {
                            System.err.println("Couldn't retrieve reference of the Repository");
                            throw new org.omg.CORBA.TRANSIENT(
                                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                        }

                        //#if CDMW_ORB_VDR == orbacus
                        String propUrl = "corbaloc::localhost:" + propServPort + "/PropertyService";
                        org.omg.CosPropertyService.PropertySetDefFactory propsetdefFactory 
                            = getPropertySetDefFactory(orb, propUrl);

                        installPropertySetDef(orb, propsetdefFactory, repositoryRef);
                        //#endif
                    }

                    orb.run();
                } catch(org.omg.CORBA.SystemException se) {
                    System.err.println(se);
                }
            }
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.err.println("Can't resolve `NameService'");
        } catch(org.omg.CORBA.SystemException se) {
            System.err.println(se);
        } catch (Exception e) {
            System.err.println("unknown exception in main");
        }

        if (orb != null) {
            try {
                orb.destroy();
                //#if CDMW_ORB_VDR == orbacus
                OS.killProcess(propServer);
                //#endif
            } catch(org.omg.CORBA.SystemException se) {
                System.err.println(se);
            }
        }

    }

}

