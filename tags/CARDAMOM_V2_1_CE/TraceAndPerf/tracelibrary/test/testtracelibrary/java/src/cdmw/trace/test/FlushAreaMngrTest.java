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



public class FlushAreaMngrTest extends cdmw.testutils.Testable {

    public FlushAreaMngrTest() {
        this.out = System.out;
        this.name = "FlushAreaMngr tests";
    }

    public void doTests() {

    	// set number of requested successfull tests
		setNbOfRequestedTestOK(265);

        println( "Starting the Orb in it's own thread" );
        OrbThread orbThread = new OrbThread();
        orbThread.start();

        org.omg.CORBA.ORB orb = orbThread.getORB();
        org.omg.PortableServer.POA rootPOA = orbThread.getPOA();

        // create messages
        String message1 = "This is the first test message.";
        String message2 = "This is the second test message.";
        String message3 = "This is the third test message.";

        Runtime.getRuntime().traceInstructions(true);
        Runtime.getRuntime().traceMethodCalls(true);

        // construct a message header
        cdmw.trace.MessageHeader header =
            new cdmw.trace.MessageHeader(Thread.currentThread().getName(),
                                          "", // ECR-0123
                                          "TEST",
                                          (short) 1,
                                          cdmw.ossupport.OS.getTime());
        int headerSize = header.threadId.length() +
                         header.fileName.length() +
                         header.userDomain.length() +
                         header.componentName.length(); // ECR-0123

        // create the initial collector list set to empty
        com.thalesgroup.CdmwTrace.TraceProducerPackage.CollectorData[] collectorList = null;

        int flushingTime = 2000;   // 2 seconds

        println("Initializing the TraceLibrary");
        com.thalesgroup.CdmwTrace.TraceProducer traceProducer;
        try {
            traceProducer = cdmw.trace.FlushAreaMngr.init(
                     rootPOA,
                     collectorList,
                     com.thalesgroup.CdmwTrace.ALL_COMPONENT_NAMES.value, // ECR-0123
                     com.thalesgroup.CdmwTrace.ALL_DOMAINS.value,
                     com.thalesgroup.CdmwTrace.ALL_VALUES.value,
                     "application_name",
                     "process_name",
                     flushingTime,
                     2,                // 2 FlushArea
                     40 + headerSize,   // Each FlushArea has 40 octets
                     cdmw.trace.FlushAreaMngr.DEFAULT_AREA_NB_MESSAGES,
                     100               // threshold for repetitive messages
                    );
        } catch (cdmw.common.BadParameterException e) {
            fail("  ERROR: BadParameterException");
            e.printStackTrace();
            return;
        } catch (cdmw.common.InternalErrorException e) {
            fail("  ERROR: InternalErrorException");
            e.printStackTrace();
            return;
        }
        check(traceProducer != null);

        println("Retrieving the FlushAreaMngr singleton");
        cdmw.trace.FlushAreaMngr flushAreaMngr =
            cdmw.trace.FlushAreaMngr.getInstance();
        succeed();

        println("Creating a fake collector");
        CollectorSimulator collectorSimServant = new CollectorSimulator();
        com.thalesgroup.CdmwTrace.Collector collector = collectorSimServant._this(orb);

        println("Registering the collector into the TraceProducer object");
        try {
            traceProducer.register_collector(collector, "collector_name", "collector_mnemonic");
        } catch (com.thalesgroup.CdmwTrace.TraceProducerPackage.InvalidRegistration e) {
            fail("  ERROR: InvalidRegistration");
            e.printStackTrace();
            return;
        }
        succeed();

        boolean ret;

        println("Adding a first message");
        ret = flushAreaMngr.addMessage (header, message1);
        check(ret == true);

        println("Adding a second message");
        ret = flushAreaMngr.addMessage (header, message2);
        check(ret == true);

        // as the flush area manager contains only 2 flush areas,
        // the third message cannot be added => error message is generated
        println("Adding a third message");
        ret = flushAreaMngr.addMessage (header, message3);
        check(ret == false);

        println("Checking if the Collector has received no message");
        com.thalesgroup.CdmwTrace.FormattedMessage[] formattedMessageSeq =
            collectorSimServant.getFormattedMessageSeq();
        int nbMessages = formattedMessageSeq.length;
        println("Nb received messages : " + nbMessages);
        check(nbMessages == 0);
        // activate flushing
        flushAreaMngr.activateFlushing();

        println("Waiting for a free flush area to add a third message");
        do {
            try {
                cdmw.ossupport.OS.sleep(getTimescale() * flushingTime);
            } catch (InterruptedException e) {
                // ignore
            }
            ret = flushAreaMngr.addMessage (header, message3);
            // as the flush area manager contains only 2 flush areas,
            // the third message will be added only after a flush area
            // had been released by the area flusher
        } while(!ret);
        succeed();

        println("Third message added to the flush area manager");

        println("Waiting a while to let the AreaFlusher does its work");
        try {
            cdmw.ossupport.OS.sleep(getTimescale() * 5 * flushingTime);
        } catch (InterruptedException e) {
            // ignore
        }

        println("Checking if the Collector has received messages");
        formattedMessageSeq = collectorSimServant.getFormattedMessageSeq();
        nbMessages = formattedMessageSeq.length;
        println("Nb received messages : " + nbMessages);
        println("Received messages : ");
        for (int i = 0; i < nbMessages; i++) {
            println("   " + formattedMessageSeq[i].message);
        }
        // note : third message is "running out of flush areas"
        check(nbMessages == 4);

        // -----
        // message flooding

        println("Deactivating the flush area manager");
        flushAreaMngr.activateFlushing();

        println("Adding the second message 50 times");
        for (int i = 0; i < 50; i++) {
            ret = flushAreaMngr.addMessage(header, message2);
            check(ret == true);
        }
        succeed();

        println("Reactivating the flush area manager");
        flushAreaMngr.activateFlushing();

        println("Waiting for the AreaFlusher to do its work");
        try {
            cdmw.ossupport.OS.sleep(getTimescale() * flushingTime * 5);
        } catch (InterruptedException e) {
            // ignore
        }

        println("Checking if the Collector has received messages");
        formattedMessageSeq = collectorSimServant.getFormattedMessageSeq();
        nbMessages = formattedMessageSeq.length;
        println("Nb received messages : " + nbMessages);
        println("Received messages : ");
        for (int i = 0; i < nbMessages; i++) {
            println("   " + formattedMessageSeq[i].message);
        }
        check(nbMessages == 6);

        println("Deactivating the flush area manager");
        flushAreaMngr.activateFlushing();

        println("Adding the first message 88 times");
        for (int i = 0; i < 88; i++) {
            ret = flushAreaMngr.addMessage(header, message1);
            check(ret == true);
        }
        succeed();

        println("Reactivating the flush area manager");
        flushAreaMngr.activateFlushing();

        println("Waiting for the AreaFlusher to do its work");
        try {
            cdmw.ossupport.OS.sleep(getTimescale() * flushingTime * 5);
        } catch (InterruptedException e) {
            // ignore
        }

        println("Checking if the Collector has received messages");
        formattedMessageSeq = collectorSimServant.getFormattedMessageSeq();
        nbMessages = formattedMessageSeq.length;
        println("Nb received messages : " + nbMessages);
        println("Received messages : ");
        for (int i = 0; i < nbMessages; i++) {
            println("   " + formattedMessageSeq[i].message);
        }
        check(nbMessages == 8);

        println("Deactivating the flush area manager");
        flushAreaMngr.activateFlushing();

        println("Adding the third message 100 times");
        for (int i = 0; i < 100; i++) {
            do {
                ret = flushAreaMngr.addMessage (header, message3);

                if (!ret) {
                    try {
                        cdmw.ossupport.OS.sleep(getTimescale() * flushingTime);
                    } catch (InterruptedException e) {
                        // ignore
                    }
                }
            } while(!ret);

            check(ret == true);
        }
        succeed();

        println("Reactivating the flush area manager");
        flushAreaMngr.activateFlushing();

        println("Waiting for the AreaFlusher to do its work");
        try {
            cdmw.ossupport.OS.sleep(getTimescale() * flushingTime * 5);
        } catch (InterruptedException e) {
            // ignore
        }

        println("Checking if the Collector has received messages");
        formattedMessageSeq = collectorSimServant.getFormattedMessageSeq();
        nbMessages = formattedMessageSeq.length;
        println("Nb received messages : " + nbMessages);
        println("Received messages : ");
        for (int i = 0; i < nbMessages; i++) {
            println("   " + formattedMessageSeq[i].message);
        }
        check(nbMessages == 10);

        // -----

        println("Checking if an unexisting domain/level is to be traced");
        traceProducer.deactivate_level (com.thalesgroup.CdmwTrace.ALL_COMPONENT_NAMES.value, // ECR-0123
                                        com.thalesgroup.CdmwTrace.ALL_DOMAINS.value,
                                        com.thalesgroup.CdmwTrace.ALL_VALUES.value);
        traceProducer.activate_level ("", "domain1", (short) 1); // ECR-0123
        check (cdmw.trace.FlushAreaMngr.isToBeTraced("", "foo", (short) 25) == false);

        println ("Trying to activate a domain/level using the TraceProducer interface");
        traceProducer.activate_level ("", "foo", (short) 25); // ECR-0123
        succeed();

        println("Checking if the level is returned activated by");
        println("     the FilterMngr");
        check (cdmw.trace.FlushAreaMngr.isToBeTraced("", "foo", (short) 25) == true); // ECR-0123

        println ("Trying to deactivate a domain/level using the TraceProducer interface");
        traceProducer.deactivate_level ("", "foo", (short) 25); // ECR-0123
        succeed();

        println("Checking if the level is returned deactivated by");
        println("     the FilterMngr");
        check(cdmw.trace.FlushAreaMngr.isToBeTraced("", "foo", (short) 25) == false); // ECR-0123

        println("Doing the test for different Domain/level");

        traceProducer.activate_level("", "foo", (short) 2); // ECR-0123
        traceProducer.activate_level("", "foo", (short) 5); // ECR-0123
        check (cdmw.trace.FlushAreaMngr.isToBeTraced("", "foo", (short) 2) == true); // ECR-0123
        check (cdmw.trace.FlushAreaMngr.isToBeTraced("", "foo", (short) 5) == true); // ECR-0123

        traceProducer.deactivate_level ("", "foo", (short) 2); // ECR-0123
        check (cdmw.trace.FlushAreaMngr.isToBeTraced("", "foo", (short) 2) == false); // ECR-0123

        traceProducer.activate_level ("", "bar", (short) 5); // ECR-0123
        check (cdmw.trace.FlushAreaMngr.isToBeTraced("", "bar", (short) 5) == true); // ECR-0123

        traceProducer.activate_level ("", "bar", (short) 4); // ECR-0123
        check (cdmw.trace.FlushAreaMngr.isToBeTraced("", "bar", (short) 4) == true); // ECR-0123

        traceProducer.deactivate_level ("", "bar", (short) 5); // ECR-0123
        check (cdmw.trace.FlushAreaMngr.isToBeTraced("", "bar", (short) 5) == false); // ECR-0123

        println ("Trying to cleanup the TraceLibrary");
        cdmw.trace.FlushAreaMngr.cleanup();
        succeed();

        println ("Stopping the ORB");
        orbThread.shutdown();

        try {
            orbThread.join();
        } catch (InterruptedException e) {
            // ignore
        }

    }

    public static void main(String[] args) {
        FlushAreaMngrTest test = new FlushAreaMngrTest();
        test.start();
    }
}
