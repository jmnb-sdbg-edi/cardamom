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
 * @brief implementation for TestLBInit with cppUnit library.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 *
 */

// Standard Files
#include <string>

// Cdmw Files
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
#include <testlbinit/CORBAManager.hpp>
#include <testlbinit/TestUtils.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include "LoadBalancing/lbcommon/LBStrategyRegistry.hpp"
#include "LoadBalancing/lbcommon/LBConfiguration.hpp"
#include <iostream>
using namespace std;

using namespace Cdmw;


#define ECHO_HEADER() \
    "[Debug] "

#define ECHO_ERROR(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)


#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment ;\
} while(0)

#endif

int main( int argc, char* argv[] )
{
    try
    {
        char** ORB_init_argv = 0;
	char** ORB_init_argv_err = 0;
        int ORB_init_argc = argc;
        std::string str_opt( "" );
	std::string str_opt_err( "" );

        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";
        str_opt += std::string( "--LBInterceptorSide=Client");
        for (int i=0; i<argc; i++)
            str_opt_err += std::string( argv[i] ) + " ";
        str_opt_err += std::string( "--LBInterceptorSide");
        
	ORB_init_argc += 1;
        ORB_init_argv = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings( str_opt ) );
	ORB_init_argv_err = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings( str_opt_err ) );
 
        
	DEBUG_DUMP("   Try to start LB Service without the --LBInterceptorSide option (CORBA::INTERNAL Exception should be throw)   ");	
	CPPUNIT_ASSERT_THROW(Cdmw::LB::LBServiceInit::Init(argc, argv), CORBA::INTERNAL);
        
	DEBUG_DUMP("   Try to start LB Service without override LB Policy (No Exception must be throw)   ");
        CPPUNIT_ASSERT_NO_THROW(Cdmw::LB::LBServiceInit::Init(ORB_init_argc, ORB_init_argv));
        
        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();

	
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

	DEBUG_DUMP("   Try to retrieve the LBStrategyRegistry from LBConfiguration  ");
	Cdmw::LB::LBStrategyRegistry* registry = Cdmw::LB::LBConfiguration::GetLbStrategyRegistry();
	
	DEBUG_DUMP("   Check if the RoundRobin strategy is defined in the LBStrategyRegistry...   ");
	CPPUNIT_ASSERT_NO_THROW(registry->get_lb_policy_factory("RoundRobin"));
	DEBUG_DUMP("...OK");

	DEBUG_DUMP("   Check if the Random strategy is defined in the LBStrategyRegistry...   ");
	CPPUNIT_ASSERT_NO_THROW(registry->get_lb_policy_factory("Random"));
	DEBUG_DUMP("...OK");

	DEBUG_DUMP("   Check if the UserDefined strategy is defined in the LBStrategyRegistry...it should be raised a StrategyNotFound Exception   ");
	CPPUNIT_ASSERT_THROW(registry->get_lb_policy_factory("UserDefined"), CdmwLB::StrategyNotFound);
	DEBUG_DUMP("...OK");

	CdmwLBInit::HelloInterface_var hello_group = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello_group_default");

        DEBUG_DUMP("   Try to call hello on Hello Object Group (Default=RoundRobin policy must be used)   ");
        hello_group->hello();
        for(int i = 0; i<16; i++)
            {
                CPPUNIT_ASSERT_NO_THROW(hello_group->hello());
            }

        DEBUG_DUMP("   Try to call hello on Hello Object on host 1(The request must be filtered through the client interceptor)   ");
        CdmwLBInit::HelloInterface_var hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello1");
        CPPUNIT_ASSERT_NO_THROW(hello_object->hello());

	delete registry;
        }
    catch( const CORBA::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << endl;
        return -1;
    }
    catch( const Cdmw::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << endl;
        return -1;
    }
    catch(...)
    {
        cerr << "Unexpected exception" << endl;
        return -1;
    }
}
    
