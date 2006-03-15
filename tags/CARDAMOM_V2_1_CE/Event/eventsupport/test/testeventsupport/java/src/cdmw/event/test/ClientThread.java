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
import cdmw.common.Locations;
import cdmw.common.Options;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;
import cdmw.orbsupport.ORBUtils;

import cdmw.tools.ProcessAdmin;
import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import com.thalesgroup.CdmwPlatformMngt.ProcessDelegate;
import com.thalesgroup.CdmwPlatformMngt.ProcessDelegateHelper;

public class ClientThread extends Thread {

    private org.omg.CORBA.ORB orb;

    private String callbackIOR;

    private String nameServicePort;

    private String managerArgs;

    private static final String profileManagerService
        = "cdmw.eventadmin.EventChannelManager";

    private int status = MainTest.SUCCESS;
    
    private int timescale = 1;

    public ClientThread(org.omg.CORBA.ORB orb, String nameServicePort,
        String callbackIOR) {

        this.orb = orb;
        this.nameServicePort = nameServicePort;
          this.callbackIOR = callbackIOR;
        this.managerArgs =
            " -ORBconfig ./obtest.conf " +
            Options.CALLBACK_IOR_OPTION + "=" + callbackIOR + " " +
            Options.PROCESS_PORT_OPTION + "=15327 " +
            Options.LOCALISATION_SERVICE_OPTION + "=15328" +
            " --ProfileXMLFile=./EventChannelConf1.xml " +
            Options.PROCESS_XML_FILE_OPTION + "=./Server.xml " +
            Options.VALIDATE_XML_OPTION;
        this.timescale = Testable.getTimescale();

    }

    public int getStatus() {
        return this.status;
    }

    public void run() {

        try {
            // init main server of this test: PROC_000
            String procUrl = "corbaname::localhost:" + nameServicePort
                + "#CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_000";
            ProcessDelegate proc = ProcessDelegateHelper.narrow(orb.string_to_object(procUrl));
            cdmw.tools.ProcessAdmin admin = new ProcessAdmin(orb, proc);

            // send initialise order
            String commands = "init LAST 2\n exit \n";
            admin.run(new java.io.BufferedReader(
                new java.io.StringReader(commands)), System.out);
            OS.sleep(timescale * 5000);

            // send run order
            commands = "run\n exit \n";
            admin.run(new java.io.BufferedReader(
                new java.io.StringReader(commands)), System.out);
            OS.sleep(timescale * 3000);

            //
            // Get NamingInterface object from RepositoryInterface
            //
            NamingInterface ni = null;
            try {
                ni = RepositoryInterface.getDomainNamingInterface(
                    Locations.CDMW_SERVICES_NAME_DOMAIN);
            } catch(com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
                nnd.printStackTrace();
            } catch(com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
                in.printStackTrace();
            }

            // I) TEST UNTYPED EVENT CHANNEL FACTORY
            testUntypedFactory(ni);

            OS.sleep(timescale * 5000);

            // II) TEST TYPED EVENT CHANNEL FACTORY
            testTypedFactory(ni);

        } catch(Exception e) {
            e.printStackTrace();
            status = MainTest.FAILURE;
        }

    }

    public void testUntypedFactory(NamingInterface ni)
        throws Exception {

        //
        // Get a reference on the EventChannelFactory
        //
        org.omg.CORBA.Object obj
            = ni.resolve("EVENT/FACTORIES/DefaultEventChannelFactory");
        com.thalesgroup.CdmwEvent.EventChannelFactory factory
            = com.thalesgroup.CdmwEvent.EventChannelFactoryHelper.narrow(obj);

        if (factory==null) {
            System.out.println("defaultEventChannelFactory is a nil object reference");
            return;
        }

        // Testing TestEventChannelFactory
        EventChannelFactoryTest factoryTest
            = new EventChannelFactoryTest(factory);
        factoryTest.getHeader();
        factoryTest.startTests();

        // Starting the EventChannelManager
        String url = "corbaname::localhost:" + nameServicePort
            + "#CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_001";
        startEventChannelManager(url);

        // Testing TestEventChannelFactory continue
        factoryTest.nextTest();

        // Starting Eventservice
        ThreadedProcess eventServer
            = ORBUtils.startEventService("-ORBconfig ./obtest.conf");
        OS.sleep(timescale * 10000);
        System.out.println("Event service started");

        // Testing TestEventChannelFactory ending
        factoryTest.nextTest();

        if (orb!=null) {
            try {
                //OS.killProcess(profileManager);
                stopEventChannelManager(url);
                OS.killProcess(eventServer);
            } catch(Exception e) {
                e.printStackTrace();
            }
        }

        factoryTest.getSummary();

    }


    public void testTypedFactory(NamingInterface ni)
        throws Exception {

        //
        // Get a reference on the TypedEventChannelFactory
        //
        org.omg.CORBA.Object obj
            = ni.resolve("EVENT/FACTORIES/DefaultTypedEventChannelFactory");
        com.thalesgroup.CdmwEvent.TypedEventChannelFactory typedFactory
            = com.thalesgroup.CdmwEvent.TypedEventChannelFactoryHelper.narrow(obj);

        if (typedFactory == null) {
            System.out.println("defaultTypedEventChannelFactory is a nil object reference");
            return;
        }

        // Testing TypedEventChannelFactory
        TypedEventChannelFactoryTest typedFactoryTest
            = new TypedEventChannelFactoryTest(typedFactory);
        typedFactoryTest.getHeader();
        typedFactoryTest.startTests();

        // Starting the EventChannelManager
        String url = "corbaname::localhost:" + nameServicePort
            + "#CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_002";
        startEventChannelManager(url);

        // Testing TypedEventChannelFactory continue
        typedFactoryTest.nextTest();

        // Starting typed Eventservice
        // Launch irserv needed by eventserv for typed event
        //ThreadedProcess irServer
        //    = OS.createProcess("irserv -ORBconfig ./obtest.conf -I../../../../../export/idl ../../idl/Echo.idl");
        ThreadedProcess eventServer
            = ORBUtils.startEventService("-t -ORBconfig ./obtest.conf");
        OS.sleep(timescale * 10000);
        System.out.println("Event service started");

        // Testing TestEventChannelFactory ending
        typedFactoryTest.nextTest();

        if (orb!=null) {
            try {
                //OS.killProcess(profileManager);
                stopEventChannelManager(url);
                OS.killProcess(eventServer);
                //OS.killProcess(irServer);
            } catch(Exception ex) {
                ex.printStackTrace();
            }
        }

        typedFactoryTest.getSummary();

    }

    public void startEventChannelManager(String url) throws Exception {

        ThreadedProcess profileManager = OS.createJavaProcess(
            profileManagerService + managerArgs, Testable.getProperties());
        System.out.println("Launch EventChannelManager " + managerArgs);
        OS.sleep(timescale * 10000);
        System.out.println("EventChannelManager started");

        ProcessDelegate proc = ProcessDelegateHelper.narrow(orb.string_to_object(url));
        ProcessAdmin admin = new ProcessAdmin(orb, proc);

        // send initialise order
        String commands = "init LAST 2\n exit \n";
        admin.run(new java.io.BufferedReader(
            new java.io.StringReader(commands)), System.out);
        OS.sleep(timescale * 5000);

        // send run order
        commands = "run\n exit \n";
        admin.run(new java.io.BufferedReader(
            new java.io.StringReader(commands)), System.out);
        OS.sleep(timescale * 3000);

    }

    public void stopEventChannelManager(String url) throws Exception {

        ProcessDelegate proc = ProcessDelegateHelper.narrow(orb.string_to_object(url));
        ProcessAdmin admin = new ProcessAdmin(orb, proc);

        // send stop order
        String commands = "stop\n exit \n";
        admin.run(new java.io.BufferedReader(
            new java.io.StringReader(commands)), System.out);

    }

}


