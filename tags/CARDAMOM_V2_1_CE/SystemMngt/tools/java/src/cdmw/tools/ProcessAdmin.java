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

import java.util.StringTokenizer;
import java.io.BufferedReader;
import java.io.PrintStream;

import cdmw.common.Assert;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.ORBUtils;
import cdmw.orbsupport.ExceptionMinorCodes;

import com.thalesgroup.CdmwPlatformMngtBase.StartupKind;
import com.thalesgroup.CdmwPlatformMngtBase.StateRetrieval;


public class ProcessAdmin {

    public static final String INFO_MESSAGE 
        = "Type h or help for a list of available commands.";
    public static final String PROMPT_MESSAGE 
        = "\nADMIN>";
    public static final String UNKNOWN_COMMAND_MESSAGE 
        = "Bad command!>";

    public static final String TAB_VALUE = "    ";

    public static final int EXIT_ORDERED = -10;
    public static final int OP_SUCCESS = 0;
    public static final int OP_FAILURE = -1;

    private static final int EXIT                = 0;
    private static final int GET_ACTIVITY_POINT = 1;
    private static final int GET_PULL            = 2;
    private static final int GET_PUSH            = 3;
    private static final int GET_SERVICE        = 4;
    private static final int H                    = 5;
    private static final int HELP                = 6;
    private static final int INIT                = 7;
    private static final int LOAD                = 8;
    private static final int NB_ACTIVITY_POINTS    = 9;
    private static final int NB_STEPS            = 10;
    private static final int NEXT                = 11;
    private static final int RUN                = 12;
    private static final int STOP                = 13;
    private static final int VERBOSE            = 14;
    private static final int X                    = 15;
    private static final int UNKNOWN_COMMAND    = 16;
    
    private static final String[] COMMANDS= {
        "exit",
        "get_activity_point",
        "get_pull",
        "get_push",
        "get_service",
        "h",
        "help",
        "init",
        "load",
        "nb_activity_points",
        "nb_steps",
        "next",
        "run",
        "stop",
        "verbose",
        "x"
    };
    
    private org.omg.CORBA.ORB orb;
    private com.thalesgroup.CdmwPlatformMngt.ProcessDelegate process_delegate;
    private boolean verbose;
    
    public ProcessAdmin(org.omg.CORBA.ORB orb, 
        com.thalesgroup.CdmwPlatformMngt.ProcessDelegate process_delegate) {
    
        Assert.check(orb!=null);
        Assert.check(process_delegate!=null);        
        this.orb = orb;
        this.process_delegate = process_delegate;
        this.verbose = false;
    
    }

    public int getOperation(String command) {
        
        for(int i=0; i<COMMANDS.length; i++) {
            if (command.equals(COMMANDS[i])) {
                return i;
            }
        }
        return UNKNOWN_COMMAND;

    }

    public int runOperation(String op, String arg, PrintStream out) {
        
        switch(getOperation(op)) {
            case EXIT:
                return exit(arg,out);

            case GET_PULL:
                return getPullMonitorable(arg,out);

            case GET_PUSH:
                return getPushMonitorable(arg,out);

            case GET_SERVICE:
                return getService(arg,out);

            case H:
                return printHelp(arg,out);

            case HELP:
                return printHelp(arg,out);

            case INIT:
                return initialise(arg,out);

            case LOAD:
                return toggleVerbose(arg,out);

            case NB_STEPS:
                return getNbSteps(arg,out);
                
            case NEXT:
                return nextStep(arg,out);
            
            case RUN:
                return runOrder(arg,out);
                
            case STOP:
                return stopOrder(arg,out);
                
            case VERBOSE:
                return toggleVerbose(arg,out);
                
            case X:
                return exit(arg,out);

            default:
                return OP_FAILURE;

        }
        
    }
    
    public void run(BufferedReader in,PrintStream out) {
        printInfo(out);
        printPrompt(out);

        String s;
        try {
            String command, arg;
            while((s = in.readLine()) != null) {
                StringTokenizer st = new StringTokenizer(s);
                command = ""; arg = "";
                if (st.hasMoreTokens()) {
                    command = st.nextToken();
                }
                while (st.hasMoreTokens()) {
                    arg += st.nextToken();
                    if (st.hasMoreTokens()) arg += " ";
                }

                System.out.println("@cmd = <" + command 
                                        + "> @args = <" + arg + ">");
                if (runOperation(command,arg,out) == EXIT_ORDERED) break;
                printPrompt(out);
            }
        } catch(java.io.IOException ioe) {
            ioe.printStackTrace(out);
        }

    }

    // Available commands
    protected int printHelp (String arg, PrintStream out) {
    
        out.println(
            " +=====================+============================================+\n"
          + " | COMMAND             | DESCRIPTION                                |\n"
          + " +=====================+============================================+\n"
          + " | x | exit            | exit ADMIN mode                            |\n"
          + " +---------------------+--------------------------------------------+\n"        
          + " | get_activity_point [index] |  print index'th activity point .    |\n"                
          + " |                            | default is all.                     |\n"                
          + " +---------------------+--------------------------------------------+\n"
          + " | get_pull [check]    | Returns the pull monitorable interface.    |\n"
          + " |                     | check: Checks if the process is alive.     |\n" 
          + " +---------------------+--------------------------------------------+\n"
          + " | get_push            | Returns the push monitorable interface.    |\n"
          + " +---------------------+--------------------------------------------+\n"
          + " | get_service         | Returns embedded service.                  |\n"
          + " +---------------------+--------------------------------------------+\n"
          + " | h | help            | print available commands                   |\n"
          + " +---------------------+--------------------------------------------+\n"
          + " | init state_retrieval startup_mode [state_id] |                   |\n"
          + " |                     |                                            |\n"
          + " |                     |   state_retrieval= LAST|RECOVER|SPECIFIED  |\n"
          + " |                     |   startup_mode: short                      |\n"
          + " |                     |   state_id= state id for SPECIFIED         |\n"
          + " +---------------------+--------------------------------------------+\n"
          + " | load filename       | Load 'filename' batch file                 |\n"
          + " +---------------------+--------------------------------------------+\n"        
          + " | nb_activity_points  | Returns the number of activity points of   |\n"
          + " |                     | the managed process.                       |\n"
          + " +---------------------+--------------------------------------------+\n"
          + " | nb_steps            | Returns the number of initialisation steps |\n"
          + " |                     | of the managed process.                    |\n"        
          + " +---------------------+--------------------------------------------+\n"
          + " | next                | Requests the process to perform its next   |\n"
          + " |                     | step.                                      |\n"        
          + " +---------------------+--------------------------------------------+\n"
          + " | run                 | Requests the process to run.               |\n"        
          + " +---------------------+--------------------------------------------+\n"
          + " | stop                | Requests the process to stop properly.     |\n"        
          + " +---------------------+--------------------------------------------+\n"
          + " | verbose             | toggle verbose mode on/off                 |\n"
          + " +---------------------+--------------------------------------------+\n");
    
        return OP_SUCCESS;
    
    }
    protected int exit (String arg, PrintStream out) {
        return EXIT_ORDERED;
    }
    
    protected int getNbSteps(String arg, PrintStream out) {
        int result = OP_FAILURE;    

        try {        
            int nb = process_delegate.nb_steps();
            out.println("Number of initialisation steps :" + nb );
            result = OP_SUCCESS;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.toString() );
        }

        return result;
    }
    
    protected int getService(String arg, PrintStream out) {
        int result = OP_FAILURE;    
        
        try {
            org.omg.CORBA.Object service = process_delegate.get_service();
            if (service==null) {
                out.println("No embedded service." );
            } else {
                String s = orb.object_to_string(service);
                out.println("Embedded service : " + s );
            }
            result = OP_SUCCESS;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.toString() );
        }

        return result;
    }
    
    protected int getPullMonitorable(String arg, PrintStream out) {
        int result = OP_FAILURE;    

        try {
            org.omg.FT.PullMonitorable pull 
                = process_delegate.get_pull_monitorable();
            if (pull==null) {
                out.println("Pull monitoring not supported." );
            } else {
                String s = orb.object_to_string(pull);
                out.println("Pull monitoring object : " + s );
                if ( arg.equals("check") ) {
                    out.println("Checking object ...");
                    try {
                        pull.is_alive();
                        out.println("OK." );
                    } catch (org.omg.CORBA.SystemException se) {
                        out.println("NON OK.\n\t<CORBA System Exception :"
                            + se.toString() + ">" );
                    }
                }
            }
            result = OP_SUCCESS;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.toString() );
        }

        return result;    
    }
    
    protected int getPushMonitorable(String arg, PrintStream out) {
        int result = OP_FAILURE;    

        try {
            com.thalesgroup.CdmwPlatformMngt.PushMonitorable push 
                = process_delegate.get_push_monitorable();
            if (push==null) {
                out.println("Push monitoring not supported." );
            } else {
                String s = orb.object_to_string(push);
                out.println("Push monitoring object : " + s );
            }
            result = OP_SUCCESS;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.toString() );
        }

        return result;
    
    }
    
    protected int initialise(String arg, PrintStream out) {
        int result = OP_FAILURE;    
    
        try {
            // arg contains the following:
            //   state_retrieval startup_mode state_id
            //
            StartupKind startupKind = new StartupKind();
            java.util.StringTokenizer st = new java.util.StringTokenizer(arg);
            // =======================
            // Extract state_retrieval
            // =======================            
            if ( st.hasMoreTokens() ) {
                String retrieval = st.nextToken();
                if ( retrieval.equals("LAST") ) {
                    startupKind.state_retrieval = StateRetrieval.LAST_STATE;
                } else if ( retrieval.equals("RECOVER") ) {
                    startupKind.state_retrieval = StateRetrieval.RECOVER_STATE;
                } else if ( retrieval.equals("SPECIFIED") ) {
                    startupKind.state_retrieval = StateRetrieval.SPECIFIED_STATE;
                } else {
                    out.println("Invalid value for state retrieval. "
                       + "Allowed values are LAST|RECOVER|SPECIFIED.");
                    return result;
                }
            } else {
                out.println("StateRetrieval is missing." );
                return result;
            }
            
            // ====================
            // Extract startup_mode
            // ====================
            // copy arguments
            if ( st.hasMoreTokens() ) {
                String mode = st.nextToken();
                try {
                    startupKind.startup_mode = Short.valueOf(mode).shortValue();
                } catch(NumberFormatException nfe) {
                    out.println("Bad value for StartupMode.");
                    return result;
                }
            } else {
                out.println("StartupMode is missing." );
                return result;
            }
            
            if (startupKind.state_retrieval.value() 
                == StateRetrieval._SPECIFIED_STATE) {
                // ================
                // Extract state_id
                // ================
                // copy arguments
                if ( st.hasMoreTokens() ) {
                    startupKind.specified_state_id = st.nextToken();
                } else {
                    out.println("Specified State Id is missing." );
                    return result;
                }
            } else {
                startupKind.specified_state_id = "";
            }
        
            out.println("Sending <initialise> order...");
            process_delegate.initialise(startupKind);
            out.println("done." );
            
            result = OP_SUCCESS;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.toString() );
        } catch(com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.BadOrder bo) {
            out.println("BadOrder Exception : \n" + bo.toString());;
        }
    
        return result;
    }
    
    protected int nextStep(String arg, PrintStream out) {
        int result = OP_FAILURE;    
        
        try {
            out.println("Sending <next_step> order...");
            process_delegate.next_step();
            out.println("done." );

            result = OP_SUCCESS;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.toString() );
        } catch (com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.InvalidStep is) {
            out.println("InvalidStep Exception : \n" + is.toString() );
        } catch (com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.BadOrder bo) {
            out.println("BadOrder Exception : \n" + bo.toString() );
        }

        return result;
    }
    
    protected int runOrder(String arg, PrintStream out) {
        int result = OP_FAILURE;    

        try {
            out.println("Sending <run> order...");
            process_delegate.run();
            out.println("done." );

            result = OP_SUCCESS;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.toString() );
        } catch (com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun nrtr) {
            out.println("NotReadyToRun Exception : \n" + nrtr.toString() );
        } catch (com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.AlreadyDone ad) {
            out.println("AlreadyDone Exception : \n" + ad.toString());
        }

        return result;   
    }
    
    protected int stopOrder(String arg, PrintStream out) {
        int result = OP_FAILURE;
        
        try {
            out.println("Sending <stop> order...");
            process_delegate.stop();
            out.println("done." );

            result = OP_SUCCESS;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.toString() );
        }

        return result;
    }
    
    protected int toggleVerbose(String arg, PrintStream out) {
        verbose = !verbose;
        out.println("(verbose mode is " + (verbose ? "on" : "off") + ")");
        return OP_SUCCESS;
    }
    
    protected int loadFile(String arg, PrintStream out) {
        int result = OP_FAILURE;
        try {
            System.out.println(" Loading batch file \"" + arg + "\".");
            BufferedReader in 
                = new BufferedReader(new java.io.FileReader(arg));

            ProcessAdmin admin = new ProcessAdmin(orb,process_delegate);
            admin.run(in,out);
            out.println("Batch file \"" + arg + "\" done.");
            result = OP_SUCCESS;

        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.getMessage());
        } catch (java.io.IOException ioe) {
            out.println("Could not open file \"" + arg + "\".");
        }
        
        return result;
    }

    private void unknownCommand(PrintStream out) {
        out.println(UNKNOWN_COMMAND_MESSAGE);
    }

    private void printPrompt(PrintStream out) {
        out.println(PROMPT_MESSAGE);
    }
    
    private void printInfo(PrintStream out) {
        out.println("\n\n"
           + "\t\t--------------------------------------------------\n"
           + "\t\t PlatformManagement Process administration console \n"
           + "\t\t--------------------------------------------------\n"
           + "\n\n"
           + INFO_MESSAGE
           + "\nInfo: (verbose mode is " + (verbose ? "on" : "off") + ")");
    }
    
    public static void main(String[] args) {
    
        org.omg.CORBA.ORB orb = null;
        String procUrl = "";
    
        if( args.length==1 ) {
            usage(System.out);
            System.exit(-1);
        }
    
        //
        // Execute command
        //
        if ( args[1].equals("--help") ) {
            usage(System.out);
            return;
        } else if ( args[1].equals("--url") ) {
            if( args.length != 2) {
                usage(System.out);
                System.exit(-1);
            } else {
                procUrl = args[2];
            }
        } else {
            usage(System.out);
            System.exit(-1);
        }
    
        try {
            orb = org.omg.CORBA.ORB.init(args, ORBUtils.getProperties());
            run(orb, procUrl);
        } catch(Exception ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    
        if (orb!=null) {
            try {
                orb.destroy();
            } catch(Exception ex) {
                ex.printStackTrace();
                System.exit(-1);
            }
        }
    
    }

    public static void run(org.omg.CORBA.ORB orb, String url) {
        // Retrieve Process object reference
        com.thalesgroup.CdmwPlatformMngt.ProcessDelegate proc 
            = getCdmwProcess(orb,url);
        ProcessAdmin adm = new ProcessAdmin (orb,proc);        
        adm.run(new BufferedReader(new java.io.InputStreamReader(System.in)),
            System.out);
    }
    
    public static void usage(PrintStream out) {
        out.println("Usage:\n"
            + "java cdmw.tools.ProcessAdmin options\n"
            + "\n"
            + "Options:\n"
            + "--help       Display this information.\n"
            + "--url  IOR   URL of the CDMW Process object.\n");
    }
    
    private static com.thalesgroup.CdmwPlatformMngt.ProcessDelegate 
        getCdmwProcess(org.omg.CORBA.ORB orb, String url) 
        throws org.omg.CORBA.TRANSIENT {
        
        Assert.check(orb!=null);
        org.omg.CORBA.Object obj = null;
        
        //
        // Get naming service
        //
        try {
            obj = orb.string_to_object(url);
        } catch(Exception e) {
            System.out.println("Invalid URL");
            throw new org.omg.CORBA.TRANSIENT( ExceptionMinorCodes.TRANSIENT, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        if(obj==null) {
            System.out.println("URL denotes a nil object reference");
            throw new org.omg.CORBA.TRANSIENT( ExceptionMinorCodes.TRANSIENT, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        // Get reference to the process interface
        com.thalesgroup.CdmwPlatformMngt.ProcessDelegate proc = null;
        try {
            proc = com.thalesgroup.CdmwPlatformMngt.ProcessDelegateHelper.narrow(obj);
            if (proc==null) {
                System.out.println("Couldn't retrieve reference of the Process");
                throw new org.omg.CORBA.TRANSIENT( ExceptionMinorCodes.TRANSIENT, 
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            System.out.println("Couldn't retrieve reference of the Process");
            throw new org.omg.CORBA.TRANSIENT( ExceptionMinorCodes.TRANSIENT, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        return proc;
    }

}

