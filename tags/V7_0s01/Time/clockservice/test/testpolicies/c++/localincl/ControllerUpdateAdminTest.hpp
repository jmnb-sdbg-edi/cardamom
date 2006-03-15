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

#ifndef CONTROLLERUPDATEADMINTEST_HPP
#define CONTROLLERUPDATEADMINTEST_HPP
/**
 * 
 * @author Francesca Annunziata <fannunziata@selex-si.com>
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CdmwTime.stub.hpp"

/**
 * @brief Class to test ControllerUpdateAdmin method.
 */
class ControllerUpdateAdminTest 
:   public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ControllerUpdateAdminTest );
       CPPUNIT_TEST( cxx_register );
       CPPUNIT_TEST_EXCEPTION( already_present,\
                               Cdmw::clock::policy::ControllerUpdateAdmin::AlreadyExist );
       CPPUNIT_TEST( get_policy );
       CPPUNIT_TEST( unregister );
    CPPUNIT_TEST_SUITE_END();

    Cdmw::clock::policy::ControllerUpdateAdmin_var m_admin;
    CORBA::Object_ptr m_obj;
    std::string m_alias_name;
  
public:
    ControllerUpdateAdminTest();
    void  setUp();
    void  tearDown() ;
    void   cxx_register();
    void   already_present();
    void   unregister();
    void   get_policy();
};

#endif // CONTROLLERUPDATEADMINTEST_HPP

