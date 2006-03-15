/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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

#include "test/TestLocationPriorityStrategy.hpp"

#include "ftmonitoring/common/CommonTypeDefinitions.hpp"
#include "common/LocationPriorityStrategy.hpp"

using namespace Cdmw::FT::Supervision;
using namespace CdmwFT;

//  
// Test implementation
//
TestLocationPriorityStrategy::TestLocationPriorityStrategy(const std::string& name)
    : Testable(name)
{
}


TestLocationPriorityStrategy::~TestLocationPriorityStrategy()
{
}


void TestLocationPriorityStrategy::do_tests()
{    
    set_nbOfRequestedTestOK(8);

    try
    {
    	TEST_INFO("Init of test: Build of locations");
        char *locations[] = { "HostA", "HostB", "HostC" };
	MonitoringLibrary::Locations locSeq(5, 3, locations);
        TEST_CHECK(locSeq[2]=="HostC");
        
        TEST_INFO("Use LocationPriorityStrategy constructor with locations");
        LocationPriorityStrategy strategy(locSeq);
        LocationList locationList = strategy.get_locations();
        TEST_CHECK(locationList.size()==3);

        TEST_INFO("try to remove an unexisting location from the strategy");
        try
	{
          strategy.remove_location("HostD");
          TEST_FAILED();
        }
        catch(MonitoringLibrary::NotFound)
	{
          TEST_SUCCEED();
        }
        
        TEST_INFO("Remove existing location");
        strategy.remove_location("HostB");
        locationList = strategy.get_locations();
        TEST_CHECK(locationList.size()==2);

        TEST_INFO("try to add an existing location to the strategy");
        try
	{
          strategy.add_location("HostA");
          TEST_FAILED();
        }
        catch(MonitoringLibrary::AlreadyExists)
	{
          TEST_SUCCEED();
        }
        
        TEST_INFO("add a new location");
        strategy.add_location("HostD");
        locationList = strategy.get_locations();
        TEST_CHECK(locationList.size()==3);
        
        TEST_INFO("reset locations");
        strategy.reset_locations();
        locationList = strategy.get_locations();
        TEST_CHECK(locationList.empty());

        TEST_INFO("set locations");
        strategy.set_locations(locSeq);
        locationList = strategy.get_locations();
        TEST_CHECK(locationList.size()==3);

    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
     
}

