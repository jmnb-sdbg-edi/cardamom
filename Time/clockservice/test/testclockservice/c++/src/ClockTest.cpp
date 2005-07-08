/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

/**
* @brief _implementation for ClockTest with cppUnit library.
*
*
* @author Lello Mele <lellomele@yahoo.com>, 
* @author Fabrizio Morciano <fmorciano@amsjv.it>
*/

#include "Time/testclockservice/PerformanceManager.hpp"
#include <cppunit/extensions/HelperMacros.h>
#include "Time/testclockservice/ClockTest.hpp"
#include "Time/clockservice/Clock_impl.hpp"
#include "Time/clockservice/config.hpp"
#include "ace/streams.h"
#include "ace/Read_Buffer.h"
#include <string>
#include <sstream>
#include <iostream>
#include "Time/testclockservice/CORBAManager.hpp"

using namespace CosClockService;
using namespace CosPropertyService;
using namespace Cdmw::clock;

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION( ClockTest );

///////////////////////////////////////////////////////////////////////////////

void
ClockTest::setUp()
{
    alias_name_ = "LocalClock";  
    CORBAManager::instance()->add(alias_name_);
    try
    {
        obj_ = CORBAManager::instance()->get_object(alias_name_);
    }
    catch(CORBAManager::InvalidObject&)
    {
        CPPUNIT_FAIL("CORBAManager::InvalidObject");
    }
    
    clock_ = CosClockService::Clock::_narrow(obj_);
    CPPUNIT_ASSERT(!CORBA::is_nil(clock_.in() ) );
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
        clock_time = clock_->current_time();
    
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
    timeval   tv;
    
    if( gettimeofday( &tv, 0 ) !=0 )
        CPPUNIT_FAIL("Error in retrieving system time");
    
    tmp_time = (TimeBase::TimeT)(tv.tv_sec ) * 10000000 +
        (TimeBase::TimeT)(tv.tv_usec) * 10 ;
    
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
    PERFORMANCE_ASSERT(tmp_time >= clock_time);
    
    TimeBase::TimeT diff = tmp_time - clock_time;  
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
                clock_pr = clock_->properties();
            
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
            clock_pr = clock_->properties();
        
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
                    CORBA::ULong    long_val;        
                    properties [i].property_value >>= long_val;
                    CPPUNIT_ASSERT( long_val == Cdmw::clock::RESOLUTION );
                }            
                
                if(name == "Width")
                {
                    CORBA::Long    long_val;        
                    properties [i].property_value >>= long_val;
                    CPPUNIT_ASSERT( long_val == 64 );
                }            
                
                if(name == "Stability_Description")
                {
                    const char *  string_val;        
                    properties [i].property_value >>= string_val;
                    CPPUNIT_ASSERT( std::string(string_val) == "Not Available" );
                }                  
                
                if(name == "Coordination")
                {
                    CORBA::Long    long_val;        
                    properties [i].property_value >>= long_val;
                    CPPUNIT_ASSERT( long_val == 0 );
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
