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


public class ToNameTest extends Testable {

    public ToNameTest() {
        this.out = System.out;
        this.name = "ToNameTest";
    }

    private void testToName() {

        String input = new String();

        println("Test name components with both ID and Kind field");
        input = "first.1st/second.2nd";
        try {
            NameComponent[] name = NameParser.toName(input);
            check( name.length == 2 );
            check( name[0].id.equals("first") );
            check( name[0].kind.equals("1st") );
            check( name[1].id.equals("second") );
            check( name[1].kind.equals("2nd") );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        println("Test name components with missing Kind field");
        input = "first.1st/second";
        try {
            NameComponent[] name = NameParser.toName(input);
            check( name.length == 2 );
            check( name[0].id.equals("first") );
            check( name[0].kind.equals("1st") );
            check( name[1].id.equals("second") );
            check( name[1].kind.length() == 0 );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        println("Test name components with missing ID field");
        input = "first.1st/.2nd";
        try {
            NameComponent[] name = NameParser.toName(input);
            check( name.length == 2 );
            check( name[0].id.equals("first") );
            check( name[0].kind.equals("1st") );
            check( name[1].id.length() == 0 );
            check( name[1].kind.equals("2nd") );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        println("Test node with empty ID and Kind field");
        input = "first.1st/./third.3rd";
        try {
            NameComponent[] name = NameParser.toName(input);
            check( name.length == 3 );
            check( name[0].id.equals("first") );
            check( name[0].kind.equals("1st") );
            check( name[1].id.length() == 0 );
            check( name[1].id.length() == 0 );
            check( name[2].id.equals("third") );
            check( name[2].kind.equals("3rd") );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        println("Test nodes with characters that are escaped");
        input = "first.1st\\.dot/second\\.dot.2nd/third\\/slash.3rd\n";
        try {
            NameComponent[] name = NameParser.toName(input);
            check( name.length == 3 );
            check( name[0].id.equals("first") );
            check( name[0].kind.equals("1st.dot") );
            check( name[1].id.equals("second.dot") );
            check( name[1].kind.equals("2nd") );
            check( name[2].id.equals("third/slash") );
            check( name[2].kind.equals("3rd\n") );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        println("Test empty name");
        input = ".";
        try {
            NameComponent[] name = NameParser.toName(input);
            check( name.length == 1 );
            check( name[0].id.length() == 0 );
            check( name[0].kind.length() == 0 );
        } catch(InvalidNameException ine) {
            fail(ine.getMessage());
        }

        input = "";
        try {
            NameComponent[] name = NameParser.toName(input);
            // The empty sequence is not a legal name
            fail("Error: An empty string is not a legal name.");
        } catch(InvalidNameException ine) {
            succeed("Success: An empty string is not a legal name.");
        }

        println("Test ending '.'");
        input = "first.1st/second.";
        try {
            NameComponent[] name = NameParser.toName(input);
            // If a name component has a non-empty id field and an empty kind field,
            // the stringified representation consists only of the id field.
            // A trailing '.' character is not permitted.
            fail("Error: A trailing '.' character is not permitted.");
        } catch(InvalidNameException ine) {
            succeed("Success: A trailing '.' character is not permitted.");
        }

        println("Test starting '/'");
        input = "/first.1st/second.2nd";
        try {
            NameComponent[] name = NameParser.toName(input);
            // The single '.' character is the only representation of a name
            // component with empty id and kind fields.
            fail("Error: A name component with a starting '/' is not permitted.");
        } catch(InvalidNameException ine) {
            succeed("Success: A name component with a starting '/' is not permitted.");
        }


    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(35);

        // This tests NameParser class
        //-------------------------------------------------
        // Testing string to name conversion
        //-------------------------------------------------
        println("Testing NameParser.toName operation");
        testToName();

    }


}

