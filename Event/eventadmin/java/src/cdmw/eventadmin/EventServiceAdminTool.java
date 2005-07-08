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


package cdmw.eventadmin;

import cdmw.common.Assert;
import cdmw.common.Locations;
import cdmw.ossupport.OS;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.namingandrepository.NamingInterface;

import cdmw.event.EventChannel;
import cdmw.event.TypedEventChannel;

import com.thalesgroup.CdmwEvent.EventChannelFactory;
import com.thalesgroup.CdmwEvent.EventChannelFactoryHelper;
import com.thalesgroup.CdmwEvent.TypedEventChannelFactory;
import com.thalesgroup.CdmwEvent.TypedEventChannelFactoryHelper;

import com.thalesgroup.CdmwEventAdmin.EventChannelProfileManager;
import com.thalesgroup.CdmwEventAdmin.EventChannelProfileManagerHelper;
import com.thalesgroup.CdmwEventAdmin.EventChannelProfileManagerName;
import com.thalesgroup.CdmwEventAdmin.EventChannelProperty;

import org.omg.CosNaming.NamingContextPackage.NotFoundReason;

import java.io.PrintStream;
import java.util.StringTokenizer;

/**
 * This class is the interface used by the EventServiceAdmin class
 * to allow users to manage event channels and profiles.
 *
 */
public class EventServiceAdminTool {

    /**
     * Maximum number of try to connect to the EventChannelManager
     */
    public static final int MAX_RETRY = 100;

    /**
     * Number of properties
     */
    public static final int NB_PROPERTIES = 5;

    /**
     * Default help message
     */
    public static final String INFO_MESSAGE =
        "Type h or help for a list of available commands.";

    /**
     * Default prompt message
     */
    public static final String PROMPT_MESSAGE = "\nEVENT SERVICE ADMIN> ";

    /**
     * Default unknown command message
     */
    public static final String UNKNOWN_COMMAND_MESSAGE = "Bad command !";

    /**
     * Internal value to exit the EventServiceAdminTool
     */
    public static final int EXIT_ORDERED = -10;

    /**
     * Internal value returned in case of a successful command execution
     */
    public static final int OP_SUCCESS = 0;

    /**
     * Internal value returned in case of a failed command execution
     */
    public static final int OP_FAILURE = -1;

    /**
     * Maximum number of properties in a profile
     */
    public static final int MAX_PROPERTY = 50;

    /**
     * URL property tag
     */
    public static final String URL_propertyTag = "URL";

    /**
     * IDL property tag
     */
    public static final String IDL_propertyTag = "IDL";

    private static final String[] COMMANDS = {
        "cg",
        "connect",
        "create_channel",
        "create_profile",
        "create_typed_channel",
        "ct",
        "destroy",
        "display_channel_profile",
        "dp",
        "exit",
        "h",
        "help",
        "lc",
        "list_created_channel",
        "list_managed_channel",
        "list_managed_channel_profile",
        "lm",
        "lp",
        "remove_profile",
        "rm",
        "s",
        "save_profile",
        "stop_ec_manager",
        "u",
        "update_profile",
        "v",
        "verbose",
        "x" 
    };

    private static final int CG = 0;
    private static final int CONNECT = 1;
    private static final int CREATE_CHANNEL = 2;
    private static final int CREATE_PROFILE = 3;
    private static final int CREATE_TYPED_CHANNEL = 4;
    private static final int CT = 5;
    private static final int DESTROY = 6;
    private static final int DISPLAY_CHANNEL_PROFILE = 7;
    private static final int DP = 8;
    private static final int EXIT = 9;
    private static final int H = 10;
    private static final int HELP = 11;
    private static final int LC = 12;
    private static final int LIST_CREATED_CHANNEL = 13;
    private static final int LIST_MANAGED_CHANNEL = 14;
    private static final int LIST_MANAGED_CHANNEL_PROFILE = 15;
    private static final int LM = 16;
    private static final int LP = 17;
    private static final int REMOVE_PROFILE = 18;
    private static final int RM = 19;
    private static final int S = 20;
    private static final int SAVE_PROFILE = 21;
    private static final int STOP_EC_MANAGER = 22;
    private static final int U = 23;
    private static final int UPDATE_PROFILE = 24;
    private static final int V = 25;
    private static final int VERBOSE = 26;
    private static final int X = 27;
    private static final int UNKNOWN_COMMAND = 28;

    /**
     * orb reference
     */
    private org.omg.CORBA.ORB orb;

    /**
     * Repository object reference
     */
    private org.omg.CosNaming.NamingContext namingcontext;

    /**
     * command file name
     */
    private String commandFile;

    /**
     * verbose flag
     */
    private boolean verbose;

    /**
     * Root context
     */
    private org.omg.CosNaming.NamingContext defaultRootContext;
    private org.omg.CosNaming.NamingContext adminRootContext;

    /**
     * Servant context
     */
    private String factoryContext;
    private String eventchannelContext;
    private String typedeventchannelContext;

    /**
     * Servant object reference
     */
    private EventChannelFactory factory = null;
    private TypedEventChannelFactory typedFactory = null;
    private EventChannelProfileManager profileManagerRef = null;

    /**
     * Constructor
     *
     * @param orb A reference to the ORB pseudo-object
     * @param nc Naming Context where to find the channels and the factories
     * @param commandFile Name of the command file
     */
    public EventServiceAdminTool(
        org.omg.CORBA.ORB orb,
        org.omg.CosNaming.NamingContext nc,
        String commandFile) {

        this.orb = orb;
        this.namingcontext = nc;
        this.commandFile = commandFile;
        this.verbose = false;

        Assert.check(orb != null);
        Assert.check(nc != null);

        factoryContext =
            Locations.CDMW_EVENT_SERVICE_NAME_DOMAIN + "/FACTORIES";

        eventchannelContext =
            Locations.CDMW_EVENT_SERVICE_NAME_DOMAIN + "/EVENT_CHANNELS";

        typedeventchannelContext =
            Locations.CDMW_EVENT_SERVICE_NAME_DOMAIN + "/EVENT_CHANNELS";

    }

    /**
     * Initialize the EventserviceAdminTool object
     *
     * @exception InitException Unable to initialize EventserviceAdminTool
     */
    public void init() throws InitException {

        NamingInterface ni = new NamingInterface(namingcontext);
        org.omg.CORBA.Object obj = null;
        String name;

        System.out.println("cdmw_event_admin initialisation ...");

        try {
            int nbRetry = MAX_RETRY;

            // wait for cdmw_event_channel_manager to be launched and initialised
            while (nbRetry > 0) {
                try {
                    name = factoryContext + "/DefaultEventChannelFactory";

                    //Get a reference on the generic EventChannelFactory
                    obj = ni.resolve(name);
                    factory = EventChannelFactoryHelper.narrow(obj);

                    if (factory == null) {
                        throw new InitException("DefaultEventChannelFactory is a nil object reference.");
                    }

                    name = factoryContext + "/DefaultTypedEventChannelFactory";
                    //Get a reference on the generic EventChannelFactory
                    obj = ni.resolve(name);
                    typedFactory = TypedEventChannelFactoryHelper.narrow(obj);

                    if (typedFactory == null) {
                        throw new InitException("DefaultTypedEventChannelFactory is a nil object reference.");
                    }

                    //Get a reference on the EventChannelProfileManager
                    name =
                        Locations.CDMW_EVENT_SERVICE_NAME_DOMAIN
                            + "/"
                            + EventChannelProfileManagerName.value;
                    obj = ni.resolve(name);
                    profileManagerRef =
                        EventChannelProfileManagerHelper.narrow(obj);

                    if (profileManagerRef == null) {
                        throw new InitException("EventChannelProfileManager is a nil object reference.");
                    }

                    // No NotFound exception raised, get out from loop
                    break;
                } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
                    nbRetry--;
                    if (nbRetry == 0) {
                        try {
                            System.out.println("Unable to resolve: "
                                + NamingInterface.toString(nf.rest_of_name));
                        } catch (
                            cdmw.namingandrepository.InvalidNameException i) {
                            System.out.println(
                                "InvalidNameException: " + i.what());
                        }
                        String reason = "";
                        switch (nf.why.value()) {
                            case NotFoundReason._missing_node :
                                reason = "Missing node.";
                                break;
                            case NotFoundReason._not_context :
                                reason = "Not context.";
                                break;
                            case NotFoundReason._not_object :
                                reason = "Not object.";
                                break;
                            default :
                                reason = "Unknown reason.";
                        }
                        System.out.println("Reason: " + reason);
                    }
                    try {
                        OS.sleep(200);
                    } catch (InterruptedException ie) {}
                }
            } // end while loop

        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            throw new InitException("Can not proceed resolve.");
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            throw new InitException("Invalid name for factory or EventChannelProfileManager.");
        }

        System.out.println("cdmw_event_admin intialisation done");

    }

    /**
     * Return the internal number associated with the command name
     *
     * @param command Name of the command
     * @return The internal number corresponding to the command name
     */
    public int getOperation(String command) {

        for (int i = 0; i < COMMANDS.length; i++) {
            if (command.equals(COMMANDS[i])) {
                return i;
            }
        }
        return UNKNOWN_COMMAND;

    }

    /**
     * Execute the operation corresponding to the command
     *
     * @param op Name of the command
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    public int runOperation(String op, String arg, PrintStream out) {

        switch (getOperation(op)) {
            case CONNECT :
                return connectChannels(arg, out);

            case CG :
                return createEventChannel(arg, out);

            case CREATE_CHANNEL :
                return createEventChannel(arg, out);

            case CREATE_PROFILE :
                return createProfile(arg, out);

            case CREATE_TYPED_CHANNEL :
                return createTypedEventChannel(arg, out);

            case CT :
                return createTypedEventChannel(arg, out);

            case DESTROY :
                return destroyEventChannel(arg, out);

            case DISPLAY_CHANNEL_PROFILE :
                return displayChannelProfile(arg, out);

            case DP :
                return displayChannelProfile(arg, out);

            case EXIT :
                return exit(arg, out);

            case H :
                return printHelp(arg, out);

            case HELP :
                return printHelp(arg, out);

            case LC :
                return listCreatedChannel(arg, out);

            case LIST_CREATED_CHANNEL :
                return listCreatedChannel(arg, out);

            case LIST_MANAGED_CHANNEL :
                return listManagedChannel(arg, out);

            case LIST_MANAGED_CHANNEL_PROFILE :
                return listManagedChannelProfile(arg, out);

            case LM :
                return listManagedChannel(arg, out);

            case LP :
                return listManagedChannelProfile(arg, out);

            case REMOVE_PROFILE :
                return removeProfile(arg, out);

            case RM :
                return removeProfile(arg, out);

            case S :
                return saveProfile(arg, out);

            case SAVE_PROFILE :
                return saveProfile(arg, out);

            case STOP_EC_MANAGER :
                return stopEcManager(arg, out);

            case U :
                return updateProfile(arg, out);

            case UPDATE_PROFILE :
                return updateProfile(arg, out);

            case V :
                return toggleVerbose(arg, out);

            case VERBOSE :
                return toggleVerbose(arg, out);

            case X :
                return exit(arg, out);

            case UNKNOWN_COMMAND :
                unknownCommand(out);
                return OP_FAILURE;

            default :
                unknownCommand(out);
                return OP_FAILURE;

        }

    }

    /**
     * Parse the input and execute the relevant operation
     *
     * @param in Input data to be parsed
     * @param out Stream to print data
     * @param commandFile Presence of a command file in the options
     */
    public void run(
        java.io.BufferedReader in,
        PrintStream out,
        boolean commandFile) {
        printInfo(out);
        printPrompt(out);
        boolean loopFlag = true;
        String s;

        while (loopFlag) {
            try {
                while ((s = in.readLine()) != null) {
                    StringTokenizer st = new StringTokenizer(s);
                    String command = "";
                    String arg = "";
                    if (st.hasMoreTokens()) {
                        command = st.nextToken();
                    }
                    while (st.hasMoreTokens()) {
                        arg += st.nextToken();
                        if (st.hasMoreTokens())
                            arg += " ";
                    }

                    System.out.println(
                        "@cmd = <" + command + "> @args = <" + arg + ">");
                    if (runOperation(command, arg, out) == EXIT_ORDERED) {
                        loopFlag = false;
                        break;
                    }
                    printPrompt(out);

                }
            } catch (java.io.IOException ioe) {
                ioe.printStackTrace(out);
            }

            // if end of command file switch on System.in
            if (commandFile) {
                in = new java.io.BufferedReader(
                    new java.io.InputStreamReader(System.in));
            }

        }

    }

    /**
     * Display a summary of the available commands
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int printHelp(String arg, PrintStream out) {

        out.println(
              " +===================================+============================================+\n"
            + " | COMMAND                           | DESCRIPTION                                |\n"
            + " +===================================+============================================+\n"
            + " | x | exit                          | exit ClientThread mode                     |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | h | help                          | print available commands                   |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | v | verbose                       | toggle verbose mode on/off                 |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | cg | create_channel <channelName> | create a new generic event channel         |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | ct | create_typed_channel         | create a new typed event channel           |\n"
            + " |      <channelName>                |                                            |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | destroy <channelName>             | destroy an event channel                   |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | connect                           | connect channel_1 to channel_2.            |\n"
            + " |      <channelName1>               | channel_1 becomes the consumer of channel_2|\n"
            + " |      <channelName2>               | interface_ref is used when connecting two  |\n"
            + " |      [<interface_ref>]            | typed event channel                        |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | lm | list_managed_channel         | list managed channel                       |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | lp | list_managed_channel_profile | list managed channel and their properties  |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | lc | list_created_channel         | list already created channel               |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | dp | display_channel_profile      | display specified event channel profile    |\n"
            + " |      <channelName>                |                                            |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | create_profile                    | create a profile for a channel             |\n"
            + " |      <channelName>                |                                            |\n"
            + " |      <factory_url>                | factory_url is a mandatory property.       |\n"
            + " |      [<IDL>]                      | IDL is used only for typed event channel.  |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | rm | remove_profile               | remove a profile for a channel             |\n"
            + " |      <channelName>                |                                            |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | s | save_profile                  | save all channel profile in a xml file     |\n"
            + " |      [<xml_file_name>]            | if no file name is specified, profiles are |\n"
            + " |                                   | saved in the current loaded xml file       |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | add_property                      | add a property into a profile for a channel|\n"
            + " |      <channelName>                |                                            |\n"
            + " |      <factory_url>                |                                            |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | u | update_profile                | update a profile for a channel             |\n"
            + " |      <channelName>                |                                            |\n"
            + " |      <factory_url>                | factory_url is a mandatory property.       |\n"
            + " |      [<IDL>]                      | IDL is used only for typed event channel.  |\n"
            + " +-----------------------------------+--------------------------------------------+\n"
            + " | stop_ec_manager                   | stop cdmw_event_channel_manager           |\n"
            + " +-----------------------------------+--------------------------------------------+");
        return OP_SUCCESS;

    }

    /**
     * Exit the EventServiceAdminTool
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int exit(String arg, PrintStream out) {
        return EXIT_ORDERED;
    }

    /**
     * Verbose mode on/off
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int toggleVerbose(String arg, PrintStream out) {
        verbose = !verbose;
        out.println("(verbose mode is " + (verbose ? "on" : "off") + ")");
        return OP_SUCCESS;
    }

    /**
     * Create a generic event channel
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int createEventChannel(String arg, PrintStream out) {

        int result = OP_FAILURE;
        String channelName = arg;

        // if undefined context name set error
        if (channelName.equals("")) {
            out.println("channel name is missing");
        } else {

            try {
                factory.create_event_channel(channelName);
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                out.println("channel already exist!");
                return result;
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                out.println("channel name has an invalid format!");
                return result;
            } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
                out.println("Creation error: " + ce.reason);
                return result;
            } catch (org.omg.CORBA.SystemException se) {
                out.println("cdmw_event_admin: " + se.toString());
                return result;
            }

            out.println("generic event channel " + channelName + " created!");
            result = OP_SUCCESS;
        }

        return result;

    }

    /**
     * Create a typed event channel
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int createTypedEventChannel(String arg, PrintStream out) {

        int result = OP_FAILURE;
        String channelName = "";

        StringTokenizer st = new StringTokenizer(arg);
        if (st.hasMoreTokens()) {
            channelName = st.nextToken();
        }

        if (channelName.equals("")) {
            out.println("channel name is missing");
            return OP_FAILURE;
        }

        try {
            typedFactory.create_event_channel(channelName);
        } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
            out.println("channel already exist!");
            return result;
        } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
            out.println("channel name has an invalid format!");
            return result;
        } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
            out.println("Creation error: " + ce.reason);
            return result;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("cdmw_event_admin: " + se.toString());
            se.printStackTrace();
            return result;
        }

        out.println("typed event channel " + channelName + " created!");
        result = OP_SUCCESS;
        return result;

    }

    /**
     * Destroy an event channel
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int destroyEventChannel(String arg, PrintStream out) {

        int result = OP_FAILURE;
        String channelName = arg;

        // if undefined channel name set error
        if (channelName.equals("")) {
            out.println("channel name is missing");
        } else {
            String wholeChannelName = eventchannelContext + "/" + arg;

            try {
                // try with an un-typed event channel
                EventChannel eventchannel =
                    new EventChannel(wholeChannelName, namingcontext);
                eventchannel.destroy();
                out.println("event channel " + channelName + " destroyed!");
                result = OP_SUCCESS;
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                // Channel can be a typed one
                try {
                    TypedEventChannel typedeventchannel =
                        new TypedEventChannel(wholeChannelName, namingcontext);
                    typedeventchannel.destroy();
                    out.println(
                        "typed event channel " + channelName + " destroyed!");
                    result = OP_SUCCESS;
                } catch (org.omg.CORBA.BAD_PARAM bad_p) {
                    out.println("Invalid channel name: " + bad_p.getMessage());
                } catch (org.omg.CORBA.INTERNAL i) {
                    out.println("Internal Error Exception");
                } catch (org.omg.CORBA.SystemException se) {
                    out.println("cdmw_event_admin: " + se.toString());
                }
            } catch (org.omg.CORBA.INTERNAL i) {
                out.println("Internal Error Exception");
            } catch (org.omg.CORBA.SystemException se) {
                out.println("cdmw_event_admin: " + se.toString());
            }
        }

        return result;

    }

    /**
     * Connect an event channel to another
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int connectChannels(String arg, PrintStream out) {
        int result = OP_FAILURE;
        String channelName1 = "";
        String channelName2 = "";
        String interfaceRef = "";

        StringTokenizer st = new StringTokenizer(arg);
        if (st.hasMoreTokens()) {
            channelName1 = st.nextToken();
        }

        if (channelName1.equals("")) {
            out.println("channel name 1 is missing");
            return OP_FAILURE;
        }

        if (st.hasMoreTokens()) {
            channelName2 = st.nextToken();
        }

        if (channelName2.equals("")) {
            out.println("channel name 2 is missing");
            return OP_FAILURE;
        }

        if (st.hasMoreTokens()) {
            interfaceRef = st.nextToken();
        }

        if (!interfaceRef.equals("")) {
            result =
                connectTypedChannels(
                    channelName1,
                    channelName2,
                    interfaceRef,
                    out);
        } else {
            result = connectGenericChannels(channelName1, channelName2, out);
        }

        return result;
    }

    /**
     * List the managed channels of the EventProfileManager
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int listManagedChannel(String arg, PrintStream out) {

        String[] managedChannelList = null;

        try {
            managedChannelList = profileManagerRef.managed_event_channels();
        } catch (org.omg.CORBA.SystemException se) {
            out.println("cdmw_event_admin: " + se.toString());
            return OP_FAILURE;
        } catch (Exception e) {
            out.println("cdmw_event_admin: unexpected exception raised!");
            return OP_FAILURE;
        }

        out.println("list of managed event channel:");

        for (int i = 0; i < managedChannelList.length; i++) {
            out.println("\t" + managedChannelList[i]);
        }

        return OP_SUCCESS;

    }

    /**
     * List the managed channel profile of the EventProfileManager
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int listManagedChannelProfile(String arg, PrintStream out) {

        int result = OP_FAILURE;
        String[] managedChannelList = null;

        try {
            managedChannelList = profileManagerRef.managed_event_channels();
        } catch (org.omg.CORBA.SystemException se) {
            out.println("cdmw_event_admin: " + se.toString());
            return OP_FAILURE;
        } catch (Exception e) {
            out.println("cdmw_event_admin: unexpected exception raised!");
            return OP_FAILURE;
        }

        out.println("list of managed event channel:");

        for (int i = 0; i < managedChannelList.length; i++) {
            result = displayChannelProfile(managedChannelList[i], out);
            if (result == OP_FAILURE) {
                break;
            }
        }

        return result;

    }

    /**
     * List the created channels
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int listCreatedChannel(String arg, PrintStream out) {

        // Iterator reference
        org.omg.CosNaming.BindingIteratorHolder it =
            new org.omg.CosNaming.BindingIteratorHolder();
        // Binding list
        org.omg.CosNaming.BindingListHolder bl =
            new org.omg.CosNaming.BindingListHolder();
        String[] ecContext = { eventchannelContext, typedeventchannelContext };
        int nbContext = 2;
        NamingInterface ni = new NamingInterface(namingcontext);
        org.omg.CORBA.Object obj = null;

        // List event channels

        if (eventchannelContext.equals(typedeventchannelContext)) {
            nbContext = 1;
        }

        for (int i = 0; i < nbContext; i++) {
            // Get NamingInterface object
            try {
                obj = ni.resolve(ecContext[i]);
            } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
                nf.printStackTrace();
                return OP_FAILURE;
            } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
                cp.printStackTrace();
                return OP_FAILURE;
            } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {
                in.printStackTrace();
                return OP_FAILURE;
            }
            org.omg.CosNaming.NamingContext ecNContext =
                org.omg.CosNaming.NamingContextHelper.narrow(obj);

            if (ecNContext == null) {
                out.println(
                    "cdmw_event_admin: error when getting event channel naming context!");
                return OP_FAILURE;
            }

            NamingInterface ecNi = new NamingInterface(ecNContext);

            try {
                int nbCreatedChannel = 0;
                // Get first chunk
                ecNi.list(NamingInterface.LIST_CHUNK_SIZE, bl, it);

                do {
                    for (int j = 0; j < bl.value.length; j++) {
                        if (bl.value[j].binding_type.equals(
                            org.omg.CosNaming.BindingType.nobject)) {
                            nbCreatedChannel++;

                            if (nbCreatedChannel == 1) {
                                out.println("list of created event channel:");
                            }

                            out.println("\t" + bl.value[j].binding_name[0].id);

                            if (bl.value[j].binding_name[0].id.equals("")
                                || !bl.value[j].binding_name[0].kind.equals("")) {
                                out.println(
                                    "." + bl.value[j].binding_name[0].kind);
                            }
                            out.println();
                        }
                    }

                    if (it.value == null) {
                        break;
                    }
                } while (it.value.next_n(NamingInterface.LIST_CHUNK_SIZE, bl));

                if (nbCreatedChannel == 0) {
                    out.println("no channel created!");
                }
            } catch (org.omg.CORBA.SystemException se) {
                out.println("cdmw_event_admin: " + se.toString());
                return OP_FAILURE;
            }
        }

        return OP_SUCCESS;

    }

    /**
     * Display a channel profile
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int displayChannelProfile(String arg, PrintStream out) {

        String channelName = "";

        StringTokenizer st = new StringTokenizer(arg);
        if (st.hasMoreTokens()) {
            channelName = st.nextToken();
        }

        if (channelName.equals("")) {
            out.println("channel name is missing");
            return OP_FAILURE;
        }

        try {
            EventChannelProperty[] profile =
                profileManagerRef.find_profile(channelName);
            out.println(channelName + ": ");
            for (int k = 0; k < profile.length; k++) {
                out.println("\t" + profile[k].name + "=" + profile[k].value);
            }

            return OP_SUCCESS;
        } catch (com.thalesgroup.CdmwEventAdmin.NotExist ne) {
            out.println("no profile exist for this channel: " + channelName);
            return OP_FAILURE;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("cdmw_event_admin: " + se.toString());
            return OP_FAILURE;
        }

    }

    /**
     * Create a profile
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int createProfile(String arg, PrintStream out) {

        int result = OP_FAILURE;
        String channelName = "";
        EventChannelProperty[] profile;
        java.util.Vector profv = new java.util.Vector();

        StringTokenizer st = new StringTokenizer(arg);
        if (st.hasMoreTokens()) {
            channelName = st.nextToken();
        }

        if (channelName.equals("")) {
            out.println("channel name is missing");
            return OP_FAILURE;
        }

        // extract channel name and profile
        result = extractChannelAndProfile(arg, profv, out);
        java.util.Enumeration e = profv.elements();
        profile = new EventChannelProperty[profv.size()];
        int i = 0;
        for (; e.hasMoreElements();) {
            profile[i] = (EventChannelProperty) e.nextElement();
            i++;
        }

        if (result == OP_SUCCESS) {
            try {
                profileManagerRef.create_profile(channelName, profile);
                out.println("profile created for channel: " + channelName);
            } catch (com.thalesgroup.CdmwEventAdmin.AlreadyExistant ae) {
                out.println("channel already exist!");
                result = OP_FAILURE;
            } catch (com.thalesgroup.CdmwEventAdmin.InvalidName in) {
                out.println("channel name has an invalid format!");
                result = OP_FAILURE;
            } catch (org.omg.CORBA.SystemException se) {
                out.println("cdmw_event_admin: " + se.toString());
                result = OP_FAILURE;
            }
        }

        return result;

    }

    /**
     * Delete a profile
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int removeProfile(String arg, PrintStream out) {

        int result = OP_FAILURE;
        String channelName = "";

        StringTokenizer st = new StringTokenizer(arg);
        if (st.hasMoreTokens()) {
            channelName = st.nextToken();
        }

        if (channelName.equals("")) {
            out.println("channel name is missing");
            return OP_FAILURE;
        }

        try {
            profileManagerRef.remove_profile(channelName);
            out.println(
                "profile removed from configuration for channel: "
                    + channelName);
            result = OP_SUCCESS;
        } catch (com.thalesgroup.CdmwEventAdmin.NotExist ne) {
            out.println("no profile exist for this channel!");
            result = OP_FAILURE;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("cdmw_event_admin: " + se.toString());
            result = OP_FAILURE;
        }

        return result;

    }

    /**
     * Save a profile
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int saveProfile(String arg, PrintStream out) {

        int result = OP_FAILURE;

        out.println("operation not implemented!");

        result = OP_SUCCESS;
        return result;

    }

    /**
     * Update a profile
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int updateProfile(String arg, PrintStream out) {

        int result = OP_FAILURE;

        String channelName = "";
        EventChannelProperty[] profile;
        java.util.Vector profv = new java.util.Vector();

        StringTokenizer st = new StringTokenizer(arg);
        if (st.hasMoreTokens()) {
            channelName = st.nextToken();
        }

        if (channelName.equals("")) {
            out.println("channel name is missing");
            return OP_FAILURE;
        }

        // extract channel name and profile
        result = extractChannelAndProfile(arg, profv, out);
        java.util.Enumeration e = profv.elements();
        profile = new EventChannelProperty[profv.size()];
        int i = 0;
        for (; e.hasMoreElements();) {
            profile[i] = (EventChannelProperty) e.nextElement();
            i++;
        }

        if (result == OP_SUCCESS) {
            try {
                profileManagerRef.update_profile(channelName, profile);
            } catch (com.thalesgroup.CdmwEventAdmin.NotExist ne) {
                out.println("channel doesn't exist! Create it!");
                result = OP_FAILURE;
            } catch (org.omg.CORBA.SystemException se) {
                out.println("cdmw_event_admin: " + se.toString());
                result = OP_FAILURE;
            }
        }

        result = OP_SUCCESS;
        return result;

    }

    /**
     * Stop the EventChannelManager
     *
     * @param arg Arguments of the command
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    protected int stopEcManager(String arg, PrintStream out) {

        int result = OP_FAILURE;

        try {
            profileManagerRef.destroy();
            out.println("cdmw_event_channel_manager stopped!");
            result = OP_SUCCESS;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("cdmw_event_admin: " + se.toString());
            result = OP_FAILURE;
        }

        return result;

    }

    /**
     * Display a message in case of an unknown command
     *
     * @param out Stream to print data
     */
    private void unknownCommand(PrintStream out) {
        out.print(UNKNOWN_COMMAND_MESSAGE);
    }

    /**
     * Print a prompt
     *
     * @param out Stream to print data
     */
    private void printPrompt(PrintStream out) {
        out.print(PROMPT_MESSAGE);
    }

    /**
     * Print an introduction message
     *
     * @param out Stream to print data
     */
    private void printInfo(PrintStream out) {
        out.println(
            "\n\n"
            + "\t\t--------------------------------------------------\n"
            + "\t\t Cdmw Event service administration console       \n"
            + "\t\t--------------------------------------------------\n"
            + "\n\n"
            + INFO_MESSAGE
            + "\nInfo: (verbose mode is "
            + (verbose ? "on" : "off")
            + ")");
    }

    /**
     * Connect a generic channel to another
     *
     * @param channelName1 First channel name
     * @param channelName2 Second channel name
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    private int connectGenericChannels(
        String channelName1,
        String channelName2,
        PrintStream out) {

        int result = OP_FAILURE;
        EventChannel eventchannel2 = null;

        String completeChannelName1 = eventchannelContext + "/" + channelName1;
        String completeChannelName2 = eventchannelContext + "/" + channelName2;

        try {
            eventchannel2 =
                new EventChannel(completeChannelName2, namingcontext);
        } catch (org.omg.CORBA.SystemException se) {
            se.printStackTrace();
            return OP_FAILURE;
        }

        try {
            eventchannel2.connectToChannel(completeChannelName1);
        } catch (org.omg.CosEventChannelAdmin.AlreadyConnected ac) {
            out.println("cdmw_event_admin: " + ac.getMessage());
            return OP_FAILURE;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("cdmw_event_admin: " + se.toString());
            return OP_FAILURE;
        }

        out.println(
            "generic event channel "
                + channelName1
                + " connected to "
                + channelName2);
        result = OP_SUCCESS;

        return result;

    }

    /**
     * Connect a typed channel to another
     *
     * @param channelName1 First channel name
     * @param channelName2 Second channel name
     * @param out Stream to print data
     * @return Status of the execution of the command
     */
    private int connectTypedChannels(
        String channelName1,
        String channelName2,
        String interfaceRef,
        PrintStream out) {

        int result = OP_FAILURE;
        TypedEventChannel eventchannel2 = null;

        String completeChannelName1 =
            typedeventchannelContext + "/" + channelName1;
        String completeChannelName2 =
            typedeventchannelContext + "/" + channelName2;

        try {
            eventchannel2 =
                new TypedEventChannel(completeChannelName2, namingcontext);
        } catch (org.omg.CORBA.SystemException se) {
            se.printStackTrace();
            return OP_FAILURE;
        }

        try {
            eventchannel2.connectToChannel(interfaceRef, completeChannelName1);
        } catch (org.omg.CosTypedEventChannelAdmin.InterfaceNotSupported ins) {
            out.println("cdmw_event_admin: " + ins.getMessage());
            return OP_FAILURE;
        } catch (org.omg.CosTypedEventChannelAdmin.NoSuchImplementation nsi) {
            out.println("cdmw_event_admin: " + nsi.getMessage());
            return OP_FAILURE;
        } catch (org.omg.CosEventChannelAdmin.AlreadyConnected ac) {
            out.println("cdmw_event_admin: " + ac.getMessage());
            return OP_FAILURE;
        } catch (org.omg.CosEventChannelAdmin.TypeError te) {
            out.println("cdmw_event_admin: " + te.getMessage());
            return OP_FAILURE;
        } catch (org.omg.CORBA.SystemException se) {
            out.println("cdmw_event_admin: " + se.toString());
            return OP_FAILURE;
        }

        out.println(
            "typed event channel "
            + channelName1
            + " connected to "
            + channelName2);
        result = OP_SUCCESS;

        return result;

    }

    /**
     * Parse the command line and create the corresponding profile
     *
     * @param arg Arguments of the command
     * @param profile Set of properties to be created
     * @param out Stream to print data
     */
    private int extractChannelAndProfile(
        String arg,
        java.util.Vector profile,
        PrintStream out) {

        String[] propertyTag = new String[MAX_PROPERTY];
        String[] property = new String[MAX_PROPERTY];
        // number of property defined in profile
        String url = "";
        int nbProperty = 0;

        StringTokenizer st = new StringTokenizer(arg);
        // we do not take into account the name of the channel
        st.nextToken();

        while ((nbProperty < MAX_PROPERTY) && st.hasMoreTokens()) {

            StringTokenizer stProperty =
                new StringTokenizer(st.nextToken(), "=");

            // extract URL tag
            propertyTag[nbProperty] = stProperty.nextToken();

            if (!propertyTag[nbProperty].equals(URL_propertyTag)
                && !propertyTag[nbProperty].equals(IDL_propertyTag)) {
                out.println("invalid profile description!");
                return OP_FAILURE;
            }

            // extract property
            if (stProperty.hasMoreTokens()) {
                property[nbProperty] = stProperty.nextToken();
                if (propertyTag[nbProperty].equals(URL_propertyTag)) {
                    url = property[nbProperty];
                }
            } else {
                out.println("invalid profile description!");
                return OP_FAILURE;
            }

            nbProperty++;

        }

        if (url.equals("")) {
            out.println("URL is missing!");
            return OP_FAILURE;
        }

        for (int i = 0; i < nbProperty; i++) {
            if (propertyTag[i] != null) {
                profile.add(
                    new EventChannelProperty(propertyTag[i], property[i]));
            }
        }

        return OP_SUCCESS;

    }

}


