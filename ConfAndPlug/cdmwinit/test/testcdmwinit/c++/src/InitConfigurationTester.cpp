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


#include "testcdmwinit/InitConfigurationTester.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"

#include <dom/DOM.hpp>
#include <parsers/DOMParser.hpp>

#include <iostream>
#include <memory>

using namespace std;


InitConfigurationTester::InitConfigurationTester()
 : Testable("InitConfigurationTester")
{

}


InitConfigurationTester::~InitConfigurationTester()
{

}


void InitConfigurationTester::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (24);

    try
    {
        Cdmw::CdmwInit::InitUtils::init_xml_library();
        
        using namespace Cdmw::CdmwInit;
        std::auto_ptr<CdmwInitConfiguration> 
            m_init(CdmwInitConfiguration::ReadConfiguration("Process_Gen.xml", true));

        TEST_INFO(std::endl << "- getServiceAttributeValue -");

        std::string res = m_init->getServiceAttributeValue("performance", "result-file-name");
        std::cout << "performance result-file-name = " << res << std::endl;
        TEST_SUCCEED();

        try
        {
            TEST_INFO(std::endl << "- testing getServiceAttributeValue -");
            res = m_init->getServiceAttributeValue("xxxx", "result-file-name");
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }

        try
        {
            TEST_INFO(std::endl << "- testing getServiceAttributeValue -");
            res = m_init->getServiceAttributeValue("performance", "xxx");
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }


        TEST_INFO(std::endl << "- getFrameworkAttributeValue -");

        res = m_init->getFrameworkAttributeValue("first.framework", 0, "domain-name");
        std::cout << "domain-name = " << res << std::endl;
        TEST_SUCCEED();

        res = m_init->getFrameworkAttributeValue("first.framework", 0, "full-factory-name");
        std::cout << "full-factory-name = " << res << std::endl;
        TEST_SUCCEED();

        try
        {
            TEST_INFO(std::endl << "- testing getFrameworkAttributeValue -");
            res = m_init->getFrameworkAttributeValue("xxxx", 0, "domain-name");
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }

        try
        {
            TEST_INFO(std::endl << "- testing getFrameworkAttributeValue -");
            res = m_init->getFrameworkAttributeValue("first.framework", 0, "xxxxx");
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }

        try
        {
            TEST_INFO(std::endl << "- testing getFrameworkAttributeValue -");
            res = m_init->getFrameworkAttributeValue("first.framework", 3, "domain-name");
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }


        TEST_INFO(std::endl << "- getFrameworkMultithreadAttribute -");

        std::string threadingPolicy;
        std::string threadingAttribute;
        std::string threadingAttributeValue;

        threadingAttributeValue = m_init->getFrameworkMultithreadAttribute("first.framework", 0, threadingPolicy, threadingAttribute);
        if (threadingPolicy == "serialize" && threadingAttribute.empty() && threadingAttributeValue.empty())
        {
            std::cout << "EntityResourceFactory_impl 1 : " << std::endl;
            std::cout << threadingPolicy << std::endl;
            TEST_SUCCEED();
        }

        TEST_INFO(std::endl << "- getFrameworkMultithreadAttribute -");
        threadingAttributeValue = m_init->getFrameworkMultithreadAttribute("first.framework", 1, threadingPolicy, threadingAttribute);
        if (threadingPolicy == "thread-per-request" && threadingAttribute.empty() && threadingAttributeValue.empty())
        {
            std::cout << "EntityResourceFactory_impl 2 : " << std::endl;
            std::cout << threadingPolicy << std::endl;
            TEST_SUCCEED();
        }

        TEST_INFO(std::endl << "- getFrameworkMultithreadAttribute -");
        threadingAttributeValue = m_init->getFrameworkMultithreadAttribute("first.framework", 2, threadingPolicy, threadingAttribute);
        if (threadingPolicy == "thread-pool" && threadingAttribute == "thread-size" && threadingAttributeValue == "5")
        {
            std::cout << "EntityResourceFactory_impl 3 : " << std::endl;
            std::cout << threadingPolicy << std::endl;
            std::cout << threadingAttribute << std::endl;
            std::cout << threadingAttributeValue << std::endl;
            TEST_SUCCEED();
        }

        try
        {
            TEST_INFO(std::endl << "- testing getFrameworkMultithreadAttribute -");
            threadingAttributeValue = m_init->getFrameworkMultithreadAttribute("xxxx", 0, threadingPolicy, threadingAttribute);
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }

        try
        {
            TEST_INFO(std::endl << "- testing getFrameworkMultithreadAttribute -");
            threadingAttributeValue = m_init->getFrameworkMultithreadAttribute("first.framework", 3, threadingPolicy, threadingAttribute);
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }


        try
        {
            TEST_INFO(std::endl << "- testing getFrameworkMultithreadAttribute -");
            threadingAttributeValue = m_init->getFrameworkMultithreadAttribute("second.framework", 0, threadingPolicy, threadingAttribute);
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }


        TEST_INFO(std::endl << "- getDatastoreAttributeValue -");

        res = m_init->getDataStoreAttributeValue("DataPart1Store.datastore", 1, "name");
        std::cout << "name = " << res << std::endl;
        TEST_SUCCEED();

 
        res = m_init->getDataStoreAttributeValue("DataPart1Store.datastore", 1, "freeze-size");
        std::cout << "freeze-size = " << res << std::endl;
        TEST_SUCCEED();

        res = m_init->getDataStoreAttributeValue("DataPart1Store.datastore", 1, "datastore-id");
        std::cout << "datastore-id = " << res << std::endl;
        TEST_CHECK(res == "12");

        res = m_init->getDataStoreAttributeValue("DataPart1Store.datastore", 2, "datastore-id");
        std::cout << "datastore-id = " << res << std::endl;
        TEST_CHECK(res == "34");

        res = m_init->getDataStoreAttributeValue("DataPart2Store.datastore", 1, "multicast-port");
        std::cout << "multicast-port = " << res << std::endl;
        TEST_CHECK(res == "5555");

        res = m_init->getDataStoreAttributeValue("DataPart2Store.datastore", 1, "chunk-size");
        std::cout << "chunk-size = " << res << std::endl;
        TEST_CHECK(res == "50");
        try
        {
            res = m_init->getDataStoreAttributeValue("DataPart1Store.datastore", 3, "datastore-id");
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }

        try
        {
            res = m_init->getDataStoreAttributeValue("xxxx", 1, "freeze-size");
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }

        try
        {
            res = m_init->getDataStoreAttributeValue("DataPart1Store.datastore", 1, "xxxx");
            TEST_FAILED();
        }
        catch(const Cdmw::BadParameterException &e)
        {
            std::cout << e.what() << std::endl;
            TEST_SUCCEED();
        }
    }
    catch(const Cdmw::Exception &e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }
    catch(...)
    {
        std::cout << "Unexpected expection" << std::endl;
        TEST_FAILED();
    }

    
    // PCR-0332: try to read another XML file using Xerces
    TEST_INFO("Try to read another XML file.");
    try {
        std::auto_ptr<DOMParser> parser(new DOMParser());
        parser->parse("Another_XML_File.xml");        
        DOM_Document document = parser->getDocument();
        DOM_Element rootElement = document.getDocumentElement();
        DOMString rootName = rootElement.getTagName();
        DOMString toCompare("myRootElement");
        std::cout << rootName.transcode() << std::endl;
        if (rootName.equals(toCompare))
            TEST_SUCCEED();
        else
            TEST_FAILED();    

    } catch (const XMLException& e) {
       std::string xmlExceptionMsg (XMLString::transcode(e.getMessage()));
       std::string msg = "Xerces error: " + xmlExceptionMsg;
       TEST_INFO(msg.c_str());
       TEST_FAILED();
        
    } catch (...) {
        TEST_FAILED();
    }
    
    Cdmw::CdmwInit::InitUtils::cleanup_xml_library();

    
}



