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


package cdmw.lifecycle.test;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.ORBUtils;

import java.io.File;
import java.io.IOException;

import org.omg.CORBA.ORB;
import org.omg.CORBA.ORBPackage.InvalidName;

import org.omg.PortableServer.POA;
import org.omg.PortableServer.POAHelper;
import org.omg.PortableServer.POAManager;
import org.omg.PortableServer.POAManagerPackage.AdapterInactive;

import org.omg.CosPropertyService.PropertySetDefFactory;
import org.omg.CosPropertyService.PropertySetDefFactoryHelper;

public class MainTest {

    public final static int SUCCESS = 0;
    public final static int FAILURE = 1;

    public static int run(ORB orb, String[] args)
        throws AdapterInactive,InvalidName {

        //
        // Resolve the Root POA
        //
        org.omg.CORBA.Object obj = orb.resolve_initial_references("RootPOA");
        POA root = POAHelper.narrow(obj);
        POAManager poaManager = root.the_POAManager();

        //
        // Initialize the server so that incoming requests are
        // handled from this point on
        //
        poaManager.activate();

        //
        // Get property service
        //
        try {
            obj = orb.resolve_initial_references("PropertyService");
        } catch(InvalidName in) {
            System.out.println("Can't resolve `PropertyService'");
            return FAILURE;
        }

        if (obj==null) {
            System.out.println("`PropertyService' is a nil object reference");
            return FAILURE;
        }

        PropertySetDefFactory factory
            = PropertySetDefFactoryHelper.unchecked_narrow(obj);

        if(factory==null) {
            System.out.println("`PropertyService' is not "
                + "a PropertyDefSetFactory object reference");
            return FAILURE;
        }

        //Testing TestNamingInterfaceResolve
        EntityCleanupNotifierTest notifierTest
            = new EntityCleanupNotifierTest(factory,orb);
        notifierTest.start();

        return SUCCESS;

    }

    public static void main(String[] args) {

        // We allow the user to specify a different port number
        // for the Name Service
        String option = "--CdmwPropertyPort";
        String portNumber = OS.getOptionValue(args,option) ;

        if ( portNumber.equals("no") ) {
            portNumber = "5200";
        }

        int status = SUCCESS;
        ThreadedProcess propertyp = null;

        try {
            propertyp = ORBUtils.startPropertyService(portNumber);
            OS.sleep(cdmw.testutils.Testable.getTimescale() * 3000);
        } catch(IOException ioe) {
            System.out.println(ioe.getMessage());
            status = FAILURE;
        } catch(InterruptedException ie) {
            System.out.println(ie.getMessage());
            status = FAILURE;
        }

        String[] strArgs = {
            "-ORBInitRef",
            "PropertyService=corbaloc::localhost:" +
            portNumber + "/PropertyService"
        };

        ORB orb = ORB.init(strArgs, ORBUtils.getProperties());
        try {
             status = run(orb, strArgs);
        } catch(InvalidName in) {
            in.printStackTrace();
            status = FAILURE;
        } catch(AdapterInactive ai) {
            ai.printStackTrace();
            status = FAILURE;
        }

        if(orb != null && propertyp != null) {
            // We destroy the ORB
            orb.destroy();
            // And we kill the Name Server
            OS.killProcess( propertyp );
        }

        if (status == SUCCESS) {
            System.out.println("Main test successful.");
        } else {
            System.out.println("Main test failed.");
        }

    }

}

