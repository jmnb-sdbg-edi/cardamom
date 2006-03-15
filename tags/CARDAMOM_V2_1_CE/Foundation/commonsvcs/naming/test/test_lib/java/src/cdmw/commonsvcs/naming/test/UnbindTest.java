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

import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosNaming.NamingContextPackage.InvalidName;
import org.omg.CosNaming.NamingContextPackage.AlreadyBound;
import org.omg.CosNaming.NamingContextPackage.NotEmpty;
import org.omg.CosNaming.NamingContextPackage.NotFoundReason;


public class UnbindTest extends Testable {

    private NamingContext context;
    private TestObject testObj;

    public UnbindTest(NamingContext nc,TestObject to) {
        this.out = System.out;
        this.name = "UnbindTest";
        this.context = nc;
        this.testObj = to;
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(5);

        NamingInterface ni0 = new NamingInterface(this.context);
        // This tests NamingInterface class
        //-------------------------------------------------
        // Testing unbind operation
        //-------------------------------------------------
        println("Testing NamingInterface::unbind operation");
        String input= "test.unbind";
        //
        // Try to unbind a binding that doesn't exist
        //
        println("Test unbind a binding that doesn't exist");
        try {
            ni0.unbind(input);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.missing_node) );
        } catch (Exception e) {
            fail();
        }
        //
        // Try to unbind a binding when an intermediate context doesn't exist
        //
        println("Test unbind a binding when an intermediate context doesn't exist");
        input = "test.unbind/no.node/name";
        try {
            ni0.unbind(input);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.missing_node) );
        } catch (Exception e) {
            fail();
        }
        // Creating correct binding
        //     test.resolve/name
        try {
            ni0.bind("test.unbind/name", testObj, true);
        } catch (Exception e) {
            println("Failed to create initial binding test.unbind/name.");
            fail();
            return;
        }
        //
        // Try to unbind a binding when an object is in the path
        //
        println("Test unbind a binding when an object is in the path");
        input = "test.unbind/name/no.name";
        try {
            ni0.unbind(input);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_context) );
        } catch (Exception e) {
            fail();
        }
        //
        // Test unbind
        //
        println("Test unbind");
        input = "test.unbind/name";
        try {
            ni0.unbind(input);
            succeed();
        } catch (Exception e) {
            fail();
        }
        //
        // Try to unbind the same binding again
        //
        println("Test unbind the same binding again");
        try {
            ni0.unbind(input);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.missing_node) );
        } catch (Exception e) {
            fail();
        }

    }

}

