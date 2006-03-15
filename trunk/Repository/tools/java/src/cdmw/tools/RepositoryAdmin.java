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
import cdmw.common.TypeMismatchException;
import cdmw.commonsvcs.naming.NamingInterface;

import java.util.StringTokenizer;
import java.io.BufferedReader;
import java.io.PrintStream;

public class RepositoryAdmin {

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
    private static final int FACTORY            = 1;
    private static final int H                    = 2;
    private static final int HELP                = 3;
    private static final int REGISTER_OBJECT    = 4;
    private static final int LOAD_FILE            = 5;
    private static final int LIST                = 6;
    private static final int RELEASE_NAME        = 7;
    private static final int VERBOSE            = 8;
    private static final int X                    = 9;
    private static final int UNKNOWN_COMMAND    = 10;

    private static final String[] COMMANDS= {
        "exit",
        "factory",
        "h",
        "help",
        "ln",
        "load",
        "ls",
        "rm",
        "verbose",
        "x"
    };

    private org.omg.CORBA.ORB orb;
    private com.thalesgroup.CdmwNamingAndRepository.Repository repository;
    private boolean verbose;

    public RepositoryAdmin(org.omg.CORBA.ORB orb,
        com.thalesgroup.CdmwNamingAndRepository.Repository rep)
        throws org.omg.CORBA.SystemException {

        Assert.check(orb!=null);
        Assert.check(rep!=null);
        this.orb = orb;
        this.repository = rep;
        this.verbose = false;

    }

    public boolean notSpace(char c) {
        return !Character.isWhitespace(c);
    }

    public String tabString(int level) {
        String tab = "";
        for (int i = 0; i < level; ++i) {
            tab += TAB_VALUE;
        }
        return tab;
    }

    boolean showChunk(org.omg.CosNaming.NamingContext nc,
        org.omg.CosNaming.Binding[] bl, PrintStream out, int level) {

        boolean done = true;

        for (int i = 0; i < bl.length; ++i) {
            // Note from CosNaming specification:
            //   In struct Binding, binding_name is incorrectly defined
            //   as a Name instead of a NameComponent. This definition is
            //   unchanged for compatibility reasons.
            try {
                String tab = tabString(level);
                out.println(tab + NamingInterface.toString(bl[i].binding_name));

                if (bl[i].binding_type.equals(org.omg.CosNaming.BindingType.ncontext)) {
                    out.println("/"); // for contexts
                    // For contexts, first destroy then unbind
                    org.omg.CORBA.Object obj = nc.resolve(bl[i].binding_name);
                    org.omg.CosNaming.NamingContext nc_
                        = org.omg.CosNaming.NamingContextHelper.narrow(obj);
                    done &= showContext(nc_, out, level + 1);
                } else { // object
                    out.println();
                }
            } catch (cdmw.commonsvcs.naming.InvalidNameException ine) {
                // This is impossible ! keep silent (ignore)
                done = false;
            } catch (Exception e) {
                // keep silent (ignore)
                done = false;
            }
        }
        return done;
    }

    // Depth first
    public boolean showContext(org.omg.CosNaming.NamingContext nc,
        PrintStream out, int level) {

        boolean allDone = false;
        // Iterator reference
        org.omg.CosNaming.BindingIteratorHolder it = null;
        // Binding list
        org.omg.CosNaming.BindingListHolder bl = null;
        // Chunk size
        int LIST_CHUNK_SIZE = 500;

        try {
            // Get first chunk
            nc.list(LIST_CHUNK_SIZE,bl,it);
            // Show first chunk
            boolean chunkDone = showChunk(nc, bl.value,out,level);

            // More bindings?
            if (it.value!=null) {
                boolean more;
                do {
                    // Get next chunk
                    more = it.value.next_n(LIST_CHUNK_SIZE,bl);
                    // Show chunk
                    chunkDone &= showChunk(nc, bl.value, out, level);
                // While not done
                } while (more);
                // Clean up iterator
                it.value.destroy();
            }
            allDone = chunkDone;
        } catch (org.omg.CORBA.SystemException se) {
            // keep silent (ignore)
        }
        return allDone;
    }

    public void showSupportedObjects(String[] seq, PrintStream out) {
        out.println("Supported Objects :");
        for (int i = 0;i<seq.length;++i) {
            out.println("\n\t RepositoryId = <" + seq[i] + ">");
        }
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

            case FACTORY:
                return factory(arg,out);

            case H:
                return printHelp(arg,out);

            case HELP:
                return printHelp(arg,out);

            case REGISTER_OBJECT:
                return registerObject(arg,out);

            case LOAD_FILE:
                return loadFile(arg,out);

            case LIST:
                return list(arg,out);

            case RELEASE_NAME:
                return releaseName(arg,out);

            case VERBOSE:
                return toggleVerbose(arg,out);

            case X:
                return exit(arg,out);

            case UNKNOWN_COMMAND:
                return OP_FAILURE;

            default:
                return OP_FAILURE;

        }

    }

    // Available commands
    protected int printHelp(String s, PrintStream out) {
        out.print("+==================+============================================+\n"
               + " | COMMAND          | DESCRIPTION                                |\n"
               + " +==================+============================================+\n"
               + " | x | exit         | exit ADMIN mode                            |\n"
               + " +------------------+--------------------------------------------+\n"
               + " | factory name     | list supported objects of the factory      |\n"
               + " +------------------+--------------------------------------------+\n"
               + " | h | help         | print available commands                   |\n"
               + " +------------------+--------------------------------------------+\n"
               + " | ln ior name      | Bind object whose object reference is 'ior'|\n"
               + " |                  | to 'name' (fully qualified)                |\n"
               + " +------------------+--------------------------------------------+\n"
               + " | load filename    | Load 'filename' batch file                 |\n"
               + " +------------------+--------------------------------------------+\n"
               + " | ls [name_domain] | list content of 'name_domain'              |\n"
               + " |                  | (default list all repository)              |\n"
               + " +------------------+--------------------------------------------+\n"
               + " | rm name          | remove object 'name' from repository       |\n"
               + " +------------------+--------------------------------------------+\n"
               + " | verbose          | toggle verbose mode on/off                 |\n"
               + " +------------------+--------------------------------------------+\n");

        return OP_SUCCESS;
    }

    protected int exit(String s, PrintStream out) {
        return EXIT_ORDERED;
    }

    protected int factory(String s, PrintStream out) {
        int result = OP_FAILURE;

        try {
            org.omg.CosNaming.NamingContext nc
                = repository.resolve_root_context(
                    com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);

            if (s.equals("")) {
                out.println("Factory name needed.");
            } else {
                NamingInterface ni = new NamingInterface(nc);
                org.omg.CORBA.Object obj = ni.resolve(s);
                com.thalesgroup.CdmwLifeCycle.FactoryBase factory
                    = com.thalesgroup.CdmwLifeCycle.FactoryBaseHelper.narrow(obj);

                String[] seq = factory.get_supported_objects();
                showSupportedObjects(seq,out);
                result = OP_SUCCESS;
            }
        } catch (com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext nrc) {
            out.println("Cannot reach DefaultRootContext!");
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            out.println("org.omg.CosNaming.NamingContext.NotFound exception!");
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            out.println("Unexpected Error (CannotProceed exception)!");
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            out.println("Invalid Name !");
        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.getMessage() );
        }

        return result;
    }

    protected int list(String s, PrintStream out) {
        int result = OP_FAILURE;

        try {
            org.omg.CosNaming.NamingContext nc = null;
            org.omg.CosNaming.NamingContext ncRootContext
                = repository.resolve_root_context(
                    com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);

            if (s.equals("")) {
                nc = ncRootContext;
            } else {
                // Check that domain_name is really a Name Domain
                com.thalesgroup.CdmwNamingAndRepository.NameDomain dom
                    = repository.resolve_name_domain(s);
                // It is a name domain, so we should be able to get its naming
                // context.
                NamingInterface ni= new NamingInterface(ncRootContext);
                org.omg.CORBA.Object obj = ni.resolve(s);
                nc = org.omg.CosNaming.NamingContextHelper.narrow(obj);
            }
            result = showContext(nc, out, 0) ? OP_SUCCESS : OP_FAILURE;
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            out.println("Not a NameDomain!");
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            out.println("InvalidName <" + in.reason + ">");
        } catch (Exception e) {
            out.println("Exception : \n" + e.getMessage() );
        }

        return result;
    }

    protected int registerObject(String s, PrintStream out) {

        int result = OP_FAILURE;

        // s contains the following: ior name
        StringTokenizer st = new StringTokenizer(s);

        // ===============
        // Extract ior
        // ===============
        String ior = "";
        if (st.hasMoreTokens()) {
            ior = st.nextToken();
        } else {
            out.println("Object's IOR and name are missing.");
            return result;
        }
        // =================
        // Extract name
        // =================
        String name = "";
        if (st.hasMoreTokens()) {
            name = st.nextToken();
        } else {
            out.println("Object's name is missing.");
            return result;
        }

        try {
            org.omg.CORBA.Object obj = orb.string_to_object(ior);

            String dirname  = NamingInterface.dirname(name);
            String basename = NamingInterface.basename(name);

            if (!dirname.equals("")) {
                try {
                    com.thalesgroup.CdmwNamingAndRepository.NameDomain dom
                        = repository.resolve_name_domain(dirname);

                    dom.register_new_object(basename,obj);
                    out.println("<" + name + "> registered.");
                    result = OP_SUCCESS;
                } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
                    out.println("<" + dirname + "> is not a NameDomain!");
                } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
                    out.println("<" + basename + "> already registered within <" + dirname + ">!");
                } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
                    out.println("BUG: InvalidName exception is received ???!");
                } catch (org.omg.CORBA.SystemException e) {
                    out.println("CORBA System Exception : \n" + e.getMessage());
                }
            } else {
                out.println("NameDomain needed!");
            }
        } catch (cdmw.commonsvcs.naming.InvalidNameException ine) {
            out.println("Invalid Name : <" + ine.what() + ">");
        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.getMessage());
        }

        return result;

    }

    protected int releaseName(String s, PrintStream out) {
        return OP_SUCCESS;
    }

    protected int toggleVerbose(String s, PrintStream out) {
        verbose = !verbose;
        out.println("(verbose mode is " + (verbose ? "on" : "off") + ")");
        return OP_SUCCESS;
    }

    protected int loadFile(String s, PrintStream out) {
        int result = OP_FAILURE;

        try {
            System.out.println(" Loading batch file \"" + s + "\".");
            BufferedReader in
                = new BufferedReader(new java.io.FileReader(s));

            RepositoryAdmin admin = new RepositoryAdmin(orb,repository);
            admin.run(in,out);
            out.println("Batch file \"" + s + "\" done.");
            result = OP_SUCCESS;

        } catch (org.omg.CORBA.SystemException se) {
            out.println("CORBA System Exception : \n" + se.getMessage());
        } catch (java.io.IOException ioe) {
            out.println("Could not open file \"" + s + "\".");
        }

        return result;
    }

    public void run(BufferedReader in,PrintStream out) {
        printInfo(out);
        printPrompt(out);

        String s;
        try {
            while((s = in.readLine()) != null) {
                StringTokenizer st = new StringTokenizer(s);
                String command = "";
                String args = "";
                if (st.hasMoreTokens()) {
                    command = st.nextToken();
                }
                while (st.hasMoreTokens()) {
                    args += st.nextToken();
                    if (st.hasMoreTokens()) args += " ";
                }

                System.out.println("@cmd = <" + command
                                        + "> @args = <" + args + ">");
                if (runOperation(command,args,out) == EXIT_ORDERED) break;
                printPrompt(out);
            }
        } catch(java.io.IOException ioe) {
            ioe.printStackTrace(out);
        }

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
           + "\t\t Cdmw Naming & Repository administration console \n"
           + "\t\t--------------------------------------------------\n"
           + "\n\n"
           + INFO_MESSAGE
           + "\nInfo: (verbose mode is " + (verbose ? "on" : "off") + ")");
    }

}

