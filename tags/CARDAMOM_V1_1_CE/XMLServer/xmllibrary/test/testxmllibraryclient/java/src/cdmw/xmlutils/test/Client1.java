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


package cdmw.xmlutils.test;

import cdmw.testutils.Testable;

import cdmw.xmlutils.XMLHelper;

import cdmw.xmlutils.DocumentDescriptionImpl;
import cdmw.xmlutils.ElementDescriptionImpl;
import cdmw.xmlutils.AttrDescriptionImpl;
import cdmw.xmlutils.TextDescriptionImpl;
import cdmw.xmlutils.CDATASectionDescriptionImpl;

import cdmw.xmlutils.DocumentDescriptionFactoryImpl;
import cdmw.xmlutils.ElementDescriptionFactoryImpl;
import cdmw.xmlutils.AttrDescriptionFactoryImpl;
import cdmw.xmlutils.TextDescriptionFactoryImpl;
import cdmw.xmlutils.CDATASectionDescriptionFactoryImpl;

import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.Node;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Document;
import com.thalesgroup.CdmwXML.DocumentHelper;
import com.thalesgroup.CdmwXML.DocumentDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.Attr;
import com.thalesgroup.CdmwXML.AttrHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.Text;
import com.thalesgroup.CdmwXML.TextHelper;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.CDATASection;
import com.thalesgroup.CdmwXML.CDATASectionHelper;
import com.thalesgroup.CdmwXML.CDATASectionDescription;

public class Client1 extends Testable {

    public static final String FILENAME  = MainTest.XML_FILES_DIRECTORY + "personal.xml";
    public static final String SAVE_NAME = "personal.xml";

    public static final String NEW_FILENAME = "new.xml";
    public static final String DTD_FILENAME = "personal.dtd";

    public XMLParser parser;

    public boolean unitTests;

    public long duration = 0;

    public Client1(XMLParser parser, boolean unitTests) {
        this.name = "XMLLibrary Test";
        this.out = System.out;
        this.unitTests = unitTests;
        this.parser = parser;
    }

    public long getDuration() {
        return this.duration;
    }

    public void doTests() {

        // set number of requested successfull tests
        setNbOfRequestedTestOK(21);

        try {

            println("Parsing file " + FILENAME + "...");
            long start = (new java.util.Date()).getTime();
            DocumentDescription docDesc = parser.parse(FILENAME);
            succeed();
            long duration = (new java.util.Date()).getTime() - start;
            println("Parsing time(in ms): " + duration);
            this.duration = duration;

            println("Getting document node...");
            Document doc = DocumentHelper.narrow(docDesc.reference());
            check(doc != null);

            // ********************************
            // tests of the Document operations
            // ********************************
            println("Getting element root node...");
            ElementDescription elemDesc = doc.get_document_element();
            Element rootElem = ElementHelper.narrow(elemDesc.reference());
            check( rootElem!=null );

            println("Creating elements...");
            Element newElem1
                = doc.create_element(XMLHelper.toShortArray("element1"));
            Element newElem2
                = doc.create_element(XMLHelper.toShortArray("element2"));
            succeed();

            println("Creating attributes...");
            Attr newAttr1 = doc.create_attribute(
                XMLHelper.toShortArray("attr1"),
                XMLHelper.toShortArray("attrValue1"));
            Attr newAttr2 = doc.create_attribute(
                XMLHelper.toShortArray("attr2"),
                XMLHelper.toShortArray("attrValue2"));
            Attr newAttr3 = doc.create_attribute(
                XMLHelper.toShortArray("attr3"),
                XMLHelper.toShortArray("attrValue3"));
            succeed();

            println("Creating text...");
            Text newText
                = doc.create_text_node(XMLHelper.toShortArray("text"));
            succeed();

            println("Creating CDATA Section...");
            CDATASection newCdata = doc.create_CDATASection(
                XMLHelper.toShortArray("CDATA section sample"));
            succeed();

            // ********************************
            // tests of the Element operations
            // ********************************
            println("Attaching some of them...");
            newElem1.set_attribute_node(newAttr1);
            newElem1.set_attribute_node(newAttr2);
            newElem2.append_child(newAttr3);
            newElem2.set_attribute(
                XMLHelper.toShortArray("attr4"),
                XMLHelper.toShortArray("attrValue4"));
            rootElem.append_child(newText);
            rootElem.append_child(newCdata);
            succeed();

            println("Getting all texts from an element...");
            // method 'getFirstChildElement' uses the get_child_elements method
            Element firstElem = getFirstChildElement(rootElem);
            TextDescription[] texts = firstElem.get_texts();
            println("Number of text nodes: " + texts.length);
            succeed();

            println("Getting all attributes from an element...");
            AttrDescription[] attrs = newElem2.get_attributes();
            println("Number of attributes nodes: " + attrs.length);
            succeed();

            println("Moving the first child element to the end...");
            rootElem.append_child(firstElem);
            succeed();
            println("Adding the created element as first element...");
            firstElem = getFirstChildElement(rootElem);
            rootElem.insert_before(newElem1, firstElem);
            succeed();
            println("Removing it...");
            rootElem.remove_child(getFirstChildElement(rootElem));
            succeed();
            println("Replacing the first element with the created one...");
            rootElem.replace_child(newElem1, getFirstChildElement(rootElem));
            succeed();

            // *****************************
            // tests of the Attr operations
            // *****************************
            println("Changing the value of an attribute...");
            newAttr1.set_value(XMLHelper.toShortArray("new attrValue1"));
            AttrDescription attrDesc = (AttrDescription) newAttr1.get_description();
            String value = XMLHelper.toString(attrDesc.value());
            println("value: " + value);
            check(value.equals("new attrValue1"));

            // *****************************
            // tests of the Text operations
            // *****************************
            println("Changing the value of a text node...");
            newText.set_data(XMLHelper.toShortArray("new value of Text"));
            TextDescription textDesc = (TextDescription) newText.get_description();
            String textData = XMLHelper.toString(textDesc.data());
            println("textData: " + textData);
            check(textData.equals("new value of Text"));

            // *************************************
            // tests of the CDATASection operations
            // *************************************
            println("Changing the value of a CDATA section...");
            newCdata.set_data(XMLHelper.toShortArray("new value of CDATASection"));
            CDATASectionDescription cdataDesc = (CDATASectionDescription) newCdata.get_description();
            String cdataSectionData = XMLHelper.toString(cdataDesc.data());
            println("cdataSectionData: " + cdataSectionData);
            check(cdataSectionData.equals("new value of CDATASection"));

            // tests of the XPath operations
            String query = "//person";
            firstElem = getFirstChildElement(rootElem);
            println("Executing the XPath query: " + query + "...");
            NodeDescription[] nodes
                = firstElem.select_nodes(XMLHelper.toShortArray(query));
            println("Number of result nodes: " + nodes.length);
            query = "//person[@id='two.worker']";
            println("Executing the XPath query: " + query + "...");
            nodes = doc.select_nodes_from_root(
                XMLHelper.toShortArray(query));
            println("Number of result nodes: " + nodes.length);
            succeed();

            // tests of the Document operations
            println("Saving file with name " + SAVE_NAME + "...");
            doc.save_as(SAVE_NAME);
            succeed();
            println("Closing the file...");
            doc.close();
            succeed();

            // *************************************
            // test of the creation of new document
            // *************************************
            println("Creating a new XML file...");
            short[] qualifiedName = XMLHelper.toShortArray("toto");
            DocumentDescription testDocDesc = parser.create_empty_document(
                NEW_FILENAME, qualifiedName, DTD_FILENAME);
            Document testDoc = DocumentHelper.narrow(testDocDesc.reference());
            testDoc.create_element(XMLHelper.toShortArray("testElement"));
            println("Saving it...");
            testDoc.save();
            testDoc.close();
            succeed();

        } catch(com.thalesgroup.CdmwXML.XPathFormatException xpfe) {
            fail(xpfe.reason);
        } catch(com.thalesgroup.CdmwXML.DOMException de) {
            fail(de.reason);
        } catch(com.thalesgroup.CdmwXML.DocumentPackage.SaveException se) {
            fail(se.reason);
        } catch(com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException pe) {
            fail(pe.reason);
        } catch(org.omg.CORBA.SystemException se) {
            fail(se.toString());
        }

    }

    private Element getFirstChildElement(Element element) {
        ElementDescription[] elements = element.get_child_elements();
        return ElementHelper.narrow(elements[0].reference());
    }

    private Element getLastChildElement(Element element) {
        ElementDescription[] elements = element.get_child_elements();
        return ElementHelper.narrow(elements[elements.length].reference());
    }

}

