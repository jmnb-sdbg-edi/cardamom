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


#ifndef INCL_NAMINGINTERFACE_TEST_NAMINGINTERFACE_LIST_HPP
#define INCL_NAMINGINTERFACE_TEST_NAMINGINTERFACE_LIST_HPP

#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.stub.hpp"
#include "test_lib/TestObject.stub.hpp"

namespace Cdmw
{
namespace CommonSvcs
{
namespace Naming
{

/**
*Purpose:
*<p>    [class description]
*
*Features:
*<p> [Thread safety, exception safety, performance]
*    [if no pertinent write none ]
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/
class TestNamingInterfaceList : public TestUtils::Testable
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestNamingInterfaceList(CosNaming::NamingContext_ptr,test::TestObject_ptr)
                throw();

        /**
        * Purpose:
        * <p>  [Copy constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestNamingInterfaceList(const TestNamingInterfaceList& rhs)
                throw();


        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~TestNamingInterfaceList()
                throw();


        /**
        * Purpose:
        * <p> [ Assignement operator description if necessary]
        * 
        *@param
        *@return
        *@exception
        */ 
        TestNamingInterfaceList&
        operator=(const TestNamingInterfaceList& rhs)
                throw();


        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 


        /**
        *[Attribute description]
        */ 


    protected:

	    virtual void do_tests();

    private:

        CosNaming::NamingContext_var m_context;
        test::TestObject_var               m_test_object;

}; // End class TestNamingInterfaceList 

} // End namespace Naming
} // End namespace CommonSvcs
} // End namespace Cdmw
#endif // INCL_NAMINGANDREPOSITORY_TEST_NAMINGINTERFACE_LIST_HPP

