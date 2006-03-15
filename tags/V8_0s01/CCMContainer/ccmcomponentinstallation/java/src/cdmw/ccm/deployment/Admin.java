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

import org.omg.Components.Deployment.ComponentInstallation;
import org.omg.Components.Deployment.InvalidLocation;
import org.omg.Components.Deployment.UnknownImplId;
import org.omg.Components.Deployment.InstallationFailure;
import org.omg.Components.RemoveFailure;


public class Admin {

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

    private static final int POA_THREAD_POOL_SIZE = 10;
    private static final String COMPONENT_INSTALLATION_NAME = "ComponentInstallation";
    
    private String[] args;
    private org.omg.CORBA.ORB orb;
    private ComponentInstallation compInstall;

    // List of all operation.
    // Key is operation name; an operation could be present
    // several time with different operation name
    private java.util.Hashtable operationsList;
    
    // Input stream
    java.io.InputStream istream;
    // Output stream
    java.io.PrintStream ostream;
    
    // verbose mode
    private boolean verbose;



    public Admin(String[] args, 
                 java.io.InputStream istream,
                 java.io.PrintStream ostream) 
    {
        this.args = args;
        this.istream = istream;
        this.ostream = ostream;
        orb = null;
        compInstall = null;
        operationsList = null;
        verbose = true;
    }
    

    private ComponentInstallation getComponentInstallation() 
        throws Exception
    {
        org.omg.CORBA.Object obj = null;
        try {
            obj = orb.resolve_initial_references(COMPONENT_INSTALLATION_NAME);
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            throw new Exception("Cannot locate "+ COMPONENT_INSTALLATION_NAME + ". "+
                "Please add argument -ORBInitRef " + COMPONENT_INSTALLATION_NAME + 
                "=<package_manager_ior>.");
        }
        
        if (obj == null) {
            throw new Exception("The specified ComponentInstallation reference is invalid.");
        }
        
        ComponentInstallation cmpInst = null;
        try {
            cmpInst = org.omg.Components.Deployment.ComponentInstallationHelper.narrow(obj);
        } catch (org.omg.CORBA.BAD_PARAM e) {
            throw new Exception("The specified reference for ComponentInstallation " +
                                "does not implement a ComponentInstallation.");
        }
        
        return cmpInst;        
    }
    


    // register definition of available operations
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
                    " +===================================+============================================+"  + "\n"
                  + " | COMMAND                           | DESCRIPTION                                |"  + "\n"
                  + " +===================================+============================================+"  + "\n"
                  + " | x | exit                          | exit ClientThread mode                     |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | h | help                          | print available commands                   |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | verbose                           | toggle verbose mode on/off                 |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | install <uuid> [<location>]       | install a component implementation         |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | replace <uuid> [<location>]       | replace a component implementation         |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | remove <uuid>                     | remove a component implementation          |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
               );

               return SUCCESS;
            }
        };
        operationsList.put("help", printHelp);
        operationsList.put("h",    printHelp);

        // ------- exit -------
        OperationBase exit = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                return EXIT_ORDERED;
            }
        };
        operationsList.put("exit", exit);
        operationsList.put("x",    exit);
        
        // ------- verbose -------
        OperationBase toggleVerbose = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
               verbose = !verbose;
               os.println(  "(verbose mode is "
                          + (verbose ? "on" : "off")
                          + ")");
               return SUCCESS;
            }
        };
        operationsList.put("verbose", toggleVerbose);

    
        // ------- install -------
        OperationBase install = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;
                
                if ((arg.length != 1) && (arg.length != 2)) {
                    os.println("Error: syntax is "
                               + "'install <uuid> [<location>]'");
                    return result;
                }
                String uuid = arg[0];
                String location = "";
                if (arg.length == 2) {
                    location = arg[1];
                }

                try {
                    compInstall.install(uuid, location);
                    result = SUCCESS;
                } catch (InvalidLocation e) {
                    printException(e, os);
                } catch (InstallationFailure e) {
                    printException(e, os);
                }
                
                return result;
            }
        };
        operationsList.put("install", install);
        
        // ------- replace -------
        OperationBase replace = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;
                
                if ((arg.length != 1) && (arg.length != 2)) {
                    os.println("Error: syntax is "
                               + "'replace <uuid> [<location>]'");
                    return result;
                }
                String uuid = arg[0];
                String location = "";
                if (arg.length == 2) {
                    location = arg[1];
                }

                try {
                    compInstall.replace(uuid, location);
                    result = SUCCESS;
                } catch (InvalidLocation e) {
                    printException(e, os);
                } catch (InstallationFailure e) {
                    printException(e, os);
                }
                
                return result;
            }
        };
        operationsList.put("replace", replace);

        // ------- remove -------
        OperationBase remove = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;
                
                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'remove <uuid>'");
                    return result;
                }

                try {
                    compInstall.remove(arg[0]);
                    result = SUCCESS;
                } catch (UnknownImplId e) {
                    printException(e, os);
                } catch (RemoveFailure e) {
                    printException(e, os);
                }
                
                return result;
            }
        };
        operationsList.put("remove", remove);
        
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

    private void printException(java.io.IOException ex,
                                java.io.PrintStream os)
    {
        os.println("I/O Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }


    private void printException(InvalidLocation ex,
                                java.io.PrintStream os)
    {
        os.println("InvalidLocation Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(os));
    }

    private void printException(UnknownImplId ex,
                                java.io.PrintStream os)
    {
        os.print("UnknownImplId : ");
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
        os.println(ex.getMessage());
        ex.printStackTrace();
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
                   + "\t\t  Cdmw ComponentInstallation Admin    \n"
                   + "\t\t---------------------------------------\n"
                   + "\n\n"
                   + INFO_MESSAGE
                  );
    }


    public boolean init() {
        boolean result = false;
        try {
            cdmw.orbsupport.StrategyList orbStrategies = 
                new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies);
            
            compInstall = getComponentInstallation();
            
            registerOperations();
            
            result = true;
        
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        return result;
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


    public void stop() {
        if (orb != null) {
            orb.destroy();
        }
    }
    


    public static void main(String[] args) {
        Admin admin = new Admin(args, System.in, System.out);
        
        if (admin.init()) {
            admin.run();
        }
        
        admin.stop();
    }
    

}



