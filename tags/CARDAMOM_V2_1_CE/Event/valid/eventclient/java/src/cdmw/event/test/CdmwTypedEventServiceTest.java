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
import cdmw.ossupport.OS;
import cdmw.event.TypedEventChannel;

import org.omg.CORBA.SystemException;

public class CdmwTypedEventServiceTest extends Testable {

    public final static int NB_RCVMSG = 4;
    public final static String MSG = "Hello world";
    public final static String INTERFACE_REF = 
        "IDL:test.event.cdmw/TypedEcho:1.0";

    public final static String CHANNEL_1_NAME = "TypedEventChannel1";
    public final static String CHANNEL_2_NAME = "TypedEventChannel2";
    public final static String CHANNEL_3_NAME = "TypedEventChannel3";
    public final static String CHANNEL_4_NAME = "TypedEventChannel4";
    public final static String COMPLETE_CHANNEL_1_NAME
        = "EVENT_CHANNELS/TypedEventChannel1";
    public final static String COMPLETE_CHANNEL_2_NAME
        = "EVENT_CHANNELS/TypedEventChannel2";
    public final static String COMPLETE_CHANNEL_3_NAME
        = "EVENT_CHANNELS/TypedEventChannel3";
    public final static String COMPLETE_CHANNEL_4_NAME
        = "EVENT_CHANNELS/TypedEventChannel4";

    public static boolean end = false;

    // ----------------------------------------------------------------------
    // EchoImpl implementation
    // ----------------------------------------------------------------------
    class EchoImpl extends TypedEchoPOA {

        /**
         * The ORB
         */
        private org.omg.CORBA.ORB orb;

        /**
         * My POA.
         */
        private org.omg.PortableServer.POA poa;

        /**
         * ref of the consumer to which msg received is pushed.
         */
        private TypedEcho pushConsumer;

        /**
         * consumer id
         */
        private int id;

        public String[] rcvMsg = new String[10];
        public int nbRcvdMsg = 0;

        public EchoImpl(org.omg.CORBA.ORB orb, org.omg.PortableServer.POA poa,
            TypedEcho pushConsumer, int id) {
                this.orb = orb;
                this.poa = poa;
                this.pushConsumer = pushConsumer;
                  this.id = id;
            }

        public void echo_string(String str) throws SystemException {
            System.out.println("Consumer" + id + " has received: " + str );
            rcvMsg[nbRcvdMsg] = str;
            nbRcvdMsg++;
            if (pushConsumer!=null) {
                pushConsumer.echo_string(str);
            }
        }

        public void disconnect_push_consumer() throws SystemException {
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

        public void push(org.omg.CORBA.Any data) throws SystemException {
            System.out.println("push any not implementedException e" );
        }

        public org.omg.PortableServer.POA _default_POA() {
            return this.poa;
        }
    }

    private org.omg.CORBA.ORB orb;
    private org.omg.CosNaming.NamingContext namingcontext;
    private int testNb;

    /**
     * Constructor
     *
     *@param
     *@exception
     */
    public CdmwTypedEventServiceTest(org.omg.CORBA.ORB orb,
        org.omg.CosNaming.NamingContext nc, int testNb) {

        this.name = "CdmwTypedEventService TestProcess" + testNb;
        this.out = System.out;
        this.orb = orb;
        this.namingcontext = nc;
         this.testNb = testNb;

    }


    public void doTests() {

        boolean testOk = false;
        TypedEcho pushConsumer = null;
        org.omg.PortableServer.POA rootPOA = null;
        EchoImpl pcImpl = null;
        org.omg.CORBA.Object obj = null;

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
        println("Testing Cdmw Typed Event Service");

        try {
            // Create the implementation object.
            // Resolve Root POA.
            rootPOA = org.omg.PortableServer.POAHelper.narrow(
                orb.resolve_initial_references("RootPOA"));

            if (rootPOA==null) {
                println("ERROR: No RootPOA");
                check(false);
                return;
            }

            TypedEventChannel eventchannel1 = null;
            TypedEventChannel eventchannel2 = null;
            TypedEventChannel eventchannel3 = null;
            TypedEventChannel eventchannel4 = null;

            println("Get Event Channel references");
            testOk = true;
            for (int i=0; i<200; i++) {
                try {
                    eventchannel4 = new TypedEventChannel(COMPLETE_CHANNEL_4_NAME, namingcontext);
                    break;
                } catch (org.omg.CORBA.BAD_PARAM bp) {
                    // try again, maybe event channel is still not created!
                }
                OS.sleep(getTimescale() * 20);
            }

            if (eventchannel4 == null) {
                println("EventChannel4 not found!");
                testOk = false;
            }
            try {
                eventchannel1 = new TypedEventChannel(COMPLETE_CHANNEL_1_NAME, namingcontext);
                eventchannel2 = new TypedEventChannel(COMPLETE_CHANNEL_2_NAME, namingcontext);
                eventchannel3 = new TypedEventChannel(COMPLETE_CHANNEL_3_NAME, namingcontext);
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println("Unexpected exception raised: " + bp.getMessage());
                testOk = false;
            }
            check(testOk);

            switch (testNb) {
            case 1:
                int loopIndex = 0;

                println("Get a PushConsumer on EventChannel1");
                testOk = false;
                try {
                    pushConsumer = TypedEchoHelper.narrow(
                        eventchannel1.getTypedPushConsumer(INTERFACE_REF,null));
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raise!");
                    e.printStackTrace();
                }
                check(testOk);

                println("Connect local consumer to EventChannel2");
                testOk = false;
                pcImpl = new EchoImpl(orb, rootPOA, null, testNb);
                try {
                    eventchannel2.connectTypedPushConsumer(INTERFACE_REF, pcImpl._this(orb));
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                OS.sleep(getTimescale() * 2000);

                println("Send message to EventChannel1");
                testOk = true;

                println("Send <" + MSG + "> to EventChannel1");
                pushConsumer.echo_string(MSG);

                // wait for received msg
                while (pcImpl.nbRcvdMsg != NB_RCVMSG) {
                    OS.sleep(getTimescale() * 10);
                    if (loopIndex == 200) {
                        testOk = false;
                        break;
                    }
                    loopIndex++;
                }

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
                println("Destroy EventChannelsException...");
                testOk = false;
                try {
                    eventchannel1.destroy();
                    eventchannel2.destroy();
                    eventchannel3.destroy();
                    eventchannel4.destroy();
                    testOk = true;
                } catch (Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                println("Try to find EventChannel1!");
                testOk = false;
                try {
                    eventchannel1 = new TypedEventChannel(COMPLETE_CHANNEL_1_NAME, namingcontext);
                } catch (org.omg.CORBA.BAD_PARAM bp) {
                    println(bp.getMessage());
                    testOk = true;
                }
                check(testOk);

                // wait for disconnect on consumer
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
                    pushConsumer = TypedEchoHelper.narrow(
                        eventchannel3.getTypedPushConsumer(INTERFACE_REF, null));
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                println("Connect local consumer to EventChannel1");
                testOk = false;
                pcImpl = new EchoImpl(orb, rootPOA, pushConsumer, testNb);
                try {
                    eventchannel1.connectTypedPushConsumer(INTERFACE_REF,pcImpl._this(orb));
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
                    pushConsumer = TypedEchoHelper.narrow(
                        eventchannel2.getTypedPushConsumer(INTERFACE_REF,null));
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raised!");
                }
                check(testOk);

                println("Connect local consumer to EventChannel3");
                testOk = false;
                pcImpl = new EchoImpl(orb, rootPOA, pushConsumer, testNb);
                try {
                    eventchannel3.connectTypedPushConsumer(INTERFACE_REF, pcImpl._this(orb));
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
                    pushConsumer = TypedEchoHelper.narrow(
                        eventchannel4.getTypedPushConsumer(INTERFACE_REF,null));
                    testOk = true;
                } catch(Exception e) {
                    println("Unexpected exception raised!");
                    e.printStackTrace();
                }
                check(testOk);

                println("Connect local consumer to EventChannel3");
                testOk = false;
                pcImpl = new EchoImpl(orb, rootPOA, pushConsumer, testNb);
                try {
                    eventchannel3.connectTypedPushConsumer(INTERFACE_REF, pcImpl._this(orb));
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
            println("ERROR: ");
            e.printStackTrace();
        }

    }

}

