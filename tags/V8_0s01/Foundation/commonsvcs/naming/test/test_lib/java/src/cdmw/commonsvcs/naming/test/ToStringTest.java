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
import cdmw.commonsvcs.naming.InvalidNameException;
import cdmw.commonsvcs.naming.NameParser;

import org.omg.CosNaming.NameComponent;

public class ToStringTest extends Testable {

    public ToStringTest() {
        this.out = System.out;
        this.name = "ToStringTest";
    }

    private void testToString() {

        NameComponent[] name;
        NameComponent nc;
        String reference;

        println("Test name components with both ID and Kind field");
        name = new NameComponent[2];
        nc = new NameComponent("first", "1st");
        name[0] = nc;
        nc = new NameComponent("second", "2nd");
        name[1] = nc;

        reference = "first.1st/second.2nd";
        try {
            String output = NameParser.toString(name);
            check( reference.equals(output) );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        println("Test name components with missing Kind field");
        name = new NameComponent[2];
        nc = new NameComponent("first", "");
        name[0] = nc;
        nc = new NameComponent("second", "2nd");
        name[1] = nc;

        reference = "first/second.2nd";
        try {
            String output = NameParser.toString(name);
            check( reference.equals(output) );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        println("Test name components with missing ID field");
        name = new NameComponent[2];
        nc = new NameComponent("first", "1st");
        name[0] = nc;
        nc = new NameComponent("", "2nd");
        name[1] = nc;

        reference = "first.1st/.2nd";
        try {
            String output = NameParser.toString(name);
            check( reference.equals(output) );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        println("Test node with empty ID and Kind field");
        name = new NameComponent[3];
        nc = new NameComponent("first", "1st");
        name[0] = nc;
        nc = new NameComponent("", "");
        name[1] = nc;
        nc = new NameComponent("third", "3rd");
        name[2] = nc;

        reference = "first.1st/./third.3rd";
        try {
            String output = NameParser.toString(name);
            check( reference.equals(output) );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        println("Test nodes with characters that are escaped");
        name = new NameComponent[3];
        nc = new NameComponent("first", "1st.dot");
        name[0] = nc;
        nc = new NameComponent("second.dot", "2nd");
        name[1] = nc;
        nc = new NameComponent("third/slash", "3rd\n");
        name[2] = nc;

        reference = "first.1st\\.dot/second\\.dot.2nd/third\\/slash.3rd\n";
        try {
            String output = NameParser.toString(name);
            check( reference.equals(output) );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        println("Test empty name");
        name = new NameComponent[1];
        nc = new NameComponent("", "");
        name[0] = nc;

        reference = ".";
        try {
            String output = NameParser.toString(name);
            check( reference.equals(output) );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        name = new NameComponent[0];
        try {
            String output = NameParser.toString(name);
            fail("Error: Invalid name component");
        } catch(InvalidNameException ine) {
            succeed("Success: Invalid name component.");
        }

    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(7);

        // This tests NameParser class
        //-------------------------------------------------
        // Testing name to string conversion
        //-------------------------------------------------
        println("Testing NameParser.toString operation");
        testToString();

    }


}

