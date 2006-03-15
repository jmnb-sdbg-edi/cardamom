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
import org.omg.CosNaming.BindingListHolder;
import org.omg.CosNaming.BindingIteratorHolder;


public class ListTest extends Testable {

    private NamingContext context;
    private TestObject testObj;

    public ListTest(NamingContext nc,TestObject to) {
             this.out = System.out;
             this.name = "ListTest";
             this.context = nc;
             this.testObj = to;
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(4);

        NamingInterface ni0 = new NamingInterface(this.context);

        // This tests NamingInterface class
        //-------------------------------------------------
        // Testing list operations
        //-------------------------------------------------
        println("Testing NamingInterface#list operations");

        // Install test name tree
        int nbBindings = 10;
        int chunkSize = 3;
        try {
            ni0.bindNewContext("test.list/ctxt.1", true);
            ni0.bindNewContext("test.list/ctxt.2", true);
            ni0.bindNewContext("test.list/ctxt.3", true);
            ni0.bindNewContext("test.list/ctxt.4", true);
            ni0.bindNewContext("test.list/ctxt.5", true);
            ni0.bind("test.list/obj.1", testObj, true);
            ni0.bind("test.list/obj.2", testObj, true);
            ni0.bind("test.list/obj.3", testObj, true);
            ni0.bind("test.list/obj.4", testObj, true);
            ni0.bind("test.list/obj.5", testObj, true);
        } catch (Exception e) {
            println("Unexpected error (couldn't install initial bindings)");
            fail();
            return;
        }

        String input = "test.list";
        NamingContext nc = null;
        try {
            org.omg.CORBA.Object obj = ni0.resolve(input);
            nc = NamingContextHelper.narrow(obj);
            check(nc!=null);
        } catch(Exception e) {
            println("Unexpected error (" + input + " not present!)");
            fail();
            return;
        }
        NamingInterface ni = new NamingInterface(nc);

        //
        // Test list operation
        //
        println("Test list operation");
        try {
            // Iterator reference
            BindingIteratorHolder it = new BindingIteratorHolder();
            // Binding list
            BindingListHolder bl = new BindingListHolder();

            int nb = 0;

            // Get first chunk
            ni.list(chunkSize,bl,it);
            nb += bl.value.length;

            if (it!=null) {
                // More bindings?
                boolean more;
                do {
                    // Get next chunk
                    more = it.value.next_n(chunkSize,bl);
                    nb += bl.value.length;
                } while (more);
                // Clean up iterator
                it.value.destroy();
            }
            check(nb == nbBindings);
        } catch (Exception e) {
            fail();
        }

        //
        // Test listContext operation
        //
        println("Test listContext operation");
        try {
            BindingListHolder bndList = new BindingListHolder();
            ni.listContext(bndList,chunkSize);

            check( bndList.value.length == nbBindings );
        } catch (Exception e) {
            fail();
        }

        // check zero value for how_many parameter
        try {
            BindingListHolder bndList = new BindingListHolder();
            ni.listContext(bndList,0);

            check( bndList.value.length == nbBindings );
        } catch (Exception e) {
            fail();
        }

    }

}

