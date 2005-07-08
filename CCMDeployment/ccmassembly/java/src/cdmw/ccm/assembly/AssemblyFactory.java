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

import org.omg.Components.Deployment.InvalidLocation;
import org.omg.Components.CreateFailure;

import cdmw.ossupport.OS;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.platformmngt.PlatformInterface;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.XMLParserHelper;
import com.thalesgroup.CdmwDeployment.PackageManager;
import com.thalesgroup.CdmwDeployment.PackageManagerHelper;
import com.thalesgroup.CdmwDeployment.StandardConfiguratorFactory;
import com.thalesgroup.CdmwDeployment.StandardConfiguratorFactoryHelper;


/**
 * Main class of CCM Assembly.
 * This class creates an AssemblyFactory CORBA object and 
 * register it into Repository (if launched by Supervision).
 */
public class AssemblyFactory {


    public static final String IOR_FILE_NAME = "assembly_factory.ior";

    private static final int POA_THREAD_POOL_SIZE = 10;
    private static final int ASSEMBLY_FACTORY_POA_THREAD_POOL_SIZE = 10;
    private static final int ASSEMBLIES_POA_THREAD_POOL_SIZE = 10;
    private static final int CONFIGURATORS_POA_THREAD_POOL_SIZE = 10;
    private static final String ASSEMBLY_FACTORY_POA = "AssemblyFactoryPOA";
    private static final String ASSEMBLIES_POA = "AssembliesPOA";
    private static final String CONFIGURATORS_POA = "ConfiguratorsPOA";

    private static final String REPOSITORY_NAME = "InterfaceRepository";
    private static final String XML_SERVER_NAME = "XMLServer";
    private static final String PACKAGE_MANAGER_NAME = "PackageManager";
    private static final String CONFIGURATOR_FACTORY_NAME = "StandardConfiguratorFactory";

    private static final String HELP_OPTION = "--help";
    private static final String REPOSITORY_URL_OPTION = "--repositoryURL";


    private String[] args;

    private org.omg.CORBA.ORB orb;

    private XMLParser xmlParser;
    private PackageManager packManager;
    private String repositoryURL;


    /**
     * Prints usage information on standard output
     */
    private static void printUsage() {
        System.out.println("Usage:");

        System.out.println("java cdmw.ccm.deployment.ccm.deployment options");
        System.out.println("");
        System.out.println("Options:");
        System.out.println(HELP_OPTION + "                         Display this information.");
        System.out.println("[" + REPOSITORY_URL_OPTION + "=<repository_url>]    URL of the Cdmw Repository.");
        System.out.println("");
    }


    /**
     * Constructor.
     * 
     * @param args the command line arguments
     */
    public AssemblyFactory(String[] args) {
        this.args = args;
        orb = null;

        xmlParser = null;
        packManager = null;
        repositoryURL = null;
    }


    /**
     * Parses command line arguments.
     */
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



    /**
     * Creates POA for AssemblyFactory object.
     *
     * @param parent the POA parent of the created POA.
     * @return the new POA
     */
    private org.omg.PortableServer.POA
    createAssemblyFactoryPOA(org.omg.PortableServer.POA parent)
        throws org.omg.PortableServer.POAPackage.AdapterAlreadyExists,
               org.omg.PortableServer.POAPackage.InvalidPolicy,
               org.omg.CORBA.SystemException,
               cdmw.common.BadParameterException
    {
        // Create policy list for packageManagerPOA
        org.omg.CORBA.Policy[] pl = new org.omg.CORBA.Policy[1];
        pl[0] = parent.create_implicit_activation_policy(
            org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);

        // Create new POA
        StrategyList poaStrategies = new StrategyList();
        poaStrategies.addPOAThreadPool(ASSEMBLY_FACTORY_POA_THREAD_POOL_SIZE);
        org.omg.PortableServer.POA poa =
            ORBSupport.createPOA(parent,
                              ASSEMBLY_FACTORY_POA,
                              parent.the_POAManager(),
                              new cdmw.orbsupport.PolicyList(pl),
                              poaStrategies);

        return poa;
    }


    /**
     * Creates POA for Assembly objects.
     *
     * @param parent the POA parent of the created POA.
     * @return the new POA
     */
    private org.omg.PortableServer.POA
    createAssembliesPOA(org.omg.PortableServer.POA parent)
        throws org.omg.PortableServer.POAPackage.AdapterAlreadyExists,
               org.omg.PortableServer.POAPackage.InvalidPolicy,
               org.omg.CORBA.SystemException,
               cdmw.common.BadParameterException
    {
        // Create policy list for assembliesPOA
        org.omg.CORBA.Policy[] pl = new org.omg.CORBA.Policy[1];
        pl[0] = parent.create_implicit_activation_policy(
            org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);

        // Create new POA
        StrategyList poaStrategies = new StrategyList();
        poaStrategies.addPOAThreadPool(ASSEMBLIES_POA_THREAD_POOL_SIZE);
        org.omg.PortableServer.POA poa =
            ORBSupport.createPOA(parent,
                              ASSEMBLIES_POA,
                              parent.the_POAManager(),
                              new cdmw.orbsupport.PolicyList(pl),
                              poaStrategies);

        return poa;
    }

    /**
     * Creates POA for Configurator objects and ConfiguratorFactory object.
     *
     * @param parent the POA parent of the created POA.
     * @return the new POA
     */
    private org.omg.PortableServer.POA
    createConfiguratorsPOA(org.omg.PortableServer.POA parent)
        throws org.omg.PortableServer.POAPackage.AdapterAlreadyExists,
               org.omg.PortableServer.POAPackage.InvalidPolicy,
               org.omg.CORBA.SystemException,
               cdmw.common.BadParameterException
    {
        // Create policy list for configuratorsPOA
        org.omg.CORBA.Policy[] pl = new org.omg.CORBA.Policy[1];
        pl[0] = parent.create_implicit_activation_policy(
            org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);

        // Create new POA
        StrategyList poaStrategies = new StrategyList();
        poaStrategies.addPOAThreadPool(CONFIGURATORS_POA_THREAD_POOL_SIZE);
        org.omg.PortableServer.POA poa =
            ORBSupport.createPOA(parent,
                              CONFIGURATORS_POA,
                              parent.the_POAManager(),
                              new cdmw.orbsupport.PolicyList(pl),
                              poaStrategies);

        return poa;
    }


    /**
     * Retrieves the XML Parser object with a orb.resolve_initial_reference.
     *
     * @return the XMLParser object
     */
    private com.thalesgroup.CdmwXML.XMLParser getXMLParser()
        throws Exception
    {
        org.omg.CORBA.Object obj = null;
        try {
            obj = orb.resolve_initial_references(XML_SERVER_NAME);
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            throw new Exception("Cannot locate " + XML_SERVER_NAME + ". "+
                "Please add argument -ORBInitRef " + XML_SERVER_NAME +
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


    /**
     * Retrieves the PackageManager object with a orb.resolve_initial_reference.
     *
     * @return the PackageManager object
     */
    private PackageManager getPackageManager()
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

        PackageManager manager = null;
        try {
            manager = PackageManagerHelper.narrow(obj);
        } catch (org.omg.CORBA.BAD_PARAM e) {
            throw new Exception("The specified reference for PackageManager " +
                                "does not implement a PackageManager.");
        }

        return manager;
    }

    /**
     * Creates the ConfiguratorFactory object.
     *
     * @return the ConfiguratorFactory object
     */
    private StandardConfiguratorFactory 
    createConfiguratorFactory(org.omg.PortableServer.POA configuratorsPOA)
        throws Exception
    {
        // create StandardConfiguratorFactory
        cdmw.ccm.configurator.StandardConfiguratorFactoryImpl cfgFactoryImpl =
            new cdmw.ccm.configurator.StandardConfiguratorFactoryImpl(
                 orb, configuratorsPOA);
        
        return cfgFactoryImpl._this(orb);
    }
    


    /**
     * Initializes the RepositoryInterface.
     */
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


    /**
     * Runs the process (ORB init, POA and objects creation, ORB run).
     */
    public void run() {

        try {
            // init orb
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = ORBSupport.ORBInit(args, orbStrategies);

            // register XML value factories
            XMLHelper.registerValueFactories(orb);

            // register Cookie value factory
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                org.omg.Components.CookieHelper.id(),
                new cdmw.ccm.common.CookieFactoryImpl());

            // register ConfigValue value factory (for configurators)
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                org.omg.Components.ConfigValueHelper.id(),
                new cdmw.ccm.deployment.ConfigValueFactoryImpl());

            // get rootPOA and activate its POA Manager
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();

            // create AssemblyFactory POA
            org.omg.PortableServer.POA assemblyFactoryPOA =
                createAssemblyFactoryPOA(rootPOA);

            // create Assemblies POA
            org.omg.PortableServer.POA assembliesPOA =
                createAssembliesPOA(rootPOA);

            // create Configurators POA
            org.omg.PortableServer.POA configuratorsPOA =
                createConfiguratorsPOA(rootPOA);

            // create StandardConfiguratorFactory
            StandardConfiguratorFactory configFactory = 
                createConfiguratorFactory(configuratorsPOA);

            if (PlatformInterface.isLaunchedByPlatformManagement(args)) {
                // usage of the AssemblyFactory as a managed process

                // create PackageManagerBehaviour
                AssemblyFactoryBehaviour serverBehaviour =
                    new AssemblyFactoryBehaviour(orb,
                                                 assemblyFactoryPOA,
                                                 assembliesPOA,
                                                 configFactory);

                // initialise the platform interface
                PlatformInterface.setup(orb, args);
                // acknowledge the creation of the process
                PlatformInterface.acknowledgeCreation(serverBehaviour);

                orb.run();
                //orb.shutdown() is done by the ProcessBehaviour
                orb.destroy();

            } else {

                // standalone usage of the AssemblyFactory

                // retrieve XMLParser
                xmlParser = getXMLParser();

                // retrieve PackageManager
                packManager = getPackageManager();

                // init RepositoryInterface
                initRepositoryInterface();

                // create AssemblyFactoryImpl
                AssemblyFactoryImpl assemblyFactoryImpl =
                    AssemblyFactoryImpl.init(orb,
                                             assemblyFactoryPOA,
                                             assembliesPOA,
                                             configFactory,
                                             xmlParser,
                                             packManager);

                // Activate AssemblyFactory
                byte[] oid = assemblyFactoryPOA.activate_object(assemblyFactoryImpl);
                org.omg.Components.Deployment.AssemblyFactory assemblyFactory =
                    org.omg.Components.Deployment.AssemblyFactoryHelper.narrow(
                        assemblyFactoryPOA.id_to_reference(oid));

                // activate POA Managers
                assemblyFactoryPOA.the_POAManager().activate();
                assembliesPOA.the_POAManager().activate();
                configuratorsPOA.the_POAManager().activate();

                // write the IOR of the AssemblyFactory in a file
                try {
                    String ior = orb.object_to_string(assemblyFactory);
                    java.io.PrintWriter out = new java.io.PrintWriter(
                        new java.io.FileOutputStream(IOR_FILE_NAME));
                    out.println(ior);
                    out.close();
                } catch(java.io.IOException ioe) {
                    ioe.printStackTrace();
                }

                System.out.println("AssemblyFactory ready.");
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



    /**
     * Main operation.
     */
    public static void main(String[] args) {
        AssemblyFactory mainObject = new AssemblyFactory(args);

        if (mainObject.parseArguments()) {
            mainObject.run();
        } else {
            printUsage();
        }
    }

}











/**
 * This class implements an inline HMI allowing  
 * commands for AssemblyFactory.
 * It is only used when AssemblyFactory is launched as 
 * a standalone process (not by Supervision).   
 */
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

    private AssemblyFactory mainObject;
    // Input stream
    private java.io.InputStream istream;
    // Output stream
    private java.io.PrintStream ostream;


    public AutoAdmin(AssemblyFactory mainObject,
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
                  + " | create_assembly <cad_location>      | create an assembly                         |"  + "\n"
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

        // ------- create_assembly -------
        OperationBase createAssembly = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'create_assembly <cad_location>'");
                    return result;
                }

                try {
                    org.omg.Components.Cookie c =
                        AssemblyFactoryImpl.getInstance().create(arg[0]);
                    result = SUCCESS;
                } catch (InvalidLocation e) {
                    printException(e, os);
                } catch (CreateFailure e) {
                    printException(e, os);
                }

                return result;
            }
        };
        operationsList.put("create_assembly", createAssembly);

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

    private void printException(InvalidLocation ex,
                                java.io.PrintStream os)
    {
        os.println("InvalidLocation Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(CreateFailure ex,
                                java.io.PrintStream os)
    {
        os.print("CreateFailure : " + ex.reason + " " + ex.getMessage());
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


