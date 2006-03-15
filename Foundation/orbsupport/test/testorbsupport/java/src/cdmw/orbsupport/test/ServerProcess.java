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


package cdmw.orbsupport.test;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.PolicyList;


public class ServerProcess {

    private static final int CDMW_PROCESS_PORT = 42536;
    private static final int CDMW_LOCALISATION_PORT = 42536;
    
    // Multi-threaded server is on rootPOA. Its POAManager use CDMW_LOCALISATION_PORT.
    public static final int SERVER_MT_PORT = CDMW_LOCALISATION_PORT;
    
    // Single-threaded server is on CdmwRootPOA. Its POAManager use CDMW_PROCESS_PORT.
    public static final int SERVER_ST_PORT = CDMW_PROCESS_PORT;
    
    public static final String SERVER_MT_CORBALOC_NAME = "ServerMT";
    public static final String SERVER_ST_CORBALOC_NAME = "ServerST";
 

   
    private void writeToFile(String ior,String filename) {

        java.io.File file = new java.io.File(filename);
        String s;
        try {
            java.io.PrintWriter out = new java.io.PrintWriter(
                new java.io.BufferedWriter( new java.io.FileWriter(file) ) );
            out.println(ior);
            	out.close();
        } catch(java.io.IOException e) {}

    }

    public ServerProcess() {
        // nothing to do
    }

    public void start(String[] args) throws org.omg.CORBA.UserException {
        // We initialise the ORB, with a thread pool of 5
        StrategyList strategyList = new StrategyList();
        strategyList.addORBThreaded();
        try {
            strategyList.addPOAThreadPool(5);
        } catch(cdmw.common.BadParameterException bpe) {}
        // force CdmwProcessPort and CdmwLocalisationService
        String[] args2 = new String[args.length + 2];
        for (int i = 0; i < args.length; i++) {
            args2[i] = args[i];
        }

        String sForceDiffProcPort = System.getProperty("force.diff.process.port");
        boolean forceDiff = new Boolean(sForceDiffProcPort).booleanValue();

        args2[args.length] = "--CdmwProcessPort=" + CDMW_PROCESS_PORT;
        args2[args.length + 1] = "--CdmwLocalisationService=" +
                                    (CDMW_LOCALISATION_PORT + (forceDiff ? 1 : 0));
       
        // ORB init
        org.omg.CORBA.ORB orb = ORBSupport.ORBInit(args2, strategyList, "");
	
	
        // And activate the root POA
        org.omg.PortableServer.POA rootPOA =
            org.omg.PortableServer.POAHelper.narrow(
                orb.resolve_initial_references("RootPOA"));
        org.omg.PortableServer.POAManager poaMgr = rootPOA.the_POAManager();
	
        // We create the servant server on the RootPOA
        ORBSupportServerImpl orbSupportServerImpl = new ORBSupportServerImpl();
        ORBSupportServer orbSupportServer = orbSupportServerImpl._this(orb);

        // bind single-threaded object to corbaloc
        ORBSupport.bindObjectToCorbaloc(orb, SERVER_MT_CORBALOC_NAME, orbSupportServer);

        // We write its ior in a file
        writeToFile( orb.object_to_string( orbSupportServer ),
            "SupportServerMT.ior" );
        // Now we create a POA mono-threaded (with a thread pool of 1)
        StrategyList strategyListSingleThread = new StrategyList();
        try {
            strategyListSingleThread.addPOAThreadPool(1);
        } catch(cdmw.common.BadParameterException bpe) {}

        PolicyList policies = new PolicyList();

        // Give it the CDMW_ROOT_POA_NAME. thus, its POAManager
        // will use the CdmwProcessPort
        org.omg.PortableServer.POA poaSingleThread
            = ORBSupport.createPOA(rootPOA,
                                   ORBSupport.CDMW_ROOT_POA_NAME,
                                    null,
                                    policies,
                                    strategyListSingleThread);
	
        // Construct instance giving orb & corbaloc, to use the remove_binding()    
	    ORBSupportServerImpl orbSsiSingleThread = new ORBSupportServerImpl(orb, SERVER_ST_CORBALOC_NAME);
        byte[] id = poaSingleThread.activate_object(orbSsiSingleThread);
        org.omg.CORBA.Object obj = poaSingleThread.id_to_reference(id);
	// We write its ior in a file
        writeToFile( orb.object_to_string( obj ), "SupportServerST.ior" );
       // bind single-threaded object to corbaloc
        ORBSupport.bindObjectToCorbaloc(orb, SERVER_ST_CORBALOC_NAME, obj);
               
        // Ok, we can start the ORB
        poaMgr.activate();
	poaSingleThread.the_POAManager().activate();
	orb.run();


    }

}

