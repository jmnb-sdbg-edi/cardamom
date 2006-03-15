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

#ifndef _TEST_Q_101_HPP_
#define _TEST_Q_101_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CdmwTime.stub.hpp"
#include <functional>
/**
 * @brief Class to test ControllerUpdateAdmin method.
 */
class Test_Q_101 
:   public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( Test_Q_101 );
       CPPUNIT_TEST( on_set );     
    CPPUNIT_TEST_SUITE_END();

    Cdmw::clock::policy::ControllerUpdateAdmin_var 
        m_admin;

    CosClockService::PeriodicExecution::Controller_var
        m_controller;
    
    CosClockService::ControlledClock_var 
        m_controlledClock;
    
    CORBA::Object_ptr 
        m_obj;
        
    std::string 
        m_alias_name;
  
public:
    Test_Q_101();
    void setUp();
    void tearDown() ;
    void on_set ();
};


#endif /*_TEST_Q_101_HPP_*/
