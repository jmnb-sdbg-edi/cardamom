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


package cdmw.event.test;

import cdmw.testutils.Testable;
import cdmw.ossupport.OS;
import cdmw.event.TypedEventChannel;
import cdmw.namingandrepository.RepositoryInterface;

import com.thalesgroup.CdmwNamingAndRepository.Repository;

import org.omg.CORBA.ORB;
import org.omg.CORBA.SystemException;
import org.omg.PortableServer.POA;


public class TypedEventChannelTest extends Testable {

    public static final short NB_MSG = 2;
    public static final short NB_RCVMSG = 10;
    public static final String[] MSG_ARRAY 
        = {"Hello world 1", "Hello world 2"};
        
    // ----------------------------------------------------------------------
    // InvalidConsumerImpl implementation
    // ----------------------------------------------------------------------
    class InvalidConsumerImpl extends EchoPOA {
    
        private ORB orb;
        private POA poa;
        
        public String[] rcvMsg = new String[10];
        public int nbRcvdMsg;
        
        public InvalidConsumerImpl(ORB orb, POA poa) {
            this.orb = orb;
            this.poa = poa;
        }
        
        public void echo_string(String str) throws SystemException {
            System.out.println("Invalid Consumer has received: " + str );
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
    
        public void push(org.omg.CORBA.Any data) throws SystemException {
            System.out.println("push any not implementedException e" );
        }
        
        public POA _default_POA() {
            return this.poa;
        } 

    }
    
    // ----------------------------------------------------------------------
    // EchoImpl implementation
    // ----------------------------------------------------------------------
    class EchoImpl extends TypedEchoPOA {
    
        /**
         * The ORB
         */
        private ORB orb;
        
        /**
         * My POA.
         */
        private POA poa;
        
        /**
         * number of consumer
         */
        public int nbConsumer = 0;
        
        /**
         * consumer id
         */
        public int id;
    
        public String[] rcvMsg = new String[10];
        public int nbRcvdMsg = 0;
        
        public EchoImpl(ORB orb, POA poa) {
            this.orb = orb;
            this.poa = poa;
            nbConsumer++;
            id = nbConsumer;
        }
        
        public void echo_string(String str) throws SystemException {
            System.out.println("Consumer" + id + " has received: " + str );
            rcvMsg[nbRcvdMsg] = str;
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
    
        public void push(org.omg.CORBA.Any data) throws SystemException {
            System.out.println("push any not implementedException e" );
        }
        
        public POA _default_POA() {
            return this.poa;
        }
        
    }
    
    public static final String TYPED_CHANNEL_1_NAME = "TestTypedEventChannel1";
    public static final String TYPED_CHANNEL_2_NAME = "TestTypedEventChannel2";
    public static final String TYPED_CHANNEL_3_NAME = "TestTypedEventChannel3";
    public static final String TYPED_CHANNEL_4_NAME = "TestTypedEventChannel4";
    public static final String COMPLETE_TYPED_CHANNEL_1_NAME 
        = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestTypedEventChannel1";
    public static final String COMPLETE_TYPED_CHANNEL_2_NAME 
        = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestTypedEventChannel2";
    public static final String COMPLETE_TYPED_CHANNEL_3_NAME 
        = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestTypedEventChannel3";
    public static final String COMPLETE_TYPED_CHANNEL_4_NAME 
        = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestTypedEventChannel4";
    public static final String UNCREATED_TYPED_CHANNEL_NAME 
        = "CDMW/SERVICES/EVENT/EVENT_CHANNELS/TestTypedEventChannel6";
    public static final String INV_TYPED_CHANNEL_NAME 
        = "Test/TypedEventChannel4";

    //public static final String INTERFACE_REF = "IDL:TypedEcho:1.0";
    public static final String INTERFACE_REF = "IDL:test.event.cdmw/TypedEcho:1.0";
    public static final String INVALID_INTERFACE_REF= "IDL:InvalidInterface:1.0";

    private org.omg.CORBA.ORB orb;

    public TypedEventChannelTest(ORB orb) {
        this.name = "TypedEventChannelTest";
        this.out = System.out;
        this.orb = orb;
    }

    public void doTests() {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(12);
        
        boolean testOk = false;
        TypedEcho pushConsumer = null;
        org.omg.CORBA.Object obj = null;
        POA rootPOA = null;


        // This tests TypedEventChannel class
        //-------------------------------------------------

        try {
            // Create the implementation object.
            // Resolve Root POA.
            rootPOA = org.omg.PortableServer.POAHelper.narrow(
                orb.resolve_initial_references("RootPOA"));

            if (rootPOA==null) {
                System.out.println("ERROR: No RootPOA" );
                check(false);
                return;
            }

             // Get a reference to the CDMW Naming And Repository from RepositoryInterface
            Repository repository = RepositoryInterface.getRepository();

            // Get the default root context from the repository
            org.omg.CosNaming.NamingContext rootCtx =
                repository.resolve_root_context(
                    com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);

            EchoImpl pcImpl = new EchoImpl(orb, rootPOA);
            EchoImpl pcImpl2 = new EchoImpl(orb, rootPOA);

            TypedEventChannel eventchannel1 = null;
            TypedEventChannel eventchannel2 = null;
            TypedEventChannel eventchannel3 = null;
            TypedEventChannel eventchannel4 = null;
            TypedEventChannel eventchannel4_2 = null;

            println("Initialise an event channel with an invalid channel name");
            testOk = false;
            try {
                eventchannel1 = new TypedEventChannel(INV_TYPED_CHANNEL_NAME, rootCtx);
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Initialise an event channel with an uncreated channel name");
            testOk = false;
            try {
                eventchannel1= new TypedEventChannel(UNCREATED_TYPED_CHANNEL_NAME, rootCtx);
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Initialise event channels with valid channel names");
            testOk = true;
            try {
                eventchannel1 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_1_NAME, rootCtx);
            } catch (SystemException se) {
                System.out.println("Unexpected exception raised !");
                se.printStackTrace();
                testOk = false;
            }

            try {
                eventchannel2 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_2_NAME, rootCtx);
            } catch (SystemException se) {
                System.out.println("Unexpected exception raised !");
                se.printStackTrace();
                testOk = false;
            }
            
            try {
                eventchannel3 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_3_NAME, rootCtx);
            } catch (SystemException se) {
                System.out.println("Unexpected exception raised !");
                se.printStackTrace();
                testOk = false;
            }
            
            try {
                eventchannel4 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_4_NAME, rootCtx);
            } catch (SystemException se) {
                System.out.println("Unexpected exception raised !");
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
                println("Unexpected exception raise!");
            }
            check(testOk);

            println("Try to get interface on eventchannel4 again");
            testOk = false;
            try {
                eventchannel4_2 = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_4_NAME, rootCtx);
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                bp.getMessage();
                testOk = true;
            } catch(Exception e) {
                System.out.println("Unexpected exception raised !");
                e.printStackTrace();
                testOk = false;
            }
            check(testOk);

            println("Get a TypedPushConsumer on eventchannel1 with an invalid interface ref");
            testOk = false;
            try {
                obj = eventchannel1.getTypedPushConsumer(INVALID_INTERFACE_REF,null);
            } catch(org.omg.CosTypedEventChannelAdmin.InterfaceNotSupported ins) {
                System.out.println("Expected exception raised: " + ins.getMessage());
                testOk = true;
            } catch(Exception e) {
                println("Unexpected exception raised!");
                e.printStackTrace();
            }
            check(testOk);

            println("Get a TypedPushConsumer on eventchannel1");
            testOk = false;
            try {
                pushConsumer = TypedEchoHelper.narrow(
                    eventchannel1.getTypedPushConsumer(INTERFACE_REF,null));

                if (pushConsumer == null) {
                    println("Typed push consumer is nil!");
                } else {
                    testOk = true;
                }
            } catch(Exception e) {
                System.out.println("Unexpected exception raised !");
                e.printStackTrace();
                testOk = false;
            }
            check(testOk);

            println("Connect local consumer to eventchannel2 with an invalid interface");
            testOk = false;
            try {
                eventchannel2.connectTypedPushConsumer(INVALID_INTERFACE_REF, 
                                                        pcImpl._this(orb));
                testOk = true;
            } catch(org.omg.CosTypedEventChannelAdmin.NoSuchImplementation nsi) {
                System.out.println("Expected exception raised: "+nsi.getMessage());
                testOk = true;
            } catch(Exception e) {
                System.out.println("Unexpected exception raised !");
                e.printStackTrace();
                testOk = false;
            }
            check(testOk);

            println("Connect local consumer to eventchannel2");
            testOk = false;
            try {
                eventchannel2.connectTypedPushConsumer(INTERFACE_REF, pcImpl._this(orb));
                testOk = true;
            } catch(Exception e) {
                System.out.println("Unexpected exception raised !");
                e.printStackTrace();
                testOk = false;
            }
            check(testOk);

            println("Connect an other local consumer to eventchannel3");
            testOk = false;
            try {
                eventchannel3.connectTypedPushConsumer(INTERFACE_REF, pcImpl2._this(orb));
                testOk = true;
            } catch(Exception e) {
                System.out.println("Unexpected exception raised !");
                e.printStackTrace();
                testOk = false;
            }
            check(testOk);

            println("Connect eventchannel3 to eventchannel1");
            testOk = false;
            try {
                eventchannel3.connectToChannel(INTERFACE_REF, COMPLETE_TYPED_CHANNEL_1_NAME);
                testOk = true;
            } catch(Exception e) {
                System.out.println("Unexpected exception raised !");
                e.printStackTrace();
                testOk = false;
            }
            check(testOk);

            // Get a reference to the POA manager.
            org.omg.PortableServer.POAManager manager 
                = rootPOA.the_POAManager();

            manager.activate();

            println("Send messages to eventchannel1");
            testOk = true;
            for (int i=0; i<NB_MSG; i++) {
                println("Send <" + MSG_ARRAY[i] + "> to eventchannel1");
                try {
                   pushConsumer.echo_string(MSG_ARRAY[i]);
                } catch (Exception e) {
                    testOk = false;
                }
            }

            // wait for received msg
            OS.sleep(getTimescale() * 2000);
            int loopIndex = 0;
            while (pcImpl.nbRcvdMsg != NB_MSG) {
                OS.sleep(getTimescale() * 200);
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
            System.out.println("ERROR: " );
            e.printStackTrace();
        }
        
    }

}


