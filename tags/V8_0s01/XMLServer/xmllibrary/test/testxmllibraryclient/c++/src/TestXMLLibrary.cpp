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


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "XMLServer/xmllibrary/XMLHelper.hpp"
#include "testxmllibraryclient/TestXMLLibrary.hpp"

namespace
{
    const int POA_THREAD_POOL_SIZE = 5;

	const int SUCCESS = 0;
	const int FAILURE = 1;

	const std::string FILENAME  = "personal.xml";
	const std::string SAVE_NAME = "personal.xml";

	const std::string NEW_FILENAME = "new.xml";
	const std::string DTD_FILENAME = "personal.dtd";

	CdmwXML::Element_ptr
	get_first_child_element(CdmwXML::Element_ptr element)
	{
		CdmwXML::ElementDescriptionSeq* elements = element -> get_child_elements();
		return CdmwXML::Element::_narrow((*elements)[0] -> reference());
	}
};

namespace Cdmw
{
namespace XMLUtils
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestXMLLibrary::TestXMLLibrary(CdmwXML::XMLParser_ptr parser,
                               CORBA::ORB_ptr orb)
    throw()
    :Testable("Cdmw::XMLUtils::XMLLibrary"),
    m_parser(CdmwXML::XMLParser::_duplicate(parser)),
    m_orb(CORBA::ORB::_duplicate(orb))

{
    // does nothing
}


TestXMLLibrary::~TestXMLLibrary()
    throw()
{
    // does nothing
}


TestXMLLibrary::TestXMLLibrary(const TestXMLLibrary& rhs)
    throw()
    :Testable("Cdmw::XMLUtils::XMLLibrary"),
    m_parser(CdmwXML::XMLParser::_duplicate(rhs.m_parser.in())),
    m_orb(CORBA::ORB::_duplicate(rhs.m_orb.in()))
{
    // does nothing
}

TestXMLLibrary&
TestXMLLibrary::operator=(const TestXMLLibrary& rhs)
    throw()
{
	if (this != &rhs) {
        m_parser = rhs.m_parser;
    }
    return *this;
}

void TestXMLLibrary::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (36);
    
    // This tests EntityCleanupNotifier class
    //-------------------------------------------------
    TEST_INFO("Testing XMLLibrary features");

    using namespace Cdmw::XMLUtils;

    try {

		{
            CORBA::ValueFactoryBase_var factory;
            CORBA::ValueBase_var copy_value;
            
            TEST_INFO("Testing DocumentDescription valuetype...");
            factory = m_orb->lookup_value_factory(
                "IDL:thalesgroup.com/CdmwXML/DocumentDescription:1.0");
            if (factory.in() == 0) {
                TEST_FAILED();
                TEST_INFO("ValuetypeFactory for DocumentDescription not found !");
            } else {
                CdmwXML::DocumentDescription_init* docFactory = 
                    dynamic_cast< CdmwXML::DocumentDescription_init* > (factory.in());
                CdmwXML::DocumentDescription_var doc =
                    docFactory->create(CdmwXML::Document::_nil(), 
                                       XMLHelper::to_DOM_string("testDoc"));
                copy_value = doc->_copy_value();
                CdmwXML::DocumentDescription* doc_copy = 
                    CdmwXML::DocumentDescription::_downcast(copy_value.in());
                TEST_CHECK(doc_copy->type() == CdmwXML::DOCUMENT_NODE);
                char* doctype = XMLHelper::to_char_array(doc_copy->doctype());
                TEST_CHECK(strcmp("testDoc", doctype) == 0);
                delete[] doctype;
            }
                
            TEST_INFO("Testing ElementDescription valuetype...");
            factory = m_orb->lookup_value_factory(
                "IDL:thalesgroup.com/CdmwXML/ElementDescription:1.0");
            if (factory.in() == 0) {
                TEST_FAILED();
                TEST_INFO("ValuetypeFactory for ElementDescription not found !");
            } else {
                CdmwXML::ElementDescription_init* eltFactory = 
                    dynamic_cast< CdmwXML::ElementDescription_init* > (factory.in());
                CdmwXML::ElementDescription_var elt =
                    eltFactory->create(CdmwXML::Element::_nil(), 
                                       XMLHelper::to_DOM_string("testElt"));
                copy_value = elt->_copy_value();
                CdmwXML::ElementDescription* elt_copy = 
                    CdmwXML::ElementDescription::_downcast(copy_value.in());
                TEST_CHECK(elt_copy->type() == CdmwXML::ELEMENT_NODE);
                char* name = XMLHelper::to_char_array(elt_copy->name());
                TEST_CHECK(strcmp("testElt", name) == 0);
                delete[] name;
            }
                
            TEST_INFO("Testing AttrDescription valuetype...");
            factory = m_orb->lookup_value_factory(
                "IDL:thalesgroup.com/CdmwXML/AttrDescription:1.0");
            if (factory.in() == 0) {
                TEST_FAILED();
                TEST_INFO("ValuetypeFactory for AttrDescription not found !");
            } else {
                CdmwXML::AttrDescription_init* attFactory = 
                    dynamic_cast< CdmwXML::AttrDescription_init* > (factory.in());
                CdmwXML::AttrDescription_var att =
                    attFactory->create(CdmwXML::Attr::_nil(), 
                                       XMLHelper::to_DOM_string("testAttr"),
                                       XMLHelper::to_DOM_string("testVal"));
                copy_value = att->_copy_value();
                CdmwXML::AttrDescription* att_copy = 
                    CdmwXML::AttrDescription::_downcast(copy_value.in());
                TEST_CHECK(att_copy->type() == CdmwXML::ATTRIBUTE_NODE);
                char* name = XMLHelper::to_char_array(att_copy->name());
                TEST_CHECK(strcmp("testAttr", name) == 0);
                delete[] name;
                char* value = XMLHelper::to_char_array(att_copy->value());
                TEST_CHECK(strcmp("testVal", value) == 0);
                delete[] value;
                att_copy->value(XMLHelper::to_DOM_string("testVal2"));
                char* value2 = XMLHelper::to_char_array(att_copy->value());
                TEST_CHECK(strcmp("testVal2", value2) == 0);
                delete[] value2;
            }
                
            TEST_INFO("Testing TextDescription valuetype...");
            factory = m_orb->lookup_value_factory(
                "IDL:thalesgroup.com/CdmwXML/TextDescription:1.0");
            if (factory.in() == 0) {
                TEST_FAILED();
                TEST_INFO("ValuetypeFactory for TextDescription not found !");
            } else {
                CdmwXML::TextDescription_init* txtFactory = 
                    dynamic_cast< CdmwXML::TextDescription_init* > (factory.in());
                CdmwXML::TextDescription_var txt =
                    txtFactory->create(CdmwXML::Text::_nil(), 
                                       XMLHelper::to_DOM_string("testTxt"));
                copy_value = txt->_copy_value();
                CdmwXML::TextDescription* txt_copy = 
                    CdmwXML::TextDescription::_downcast(copy_value.in());
                TEST_CHECK(txt_copy->type() == CdmwXML::TEXT_NODE);
                char* data = XMLHelper::to_char_array(txt_copy->data());
                TEST_CHECK(strcmp("testTxt", data) == 0);
                delete[] data;
                txt_copy->data(XMLHelper::to_DOM_string("testTxt2"));
                char* data2 = XMLHelper::to_char_array(txt_copy->data());
                TEST_CHECK(strcmp("testTxt2", data2) == 0);
                delete[] data2;
            }
                
            TEST_INFO("Testing CDATASectionDescription valuetype...");
            factory = m_orb->lookup_value_factory(
                "IDL:thalesgroup.com/CdmwXML/CDATASectionDescription:1.0");
            if (factory.in() == 0) {
                TEST_FAILED();
                TEST_INFO("ValuetypeFactory for CDATASectionDescription not found !");
            } else {
                CdmwXML::CDATASectionDescription_init* cdataFactory = 
                    dynamic_cast< CdmwXML::CDATASectionDescription_init* > (factory.in());
                CdmwXML::CDATASectionDescription_var cdata =
                    cdataFactory->create(CdmwXML::CDATASection::_nil(), 
                                         XMLHelper::to_DOM_string("testCDATA"));
                copy_value = cdata->_copy_value();
                CdmwXML::CDATASectionDescription* cdata_copy = 
                    CdmwXML::CDATASectionDescription::_downcast(copy_value.in());
                TEST_CHECK(cdata_copy->type() == CdmwXML::CDATA_SECTION_NODE);
                char* data = XMLHelper::to_char_array(cdata_copy->data());
                TEST_CHECK(strcmp("testCDATA", data) == 0);
                delete[] data;
                cdata_copy->data(XMLHelper::to_DOM_string("testCDATA2"));
                char* data2 = XMLHelper::to_char_array(cdata_copy->data());
                TEST_CHECK(strcmp("testCDATA2", data2) == 0);
                delete[] data2;
            }
                
        }
        
		TEST_INFO("Parsing file " << FILENAME << "...");
		time_t start = time(0);
		CdmwXML::DocumentDescription* docDesc = m_parser -> parse(FILENAME.c_str());
		TEST_SUCCEED();
		time_t end = time(0);
		TEST_INFO("Parsing time (in s): " << difftime(end, start));

		TEST_INFO("Getting document node...");
		CdmwXML::Node_ptr node = docDesc -> reference();
		CdmwXML::Document_var doc = CdmwXML::Document::_narrow(node);
		TEST_CHECK(!CORBA::is_nil(doc.in()));

		// ********************************
		// tests of the Document operations
		// ********************************
		TEST_INFO("Getting element root node...");
		CdmwXML::ElementDescription* elemDesc = doc -> get_document_element();
		CdmwXML::Element_var rootElem
			= CdmwXML::Element::_narrow(elemDesc -> reference());
		TEST_CHECK(!CORBA::is_nil(rootElem.in()));

		TEST_INFO("Creating elements...");
		CdmwXML::Element_ptr newElem1
			= doc -> create_element(XMLHelper::to_DOM_string("element1"));
		CdmwXML::Element_ptr newElem2
			= doc -> create_element(XMLHelper::to_DOM_string("element2"));
		TEST_SUCCEED();

		TEST_INFO("Creating attributes...");
		CdmwXML::Attr_ptr newAttr1 = doc -> create_attribute(
			XMLHelper::to_DOM_string("attr1"),
			XMLHelper::to_DOM_string("attrValue1"));
		CdmwXML::Attr_ptr newAttr2 = doc -> create_attribute(
			XMLHelper::to_DOM_string("attr2"),
			XMLHelper::to_DOM_string("attrValue2"));
		CdmwXML::Attr_ptr newAttr3 = doc -> create_attribute(
			XMLHelper::to_DOM_string("attr3"),
			XMLHelper::to_DOM_string("attrValue3"));
		TEST_SUCCEED();

		TEST_INFO("Creating text...");
		CdmwXML::Text_ptr newText
			= doc -> create_text_node(XMLHelper::to_DOM_string("text"));
		TEST_SUCCEED();

		TEST_INFO("Creating CDATA Section...");
		CdmwXML::CDATASection_ptr newCdata = doc -> create_CDATASection(
			XMLHelper::to_DOM_string("CDATA section sample"));
		TEST_SUCCEED();

		// ********************************
		// tests of the Element operations
		// ********************************
		TEST_INFO("Attaching some of them...");
		newElem1 -> set_attribute_node(newAttr1);
		newElem1 -> set_attribute_node(newAttr2);
		newElem2 -> append_child(newAttr3);
		newElem2 -> set_attribute(
			XMLHelper::to_DOM_string("attr4"),
			XMLHelper::to_DOM_string("attrValue4"));
		rootElem -> append_child(newText);
		rootElem -> append_child(newCdata);
		TEST_SUCCEED();

		// method 'get_first_child_element' uses the get_child_elements method
		CdmwXML::Element_ptr firstElem = get_first_child_element(rootElem.in());
		TEST_INFO("Getting all texts from an element...");
		CdmwXML::TextDescriptionSeq* texts = firstElem -> get_texts();
		TEST_INFO("Number of text nodes: " << texts -> length());
		TEST_SUCCEED();

		TEST_INFO("Getting all attributes from an element...");
		CdmwXML::AttrDescriptionSeq* attrs = newElem2 -> get_attributes();
		TEST_INFO("Number of attributes nodes: " << attrs -> length());
		TEST_SUCCEED();

		TEST_INFO("Moving the first child element to the end...");
		rootElem -> append_child(firstElem);
		TEST_SUCCEED();
		TEST_INFO("Adding the created element as first element...");
		firstElem = get_first_child_element(rootElem.in());
		rootElem -> insert_before(newElem1, firstElem);
		TEST_SUCCEED();
		TEST_INFO("Removing it...");
		rootElem -> remove_child(get_first_child_element(rootElem.in()));
		TEST_SUCCEED();
		TEST_INFO("Replacing the first element with the created one...");
		rootElem -> replace_child(newElem1, get_first_child_element(rootElem.in()));
		TEST_SUCCEED();

		// *****************************
		// tests of the Attr operations
		// *****************************
		TEST_INFO("Changing the value of an attribute...");
		newAttr1 -> set_value(XMLHelper::to_DOM_string("new attrValue1"));
		CdmwXML::AttrDescription_var attrDesc
			= dynamic_cast<CdmwXML::AttrDescription *>(newAttr1->get_description());
		char* value = XMLHelper::to_char_array(attrDesc -> value());
		TEST_CHECK(strcmp(value, "new attrValue1") == 0);

		// *****************************
		// tests of the Text operations
		// *****************************
		TEST_INFO("Changing the value of a text node...");
		newText -> set_data(XMLHelper::to_DOM_string("new value of Text"));
		CdmwXML::TextDescription_var textDesc
			= dynamic_cast<CdmwXML::TextDescription *>(newText->get_description());
		char* textData = XMLHelper::to_char_array(textDesc -> data());
		TEST_CHECK(strcmp(textData, "new value of Text") == 0);

		// *************************************
		// tests of the CDATASection operations
		// *************************************
		TEST_INFO("Changing the value of a CDATA section...");
		newCdata -> set_data(XMLHelper::to_DOM_string("new value of CDATASection"));
		CdmwXML::CDATASectionDescription_var cdataDesc
			= dynamic_cast<CdmwXML::CDATASectionDescription *>(newCdata->get_description());
		char* cdataSectionData = XMLHelper::to_char_array(cdataDesc -> data());
		TEST_CHECK(strcmp(cdataSectionData, "new value of CDATASection") == 0);

		// tests of the XPath operations
		std::string query = "//person";
		TEST_INFO("Executing the XPath query: " <<  query <<  "...");
		firstElem = get_first_child_element(rootElem.in());
		CdmwXML::NodeDescriptionSeq* nodes = firstElem -> select_nodes(
			XMLHelper::to_DOM_string(query.c_str()));
		TEST_INFO("Number of result nodes: " <<  nodes -> length());
		query = "//person[@id='two.worker']";
		TEST_INFO("Executing the XPath query: " <<  query <<  "...");
		nodes = doc -> select_nodes_from_root(
			XMLHelper::to_DOM_string(query.c_str()));
		TEST_INFO("Number of result nodes: " <<  nodes -> length());
		TEST_SUCCEED();

		// tests of the Document operations
		TEST_INFO("Saving file with name " << SAVE_NAME << "...");
		doc -> save_as(SAVE_NAME.c_str());
		TEST_SUCCEED();
		TEST_INFO("Closing the file...");
		doc -> close();
		TEST_SUCCEED();

		// ***************************************
		// test of the creation of a new document
		// ***************************************
		TEST_INFO("Creating a new XML file...");
		CdmwXML::DOMString& qualifiedName = XMLHelper::to_DOM_string("toto");
		CdmwXML::DocumentDescription* testDocDesc
			= m_parser -> create_empty_document(
				NEW_FILENAME.c_str(), qualifiedName, DTD_FILENAME.c_str());
		TEST_SUCCEED();
		CdmwXML::Document_var testDoc
			= CdmwXML::Document::_narrow(testDocDesc -> reference());
		testDoc -> create_element(XMLHelper::to_DOM_string("testElement"));

		TEST_INFO("Saving it...");
		testDoc -> save();
		testDoc -> close();
		TEST_SUCCEED();

	} catch (const CdmwXML::DOMException & de) {
	    std::cerr << "DOMException: " << de.reason.in() << std::endl;
	} catch (const CdmwXML::Document::SaveException & se) {
	    std::cerr << "SaveException: " << se.reason.in() << std::endl;
	} catch (const CdmwXML::XMLParser::ParsingException & pe) {
	    std::cerr << "ParsingException: " << pe.reason.in() << std::endl;
    } catch (const CORBA::Exception & e) {
	    std::cerr << "ERROR: " << OrbSupport::OrbSupport::exception_to_string( e ) << std::endl;
    }

}

}; // End namespace XMLUtils
}; // End namespace Cdmw

