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

import cdmw.testutils.Testable;
import cdmw.commonsvcs.naming.NamingInterface;

import org.omg.CosNaming.NamingContext;
import org.omg.CosNaming.NameComponent;

import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosNaming.NamingContextPackage.InvalidName;
import org.omg.CosNaming.NamingContextPackage.AlreadyBound;
import org.omg.CosNaming.NamingContextPackage.NotEmpty;
import org.omg.CosNaming.NamingContextPackage.NotFoundReason;


public class ResolveTest extends Testable {

    private NamingContext context;
    private TestObject testObj;

    public ResolveTest(NamingContext nc, TestObject to) {
        this.out = System.out;
        this.name = "ResolveTest";
        this.context = nc;
        this.testObj = to;
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(4);

        NamingInterface ni0 = new NamingInterface(this.context);

        // This tests NamingInterface class
        //-------------------------------------------------
        // Testing resolve operation
        //-------------------------------------------------

        println("Testing NamingInterface#resolve operation");
        String input = "test.resolve";

        //
        // Try to resolve a binding that doesn't exist
        //
        println("Test resolve a binding that doesn't exist");
        try {
            org.omg.CORBA.Object obj = ni0.resolve(input);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.missing_node) );
        } catch (Exception e) {
            fail();
        }

        //
        // Try to resolve a binding when an intermediate context doesn't exist
        //
        println("Test resolve a binding when an intermediate context doesn't exist");
        input = "test.resolve/no.node/name";
        try {
            org.omg.CORBA.Object obj = ni0.resolve(input);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.missing_node) );
        } catch (Exception e) {
            fail();
        }

        //
        // Creating correct binding
        //     test.resolve/name
        try {
            NameComponent[] name = NamingInterface.toName("test.resolve");
            NamingContext nc = this.context.bind_new_context(name);
            name = NamingInterface.toName("test.resolve/name");
            this.context.bind(name, testObj);
        } catch (Exception e) {
            System.out.println("Failed to create initial binding test.resolve/name.");
            fail();
        }

        //
        // Test resolve
        //
        println("Test resolve");
        input = "test.resolve/name";
        try {
            org.omg.CORBA.Object obj = ni0.resolve(input);
            check( obj!=null );
        } catch (Exception e) {
            fail();
        }

        //
        // Try to resolve a binding when an object is in the path
        //
        println("Test resolve a binding when an object is in the path");
        input = "test.resolve/name/no.name";
        try {
            org.omg.CORBA.Object obj = ni0.resolve(input);
            fail();
        } catch (NotFound nf) {
            check(nf.why.equals(NotFoundReason.not_context));
        } catch (Exception e) {
            fail();
        }

    }

}

