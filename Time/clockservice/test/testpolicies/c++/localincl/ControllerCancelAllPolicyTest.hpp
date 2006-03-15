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

#ifndef CONTROLLERCANCELALLPOLICYTEST_HPP
#define CONTROLLERCANCELALLPOLICYTEST_HPP
/**
 * 
 * @author Francesca Annunziata <fannunziata@selex-si.com>
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CdmwTime.stub.hpp"
#include <functional>

struct  controller_functor 
: std::unary_function<void,void> 
{
    Cdmw::clock::ControlledExecutor_ptr
        m_controller;
    
    controller_functor(Cdmw::clock::ControlledExecutor_ptr ref)
        :   m_controller(ref)
    {
    }
    
    virtual void 
    operator()() = 0;
    
    Cdmw::clock::ControlledExecutor_ptr 
    getExecutor()
    {
        return m_controller;
    }
};

/**
 * @brief Class to test ControllerUpdateAdmin method.
 */
class ControllerCancelAllPolicyTest 
:   public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ControllerCancelAllPolicyTest );
       CPPUNIT_TEST( on_set );
       CPPUNIT_TEST( on_set_rate );
       CPPUNIT_TEST( on_pause );
       CPPUNIT_TEST( on_terminate );
    CPPUNIT_TEST_SUITE_END();

protected:
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
  
private:   
    void checkPolicyForMethod(controller_functor&);    
    
public:
    ControllerCancelAllPolicyTest();
    void setUp();
    void tearDown() ;
    void on_set ();
    void on_set_rate ();
    void on_pause ();
    //  void on_resume ();
    void on_terminate ();
};

#endif // CONTROLLERCANCELALLPOLICYTEST_HPP

