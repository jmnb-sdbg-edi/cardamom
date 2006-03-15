/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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
/**
 * @brief _implementation for ControllerUpdateAdminTest with cppUnit library.
 *
 * @author Francesca Annunziata<fannunziata@selex-si.com>, 
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

#include "testpolicies/ControllerUpdateAdminTest.hpp"
#include "clockservice/ControllerCancelAllPolicy.hpp"
#include "clockservice/ControllerUpdateAdmin_impl.hpp"

#include <string>
#include <iostream>
#include <sstream>

#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"


///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION(ControllerUpdateAdminTest);

///////////////////////////////////////////////////////////////////////////////

ControllerUpdateAdminTest::ControllerUpdateAdminTest()
{  
}

///////////////////////////////////////////////////////////////////////////////

void  
ControllerUpdateAdminTest::setUp()
{
    m_alias_name = "PolicyControllerUpdateAdmin";  
    Cdmw::TestUtils::CORBATestManager::instance()->add(m_alias_name);
    try
    {
        m_obj = Cdmw::TestUtils::CORBATestManager::instance()->get_object(m_alias_name);
    }
    catch(Cdmw::TestUtils::CORBATestManager::InvalidObject&)
    {
        CPPUNIT_FAIL("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }
    
    m_admin = Cdmw::clock::policy::ControllerUpdateAdmin::_narrow(m_obj);    
    CPPUNIT_ASSERT(!CORBA::is_nil(m_admin.in() ) );
}

///////////////////////////////////////////////////////////////////////////////

void  
ControllerUpdateAdminTest::tearDown()
{
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerUpdateAdminTest::cxx_register()
{
	Cdmw::clock::ControllerCancelAllPolicy* 
       cancelAllPolicy = new Cdmw::clock::ControllerCancelAllPolicy();
	Cdmw::clock::policy::ControllerUpdate_ptr 
        policy = cancelAllPolicy->_this();            
	m_admin->_cxx_register(Cdmw::clock::policy::USER_DEFINED_0,
                           policy);
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerUpdateAdminTest::already_present()
{
    Cdmw::clock::ControllerCancelAllPolicy* 
       cancelAllPolicy = new Cdmw::clock::ControllerCancelAllPolicy();
    Cdmw::clock::policy::ControllerUpdate_ptr 
        policy = cancelAllPolicy->_this();            
    m_admin->_cxx_register(Cdmw::clock::policy::USER_DEFINED_0,
                           policy);    
}

///////////////////////////////////////////////////////////////////////////////

void
ControllerUpdateAdminTest::unregister()
{
    m_admin->unregister(Cdmw::clock::policy::USER_DEFINED_0);    
}

///////////////////////////////////////////////////////////////////////////////
void
ControllerUpdateAdminTest::get_policy()
{
    Cdmw::clock::policy::ControllerUpdate_ptr 
        policy;
    policy = m_admin->get_policy(Cdmw::clock::policy::USER_DEFINED_0);
    CPPUNIT_ASSERT(!CORBA::is_nil(policy)); // just to avoid warning
}

// End Of File
///////////////////////////////////////////////////////////////////////////////
