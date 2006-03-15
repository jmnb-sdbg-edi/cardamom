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
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;

import cdmw.platformmngt.PlatformInterface;


public class ServerActivator {

    public static final String IOR_FILE_NAME = "server_activator.ior";
    public static final String NAMING_SERVICE_NAME = "NameService";

    private static final String HELP_OPTION = "--help";
    private static final String REPOSITORY_URL_OPTION = "--repositoryURL";    
    private static final int POA_THREAD_POOL_SIZE = 10;
    private static final String SERVER_ACTIVATOR_POA = "ServerActivatorPOA";
    private String[] args;
    private org.omg.CORBA.ORB orb;
    private String repositoryURL;



    private static void printUsage() {
        System.out.println("Usage:");

        System.out.println("java cdmw.ccm.serveractivator.ServerActivator options");
        System.out.println("");
        System.out.println("Options:");
        System.out.println(HELP_OPTION + "                         Display this information.");
        System.out.println("[" + REPOSITORY_URL_OPTION + "=<repository_url>]    URL of the Cdmw Repository.");
        System.out.println("");
    }


    public ServerActivator(String[] args) {
        this.args = args;
        orb = null;
        repositoryURL = null;
    }


    public boolean parseArguments() {
        String optionValue;

        // test if --help is specified
        optionValue = OS.getOptionValue(args, HELP_OPTION);
        if (optionValue.equals("yes")) {
            return false;
        }

        // get repository url (if specified)
        optionValue = OS.getOptionValue(args, REPOSITORY_URL_OPTION);
        if (optionValue.equals("yes")) {
            System.out.println("Repository URL is not specified.");
            return false;
        } else if (! optionValue.equals("no")) {
            repositoryURL = optionValue;
        }
        
        // if not managed, repository url is required
        if (repositoryURL == null && 
            !PlatformInterface.isLaunchedByPlatformManagement(args)) 
        {
            System.out.println("Repository URL must be specified if " +
                               "not launched by PlatformManagement.");
            return false;
        }

        return true;
    }


    private org.omg.PortableServer.POA
    createServerActivatorPOA(org.omg.PortableServer.POA parent)
        throws org.omg.PortableServer.POAPackage.AdapterAlreadyExists,
               org.omg.PortableServer.POAPackage.InvalidPolicy,
               org.omg.CORBA.SystemException
    {
        // Create policy list for serverActivatorPOA
        org.omg.CORBA.Policy[] pl = new org.omg.CORBA.Policy[1];
        pl[0] = parent.create_implicit_activation_policy(
            org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);

        // Create new POA
        StrategyList poaStrategies = new StrategyList();
        org.omg.PortableServer.POA poa =
            ORBSupport.createPOA(parent,
                              SERVER_ACTIVATOR_POA,
                              parent.the_POAManager(),
                              new cdmw.orbsupport.PolicyList(pl),
                              poaStrategies);

        return poa;
    }


    private org.omg.CosNaming.NamingContext getNamingService()
        throws Exception
    {
        org.omg.CORBA.Object obj = null;
        try {
            obj = orb.resolve_initial_references(NAMING_SERVICE_NAME);
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            throw new Exception("Cannot locate " + NAMING_SERVICE_NAME + ". "+
                "Please add argument -ORBInitRef " + NAMING_SERVICE_NAME +
                "=<naming_service_ior>.");
        }

        if (obj == null) {
            throw new Exception("The specified NamingService reference is invalid.");
        }

        org.omg.CosNaming.NamingContext naming = null;
        try {
            naming = org.omg.CosNaming.NamingContextHelper.narrow(obj);
        } catch (org.omg.CORBA.BAD_PARAM e) {
            throw new Exception("The specified reference for NameSerice " +
                                "does not implement a NamingContext.");
        }

        return naming;
    }

    private void initRepositoryInterface() {
        try {
            // Retrieve Repository object reference
            org.omg.CORBA.Object obj = orb.string_to_object(repositoryURL);
            com.thalesgroup.CdmwNamingAndRepository.Repository repository = 
                com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj);

            // Init of RepositoryInterface
            cdmw.namingandrepository.RepositoryInterface.init(
                cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN, repository);

            repository.resolve_root_context(
                com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);

        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        }
    }

    public void run() {

        try {
            // init orb
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = ORBSupport.ORBInit(args, orbStrategies);

            // register ConfigValue factory
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                org.omg.Components.ConfigValueHelper.id(), 
                new cdmw.ccm.deployment.ConfigValueFactoryImpl());                        

            // get rootPOA and activate its POA Manager
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();

            // create ServerActivator POA
            org.omg.PortableServer.POA serverActivatorPOA =
                createServerActivatorPOA(rootPOA);

           if (PlatformInterface.isLaunchedByPlatformManagement(args)) {
                // usage of the ServerActivator as a managed process

                // create ServerActivatorBehaviour
                ServerActivatorBehaviour activatorBehaviour =
                    new ServerActivatorBehaviour(orb, serverActivatorPOA);

                // initialise the platform interface
                PlatformInterface.setup(orb, args);
                // acknowledge the creation of the process
                PlatformInterface.acknowledgeCreation(activatorBehaviour);

                orb.run();
                //orb.shutdown() is done by the ProcessBehaviour
                orb.destroy();

           } else {
                // standalone usage of the ServerActivator

                // get NamingService
                org.omg.CosNaming.NamingContext naming = getNamingService();

                // init RepositoryInterface
                initRepositoryInterface();

                // create ServerActivatorImpl
                ServerActivatorImpl activatorImpl =
                    new ServerActivatorImpl(orb, serverActivatorPOA, naming);

                // Activate ServerActivator
                byte[] oid = serverActivatorPOA.activate_object(activatorImpl);
                org.omg.Components.Deployment.ServerActivator activator =
                    org.omg.Components.Deployment.ServerActivatorHelper.narrow(
                        serverActivatorPOA.id_to_reference(oid));

                // activate POA Manager
                serverActivatorPOA.the_POAManager().activate();

                // write the IOR of the ServerActivator in a file
                try {
                    String ior = orb.object_to_string(activator);
                    java.io.PrintWriter out = new java.io.PrintWriter(
                        new java.io.FileOutputStream(IOR_FILE_NAME));
                    out.println(ior);
                    out.close();
                } catch(java.io.IOException ioe) {
                    ioe.printStackTrace();
                }

                System.out.println("ServerActivator ready.");
                orb.run();
                //orb.shutdown() is done when we call the stop method
                orb.destroy();
            }

        } catch (Exception ex) {
            if (orb != null) {
                orb.shutdown(false);
                orb.destroy();
            }
            ex.printStackTrace();
        }
    }


    public static void main(String[] args) {
        ServerActivator mainObject = new ServerActivator(args);

        if (mainObject.parseArguments()) {
            mainObject.run();
        } else {
            printUsage();
        }
    }

}


