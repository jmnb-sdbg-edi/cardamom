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

import cdmw.ccm.deployment.Requirement;
import cdmw.ccm.deployment.RequirementList;
import cdmw.ccm.deployment.CsdReader;


public class CsdReaderTest extends cdmw.testutils.Testable {

    private static final String CSD_FILENAME
        = new java.io.File("./CsdReaderTest.csd").getAbsolutePath();

    private static final String NAME             = "CsdTest";
    private static final String VERSION          = "0,1,2,3";
    private static final String UUID             = "DCE:12345678-1234-1234-0003-123456789abc";
    private static final String PROC_NAME        = "x86";
    private static final String OS1_NAME         = "WinNT";
    private static final String OS1_VERSION      = "4,0,0,0";
    private static final String OS2_NAME         = "Win95";
    private static final String OS2_VERSION      = null;
    private static final String LANG_NAME        = "C++";
    private static final String LANG_VERSION     = null;
    private static final String COMPILER_NAME    = "gcc";
    private static final String COMPILER_VERSION = "2,95,2,0";
    private static final String RUNTIME_NAME     = "gdb";
    private static final String RUNTIME_VERSION  = "5,0,0,0";
    private static final String CODE_TYPE        = "DLL";
    private static final String ENTRYPOINT       = "createTestHome";



    private static XMLParser parser = null;


    public CsdReaderTest(XMLParser parser)
    {
        this.parser = parser;
        this.out = System.out;
        this.name = "CsdReader tests";
    }


    public void doTests()
    {
        // set number of requested successfull tests
        setNbOfRequestedTestOK(10);
        
        //
        // create CsdReader
        //
        println("Test creation of CsdReader for " + CSD_FILENAME);
        CsdReader csdReader;
        try {
            csdReader = new CsdReader(parser, CSD_FILENAME);
            succeed();
        } catch (com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException e) {
            fail("Error while parsing CSD file \"" + CSD_FILENAME + "\":\n" +
                 e.reason);
            return;
        }

        //
        // test getName()
        //
        println("Test CsdReader.getName()");
        String name = csdReader.getName();
        if (NAME.equals(name)) {
            succeed();
        } else {
            fail("Name found is " + name + " instead of " + NAME);
        }

        //
        // test getVersion()
        //
        println("Test CsdReader.getVersion()");
        String version = csdReader.getVersion();
        if (VERSION.equals(version)) {
            succeed();
        } else {
            fail("Version found is " + version + " instead of " + VERSION);
        }

        //
        // test getProcessorName()
        //
        println("Test CsdReader.getProcessorName()");
        String procName = csdReader.getProcessorName(UUID);
        if (PROC_NAME.equals(procName)) {
            succeed();
        } else {
            fail("Processor name found is " + procName + " instead of " + PROC_NAME);
        }

        //
        // test getOsList()
        //
        try {
            println("Test CsdReader.getOsList()");
            Requirement os1 = new Requirement(OS1_NAME, OS1_VERSION);
            Requirement os2 = new Requirement(OS2_NAME, OS2_VERSION);

            RequirementList osList = csdReader.getOsList(UUID);
            if (!osList.contains(os1)) {
                fail("OsList doesn't contain " + os1.toString());
            } else if (!osList.contains(os2)) {
                fail("OsList doesn't contain " + os2.toString());
            } else if (osList.size() != 2) {
                fail("OsList size is " + osList.size() + ". It should be 2.");
            } else {
                succeed();
            }
        } catch (cdmw.common.BadParameterException e) {
            fail(e.getMessage());
        } catch (org.omg.Components.CreateFailure e) {
            fail(e.getMessage());
        }

        //
        // test getProgrammingLanguage()
        //
        try {
            println("Test CsdReader.getProgrammingLanguage()");
            Requirement progLang = new Requirement(LANG_NAME, LANG_VERSION);

            Requirement result = csdReader.getProgrammingLanguage(UUID);
            if (!progLang.equals(result)) {
                fail("Programing language found is " + result.toString() +
                     ". Not " + progLang.toString());
            } else {
                succeed();
            }
        } catch (cdmw.common.BadParameterException e) {
            fail(e.getMessage());
        } catch (org.omg.Components.CreateFailure e) {
            fail(e.getMessage());
        }

        //
        // test getCompiler()
        //
        try {
            println("Test CsdReader.getCompiler()");
            Requirement compiler = new Requirement(COMPILER_NAME, COMPILER_VERSION);

            Requirement result = csdReader.getCompiler(UUID);
            if (!compiler.equals(result)) {
                fail("Compiler found is " + result.toString() +
                     ". Not " + compiler.toString());
            } else {
                succeed();
            }
        } catch (cdmw.common.BadParameterException e) {
            fail(e.getMessage());
        } catch (org.omg.Components.CreateFailure e) {
            fail(e.getMessage());
        }

        //
        // test getRuntime()
        //
        try {
            println("Test CsdReader.getRuntime()");
            Requirement runtime = new Requirement(RUNTIME_NAME, RUNTIME_VERSION);

            Requirement result = csdReader.getRuntime(UUID);
            if (!runtime.equals(result)) {
                fail("Runtime found is " + result.toString() +
                     ". Not " + runtime.toString());
            } else {
                succeed();
            }
        } catch (cdmw.common.BadParameterException e) {
            fail(e.getMessage());
        } catch (org.omg.Components.CreateFailure e) {
            fail(e.getMessage());
        }

        //
        // test getCodeType()
        //
        println("Test CsdReader.getCodeType()");
        String codeType = csdReader.getCodeType(UUID);
        if (CODE_TYPE.equals(codeType)) {
            succeed();
        } else {
            fail("CodeType name found is " + codeType + " instead of " + CODE_TYPE);
        }

        //
        // test getEntrypoint()
        //
        println("Test CsdReader.getEntrypoint()");
        String entrypoint = csdReader.getEntrypoint(UUID);
        if (ENTRYPOINT.equals(entrypoint)) {
            succeed();
        } else {
            fail("Entrypoint name found is " + entrypoint + " instead of " + ENTRYPOINT);
        }

    }

}

