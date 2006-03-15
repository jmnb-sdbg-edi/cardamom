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


public class BindTest extends Testable {

    private NamingContext context;
    private TestObject testObj;

    public BindTest(NamingContext nc,TestObject to) {
        this.out = System.out;
        this.name = "BindTest";
        this.context = nc;
        this.testObj = to;
    }

    public void doTests() {
        testBind();
        testRebind();
    }

    private void testBind() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(16);

        NamingInterface ni0 = new NamingInterface(context);
        //-------------------------------------------------
        // Testing bind operation
        //-------------------------------------------------
        println("Testing NamingInterface#bind operation");

        // This test creates the following bindings in the naming service:
        //     cdmw.test/archive.3/orbacus.mk

        //
        // Try to bind an object when an intermediate context doesn't exist
        //
        String input = "cdmw.test/archive.3/orbacus.mk";
        println("Attempt to bind "+ input);
        try {
            ni0.bind(input, testObj, false);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.missing_node) );
        } catch (Exception e) {
            fail();
        }

        // Forcing binding
        println("Forcing bind of " + input);
        try {
            ni0.bind(input, testObj, true);
            org.omg.CORBA.Object obj = ni0.resolve(input);
            check(obj!=null);
        } catch (Exception e) {
            fail();
        }

        //
        // Try to bind an object when an object is in the path
        //
        input = "cdmw.test/archive.3/orbacus.mk/bla.bla";
        println("Attempt to bind " + input);
        try {
            ni0.bind(input, testObj, false);
            fail();
        } catch (NotFound nf) {
            succeed();
        } catch (Exception e) {
            fail();
        }

        // Forcing Binding
        println("Forcing bind of " + input);
        try {
            ni0.bind(input, testObj, true);
            fail();
        } catch (NotFound nf) {
            succeed();
        } catch (Exception e) {
            fail();
        }

        //
        // Try to bind an object using the name of an existing context
        //
        input = "cdmw.test/archive.3";
        println("Attempt to bind " + input);
        try {
            ni0.bind(input, testObj, false);
            fail();
        } catch (AlreadyBound ab ) {
            succeed();
        } catch (Exception e) {
            fail();
        }

        // Forcing bind
        println("Forcing bind of " + input);
        try {
            ni0.bind(input, testObj, true);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_object) );
        } catch (Exception e) {
            fail();
        }

        //
        // Try to bind an object using the name of an existing object
        //
        input = "cdmw.test/archive.3/orbacus.mk";
        println("Attempt to bind " + input);
        try {
            ni0.bind(input, testObj, false);
            fail();
        } catch (AlreadyBound ab) {
            succeed();
        } catch (Exception e) {
            fail();
        }

        // Forcing bind
        println("Forcing bind of " + input);
        try {
            ni0.bind(input, testObj, true);
            succeed();
        } catch (Exception e) {
            fail();
        }

    }

    private void testRebind() {

        NamingInterface ni0 = new NamingInterface(context);
        //-------------------------------------------------
        // Testing rebind operation
        //-------------------------------------------------
        println("Testing NamingInterface#rebind operation");

        // This test creates the following bindings in the naming service:
        //     cdmw.test/archive.4/orbacus.mk

        //
        // Try to rebind an object when an intermediate context doesn't exist
        //
        String input = "cdmw.test/archive.4/orbacus.mk";
        println("Attempt to rebind " + input);
        try {
            ni0.rebind(input, testObj, false);
            fail();
        } catch(NotFound nf) {
            check( nf.why.equals(NotFoundReason.missing_node) );
        } catch(Exception e) {
            fail();
        }

        // Forcing binding
        println("Forcing rebind of " + input);
        try {
            ni0.rebind(input, testObj, true);
            org.omg.CORBA.Object obj = ni0.resolve(input);
            check( obj != null );
        } catch(Exception e) {
            fail();
        }

        //
        // Try to rebind an object when an object is in the path
        //
        input = "cdmw.test/archive.4/orbacus.mk/bla.bla";
        println("Attempt to rebind " + input);
        try {
            ni0.rebind(input, testObj, false);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_context) );
        } catch(Exception e) {
            fail();
        }

        // Forcing Rebind
        println("Forcing rebind of " + input);
        try {
            ni0.rebind(input, testObj, true);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_context) );
        } catch(Exception e) {
            fail();
        }

        //
        // Try to rebind an object using the name of an existing context
        //
        input = "cdmw.test/archive.4";
        println("Attempt to rebind " + input);
        try {
            ni0.rebind(input, testObj, false);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_object) );
        } catch(Exception e) {
            fail();
        }

        // Forcing rebind
        println("Forcing rebind of " + input);
        try {
            ni0.rebind(input, testObj, true);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_object) );
        } catch (Exception e) {
            fail();
        }

        //
        // Try to rebind an object using the name of an existing object
        //
        input = "cdmw.test/archive.4/orbacus.mk";
        println("Attempt to rebind " + input);
        try {
            ni0.rebind(input, testObj, false);
            succeed();
        } catch (Exception e) {
            fail();
        }

        // Forcing rebind
        println("Forcing rebind of " + input);
        try {
            ni0.rebind(input, testObj, true);
            succeed();
        } catch (Exception e) {
            fail();
        }

    }

}

