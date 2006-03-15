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


package cdmw.ccm.configurator.test;

import cdmw.xmlutils.XMLHelper;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ORBSupport;

import com.thalesgroup.HelloModule.HelloWorld;

public class ServerThread extends Thread {

    public static final int POA_THREAD_POOL_SIZE = 10;

    private org.omg.CORBA.ORB orb = null;

    public ServerThread() {}

    public void run() {

        try {
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            String[] orbArgs = {"-ORBconfig", "./obtest.conf"};
            orb = ORBSupport.ORBInit(orbArgs, orbStrategies);

            System.out.println("Registering valuetypes factories...");
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                com.thalesgroup.HelloModule.Value1Helper.id(),
                    new Value1FactoryImpl());

            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));

            HelloWorldImpl helloImpl = new HelloWorldImpl(null);
            HelloWorld hello = helloImpl._this(orb);

            System.out.println("Writing IOR file...");
            String ref = orb.object_to_string(hello);
            String refFile = "./Hello.ior";
            java.io.FileOutputStream file =
                new java.io.FileOutputStream(refFile);
            java.io.PrintWriter out = new java.io.PrintWriter(file);
            out.println(ref);
            out.flush();
            file.close();

            rootPOA.the_POAManager().activate();
            System.out.println("Starting ORB event loop...");
            orb.run();

        } catch(Exception e) {
            System.out.println("Server exception: ");
            e.printStackTrace();
            if (orb != null) {
                orb.shutdown(false);
                orb.destroy();
            }
        }

    }

    public void destroy() {
        orb.shutdown(false);
        orb.destroy();
    }

}

