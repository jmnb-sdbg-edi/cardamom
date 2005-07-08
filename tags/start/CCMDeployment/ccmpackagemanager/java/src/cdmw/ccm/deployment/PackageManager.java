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


package cdmw.ccm.deployment;

import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;
import com.thalesgroup.CdmwXML.DOMException;

import cdmw.ossupport.OS;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.xmlutils.XMLHelper;
import cdmw.platformmngt.PlatformInterface;

import org.omg.Components.Deployment.InstallationFailure;
import org.omg.Components.Deployment.InvalidLocation;
import org.omg.Components.RemoveFailure;
import com.thalesgroup.CdmwDeployment.PackageNotFound;


public class PackageManager {

    public static final String IOR_FILE_NAME = "package_manager.ior";

    private static final int POA_THREAD_POOL_SIZE = 10;
    private static final String PACKAGE_MANAGER_POA = "PackageManagerPOA";
    private static final String PACKAGES_POA = "PackagesPOA";
    private static final int PACKAGES_POA_THREAD_POOL_SIZE = 10;
    private static final String XML_SERVICE = "XMLServer";

    private static final String HELP_OPTION = "--help";
    private static final String INSTALL_DIR_OPTION = "--installDir";
    private static final String FTP_DIR_OPTION = "--ftpHomeDir";
    private static final String FTP_LOGIN_OPTION = "--ftpLogin";
    private static final String FTP_PASSWORD_OPTION = "--ftpPassword";
    private static final String FTP_HOST_OPTION = "--ftpHost";
    private static final String SAVE_FILE_OPTION = "--saveFile";


    private String[] args;

    private org.omg.CORBA.ORB orb;

    private String installDir;
    private String ftpRootDir;
    private String ftpLogin;
    private String ftpPassword;
    private String ftpHost;
    private java.io.File saveFile;

    private XMLParser xmlParser;


    private static void printUsage() {
        System.out.println("Usage:");

        System.out.println("java cdmw.ccm.deployment.PackageManager options");
        System.out.println("");
        System.out.println("Options:");
        System.out.println(HELP_OPTION + "                         Display this information.");
        System.out.println("");
        System.out.println(INSTALL_DIR_OPTION + "=<install_dir>     Directory in which packages will be installed.");
        System.out.println("                               This directory must be a subdirectory of ftp directory.");
        System.out.println(FTP_DIR_OPTION + "=<ftp_dir>         A directory which is remotely accessble via FTP (anonymous or with login).");
        System.out.println("[" + FTP_LOGIN_OPTION + "=<ftp_login>]       An FTP login (if specified, a password must be specified too).");
        System.out.println("                               If no login is specified, the PackageManager will generate URLs with anonymous FTP connections.");
        System.out.println("[" + FTP_PASSWORD_OPTION + "=<ftp_password>] An FTP password (if specified, a password must be specified too).");
        System.out.println("[" + FTP_HOST_OPTION + "=<ftp_host>]         The FTP host which will be used in generated URLs. By default the name of the host is used.");
        System.out.println("[" + SAVE_FILE_OPTION + "=<save_filename>]   The file which will be used to load and save state of CcmPackageManager.");
        System.out.println("");
    }





    public PackageManager(String[] args) {
        this.args = args;

        orb = null;

        installDir  = null;
        ftpRootDir  = null;
        ftpLogin    = null;
        ftpPassword = null;
        ftpHost     = null;
        saveFile    = null;

        xmlParser = null;
    }




    public boolean parseArguments() {
        String optionValue;

        // test if --help is specified
        optionValue = OS.getOptionValue(args, HELP_OPTION);
        if (optionValue.equals("yes")) {
            return false;
        }

        // get installDir
        optionValue = OS.getOptionValue(args, INSTALL_DIR_OPTION);
        if (optionValue.equals("no") || optionValue.equals("yes")) {
            System.out.println("Installation directory is not specified.");
            return false;
        }
        if (optionValue.startsWith("\"") && optionValue.endsWith("\"")) {
            installDir = optionValue.substring(1, optionValue.length()-2);
        } else {
            installDir = optionValue;
        }

        // get ftpRootDir
        optionValue = OS.getOptionValue(args, FTP_DIR_OPTION);
        if (optionValue.equals("no") || optionValue.equals("yes")) {
            System.out.println("FTP root directory is not specified.");
            return false;
        }
        if (optionValue.startsWith("\"") && optionValue.endsWith("\"")) {
            ftpRootDir = optionValue.substring(1, optionValue.length()-2);
        } else {
            ftpRootDir = optionValue;
        }

        // check if installDir is in ftpRootDir
        if (!installDir.startsWith(ftpRootDir)) {
            System.out.println("The installation directory must be " +
                               "a subdirectory of the FTP root directory.");
            return false;
        }

        // get ftpLogin
        optionValue = OS.getOptionValue(args, FTP_LOGIN_OPTION);
        if (optionValue.equals("no") || optionValue.equals("yes")) {
            // ignore option if set without value
            ftpLogin = null;
        } else {
            ftpLogin = optionValue;
        }

        // get ftpPassword
        optionValue = OS.getOptionValue(args, FTP_PASSWORD_OPTION);
        if (optionValue.equals("no") || optionValue.equals("yes")) {
            // ignore option if set without value
            ftpPassword = null;
        } else {
            ftpPassword = optionValue;
        }

        // check if login and password are both specified (or none)
        if ((ftpLogin != null) && (ftpPassword == null)) {
            System.out.println("The FTP password is not specified.");
            return false;
        }
        if ((ftpPassword != null) && (ftpLogin == null)) {
            System.out.println("The FTP login is not specified.");
            return false;
        }

        // get ftpHost
        optionValue = OS.getOptionValue(args, FTP_HOST_OPTION);
        if (optionValue.equals("no") || optionValue.equals("yes")) {
            // ignore option if set without value
            ftpHost = null;
        } else {
            ftpHost = optionValue;
        }

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
    createPackageManagerPOA(org.omg.PortableServer.POA parent)
        throws org.omg.PortableServer.POAPackage.AdapterAlreadyExists,
               org.omg.PortableServer.POAPackage.InvalidPolicy,
               org.omg.CORBA.SystemException
    {
        // Create policy list for packageManagerPOA
        org.omg.CORBA.Policy[] pl = new org.omg.CORBA.Policy[2];
        pl[0] = parent.create_implicit_activation_policy(
            org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);
        pl[1] = parent.create_thread_policy(
            org.omg.PortableServer.ThreadPolicyValue.SINGLE_THREAD_MODEL);

        // Create new POA
        StrategyList poaStrategies = new StrategyList();
        org.omg.PortableServer.POA poa =
            ORBSupport.createPOA(parent,
                              PACKAGE_MANAGER_POA,
                              parent.the_POAManager(),
                              new cdmw.orbsupport.PolicyList(pl),
                              poaStrategies);

        return poa;
    }

    private org.omg.PortableServer.POA
    createPackagesPOA(org.omg.PortableServer.POA parent)
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
        poaStrategies.addPOAThreadPool(PACKAGES_POA_THREAD_POOL_SIZE);
        org.omg.PortableServer.POA poa =
            ORBSupport.createPOA(parent,
                              PACKAGES_POA,
                              parent.the_POAManager(),
                              new cdmw.orbsupport.PolicyList(pl),
                              poaStrategies);

        return poa;
    }

    private LocationFactory createLocationFactory() {
        if (ftpHost == null) {
            return new FtpLocationFactory(ftpRootDir, ftpLogin, ftpPassword);
        } else {
            return new FtpLocationFactory(ftpRootDir, ftpLogin, ftpPassword, ftpHost);
        }
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
            XMLHelper.registerValueFactories(orb);

            // get rootPOA and activate its POA Manager
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();

            // create PackageManager POA
            org.omg.PortableServer.POA packageManagerPOA = createPackageManagerPOA(rootPOA);
            // create Packages POA
            org.omg.PortableServer.POA packagesPOA = createPackagesPOA(rootPOA);

            // create LocationFactory
            LocationFactory locationFactory = createLocationFactory();


           if (PlatformInterface.isLaunchedByPlatformManagement(args)) {
                // usage of the PackageManager as a managed process

                // create PackageManagerBehaviour
                PackageManagerBehaviour serverBehaviour =
                    new PackageManagerBehaviour(orb,
                                                packageManagerPOA,
                                                packagesPOA,
                                                saveFile,
                                                installDir,
                                                locationFactory);

                // initialise the platform interface
                PlatformInterface.setup(orb, args);
                // acknowledge the creation of the process
                PlatformInterface.acknowledgeCreation(serverBehaviour);

                orb.run();
                //orb.shutdown() is done by the ProcessBehaviour
                orb.destroy();

            } else {
                // standalone usage of the PackageManager

                // retrieve XMLParser
                this.xmlParser = getXMLParser();

                // create PackageManagerImpl
                PackageManagerImpl managerImpl =
                    PackageManagerImpl.init(packageManagerPOA,
                                            packagesPOA,
                                            xmlParser,
                                            saveFile,
                                            installDir,
                                            locationFactory);

                // Activate PackageManager
                byte[] oid = packageManagerPOA.activate_object(managerImpl);
                com.thalesgroup.CdmwDeployment.PackageManager manager =
                    com.thalesgroup.CdmwDeployment.PackageManagerHelper.narrow(
                        packageManagerPOA.id_to_reference(oid));

                // activate POA Managers
                packageManagerPOA.the_POAManager().activate();
                packagesPOA.the_POAManager().activate();

                // write the IOR of the PackageManager in a file
                try {
                    String ior = orb.object_to_string(manager);
                    java.io.PrintWriter out = new java.io.PrintWriter(
                        new java.io.FileOutputStream(IOR_FILE_NAME));
                    out.println(ior);
                    out.close();
                } catch(java.io.IOException ioe) {
                    ioe.printStackTrace();
                }

                System.out.println("PackageManager ready.");
                AutoAdmin admin = new AutoAdmin(this, System.in, System.out);
                admin.start();
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
        PackageManager mainObject = new PackageManager(args);

        if (mainObject.parseArguments()) {
            mainObject.run();
        } else {
            printUsage();
        }

    }

}





class AutoAdmin extends Thread {


    // Definition of a basic operation.
    // Each operation must redefine the method execute.
    abstract class OperationBase {
        public static final int SUCCESS       = 0;
        public static final int FAILURE      = -1;
        public static final int EXIT_ORDERED = -10;

        public abstract int execute(String[] arg, java.io.PrintStream os);
    }

    private static final String INFO_MESSAGE = "Type h or help for a list of available commands.";
    private static final String PROMPT_MESSAGE = "\n%>";
    private static final String UNKNOWN_COMMAND_MESSAGE = "Bad command!>";

    // List of all operation.
    // Key is operation name; an operation could be present
    // several time with different operation name
    private java.util.Hashtable operationsList;

    private PackageManager mainObject;
    // Input stream
    private java.io.InputStream istream;
    // Output stream
    private java.io.PrintStream ostream;


    public AutoAdmin(PackageManager mainObject,
                     java.io.InputStream istream,
                     java.io.PrintStream ostream)
    {
        this.mainObject = mainObject;
        this.istream = istream;
        this.ostream = ostream;
        registerOperations();
    }


    private void registerOperations() {
        operationsList = new java.util.Hashtable();

        // -- no operation
        OperationBase noOperation = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
               return SUCCESS;
            }
        };
        operationsList.put("", noOperation);

        // -- help
        OperationBase printHelp = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                os.println(
                    " +=====================================+============================================+"  + "\n"
                  + " | COMMAND                             | DESCRIPTION                                |"  + "\n"
                  + " +=====================================+============================================+"  + "\n"
                  + " | x | exit                            | exit ClientThread mode                     |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | h | help                            | print available commands                   |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | install_component <location>        | install a component package                |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | install_assembly <location>         | install an assembly package                |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | remove_component <name> [<version>] | remove a component package                 |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | remove_assembly <uuid>              | remove an assembly package                 |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | save                                | save actual configuration                  |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
               );

               return SUCCESS;
            }
        };
        operationsList.put("help", printHelp);
        operationsList.put("h",    printHelp);

        // ------- exit -------
        OperationBase exit = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                mainObject.stopStandaloneServer();
                return EXIT_ORDERED;
            }
        };
        operationsList.put("exit", exit);
        operationsList.put("x",    exit);


        // ------- install_component -------
        OperationBase installComponent = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'install_component <component_location>'");
                    return result;
                }

                try {
                    PackageManagerImpl.getInstance().install_component_package(arg[0]);
                    result = SUCCESS;
                } catch (InvalidLocation e) {
                    printException(e, os);
                } catch (InstallationFailure e) {
                    printException(e, os);
                }

                return result;
            }
        };
        operationsList.put("install_component", installComponent);

        // ------- install_assembly -------
        OperationBase installAssembly = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'install_assembly <assembly_location>'");
                    return result;
                }

                try {
                    PackageManagerImpl.getInstance().install_assembly_package(arg[0]);
                    result = SUCCESS;
                } catch (InvalidLocation e) {
                    printException(e, os);
                } catch (InstallationFailure e) {
                    printException(e, os);
                }

                return result;
            }
        };
        operationsList.put("install_assembly", installAssembly);


        // ------- remove_component -------
        OperationBase removeComponent = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if ((arg.length != 1) && (arg.length != 2)) {
                    os.println("Error: syntax is "
                               + "'remove_component <name> [<version>]'");
                    return result;
                }

                String name = arg[0];
                String version = "";
                if (arg.length ==2) {
                    version = arg[1];
                }

                try {
                    PackageManagerImpl.getInstance().remove_component_package(name, version);
                    result = SUCCESS;
                } catch (PackageNotFound e) {
                    printException(e, os);
                } catch (RemoveFailure e) {
                    printException(e, os);
                }

                return result;
            }
        };
        operationsList.put("remove_component", removeComponent);

        // ------- remove_assembly -------
        OperationBase removeAssembly = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'remove_assembly <uuid>'");
                    return result;
                }

                try {
                    PackageManagerImpl.getInstance().remove_assembly_package(arg[0]);
                    result = SUCCESS;
                } catch (PackageNotFound e) {
                    printException(e, os);
                } catch (RemoveFailure e) {
                    printException(e, os);
                }

                return result;
            }
        };
        operationsList.put("remove_assembly", removeAssembly);


        // ------- save -------
        OperationBase save = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                PackageManagerImpl.getInstance().save();
                result = SUCCESS;

                return result;
            }
        };
        operationsList.put("save", save);

    } // end of registerOperations()



    private void printException(cdmw.common.Exception ex,
                                java.io.PrintStream os)
    {
        os.println("CDMW Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(org.omg.CORBA.SystemException ex,
                                java.io.PrintStream os)
    {
        os.println("CORBA system Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(org.omg.CORBA.UserException ex,
                                java.io.PrintStream os)
    {
        os.println("CORBA user Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(ParsingException ex,
                                java.io.PrintStream os)
    {
        os.println("XML Parsing Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(DOMException ex,
                                java.io.PrintStream os)
    {
        os.println("XML DOM Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(java.io.IOException ex,
                                java.io.PrintStream os)
    {
        os.println("I/O Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(PackageNotFound ex,
                                java.io.PrintStream os)
    {
        os.println("PackageNotFound Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(InvalidLocation ex,
                                java.io.PrintStream os)
    {
        os.println("InvalidLocation Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(InstallationFailure ex,
                                java.io.PrintStream os)
    {
        os.print("InstallationFailure : ");
        switch (ex.reason) {
            case com.thalesgroup.CdmwDeployment.BAD_LOCATION.value:
                os.println("BAD_LOCATION");
                break;
            case com.thalesgroup.CdmwDeployment.READ_WRITE_ERROR.value:
                os.println("READ_WRITE_ERROR");
                break;
            case com.thalesgroup.CdmwDeployment.ERROR_IN_ARCHIVE.value:
                os.println("ERROR_IN_ARCHIVE");
                break;
            case com.thalesgroup.CdmwDeployment.ERROR_IN_CSD.value:
                os.println("ERROR_IN_CSD");
                break;
            case com.thalesgroup.CdmwDeployment.ERROR_IN_CCD.value:
                os.println("ERROR_IN_CCD");
                break;
            case com.thalesgroup.CdmwDeployment.ERROR_IN_CAD.value:
                os.println("ERROR_IN_CAD");
                break;
            case com.thalesgroup.CdmwDeployment.FILE_NOT_INSTALLED.value:
                os.println("FILE_NOT_INSTALLED");
                break;
            case com.thalesgroup.CdmwDeployment.FILE_ALREADY_INSTALLED.value:
                os.println("FILE_ALREADY_INSTALLED");
                break;
            case com.thalesgroup.CdmwDeployment.ASSEMBLY_ALREADY_INSTALLED.value:
                os.println("ASSEMBLY_ALREADY_INSTALLED");
                break;
            case com.thalesgroup.CdmwDeployment.COMPONENT_ALREADY_INSTALLED.value:
                os.println("COMPONENT_ALREADY_INSTALLED");
                break;
            case com.thalesgroup.CdmwDeployment.UNSUPPORTED_COMPONENT_TYPE.value:
                os.println("UNSUPPORTED_COMPONENT_TYPE");
                break;
            case com.thalesgroup.CdmwDeployment.UUID_NOT_FOUND.value:
                os.println("UUID_NOT_FOUND");
                break;
            case com.thalesgroup.CdmwDeployment.PACKAGE_MANAGER_ERROR.value:
                os.println("PACKAGE_MANAGER_ERROR");
                break;
            default:
                os.println("Unkown !!");
                break;
        }

        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(RemoveFailure ex,
                                java.io.PrintStream os)
    {
        os.print("RemoveFailure : ");
        switch (ex.reason) {
            case com.thalesgroup.CdmwDeployment.COMPONENT_IN_USE.value:
                os.println("COMPONENT_IN_USE");
                break;
            case com.thalesgroup.CdmwDeployment.DELETE_ERROR.value:
                os.println("DELETE_ERROR");
                break;
            default:
                os.println("Unkown !!");
                break;
        }
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printInfo(java.io.PrintStream os) {
        os.println(  "\n\n"
                   + "\t\t---------------------------------------\n"
                   + "\t\t  Cdmw PackageManager Admin           \n"
                   + "\t\t---------------------------------------\n"
                   + "\n\n"
                   + INFO_MESSAGE
                  );
    }

    public void run() {
        java.io.BufferedReader inReader =
            new java.io.BufferedReader(new java.io.InputStreamReader(istream));

        printInfo(ostream);

        int operationResult = 0;
        do {
            ostream.print(PROMPT_MESSAGE);
            try {
                // read command
                String command = inReader.readLine();

                // if Ctrl-C readLine() return null
                if (command == null) {
                    break;
                }

                java.util.StringTokenizer st = new java.util.StringTokenizer(command);
                int nbTokens = st.countTokens();

                // if input not empty
                if (nbTokens != 0) {

                    // parse input
                    String commandName = "";
                    String[] commandArgs = new String[nbTokens - 1];
                    try {
                        commandName = st.nextToken();
                        for (int i = 0; i < commandArgs.length; i++) {
                            commandArgs[i] = st.nextToken();
                        }

                    } catch (java.util.NoSuchElementException e) {
                        // should not happen
                    }

                    // execute command
                    OperationBase op = (OperationBase) operationsList.get(commandName);
                    if (op != null) {
                        operationResult = op.execute(commandArgs, ostream);
                    } else {
                        ostream.println(UNKNOWN_COMMAND_MESSAGE);
                        operationResult = OperationBase.FAILURE;
                    }
                }

            } catch (java.io.IOException e) {
                ostream.println("ERROR in readLine:");
                e.printStackTrace(new java.io.PrintWriter(ostream));
                operationResult = OperationBase.FAILURE;
            }

        } while (operationResult != OperationBase.EXIT_ORDERED);
    }




}



