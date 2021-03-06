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
#include <testlbinit/TestUtils.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <testlbinit/UserDefinedStrategyFactory_impl.hpp>
#include <testlbinit/CORBAManager.hpp>
#include <iostream>
using namespace std;

#if CDMW_ORB_VDR == tao
#include <LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp>

#else
#error "Unsupported ORB"
#endif


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
        int ORB_init_argc = argc;
        std::string str_opt( "" );

        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";
        str_opt += std::string( "--LBInterceptorSide=Client");
        ORB_init_argc += 1;
        ORB_init_argv = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings( str_opt ) );
        
        DEBUG_ECHO("\n");
        DEBUG_DUMP("   Try to start LB Service overriding Default LB Policy (No Exception must be throw)   ");
        CPPUNIT_ASSERT_NO_THROW(Cdmw::LB::LBServiceInit::Init(ORB_init_argc, ORB_init_argv));
        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();


        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

        DEBUG_DUMP("   Try to register a new User Defined Strategy (A Round Robin, but the next replica must be selected after two requests)   ");
        Cdmw::LB::IOGRFactory* iogr_factory = new Cdmw::LB::TAO_LB_IOGRFactory(std::cerr, orb.in());
        CdmwLB::StrategyInfo* stgy_info = new CdmwLB::StrategyInfo();
        stgy_info->strategy_name = "UserDefinedStrategy";
        stgy_info->strategy_factory = new Cdmw::UserDefinedStrategyFactory_impl(iogr_factory);;
        CPPUNIT_ASSERT_NO_THROW(Cdmw::LB::LBServiceInit::Register_lb_policy(stgy_info));
    
    
        CdmwLBInit::HelloInterface_var hello_group = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello_group_default");

        DEBUG_DUMP("   Try to call hello on Hello Object Group with Default LB Policy (UserDefined Strategy must be used)   ");
        
        for(int i = 0; i<16; i++)
            {
                CPPUNIT_ASSERT_NO_THROW(hello_group->hello());
            }

        hello_group = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello_group_random");

        DEBUG_DUMP("   Try to call hello on Hello Object Group with Random LB Policy(IOGR policy=Random must be used)   ");
        
        for(int i = 0; i<16; i++)
            {
                CPPUNIT_ASSERT_NO_THROW(hello_group->hello());
            }
        

        DEBUG_DUMP("   Try to call hello on Hello Object on host 1(The request must be filtered through the client interceptor)   ");
        CdmwLBInit::HelloInterface_var hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello1");
        CPPUNIT_ASSERT_NO_THROW(hello_object->hello());
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
    
