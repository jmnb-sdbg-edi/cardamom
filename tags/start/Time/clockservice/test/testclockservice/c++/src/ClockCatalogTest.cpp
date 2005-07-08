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
 * @brief _implementation for ClockCatalogTest with cppUnit library.
 *
 * @author Lello Mele <lellomele@yahoo.com>, 
 * @author Fabrizio Morciano <fmorciano@amsjv.it>
 */

#include "Time/testclockservice/ClockCatalogTest.hpp"
#include "Time/clockservice/ClockCatalog_impl.hpp"
#include "ace/streams.h"
#include "ace/Read_Buffer.h"

#include <string>
#include <iostream>
#include <sstream>

#include "Time/testclockservice/CORBAManager.hpp"


///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION(ClockCatalogTest);

///////////////////////////////////////////////////////////////////////////////

ClockCatalogTest::ClockCatalogTest()
{  
}

///////////////////////////////////////////////////////////////////////////////

void  
ClockCatalogTest::setUp()
{
    alias_name_ = "ClockService";  
    CORBAManager::instance()->add(alias_name_);
    try
    {
        obj_ = CORBAManager::instance()->get_object(alias_name_);
    }
    catch(CORBAManager::InvalidObject&)
    {
        CPPUNIT_FAIL("CORBAManager::InvalidObject");
    }
    
    clock_catalog_ = CosClockService::ClockCatalog::_narrow(obj_);    
    CPPUNIT_ASSERT(!CORBA::is_nil(clock_catalog_.in() ) );
}

///////////////////////////////////////////////////////////////////////////////

void  
ClockCatalogTest::tearDown()
{
}

///////////////////////////////////////////////////////////////////////////////

int   
ClockCatalogTest::get_entries_count()
{
    CPPUNIT_ASSERT(!CORBA::is_nil(clock_catalog_.in()));
    ::CosClockService::ClockCatalog::ClockEntries_var 
        clockEntries = clock_catalog_->available_entries();
    return clockEntries->length();
}

///////////////////////////////////////////////////////////////////////////////

void  
ClockCatalogTest::empty_entry()
{ 
    CPPUNIT_ASSERT(!CORBA::is_nil(obj_) );
    // Narrow the IOR to a ClockCatalogTest object reference
    CPPUNIT_ASSERT(!CORBA::is_nil(clock_catalog_.in()) );
    
    bool exc = false;
    // check for UnknownEntry
    try
    {
        clock_catalog_->get_entry("");
    }
    catch( CosClockService::ClockCatalog::UnknownEntry& )
    {
        exc = true;
    }
    
    CPPUNIT_ASSERT(exc);
}

///////////////////////////////////////////////////////////////////////////////

void  
ClockCatalogTest::entry_management()
{
    CPPUNIT_ASSERT(!CORBA::is_nil(obj_) );
    // Narrow the IOR to a ClockCatalogTest object reference
    CPPUNIT_ASSERT(!CORBA::is_nil(clock_catalog_.in() ) );
    
    // variable to test exception raising
    bool exc = false;
    
    CosClockService::ClockCatalog::ClockEntry 
        entry;
    
    int 
        i, 
        length = get_entries_count();
    
    // entries to add 
    const int NEW_ENTRIES = 1000;
    const int MAX_ENTRIES = NEW_ENTRIES + length;  
    
    // add some entry
    for (i = length; i<MAX_ENTRIES ; ++i)
    {
        std::ostringstream oss;
        oss<<"Clock "<<i;
        // entry.subject = myClock::_nil();
        entry.name = CORBA::string_dup(oss.str().c_str());
        clock_catalog_->_cxx_register(entry);
    }
    
    // check number of new entry
    CPPUNIT_ASSERT( get_entries_count() ==  MAX_ENTRIES );
    
    CosClockService::ClockCatalog::ClockEntry *entryRef;
    
    // try to recover available clock
    i = MAX_ENTRIES;
    while( --i >= length )
    {
        std::ostringstream oss;
        oss<<"Clock "<<i;
        try
        {
            entryRef = clock_catalog_->get_entry(oss.str().c_str());
        }
        catch (::CosClockService::ClockCatalog::UnknownEntry&)
        {
            CPPUNIT_FAIL("ClockCatalog::UnknownEntry");
        }
    }
    
    // remove some entry from the catalog
    for (i = length; i<MAX_ENTRIES; i+=2)
    {
        std::ostringstream oss;
        oss<<"Clock "<<i;
        try
        {
            clock_catalog_->delete_entry(oss.str().c_str());
        }
        catch (::CosClockService::ClockCatalog::UnknownEntry&)
        {
            CPPUNIT_FAIL("ClockCatalog::UnknownEntry");
        }
    }
    
    CPPUNIT_ASSERT(get_entries_count() == (NEW_ENTRIES>>1) + length);
    
    // Search for all voices in the catalog
    i = MAX_ENTRIES;
    while( --i >= length  )
    {
        exc = false;
        std::ostringstream oss;
        oss<<"Clock "<<i;
        try
        {
            entryRef = clock_catalog_->get_entry(oss.str().c_str());
        }
        catch (::CosClockService::ClockCatalog::UnknownEntry&)
        {
            exc = true;
            if( (i+length)&1 )
                CPPUNIT_FAIL("ClockCatalog::UnknownEntry");
        }
        
        if( !((i+length)&1) )
            CPPUNIT_ASSERT( (exc == true) );
    }
    
    // clean entries for new test
    i = MAX_ENTRIES;
    while( --i >= length )
    {
        std::ostringstream oss;
        oss<<"Clock "<<i;
        try
        {
            clock_catalog_->delete_entry(oss.str().c_str());
        }
        catch(::CosClockService::ClockCatalog::UnknownEntry&)
        {
        }
    }
    
    CPPUNIT_ASSERT(get_entries_count() == length);
}

///////////////////////////////////////////////////////////////////////////////

void  
ClockCatalogTest::available()
{  
    // Return available entries in the sequence
    CPPUNIT_ASSERT(!CORBA::is_nil(clock_catalog_.in() ) );
    ::CosClockService::ClockCatalog::ClockEntries_var 
        clockEntries = clock_catalog_->available_entries();
    
    int length = clockEntries->length();
    // by default some entry is available
    CPPUNIT_ASSERT( length != 0 );

    /*
    for(int i = 0; i< length; ++i )
    {
        CosClockService::ClockCatalog::ClockEntry& ref = (*clockEntries)[i];
        std::string(CORBA::string_dup(ref.name));
    }
    */
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
