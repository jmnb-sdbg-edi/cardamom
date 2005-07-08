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


package cdmw.ccm.deployment.test;

import cdmw.ccm.deployment.Requirement;
import cdmw.ccm.deployment.RequirementList;



public class RequirementListTest extends cdmw.testutils.Testable {

    private static final String REQ1_NAME    = "ORBacus";
    private static final String REQ1_VERSION = "4,0,4,0";
    private static final String REQ2_NAME    = "JOB";
    private static final String REQ2_VERSION = "4,1,0,0";
    private static final String REQ3_NAME    = "Xerces";
    private static final String REQ3_VERSION = "1,1,3,0";
    private static final String REQ4_NAME    = "Xalan";
    private static final String REQ4_VERSION = "2,3,1,0";
    private static final String REQ5_NAME    = "Cdmw";
    private static final String REQ5_VERSION = "3,0,0,0";


    public RequirementListTest()
    {
        this.out = System.out;
        this.name = "RequirementList tests";
    }

    public void doTests()
    {
        // set number of requested successfull tests
        setNbOfRequestedTestOK(2);
        
        // create requirements
        Requirement r1;
        Requirement r2;
        Requirement r3;
        Requirement r4;
        Requirement r5;

        try {
            r1 = new Requirement(REQ1_NAME, REQ1_VERSION);
            r2 = new Requirement(REQ2_NAME, REQ2_VERSION);
            r3 = new Requirement(REQ3_NAME, REQ3_VERSION);
            r4 = new Requirement(REQ4_NAME, REQ4_VERSION);
            r5 = new Requirement(REQ5_NAME, REQ5_VERSION);
        } catch (cdmw.common.BadParameterException e) {
            fail("Error in creation of a Requirement: " +
                 e.getMessage());
            return;
        }

        // create Requirement lists
        RequirementList rl1 = new RequirementList();
        RequirementList rl2 = new RequirementList();

        // fill Requirement lists
        rl1.add(r1);     rl2.add(r1);
        rl1.add(r3);     rl2.add(r2);
        rl1.add(r4);     rl2.add(r3);
        rl1.add(r5);     rl2.add(r5);

        //
        // Test than add the same Requirement once more does nothing
        //
        println("Test RequirementList.add()");
        Requirement r1bis;
        try {
            r1bis = new Requirement(REQ1_NAME, REQ1_VERSION);
            if (rl1.add(r1bis)) {
                fail("Add of " + r1bis.toString() + " twice succeed !!");
            } else {
                succeed();
            }
        } catch (cdmw.common.BadParameterException e) {
            fail("Error in creation of a Requirement: " +
                 e.getMessage());
            return;
        }

        //
        // Test getIntersectionWith()
        //
        println("Test RequirementList.getIntersectionWith()");
        RequirementList result = rl1.getIntersectionWith(rl2);
        if (!result.contains(r1)) {
            fail("Result doesn't contain " + r1.toString());
        } else if (result.contains(r2)) {
            fail("Result contains " + r2.toString());
        } else if (!result.contains(r3)) {
            fail("Result doesn't contain " + r3.toString());
        } else if (result.contains(r4)) {
            fail("Result contains " + r4.toString());
        } else if (!result.contains(r5)) {
            fail("Result doesn't contain " + r5.toString());
        } else if (result.size() != 3) {
            fail("Result's size is " + result.size() + ". It should be 3.");
        } else {
            succeed();
        }

    }

}

