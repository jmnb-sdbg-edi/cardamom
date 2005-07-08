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

import com.thalesgroup.CdmwXML.XMLParser;

import cdmw.ccm.deployment.CadReader;

public class CadReaderTest extends cdmw.testutils.Testable {

    //private static final String CAD_FILENAME = "../data/CadReaderTest.cad";
    private static final String CAD_FILENAME
        = new java.io.File("./CadReaderTest.cad").getAbsolutePath();

    private static final String UUID = "DCE:ffffffff-0000-0000-0000-123456789abc";

    private static XMLParser parser = null;


    public CadReaderTest(XMLParser parser)
    {
        this.parser = parser;
        this.out = System.out;
        this.name = "CadReader tests";
    }


    public void doTests()
    {
        // set number of requested successfull tests
        setNbOfRequestedTestOK(2);
        
        //
        // create CadReader
        //
        println("Test creation of CadReader for " + CAD_FILENAME);
        CadReader cadReader;
        try {
            cadReader = new CadReader(parser, CAD_FILENAME);
            succeed();
        } catch (com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException e) {
            fail("Error while parsing CAD file \"" + CAD_FILENAME + "\":\n" +
                 e.reason);
            return;
        }

        //
        // test getUUID()
        //
        println("Test CadReader.getUUID()");
        String uuid = cadReader.getUUID();
        if (uuid.equals(UUID)) {
            succeed();
        } else {
            fail("UUID found is " + uuid + " instead of " + UUID);
        }
    }

}



