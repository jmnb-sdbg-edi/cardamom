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


#ifndef INCL_EVENT_TEST_TYPED_EVENT_CHANNEL_FACTORY_HPP
#define INCL_EVENT_TEST_TYPED_EVENT_CHANNEL_FACTORY_HPP

#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "Event/idllib/CdmwEvent.stub.hpp"

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
class TestTypedEventChannelFactory : public TestUtils::Testable
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestTypedEventChannelFactory(CdmwEvent::TypedEventChannelFactory_ptr factory)
                throw();

        /**
        * Purpose:
        * <p>  [Copy constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestTypedEventChannelFactory(const TestTypedEventChannelFactory& rhs)
                throw();


        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~TestTypedEventChannelFactory()
                throw();


        /**
        * Purpose:
        * <p> [ Assignement operator description if necessary]
        * 
        *@param
        *@return
        *@exception
        */ 
        TestTypedEventChannelFactory&
        operator=(const TestTypedEventChannelFactory& rhs)
                throw();


        /**
        * Purpose:
        * <p>
        * init and start next test
        * 
        *@param
        *@return
        *@exception
        */ 
        void 
        next_test();

        /**
        *[Attribute description]
        */ 


    protected:

        virtual void do_tests();

    private:
         CdmwEvent::TypedEventChannelFactory_var m_factory;
         int                                      m_test_number; // indicate the current test number

}; // End class TestTypedEventChannelFactory

}; // End namespace Event
}; // End namespace Cdmw
#endif // INCL_EVENT_TEST_TYPED_EVENT_CHANNEL_FACTORY_HPP


