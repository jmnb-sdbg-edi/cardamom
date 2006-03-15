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


package cdmw.ccm.deployment.test;

import cdmw.ccm.deployment.Requirement;


public class RequirementTest extends cdmw.testutils.Testable {

    private static final String REQ1_NAME    = "ORBacus";
    private static final String REQ1_VERSION = "4,0,4,0";
    private static final String REQ2_NAME    = "ORBacus";
    private static final String REQ2_VERSION = "4,0,4,1";
    private static final String REQ3_NAME    = "ORBacus";
    private static final String REQ3_VERSION = null;


    public RequirementTest()
    {
        this.out = System.out;
        this.name = "Requirement tests";
    }

    public void doTests()
    {
        // set number of requested successfull tests
        setNbOfRequestedTestOK(8);
        
        Requirement r1;
        Requirement r2;
        Requirement r3;

        com.thalesgroup.CdmwDeployment.RequirementDescription req2Descr =
            new com.thalesgroup.CdmwDeployment.RequirementDescription();
        req2Descr.name = REQ2_NAME;
        req2Descr.version = REQ2_VERSION;

        try {
            //
            // test creation of Requirement
            //
            println("Test creation of Requirements");
            r1 = new Requirement(REQ1_NAME, REQ1_VERSION);
            succeed();
            r2 = new Requirement(req2Descr);
            succeed();
            r3 = new Requirement(REQ3_NAME, REQ3_VERSION);
            succeed();
        } catch (cdmw.common.BadParameterException e) {
            fail("Error in creation of a Requirement: " +
                 e.getMessage());
            return;
        }

        //
        // test getVersion()
        //
        println("Test Requirement.getVersion()");
        if (r1.getVersion().equals(REQ1_VERSION)) {
            succeed();
        } else {
            fail(r1.getVersion() + " is different of " + REQ1_VERSION);
        }

        //
        // test equals()
        //
        println("Test Requirement.equals()");
        Requirement r1bis;
        try {
            r1bis = new Requirement(REQ1_NAME, REQ1_VERSION);

            if (r1.equals(r1bis)) {
                succeed();
            } else {
                fail("(" + r1.toString() + ") != (" + r1bis.toString() + ") !!");
            }
            if (!r1.equals(r2)) {
                succeed();
            } else {
                fail("(" + r1.toString() + ") == (" + r2.toString() + ") !!");
            }
        } catch (cdmw.common.BadParameterException e) {
            fail("Error in creation of a Requirement: " +
                 e.getMessage());
        }

        //
        // test moreRecentThan
        //
        println("Test Requirement.moreRecentThan()");
        try {
            if (r2.moreRecentThan(r1)) {
                succeed();
            } else {
                fail("(" + r1.toString() + ") is more recent than (" +
                     r2.toString() + ")");
            }
            if (r1.moreRecentThan(r3)) {
                succeed();
            } else {
                fail("(" + r3.toString() + ") is more recent than (" +
                     r1.toString() + ")");
            }

        } catch (cdmw.common.BadParameterException e) {
            fail("Error in comparaison of Requirements: " +
                 e.getMessage());
        }

    }

}


