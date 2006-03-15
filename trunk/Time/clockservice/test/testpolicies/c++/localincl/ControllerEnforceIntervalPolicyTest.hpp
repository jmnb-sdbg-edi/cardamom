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

#ifndef CONTROLLERENFORCEINTERVALPOLICYTEST_HPP
#define CONTROLLERENFORCEINTERVALPOLICYTEST_HPP

/**
 * @author Francesca Annunziata <fannunziata@selex-si.com>
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CdmwTime.stub.hpp"
#include <functional>

struct  controller_functor 
    : std::unary_function<void,void> 
{
    Cdmw::clock::ControlledExecutor_ptr m_controller;
    
    controller_functor(Cdmw::clock::ControlledExecutor_ptr ref)
        : m_controller(ref)
    {
    }
    
    virtual void operator()() = 0;
    
    Cdmw::clock::ControlledExecutor_ptr getExecutor()
    {
        return m_controller;
    }
};

class ControllerEnforceIntervalPolicyTest 
    : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ControllerEnforceIntervalPolicyTest );
      CPPUNIT_TEST( check_policy );
      CPPUNIT_TEST( PCR_0450_on_set_rate );     
      CPPUNIT_TEST( PCR_0450_on_set );
      CPPUNIT_TEST( PCR_0450_on_pause_resume );
      // CPPUNIT_TEST( PCR_0450_on_mixed_controls );
      CPPUNIT_TEST( PCR_0450_on_terminate );
    CPPUNIT_TEST_SUITE_END();

protected:

    std::string                                        m_alias_name;
    CosClockService::ControlledClock_var               m_controlledClock;
    CosClockService::PeriodicExecution::Controller_var m_controller;
    Cdmw::clock::policy::ControllerUpdateAdmin_var     m_admin;
    CORBA::Object_var                                  m_obj;
 
public:

    ControllerEnforceIntervalPolicyTest();

    void setUp();
    void tearDown() ;

    void check_policy();
    void PCR_0450_on_set_rate();
    void PCR_0450_on_set();
    void PCR_0450_on_pause_resume();
    // void PCR_0450_on_mixed_controls();
    void PCR_0450_on_terminate();
};

#endif // CONTROLLERENFORCEINTERVALPOLICYTEST_HPP

