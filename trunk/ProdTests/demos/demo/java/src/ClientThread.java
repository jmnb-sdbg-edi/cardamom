/* =========================================================================== *
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
 * =========================================================================== */


package cdmw.lifecycle;

import java.util.StringTokenizer;

import java.io.StringReader;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.InputStream;
import java.io.PrintStream;
import java.io.IOException;

//import cdmw.event.EventServiceTest;

import com.thalesgroup.CdmwNamingAndRepository.Repository;

public class ClientThread extends Thread {

    public static final String INFO_MESSAGE =
        "Type h or help for a list of available commands.";
    public static final String PROMPT_MESSAGE = "\n%>";
    public static final String UNKNOWN_COMMAND_MESSAGE = "Bad command!";

    public static final int EXIT_ORDERED     = -10;
    public static final int OP_SUCCESS       = 0;
    public static final int OP_FAILURE       = -1;

    private static final int ALL                = 0;
    private static final int ENTITY             = 1;
    private static final int ENTITY_RESTART     = 2;
    private static final int EXIT               = 3;
    private static final int H                  = 4;
    private static final int HELP               = 5;
    private static final int SERVICE            = 6;
    private static final int SESSION            = 7;
    private static final int UNMANAGED          = 8;
    private static final int UNMANAGED_RESTART  = 9;
    private static final int VERBOSE            = 10;
    private static final int X                  = 11;
    private static final int STOP_PROC          = 12;
    private static final int SLEEP              = 13;
    private static final int EVENT              = 14;
    private static final int UNKNOWN_COMMAND    = 15;

    // should be sorted alphabetically (by operation name)
    private static final String[] COMMANDS= {
        "all",
        "entity",
        "entity_restart",
        "exit",
        "h",
        "help",
        "service",
        "session",
        "unmanaged",
        "unmanaged_restart",
        "verbose",
        "x",
        "stop",
        "sleep",
        "event"
    };

    private org.omg.CORBA.ORB orb;
    private Repository repository;
    private boolean verbose;
    private boolean interactive;
    private BufferedReader is;
    private PrintStream out;

    public ClientThread(org.omg.CORBA.ORB orb,Repository rep,
                        BufferedReader reader,PrintStream out) {

        this.orb = orb;
        this.repository = rep;
        this.verbose = false;
        this.interactive = true;
        this.is = reader;
        this.out = out;

    }

    public void run() {
        run(this.is,this.out);
    }

    public void run(BufferedReader is,PrintStream out) {

        out.println("Starting client...");
        printInfo(out);
        printPrompt(out);
        String s;
        try {
            while((s = is.readLine().trim()) != null) {
                String command = null;
                String arg = null;
                int ind = s.indexOf(' ');
                if (ind != -1) {
                    command = s.substring(0, ind);
                    if (!interactive) out.print(command);
                    arg = s.substring(ind + 1);
                    if (!interactive) out.println(" " + arg);
                } else {
                    command = s;
                    if (!interactive) out.print(command);
                }

                if (runOperation(command,arg,out) == EXIT_ORDERED) break;
                printPrompt(out);
            }
        } catch(IOException ioe) {
            ioe.printStackTrace(out);
        }

    }

    // Available commands
    protected int printHelp(String arg,PrintStream out) {

        out.println(
        "\n +======================+============================================+\n"
        + " | COMMAND              | DESCRIPTION                                |\n"
        + " +======================+============================================+\n"
        + " | all                  | test all the 4 strategies                  |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | entity               | test the Entity strategy                   |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | entity_restart opt   | test restart of the Entity strategy        |\n"
        + " |                      | opt = create|start|stop|remove|all         |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | x | exit             | exit ClientThread mode                     |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | event                | test CDMW Event service                   |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | h | help             | print available commands                   |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | service              | test the Service strategy                  |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | session              | test the Session strategy                  |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | sleep n              | Sleeps for n milliseconds                  |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | stop  procAgentFac   | Stops a process hosting the ProcessAgent   |\n"
        + " |                      | factory.                                   |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | unmanaged            | test the UnManagedEntity strategy          |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | unmanaged_restart opt| test restart of UnManagedEntity strategy   |\n"
        + " |                      | opt = create|start|stop|remove|all         |\n"
        + " +----------------------+--------------------------------------------+\n"
        + " | verbose              | toggle verbose mode on/off                 |\n"
        + " +----------------------+--------------------------------------------+");

         return OP_SUCCESS;

    }

    protected int exit(PrintStream out) {
        // ORB shutdown is now done in CdmwProcessImpl
        return EXIT_ORDERED;
    }

    protected int toggleVerbose(PrintStream out) {
        verbose = !verbose;
        out.println("(verbose mode is " + (verbose ? "on" : "off") + ")");
        return OP_SUCCESS;
    }

    protected int stopProc(String arg, PrintStream out) {

        int result = OP_FAILURE;

        try {
            org.omg.CosNaming.NamingContext nc = repository.resolve_root_context(
                com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);

            if (arg.length() == 0) {
                out.println("Factory name needed.");
            } else {
                cdmw.commonsvcs.naming.NamingInterface ni =
                    new cdmw.commonsvcs.naming.NamingInterface(nc);

                com.thalesgroup.Validation.ProcessAgentFactory factory
                    = com.thalesgroup.Validation.ProcessAgentFactoryHelper.narrow(
                        ni.resolve(arg));

                // Create a ProcessAgent object
                com.thalesgroup.Validation.ProcessAgent procAgent
                    = factory.create_process_agent();
                if (procAgent == null) {
                    out.println("Nil ProcessAgent reference!");
                } else {
                    procAgent.stop_process();
                }
                result = OP_SUCCESS;
            }
        } catch (com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext nrc) {
            out.println("Cannot reach DefaultRootContext!");
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            out.println("org.omg.CosNaming.NamingContextPackage.NotFound exception!");
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            out.println("Unexpected Error (CannotProceed exception)!");
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            out.println("Invalid Name <" + arg + ">!");
        } catch (org.omg.CORBA.SystemException se) {
            se.printStackTrace();
            out.println("CORBA System Exception : \n" + se.toString() );
        }

        return result;
    }

    protected int sleep(String arg, PrintStream out) {

        int result = OP_FAILURE;

        try {
            if (arg.length() == 0) {
                out.println("Number of milliseconds needed.");
            } else {
                try {
                    int n = Integer.parseInt(arg);
                    if (verbose) {
                        out.println("Sleeping for " + n + " milliseconds...");
                    }
                    cdmw.ossupport.OS.sleep(
                        cdmw.testutils.Testable.getTimescale() * n);
                    result = OP_SUCCESS;
                } catch(NumberFormatException nfe) {
                    out.println("Invalid number of milliseconds!");
                }
            }
        } catch (Exception e) {
            out.println("Exception : " + e.toString());
        }

        return result;
    }

    protected int testEntity(String arg, PrintStream out) {
        EntityStrategyTest entity = new EntityStrategyTest(arg,out);
        entity.start();
        return OP_SUCCESS;
    }

    protected int testEntityRestart(String arg, PrintStream out) {
        EntityStrategyRestartTest entityRestart
            = new EntityStrategyRestartTest(arg,out);
        entityRestart.start();
        return OP_SUCCESS;
    }


    protected int testUnManagedEntity(String arg, PrintStream out) {
        UnManagedEntityStrategyTest unmngEntity
            = new UnManagedEntityStrategyTest(arg,out);
        unmngEntity.start();
        return OP_SUCCESS;
    }

    protected int testUnManagedEntityRestart(String arg, PrintStream out) {
        UnManagedEntityStrategyRestartTest unmngEntityRestart
            = new UnManagedEntityStrategyRestartTest(arg,out);
        unmngEntityRestart.start();
        return OP_SUCCESS;
    }

    protected int testSession(String arg, PrintStream out) {
        SessionStrategyTest session
            = new SessionStrategyTest(arg,out);
        session.start();
        return OP_SUCCESS;
    }

    protected int testService(String arg, PrintStream out) {
        ServiceStrategyTest service
            = new ServiceStrategyTest(arg,out);
        service.start();
        return OP_SUCCESS;
    }

    /*
    protected int testEvent(String arg, PrintStream out) {
        try {
            org.omg.CosNaming.NamingContext nc =
                cdmw.namingandrepository.RepositoryInterface.getDomainContext(
                    cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN);
            EventServiceTest event = new EventServiceTest(orb, nc);
            event.start();
            return OP_SUCCESS;
        } catch(com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            return OP_FAILURE;
        } catch(com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            return OP_FAILURE;
        }
    }
    */

    protected int testAll(String arg, PrintStream out) {
        if (testEntity(arg, out) == OP_FAILURE) {
            return OP_FAILURE;
        }
        if (testUnManagedEntity(arg, out) == OP_FAILURE) {
            return OP_FAILURE;
        }
        if (testSession(arg, out) == OP_FAILURE) {
            return OP_FAILURE;
        }
        if (testService(arg, out) == OP_FAILURE ) {
            return OP_FAILURE;
        }
        /*
        if (testEvent(arg, out) == OP_FAILURE ) {
            return OP_FAILURE;
        }
        */

        return OP_SUCCESS;
    }

    private int runOperation(String op,String arg,PrintStream o) {

        switch( getOperation(op) ) {

            case ALL:
                return testAll(arg, o);

            case ENTITY:
                return testEntity(arg, o);

            case ENTITY_RESTART:
                return testEntityRestart(arg, o);

            case EXIT:
                return exit(o);

            case H:
                return printHelp(arg, o);

            case HELP:
                return printHelp(arg, o);

            case SERVICE:
                return testService(arg, o);

            case SESSION:
                return testSession(arg, o);

            case UNMANAGED:
                return testUnManagedEntity(arg, o);

            case UNMANAGED_RESTART:
                return testUnManagedEntityRestart(arg, o);

            case VERBOSE:
                return toggleVerbose(o);

            case X:
                return exit(o);

            case STOP_PROC:
                return stopProc(arg, o);

            case SLEEP:
                return sleep(arg, o);

            /*
            case EVENT:
                return testEvent(arg, o);
            */

            case UNKNOWN_COMMAND:
                unknownCommand(o);
                break;

            default :
                if(!op.equals("")) {
                    unknownCommand(o);
                }
        }

        return OP_FAILURE;

    }

    private int getOperation(String op) {
        for(int i=0; i< COMMANDS.length; i++) {
            if (COMMANDS[i].equals(op)) {
                return i;
            }
        }
        return UNKNOWN_COMMAND;
    }

    private void unknownCommand(PrintStream out) {
        out.println(UNKNOWN_COMMAND_MESSAGE);
    }

    private void printPrompt(PrintStream out) {
        out.print(PROMPT_MESSAGE);
        out.flush();
    }

    private void printInfo(PrintStream out) {

        out.println("\n\n"
            + "\t\t---------------------------------------\n"
            + "\t\t  Cdmw LifeCycle Service Test Driver  \n"
            + "\t\t---------------------------------------\n"
            + "\n\n"
            + INFO_MESSAGE
            + "\nInfo: (verbose mode is "
            + (this.verbose ? "on" : "off") + ")");

    }


}

