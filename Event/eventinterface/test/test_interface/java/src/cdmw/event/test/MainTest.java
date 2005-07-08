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
import cdmw.common.Options;
import cdmw.common.Locations;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.ORBUtils;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import cdmw.event.EventChannel;
import cdmw.event.TypedEventChannel;

import com.thalesgroup.CdmwPlatformMngt.ProcessCallback;
import com.thalesgroup.CdmwPlatformMngt.ProcessCallbackHelper;

public class MainTest {

    public static final String CHANNEL_1_NAME = "TestEventChannel1";
    public static final String CHANNEL_2_NAME = "TestEventChannel2";
    public static final String CHANNEL_3_NAME = "TestEventChannel3";
    public static final String CHANNEL_4_NAME = "TestEventChannel4";
    public static final String COMPLETE_CHANNEL_1_NAME
        = "EVENT/EVENT_CHANNELS/TestEventChannel1";
    public static final String COMPLETE_CHANNEL_2_NAME
        = "EVENT/EVENT_CHANNELS/TestEventChannel2";
    public static final String COMPLETE_CHANNEL_3_NAME
        = "EVENT/EVENT_CHANNELS/TestEventChannel3";
    public static final String COMPLETE_CHANNEL_4_NAME
        = "EVENT/EVENT_CHANNELS/TestEventChannel4";

    public static final String TYPED_CHANNEL_1_NAME
        = "TestTypedEventChannel1";
    public static final String TYPED_CHANNEL_2_NAME
        = "TestTypedEventChannel2";
    public static final String TYPED_CHANNEL_3_NAME
        = "TestTypedEventChannel3";
    public static final String TYPED_CHANNEL_4_NAME
        = "TestTypedEventChannel4";
    public static final String COMPLETE_TYPED_CHANNEL_1_NAME
        = "EVENT/EVENT_CHANNELS/TestTypedEventChannel1";
    public static final String COMPLETE_TYPED_CHANNEL_2_NAME
        = "EVENT/EVENT_CHANNELS/TestTypedEventChannel2";
    public static final String COMPLETE_TYPED_CHANNEL_3_NAME
        = "EVENT/EVENT_CHANNELS/TestTypedEventChannel3";
    public static final String COMPLETE_TYPED_CHANNEL_4_NAME
        = "EVENT/EVENT_CHANNELS/TestTypedEventChannel4";

    //public static final String INTERFACE_REF = "IDL:TypedEcho:1.0";
    public static final String INTERFACE_REF = "IDL:test.event.cdmw/TypedEcho:1.0";

    // The naming context of the current Process object within the naming
    public final static String PROC_CTXT
        = "CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_000";

    static class PushConsumerImpl extends org.omg.CosEventComm.PushConsumerPOA {

        /**
         * The ORB
         */
        private org.omg.CORBA.ORB orb;

        /**
         * My POA.
         */
        private org.omg.PortableServer.POA poa;

        /**
         * consumer on eventchannel2
         */
        private org.omg.CosEventComm.PushConsumer consumer;

        public PushConsumerImpl(
            org.omg.CORBA.ORB orb,
            org.omg.PortableServer.POA poa,
            org.omg.CosNaming.NamingContext nc) {

            this.orb = orb;
            this.poa = poa;
            try {
                EventChannel evtchannel
                    = new EventChannel(COMPLETE_CHANNEL_2_NAME, nc);
                consumer = evtchannel.getPushConsumer(null);
            } catch (Exception e) {
                System.out.println("Exception raised during PushConsumerImpl init");
                e.printStackTrace();
            }

        }

        public void push(org.omg.CORBA.Any data) 
            throws org.omg.CORBA.SystemException {
            try {
                String str = data.extract_string();
                System.out.println("Consumer1 has received: " + str);
                consumer.push(data);
            } catch(org.omg.CORBA.SystemException se) {
                System.out.println("Consumer1 has received an unknown event.");
            } catch(org.omg.CosEventComm.Disconnected d) {
                d.printStackTrace();
            }
        }

        public void disconnect_push_consumer() 
            throws org.omg.CORBA.SystemException {
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

    // ----------------------------------------------------------------------
    // EchoImpl implementation  (typed event push consumer)
    // ----------------------------------------------------------------------
    static class EchoImpl extends TypedEchoPOA {

        /**
         * The ORB
         */
        private org.omg.CORBA.ORB orb;

        /**
         * My POA.
         */
        private org.omg.PortableServer.POA poa;

        /**
         * consumer on eventchannel2
         */
        private TypedEcho consumer;

        public EchoImpl(
            org.omg.CORBA.ORB orb,
            org.omg.PortableServer.POA poa,
            org.omg.CosNaming.NamingContext nc) {

            this.orb = orb;
            this.poa = poa;
            TypedEventChannel evtchannel;
            org.omg.CORBA.Object obj;

            try {
                evtchannel = new TypedEventChannel(COMPLETE_TYPED_CHANNEL_2_NAME, nc);
                consumer = TypedEchoHelper.narrow(
                    evtchannel.getTypedPushConsumer(INTERFACE_REF,null));
            } catch (Exception e) {
                System.out.println("Exception raised during EchoImpl init");
                e.printStackTrace();
            }

        }

        public void echo_string(String str) 
            throws org.omg.CORBA.SystemException {
            System.out.println("Consumer1 has received: " + str);
            consumer.echo_string(str);
        }

        public void disconnect_push_consumer() 
            throws org.omg.CORBA.SystemException {
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

        public void push(org.omg.CORBA.Any data) 
            throws org.omg.CORBA.SystemException  {
            System.out.println("push any not implemented...");
        }

        public org.omg.PortableServer.POA _default_POA() {
            return this.poa;
        }

    }

    public static final int SUCCESS = 0;
    public static final int FAILURE = 1;

    public static final int POA_THREAD_POOL_SIZE = 5;

    public static final String FACTORY_SERVER_IOR = "FactoryServerIor";

    public static ProcessCallback getProcessCallback(org.omg.CORBA.ORB orb)
        throws org.omg.CORBA.TRANSIENT {

        org.omg.CORBA.Object obj = null;
        //
        // Get naming service
        //
        try {
            obj = orb.resolve_initial_references("NameService");
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
             System.out.println("Can't resolve `NameService'");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        if(obj==null) {
            System.out.println("`NameService' is a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        org.omg.CosNaming.NamingContextExt nc
            = org.omg.CosNaming.NamingContextExtHelper.narrow(obj);

        if (nc==null) {
            System.out.println("`NameService' is not a NamingContextExt object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        String PROCESS_CALLBACK_BINDING_NAME
            = "CDMW.I/process-callback.simulated";

        // Get reference to the ProcessCallback interface
        ProcessCallback callback = null;
        try {
            NamingInterface ni = new NamingInterface(nc);
            org.omg.CORBA.Object obj_ = ni.resolve(PROCESS_CALLBACK_BINDING_NAME);
            callback = ProcessCallbackHelper.narrow(obj_);
            if (callback==null) {
                System.out.println("Couldn't retrieve reference of the ProcessCallback");
                // TODO: minor cde to be defined
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENT,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            System.out.println("Couldn't retrieve reference of the ProcessCallback");
            // TODO: minor cde to be defined
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENT,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        return callback;
    }

    public static void initRepository(org.omg.CORBA.ORB orb) throws Exception {

        //
        // Get naming service
        //
        org.omg.CORBA.Object obj = null;
        try {
            obj = orb.resolve_initial_references("NameService");
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.out.println("Can't resolve `NameService'");
            throw in;
        }

        if (obj==null) {
            System.out.println("`NameService' is a nil object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        org.omg.CosNaming.NamingContext nc
            = org.omg.CosNaming.NamingContextHelper.narrow(obj);

        if (nc == null) {
            System.out.println("`NameService' is not a NamingContextExt object reference");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        String REPOSITORY_NAME = "CDMW.I/repository.simulated";

        // Get reference to the repository interface
        com.thalesgroup.CdmwNamingAndRepository.Repository rep = null;
        try {
            NamingInterface ni = new NamingInterface(nc);
            org.omg.CORBA.Object obj_ = ni.resolve(REPOSITORY_NAME);
            rep = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj_);
            if (rep == null)  {
                System.out.println("Couldn't retrieve reference of the Repository");
                throw new org.omg.CORBA.TRANSIENT(
                    ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch (Exception e) {
            System.out.println("Couldn't retrieve reference of the Repository");
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        try {
            RepositoryInterface.init(Locations.CDMW_SERVICES_NAME_DOMAIN, rep);
        } catch (Exception e) {
            System.out.println("Couldn't init Repository");
            throw e;
        }

    }

    public static int run(org.omg.CORBA.ORB orb) {
        ClientThread client = new ClientThread(orb);

        client.start();
        try {
            client.join();
        } catch(InterruptedException ie) {}

        return SUCCESS;
    }


    public static void main(String[] args) {

        System.out.println("===================================================");
        System.out.println("Starting test EventInterfaceTest");
        System.out.println("===================================================");

        int status = SUCCESS;
        int timescale = Testable.getTimescale();
        org.omg.CORBA.ORB orb = null;
        org.omg.CORBA.Object obj = null;

        ThreadedProcess profileManager = null;
        ThreadedProcess eventServer = null;
        ThreadedProcess eventAdmin = null;
        //ThreadedProcess irServ = null;
        ThreadedProcess typedEventServer = null;

        String callbackIOR
            = OS.getOptionValue( args, Options.CALLBACK_IOR_OPTION);

        boolean unitTests = true;
        // ---- Is it launched by platform management for integration tests? ----
        if ( !callbackIOR.equals("no") && !callbackIOR.equals("yes") ) {
            unitTests = false;
        }

        try {
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
			String nameServicePort = "5010";
            /*
            String[] serverArgs = {ORBUtils.NAMING_PORT_OPTION + "corbaloc::localhost:" 
                      + nameServicePort + "/NameService"};
            */
            String s = ORBUtils.NAMING_PORT_OPTION + "corbaloc::localhost:" 
                       + nameServicePort + "/NameService";
            String[] serverArgs = s.split(" ");
            orb = ORBSupport.ORBInit(serverArgs, orbStrategies);

            if (unitTests) {
                // Starting the simrepository
                cdmw.tools.SimuUtils.startSimu(System.out);
                System.out.println("Simrepository started");
                OS.sleep(timescale * 10000);

                // init repository
                initRepository(orb);
                System.out.println("Simrepository initialised");

                // Getting ProcessCallback IOR
                callbackIOR = orb.object_to_string(getProcessCallback(orb));
            }

            // Starting the EventChannelManager
            String profileManagerService = "cdmw_event_channel_manager";
            String managerArgs =
                " " +
                " " + Options.CALLBACK_IOR_OPTION + "=" + callbackIOR +
                " " + Options.PROCESS_PORT_OPTION + "=15327" +
                " " + Options.LOCALISATION_SERVICE_OPTION + "=15328" +
                " " + Options.PROCESS_XML_FILE_OPTION + "=Server.xml" +
                " --ProfileXMLFile=EventChannelConf1.xml" +
                " " + Options.VALIDATE_XML_OPTION;

            System.out.println("Starting EventChannelManager with the following parameters: ");
            System.out.println(managerArgs);
            profileManager = OS.createProcess(
                profileManagerService + managerArgs);
            //ProcessThread pt = new ProcessThread(managerArgs);
            //pt.start();
            OS.sleep(timescale * 20000);
            System.out.println("EventChannelManager started");

            String commands;
            cdmw.tools.ProcessAdmin admin = null;
            if (unitTests) {
                obj = orb.string_to_object("corbaname::localhost:5010#" + PROC_CTXT);
                com.thalesgroup.CdmwPlatformMngt.Process proc
                    = com.thalesgroup.CdmwPlatformMngt.ProcessHelper.narrow(obj);
                admin = new cdmw.tools.ProcessAdmin(orb, proc);

                // send initialise order
                commands = "verbose \n init LAST 2\n exit \n";
                admin.run(new java.io.BufferedReader(
                    new java.io.StringReader(commands)), System.out);

                // send run order
                commands = "verbose \n run\n exit \n";
                admin.run(new java.io.BufferedReader(
                    new java.io.StringReader(commands)), System.out);
            }

           /* // Starting Eventservice
            System.out.println("Starting EventService on port 21410...");
            eventServer = ORBUtils.startEventService("-OAport 21410 -ORBconfig ./obtest.conf");
            OS.sleep(timescale * 10000);
            System.out.println("EventService started");

            // Starting irserv (needed by typed event service)
            //System.out.println("Starting irserv on port 21412...");
            //irServ = OS.createProcess("irserv -ORBconfig ./obtest.conf -I../../../../../../export/idl ../../idl/Echo.idl");
            //OS.sleep(timescale * 10000);
            //System.out.println("irserv started");

            // Starting Eventservice
            System.out.println("Starting TypedEventService on port 21420...");
            typedEventServer = ORBUtils.startEventService("-t -OAport 21420 -ORBconfig ./obtest.conf");
            OS.sleep(timescale * 10000);
            System.out.println("TypedEventService started");*/

            // Starting the cdmw_event_admin which creates event channels
            // This process stops itself
            String adminArgs =
                " " + ORBUtils.NAMING_PORT_OPTION_SLAVE + "corbaloc::localhost:5020/NameService" +
                " --commandFile-name=ec_creation.cmd";
            System.out.println("Starting EventServiceAdmin with the following parameters: ");
            System.out.println(adminArgs);
            eventAdmin = OS.createProcess(
                "cdmw_event_admin" + adminArgs);
            OS.sleep(timescale * 10000);

            //
            // Get the root POA
            //
            obj = orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA
                = org.omg.PortableServer.POAHelper.narrow(obj);

            //
            // Run implementation (Activate the POA manager)
            //
            rootPOA.the_POAManager().activate();

            //
            // Get NamingInterface object from RepositoryInterface
            //
            NamingInterface ni = RepositoryInterface.getDomainNamingInterface(
                Locations.CDMW_SERVICES_NAME_DOMAIN);

            // Create consumer
            PushConsumerImpl pcImpl
                = new PushConsumerImpl(orb, rootPOA, ni.getNamingContext());

            System.out.println("Initialise eventchannel1");
            EventChannel eventchannel1 = new EventChannel(
                COMPLETE_CHANNEL_1_NAME, ni.getNamingContext() );

            System.out.println("Connect to eventchannel1");
            try {
                eventchannel1.connectPushConsumer(pcImpl._this(orb));
            } catch (Exception e) {
                System.out.println("Unexpected exception raised!");
                throw e;
            }

        /*    // Create consumer
            EchoImpl typedpcImpl = new EchoImpl(orb, rootPOA, ni.getNamingContext());

            System.out.println("Initialise typedeventchannel1");
            TypedEventChannel typedeventchannel1 = new TypedEventChannel(
                COMPLETE_TYPED_CHANNEL_1_NAME, ni.getNamingContext() );

            System.out.println("Connect to typedeventchannel1");
            try {
                typedeventchannel1.connectTypedPushConsumer(INTERFACE_REF,
                    typedpcImpl._this(orb));
            } catch(Exception e) {
                System.out.println("Unexpected exception raised!");
                throw e;
            }*/

            OS.sleep(timescale * 1000);
            System.out.println("server initialised");

            status = run(orb);

            if (unitTests) {
                // send stop order
                commands = "verbose \n stop\n exit \n";
                admin.run(new java.io.BufferedReader(
                    new java.io.StringReader(commands)), System.out);
            }
            OS.sleep(timescale * 2000);
        } catch(Exception e) {
            e.printStackTrace();
            status = FAILURE;
        }

        if (orb!=null) {
            try {
                OS.killProcess(profileManager);
                OS.killProcess(eventServer);
                OS.killProcess(typedEventServer);
                //OS.killProcess(irServ);
            } catch(Exception e) {
                e.printStackTrace();
            }

            try {
                orb.destroy();
            } catch(Exception e) {
                e.printStackTrace();
                status = FAILURE;
            }
        }

        // And we kill launched process
        // for the simulated repository, launch process is a script shell
        // which ended when process are launched: no kill of this process
        // to stop sim repository, the stop_simu.sh script shall be called
        if (unitTests) {
            try {
                cdmw.tools.SimuUtils.stopSimu(System.out);
                OS.sleep(timescale * 2000);
            } catch (InterruptedException ie) {
                ie.printStackTrace();
                status = FAILURE;
            }
        }

        System.out.println("===================================================");
        System.out.println("Ending test EventInterfaceTest");
        System.out.println("===================================================");

        System.out.println("Summary:");
        if (status == SUCCESS) {
            System.out.println("   Main test successful.");
        } else {
            System.out.println("   Main test failed.");
        }


    }
    
    /*
    private static class ProcessThread extends Thread {
        
        private String[] args;
        
        public ProcessThread(String argsLine) {
            java.util.StringTokenizer st = new java.util.StringTokenizer(argsLine);
            this.args = new String[st.countTokens()];
            for (int i = 0; st.hasMoreTokens(); i++) {
                this.args[i] = (String) st.nextToken();
            }
        }
        
        public void run() {
            cdmw.eventadmin.EventChannelManager.main(args);
        }
        
    }
    */

}


