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
 * @author Raffaele Mele <rmele@progesi.it>, 
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

#include "Time/clockservice/test/testcommon/ClockTest.hpp"
#include "Time/clockservice/test/testcommon/PerformanceManager.hpp"
#include <cppunit/extensions/HelperMacros.h>
#include "clockservice/Clock_impl.hpp"
#include "Time/clockservice/config.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( ClockTest );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ClockTest, "testclockservice");

///////////////////////////////////////////////////////////////////////////////

void
ClockTest::setUp()
{
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
    
    m_clock = CosClockService::Clock::_narrow(m_obj);
    CPPUNIT_ASSERT(!CORBA::is_nil(m_clock.in() ) );
}

///////////////////////////////////////////////////////////////////////////////

void
ClockTest::tearDown()
{
}

///////////////////////////////////////////////////////////////////////////////

ClockTest::ClockTest()
{  
}

///////////////////////////////////////////////////////////////////////////////

void
ClockTest::current_time()
{  
    TimeBase::TimeT 
        clock_time = m_clock->current_time();
    
    // update base of time for windows compatibility    
    clock_time -= TIME_BASE;
    
    TimeBase::TimeT 
        tmp_time;
    
#ifdef  WIN32
    
    /*  
        SYSTEMTIME 
        system_time;
        
        // convert evot time to local time
        if( FileTimeToSystemTime( (FILETIME*)&clock_time, &system_time ) == 0 )
          CPPUNIT_FAIL("Error in coverting EVoT time to system time");
          
        SYSTEMTIME 
            local_time;
            
        // retrieve system time of current machine
        GetLocalTime( &local_time );
              
        // converion vs TimeBase
        if( SystemTimeToFileTime( &local_time, (FILETIME*)&tmp_time ) == 0 )
            CPPUNIT_FAIL("Error in retrieving system time");
    */
    
    TimeBase::TimeT 
        atime_;
    
    GetSystemTimeAsFileTime( (FILETIME*)&atime_ );
    
    if( FileTimeToLocalFileTime( 
        (FILETIME*)&atime_, 
        (FILETIME*)&tmp_time ) == 0 )
        CPPUNIT_FAIL("Error in retrieving system time");
    
#else
    struct timeval   tv;
    struct timezone  tz;
    
    if( gettimeofday( &tv, &tz ) !=0 )
        CPPUNIT_FAIL("Error in retrieving system time");
    
    tmp_time = 
        (TimeBase::TimeT)(tv.tv_sec ) * 10000000 +
        (TimeBase::TimeT)(tv.tv_usec) * 10 ;
 //  std::cerr<<"tmp_time: "<<tmp_time<<std::endl;    
	// all computation are done for East
	TimeBase::TimeT 
		offset_in_TimeT = (TimeBase::TimeT)(abs(tz.tz_minuteswest))*600000000;
		
	// values are computed for West then it is necessary to
	// verify the sign	
	if(tz.tz_minuteswest > 0)
		tmp_time -= offset_in_TimeT;  // West
	else
		tmp_time += offset_in_TimeT;  // East

       // std::cerr<<"offset_in_TimeT: "<<offset_in_TimeT<<std::endl;
   //std::cerr<<"tmp_time: "<<tmp_time<<std::endl;

#endif
    
    /*
        typedef   unsigned long ulong;
        
        cout<<endl;
        ulong  test = (ulong)(0x00000000FFFFFFFF & tmp_time );
        cout<<"Diff low is :"<<test<<endl;
        test = (ulong)(0xFFFFFFFF00000000 & tmp_time );
        cout<<"Diff up  is :"<<test<<endl;
          
        test = (ulong)(0x00000000FFFFFFFF & clock_time );
        cout<<"Diff low is :"<<test<<endl;
        test = (ulong)(0xFFFFFFFF00000000 & clock_time );
        cout<<"Diff up  is :"<<test<<endl;
    */    
 //   std::cerr<<"tmp_time>= clock_time: "<<tmp_time<<">="<<clock_time<<std::endl;
    PERFORMANCE_ASSERT(tmp_time >= clock_time);
    
    TimeBase::TimeT diff = tmp_time - clock_time;  
 //   std::cerr<<"Diff: "<<diff<<std::endl;
    TimeBase::TimeT tol = (TimeBase::TimeT)100000; // tolerance of 10 usec
    
    /*
        test = (ulong)(0x00000000FFFFFFFF & diff );
        cout<<"Diff low is :"<<test<<endl;
        test = (ulong)(0xFFFFFFFF00000000 & diff );
        cout<<"Diff up  is :"<<test<<endl;
    */
    
    PERFORMANCE_ASSERT(diff < tol );
}

///////////////////////////////////////////////////////////////////////////////

void
ClockTest::properties()
{
    // properties to test
    PropertyNames names;
    names.length(5);
    names[0] = CORBA::string_dup("Resolution");  
    names[1] = CORBA::string_dup("Width");
    names[2] = CORBA::string_dup("Stability_Description");
    names[3] = CORBA::string_dup("Coordination");
    names[4] = CORBA::string_dup("TimeScale");
    
    
    // check double get
    const int max_count = 3;
    int counter = 0;
    try
    {
        while( ++counter <= max_count )
        {
            // recover LocalClock properties
            PropertySet_var
                clock_pr = m_clock->properties();
            
            Properties *properties_ptr = 0;
            Properties_out properties_out(properties_ptr);
            
            CORBA::Boolean 
                ret =  clock_pr->get_properties(names, properties_out);
            
            // properties are available  
            CPPUNIT_ASSERT( ret != false );
            
            Properties_var properties = properties_out.ptr ();
            if (properties.ptr () != 0)
            {
                CORBA::ULong len = properties->length ();
                
                for (CORBA::ULong i = 0;  i < len; i++)
                {
                    // just to recover something
                    std::string name = CORBA::string_dup(
                        properties [i].property_name.in ());
                }
            }
        }
    }
    catch(...)
    {
        std::ostringstream oss;
        oss<<"Not valid properties after: "<<counter<<" times!!";
        CPPUNIT_FAIL( oss.str() );
    }
    
    try
    {
        // recover LocalClock properties
        PropertySet_var
            clock_pr = m_clock->properties();
        
        // variable to store properties 
        Properties *properties_ptr = 0;
        Properties_out properties_out (properties_ptr);
        
        CORBA::Boolean 
            ret =  clock_pr->get_properties ( names, properties_out );
        
        Properties_var properties = properties_out.ptr ();
        
        // properties are available  
        CPPUNIT_ASSERT( ret != false );
        
        if (properties.ptr () != 0)
        {
            CORBA::ULong len = properties->length ();
            
            for (CORBA::ULong i = 0;  i < len; i++)
            {
                //std::cerr<<std::endl<<"Prop: "<<properties [i].property_name.in ()<<std::endl;
                //std::string name = /*CORBA::string_dup( */properties [i].property_name.in ();
                //std::cerr<<std::endl<<"Prop: "<<name<<std::endl;
                
                std::string name = CORBA::string_dup( properties [i].property_name.in () );
                if(name == "Resolution")
                {
                    CORBA::ULong   value;        
                    properties [i].property_value >>= value;
                    CPPUNIT_ASSERT_EQUAL( (CORBA::ULong)Cdmw::clock::RESOLUTION , value );
                }            
                
                if(name == "Width")
                {
                    CORBA::Short  value;        
                    properties [i].property_value >>= value;
                    CPPUNIT_ASSERT_EQUAL( (CORBA::Short)64, value);
                }            
                
                if(name == "Stability_Description")
                {
                    const char *  string_val;        
                    properties [i].property_value >>= string_val;
                    CPPUNIT_ASSERT_EQUAL(std::string("Not Available"), std::string(string_val));
                }                  
                
                if(name == "Coordination")
                {
                    CORBA::Short value;        
                    properties [i].property_value >>= value;
                    CPPUNIT_ASSERT_EQUAL( (CORBA::Short)0, value );
                }

                if(name == "TimeScale")
                {
                    CORBA::Short value;
                    properties [i].property_value >>= value;
                    CPPUNIT_ASSERT_EQUAL( (CORBA::Short)CosClockService::ClockProperty::Local_, value );
                }
            }
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

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
