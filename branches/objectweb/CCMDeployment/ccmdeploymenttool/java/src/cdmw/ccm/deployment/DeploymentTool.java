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

import cdmw.common.Locations;
import cdmw.ossupport.OS;
import cdmw.namingandrepository.RepositoryInterface;


public class DeploymentTool {

    private static final String HELP_OPTION            = "--help";
    private static final String REPOSITORY_URL_OPTION  = "--repositoryURL"; 
    private static final String REPOSITORY_PORT_OPTION = "--repositoryPort"; 
    private static final String REPOSITORY_HOST_OPTION = "--repositoryHost";
    private static final String EXEC_OPTION            = "--exec";

    private static final String REPOSITORY_NAME = "CdmwRepository";
    private static final String XML_PARSER_NAME = "XMLParser";
    private static final String PACKAGE_MANAGER_NAME = "PackageManager";
    private static final String ASSEMBLY_FACTORY_NAME = "AssemblyFactory";

    private static final int POA_THREAD_POOL_SIZE = 1;

    // Definition of a basic command.
    // Each command must redefine the method execute.
    abstract class CommandBase {
        public static final int SUCCESS       = 0;
        public static final int FAILURE      = -1;
        public static final int EXIT_COMMANDED = -10;

        public abstract int execute(String[] arg, java.io.PrintStream os);
    }

    // List of all deployer.
    // Key is command name; a command could be present
    // several time with different command name
    private java.util.Hashtable commandsList;
    
    // Input stream
    java.io.InputStream istream;
    // Output stream
    java.io.PrintStream ostream;    

    // verbose mode
    private boolean verbose;

    private org.omg.CORBA.ORB orb;
    private Deployer deployer;

    
    public DeploymentTool(org.omg.CORBA.ORB orb,
                           java.io.InputStream istream,
                           java.io.PrintStream ostream) 
    {
        this.orb = orb;
        this.istream = istream;
        this.ostream = ostream;
        deployer = null;
        verbose = false;
    }
    

    private void printUsage() {
        ostream.println("Usage: ");
        ostream.println("  java cdmw.ccm.deployment.DeploymentTool " + HELP_OPTION);
        ostream.println("     Display this information.");
        ostream.println("  java cdmw.ccm.deployment.DeploymentTool <options>");
        ostream.println("     To obtain a shell for inline commands");
        ostream.println("  java cdmw.ccm.deployment.DeploymentTool <options> " + EXEC_OPTION + " <command>");
        ostream.println("     To execute only one command");
        ostream.println("");
        ostream.println("  Options:");
        ostream.println("    [" + REPOSITORY_URL_OPTION + "=<repository_url>]    URL of the Cdmw Repository.");
        ostream.println("    [" + REPOSITORY_PORT_OPTION + "=<repository_port>]  Port of the Cdmw Repository.");
        ostream.println("                                          Required if no repository URL is not specified.");
        ostream.println("    [" + REPOSITORY_HOST_OPTION + "=<repository_host>]  Host of the Cdmw Repository.");
        ostream.println("                                      (default value is localhost)");
        ostream.println("");
        ostream.println("  Commands:");
        ostream.println("    install_component <location>          Install a component package");
        ostream.println("    remove_component <name> [<version>]   Remove a component package");
        ostream.println("    install_assembly <location>           Install an assembly package");
        ostream.println("    remove_assembly <uuid>                Remove an assembly package");
        ostream.println("    create_assembly  <uuid> <name>        Create an assembly");
        ostream.println("    destroy_assembly <name>               Destroy an assembly");
        ostream.println("    build_assembly <name>                 Build an assembly");
        ostream.println("    tear_assembly_down <name>             Tear an assembly down");
        ostream.println("");
    }
    
    private void printInfo(java.io.PrintStream os) {
        ostream.println(  "\n\n"
                        + "\t\t------------------------\n"
                        + "\t\t  Cdmw DeploymentTool  \n"
                        + "\t\t------------------------\n"
                        + "\n\n"
                        + "Type h or help for a list of available deployer."
                       );
    }



    // register definition of available commands
    private void registerCommands() {
        commandsList = new java.util.Hashtable();

        // -- no command
        CommandBase noCommand = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
               return SUCCESS;
            }
        };
        commandsList.put("", noCommand);

        // -- help
        CommandBase printHelp = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                os.println(
                    " +=====================================+============================================+"  + "\n"
                  + " | COMMAND                             | DESCRIPTION                                |"  + "\n"
                  + " +=====================================+============================================+"  + "\n"
                  + " | x | exit                            | exit ClientThread mode                     |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | h | help                            | print available commands                   |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | verbose                             | toggle verbose mode on/off                 |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | install_component <location>        | install a component package                |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | remove_component <name> [<version>] | remove a component package                 |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | install_assembly <location>         | install an assembly package                |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | remove_assembly <uuid>              | remove an assembly package                 |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | create_assembly  <uuid> <name>      | create an assembly                         |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | destroy_assembly <name>             | destroy an assembly                        |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | build_assembly <name>               | build an assembly                          |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | tear_assembly_down <name>           | tear an assembly down                      |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
                  + " | sleep <time>                        | sleep during x milliseconds                |"  + "\n"
                  + " +-------------------------------------+--------------------------------------------+"  + "\n"
               );

               return SUCCESS;
            }
        };
        commandsList.put("help", printHelp);
        commandsList.put("h",    printHelp);

        // ------- exit -------
        CommandBase exit = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                return EXIT_COMMANDED;
            }
        };
        commandsList.put("exit", exit);
        commandsList.put("x",    exit);
        
        // ------- verbose -------
        CommandBase toggleVerbose = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
               verbose = !verbose;
               os.println(  "(verbose mode is "
                          + (verbose ? "on" : "off")
                          + ")");
               return SUCCESS;
            }
        };
        commandsList.put("verbose", toggleVerbose);


        // ------- install_component -------
        CommandBase installComponent = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;
                
                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'install_component <component_location>'");
                    return result;
                }
                
                try {
                    deployer.installComponent(arg[0]);
                    result = SUCCESS;
                } catch (Exception e) {
                    os.println(e.getMessage());
                }

                return result;
            }
        };
        commandsList.put("install_component", installComponent);


        // ------- remove_component -------
        CommandBase removeComponent = new CommandBase() {
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
                    deployer.removeComponent(name, version);
                    result = SUCCESS;
                } catch (Exception e) {
                    os.println(e.getMessage());
                }

                return result;
            }
        };
        commandsList.put("remove_component", removeComponent);


        // ------- install_assembly -------
        CommandBase installAssembly = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'install_assembly <assembly_location>'");
                    return result;
                }

                try {
                    deployer.installAssembly(arg[0]);
                    result = SUCCESS;
                } catch (Exception e) {
                    os.println(e.getMessage());
                }

                return result;
            }
        };
        commandsList.put("install_assembly", installAssembly);


        // ------- remove_assembly -------
        CommandBase removeAssembly = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'remove_assembly <uuid>'");
                    return result;
                }

                try {
                    deployer.removeAssembly(arg[0]);
                    result = SUCCESS;
                } catch (Exception e) {
                    os.println(e.getMessage());
                }

                return result;
            }
        };
        commandsList.put("remove_assembly", removeAssembly);


        // ------- create_assembly -------
        CommandBase createAssembly = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 2) {
                    os.println("Error: syntax is "
                               + "'create_assembly <assembly_uuid> <assembly_name>'");
                    return result;
                }

                try {
                    deployer.createAssembly(arg[0], arg[1]);
                    result = SUCCESS;
                } catch (Exception e) {
                    os.println(e.getMessage());
                }

                return result;
            }
        };
        commandsList.put("create_assembly", createAssembly);


        // ------- destroy_assembly -------
        CommandBase destroyAssembly = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'destroy_assembly <assembly_name>'");
                    return result;
                }

                try {
                    deployer.destroyAssembly(arg[0]);
                    result = SUCCESS;
                } catch (Exception e) {
                    os.println(e.getMessage());         
                }
            
                return result;
            }
        };
        commandsList.put("destroy_assembly", destroyAssembly);


        // ------- build_assembly -------
        CommandBase buildAssembly = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'build_assembly <assembly_name>'");
                    return result;
                }

                try {
                    deployer.buildAssembly(arg[0]);
                    result = SUCCESS;
                } catch (Exception e) {
                    os.println(e.getMessage());                
                }

                return result;
            }
        };
        commandsList.put("build_assembly", buildAssembly);


        // ------- tear_assembly_down -------
        CommandBase tearAssemblyDown = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'build_assembly <assembly_name>'");
                    return result;
                }

                try {
                    deployer.tearAssemblyDown(arg[0]);
                    result = SUCCESS;
                } catch (Exception e) {
                    os.println(e.getMessage());
                }

                return result;
            }
        };
        commandsList.put("tear_assembly_down", tearAssemblyDown);


        // ------- sleep -------
        CommandBase sleep = new CommandBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'sleep <time>'");
                    return result;
                }

                try {
                    Integer i = Integer.valueOf(arg[0]);
                    cdmw.ossupport.OS.sleep(i.intValue());
                    result = SUCCESS;
                    
                } catch (InterruptedException e) {
                    // ignore 
                    result = SUCCESS;
                } catch (NumberFormatException e) {
                    os.println("Error: " + arg[0] + " is not a number.");
                }
                
                return result;
            }
        };
        commandsList.put("sleep", sleep);
    }

    // return false is exit if commanded
    private boolean executeCommand(String commandName, String[] commandArgs) {
        // if verbose print command
        if (verbose) {
            ostream.print("@cmd = <" + commandName + "> ");
            ostream.print("@args = <");
            for (int i = 0; i < commandArgs.length; i++) {
                ostream.print(commandArgs[i]);
                if (i < commandArgs.length-1)
                    ostream.print(", ");
            }
            ostream.println(">");
        }
        
        // execute command
        CommandBase c = (CommandBase) commandsList.get(commandName);
        int commandResult;
        if (c != null) {
            commandResult = c.execute(commandArgs, ostream);
        } else {
            ostream.println("Bad command: " + commandName);
            commandResult = CommandBase.FAILURE;
        }
        
        return (commandResult != CommandBase.EXIT_COMMANDED);
    }
    

    private void startShell() {
        java.io.BufferedReader inReader =
            new java.io.BufferedReader(new java.io.InputStreamReader(istream));

        printInfo(ostream);

        boolean continueShell = true;
        do {
            ostream.print("\n%>");
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
                    
                    continueShell = executeCommand(commandName, commandArgs);
                }
            } catch (java.io.IOException e) {
                ostream.println("ERROR in readLine:");
                e.printStackTrace(new java.io.PrintWriter(ostream));
            }
        } while (continueShell);
    }
    


    private com.thalesgroup.CdmwNamingAndRepository.Repository 
    getRepository(String repositoryURL) 
    {
        try {
            // Retrieve Repository object reference
            org.omg.CORBA.Object obj = orb.string_to_object(repositoryURL);
            if (obj == null) {
                ostream.println("Object at URL " + repositoryURL + " is null.");
                cdmw.common.Assert.check(false);
            }
            
            com.thalesgroup.CdmwNamingAndRepository.Repository repository = 
                com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj);

            // Init of RepositoryInterface
            RepositoryInterface.init(
                Locations.CDMW_SERVICES_NAME_DOMAIN, repository);

            repository.resolve_root_context(
                com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);
        
            return repository;
        
        } catch (org.omg.CORBA.SystemException ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        }
        // unused (Assert stop process)
        return null;
    }
    
    private com.thalesgroup.CdmwXML.XMLParser getXMLParser() {
        try {
            // register XML value factories
            cdmw.xmlutils.XMLHelper.registerValueFactories(orb);

            String servicesNameDomain = Locations.CDMW_SERVICES_NAME_DOMAIN;
            // Get NamingInterface object from RepositoryInterface
            cdmw.namingandrepository.NamingInterface ni =
                RepositoryInterface.getDomainNamingInterface(servicesNameDomain);

            // Get a reference on the PackageManager
            org.omg.CORBA.Object obj = ni.resolve(XML_PARSER_NAME);
            return com.thalesgroup.CdmwXML.XMLParserHelper.narrow(obj);

        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        }

        // unused (Assert stop process)
        return null;
    }
    

    private com.thalesgroup.CdmwDeployment.PackageManager getPackageManager() {
        try {
            String deploymentNameDomain = Locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN;
            // Get NamingInterface object from RepositoryInterface
            cdmw.namingandrepository.NamingInterface ni =
                RepositoryInterface.getDomainNamingInterface(deploymentNameDomain);

            // Get a reference on the PackageManager
            org.omg.CORBA.Object obj = ni.resolve(PACKAGE_MANAGER_NAME);
            return com.thalesgroup.CdmwDeployment.PackageManagerHelper.narrow(obj);

        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        }

        // unused (Assert stop process)
        return null;
    }

    private com.thalesgroup.CdmwDeployment.AssemblyFactory getAssemblyFactory() {
        try {
            // register Cookie value factory
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                org.omg.Components.CookieHelper.id(), 
                new cdmw.ccm.common.CookieFactoryImpl());            
            
            String deploymentNameDomain = Locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN;
            // Get NamingInterface object from RepositoryInterface
            cdmw.namingandrepository.NamingInterface ni =
                RepositoryInterface.getDomainNamingInterface(deploymentNameDomain);

            // Get a reference on the AssemblyFactory
            org.omg.CORBA.Object obj = ni.resolve(ASSEMBLY_FACTORY_NAME);
            return com.thalesgroup.CdmwDeployment.AssemblyFactoryHelper.narrow(obj);

        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed ex) {
            // shouldn't happen
            ex.printStackTrace(new java.io.PrintWriter(ostream));
            cdmw.common.Assert.check(false);
        }

        // unused (Assert stop process)
        return null;
    }

    



    public void run(String[] args) {
        String repURL = null;
        String repPort = null;
        String repHost = null;
        boolean execCommand = false;

        if (args.length == 0) {
            printUsage();
            return;
        }

        // parse args
        int i = 0;
        while (i < args.length) {
            if (args[i].equals(HELP_OPTION)) {
                printUsage();
                return;
            } else if (args[i].startsWith(REPOSITORY_URL_OPTION + "=")) {
                repURL = args[i].substring(args[i].indexOf('=')+1);
            } else if (args[i].startsWith(REPOSITORY_PORT_OPTION + "=")) {
                repPort = args[i].substring(args[i].indexOf('=')+1);
            } else if (args[i].startsWith(REPOSITORY_HOST_OPTION + "=")) {
                repHost = args[i].substring(args[i].indexOf('=')+1);
            } else if (args[i].equals(EXEC_OPTION)) {
                execCommand = true;
                break;
            } else {
                ostream.println("Unkown option: " + args[i]);
                printUsage();
                return;
            }
            ++i;
        }

        // get Repository URL (if not specified)
        if (repURL == null) {
            // get repository port
            if (repPort == null) {
                ostream.println("Repository is not specified.");
                printUsage();
                return;
            }
            try {
                int port = Integer.valueOf(repPort).intValue();
            } catch (NumberFormatException e) {
                ostream.println(repPort + " is not a port number.");
                printUsage();
                return;
            }

            if (repHost == null) {
                repHost = "localhost";
            }

            // construct Repository corbaloc
            repURL = "corbaloc::" + repHost + ":" + repPort + 
                     "/" + REPOSITORY_NAME;
        }
            
        // create Deployer
        deployer = new Deployer(getRepository(repURL),
                                getXMLParser(),
                                getPackageManager(),
                                getAssemblyFactory());

        // register available commands
        registerCommands();

        if (execCommand) {
            // get command name
            if (i == args.length-1) {
                ostream.println("No command specified !");
                printUsage();
                return;
            }
            String commandName = args[i+1];
            String[] commandArgs = new String[args.length-(i+2)];
            for (int j=i+2; j < args.length;  j++) {
                commandArgs[j-(i+2)] = args[j];
            }
            
            // execute command
            executeCommand("verbose", new String[0]);
            executeCommand(commandName, commandArgs);
            
        } else {
            // start shell
            startShell();
        }
    }
    

    public static void main(String[] args) {

        org.omg.CORBA.ORB orb = null;
        
        try {
            // initialize ORB
            cdmw.orbsupport.StrategyList orbStrategies = 
                new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies);

            DeploymentTool tool = 
                new DeploymentTool(orb, System.in, System.out);

            tool.run(args);

        } catch (Exception e) {
            e.printStackTrace();
        }

        if (orb != null) {
            try {
                orb.destroy();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }


}


