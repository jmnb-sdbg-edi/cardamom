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


package cdmw.xmlutils.test;

import cdmw.testutils.Testable;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;

import cdmw.xmlutils.XMLHelper;

import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.XMLParserHelper;
import com.thalesgroup.CdmwXML.DocumentDescription;

public class MainTest {

    /**
     * Thread pool size of the ORB
     */
    public static final int POA_THREAD_POOL_SIZE = 5;

    /**
     * Command line option to have a choice between different tests
     */
    public static final String MENU_OPTION = "--menu";

    /**
     * Name of the file containing the IOR of the XML Parser
     */
    public static final String IOR_FILE_NAME = "xml_server.ior";

    /**
     * Relative path to the directory that contains the XML files to parse
     */
    public static final String XML_FILES_DIRECTORY
        = "./";


    /**
     * Returns the XML Parser object.
     *
     * @param orb Current ORB reference.
     * @param args Arguments of the command line.
     * @return The XML Parser object.
     */
    public static XMLParser getXMLParser(
        org.omg.CORBA.ORB orb, String[] args) {

        // get a reference on the XMLParser by reading the IOR file
        org.omg.CORBA.Object obj = null;
        try {
            java.io.BufferedReader in = new java.io.BufferedReader(
                new java.io.FileReader(IOR_FILE_NAME));
            obj = orb.string_to_object(in.readLine());
        } catch(java.io.IOException ioe) {
            ioe.printStackTrace();
        }
        return XMLParserHelper.narrow(obj);

    }

    /**
     * Runs the tests.
     *
     * @param orb Current ORB reference.
     * @param args Arguments of the command line.
     */
    public static void run(org.omg.CORBA.ORB orb, String[] args) {

        boolean unitTests = true;
        String menu = OS.getOptionValue(args, MENU_OPTION);

        if ( !menu.equals("yes") ) {

            test2(getXMLParser(orb, args), unitTests);

        } else {

            unitTests = false;
            System.out.println("Which kind of test do you want to run ?\n");
            System.out.println("\t1. A basic test: a simple XML file to parse.");
            System.out.println("\t2. A classical test: a simple client using all the available functions.");
            System.out.println("\t3. A more sophisticated test: several clients parsing a file.");
            System.out.println("\t4. An exhaustive test: several clients parsing and doing XPath queries at the same time.\n");

            int choice = 0;
            boolean ok = false;
            while(!ok) {
                try {
                    System.out.println("Your choice ? ");
                    java.io.BufferedReader in = new java.io.BufferedReader(
                        new java.io.InputStreamReader(System.in));
                    choice = Integer.valueOf(in.readLine()).intValue();
                    if (choice>0 && choice<5) ok = true;
                } catch(java.io.IOException ioe) {
                } catch(NumberFormatException nfe) {}
            }

            XMLParser parser = getXMLParser(orb, args);
            if (parser != null) {
                switch(choice) {
                    case 1:
                        test1(parser);
                        break;
                    case 2:
                        test2(parser, unitTests);
                        break;
                    case 3:
                        test3(parser);
                        break;
                    case 4:
                        test4(parser);
                        break;
                }
            }
        }

    }

    /**
     * This test just parses a simple XML file.
     *
     * @param parser The XML parser to use.
     */
    public static void test1(XMLParser parser) {

        try {
            // Parse a file
            String systemId = XML_FILES_DIRECTORY + "CFFactories.xml";
            System.out.println("Parsing a file (" + systemId + ")...");
            long start = (new java.util.Date()).getTime();
            DocumentDescription docDesc = parser.parse(systemId);
            long duration = (new java.util.Date()).getTime() - start;
            System.out.println("Parsing time(in ms): " + duration);

        } catch(com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException pe) {
            pe.printStackTrace();
        }

    }

    /**
     * This test starts a client which tests all the available functions
     * of the DOM common interface.
     *
     * @param parser The XML parser to use.
     */
    public static void test2(XMLParser parser, boolean unitTests) {

        Client1 client = new Client1(parser, unitTests);
        client.start();

    }

    /**
     * This test starts a simple type of client which only parses an XML file.
     *
     * @param orb Current ORB reference.
     * @param parser The XML parser to use.
     */
    public static void test3(XMLParser parser) {

        int CLIENT_NUMBER = 100;
        System.out.println(
            "Number of clients : " + CLIENT_NUMBER);
        Client4[] clients4 = new Client4[CLIENT_NUMBER];
        System.out.println("Creating clients...");
        for(int i=0; i<CLIENT_NUMBER; i++) {
            clients4[i]
                = new Client4(parser, XML_FILES_DIRECTORY + "othello.xml");
        }

        System.out.println("Starting clients...");
        for(int j=0; j<CLIENT_NUMBER; j++) {
            clients4[j].start();
        }

        try {
            for(int k=0; k<CLIENT_NUMBER; k++) {
                clients4[k].join();
            }
        } catch(InterruptedException ie) {}

        long average4 = 0;
        for(int l=0; l<CLIENT_NUMBER; l++) {
            average4 += clients4[l].getDuration();
        }

        average4 = average4 / CLIENT_NUMBER;
        System.out.println("Average parsing time type 4: " + average4);

    }

    /**
     * This test starts clients of 2 different types. Each type parses
     * an XML file and does some XPath queries.
     *
     * @param orb Current ORB reference.
     * @param parser The XML parser to use.
     */
    public static void test4(XMLParser parser) {

        int CLIENT_TYPE_NUMBER = 10;
        System.out.println(
            "Number of clients of each type: " + CLIENT_TYPE_NUMBER);
        Client2[] clients2 = new Client2[CLIENT_TYPE_NUMBER];
        Client3[] clients3 = new Client3[CLIENT_TYPE_NUMBER];
        System.out.println("Creating clients...");
        for(int i=0; i<CLIENT_TYPE_NUMBER; i++) {
            clients2[i] = new Client2(parser);
            clients3[i] = new Client3(parser);
        }

        System.out.println("Starting clients...");
        for(int j=0; j<CLIENT_TYPE_NUMBER; j++) {
            clients2[j].start();
            clients3[j].start();
        }

        try {
            for(int k=0; k<CLIENT_TYPE_NUMBER; k++) {
                clients2[k].join();
                clients3[k].join();
            }
        } catch(InterruptedException ie) {}

        long average2 = 0;
        long average3 = 0;
        long queryAvg = 0;
        for(int l=0; l<CLIENT_TYPE_NUMBER; l++) {
            average2 += clients2[l].getDuration();
            queryAvg += clients2[l].getQueryDuration();
            average3 += clients3[l].getDuration();
        }

        average2 = average2 / CLIENT_TYPE_NUMBER;
        queryAvg = queryAvg / CLIENT_TYPE_NUMBER;
        average3 = average3 / CLIENT_TYPE_NUMBER;
        System.out.println("Average parsing time type 2: " + average2);
        System.out.println("Average XPath query time : " + queryAvg);
        System.out.println("Average parsing time type 3: " + average3);

    }

    /**
     * Main function. Initializes the ORB, starts the XML Server
     * and runs the tests.
     *
     * @param args Arguments of the command line.
     */
    public static void main(String[] args) {

        org.omg.CORBA.ORB orb = null;
        try {
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
            orb = ORBSupport.ORBInit(args, orbStrategies);
            XMLHelper.registerValueFactories(orb);

            System.out.println("Starting XMLService...");
            ThreadedProcess server = OS.createJavaProcess(
                "cdmw.xmlutils.XMLServer --validate --doctypedir=" + XML_FILES_DIRECTORY, 
                Testable.getProperties());
            OS.sleep(Testable.getTimescale() * 10000);
            System.out.println("XMLService started");

            run(orb, args);
            orb.destroy();

            // And we stop the XML Server
            OS.killProcess(server);

        } catch(Exception e) {
            e.printStackTrace();
            if (orb != null) {
                orb.shutdown(false);
                orb.destroy();
            }
        }

    }


}

