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


package cdmw.orbsupport.test;

import java.util.Vector;
import java.util.Enumeration;

import java.io.File;
import java.io.IOException;

import cdmw.ossupport.OS;
import cdmw.orbsupport.ORBUtils;
import cdmw.orbsupport.ORBSupport;
import cdmw.ossupport.ThreadedProcess;
import cdmw.orbsupport.StrategyList;
import cdmw.testutils.Testable;
import cdmw.common.BadParameterException;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.POAHelper;
import org.omg.PortableServer.POAManagerPackage.AdapterInactive;
import org.omg.PortableServer.POAManager;
import org.omg.CORBA.ORBPackage.InvalidName;

public class ORBSupportTest extends Testable {
	
ThreadedProcess namingp;
ThreadedProcess propertyp= null;
    public ORBSupportTest() {
        this.name = "ORBSupportTest";
        this.out = System.out;
    }

    public String readFromFile(String file) {

        try {
            java.io.BufferedReader in = new java.io.BufferedReader(
                new java.io.FileReader(file) );
            return in.readLine();
        }
        catch(IOException ioe) {
            return null;
        }
    }

    public void doPortNumberTest(org.omg.CORBA.ORB orb) {
        try {
            String sForceDiffProcPort = System.getProperty("force.diff.process.port");
            boolean forceDiff = new Boolean(sForceDiffProcPort).booleanValue();

            String serverMTCorbaloc = "corbaloc::localhost:" +
                                      (ServerProcess.SERVER_MT_PORT + (forceDiff ? 1 : 0)) +
                                      "/" + ServerProcess.SERVER_MT_CORBALOC_NAME;
            println("Try to contact multi-threaded server via corbaloc: " + serverMTCorbaloc);
            org.omg.CORBA.Object objMT = orb.string_to_object( serverMTCorbaloc );
            ORBSupportServer serverMT = ORBSupportServerHelper.narrow( objMT );
            check( serverMT!=null );

            serverMT.computeSquare((short)5);
            succeed();
        } catch (Exception e) {
            fail();
            println("Exception: " + e.toString()); 
        }

        try {        
            String serverSTCorbaloc = "corbaloc::localhost:" + ServerProcess.SERVER_ST_PORT + 
                                      "/" + ServerProcess.SERVER_ST_CORBALOC_NAME;
            println("Try to contact single-threaded server via corbaloc: " + serverSTCorbaloc);
     	    org.omg.CORBA.Object objST = orb.string_to_object( serverSTCorbaloc );
            ORBSupportServer serverST = ORBSupportServerHelper.narrow( objST );
            check( serverST!=null );

            serverST.computeSquare((short)5);
            succeed();
        } catch (Exception e) {
            fail();
            println("Exception: " + e.toString()); 
        }        
    }
    
    
    
    public void doPOACreationTests(ORBSupportServer server)
        throws InterruptedException {

        println("Starting a thread with requesting a slow service");
        ClientThread thread_1 = new ClientThread(server,"slow");
        thread_1.start();

        println("Starting a thread with requesting a fast service");
        ClientThread thread_2 = new ClientThread(server,"");
        thread_2.start();

        println("Sleeping a while to wait the second thread");
        OS.sleep(getTimescale() * 2000);

        println("Checking if the second thread is blocked");
        println("  because the signle thread server process");
        println("  process the slow request of the first one");
        check( thread_2.getResult() == 0 );

        println("Waiting for the completion of the first thread");
        thread_1.join();

        println("Sleeping a while to wait the second thread");
        OS.sleep(getTimescale() * 1000);
        println("Checking the second thread cna have its request processed");
        check( thread_2.getResult() == 25 );
        thread_2.join();

    }

    public void doORBThreadPoolLimitTests(ORBSupportServer server)
        throws InterruptedException {

        int nbThreads = 5;

        println("Creating five threads with a reference to the server");
        Vector threads = new Vector();

        for (int iThread=0; iThread < nbThreads; ++iThread ) {
            threads.add( new ClientThread(server,"slow") );
        }

        println("Starting the threads");
        for (Enumeration e = threads.elements() ; e.hasMoreElements() ;) {
            ((ClientThread) e.nextElement()).start();
        }

        println("Sleeping a while to wait the threads");
        OS.sleep(getTimescale() * 2000);

        println("Starting a new thread");
        ClientThread testClientThread = new ClientThread( server,"" );
        testClientThread.start();

        println("Sleeping a while to wait the last thread");
        OS.sleep(getTimescale() * 1000);
        println("Checking the last thread is blocked by the");
        println("  limit of the server thread pool");
        check( testClientThread.getResult() == 0 );

        println("Waiting the threads completion");
        for (Enumeration e = threads.elements() ; e.hasMoreElements() ;) {
            ((ClientThread) e.nextElement()).join();
        }

        println("Sleeping a while to wait the last thread");
        OS.sleep(getTimescale() * 1000);

        println("Checking the last thread is unblocked");
        check( testClientThread.getResult() == 25 );
        testClientThread.join();

    }


    public void doORBMultithreadTests(ORBSupportServer server)
        throws InterruptedException {

        int nbThreads = 4;

        println("Creating four threads with a reference to the server");
        Vector threads = new Vector();

        for (int iThread=0; iThread < nbThreads; ++iThread ) {
            threads.add( new ClientThread(server, "slow") );
        }

        println("Starting the threads");
        for (Enumeration e = threads.elements() ; e.hasMoreElements() ;) {
            ((ClientThread) e.nextElement()).start();
        }

        println("Sleeping a while to wait the threads");
        OS.sleep(getTimescale() * 3000);

        println("Querying the server how much threads are in service ?");
        int nb = server.getNumberOfThreads();

        check( nb == nbThreads );

        println("Waiting for the threads completion");
        for (Enumeration e = threads.elements() ; e.hasMoreElements() ;) {
            ((ClientThread) e.nextElement()).join();
        }

    }

    public void doORBTests() throws InterruptedException,AdapterInactive,InvalidName {

        String iorMTFile = "SupportServerMT.ior";
        String iorSTFile = "SupportServerST.ior";
        StrategyList strategies = new StrategyList();
        strategies.addORBThreaded();
        try {
            strategies.addPOAThreadPerRequest();
        } catch(BadParameterException bpe) {
            throw new InterruptedException("Unable to initialize "
                + "the strategy list");
        }
		// ORB init
	 	org.omg.CORBA.ORB orb = ORBSupport.ORBInit(new String[0], strategies);
		
        succeed("<doORBTest> ORB init successful.");

       	println("Starting a CORBA server in another process");

        String classname = "cdmw.orbsupport.test.MainTest --server";
        ThreadedProcess serverp = null;
       	try {
            //serverp = OS.createJavaProcess(classname, getProperties());
            Vector props = getProperties();
            String myprop = "force.diff.process.port";
            props.add(myprop + "=" +
                new Boolean(
                    System.getProperty("force.diff.process.port")).booleanValue());
            serverp = OS.createJavaProcess(classname, props);
        } catch(IOException ioe) {
            throw new InterruptedException("Unable to create server process.");
        }

        println("Sleeping a while to wait the process creation");
        OS.sleep(getTimescale() * 5000);

        println("Getting back the IOR from the file for the multi-thread POA");
        String iorMT = readFromFile(iorMTFile);

        println("Getting back the IOR from the file for the single thread POA");
        String iorST = readFromFile(iorSTFile);

      	if (iorMT==null || iorST==null ) {
            orb.destroy();
            throw new InterruptedException("Unable to retrieve IOR.");
        	} 

        println("Creating the server stub multi-thread POA");
        org.omg.CORBA.Object objMT = orb.string_to_object( iorMT );
        ORBSupportServer serverMT = ORBSupportServerHelper.narrow( objMT );
        check( serverMT!=null );

        println( "Creating the server stub single-thread POA" );
        org.omg.CORBA.Object objST = orb.string_to_object( iorST );
        ORBSupportServer serverST = ORBSupportServerHelper.narrow( objST );
        check( serverST!=null );

       	println("Invoking a method on the server Multi-thread");
        int resultMT = serverMT.computeSquare((short)5);
        check( resultMT == 25 );

		println("Invoking a method on the server Single-thread");
		int resultST = serverST.computeSquare((short)5);
		check( resultST == 25 );

        // Check if the server accept concurrent requests
        doORBMultithreadTests(serverMT);

        // Check if the server use a thread pool limiting
        //    the number of request to be handled
        doORBThreadPoolLimitTests(serverMT);

       // Check if the server is truely single-threaded
        doPOACreationTests(serverST);
        // Check if servers are listening to the appropriate ports
        doPortNumberTest(orb);
        
		OS.sleep(getTimescale() * 3000);
	
        println("Killing the server");
        OS.killProcess( serverp );
		println("Removing IOR files");
        OS.unlink(iorSTFile);
        OS.unlink(iorMTFile);
        orb.destroy();

    }
    
    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(36);
	
        println("Creating a strategy list");
        StrategyList strategyList = new StrategyList();
        StrategyList strategyList_2 = new StrategyList();
        StrategyList strategyList_3 = new StrategyList();
        succeed("Strategy List successfully created");

        println("Checking the strategy list is empty");
        check( !strategyList.isORBThreaded() );
        check( !strategyList.isPOAThreadPool() );
        check( !strategyList.isPOAThreadPerRequest() );
        check( !strategyList.isPOAThreadPerConnection() );

        println("Setting the Orb ThreadPerConnection strategy in the list");
        strategyList.addORBThreaded();
        println("Checking the Orb ThreadPerConnection strategy in the list");
        check( strategyList.isORBThreaded() );

        println("Setting the Orb ThreadPerRequest strategy in the list");
        strategyList.addORBThreaded();
        println("Checking the Orb ThreadPerRequest strategy in the list");
        check( strategyList.isORBThreaded() );

        println("Setting the Poa Thread pool strategy in the list");
        try {
            strategyList.addPOAThreadPool( 3 );
        } catch (BadParameterException bpe) {}
        println( "Checking the Poa Thread pool strategy in the list" );
        check( strategyList.isPOAThreadPool() );
        check( strategyList.getNbPOAThreads() == 3 );


        println("Setting the POA ThreadPerRequest strategy incompatible with the thread pool");
        try {
            strategyList.addPOAThreadPerRequest();
            fail("Error: POAThreadPool should be already set");
        } catch (BadParameterException bpe) {
            succeed("Success: POAThreadPool already set");
        }

        println("Setting the POA Threaded strategy in the list (ThreadPerRequest)");
        try {
            strategyList_2.addPOAThreadPerRequest();
        } catch (BadParameterException bpe) {}
        println("Checking the POA Threaded strategy in the list");
        check( strategyList_2.isPOAThreadPerRequest() );

        println("Setting the POA Thread Pool strategy incompatible with the threaded");
        try {
            strategyList_2.addPOAThreadPerRequest();
            fail("Error: POAThreaded should be already set");
        } catch (BadParameterException bpe) {
            succeed("Success: POAThreaded already set");
        }

        println("Setting the POA Threaded strategy in the list (ThreadPerConnection)");
        try {
            strategyList_3.addPOAThreadPerConnection();
        } catch (BadParameterException bpe) {}
        println("Checking the POA Threaded strategy in the list");
        check( strategyList_3.isPOAThreadPerConnection() );

        println("Setting the POA Thread Pool strategy incompatible with the threaded");
        try {
            strategyList_3.addPOAThreadPerConnection();
            fail("Error: POAThreaded should be already set");
        } catch (BadParameterException bpe) {
            succeed("Success: POAThreaded already set");
        }

        println( "Trying to construct a new strategy from this one" );
        StrategyList strategyListCopied = strategyList.copy();
        StrategyList strategyListCopied_2 = strategyList_2.copy();
        println( "Checking the value" );
        check( strategyListCopied.isORBThreaded() );
        check( strategyListCopied.isPOAThreadPool() );
        check( strategyListCopied.getNbPOAThreads() == 3 );

        check( strategyListCopied_2.isPOAThreadPerRequest() );

        println("Trying to assign a new strategy from this one");
        StrategyList strategyListAssigned = strategyList;
        StrategyList strategyListAssigned_2 = strategyList_2;

        println("Checking the value");
        check( strategyListAssigned.isORBThreaded() );
        check( strategyListAssigned.isPOAThreadPool() );
        check( strategyListAssigned.getNbPOAThreads() == 3 );

        check( strategyListAssigned_2.isPOAThreadPerRequest() );
	
	 	try {
            doORBTests();
        } catch(InterruptedException ie) {
            ie.printStackTrace();
        }catch(AdapterInactive ai) {
            System.out.println(ai.getMessage());
        }catch(InvalidName in) {
            System.out.println(in.getMessage());
        }


    }

}

