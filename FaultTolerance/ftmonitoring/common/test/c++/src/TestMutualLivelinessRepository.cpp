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

#include "test/TestMutualLivelinessRepository.hpp"

#include "ftmonitoring/common/CommonTypeDefinitions.hpp"
#include "common/MutualLivelinessRepository.hpp"

using namespace Cdmw::FT::Supervision;
using namespace CdmwFT;

//  
// Test implementation
//
TestMutualLivelinessRepository::TestMutualLivelinessRepository(const std::string& name)
    : Testable(name)
{
}


TestMutualLivelinessRepository::~TestMutualLivelinessRepository()
{
}


void TestMutualLivelinessRepository::do_tests()
{    
    set_nbOfRequestedTestOK(13);

    try
    {
    	TEST_INFO("Init of test: Build of locations");
        char *locations[] = { "HostA", "HostB", "HostC" };
	MonitoringLibrary::Locations locSeq(5, 3, locations);
        TEST_CHECK(locSeq[2]=="HostC");
        
        TEST_INFO("Try to call MutualLivelinessRepository constructor with unimplemented strategy");
        try
	{
          MutualLivelinessRepository mutualLiveliness(CPU_LOAD, locSeq);
          TEST_FAILED();
        }
        catch(CORBA::NO_IMPLEMENT)
	{
          TEST_SUCCEED();
        }
        
        TEST_INFO("call MutualLivelinessRepository constructor with strategy and locations");
        MutualLivelinessRepository mutualLiveliness(HOST_PRIORITY, locSeq);
        TEST_SUCCEED();

        LocationList livLocations;
        
        TEST_INFO("Fill the liveliness map");
        mutualLiveliness.update_liveliness("HostA", MonitoringLibrary::CANDIDATURE, 0);
        mutualLiveliness.update_liveliness("HostD", MonitoringLibrary::CANDIDATURE, 0);
        TEST_SUCCEED();

        TEST_INFO("Check no faulty slave given at the first get_faultySlaves call");
        mutualLiveliness.get_faultySlaves(livLocations, 1);
        TEST_CHECK(livLocations.size()==0);

        TEST_INFO("Check 2 faulty slaves given at the second get_faultySlaves call");
        TEST_INFO("(because a reset occurs at the first time)");
        mutualLiveliness.get_faultySlaves(livLocations, 1);
        TEST_CHECK(livLocations.size()==2);

        TEST_INFO("Check no faulty slave given at the third get_faultySlaves call");
        TEST_INFO("(because UNKNOWNs erased at the second time)");
        mutualLiveliness.get_faultySlaves(livLocations, 1);
        TEST_CHECK(livLocations.size()==0);
        
        TEST_INFO("Fill the liveliness map once more");
        mutualLiveliness.update_liveliness("HostA", MonitoringLibrary::ACKNOWLEDGEMENT, 0);
        mutualLiveliness.update_liveliness("HostB", MonitoringLibrary::ACKNOWLEDGEMENT, 0);
        mutualLiveliness.update_liveliness("HostC", MonitoringLibrary::CANDIDATURE, 0);
        TEST_SUCCEED();
        
        TEST_INFO("test get_masterCandidate: check candidature");
        MonitoringLibrary::Location master_loc = mutualLiveliness.get_masterCandidate();
        TEST_CHECK(strcmp(master_loc, "HostC")==0);

        TEST_INFO("Add a liveliness entry which is not in the strategy");
        mutualLiveliness.update_liveliness("HostD", MonitoringLibrary::CANDIDATURE, 0);
        TEST_INFO("Unset the HostC candidature");
        mutualLiveliness.update_liveliness("HostC", MonitoringLibrary::ACKNOWLEDGEMENT, 0);
        
        TEST_INFO("try to get_masterCandidate: check no candidature because Host not in strategy");
        try
	{
          master_loc = mutualLiveliness.get_masterCandidate();
          TEST_FAILED();
        }
        catch(Cdmw::InternalErrorException)
	{
          TEST_SUCCEED();
        }

        TEST_INFO("try reset_liveliness on unknown location");
        try
	{
          mutualLiveliness.reset_liveliness("HostE");
          TEST_FAILED();
        }
        catch(MonitoringLibrary::NotFound)
	{
          TEST_SUCCEED();
        }
        
        
        TEST_INFO("reset_liveliness of a particular location");
        mutualLiveliness.reset_liveliness("HostD");
        mutualLiveliness.get_faultySlaves(livLocations, 1);
        TEST_CHECK(livLocations.size()==1);
        
        TEST_INFO("reset all livelinesses");
        mutualLiveliness.reset_livelinesses();
        mutualLiveliness.get_faultySlaves(livLocations, 1);
        TEST_CHECK(livLocations.size()==3);
	
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
     
}

