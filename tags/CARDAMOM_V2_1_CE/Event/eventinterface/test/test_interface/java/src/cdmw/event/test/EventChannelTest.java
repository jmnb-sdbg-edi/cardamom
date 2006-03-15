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
import cdmw.event.EventChannel;
import cdmw.ossupport.OS;
import cdmw.namingandrepository.RepositoryInterface;

import com.thalesgroup.CdmwNamingAndRepository.Repository;

import org.omg.CORBA.ORB;
import org.omg.CORBA.SystemException;


public class EventChannelTest extends Testable {

    public static final short NB_MSG = 2;
    public static final short NB_RCVMSG = 10;
    public static final String[] MSG_ARRAY 
        = {"Hello world 1", "Hello world 2"};

    class PushConsumerImpl extends org.omg.CosEventComm.PushConsumerPOA {
    
        /**
         * the ORB
         */
        private ORB orb;
        
        /**
         * my POA
         */
        private org.omg.PortableServer.POA poa;
        
        /**
         * consumer id
         */
        private int id;
        
        /**
         * number of consumer
         */
        private int nbConsumer = 0;
    
        /**
         * received messages
         */
        public String[] rcvMsg = new String[10];
        
        /**
         * number of received messages
         */
        public short nbRcvdMsg = 0;
        
        public PushConsumerImpl(ORB orb, org.omg.PortableServer.POA poa) {
            this.orb = orb;
            this.poa = poa;
            nbConsumer++;
            this.id = nbConsumer;
        }
    
        public void push(org.omg.CORBA.Any data) throws SystemException {
            String str;            
            try {
                str = data.extract_string();
                System.out.println("Consumer" + id + " has received: " + str);
                rcvMsg[nbRcvdMsg] = str;
            } catch (org.omg.CORBA.BAD_OPERATION bo) {
                System.out.println("Consumer" + id + " has an unknown event");
                rcvMsg[nbRcvdMsg] = "Unknown event";
            }
            nbRcvdMsg++;
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
            orb.shutdown(false);
        }
    
        public org.omg.PortableServer.POA _default_POA() {
            return this.poa;
        }

    }
    
    public static final String CHANNEL_1_NAME = "TestEventChannel1";
    public static final String CHANNEL_2_NAME = "TestEventChannel2";
    public static final String CHANNEL_3_NAME = "TestEventChannel3";
    public static final String CHANNEL_4_NAME = "TestEventChannel4";
    public static final String COMPLETE_CHANNEL_1_NAME 
        = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel1";
    public static final String COMPLETE_CHANNEL_2_NAME 
        = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel2";
    public static final String COMPLETE_CHANNEL_3_NAME 
        = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel3";
    public static final String COMPLETE_CHANNEL_4_NAME 
        = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel4";
    public static final String UNCREATED_CHANNEL_NAME 
        = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestEventChannel6";
    public static final String INV_CHANNEL_NAME = "Test/EventChannel4";

    private org.omg.CORBA.ORB orb;

    public EventChannelTest(ORB orb) {
        this.out = System.out;
        this.name = "EventChannelTest";
        this.orb = orb;
    }

    public void doTests() {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(10);
    
        boolean testOk = false;
        org.omg.CosEventComm.PushConsumer pushConsumer = null;
        org.omg.PortableServer.POA rootPOA = null;

        // This tests EventChannel class
        //-------------------------------------------------

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

            // Get a reference to the CDMW Naming And Repository from RepositoryInterface
            Repository repository = RepositoryInterface.getRepository();

            // Get the default root context from the repository
            org.omg.CosNaming.NamingContext rootCtx =
                repository.resolve_root_context(
                com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);

            PushConsumerImpl pcImpl = new PushConsumerImpl(orb, rootPOA);
            PushConsumerImpl pcImpl2 = new PushConsumerImpl(orb, rootPOA);

            EventChannel eventchannel1 = null;
            EventChannel eventchannel2 = null;
            EventChannel eventchannel3 = null;
            EventChannel eventchannel4 = null;
            EventChannel eventchannel4_2 = null;

            println("Initialise an event channel with an invalid channel name");
            testOk = false;
            try {
                eventchannel1 = new EventChannel(INV_CHANNEL_NAME, rootCtx);
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
                testOk = true;
            }
            check(testOk);

            println("Initialise an event channel with an uncreated channel name");
            testOk = false;
            try {
                eventchannel1 = new EventChannel(UNCREATED_CHANNEL_NAME, rootCtx);
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
                testOk = true;
            } catch (SystemException se) {
                println("Unexpected exception raised !");
                se.printStackTrace();
                testOk = false;
            }
            check(testOk);

            println("Initialise event channels with valid channel names");
            testOk = true;
            try {
                eventchannel1 = new EventChannel(COMPLETE_CHANNEL_1_NAME, rootCtx);
            } catch (SystemException se) {
                println("Unexpected exception raised !");
                se.printStackTrace();
                testOk = false;
            }

            try {
                eventchannel2 = new EventChannel(COMPLETE_CHANNEL_2_NAME, rootCtx);
            } catch (SystemException se) {
                println("Unexpected exception raised !");
                se.printStackTrace();
                testOk = false;
            }
            
            try {
                eventchannel3 = new EventChannel(COMPLETE_CHANNEL_3_NAME, rootCtx);
            } catch (SystemException se) {
                println("Unexpected exception raised !");
                se.printStackTrace();
                testOk = false;
            }
            
            try {
                eventchannel4 = new EventChannel(COMPLETE_CHANNEL_4_NAME, rootCtx);
            } catch (SystemException se) {
                println("Unexpected exception raised !");
                se.printStackTrace();
                testOk = false;
            }
            check(testOk);

            println("Destroy eventchannel4");
            testOk = false;
            try {
                eventchannel4.destroy();
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised !");
            }
            check(testOk);

            println("Try to get interface on eventchannel4 again");
            testOk = false;
            try {
                eventchannel4_2 = new EventChannel(COMPLETE_CHANNEL_4_NAME, rootCtx);
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised");
                testOk = false;
            }
            check(testOk);

            println("Get a PushConsumer on eventchannel1");
            testOk = false;
            try {
                pushConsumer = eventchannel1.getPushConsumer(null);
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raise!");
            }
            check(testOk);

            println("Connect local consumer to eventchannel2");
            testOk = false;
            try {
                eventchannel2.connectPushConsumer(pcImpl._this(orb));
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Connect an other local consumer to eventchannel3");
            testOk = false;
            try {
                eventchannel3.connectPushConsumer(pcImpl2._this(orb));
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Connect eventchannel3 to eventchannel1");
            testOk = false;
            try {
                eventchannel3.connectToChannel(COMPLETE_CHANNEL_1_NAME);
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            // Get a reference to the POA manager.
            org.omg.PortableServer.POAManager manager 
                = rootPOA.the_POAManager();

            manager.activate();

            println("Send messages to eventchannel1");
            testOk = true;
            for (int i=0; i<NB_MSG; i++) {
                org.omg.CORBA.Any data = orb.create_any();
                System.out.println("Send <" + MSG_ARRAY[i] + "> to eventchannel1");
                try {
                    data.insert_string(MSG_ARRAY[i]);
                } catch(SystemException se) {
                    se.printStackTrace();
                }
                pushConsumer.push(data);
            }

            // wait for received msg
            OS.sleep(getTimescale() * 2000);
            int loopIndex = 0;
            while (pcImpl.nbRcvdMsg != NB_MSG) {
                OS.sleep(getTimescale() * 2);
                if (loopIndex == 200) {
                    testOk = false;
                    break;
                }
                loopIndex++;
            }

            OS.sleep(getTimescale() * 2000);
            if (testOk) {
                // message received! Compare them!
                for (int i=0; i<NB_MSG ; i++) {
                    if (!MSG_ARRAY[i].equals(pcImpl.rcvMsg[i])) {
                        testOk = false;
                    }
                }

                for (int i=0; i<NB_MSG ; i++) {
                    if (!MSG_ARRAY[i].equals(pcImpl2.rcvMsg[i])) {
                        testOk = false;
                    }
                }

            }
            check(testOk);

        } catch (Exception e) {
            println("ERROR: ");
            e.printStackTrace();
        }
    
    }

}


