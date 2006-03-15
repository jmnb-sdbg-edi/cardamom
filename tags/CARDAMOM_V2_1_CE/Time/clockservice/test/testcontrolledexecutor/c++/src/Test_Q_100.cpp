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
 *
 * @author Raffaele Mele <rmele@progesi.it>
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

#include <string>
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstring>
#include <limits>

#include "testcontrolledexecutor/Test_Q_100.hpp"
#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"

#include "clockservice/ControlledClock_impl.hpp"
#include "Time/clockservice/config.hpp"
#include "Time/clockservice/Macro.hpp"
#include "Time/clockservice/Util.hpp"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( Test_Q_100 );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Test_Q_100, "test_Q_100");

///////////////////////////////////////////////////////////////////////////////

template <class T>
bool compare(const T a, const T b)
{
    return (a == b);
}

///////////////////////////////////////////////////////////////////////////////

template <>
bool compare(const char* a, const char* b)
{
    return (strcmp(a, b) == 0);
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
    template <class T> 
    void test_property(const std::string& property_name,
                       Clock_ptr          the_clock,
                       T                  expected_value)
    {
        try
        {
            // recover LocalClock properties
            PropertySet_var clock_pr = the_clock->properties();
            
            // variable to store properties 
            Properties *properties_ptr = 0;
            Properties_out properties_out(properties_ptr);
            try 
            {
                T value;        
                CORBA::Any_ptr any;
                any = clock_pr->get_property_value(property_name.c_str()); 
                (*any) >>= value;
                // std::cerr << std::endl;
                // std::cerr << "Expexted Value is: " << expected_value << std::endl;
                // std::cerr << "         Value is: " << value << std::endl;
                CPPUNIT_ASSERT( compare(expected_value, value ) );
             }
            catch( PropertyNotFound  )
            {
                CPPUNIT_FAIL("Property Not Found");
            }
            catch ( InvalidPropertyName ) 
            {
                CPPUNIT_FAIL("Invalid Property Name");
            }    
        }
        catch(std::exception& e)
        {
            std::cerr<<e.what()<<std::endl;
            CPPUNIT_FAIL(e.what());
        }
        catch (CORBA::SystemException& e)
        {
            std::cerr<< e <<std::endl;
            CPPUNIT_FAIL(e._name());
        }
        catch(...)
        {
            CPPUNIT_FAIL("Unknown exception caught!");
        }        
    }       
}

///////////////////////////////////////////////////////////////////////////////

void
Test_Q_100::setUp()
{
    m_clockType1 =  "ControlledClock1" ;
    m_clockType2 =  "ControlledClock2" ;
    m_alias_name = "ClockService";

    Cdmw::TestUtils::CORBATestManager::instance()->add(m_alias_name);
    try
    {
        m_obj = Cdmw::TestUtils::CORBATestManager::instance()->get_object(m_alias_name);
    }
    catch(Cdmw::TestUtils::CORBATestManager::InvalidObject&)
    {
        CPPUNIT_FAIL("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }

    m_clockCatalog = ClockCatalog::_narrow(m_obj);

    if (CORBA::is_nil(m_clockCatalog.in())) {
        CPPUNIT_FAIL("testControlledClock:>> Unable to resolve ClockCatalog");
    }

    ClockCatalog::ClockEntry_var clockEntry =
        m_clockCatalog->get_entry(m_clockType1.c_str());

    m_controlledClock1 =
        ControlledClock::_narrow(clockEntry->subject.in());
    CPPUNIT_ASSERT(!CORBA::is_nil(m_controlledClock1.in() ) );

    clockEntry = m_clockCatalog->get_entry(m_clockType2.c_str());

    m_controlledClock2 =
        ControlledClock::_narrow(clockEntry->subject.in());
    CPPUNIT_ASSERT(!CORBA::is_nil(m_controlledClock2.in() ) );


    m_alias_name = "LocalClock";  
    Cdmw::TestUtils::CORBATestManager::instance()->add(m_alias_name);
    try
    {
        m_obj = Cdmw::TestUtils::CORBATestManager::instance()->get_object(m_alias_name);
    }
    catch(Cdmw::TestUtils::CORBATestManager::InvalidObject&)
    {
        CPPUNIT_FAIL("Cdmw::TestUtils::CORBATestManager::InvalidObject");
    }

    m_localClock = Clock::_narrow(m_obj);
    CPPUNIT_ASSERT(!CORBA::is_nil(m_localClock.in() ) );
}

///////////////////////////////////////////////////////////////////////////////

void
Test_Q_100::tearDown()
{
    try { m_controlledClock1->set_rate(1.0);}
    catch(...){}
    try { m_controlledClock1->set(Cdmw::clock::compute_current_time()); }
    catch(...) {}
    try { m_controlledClock2->set_rate(1.0);}
    catch(...){}
    try { m_controlledClock2->set(Cdmw::clock::compute_current_time()); }
    catch(...) {}
}

///////////////////////////////////////////////////////////////////////////////

Test_Q_100::Test_Q_100()
{  

}

///////////////////////////////////////////////////////////////////////////////

void Test_Q_100::resolution ()
{
    test_property("Resolution",
                  m_localClock.in(),
                  (CORBA::ULong)Cdmw::clock::RESOLUTION);
    
    test_property("Resolution",
                  m_controlledClock1.in(),
                  (CORBA::ULong)Cdmw::clock::RESOLUTION);
                    
    test_property("Resolution",
                  m_controlledClock2.in(),
                  (CORBA::ULong)Cdmw::clock::RESOLUTION);       
}

///////////////////////////////////////////////////////////////////////////////

void Test_Q_100::precision ()
{
    test_property("Precision",
                  m_localClock.in(),
                  (CORBA::Short)24);
    
    test_property("Precision",
                  m_controlledClock1.in(),
                  (CORBA::Short)24);
                    
    test_property("Precision",
                  m_controlledClock2.in(),
                  (CORBA::Short)24);
}

///////////////////////////////////////////////////////////////////////////////

void Test_Q_100::readout_bits ()
{
    test_property("Width",
                  m_localClock.in(),
                  (CORBA::Short)64);
    
    test_property("Width",
                  m_controlledClock1.in(),
                  (CORBA::Short)64);
                    
    test_property("Width",
                  m_controlledClock2.in(),
                  (CORBA::Short)64);
}

///////////////////////////////////////////////////////////////////////////////

void Test_Q_100::coordination_status ()
{
    test_property("Coordination",
                  m_localClock.in(),
                  (CORBA::Short)ClockProperty::Uncoordinated);
    
    test_property("Coordination",
                  m_controlledClock1.in(),
                  (CORBA::Short)ClockProperty::Uncoordinated);
                    
    test_property("Coordination",
                  m_controlledClock2.in(),
                  (CORBA::Short)ClockProperty::Uncoordinated);       
}

///////////////////////////////////////////////////////////////////////////////

void Test_Q_100::coordination_stability ()
{
    const char *expected_string = "Not Available";
    test_property("Stability_Description",
                  m_localClock.in(),
                  expected_string);
    
    test_property("Stability_Description",
                  m_controlledClock1.in(),
                  expected_string);
                    
    test_property("Stability_Description",
                  m_controlledClock2.in(),
                  expected_string);
}    

///////////////////////////////////////////////////////////////////////////////
