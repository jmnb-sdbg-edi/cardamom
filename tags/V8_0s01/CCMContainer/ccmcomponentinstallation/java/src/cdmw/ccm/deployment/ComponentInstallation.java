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


package cdmw.ccm.deployment;

import cdmw.ossupport.OS;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.platformmngt.PlatformInterface;



public class ComponentInstallation {

    public static final String IOR_FILE_NAME = "component_installation.ior";
    private static final String POA_NAME = "ComponentInstallationPOA";
    private static final int POA_THREAD_POOL_SIZE = 10;
    private static final String PACKAGE_MANAGER_NAME = "PackageManager";
    private static final String XML_SERVICE = "XMLService";

    private static final String HELP_OPTION = "--help";
    private static final String INSTALL_DIR_OPTION = "--installDir";
    private static final String SAVE_FILE_OPTION = "--saveFile";


    private String[] args;

    private org.omg.CORBA.ORB orb;

    private java.io.File installDir;
    private java.io.File saveFile;


    public ComponentInstallation(String[] args) {
        this.args = args;

        orb = null;
        installDir  = null;
        saveFile    = null;
    }




    private static void printUsage() {
        System.out.println("Usage:");

        System.out.println("java cdmw.ccm.deployment.ComponentInstallation options");
        System.out.println("");
        System.out.println("Options:");
        System.out.println(HELP_OPTION + "                         Display this information.");
        System.out.println("");
        System.out.println(INSTALL_DIR_OPTION + "=<install_dir>     Directory in which packages will be installed.");
        System.out.println("[" + SAVE_FILE_OPTION + "=<save_filename>]   The file which will be used to load and save state of CcmPackageManager.");
        System.out.println("");
    }


    public boolean parseArguments() {
        String optionValue;

        // get installDir
        optionValue = OS.getOptionValue(args, INSTALL_DIR_OPTION);
        if (optionValue.equals("no") || optionValue.equals("yes")) {
            System.out.println("Installation directory is not specified.");
            return false;
        }
        if (optionValue.startsWith("\"") && optionValue.endsWith("\"")) {
            optionValue = optionValue.substring(1, optionValue.length()-2);
        }
        installDir = new java.io.File(optionValue);

        // get saveFile
        optionValue = OS.getOptionValue(args, SAVE_FILE_OPTION);
        if (optionValue.equals("no") || optionValue.equals("yes")) {
            // ignore option if set without value
            saveFile = null;
        } else {
            saveFile = new java.io.File(optionValue);
        }

        return true;
    }


    private org.omg.PortableServer.POA
    createComponentInstallationPOA(org.omg.PortableServer.POA parent)
        throws org.omg.PortableServer.POAPackage.AdapterAlreadyExists,
               org.omg.PortableServer.POAPackage.InvalidPolicy,
               org.omg.CORBA.SystemException,
               cdmw.common.BadParameterException
    {
        // Create policy list for packagesPOA
        org.omg.CORBA.Policy[] pl = new org.omg.CORBA.Policy[1];
        pl[0] = parent.create_implicit_activation_policy(
            org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);

        // Create new POA
        StrategyList poaStrategies = new StrategyList();
        poaStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
        org.omg.PortableServer.POA poa =
            ORBSupport.createPOA(parent,
                              POA_NAME,
                              parent.the_POAManager(),
                              new cdmw.orbsupport.PolicyList(pl),
                              poaStrategies);

        return poa;
    }


    private com.thalesgroup.CdmwDeployment.PackageManager getPackageManager()
        throws Exception
    {
        org.omg.CORBA.Object obj = null;
        try {
            obj = orb.resolve_initial_references(PACKAGE_MANAGER_NAME);
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            throw new Exception("Cannot locate " + PACKAGE_MANAGER_NAME + ". "+
                "Please add argument -ORBInitRef " + PACKAGE_MANAGER_NAME +
                "=<package_manager_ior>.");
        }

        if (obj == null) {
            throw new Exception("The specified PackageManager reference is invalid.");
        }

        com.thalesgroup.CdmwDeployment.PackageManager manager = null;
        try {
            manager = com.thalesgroup.CdmwDeployment.PackageManagerHelper.narrow(obj);
        } catch (org.omg.CORBA.BAD_PARAM e) {
            throw new Exception("The specified reference for PackageManager " +
                                "does not implement a PackageManager.");
        }

        return manager;
    }


    private com.thalesgroup.CdmwXML.XMLParser getXMLParser()
        throws Exception
    {
        org.omg.CORBA.Object obj = null;
        try {
            obj = orb.resolve_initial_references(XML_SERVICE);
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            throw new Exception("Cannot locate " + XML_SERVICE + ". "+
                "Please add argument -ORBInitRef " + XML_SERVICE +
                "=<xml_parser_ior>.");
        }

        if (obj == null) {
            throw new Exception("The specified XMLParser reference is invalid.");
        }

        com.thalesgroup.CdmwXML.XMLParser parser = null;
        try {
            parser = com.thalesgroup.CdmwXML.XMLParserHelper.narrow(obj);
        } catch (org.omg.CORBA.BAD_PARAM e) {
            throw new Exception("The specified reference for XMLParser " +
                                "does not implement a XMLParser.");
        }

        return parser;
    }



    public void run() {
        try {
            // init orb
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = ORBSupport.ORBInit(args, orbStrategies);

            // register XML value factories
            cdmw.xmlutils.XMLHelper.registerValueFactories(orb);

            // get rootPOA and activate its POA Manager
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();

            // create componentInstallation POA
            org.omg.PortableServer.POA compInstallationPOA =
                createComponentInstallationPOA(rootPOA);

           if (PlatformInterface.isLaunchedByPlatformManagement(args)) {
                // usage of the ComponentInstallation as a managed process

                // create ComponentInstallationBehaviour
                ComponentInstallationBehaviour serverBehaviour =
                    new ComponentInstallationBehaviour(orb,
                                                       compInstallationPOA,
                                                       installDir,
                                                       saveFile);

                // initialise the platform interface
                PlatformInterface.setup(orb, args);
                // acknowledge the creation of the process
                PlatformInterface.acknowledgeCreation(serverBehaviour);

                orb.run();
                //orb.shutdown() is done by the ProcessBehaviour
                orb.destroy();

            } else {
                // standalone usage of the ComponentInstallation

                System.out.println("get PM...");
                // retrieve PackageManager
                com.thalesgroup.CdmwDeployment.PackageManager manager =
                    getPackageManager();

                System.out.println("get XML...");
                // retrieve XMLServer
                com.thalesgroup.CdmwXML.XMLParser parser =
                    getXMLParser();

                System.out.println("create servant...");
                // create ComponentInstallationImpl
                ComponentInstallationImpl compInstallImpl =
                    new ComponentInstallationImpl(compInstallationPOA,
                                                  manager,
                                                  parser,
                                                  installDir,
                                                  saveFile);

                System.out.println("activate POAM...");
                // activate POA Manager
                compInstallationPOA.the_POAManager().activate();

                System.out.println("activate servant...");
                // activate ComponentInstallation
                byte[] oid = compInstallationPOA.activate_object(compInstallImpl);
                org.omg.Components.Deployment.ComponentInstallation compInstall =
                    org.omg.Components.Deployment.ComponentInstallationHelper.narrow(
                        compInstallationPOA.id_to_reference(oid));


                System.out.println("write ior...");
                // write the IOR of the ComponentInstallation in a file
                try {
                    String ior = orb.object_to_string(compInstall);
                    java.io.PrintWriter out = new java.io.PrintWriter(
                        new java.io.FileOutputStream(IOR_FILE_NAME));
                    out.println(ior);
                    out.close();
                } catch(java.io.IOException ioe) {
                    ioe.printStackTrace();
                }

                System.out.println("ComponentInstallation ready.");
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


    /**
     * Stops the PackageManager only if it's a standalone program.
     */
    public void stopStandaloneServer() {
        if (!PlatformInterface.isLaunchedByPlatformManagement(args)) {
            orb.shutdown(false);
        }
    }



    public static void main(String[] args) {
        ComponentInstallation mainObject = new ComponentInstallation(args);

        if (mainObject.parseArguments()) {
            mainObject.run();
        } else {
            printUsage();
        }

    }

}

