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


package cdmw.trace.test;


class ClientThread extends Thread {

    public static final String TRACE_LIBRARY_IOR_FILE = "traceLibrary.ior";

    private static final String INFO_MESSAGE = "Type h or help for a list of available commands.";
    private static final String PROMPT_MESSAGE = "\n%>";
    private static final String UNKNOWN_COMMAND_MESSAGE = "Bad command!>";

    // Definition of a basic operation.
    // Each operation must redefine the method execute.
    abstract class OperationBase {
        public static final int SUCCESS       = 0;
        public static final int FAILURE      = -1;
        public static final int EXIT_ORDERED = -10;

        public abstract int execute(String[] arg, java.io.PrintStream os);
    }


    // List of all operation.
    // Key is operation name; an operation could be present
    // several time with different operation name
    private java.util.Hashtable operationsList;

    // ORB
    private org.omg.CORBA.ORB orb;

    // verbose mode
    private boolean verbose;

    // Input stream
    java.io.InputStream istream;

    // Output stream
    java.io.PrintStream ostream;

    // root context
    String rootContext;

    // Trace Collector Servant context
    String collectorContext;

    // Naming and repository reference
    com.thalesgroup.CdmwNamingAndRepository.Repository repository;

    // Servant object reference
    com.thalesgroup.CdmwTrace.TraceProducer traceProducer;

    // Target thread
    TargetThread2 theTarget;





    public ClientThread(org.omg.CORBA.ORB orb,
                        java.io.InputStream is,
                        java.io.OutputStream os,
                        com.thalesgroup.CdmwNamingAndRepository.Repository rep)
    {
        super("ClientThread");

        // members initialization
        this.orb = orb;
        this.verbose = false;
        this.istream = is;
        this.ostream = new java.io.PrintStream(os);
        this.rootContext =
            com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value;
        this.repository = rep;
        this.theTarget = null;

        registerOperations();

        String fileName = TRACE_LIBRARY_IOR_FILE;

        // import the object reference from the file
        org.omg.CORBA.Object obj =
            orb.string_to_object("relfile:/" + fileName);
        if (obj == null) {
            ostream.println("Error opening reference filename :" + fileName);
        }

        traceProducer =
            com.thalesgroup.CdmwTrace.TraceProducerHelper.narrow(obj);

        if (traceProducer == null) {
            ostream.println("Could not narrow object to type TraceProducer");
        }

        // initialize trace collector context
        // Pattern is :
        //    CDMW/SERVICES/TRACE/COLLECTORS/
        collectorContext  = cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN;
        collectorContext += "/";
        collectorContext += cdmw.common.Locations.TRACE_COLLECTORS_NAME_DOMAIN;
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
                  + " | set_root_context <name>           | set root context for trace collector       |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | set_collector_context <name>      | set context for trace collector            |"  + "\n"
                  + " |                                   | default is CDMW/SERVICES/TRACE/COLLECTORS/ |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | activate_level                    | activate the level    all-> [* -1]         |"  + "\n"
                  + " |  [componentname] <domain> <level> |                                            |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | deactivate_level                  | deactivate the level                       |"  + "\n"
                  + " |  [componentname] <domain> <level> |                                            |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | get_levels                        | get levels                                 |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | register_collector                |                                            |"  + "\n"
                  + " |  <name> <mnemonic>                | register the collector                     |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | unregister_collector <ident>      | unregister the collector                   |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | get_collectors                    | get collectors                             |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | start_flushing                    | start flushing trace message               |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | stop_flushing                     | stop flushing trace message                |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | start_target                      | start target with trace                    |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | stop_target                       | stop target with trace                     |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | start_stop_targets                | start (and stop) targets with trace        |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | start_domain_target               | start (and stop) target with domain trace  |"  + "\n"
                  + " +-----------------------------------+--------------------------------------------+"  + "\n"
                  + " | sleep_process <time>              | set process to sleep, time in msec         |"  + "\n"
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
                try {
                    if (verbose)
                        os.println("Initiating ORB shutdown.");
                    orb.shutdown(false);
                } catch (org.omg.CORBA.SystemException ex) {
                    printException(ex,os);
                }
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


        // ------- set_root_context -------
        OperationBase setRootContext = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;

                // if undefined context name set defaut root context
                if (arg.length == 0) {
                    rootContext =
                        com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value;
                } else {
                    rootContext = arg[0];
                }

                os.println("Context set to : " + rootContext);

                try {
                    org.omg.CosNaming.NamingContext nc =
                          repository.resolve_root_context(rootContext);

                    result = SUCCESS;
                } catch (com.thalesgroup.CdmwNamingAndRepository.
                         RepositoryPackage.NoRootContext e) {
                    os.println("Cannot reach DefaultRootContext!");
                } catch (org.omg.CORBA.SystemException e) {
                    printException(e, os);
                } catch (org.omg.CORBA.UserException e) {
                    printException(e, os);
                }

                return result;
            }
        };
        operationsList.put("set_root_context", setRootContext);


        // ------- set_collector_context -------
        OperationBase setCollectorContext = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                // if undefined context name set defaut context
                if (arg.length == 0) {
                    // CDMW/SERVICES/TRACE/COLLECTORS/
                    collectorContext  = cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN;
                    collectorContext += "/";
                    collectorContext += cdmw.common.Locations.TRACE_COLLECTORS_NAME_DOMAIN;
                } else {
                    collectorContext = arg[0];
                }

                os.println("collector context set to : "
                           + collectorContext);
                return SUCCESS;
            }
        };
        operationsList.put("set_collector_context", setCollectorContext);


        // ------- activate_level -------
        OperationBase activateLevel = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
               return processLevel(true, arg, os);
            }
        };
        operationsList.put("activate_level", activateLevel);
        operationsList.put("al", activateLevel);


        // ------- deactivate_level -------
        OperationBase deactivateLevel = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
               return processLevel(false, arg, os);
            }
        };
        operationsList.put("deactivate_level", deactivateLevel);
        operationsList.put("dl", deactivateLevel);


        // ------- get_levels -------
        OperationBase getLevels = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                String titleComponent = "component name          "; // ECR-0123
                String titleName  = "domain name             ";
                String titleLevel = "level    ";
                String titleActiv = "activation";

                // get sequence of trace filter
                com.thalesgroup.CdmwTrace.Trace_Filter[] filters;
                try {
                    filters = traceProducer.get_trace_filters();
                } catch (org.omg.CORBA.SystemException e) {
                    printException(e, os);
                    return FAILURE;
                }

                os.println("list of trace filters");
                os.println("---------------------");
                os.println(titleComponent + titleName + titleLevel + titleActiv);

                for (int i = 0; i < filters.length; i++) {
                    // ECR-0123
                    StringBuffer component =
                        new StringBuffer(filters[i].the_component_name);
                    component.setLength(titleComponent.length());
                    for (int j = filters[i].the_component_name.length();
                         j < titleComponent.length();
                         j++)
                    {
                        component.setCharAt(j, ' ');
                    }
                    os.print(component.toString());

                    StringBuffer title = new StringBuffer(filters[i].the_domain);
                    title.setLength(titleName.length());
                    for (int j = filters[i].the_domain.length();
                         j < titleName.length();
                         j++)
                    {
                        title.setCharAt(j, ' ');
                    }
                    os.print(title.toString());

                    StringBuffer level = new StringBuffer();
                    if (filters[i].the_value == -1) {
                        level.append('*');
                    } else {
                        level.append(filters[i].the_value);
                    }
                    for (int j = level.length();
                         j < titleLevel.length();
                         j++)
                    {
                        level.append(' ');
                    }
                    os.print(level.toString());

                    if (filters[i].activation) {
                        os.println("yes");
                    } else {
                        os.println("no");
                    }
                }
                os.println();
                return SUCCESS;
            }
        };
        operationsList.put("gl", getLevels);


        // ------- register_collector -------
        OperationBase registerCollector = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                if (arg.length != 2) {
                    os.println("Error: syntax is "
                               + "'register_collector <name> <mnemonic>'");
                    return FAILURE;
                }

                int result = FAILURE;

                // find collector servant object reference from repository
                // and call its service
                try {
                    // get root naming context
                    org.omg.CosNaming.NamingContext ncRoot =
                        repository.resolve_root_context(rootContext);

                    // NamingInterface on Root context
                    cdmw.commonsvcs.naming.NamingInterface niRoot =
                        new cdmw.commonsvcs.naming.NamingInterface(ncRoot);

                    // set complete collector name
                    String collectorPath = collectorContext;
                    collectorPath += "/";
                    collectorPath += arg[0];

                    // get trace collector servant using Root context NamingInterface
                    org.omg.CORBA.Object obj = niRoot.resolve(collectorPath);
                    com.thalesgroup.CdmwTrace.Collector traceCollector =
                        com.thalesgroup.CdmwTrace.CollectorHelper.narrow(obj);

                    // register the collector servant in trace producer
                    traceProducer.register_collector(traceCollector, arg[0], arg[1]);

                    result = SUCCESS;

                } catch(com.thalesgroup.CdmwNamingAndRepository.
                        RepositoryPackage.NoRootContext e) {
                    os.println("Cannot reach DefaultRootContext!");
                } catch(org.omg.CosNaming.NamingContextPackage.NotFound e) {
                    os.println("CosNaming.NamingContext.NotFound exception!");
                } catch(org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                    os.println("Unexpected Error (CannotProceed exception)!");
                } catch(org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                    os.println("Invalid Name <" + arg[0] + ">!");
                } catch(org.omg.CORBA.SystemException e) {
                    printException(e, os);
                } catch(org.omg.CORBA.UserException e) {
                    printException(e, os);
                }

                return result;
            }
        };
        operationsList.put("register_collector", registerCollector);


        // ------- unregister_collector -------
        OperationBase unregisterCollector = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                if (arg.length != 1) {
                    os.println("Error: syntax is "
                               + "'unregister_collector <ident>'");
                    return FAILURE;
                }

                int result = FAILURE;
                try {
                    traceProducer.unregister_collector(Integer.parseInt(arg[0]));
                    result = SUCCESS;
                } catch(NumberFormatException e) {
                    os.println("collector ident is not valid");
                } catch(org.omg.CORBA.SystemException e) {
                    printException(e, os);
                } catch(org.omg.CORBA.UserException e) {
                    printException(e, os);
                }

                return result;
            }
        };
        operationsList.put("unregister_collector", unregisterCollector);


        // ------- get_registered_collectors -------
        OperationBase getRegisteredCollectors = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = FAILURE;
                try {
                    // get sequence of collectors from trace producer
                    com.thalesgroup.CdmwTrace.
                    TraceProducerPackage.Registration[] collectors =
                        traceProducer.get_registered_collectors();

                    String titleMnemoName  = "mnemonic name          ";
                    String titleObjectName = "object name            ";
                    String titleIdent      = "identifier";

                    os.println("list of registered collectors");
                    os.println("-----------------------------");
                    os.println(titleMnemoName + titleObjectName + titleIdent);

                    for (int i = 0; i < collectors.length; i++) {
                        StringBuffer mnemoName =
                            new StringBuffer(collectors[i].collectorMnemoName);
                        mnemoName.setLength(titleMnemoName.length());
                        for (int j = collectors[i].collectorMnemoName.length();
                             j < titleMnemoName.length();
                             j++)
                        {
                            mnemoName.setCharAt(j, ' ');
                        }
                        os.print(mnemoName.toString());

                        StringBuffer objectName =
                            new StringBuffer(collectors[i].collectorObjName);
                        objectName.setLength(titleObjectName.length());
                        for (int j = collectors[i].collectorObjName.length();
                             j < titleObjectName.length();
                             j++)
                        {
                            objectName.setCharAt(j, ' ');
                        }
                        os.print(objectName.toString());

                        os.println(collectors[i].collectorId);
                    }
                    os.println();
                    result = SUCCESS;

                } catch(org.omg.CORBA.SystemException e) {
                    printException(e, os);
                }

                return result;
            }
        };
        operationsList.put("get_collectors", getRegisteredCollectors);


        // ------- start_stop_targets -------
        OperationBase startStopTargets = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = SUCCESS;

                try {
                    // create 2 target threads
                    TargetThread target1 = new TargetThread();
                    TargetThread target2 = new TargetThread();

                    // start clients which stop automatically
                    target1.start();
                    target2.start();

                    // wait end of targets
                    target1.join();
                    target2.join();

                    result = SUCCESS;
                } catch (InterruptedException e) {
                    e.printStackTrace(new java.io.PrintWriter(os));
                } catch(org.omg.CORBA.SystemException e) {
                    printException(e, os);
                }
                return result;
            }
        };
        operationsList.put("start_stop_targets", startStopTargets);


        // ------- start_domain_target -------
        OperationBase startDomainTarget = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = SUCCESS;

                try {
                    // create target thread 1
                    TargetThread1 target = new TargetThread1();

                    // start client which stop automatically
                    target.start();

                    // wait end of targets
                    target.join();

                    result = SUCCESS;
                } catch(InterruptedException e) {
                    e.printStackTrace(new java.io.PrintWriter(os));
                } catch(org.omg.CORBA.SystemException e) {
                    printException(e, os);
                }
                return result;
            }
        };
        operationsList.put("start_domain_target", startDomainTarget);


        // ------- start_target -------
        OperationBase startTarget = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = SUCCESS;

                try {
                    // create target thread 2
                    theTarget = new TargetThread2();

                    // start client which stop automatically
                    theTarget.start();

                    result = SUCCESS;
                } catch(org.omg.CORBA.SystemException e) {
                    printException(e, os);
                }

                try {
                    cdmw.ossupport.OS.sleep(
                        cdmw.testutils.Testable.getTimescale() * 500);
                } catch (InterruptedException e) {
                    e.printStackTrace(new java.io.PrintWriter(os));
                }

                return result;
            }
        };
        operationsList.put("start_target", startTarget);


        // ------- stop_target -------
        OperationBase stopTarget = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                int result = SUCCESS;

                try {
                    if (theTarget != null) {
                        theTarget.stopThread();

                        // wait end of target
                        theTarget.join();

                        // delete target thread 2
                        theTarget = null;
                    }

                    result = SUCCESS;
                } catch (InterruptedException e) {
                    e.printStackTrace(new java.io.PrintWriter(os));
                } catch(org.omg.CORBA.SystemException e) {
                    printException(e, os);
                }
                return result;
            }
        };
        operationsList.put("stop_target", stopTarget);


        // ------- start_flushing -------
        OperationBase startFlushing = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                cdmw.trace.Tracer.activeFlushing();
                return SUCCESS;
            }
        };
        operationsList.put("start_flushing", startFlushing);


        // ------- stop_flushing -------
        OperationBase stopFlushing = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                cdmw.trace.Tracer.deactiveFlushing();
                return SUCCESS;
            }
        };
        operationsList.put("stop_flushing", stopFlushing);


        // ------- sleep_process -------
        OperationBase sleepProcess = new OperationBase() {
            public int execute(String[] arg, java.io.PrintStream os) {
                if (arg.length != 1) {
                    os.println("Error: syntax is 'sleep_process <time>'");
                    return FAILURE;
                }
                try {
                    cdmw.ossupport.OS.sleep(
                        cdmw.testutils.Testable.getTimescale() * Integer.parseInt(arg[0]));
                    return SUCCESS;
                } catch (InterruptedException e) {
                    e.printStackTrace(new java.io.PrintWriter(os));
                    return FAILURE;
                } catch (NumberFormatException e) {
                    os.println("bad value for time");
                    return FAILURE;
                }
            }
        };
        operationsList.put("sleep_process", sleepProcess);

    } // end of registerOperations()



    private int processLevel(boolean activation,
                             String[] arg,
                             java.io.PrintStream os)
    {
        String componentName;
        String domain;
        short  level;

        switch (arg.length) {
            case 0:
                componentName = com.thalesgroup.CdmwTrace.ALL_COMPONENT_NAMES.value; // ECR-0123
                domain = com.thalesgroup.CdmwTrace.ALL_DOMAINS.value;
                level  = com.thalesgroup.CdmwTrace.ALL_VALUES.value;
                break;

            case 1:
                componentName = com.thalesgroup.CdmwTrace.ALL_COMPONENT_NAMES.value; // ECR-0123
                domain = arg[0];
                level  = com.thalesgroup.CdmwTrace.ALL_VALUES.value;
                break;

            case 2:
                componentName = com.thalesgroup.CdmwTrace.ALL_COMPONENT_NAMES.value; // ECR-0123
                domain = arg[0];
                try {
                    level  = Short.parseShort(arg[1]);
                } catch (NumberFormatException e) {
                    os.println("bad value for domain level");
                    return OperationBase.FAILURE;
                }
                break;

            default:
                // ECR-0123
                componentName = arg[0];
                domain = arg[1];
                try {
                    level  = Short.parseShort(arg[2]);
                } catch (NumberFormatException e) {
                    os.println("bad value for domain level");
                    return OperationBase.FAILURE;
                }
                break;
        }

        try {
            if (activation) {
                traceProducer.activate_level(componentName, domain, level); // ECR-0123
            } else {
                traceProducer.deactivate_level(componentName, domain, level); // ECR-0123
            }
        } catch (org.omg.CORBA.SystemException e) {
            printException(e, os);
            return OperationBase.FAILURE;
        }
        return OperationBase.SUCCESS;
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
                        // should not append
                    }

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


    private void printInfo(java.io.PrintStream os) {
        os.println(  "\n\n"
                   + "\t\t---------------------------------------\n"
                   + "\t\t  Cdmw Trace Library Test             \n"
                   + "\t\t---------------------------------------\n"
                   + "\n\n"
                   + INFO_MESSAGE
                   + "\nInfo: (verbose mode is " + (verbose ? "on" : "off") + ")"
                  );
    }


    private void printException(cdmw.common.Exception ex,
                                java.io.PrintStream os)
    {
        os.println("CDMW Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(ostream));
    }

    private void printException(org.omg.CORBA.SystemException ex,
                                java.io.PrintStream os)
    {
        os.println("CORBA system Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(ostream));
    }

    private void printException(org.omg.CORBA.UserException ex,
                                java.io.PrintStream os)
    {
        os.println("CORBA user Exception : ");
        ex.printStackTrace(new java.io.PrintWriter(ostream));
    }



}



