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
 * @brief implementation for TestLBStrategy with cppUnit library.
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
#include <LoadBalancing/idllib/CdmwLBGroupManager.stub.hpp>
#include <testlbstrategy/CORBAManager.hpp>
#include <testlbstrategy/TestUtils.hpp>
#include <idllib/PortableGroup.stub.hpp>
#include <cppunit/extensions/HelperMacros.h>

#if CDMW_ORB_VDR == tao
#include "ace/Basic_Stats.h"
#include "ace/High_Res_Timer.h"
#include "ace/Stats.h"
#include "ace/Sample_History.h"
#include <iostream>
using namespace std;

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
        
        DEBUG_DUMP("   Try to start LB Service (No Exception must be throw)   ");
        CPPUNIT_ASSERT_NO_THROW(Cdmw::LB::LBServiceInit::Init(ORB_init_argc, ORB_init_argv));
        
        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();


        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        CdmwLB::LBGroupManager_var group_mgr = CdmwLB::LBGroupManager::_narrow(orb->resolve_initial_references("LBGroupManager"));
        ::PortableGroup::Location loc;
        loc.length(3);
        loc[0].id="HOST5";
        loc[0].kind="hostname1";
        loc[1].id="APPL1";
        loc[1].kind="applicationname1";
        loc[2].id="PROC1";
        loc[2].kind="processname1";
                
        CdmwLBStrategy::Pingable_var pingable_group = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "pingable_group_round_robin");

        PortableGroup::ObjectGroupRefVersion ping_version = group_mgr->get_object_group_version_from_ref(pingable_group.in());
        CdmwLBStrategy::Pingable_var pingable_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "pingServer5");

        DEBUG_DUMP("   Try to call ping on Pingable Object Group with Round Robin load balancing policy (TIME REFRESH = 5) ");

        ACE_Sample_History history (20);
        ACE_hrtime_t test_start = ACE_OS::gethrtime ();

        for(int i = 0; i<20; i++)
        {
            if (i==5)
            {
                DEBUG_DUMP("   Try to add a replica to Pingable Object Group   ");
                CPPUNIT_ASSERT_NO_THROW(pingable_group = CdmwLBStrategy::Pingable::_narrow(group_mgr->add_member(pingable_group.in(),
                                                              loc,
                                                              pingable_object.in())));
                DEBUG_DUMP("   Check the good object group version on the Group Manager   ");
                CPPUNIT_ASSERT(ping_version < group_mgr->get_object_group_version_from_ref(pingable_group.in()));
                DEBUG_DUMP("   In the next request the Client Interceptor updates the Object Group with the new replica   ");
            }
            ACE_hrtime_t start = ACE_OS::gethrtime ();
            CPPUNIT_ASSERT_NO_THROW(pingable_group->ping());
            ACE_hrtime_t now = ACE_OS::gethrtime ();
            history.sample (now - start);
        }

        ACE_hrtime_t test_end = ACE_OS::gethrtime ();
        history.sample (test_end - test_start);
        ACE_UINT32 gsf = ACE_High_Res_Timer::global_scale_factor ();
        
        ACE_Basic_Stats stats;
        history.collect_basic_stats (stats);
        stats.dump_results ("Total", gsf);
        
        ACE_Throughput_Stats::dump_throughput ("Total",
                                               gsf,
                                               test_end - test_start,
                                               stats.samples_count ());

        pingable_group = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "pingable_group_random");

        ping_version = group_mgr->get_object_group_version_from_ref(pingable_group.in());

        DEBUG_DUMP("   Try to call ping on Pingable Object Group with Random load balancing policy  ");

        test_start = ACE_OS::gethrtime ();

        for(int i = 0; i<20; i++)
        {
            if (i==5)
            {
                DEBUG_DUMP("   Try to add a replica to Pingable Object Group   ");
                CPPUNIT_ASSERT_NO_THROW(pingable_group = CdmwLBStrategy::Pingable::_narrow(group_mgr->add_member(pingable_group.in(),
                                                              loc,
                                                              pingable_object.in())));
                DEBUG_DUMP("   Check the good object group version on the Group Manager   ");
                CPPUNIT_ASSERT(ping_version < group_mgr->get_object_group_version_from_ref(pingable_group.in()));
                DEBUG_DUMP("   In the next request the Client Interceptor updates the Object Group with the new replica   ");
            }
            
            ACE_hrtime_t start = ACE_OS::gethrtime ();
            CPPUNIT_ASSERT_NO_THROW(pingable_group->ping());
            ACE_hrtime_t now = ACE_OS::gethrtime ();
            history.sample (now - start);
        }

        test_end = ACE_OS::gethrtime ();
        history.sample (test_end - test_start);
        gsf = ACE_High_Res_Timer::global_scale_factor ();
        
        history.collect_basic_stats (stats);
        stats.dump_results ("Total", gsf);
        
        ACE_Throughput_Stats::dump_throughput ("Total",
                                               gsf,
                                               test_end - test_start,
                                               stats.samples_count ());
       
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
    
