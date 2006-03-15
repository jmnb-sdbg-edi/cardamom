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


package cdmw.xmlutils;

import cdmw.ossupport.OS;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ORBUtils;

import cdmw.xmlutils.XMLHelper;

import cdmw.platformmngt.PlatformInterface;

/**
 * This class initializes the ORB, registers the XML parser in the Name Service
 * and waits for client requests.
 * The XMLServer is a managed process or a standalone program.
 *
 */
public class XMLServer extends Thread {

    /**
     * Thread pool size of the Root POA
     */
    public static final int POA_THREAD_POOL_SIZE = 5;

    /**
     * Name of the file where the parser's IOR is written
     */
    public static final String IOR_FILE_NAME = "xml_server.ior";

    /**
     * Default port number for the XML Server
     */
    public static final String XML_SERVER_PORT = "5060";

    /**
     * Port Option used in ORBInit. 
     */
    public static final String PORT_OPTION = "--CdmwLocalisationService"; 
    
    /**
     * Current ORB reference.
     */
    public org.omg.CORBA.ORB orb = null;

    /**
     * Arguments of the command line.
     */
    public String[] args = null;

    /**
     * Constructor
     *
     * @param args Arguments passed to the ORB
     */
    public XMLServer(String[] args) {

        String portOption = OS.getOptionValue(args, PORT_OPTION);
        if (portOption.equals("yes") || portOption.equals("no")) {
            String[] serverArgs = new String[args.length + 1];
            for(int i=0; i<args.length; i++) {
                serverArgs[i] = args[i];
            }
            serverArgs[args.length] = PORT_OPTION + "=" + XML_SERVER_PORT;
            this.args = serverArgs;
        } else {
            this.args = args;
        }

    }

    /**
     * Prints a short message describing the usage of the XML Service.
     */
    private void printHelp() {
        System.out.println("usage : java -cp [relevant_classpath] "
            + "cdmw.xmlutils.XMLServer [options]");
        System.out.println("where options can be: ");
        System.out.println(XMLParserImpl.VALIDATING
            + "             to enable DTD validation");
        System.out.println(XMLParserImpl.DTD_DIRNAME_OPTION
            + "=[path]    to specify a directory containing DTD files");
        System.out.println(XMLParserImpl.HELP
            + "                 to display this message");
    }

    /**
     * Main method. Starts the XML Server.
     *
     * @param args Command line arguments.
     */
    public static void main(String[] args) {

        XMLServer server = new XMLServer(args);
        server.start();

    }

    /**
     * Inits the ORB and starts the XML Server as a managed process
     * or as a standalone program (it depends on the arguments
     * of the command line).
     */
    public void run() {

        String help = OS.getOptionValue(args, XMLParserImpl.HELP);
        if (!help.equals("no")) {
            printHelp();
        } else {

            try {
                // Init ORB
                StrategyList orbStrategies = new StrategyList();
                orbStrategies.addORBThreaded();
                orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
                orb = ORBSupport.ORBInit(args, orbStrategies);
                XMLHelper.registerValueFactories(orb);

                org.omg.PortableServer.POA rootPOA =
                    org.omg.PortableServer.POAHelper.narrow(
                        orb.resolve_initial_references("RootPOA"));
                rootPOA.the_POAManager().activate();

                // usage of the XML Parser as a managed process
                if (PlatformInterface.isLaunchedByPlatformManagement(args)) {
                    // creates the process behaviour
                    XMLParserBehaviour serverBehaviour
                        = new XMLParserBehaviour(orb, rootPOA, args);

                    // initialise the platform interface
                    PlatformInterface.setup(orb, args);
                    // acknowledge the creation of the process
                    PlatformInterface.acknowledgeCreation(serverBehaviour);

                    orb.run();
                    //orb.shutdown() is done by the ProcessBehaviour
                    orb.destroy();

                // standalone usage of the XML Parser
                } else {
                    // Create XML Parser object
                    com.thalesgroup.CdmwXML.XMLParser parser
                        = XMLParserImpl.init(orb, rootPOA, args);

                    // we write the IOR of the parser in a file
                    try {
                        String ior = orb.object_to_string(parser);
                        java.io.PrintWriter out = new java.io.PrintWriter(
                            new java.io.FileOutputStream(IOR_FILE_NAME));
                        out.println(ior);
               
                        out.close();
                    } catch(java.io.IOException ioe) {
                        ioe.printStackTrace();
                    }
                    // Bind the XML Parser to a Corbaloc name
                    ORBSupport.bindObjectToCorbaloc( orb,
                        XMLParserImpl.XML_PARSER_CORBALOC_ID, parser);
System.out.println ("XMLServer corbaloc bind OK");
                    orb.run();
                    //orb.shutdown() is done when we call the stop method
                    ORBSupport.removeCorbalocBinding(
                        orb, XMLParserImpl.XML_PARSER_CORBALOC_ID);
                    orb.destroy();
                }

            } catch(Exception e) {
                // cleanup the ORB to exit completely
                if (orb!=null) {
                    orb.shutdown(false);
                    orb.destroy();
                }
                e.printStackTrace();
            }

        }

    }

    /**
     * Stops the XML Server only if it's a standalone program.
     */
    public void stopStandaloneServer() {
        if (!PlatformInterface.isLaunchedByPlatformManagement(args)) {
            orb.shutdown(false);
        }
    }

}

