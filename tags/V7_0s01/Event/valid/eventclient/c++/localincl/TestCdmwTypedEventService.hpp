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

#ifndef INCL_EVENT_TEST_CDMW_TYPED_EVENT_SERVICE_HPP
#define INCL_EVENT_TEST_CDMW_TYPED_EVENT_SERVICE_HPP

#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"



namespace Cdmw
{
namespace Event
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
class TestCdmwTypedEventService : public TestUtils::Testable
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestCdmwTypedEventService(CORBA::ORB_ptr orb,
                                   CosNaming::NamingContext_ptr nc,
                                   int test_nb)
                throw();

        /**
        * Purpose:
        * <p>  [Copy constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestCdmwTypedEventService(const TestCdmwTypedEventService& rhs)
                throw();


        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~TestCdmwTypedEventService()
                throw();


        /**
        * Purpose:
        * <p> [ Assignement operator description if necessary]
        * 
        *@param
        *@return
        *@exception
        */ 
        TestCdmwTypedEventService&
        operator=(const TestCdmwTypedEventService& rhs)
                throw();

        /**
        *[Attribute description]
        */ 


    protected:

	    virtual void do_tests();

    private:
       CORBA::ORB_var               m_orb;
       CosNaming::NamingContext_var m_namingcontext;
       int                          m_test_nb;

}; // End class TestCdmwTypedEventService

}; // End namespace Event
}; // End namespace Cdmw
#endif // INCL_EVENT_TEST_CDMW_TYPED_EVENT_SERVICE_HPP

