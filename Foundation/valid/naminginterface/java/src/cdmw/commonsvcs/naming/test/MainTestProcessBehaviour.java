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


package cdmw.commonsvcs.naming.test;

import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;

import cdmw.platformmngt.PlatformInterface;
import cdmw.platformmngt.ProcessBehaviour;
import cdmw.platformmngt.ServiceNames;

import com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun;

/**
 * An implmentation of the abstract process behaviour.
 *
 */
public class MainTestProcessBehaviour extends ProcessBehaviour {

    /**
     * The ORB.
     */
    private org.omg.CORBA.ORB orb;
    
    /**
     * The constructor.
     *
     */ 
    public MainTestProcessBehaviour(org.omg.CORBA.ORB orb) {
        this.orb = orb;
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
     * operation
     */
    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {
        // example of using the PlatformInterface for notifying a message
        try {
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                PlatformInterface.getProcessName(), "Initialisation requested");
        } catch(BadOrderException boe) {
        } catch(BadParameterException bpe) {}
    }


    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
     * operation
     */
    public void run() throws NotReadyToRun {
        
        try {
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                PlatformInterface.getProcessName(), "Run requested");
        } catch(BadOrderException boe) {
        } catch(BadParameterException bpe) {}
        
        // The naming context used for the tests
        org.omg.CosNaming.NamingContext nc;
        
        // Resolve the Root POA
        org.omg.PortableServer.POA root = null;
        org.omg.CORBA.Object obj = null;
        try {
            obj = orb.resolve_initial_references("RootPOA");
            root = org.omg.PortableServer.POAHelper.narrow(obj);
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.out.println("Can't resolve `RootPOA'");
            throw new NotReadyToRun();
        }
        
        // Get naming service
        try {
            obj = orb.resolve_initial_references("NameService");
        } catch(org.omg.CORBA.ORBPackage.InvalidName in) {
            System.out.println("Can't resolve `NameService'");
            return;
        }
        if (obj == null) {
            System.out.println("`NameService' is a nil object reference");
            return;
        }

        nc = org.omg.CosNaming.NamingContextHelper.narrow(obj);
        if (nc == null) {
            System.out.println("`NameService' is not a NamingContext object reference");
            return;
        }
        
        // Create TestObject implementation
        System.out.println("Creating TestObject...");
        TestObjectImpl test = new TestObjectImpl(root);
        System.out.println("Getting test ref...");
        TestObject testRef = test._this(orb);

        // Testing ResolveTest
        ResolveTest resolveTest = new ResolveTest(nc, testRef);
        resolveTest.start();

        // Testing BindContextTest
        BindContextTest bindContextTest = new BindContextTest(nc, testRef);
        bindContextTest.start();

        // Testing BindTest
        BindTest bindTest = new BindTest(nc, testRef);
        bindTest.start();

        // Testing UnbindTest
        UnbindTest unbindTest = new UnbindTest(nc, testRef);
        unbindTest.start();

        // Testing ListTest
        ListTest listTest = new ListTest(nc, testRef);
        listTest.start();

        // Testing TestNamingInterfaceDestroy
        DestroyTest destroyTest = new DestroyTest(nc, testRef);
        destroyTest.start();
        
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    public void stop() {
        try {
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                PlatformInterface.getProcessName(), "Stop requested");
        } catch(BadOrderException boe) {
        } catch(BadParameterException bpe) {}
        orb.shutdown(false);
    }

}

