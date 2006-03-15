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

import cdmw.common.Options;
import cdmw.ossupport.OS;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.cdmwinit.CdmwInterface;

public class MainTest {

    public final static int SUCCESS = 0;
    public final static int FAILURE = 1;  

    public final static int POA_THREAD_POOL_SIZE = 5;   

    public final static String FACTORY_SERVER_IOR = "FactoryServerIor";
    
    public static void main(String[] args) {
        
        int status = SUCCESS;
        org.omg.CORBA.ORB orb = null;
        boolean typedEvtTest = false;

        String typedTestOption = OS.getOptionValue( args, "-t");
        if ( typedTestOption.equals("yes") ) {
           typedEvtTest = true;
        }

        String callbackIOR
            = OS.getOptionValue( args, Options.CALLBACK_IOR_OPTION);
        // ---- Is it launched by platform management for validation tests? ----
        if ( callbackIOR.equals("no") || callbackIOR.equals("yes")) {
            System.exit(1);
        }

        try {
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            orb = ORBSupport.ORBInit(args, orbStrategies);

            // Get the root POA 
            org.omg.CORBA.Object obj 
                = orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA 
                = org.omg.PortableServer.POAHelper.narrow(obj);

            // Run implementation (Activate the POA manager)
            rootPOA.the_POAManager().activate();

            TestProcessControl ctrl 
                = new TestProcessControl(orb, typedEvtTest);

            // init Cdmw
            CdmwInterface.cdmwInit(orb, args, 
                ctrl, "cdmw.event.test.ServerCdmwInterface");            

            // Run implementation
            orb.run();
            
        } catch(Exception e) {
            e.printStackTrace();
            status = FAILURE;
        }
        
        if (orb!=null) {
            try {
                CdmwInterface.cdmwCleanup(orb, 
                    "cdmw.event.test.ServerCdmwInterface");
                orb.destroy();
            } catch(Exception e) {
                e.printStackTrace();
                status = FAILURE;
            }
        }        

        if (status == SUCCESS) {
            System.out.println("Main test successful.");
        } else {
            System.out.println("Main test failed.");
        }
    }

}

