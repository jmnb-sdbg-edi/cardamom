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

package cdmw.event.test;

import cdmw.testutils.Testable;
import cdmw.ossupport.OS;
import cdmw.event.EventChannel;

import org.omg.CORBA.SystemException;

public class CdmwEventServiceTest extends Testable {

    public static final int NB_RCVMSG = 4;
    public static final String MSG = "Hello world";

    public static final String CHANNEL_1_NAME = "EventChannel1";
    public static final String CHANNEL_2_NAME = "EventChannel2";
    public static final String CHANNEL_3_NAME = "EventChannel3";
    public static final String CHANNEL_4_NAME = "EventChannel4";
    public static final String COMPLETE_CHANNEL_1_NAME
        = "EVENT_CHANNELS/EventChannel1";
    public static final String COMPLETE_CHANNEL_2_NAME
        = "EVENT_CHANNELS/EventChannel2";
    public static final String COMPLETE_CHANNEL_3_NAME
        = "EVENT_CHANNELS/EventChannel3";
    public static final String COMPLETE_CHANNEL_4_NAME
        = "EVENT_CHANNELS/EventChannel4";

    public static boolean end = false;

    private org.omg.CORBA.ORB orb;
    private org.omg.CosNaming.NamingContext namingcontext;
    private int testNb;

    class PushConsumerImpl extends org.omg.CosEventComm.PushConsumerPOA {

        /**
         * The ORB
         */
        private org.omg.CORBA.ORB orb;

        /**
         * My POA
         */
        private org.omg.PortableServer.POA poa;

        /**
         * ref of the consumer to which msg received is pushed.
         */
        private org.omg.CosEventComm.PushConsumer pushConsumer;

        /**
         * ref of the consumer to consumer id
         */
        private int id;

        public String[] rcvMsg = new String[10];

        public int nbRcvdMsg = 0;

        public PushConsumerImpl(
            org.omg.CORBA.ORB orb,
            org.omg.PortableServer.POA poa,
            org.omg.CosEventComm.PushConsumer pushConsumer,
            int id) {

            this.orb = orb;
            this.poa = poa;
            this.pushConsumer = pushConsumer;
            this.id = id;

        }

        public void push(org.omg.CORBA.Any data) throws SystemException {

            try {
                String str = data.extract_string();
                System.out.println("Consumer" + id + " has received: " + str);
                rcvMsg[nbRcvdMsg] = str;
            } catch(SystemException se) {
                System.out.println("Consumer" + id + " has an unknown event");
                rcvMsg[nbRcvdMsg] = "Unknown event";
            }
            nbRcvdMsg++;

            if (pushConsumer!=null) {
                try {
                    pushConsumer.push(data);
                } catch(org.omg.CosEventComm.Disconnected d) {
                    d.printStackTrace();
                }
            }

        }

        public void disconnect_push_consumer()
            throws SystemException {

            System.out.println("Disconnect");
            try {
                byte[] oid = poa.servant_to_id(this);
                poa.deactivate_object(oid);
            } catch(org.omg.PortableServer.POAPackage.ObjectNotActive ona) {
                ona.printStackTrace();
            } catch(org.omg.PortableServer.POAPackage.ServantNotActive sna) {
                sna.printStackTrace();
            } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
                wp.printStackTrace();
            }
            end = true;

        }

        public org.omg.PortableServer.POA _default_POA() {
            return this.poa;
        }

    }

    /**
     * Constructor
     *
     *@param
     *@exception
     */
    public CdmwEventServiceTest(org.omg.CORBA.ORB orb,
        org.omg.CosNaming.NamingContext nc, int testNb) {

        this.name = "CdmwEventService TestProcess" + testNb;
        this.out = System.out;
        this.orb = orb;
        this.namingcontext = nc;
        this.testNb = testNb;

    }

    public void doTests() {

        boolean testOk = false;
        org.omg.CosEventComm.PushConsumer pushConsumer = null;
        org.omg.PortableServer.POA rootPOA = null;
        PushConsumerImpl pcImpl = null;
        
        
        switch (testNb) {
            case 1:
              // set number of requested successfull tests
		      setNbOfRequestedTestOK(6);
		      break;

            default:
              // set number of requested successfull tests
		      setNbOfRequestedTestOK(3);
              break;
        }


        // This tests EventChannel class
        //-------------------------------------------------
        println("Testing Cdmw Event Service");

        try {
            // Create the implementation object.
            // Resolve Root POA.
            rootPOA = org.omg.PortableServer.POAHelper.narrow(
                orb.resolve_initial_references("RootPOA"));

            if (rootPOA==null) {
                System.out.println("ERROR: No RootPOA");
                check(false);
                return;
            }

            EventChannel eventchannel1 = null;
            EventChannel eventchannel2 = null;
            EventChannel eventchannel3 = null;
            EventChannel eventchannel4 = null;

            println("Get Event Channel references");
            testOk = true;
            for (int i=0; i<200; i++) {
                try {
                    eventchannel4 = new EventChannel(COMPLETE_CHANNEL_4_NAME, namingcontext);
                    break;
                } catch (org.omg.CORBA.BAD_PARAM bp) {
                    // try again, maybe event channel is still not created!
                }
                OS.sleep(getTimescale() * 20);
            }

            if (eventchannel4 == null) {
                testOk = false;
                println("EventChannel4 not found!");
            }

            try {
                eventchannel2 = new EventChannel(COMPLETE_CHANNEL_2_NAME, namingcontext);
                eventchannel3 = new EventChannel(COMPLETE_CHANNEL_3_NAME, namingcontext);
                eventchannel1 = new EventChannel(COMPLETE_CHANNEL_1_NAME, namingcontext);
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println("Unexpected exception raised !");
                bp.printStackTrace();
                testOk = false;
            }
            check(testOk);

            switch (testNb) {
            case 1:
                int loopIndex = 0;

                println("Get a PushConsumer on EventChannel1");
                testOk = false;
                try {
                    pushConsumer = eventchannel1.getPushConsumer(null);
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                println("Connect local consumer to EventChannel2");
                testOk = false;
                pcImpl = new PushConsumerImpl(orb, rootPOA, null, testNb);
                try {
                    eventchannel2.connectPushConsumer(pcImpl._this(orb));
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                OS.sleep(getTimescale() * 2000);

                println("Send message to EventChannel1");
                testOk = true;

                org.omg.CORBA.Any data = orb.create_any();
                println("Send <" + MSG + "> to Eventchannel1");
                try {
                    data.insert_string(MSG);
                } catch(SystemException se) {
                    se.printStackTrace();
                }
                pushConsumer.push(data);

                // wait for received msg
                OS.sleep(getTimescale() * 1000);
                while (pcImpl.nbRcvdMsg != NB_RCVMSG) {
                    OS.sleep(getTimescale() * 10);
                    if (loopIndex == 100) {
                        testOk = false;
                        break;
                    }
                    loopIndex++;
                }

                println("Number of received message = " + pcImpl.nbRcvdMsg);

                if (testOk) {
                    // messages received! Compare them!
                    for (int i=0; i<NB_RCVMSG ; i++) {
                        if (!MSG.equals(pcImpl.rcvMsg[i])) {
                            testOk = false;
                        }
                    }
                }
                check(testOk);

                OS.sleep(getTimescale() * 2000);
                println("Destroy EventChannels...");
                testOk = false;
                try {
                    println("    Destroy EventChannel2...");
                    eventchannel2.destroy();
                    println("    Destroy EventChannel3...");
                    eventchannel3.destroy();
                    println("    Destroy EventChannel1...");
                    eventchannel1.destroy();
                    println("    Destroy EventChannel4...");
                    eventchannel4.destroy();
                    testOk = true;
                } catch (Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                OS.sleep(getTimescale() * 1000);
                println("Try to find EventChannel1!");
                testOk = false;
                try {
                    eventchannel1 = new EventChannel(COMPLETE_CHANNEL_1_NAME, namingcontext);
                } catch (org.omg.CORBA.BAD_PARAM bp) {
                    println(bp.toString());
                    testOk = true;
                }
                check(testOk);
                while (!end) {
                    OS.sleep(getTimescale() * 100);
                }
                println("TestProcess1 ending...");
                orb.shutdown(false);
                break;

            case 2:
                println("Get a PushConsumer on EventChannel3");
                testOk = false;
                try {
                    pushConsumer = eventchannel3.getPushConsumer(null);
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raise!");
                    e.printStackTrace();
                }
                check(testOk);

                println("Connect local consumer to EventChannel1");
                testOk = false;
                pcImpl = new PushConsumerImpl(orb, rootPOA, pushConsumer, testNb);
                try {
                    eventchannel1.connectPushConsumer(pcImpl._this(orb));
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                // wait for disconnect on consumer
                while (!end) {
                    OS.sleep(getTimescale() * 100);
                }

                println("TestProcess2 ending...");

                orb.shutdown(false);
                break;

            case 3:
                println("Get a PushConsumer on EventChannel2");
                testOk = false;
                try {
                    pushConsumer = eventchannel2.getPushConsumer(null);
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                println("Connect local consumer to EventChannel3");
                testOk = false;
                pcImpl = new PushConsumerImpl(orb, rootPOA, pushConsumer, testNb);
                try {
                    eventchannel3.connectPushConsumer(pcImpl._this(orb));
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                // wait for disconnect on consumer
                while (!end) {
                    OS.sleep(getTimescale() * 100);
                }

                println("TestProcess3 ending...");

                orb.shutdown(false);
                break;

            case 4:
                println("Get a PushConsumer on EventChannel4");
                testOk = false;
                try {
                    pushConsumer = eventchannel4.getPushConsumer(null);
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raise!");
                    e.printStackTrace();
                }
                check(testOk);

                println("Connect local consumer to EventChannel3");
                testOk = false;
                pcImpl = new PushConsumerImpl(orb, rootPOA, pushConsumer, testNb);
                try {
                    eventchannel3.connectPushConsumer(pcImpl._this(orb));
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                // wait for disconnect on consumer
                while (!end) {
                    OS.sleep(getTimescale() * 100);
                }

                println("TestProcess4 ending...");

                orb.shutdown(false);
                break;

            default:
                println("Invalid test number!");
                break;
            }
        } catch (Exception e) {
            System.out.println("ERROR: ");
            e.printStackTrace();
        }

    }

}

