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


#include "testplatformlibrary/TestContainer.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/System.hpp"

#include <iostream>
#include <string>
#include <memory>



using namespace Cdmw::PlatformMngt;


CPPUNIT_TEST_SUITE_REGISTRATION( TestContainer );

// ElementTest implementation

//
// default constructor
//
ElementTest::ElementTest()
{

}

//
// constructor with parameters
// param info  element info
//
ElementTest::ElementTest(const std::string& info)
     : m_info(info)
{
}

//
// copy constructor
//
ElementTest::ElementTest(const ElementTest& rhs)
{
    m_info = rhs.m_info;
}

//
// destructor
//
ElementTest::~ElementTest()
{

}


//
// = operator
//
ElementTest& ElementTest::operator=(const ElementTest& rhs)
{
    m_info = rhs.m_info;

    return *this;
}



//
// initialization of static member of entity container
//

#ifdef AIX

namespace Cdmw
{
namespace PlatformMngt
{
template <class ELEMENT_DATA> Cdmw::OsSupport::Mutex Container<ELEMENT_DATA>::M_existingContainers_mutex;
template <class ELEMENT_DATA> typename Container<ELEMENT_DATA>::Containers Container<ELEMENT_DATA>::M_existingContainers;
}
}

#else

Cdmw::OsSupport::Mutex TestContainer::XContainer::M_existingContainers_mutex;
TestContainer::XContainer::Containers TestContainer::XContainer::M_existingContainers;

#endif



void displayXElement (const char* s)
{
    std::cout << "element name :" << s << std::endl << std::endl;
}


void displayXElement (const TestContainer::XElementNameList& el)
{
	// get length of list
	size_t len = el.size();
	
	// initialize list iterator
    TestContainer::XElementNameList::const_iterator it = el.begin();
             
    for (size_t i = 0; i < len; i++)
    {           	
        // print element name from list using iterator
        std::cout << "element name :" << (*it).c_str() << std::endl;

        // increment iterator
        it++;
    }
 
    std::cout << std::endl;
}


void displayXElement (const TestContainer::XElement& e)
{
    std::cout << "element name   :" << e.m_name.c_str() << std::endl;
    std::cout << "element info   :" << e.m_data.m_info.c_str() << std::endl;   
    std::cout << std::endl;
}

void displayXElement (const TestContainer::XElementList& el)
{
	// get length of list
	size_t len = el.size();
	
	// initialize list iterator
    TestContainer::XElementList::const_iterator it = el.begin();
             
    for (size_t i = 0; i < len; i++)
    {   
    	// get element
        TestContainer::XElement element = *it;  
        
        // display element data
        displayXElement (element);      	

        // increment iterator
        it++;
    }
 
    std::cout << std::endl;
}

/*
TestContainer::TestContainer(const std::string& name, PortableServer::POA_ptr poa)
{
    m_POA = PortableServer::POA::_duplicate(poa);
}


TestContainer::~TestContainer()
{
}
*/


void TestContainer::do_tests()
{
    // set number of requested successfull tests
// //     set_nbOfRequestedTestOK (50);
    
    
	XContainer *pSystemContainer = NULL;
	XContainer *pApplicationContainer = NULL;
	XContainer *pProcessContainer = NULL;
	
    try
    {
        // create the element containers
        TEST_INFO("");
        TEST_INFO("Creates a System Container");
        pSystemContainer = 
                XContainer::createSystemContainer ();
  
  
        TEST_INFO("Creates an Application Container");
        pApplicationContainer = 
                XContainer::createApplicationContainer ("application_1");
                
        TEST_INFO("Creates a Process Container");
        pProcessContainer = 
                XContainer::createProcessContainer ("application_1", "process_1", "host_1");
 
        CPPUNIT_ASSERT(true);
 
        // try to recreate same element container : exception must be thrown
        TEST_INFO("Recreates a System Container");
        try
        {
            XContainer::createSystemContainer ();
   
            CPPUNIT_ASSERT(false);
        }
        catch (AlreadyExistsException& ex)
        {
            CPPUNIT_ASSERT(true);
        }
  
  
        TEST_INFO("Recreates an Application Container");
        try
        {
            XContainer::createApplicationContainer ("application_1");
            CPPUNIT_ASSERT(false);
        }
        catch (AlreadyExistsException& ex)
        {
            CPPUNIT_ASSERT(true);
        }
                
        TEST_INFO("Recreates a Process Container");
        try
        {
            XContainer::createProcessContainer ("application_1", "process_1", "host_1");
            CPPUNIT_ASSERT(false);
        }
        catch (AlreadyExistsException& ex)
        {
            CPPUNIT_ASSERT(true);
        }
    
    
        // check if containers exist
        TEST_INFO("Check if containers exist");
        
        std::string containerId = 
             XContainer::systemContainerNameToId ();
        bool ret = XContainer::existsContainerId(containerId);
        if (ret)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        XContainer* pContainer =
             XContainer::findContainerById(containerId);
        if (pContainer != NULL)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
  
        containerId = 
             XContainer::applicationContainerNameToId ("application_1");                                                            
        ret = XContainer::existsContainerId(containerId);
        if (ret)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        pContainer =
             XContainer::findContainerById(containerId);
        if (pContainer != NULL)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        containerId = 
            XContainer::processContainerNameToId ("application_1", "process_1", "host_1");
        ret = XContainer::existsContainerId(containerId);
        if (ret)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        pContainer =
             XContainer::findContainerById(containerId);
        if (pContainer != NULL)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
 
         
        // check if container does not exist
        TEST_INFO("Check if container does not exist"); 
        ret = XContainer::existsContainerId("xxxx");
        if (ret)
        {
            CPPUNIT_ASSERT(false);
        }
        else
        {
            CPPUNIT_ASSERT(true);
        }   
        
        pContainer =
             XContainer::findContainerById("xxxx");
        if (pContainer == NULL)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        } 
        
        CPPUNIT_ASSERT(true); 
         
        // Check container identification
        TEST_INFO("Check container identification"); 
        std::string containerIdentStr = " system collector ident      \"";
        containerIdentStr += pSystemContainer->id();
        containerIdentStr += "\"";
        TEST_INFO(containerIdentStr.c_str());
        containerIdentStr = " application collector ident \"";
        containerIdentStr += pApplicationContainer->id();
        containerIdentStr += "\"";
        TEST_INFO(containerIdentStr.c_str());
        containerIdentStr = " process collector ident     \"";
        containerIdentStr += pProcessContainer->id();
        containerIdentStr += "\"";
        TEST_INFO(containerIdentStr.c_str());
        CPPUNIT_ASSERT(true);
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        CPPUNIT_ASSERT(false);
        return;
    } 
    
    
    
    
    // Check when no element inserted in container
    try
    {    
    	// Check number of elements : must be 0
    	TEST_INFO("");
    	TEST_INFO("Check Container is empty");
        size_t nbrOfElements = pSystemContainer->getNumberOfElements();
        
        if (nbrOfElements == 0)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        bool containerEmpty = pSystemContainer->isEmpty();
        
        if (containerEmpty == true)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        // Get all element names : must be empty
        
        // list of request element names from container
        XElementNameList elementNameReqList;
         
        // list of rest of element names from container
        XElementNameList elementNameRestList;

        // get list of element names from container
        TEST_INFO("Check Container return empty name lists");
        
        pSystemContainer->listName (5, 
                                    elementNameReqList, elementNameRestList);
        
        if (elementNameReqList.size() == 0)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        if (elementNameRestList.size() == 0)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        pSystemContainer->listName (elementNameReqList);
        
        if (elementNameReqList.size() == 0)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        
        // Get all element : must be empty
        
        // list of request elements from container
        XElementList elementReqList;
         
        // list of rest of elements from container
        XElementList elementRestList;

        TEST_INFO("Check Container return empty element lists");
        pSystemContainer->list (5, elementReqList, elementRestList);

        if (elementReqList.size() == 0)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        if (elementRestList.size() == 0)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
  
        pSystemContainer->list (elementReqList);

        if (elementReqList.size() == 0)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
  

        // get element of container
        TEST_INFO("Check to get element with undefined name");
        bool exist = pSystemContainer->existsElement("e1");
        if (exist == false)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        ElementTest data;
        exist = pSystemContainer->findElement("e1", data);
        if (exist == false)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        CPPUNIT_ASSERT(false);
        return;
    }





    // Check elements insertion in container
    try
    {
    	TEST_INFO("");
        
        TEST_INFO("Check adding elements in Container");
        
        ElementTest data1  ("information 1");
        ElementTest data2  ("information 2");
        ElementTest data3  ("information 3");
        ElementTest data4  ("information 4");
        ElementTest data5  ("information 5");
        ElementTest data11 ("information 11");
        
        bool added;
        
        added = pSystemContainer->addElement ("e1",  data1);
        if (added == true)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        added = pSystemContainer->addElement ("e2",  data2);
        if (added == true)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        added = pSystemContainer->addElement ("e3",  data3);
        if (added == true)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        added = pSystemContainer->addElement ("e4",  data4);
        if (added == true)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        added = pSystemContainer->addElement ("e5",  data5);
        if (added == true)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        added = pSystemContainer->addElement ("e11", data11);
        if (added == true)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }

        
        TEST_INFO("Check adding duplicate element in Container");
        added = pSystemContainer->addElement ("e11", data11);
        if (added == true)
        {
            CPPUNIT_ASSERT(false);
        }
        else
        {
            CPPUNIT_ASSERT(true);
        }
 
 
        
        
        TEST_INFO("");
        
        TEST_INFO("Check updating elements in Container");
        
        ElementTest data1a ("information 1a");
        ElementTest data2a ("information 2a");
        ElementTest data6  ("information 6");
        ElementTest data7  ("information 7");
        ElementTest data8  ("information 8");
        ElementTest data9  ("information 9");
        ElementTest data10 ("information 10");
        
        bool updated;

        updated = pSystemContainer->updateElement ("e1",  data1a);
        if (updated)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }

        
        updated = pSystemContainer->updateElement ("e6",  data6);
        if (!updated)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }

             
        TEST_INFO("");
        
        TEST_INFO("Check removing undefined element in Container");
        bool removed = pSystemContainer->removeElement ("e12");
        if (removed == true)
        {
            CPPUNIT_ASSERT(false);
        }
        else
        {
            CPPUNIT_ASSERT(true);
        }
        
        TEST_INFO("Check removing element in Container");
        removed = pSystemContainer->removeElement ("e11");
        if (removed == true)
        {
        	CPPUNIT_ASSERT(true);          
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        CPPUNIT_ASSERT(false);
        return;
    }
    
 
 
 
 

    try
    {    
    	// Check number of elements : must be 5
    	TEST_INFO("");
    	TEST_INFO("Check Container is filled");
        size_t nbrOfElements = pSystemContainer->getNumberOfElements();
        
        if (nbrOfElements == 5)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        bool containerEmpty = pSystemContainer->isEmpty();
        
        if (containerEmpty == false)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        // Get all element names
        
        // list of request element names from container
        XElementNameList elementNameReqList;
         
        // list of rest of element names from container
        XElementNameList elementNameRestList;

        // get list of element names from container
        TEST_INFO("");
        TEST_INFO("Check Container return name lists");
        
        pSystemContainer->listName (3, 
                                    elementNameReqList, elementNameRestList);
 
        TEST_INFO("requested name list :");              
        size_t len = elementNameReqList.size();
        if (len == 3)
        {
        	displayXElement (elementNameReqList);
  
            TEST_INFO("");
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        

             
        TEST_INFO("rest name list :");
        len = elementNameRestList.size();
        if (len == 2)
        {
        	displayXElement (elementNameRestList);
            
            TEST_INFO("");
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
 
 
 
 
 
        // list of all element names from container
        XElementNameList elementNameAllList;  
             
        pSystemContainer->listName (elementNameAllList);
 
        TEST_INFO("complete name list :");
        len = elementNameAllList.size();       
        if (len == 5)
        {
        	displayXElement (elementNameAllList);
            
            TEST_INFO("");
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        
        // Get all elements
        
        // list of request elements from container
        XElementList elementReqList;
         
        // list of rest of elements from container
        XElementList elementRestList;

        TEST_INFO("");
        TEST_INFO("Check Container return element lists");
        pSystemContainer->list (3, elementReqList, elementRestList);


        TEST_INFO("requested element list :");              
        len = elementReqList.size();
        if (len == 3)
        {
        	displayXElement (elementReqList);
            
            TEST_INFO("");
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        

             
        TEST_INFO("rest name list :");
        len = elementNameRestList.size();
        if (len == 2)
        {
        	displayXElement (elementRestList);
            
            TEST_INFO("");
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
  
  
        // list of all elements from container
        XElementList elementAllList;  
        
        pSystemContainer->list (elementAllList);

        if (elementAllList.size() == 5)
        {
        	displayXElement (elementAllList);
            
            TEST_INFO("");
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
  

        // get element of container
        TEST_INFO("Check to get element with undefined name");
        bool exist = pSystemContainer->existsElement("e11");
        if (exist == false)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        ElementTest data;
        exist = pSystemContainer->findElement("e11", data);
        if (exist == false)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        
        
        TEST_INFO("Check to get element");
        std::string elem_name = "e5";
        exist = pSystemContainer->existsElement(elem_name);
        if (exist == true)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        
       
        exist = pSystemContainer->findElement(elem_name, data);
        if (exist == true)
        {
        	XElement element (elem_name, data);
        	displayXElement (element);
            CPPUNIT_ASSERT(true);
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        CPPUNIT_ASSERT(false);
        return;
    }    



    try
    {
        // destroy the containers
        TEST_INFO("Destroy containers");
        
        bool ret = XContainer::destroy (pSystemContainer);
        if (ret)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {        
            CPPUNIT_ASSERT(false);
        }
        
        ret = XContainer::destroy (pApplicationContainer);
        if (ret)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {        
            CPPUNIT_ASSERT(false);
        }
        
        ret = XContainer::destroy (pProcessContainer);
        if (ret)
        {
            CPPUNIT_ASSERT(true);
        }
        else
        {        
            CPPUNIT_ASSERT(false);
        }
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        CPPUNIT_ASSERT(false);
        return;
    } 
}


