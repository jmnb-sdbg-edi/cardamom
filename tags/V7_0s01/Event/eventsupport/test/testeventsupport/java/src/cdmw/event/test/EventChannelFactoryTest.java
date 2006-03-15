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


package cdmw.event.test;

import cdmw.testutils.Testable;

import com.thalesgroup.CdmwEvent.EventChannelFactory;


public class EventChannelFactoryTest extends Testable {

    public static final String CHANNEL_1_NAME = "TestEventChannel1";
    public static final String CHANNEL_2_NAME = "TestEventChannel2";
    public static final String MIS_URL_CHANNEL_NAME = "TestEventChannel3";
    public static final String UNDEF_XML_CHANNEL_NAME = "TestEventChannel4";
    public static final String INV_CHANNEL_NAME = "Test/EventChannel4";

    /**
     * [Attribute description]
     */
    private EventChannelFactory factory;
        
    /**
     * indicate the current test number
     */ 
    private int testNumber;
    
    /**
     * [Constructor description if necessary]
     * 
     *@param
     *@exception
     */ 
    public EventChannelFactoryTest(EventChannelFactory factory) {
        this.out = System.out;
        this.name = "EventChannelFactoryTest";
        this.testNumber = 1;
        this.factory = factory;
    }

    /**
     * Init and start next test
     * 
     *@param
     *@return
     *@exception
     */ 
    public void nextTest() {
        testNumber++;
        doTests();
    }
    
    public void startTests() {
        this.testNumber = 1;
        doTests();
    }

    public void doTests() {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(7);
    
        boolean testOk = false;
        org.omg.CosEventChannelAdmin.EventChannel channel;

        // This tests EventChannelFactory class
        //-------------------------------------------------

        try  {
        switch (testNumber) {
        case 1 :
            out.println("Creating a new channel CHANNEL 1" 
                + " while EventchannelManager is not running");
            try {
                channel = factory.create_event_channel(CHANNEL_1_NAME);
            } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
                testOk = true;
                out.println("Unexpected CORBA exception raised: " + ce.reason);
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                out.println("Unexpected CORBA exception raised: " + ae.name);
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                out.println("Unexpected CORBA exception raised: " + in.reason);
            } catch (Exception e) {
                // test failed!
                out.println("Unexpected exception raised!");
                e.printStackTrace();
            }
            check(testOk);
            break;

        case 2:
            out.println("Creating a new channel CHANNEL 1"
                + " while EventServer is not running");
            testOk = false;
            try {
                channel = factory.create_event_channel(CHANNEL_1_NAME);
            } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
                out.println("Unexpected CORBA exception raised: " + ce.reason);
                ce.printStackTrace();
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                out.println("Unexpected CORBA exception raised: " + ae.name);
                ae.printStackTrace();
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                out.println("Unexpected CORBA exception raised: " + in.reason);
                in.printStackTrace();
            } catch (org.omg.CORBA.TRANSIENT t) {
                testOk = true;
                out.println("CORBA exception raised: " + t.getMessage());
            } catch (Exception e) {
                // test failed!
                out.println("Unexpected exception raised!");
                e.printStackTrace();
            }
            check(testOk);
            break;

        case 3:
            out.println("Creating a new channel CHANNEL 4 undefined in the XML file");
            testOk = false;
            try {
                channel = factory.create_event_channel(UNDEF_XML_CHANNEL_NAME);
                testOk = true;
            } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
                out.println("Unexpected CORBA exception raised: " + ce.reason);
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                out.println("Unexpected CORBA exception raised: " + ae.name);
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                out.println("Unexpected CORBA exception raised: " + in.reason);
            } catch (Exception e) {
                // test failed!
                out.println("Unexpected exception raised!");
                e.printStackTrace();
            }
            check(testOk);

            testOk = false;
            out.println("Creating a new channel CHANNEL 1");
            try {
                channel = factory.create_event_channel(CHANNEL_1_NAME);
                testOk = true;
            } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
                out.println("Unexpected CORBA exception raised: " + ce.reason);
                ce.printStackTrace();
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                out.println("Unexpected CORBA exception raised: " + ae.name);
                ae.printStackTrace();
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                out.println("Unexpected CORBA exception raised: " + in.reason);
                in.printStackTrace();
            } catch (Exception e) {
                // test failed!
                out.println("Unexpected exception raised!");
                e.printStackTrace();
            }
            check(testOk);

            out.println("Creating a new channel CHANNEL 2");
            testOk = false;
            try {
                channel = factory.create_event_channel(CHANNEL_2_NAME);
                testOk = true;
            } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
                out.println("Unexpected CORBA exception raised: " + ce.reason);
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                out.println("Unexpected CORBA exception raised: " + ae.name);
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                out.println("Unexpected CORBA exception raised: " + in.reason);
            } catch (Exception e) {
                // test failed!
                out.println("Unexpected exception raised!");
                e.printStackTrace();
            }
            check(testOk);

            out.println("Creating a new channel CHANNEL 1 "
                + "=> should raise AlreadyExistant exception ");
            testOk = false;
            try {
                channel = factory.create_event_channel(CHANNEL_1_NAME);
            } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
                out.println("Unexpected CORBA exception raised: " + ce.reason);
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                testOk = true;
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                out.println("Unexpected CORBA exception raised: " + in.reason);
            } catch (Exception e) {
                // test failed!
                out.println("Unexpected exception raised!");
            }
            check(testOk);

            out.println("Creating a new channel CHANNEL 3 with no URL defined "
                + "=> should raise CreationError exception ");
            testOk = false;
            try {
                channel = factory.create_event_channel(MIS_URL_CHANNEL_NAME);
            } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
                testOk = true;
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                out.println("Unexpected CORBA exception raised: " + ae.name);
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                out.println("Unexpected CORBA exception raised: " + in.reason);
            } catch (Exception e) {
                // test failed!
                out.println("Unexpected exception raised!");
            }
            check(testOk);

            break;

        default:
            out.println("**** NO MORE TEST AVAILABLE!");
            break;
        }
        } catch (Exception e) {
            e.printStackTrace();
        }
    
    }


}


