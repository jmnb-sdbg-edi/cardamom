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


#ifndef INCL_NAMINGANDREPOSITORY_TESTOBJECT_IMPL_HPP 
#define INCL_NAMINGANDREPOSITORY_TESTOBJECT_IMPL_HPP 

#include "test_lib/TestObject.skel.hpp"

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
class TestObject_impl : virtual public POA_test::TestObject,
                        virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestObject_impl(PortableServer::POA_ptr)
                throw();


        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~TestObject_impl()
                throw();


        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */
	    CORBA::Boolean is_alive() throw (CORBA::SystemException); 
	    
	    inline PortableServer::POA_ptr _default_POA()
	    {
	        return PortableServer::POA::_duplicate(m_poa.in());
	    }


        /**
        *[Attribute description]
        */


    protected:


    private:
        PortableServer::POA_var m_poa;
        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestObject_impl()
                throw();
		
        /**
        * Purpose:
        * <p>  [Copy constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        TestObject_impl(const TestObject_impl& rhs)
                throw();
		
		/**
        * Purpose:
        * <p> [ Assignement operator description if necessary]
        * 
        *@param
        *@return
        *@exception
        */ 
        TestObject_impl&
        operator=(const TestObject_impl& rhs)
                throw();


}; // End class TestObject_impl 

} // End namespace Naming
} // End namespace CommonSvcs
} // End namespace Cdmw
#endif // INCL_NAMINGANDREPOSITORY_TESTOBJECT_IMPL_HPP

