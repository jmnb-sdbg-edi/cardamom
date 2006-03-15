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


#ifndef INCL_TESTCONTAINER_HPP
#define INCL_TESTCONTAINER_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/testutils/TestManager.hpp"

#include "SystemMngt/platformlibrary/Container.hpp"

#include <string>

/**
*Purpose: 
* <p> This class defines element class for test
*
*/
class ElementTest
{

public:

    /**
    * info
    */
    std::string m_info;

    /**
    * Purpose:
    * <p> Constructor
    * 
    */ 
    ElementTest();

    /**
    * Purpose:
    * <p> Constructor
    *
    *@param info  element info
    * 
    */ 
    ElementTest(const std::string& info); 


    /**
    * Purpose:
    * <p> Copy constructor.
    *
    */ 
    ElementTest(const ElementTest& rhs);

    /**
    * Purpose:
    * <p> Destructor
    * 
    */ 
    virtual ~ElementTest();

    /**
    * Purpose:
    * <p> Assignment operator
    * 
    */ 
    ElementTest& operator=(const ElementTest& rhs);
    
};




class TestContainer : public CppUnit::TestFixture
{

public:

   
    /**
    * Define the XContainer from Container template
    */
    typedef Cdmw::PlatformMngt::Container<ElementTest> XContainer;
        
    /**
    * Define the X element
    */
    typedef Cdmw::PlatformMngt::Container<ElementTest>::Element XElement;
        
    /**
    * Define the EntityNameList
    */
    typedef Cdmw::PlatformMngt::Container<ElementTest>::ElementNameList XElementNameList;
    
    /**
    * Define the EntityList
    */
    typedef Cdmw::PlatformMngt::Container<ElementTest>::ElementList XElementList;
        
        
        
        
    /*
    TestContainer(const std::string& name, PortableServer::POA_ptr poa);

    virtual ~TestContainer();
    */

protected:

	virtual void do_tests();

    PortableServer::POA_var m_POA;


CPPUNIT_TEST_SUITE(TestContainer);
CPPUNIT_TEST( do_tests );
CPPUNIT_TEST_SUITE_END();
};

#endif // INCL_TESTCONTAINER_HPP

