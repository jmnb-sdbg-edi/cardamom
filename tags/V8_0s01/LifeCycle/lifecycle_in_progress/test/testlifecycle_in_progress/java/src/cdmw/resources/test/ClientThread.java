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


package cdmw.resources.test;

import java.util.StringTokenizer;

import java.io.StringReader;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.InputStream;
import java.io.PrintStream;
import java.io.IOException;

import com.thalesgroup.CdmwNamingAndRepository.Repository;

import org.omg.CORBA.ORB;
import org.omg.CORBA.SystemException;

public class ClientThread extends Thread {

    public static final String INFO_MESSAGE =
        "Type h or help for a list of available commands.";
    public static final String PROMPT_MESSAGE = "\n%>";
    public static final String UNKNOWN_COMMAND_MESSAGE = "Bad command!";

    public static final int EXIT_ORDERED     = -10;
    public static final int OP_SUCCESS         = 0;
    public static final int OP_FAILURE         = -1;

    private static final int ALL                = 0;
    private static final int ENTITY                = 1;
    private static final int ENTITY_RESTART        = 2;
    private static final int EXIT                = 3;
    private static final int H                    = 4;
    private static final int HELP                = 5;
    private static final int SERVICE            = 6;
    private static final int SESSION            = 7;
    private static final int UNMANAGED            = 8;
    private static final int UNMANAGED_RESTART    = 9;
    private static final int VERBOSE            = 10;
    private static final int X                    = 11;
    private static final int UNKNOWN_COMMAND    = 12;

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
        "x"
    };

    private ORB orb;
    private Repository repository;
    private boolean verbose;
    private boolean interactive;
    private BufferedReader is;
    private PrintStream out;

    public ClientThread(ORB orb,Repository rep,
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
            while((s = is.readLine()) != null) {
                StringTokenizer st = new StringTokenizer(s);
                String command = null;
                String arg = null;
                if (st.hasMoreTokens()) {
                    command = st.nextToken();
                    if (!interactive) out.print(command);
                }
                if (st.hasMoreTokens()) {
                    arg = st.nextToken();
                    if (!interactive) out.print(" "+arg);
                }
                if (!interactive) out.println();

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
        "\n +======================+============================================+" + "\n"
        + " | COMMAND              | DESCRIPTION                                |" + "\n"
        + " +======================+============================================+" + "\n"
        + " | all                  | test all the 4 strategies                  |" + "\n"
        + " +----------------------+--------------------------------------------+" + "\n"
        + " | entity               | test the Entity strategy                   |" + "\n"
        + " +----------------------+--------------------------------------------+" + "\n"
        + " | entity_restart opt   | test restart of the Entity strategy        |" + "\n"
        + " |                      | opt = create|start|stop|remove|all         |" + "\n"
        + " +----------------------+--------------------------------------------+" + "\n"
        + " | x | exit             | exit ClientThread mode                     |" + "\n"
        + " +----------------------+--------------------------------------------+" + "\n"
        + " | h | help             | print available commands                   |" + "\n"
        + " +----------------------+--------------------------------------------+" + "\n"
        + " | service              | test the Service strategy                  |" + "\n"
        + " +----------------------+--------------------------------------------+" + "\n"
        + " | session              | test the Session strategy                  |" + "\n"
        + " +----------------------+--------------------------------------------+" + "\n"
        + " | unmanaged            | test the UnManagedEntity strategy          |" + "\n"
        + " +----------------------+--------------------------------------------+" + "\n"
        + " | unmanaged_restart opt| test restart of UnManagedEntity strategy   |" + "\n"
        + " |                      | opt = create|start|stop|remove|all         |" + "\n"
        + " +----------------------+--------------------------------------------+" + "\n"
        + " | verbose              | toggle verbose mode on/off                 |" + "\n"
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

        return OP_SUCCESS;
    }

    private int runOperation(String op,String arg,PrintStream o) {

        switch( getOperation(op) ) {

            case ALL:
                return testAll(arg,o);

            case ENTITY:
                return testEntity(arg,o);

            case ENTITY_RESTART:
                return testEntityRestart(arg,o);

            case EXIT:
                return exit(o);

            case H:
                return printHelp(arg,o);

            case HELP:
                return printHelp(arg,o);

            case SERVICE:
                return testService(arg,o);

            case SESSION:
                return testSession(arg,o);

            case UNMANAGED:
                return testUnManagedEntity(arg,o);

            case UNMANAGED_RESTART:
                return testUnManagedEntityRestart(arg,o);

            case VERBOSE:
                return toggleVerbose(o);

            case X:
                return exit(o);
                
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

