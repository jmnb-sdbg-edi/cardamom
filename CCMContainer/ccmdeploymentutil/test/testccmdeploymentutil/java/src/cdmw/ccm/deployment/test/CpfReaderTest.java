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

import cdmw.testutils.Testable;

import cdmw.ccm.deployment.CpfReader;
import cdmw.ccm.deployment.ConfigValues;

import com.thalesgroup.CdmwXML.XMLParser;
import org.omg.Components.ConfigValue;

public class CpfReaderTest extends Testable {

    public static final String PROPERTIES_FILE
        = new java.io.File("./CpfReaderTest.cpf").getAbsolutePath();

    public static final String INTERFACE_ID
        = "IDL:thalesgroup.com/Test/Component1:1.0";

    public static String[] attrNames = {
        "attrib1",
        "attrib2",
        "attrib3",
        "attrib4",
       // "attrib5",
       // "attrib6",
       //"attrib7",
       "attrib8",
        "attrib9"
    };

    private XMLParser parser = null;

    private org.omg.CORBA.ORB orb = null;

    /**
     * Names of the TCKind values
     */
    private static java.util.Hashtable typeNames;

    static {
        //
        // Put the names of the TCKind values in a hashtable
        //
        typeNames = new java.util.Hashtable();
        typeNames.put(org.omg.CORBA.TCKind.tk_null, "null");
        typeNames.put(org.omg.CORBA.TCKind.tk_void, "void");
        typeNames.put(org.omg.CORBA.TCKind.tk_short, "short");
        typeNames.put(org.omg.CORBA.TCKind.tk_ushort, "unsigned short");
        typeNames.put(org.omg.CORBA.TCKind.tk_long, "long");
        typeNames.put(org.omg.CORBA.TCKind.tk_ulong, "unsigned long");
        typeNames.put(org.omg.CORBA.TCKind.tk_float, "float");
        typeNames.put(org.omg.CORBA.TCKind.tk_double, "double");
        typeNames.put(org.omg.CORBA.TCKind.tk_boolean, "boolean");
        typeNames.put(org.omg.CORBA.TCKind.tk_char, "char");
        typeNames.put(org.omg.CORBA.TCKind.tk_octet, "octet");
        typeNames.put(org.omg.CORBA.TCKind.tk_any, "any");
        typeNames.put(org.omg.CORBA.TCKind.tk_TypeCode, "TypeCode");
        typeNames.put(org.omg.CORBA.TCKind.tk_Principal, "Principal");
        typeNames.put(org.omg.CORBA.TCKind.tk_objref, "objref");
        typeNames.put(org.omg.CORBA.TCKind.tk_struct, "struct");
        typeNames.put(org.omg.CORBA.TCKind.tk_union, "union");
        typeNames.put(org.omg.CORBA.TCKind.tk_enum, "enum");
        typeNames.put(org.omg.CORBA.TCKind.tk_string, "string");
        typeNames.put(org.omg.CORBA.TCKind.tk_sequence, "sequence");
        typeNames.put(org.omg.CORBA.TCKind.tk_array, "array");
        typeNames.put(org.omg.CORBA.TCKind.tk_alias, "alias");
        typeNames.put(org.omg.CORBA.TCKind.tk_except, "exception");
        typeNames.put(org.omg.CORBA.TCKind.tk_longlong, "long long");
        typeNames.put(org.omg.CORBA.TCKind.tk_ulonglong,
                       "unsigned long long");
        typeNames.put(org.omg.CORBA.TCKind.tk_longdouble, "long double");
        typeNames.put(org.omg.CORBA.TCKind.tk_wchar, "wchar");
        typeNames.put(org.omg.CORBA.TCKind.tk_wstring, "wstring");
        typeNames.put(org.omg.CORBA.TCKind.tk_fixed, "fixed");
        typeNames.put(org.omg.CORBA.TCKind.tk_value, "valuetype");
        typeNames.put(org.omg.CORBA.TCKind.tk_value_box, "valuebox");
        typeNames.put(org.omg.CORBA.TCKind.tk_native, "native");
        typeNames.put(org.omg.CORBA.TCKind.tk_abstract_interface,
                       "abstract interface");
    }

    public CpfReaderTest(XMLParser parser, org.omg.CORBA.ORB orb) {
        this.parser = parser;
        this.orb = orb;
        this.name = "CpfReader tests";
        this.out = System.out;
    }

    public void doTests() {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(8);

        try {
            println("Registering valuetypes factories...");
            ((org.omg.CORBA_2_3.ORB)orb).register_value_factory(
                com.thalesgroup.Test.Value1Helper.id(),
                    new Value1FactoryImpl());

            CpfReader reader
                = new CpfReader(parser, PROPERTIES_FILE, orb, INTERFACE_ID);
            println(
                "Reading data from the XML file: " + PROPERTIES_FILE + "...");
            ConfigValues values = reader.getConfigValues();
            succeed();

            println("Analysing values...");
            for(int i=0; i<attrNames.length; i++) {
	    ConfigValue cv = values.getConfigValue(attrNames[i]);
                println("data " + i + ": " + "name: " + cv.name
                    + " kind: " + typeNames.get(cv.value.type().kind()));
                printConfigValue(cv);
            }
        } catch(com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException pe) {
            println(pe.reason);
            fail();
        } catch(org.omg.CORBA.SystemException se) {
            se.printStackTrace();
            fail();
        }

    }

    public void printConfigValue(ConfigValue cv) {

        String name = cv.name;

        if (name.equals("attrib1")) {
            println(name + " found, value :" + cv.value.extract_short());
            succeed();
        } else if (name.equals("attrib2")) {
            println(name + " found, value: " + cv.value.extract_ulong());
            succeed();
        } else if (name.equals("attrib3")) {
            println(name + " found, values: ");
            com.thalesgroup.Test.Struct1 struct1
                = com.thalesgroup.Test.Struct1Helper.extract(cv.value);
            println("long1: " + struct1.long1);
            println("string1: " + struct1.string1);
            succeed();
        } else if (name.equals("attrib4")) {
            println(name + " found, values: ");
            com.thalesgroup.Test.Struct1[] struct1Seq
                = com.thalesgroup.Test.Struct1SeqHelper.extract(cv.value);
            for(int i=0; i<struct1Seq.length; i++) {
                println("struct1 " + i);
                println("long1: " + struct1Seq[i].long1);
                println("string1: " + struct1Seq[i].string1);
            }
            succeed();
        } else if (name.equals("attrib5")) {
            println(name + " found, values: ");
            com.thalesgroup.Test.Value1 value1
                = com.thalesgroup.Test.Value1Helper.extract(cv.value);
            println("member1: ");
            com.thalesgroup.Test.Struct1[] member1 = value1.member1;
            for(int i=0; i<member1.length; i++) {
                println("struct1 " + i);
                println("long1: " + member1[i].long1);
                println("string1: " + member1[i].string1);
            }
            succeed();
        } else if (name.equals("attrib6")) {
            println(name + " found, values: ");
            com.thalesgroup.Test.Value1[] value1Seq
                = com.thalesgroup.Test.Value1SeqHelper.extract(cv.value);
            for(int j=0; j<value1Seq.length; j++) {
                println("member1 de la value1 " + j + ": ");
                com.thalesgroup.Test.Struct1[] member1 = value1Seq[j].member1;
                for(int i=0; i<member1.length; i++) {
                    println("struct1 " + i);
                    println("long1: " + member1[i].long1);
                    println("string1: " + member1[i].string1);
                }
            }
            succeed();
        } else if (name.equals("attrib7")) {
            println(name + " found, values: ");
            com.thalesgroup.Test.Value1[][] value1SeqSeq
                = com.thalesgroup.Test.Value1SeqSeqHelper.extract(cv.value);
            for(int k=0; k<value1SeqSeq.length; k++) {
                for(int j=0; j<value1SeqSeq[k].length; j++) {
                    println("member1 de la value1 " + k + "," + j + ": ");
                    com.thalesgroup.Test.Struct1[] member1 = value1SeqSeq[k][j].member1;
                    for(int i=0; i<member1.length; i++) {
                        println("struct1 " + i);
                        println("long1: " + member1[i].long1);
                        println("string1: " + member1[i].string1);
                    }
                }
            }
            succeed();
        } else if (name.equals("attrib8")) {
            println("Checking if enum value is B");
            com.thalesgroup.Test.TestEnum testEnum
                = com.thalesgroup.Test.TestEnumHelper.extract(cv.value);
            check(testEnum.value() == com.thalesgroup.Test.TestEnum.B.value());
        } else if (name.equals("attrib9")) {
            println("Checking if enum sequence length is 2");
            com.thalesgroup.Test.TestEnum[] testEnumSeq
                = com.thalesgroup.Test.TestEnumSeqHelper.extract(cv.value);
            check(testEnumSeq.length == 2);
            println("Checking if enum sequence is A,C");
            check(testEnumSeq[0].value() == com.thalesgroup.Test.TestEnum.A.value()
                && testEnumSeq[1].value() == com.thalesgroup.Test.TestEnum.C.value());
        } else {
            fail();
        }

    }

}

