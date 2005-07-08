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

#ifndef CLOCKCATALOGTEST_HPP
#define CLOCKCATALOGTEST_HPP
/**
 * @file
 * @brief Clock Catalog unit test case
 * 
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 * @author Lello Mele <lellomele@yahoo.com>
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CosClockService.skel.hpp"

/**
 * @brief Class to test ClockCatalog method.
 */
class ClockCatalogTest :
public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ClockCatalogTest );
        CPPUNIT_TEST( available );    
        CPPUNIT_TEST( empty_entry );
        CPPUNIT_TEST( entry_management );
    CPPUNIT_TEST_SUITE_END( );

    CosClockService::ClockCatalog_var clock_catalog_;
    CORBA::Object_ptr obj_;
    std::string alias_name_;
  
public:
    ClockCatalogTest();
    void  setUp();
    void  tearDown() ;
    int   get_entries_count();
  
    /**
     * @brief test available Catalog Entries.
     *
     */
    void  available();

    /**
     * @brief test Catalog entries creation, search and deletion.
     *
     */
    void  entry_management();

    /**
     * @brief test Catalog readind of empty entries.
     *
     */
    void  empty_entry();
};

#endif // CLOCKCATALOGTEST_HPP

