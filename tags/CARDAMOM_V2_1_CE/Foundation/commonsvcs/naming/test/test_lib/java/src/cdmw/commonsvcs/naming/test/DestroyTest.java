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
import cdmw.common.TypeMismatchException;
import cdmw.commonsvcs.naming.NamingInterface;

import org.omg.CORBA.SystemException;

import org.omg.CosNaming.NamingContext;
import org.omg.CosNaming.NamingContextHelper;
import org.omg.CosNaming.NameComponent;
import org.omg.CosNaming.BindingListHolder;

import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosNaming.NamingContextPackage.InvalidName;
import org.omg.CosNaming.NamingContextPackage.AlreadyBound;
import org.omg.CosNaming.NamingContextPackage.NotEmpty;
import org.omg.CosNaming.NamingContextPackage.NotFoundReason;


public class DestroyTest extends Testable {

    private NamingContext context;
    private TestObject testObj;


    public DestroyTest(NamingContext nc,TestObject to) {
        this.out = System.out;
        this.name = "DestroyTest";
        this.context = nc;
        this.testObj = to;
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(10);

        NamingInterface ni0 = new NamingInterface(this.context);

        // This tests NamingInterface class
        //-------------------------------------------------
        // Testing destroy operation
        //-------------------------------------------------
        println("Testing NamingInterface#destroy operation");
        String base = "test.destroy/runtime";

        // Install test name tree
        if ( !installInitialBindings(ni0, base, testObj) ) {
            fail("Unable to install initial bindings.");
            return;
        }

        //
        // Additional tests to test resolveName()
        //
        println("Additional tests to test resolveName() function");
        String input = base + "/commonsvcs/naming/dummy/ada.1";
        // Testing NotFound Exception
        System.out.println("Testing NotFound Exception");
        try {
            NamingContext nc = resolveName(ni0,input);
            fail();
        } catch (NotFound nf) {
            succeed();
        } catch (Exception e) {
            fail();
        }
        // Testing TypeMismatch Exception
        input = base + "/bin/CVS/Root";
        System.out.println("Testing TypeMismatch Exception");
        try {
            NamingContext nc = resolveName(ni0,input);
            fail();
        } catch (TypeMismatchException tme) {
            succeed();
        } catch (Exception e) {
            fail();
        }

        //
        // Destroy commonsvcs/naming/ada.1 naming context
        //
        input = base + "/commonsvcs/naming/ada.1";
        println("Destroying an empty context");
        try {
            NamingContext nc = resolveName(ni0,input);
            NamingInterface n = new NamingInterface(nc);
            n.destroy();
            ni0.unbind(input);
            succeed();
        } catch (Exception e) {
            fail();
        }
        // Forcing destroy
        input = base + "/commonsvcs/naming/ada.2";
        println("Forcing destroy");
        try {
            NamingContext nc = resolveName(ni0,input);
            NamingInterface n = new NamingInterface(nc);
            boolean destroyed = n.destroyAll();
            check(destroyed);
            if (destroyed) ni0.unbind(input);
            succeed();
        } catch (Exception e) {
            fail();
        }

        //
        // Try to destroy a context that's not empty
        //
        println("Test of destroying a context that's not empty");
        input = base;
        NamingContext nc = null;
        try {
            nc = resolveName(ni0,input);
            NamingInterface n = new NamingInterface(nc);
            n.destroy();
            fail();
        } catch (NotEmpty ne) {
            succeed();
        } catch (Exception e) {
            fail();
        }
        // Forcing destroy
        println("Forcing destroy");
        try {
            NamingInterface n = new NamingInterface(nc);
            check(n.destroyAll());
        } catch (Exception e) {
            fail();
        }
        // Is it destroyed ?
        println("Is it destroyed ?");
        try {
            NameComponent[] name = NamingInterface.toName("toto");
            NamingContext nc_ = nc.bind_new_context(name);
            fail();
        } catch (org.omg.CORBA.OBJECT_NOT_EXIST one) {
            succeed();
            // remove binding
            try {
                ni0.unbind(input);
            } catch (Exception e) {
                // Unexpected error
                fail();
            }
        } catch (Exception e) {
            fail();
        }

        //
        // Try to destroy a context that's not empty
        // with an undestroyable context
        //
        // Install test name tree
        println("Install test name tree");
        if (!installInitialBindings(ni0, base, testObj)) {
            fail();
            return;
        }
        println("Test of destroying a context that's not empty "+
            "with an undestroyable context");
        // To simulate an undestroyable context, we destroy a context without
        // removing the binding. This will generate a system exception when its
        // destruction is requested.
        input = base + "/commonsvcs/naming/ada.1";
        try {
            nc = resolveName(ni0,input);
            NamingInterface n = new NamingInterface(nc);
            n.destroy();
        } catch (Exception e) {
            fail();
            return;
        }
        // Forcing destroy
        println("Forcing destroy");
        try {
            nc = resolveName(ni0,base);
        } catch (Exception e) {
            fail();
            return;
        }
        try {
            NamingInterface n = new NamingInterface(nc);
            check(!n.destroyAll());
        } catch (Exception e) {
            fail();
        }
        // Is it destroyed
        println("Is it destroyed ?");
        try {
            NamingInterface n = new NamingInterface(nc);
            BindingListHolder bndList = new BindingListHolder();
            n.listContext(bndList);
            check(bndList.value.length == 1);
        } catch (Exception e) {
            fail();
        }

    }

    private boolean installInitialBindings(NamingInterface n,String base,
        org.omg.CORBA.Object obj) {

        boolean result = true;
        try {
            NamingInterface ni = n.bindNewContext(base,true);

            ni.bind("bin/CVS/Entries",obj,true);
            ni.bind("bin/CVS/Repository",obj,true);
            ni.bind("bin/CVS/Root",obj,true);
            ni.bind("bin/Makefile",obj,true);
            ni.bind("bin/Makefile.mak",obj,true);
            ni.bind("bin/Makefile.tmpl",obj,true);
            ni.bind("bin/mk_idl_include.sh",obj,true);
            ni.bind("bin/CdmwCreateClass",obj,true);
            ni.bind("bin/CdmwCreateCSC",obj,true);
            ni.bind("commonsvcs/naming/c++/CVS/Entries",obj,true);
            ni.bind("commonsvcs/naming/c++/CVS/Entries.Log",obj,true);
            ni.bind("commonsvcs/naming/c++/CVS/Repository",obj,true);
            ni.bind("commonsvcs/naming/c++/CVS/Root",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/CVS/Entries",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/CVS/Repository",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/CVS/Root",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/NameParser.hpp",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/NameParserContext.hpp",obj,true);
            ni.bind("commonsvcs/naming/c++/incl/NameParserFSM.hpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/CSC.mk",obj,true);
            ni.bind("commonsvcs/naming/c++/src/CVS/Entries",obj,true);
            ni.bind("commonsvcs/naming/c++/src/CVS/Repository",obj,true);
            ni.bind("commonsvcs/naming/c++/src/CVS/Root",obj,true);
            ni.bind("commonsvcs/naming/c++/src/InvalidName.cpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/main.cpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/NameParser.cpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/NameParserContext.cpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/NameParserFSM.cpp",obj,true);
            ni.bind("commonsvcs/naming/c++/src/NamingInterface.cpp",obj,true);
            NamingInterface nitf
                = ni.bindNewContext("commonsvcs/naming/ada.1",true);
            nitf = ni.bindNewContext("commonsvcs/naming/ada.2",true);
        } catch (Exception e) {
            println("Unexpected error (could'nt install initial bindings)");
            result = false;
        }
        return result;
    }

    private NamingContext resolveName(NamingInterface ni,String s)
        throws  NotFound,
                CannotProceed,
                InvalidName,
                TypeMismatchException {

        org.omg.CORBA.Object obj = ni.resolve(s);
        NamingContext nc = null;

        try {
            nc = NamingContextHelper.narrow(obj);
        } catch(SystemException se) {
            throw new TypeMismatchException();
        }

        return nc;
    }

}

