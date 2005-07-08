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
#include "TestUtils.hpp"
#include <LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp>
#include <LoadBalancing/idllib/PortableGroup.stub.hpp>
#include "UserDefinedStrategyFactory_impl.hpp"
#include <iostream>
using namespace std;

using namespace Cdmw;


#define ECHO_HEADER() \
    "[Debug] "

#define ECHO_ERROR(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)
#define ENABLE_LB_DEBUG_DUMP 

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
        int ORB_init_argc = argc;
        std::string str_opt( "" );

        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";
        str_opt += std::string( "--LBInterceptorSide=Client");
        ORB_init_argc += 1;
        ORB_init_argv = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings( str_opt ) );

	CORBA::Boolean override_default = false;
	CORBA::Boolean override_force = false;
	std::string policy_override = Cdmw::OsSupport::OS::get_option_value(argc, argv, "--CdmwLBPolicyDefault");
	if (policy_override.compare("yes") && policy_override.compare("no") )
	  override_default = true;
	else
	  {
	    policy_override = Cdmw::OsSupport::OS::get_option_value(argc, argv, "--CdmwLBPolicyForce");
	    if(policy_override.compare("yes") && policy_override.compare("no"))
	      override_force = true;
	  }
        
	if(override_default)
	   DEBUG_DUMP("   Start LB Service overriding IOGR LBPolicy with default value...    ");
	else
	  if(override_force)
	   DEBUG_DUMP("   Start LB Service overriding LBPolicy for all IOGRs   ");
	  else
	    DEBUG_DUMP("   Start LB Service without override IOGR LB Policy   ");
        Cdmw::LB::LBServiceInit::Init(ORB_init_argc, ORB_init_argv);
        
        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();


        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

        DEBUG_DUMP("   Register a new User Defined Strategy (A Round Robin, but the next replica must be selected after two requests)   ");
        Cdmw::LB::IOGRFactory* iogr_factory = new Cdmw::LB::TAO_LB_IOGRFactory(std::cerr, orb.in());
        CdmwLB::StrategyInfo* stgy_info = new CdmwLB::StrategyInfo();
        stgy_info->strategy_name = "UserDefinedStrategy";
        stgy_info->strategy_factory = new Cdmw::UserDefinedStrategyFactory_impl(iogr_factory);;
        Cdmw::LB::LBServiceInit::Register_lb_policy(stgy_info);
    

        
        CdmwLBInit::HelloInterface_var hello_group = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello_group_default");

        DEBUG_DUMP("   Call hello on Hello Object Group with Default=RoundRobin policy    ");
	for(int i = 0; i<16; i++)
	  {
	    DEBUG_DUMP("  #####HELLO##### from " <<  CORBA::string_dup(hello_group->hello()));
	    Cdmw::OsSupport::OS::sleep(800);	    
            }

       	hello_group = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello_group_random");

        DEBUG_DUMP("   Call hello on Hello Object Group with Random policy   ");
	for(int i = 0; i<16; i++)
	  {
	    DEBUG_DUMP("  #####HELLO##### from " <<  CORBA::string_dup(hello_group->hello()));
	    Cdmw::OsSupport::OS::sleep(800);
	  }

        DEBUG_DUMP("   Try to call hello on Hello Object on host 1(The request must be filtered through the client interceptor)   ");
        CdmwLBInit::HelloInterface_var hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello1");
	DEBUG_DUMP("  #####HELLO##### from " <<  CORBA::string_dup(hello_group->hello()));


	

	std::cerr << "Hit a key and ENTER...." << std::endl;
	std::string dummy;
	cin >> dummy;
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
    
