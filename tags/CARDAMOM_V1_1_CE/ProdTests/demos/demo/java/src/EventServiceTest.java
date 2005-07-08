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


package cdmw.event;

import cdmw.testutils.Testable;
import cdmw.common.Locations;
import cdmw.ossupport.OS;

import cdmw.event.EventChannel;
import cdmw.event.TypedEventChannel;
import cdmw.event.EventChannelInterface;

import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

public class EventServiceTest extends Testable {

    public static final int NB_RCVMSG = 2;
    public static final String MSG = "Hello world";
    public static final String INTERFACE_REF
        = "IDL:thalesgroup.com/EchoModule/TypedEcho:1.0";

    public static final String CHANNEL_1_NAME = "EventChannel1";
    public static final String TYPED_CHANNEL_1_NAME = "TypedEventChannel1";
    public static final String TYPED_CHANNEL_2_NAME = "TypedEventChannel2";
    public static final String COMPLETE_CHANNEL_1_NAME
        = "EVENT/EVENT_CHANNELS/EventChannel1";
    public static final String COMPLETE_TYPED_CHANNEL_1_NAME
        = "EVENT/EVENT_CHANNELS/TypedEventChannel1";
    public static final String COMPLETE_TYPED_CHANNEL_2_NAME
        = "EVENT/EVENT_CHANNELS/TypedEventChannel2";

    public static boolean end = false;

    private org.omg.CORBA.ORB orb;
    private org.omg.CosNaming.NamingContext namingcontext;

    class PushConsumerImpl extends org.omg.CosEventComm.PushConsumerPOA {

        /**
         * The ORB
         */
        private org.omg.CORBA.ORB orb;

        /**
         * My POA
         */
        private org.omg.PortableServer.POA poa;

        public String[] rcvMsg = new String[10];

        public int nbRcvdMsg = 0;

        public PushConsumerImpl(
            org.omg.CORBA.ORB orb,
            org.omg.PortableServer.POA poa) {
            this.orb = orb;
            this.poa = poa;
        }

        public void push(org.omg.CORBA.Any data) {

            try {
                String str = data.extract_string();
                System.out.println("Consumer has received: " + str);
                rcvMsg[nbRcvdMsg] = str;
            } catch(org.omg.CORBA.SystemException se) {
                System.out.println("Consumer has an unknown event");
                rcvMsg[nbRcvdMsg] = "Unknown event";
            }
            nbRcvdMsg++;

        }

        public void disconnect_push_consumer() {

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
    public EventServiceTest(org.omg.CORBA.ORB orb,
        org.omg.CosNaming.NamingContext nc) {

        this.name = "Event Service Test";
        this.out = System.out;
        this.orb = orb;
        this.namingcontext = nc;

    }

    public void doTests() {

        boolean testOk = false;
        com.thalesgroup.EchoModule.TypedEcho pushConsumer1 = null;
        com.thalesgroup.EchoModule.TypedEcho pushConsumer2 = null;
        org.omg.PortableServer.POA rootPOA = null;
        PushConsumerImpl pcImpl = null;
        org.omg.CORBA.Object obj = null;
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(9);

        // This tests EventChannel class
        //-------------------------------------------------
        println("Testing Event Service");

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

            org.omg.CosEventChannelAdmin.EventChannel channel = null;

            // Get NamingInterface object from RepositoryInterface
            NamingInterface ni
                = RepositoryInterface.getDomainNamingInterface(
                    Locations.CDMW_SERVICES_NAME_DOMAIN);

            OS.sleep(getTimescale() * 10000);
            //Get a reference on the EventChannelFactory
            println("Find a factory to create a generic channel");
            testOk = true;
            obj = ni.resolve("EVENT/FACTORIES/ApplEventChannelFactory");
            com.thalesgroup.CdmwEvent.EventChannelFactory factory =
                com.thalesgroup.CdmwEvent.EventChannelFactoryHelper.narrow(obj);

            if (factory == null) {
                println("ApplEventChannelFactory is a nil object reference");
                testOk = false;
                check(testOk);
                return;
            }
            check(testOk);

            println("Creating a new generic channel EventChannel1");
            testOk = true;
            try {
                channel = factory.create_event_channel(CHANNEL_1_NAME);
            } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
                println("Unexpected CORBA exception raised: "
                    + ce.toString() + " " + ce.reason );
                testOk = false;
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                println("Unexpected CORBA exception raised: "
                    + ae.toString() + " " + ae.name );
                testOk = false;
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                println("Unexpected CORBA exception raised: "
                    + in.toString() + " " + in.reason );
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected CORBA exception raised: " + se.toString());
                testOk = false;
            } catch (Exception e) {
                // test failed!
                println("Unexpected exception raised!");
                testOk = false;
            }
            check(testOk);

            EventChannelInterface eventchannel1 =
                new EventChannelInterface(channel);
            TypedEventChannel typedeventchannel1 = null;
            TypedEventChannel typedeventchannel2 = null;

            println("Get Event Channel references");
            testOk = true;
            for (int i=0; i<200; i++) {
                try {
                   typedeventchannel2 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_2_NAME, namingcontext);
                   break;
                } catch (org.omg.CORBA.BAD_PARAM bp) {
                    // try again, maybe event channel is still not created!
                }
                OS.sleep(getTimescale() * 200);
            }

            if (eventchannel1 == null) {
                println("EventChannel4 not found!");
                testOk = false;
            }
            try {
                typedeventchannel1 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_1_NAME, namingcontext);
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println("Unexpected exception raised: " + bp.toString());
                testOk = false;
            }
            check(testOk);


            println("Get a PushConsumer on TypedEventChannel1");
            testOk = false;
            try {
                pushConsumer1 = com.thalesgroup.EchoModule.TypedEchoHelper.narrow(
                    typedeventchannel1.getTypedPushConsumer(INTERFACE_REF, null));
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Get a PushConsumer on TypedEventChannel2");
            testOk = false;
            try {
                pushConsumer2 = com.thalesgroup.EchoModule.TypedEchoHelper.narrow(
                    typedeventchannel1.getTypedPushConsumer(INTERFACE_REF, null));
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Connect local consumer to EventChannel1");
            testOk = false;
            pcImpl = new PushConsumerImpl(orb, rootPOA);
            try {
                eventchannel1.connectPushConsumer(pcImpl._this(orb));
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Send message to EventChannel1");
            testOk = true;

            println("Send <" + MSG + "> to EventChannel1" );
            pushConsumer1.echo_string(MSG);
            pushConsumer2.echo_string(MSG);

            // wait for received msg
            int loopIndex = 0;
            while (pcImpl.nbRcvdMsg != NB_RCVMSG) {
                OS.sleep(getTimescale() * 100);
                if (loopIndex == 100) {
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
            println("Destroy EventChannels...");
            testOk = false;
            try {
                eventchannel1.destroy();
                typedeventchannel1.destroy();
                typedeventchannel2.destroy();
                testOk = true;
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Try to find destroyed EventChannel1!");
            testOk = false;
            try {
                typedeventchannel1 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_1_NAME, namingcontext);
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
                testOk = true;
            }
            check(testOk);

        } catch (Exception e)  {
            println("ERROR: " + e.toString());
        }

    }

}

