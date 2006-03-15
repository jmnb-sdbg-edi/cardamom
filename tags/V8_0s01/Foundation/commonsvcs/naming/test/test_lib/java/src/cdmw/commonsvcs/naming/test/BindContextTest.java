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
import org.omg.CosNaming.NamingContextHelper;
import org.omg.CosNaming.NameComponent;

import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosNaming.NamingContextPackage.InvalidName;
import org.omg.CosNaming.NamingContextPackage.AlreadyBound;
import org.omg.CosNaming.NamingContextPackage.NotEmpty;
import org.omg.CosNaming.NamingContextPackage.NotFoundReason;


public class BindContextTest extends Testable {

    private NamingContext context;
    private TestObject testObj;

    public BindContextTest(NamingContext nc,TestObject to) {
        this.out = System.out;
        this.name = "BindContextTest";
        this.context = nc;
        this.testObj = to;
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(24);
    	
        testBindNewContext();
        testBindContext();
        testRebindContext();
        testNewContext();
    }

    private void testBindNewContext() {

        NamingInterface ni0 = new NamingInterface(this.context);
        // This tests NamingInterface class
        //-------------------------------------------------
        // Testing bind_new_context operation
        //-------------------------------------------------
        println("Testing NamingInterface::bind_new_context operation");

        // This test creates the following bindings in the naming service:
        //     cdmw.test/runtime/config.mk
        //     cdmw.test/runtime/commonsvcs/naming/c++/test/idl
        String input = "cdmw.test";
        println("Creating cdmw.test binding");
        try {
            NamingInterface ni = ni0.bindNewContext(input,false);
            org.omg.CORBA.Object obj = ni0.resolve(input);
            check(obj!=null);
        } catch (AlreadyBound ab) {
            println("cdmw.test context should be cleared before testing.");
            fail();
        } catch (Exception e) {
            fail();
        }
        // expecting an AlreadyBound exception
        try {
            NamingInterface ni = ni0.bindNewContext(input,false);
            fail();
        } catch (AlreadyBound ab) {
            succeed();
        } catch (Exception e) {
            fail();
        }
        // This should not be OK (AlreadyBound exception expected)
        try {
            NamingInterface ni = ni0.bindNewContext(input,true);
            fail();
        } catch (AlreadyBound ab) {
            succeed();
        } catch (Exception e) {
            fail();
        }
        // cdmw.test/runtime/commonsvcs/naming/c++/test/idl
        input += "/runtime/commonsvcs/naming/c++/test/idl";
        println("Creating cdmw.test/runtime/commonsvcs/naming/c++/test/idl binding");
        try {
            NamingInterface ni = ni0.bindNewContext(input,false);
            fail();
        } catch (NotFound nf) {
            succeed();
        } catch (Exception e) {
            fail();
        }
        //
        // Try to bind a context when an intermediate context
        // doesn't exist (Forcing creation)
        //
        try {
            NamingInterface ni = ni0.bindNewContext(input,true);
            org.omg.CORBA.Object obj = ni0.resolve(input);
            check(obj!=null);
        } catch (Exception e) {
            fail();
        }
        //
        // Try to bind a context when an object is in the path
        //
        //    cdmw.test/runtime/config.mk
        println("Binding a context when an object is in the path");
        try {
            NameComponent[] name // This has already been tested
                = NamingInterface.toName("cdmw.test/runtime/config.mk");
            this.context.bind( name, testObj );
        } catch (InvalidName in) {
            println("Unexpected error " + in );
            return;
        } catch (Exception ex) {
            println("Unexpected error " + ex );
            return;
        }
        input = "cdmw.test/runtime/config.mk";
        try {
            NamingInterface ni = ni0.bindNewContext(input,true);
            fail();
        } catch (AlreadyBound ab) {
            succeed();
        } catch (Exception e) {
            fail();
        }
        input = "cdmw.test/runtime/config.mk/bla.bla";
        try {
            NamingInterface ni = ni0.bindNewContext(input,true);
            fail();
        } catch (NotFound nf) {
            succeed();
        } catch (Exception e) {
            fail();
        }

    }

    private void testBindContext() {

        NamingInterface ni0 = new NamingInterface(this.context);
        //-------------------------------------------------
        // Testing bind_context operation
        //-------------------------------------------------
        println("Testing NamingInterface#bindContext operation");

        // This test creates the following bindings in the naming service:
        //     cdmw.test/archive/orbacus.mk
        //     cdmw.test/archive/commonsvcs/naming/c++
        //        (link to cdmw.test/runtime/commonsvcs/naming/c++)

        // Get a reference to the naming context bound to
        // cdmw.test/runtime/commonsvcs/naming/c++
        NamingContext nc = null;
        try {
            // resolve has already been tested
            org.omg.CORBA.Object obj = ni0.resolve("cdmw.test/runtime/commonsvcs/naming/c++");
            nc = NamingContextHelper.narrow(obj);
            if (nc==null) {
                println("Unexpected error (test_bind_new_context failed before)");
                return;
            }
        } catch (InvalidName in) {
            println("Unexpected error " + in );
            return;
        } catch (Exception ex) {
            println("Unexpected error " + ex );
            return;
        }
        //
        // Try to bind a context when an intermediate context doesn't exist
        //
        // Expecting NamingContext::NotFound exception
        //     ex.why == NamingContext::not_context
        String input = "cdmw.test/archive/runtime/commonsvcs/naming/c++";
        println("Attempt to bind " + input);
        try {
            ni0.bindContext(input, nc, false);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.missing_node) );
        } catch (Exception e) {
            fail();
        }
        // Forcing creation
        println("Forcing bind of cdmw.test/archive/runtime/commonsvcs/naming/c++");
        try {
            ni0.bindContext(input, nc, true);
            org.omg.CORBA.Object obj = ni0.resolve(input);
            check(obj!=null);;
        } catch (Exception e) {
            fail();
        }

        //
        // Try to bind a context when an object is in the path
        //
        //    cdmw.test/archive/orbacus.mk
        println("Binding a context when an object is in the path");
        try {
            NameComponent[] name // This has already been tested
                = NamingInterface.toName("cdmw.test/archive/orbacus.mk");
            this.context.bind(name, this.testObj);
        } catch (InvalidName in) {
            println("Unexpected error " + in );
            return;
        } catch (Exception ex) {
            println("Unexpected error " + ex );
            return;
        }

        // Expecting NamingContext::NotFound exception
        //     ex.why == NamingContext::not_context
        input = "cdmw.test/archive/orbacus.mk/runtime/commonsvcs/naming/c++";
        println("Attempt to bind " + input);
        try {
            // to test bind_context with NamingInterface
            NamingInterface n = new NamingInterface(nc);
            ni0.bindContext(input, nc, false);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_context) );
        } catch (Exception e) {
            fail();
        }
        // Forcing creation (exception NamingContext::NotFound expected)
        println("Forcing bind of " + input);
        try {
            ni0.bindContext(input, nc, true);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_context) );
        } catch (Exception e) {
            fail();
        }

        //
        // Try to bind a context using the name of an existing object
        //
        input = "cdmw.test/archive/orbacus.mk";
        println("Test of binding a context using the name of an existing object");
        try {
            ni0.bindContext(input, nc, false);
            fail();
        } catch (AlreadyBound ab) {
            succeed();
        } catch (Exception e) {
            fail();
        }
        // Forcing bind (exception NamingContext::NotFound expected)
        println("Test of forcing bind of a context using the name of an existing object");
        try {
            ni0.bindContext(input, nc, true);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_context) );
        } catch (Exception e) {
            fail();
        }

        //
        // Try to bind a context using the name of an existing context
        //
        input = "cdmw.test/archive/runtime/commonsvcs/naming/c++";
        println("Test of binding a context using the name of an existing context");
        try {
            ni0.bindContext(input, nc, false);
            fail();
        } catch (AlreadyBound ab) {
            succeed();
        } catch (Exception e) {
            fail();
        }
        // Forcing bind
        println("Test of forcing bind of a context using the name of an existing context");
        try {
            ni0.bindContext(input, nc, true);
            succeed();
        } catch (Exception e) {
            fail();
        }

    }

    private void testRebindContext() {

        NamingInterface ni0 = new NamingInterface(this.context);
        //-------------------------------------------------
        // Testing rebind_context operation
        //-------------------------------------------------
        println("Testing NamingInterface::rebind_context operation");

        // This test creates the following bindings in the naming service:
        //     cdmw.test/archive.2/orbacus.mk
        //     cdmw.test/archive.2/commonsvcs/naming/c++
        //        (link to cdmw.test/runtime/commonsvcs/naming/c++)

        // Get a reference to the naming context bound to
        // cdmw.test/runtime/commonsvcs/naming/c++
        NamingContext nc;
        String input = "cdmw.test/runtime/commonsvcs/naming/c++";
        try {
            NameComponent[] name // This has already been tested
                = NamingInterface.toName(input);
            org.omg.CORBA.Object obj = this.context.resolve(name);
            nc = NamingContextHelper.narrow(obj);
            if (nc==null) {
                println("Unexpected error (test_bind_new_context failed before)");
                return;
            }
        } catch (InvalidName in) {
            println("Unexpected error " + in);
            return;
        } catch (Exception ex) {
            println("Unexpected error " + ex);
            return;
        }

        //
        // Try to rebind a context when an intermediate context doesn't exist
        //
        // Expecting NamingContext::NotFound exception
        //     ex.why == NamingContext::not_context
        input = "cdmw.test/archive.2/runtime/commonsvcs/naming/c++";
        println("Attempt to rebind " + input);
        try {
            ni0.rebindContext(input, nc, false);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.missing_node) );
        } catch (Exception e) {
            fail();
        }

        // Forcing creation
        println("Forcing rebind of " + input);
        try {
            // to test rebind_context with NamingInterface
            NamingInterface n = new NamingInterface(nc);
            ni0.rebindContext(input, nc, true);
            org.omg.CORBA.Object obj = ni0.resolve(input);
            check(obj!=null);
        } catch (Exception e) {
            fail();
        }

        //
        // Try to rebind a context when an object is in the path
        //
        //    cdmw.test/archive.2/orbacus.mk
        input = "cdmw.test/archive.2/orbacus.mk";
        println("Rebinding a context when an object is in the path");
        try {
            NameComponent[] name // This has already been tested
                = NamingInterface.toName(input);
            this.context.bind(name,testObj);
        } catch (InvalidName in) {
            println("Unexpected error " + in );
            return;
        } catch (Exception ex) {
            println("Unexpected error " + ex );
            return;
        }

        // Expecting NamingContext::NotFound exception
        //     ex.why == NamingContext::not_context
        input = "cdmw.test/archive.2/orbacus.mk/runtime/commonsvcs/naming/c++";
        println("Attempt to rebind " + input);
        try {
            ni0.rebindContext(input, nc, false);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_context) );
        } catch (Exception e) {
            fail();
        }
        // Forcing creation (exception NamingContext::NotFound expected)
        println("Forcing rebind of cdmw.test/archive/orbacus.mk/runtime/commonsvcs/naming/c++");
        try {
            ni0.rebindContext(input, nc, true);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_context) );
        } catch (Exception e) {
            fail();
        }

        //
        // Try to rebind a context using the name of an existing object
        //
        input = "cdmw.test/archive.2/orbacus.mk";
        println("Test of rebinding a context using the name of an existing object");
        try {
            ni0.rebindContext(input, nc, false);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_context) );
        } catch (Exception e) {
            fail();
        }

        // Forcing rebind (exception NamingContext::NotFound expected)
        println("Test of forcing rebind of a context using the name of an existing object");
        try {
            ni0.rebindContext(input, nc, true);
            fail();
        } catch (NotFound nf) {
            check( nf.why.equals(NotFoundReason.not_context) );
        } catch (Exception e) {
            fail();
        }

        //
        // Try to rebind a context using the name of an existing context
        //
        input = "cdmw.test/archive.2/runtime/commonsvcs/naming/c++";
        println("Test of rebinding a context using the name of an existing context");
        try {
            ni0.rebindContext(input, nc, false);
            succeed();
        } catch (Exception e) {
            fail();
        }

        // Forcing bind
        println("Test of forcing rebind of a context using the name of an existing context");
        try {
            ni0.rebindContext(input, nc, true);
            succeed();
        } catch (Exception e) {
            fail();
        }

    }

    private void testNewContext() {

        NamingInterface ni0 = new NamingInterface(this.context);
        //-------------------------------------------------
        // Testing new_context operation
        //-------------------------------------------------
        println("Testing NamingInterface::new_context operation");

        // This test creates the following bindings in the naming service:
        //     cdmw.test/runtime/commonsvcs/naming/c++/test/incl

        String input = "cdmw.test/runtime/commonsvcs/naming/c++/test";

        try {
            org.omg.CORBA.Object obj = ni0.resolve(input);
            NamingContext nc = NamingContextHelper.narrow(obj);

            if (nc == null) {
                println("Unexpected error (" + input + " not present!)");
                return;
            }
            NamingInterface ni = new NamingInterface(nc);
            ni.bindContext("incl", ni.newContext(), false);

            input += "/incl";
            obj = ni0.resolve(input);
            check( obj!=null );
        } catch(NotFound nf) {
            switch(nf.why.value()) {
                case NotFoundReason._missing_node:
                    System.out.println("Missing node.");
                    break;
                case NotFoundReason._not_context:
                    System.out.println("Not context.");
                    break;
                case NotFoundReason._not_object:
                    System.out.println("Not object.");
                    break;
                default:
                    nf.printStackTrace();
            }
            fail();
        } catch (Exception e) {
            fail();
        }

    }

}

