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
 * @brief The main for test lbgroupmanager.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 *
 */
#ifndef LBGROUPMANAGER_PLUGIN_EXPORTS
#include <string>
#include <Foundation/common/String.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>
#include <iostream>
#include <fstream>
#include <LoadBalancing/lbgroupmanager/LBGroupManager_impl.hpp>
#include "testlbgroupmanager/CORBAManager.hpp"
#include <Foundation/commonsvcs/datastore/StdMapDataStoreFactory.hpp>
#include <LoadBalancing/lbcommon/StateTransferConfig.hpp>
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


typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory
< PortableGroup::ObjectGroupId,
  CdmwLB::ObjectGroupData> 
DefaultDataStoreFactory;

using namespace std;

using namespace Cdmw;

int main( int argc, char* argv[] )
{
    char *outFileName = "res.xml";
  
    try
    {
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
	//        strategyList.add_multicast();
	strategyList.add_PoaThreadPerConnection();
	strategyList.add_multicast();

	// set LBGroupManager Initial Reference
        char** ORB_init_argv = 0;
        int ORB_init_argc(0);
        std::string arguments = "-ORBInitRef LBGroupManager=corbaloc::localhost:5040/group_mgr";
        std::string strArgv = argv[0] + std::string(" ") + arguments;
        ORB_init_argv = Common::String::to_char_array (Common::String::to_strings(strArgv));
        ORB_init_argc = 3;


	// Initialises the ORB
        CORBA::ORB_var orb =
          Cdmw::OrbSupport::OrbSupport::ORB_init(ORB_init_argc,ORB_init_argv , strategyList);
	// Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();
	DEBUG_DUMP("Try to start LBGroupManager process without specify the XML configuration file");
	DEBUG_DUMP("The LBGroupManager process should be stopped with an error message...");
	Cdmw::OsSupport::OS::ProcessId group_manager_id 
	  = Cdmw::OsSupport::OS::create_process("cdmw_lb_group_manager", "");
	OsSupport::OS::sleep(3000);

	DEBUG_DUMP("Try to start LBGroupManager process using the XML LB configuration file option but without specify an XML configuration file");
	DEBUG_DUMP("The LBGroupManager process should be stopped with an error message...");
	group_manager_id  = Cdmw::OsSupport::OS::create_process("cdmw_lb_group_manager", "--CdmwLBXMLFile");
	OsSupport::OS::sleep(3000);
	
	DEBUG_DUMP("Try to start LBGroupManager process specifying a wrong XML configuration file");
	DEBUG_DUMP("The LBGroupManager process should be stopped with an error message...");
	group_manager_id  = Cdmw::OsSupport::OS::create_process("cdmw_lb_group_manager", "--CdmwLBXMLFile=CdmwLBGroupManager_not_exist_conf.xml");
	OsSupport::OS::sleep(3000);

	DEBUG_DUMP("Try to start LBGroupManager process specifying a right XML configuration file");
	DEBUG_DUMP("The LBGroupManager process should be started without errors...");
	group_manager_id = Cdmw::OsSupport::OS::create_process("cdmw_lb_group_manager", "--CdmwLBXMLFile=CdmwLBGroupManager_test_conf.xml");
	OsSupport::OS::sleep(2000);

    try
        {
            CORBAManager::instance()->init(orb.in());
            
        }
    catch(std::logic_error& e)
        {
            ECHO_ERROR(e.what());
            return -1;
        }
    catch(...)
        {
            return -1;
        }
    
    // Create the event manager and test controller
    CppUnit::TestResult controller;
    
    // Add a listener that colllects test result
    CppUnit::TestResultCollector result;
    controller.addListener( &result );
    
    // Add a listener that print info about running test.
    CppUnit::BriefTestProgressListener progress;
    controller.addListener( &progress );

    // Add the top suite to the test runner
    CppUnit::TestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
    
    try
        {
            std::ofstream outFile(outFileName);
            runner.run( controller );
            // Print test in xml format on outFile.
            CppUnit::XmlOutputter outputter( &result, outFile );
            outputter.write();
            outFile.close();
        }
    catch ( std::invalid_argument &e )  // Test path not resolved
        {
            ECHO_ERROR( "ERROR: "  <<  e.what());
            return -1;
            }
    int total_fails = result.testFailuresTotal ();
    int total_tests = result.runTests () ;

    std::cerr<<"==================================================="<<std::endl;
    std::cerr<<"Ending test LoadBalancing --> lbgroupmanager"<<std::endl;
    std::cerr<<"==================================================="<<std::endl;
    std::cerr<<"Summary:"<<std::endl;
    std::cerr<<"   Failed    : "<<total_fails<<std::endl;
    std::cerr<<"   Succeeded : "<<total_tests  - total_fails <<std::endl;
    std::cerr<<"   Expected successfull tests (CPP) :"<< total_tests <<std::endl;

    if( total_fails == 0 )
        std::cerr<<"   >>>> Test result is set as TESTABLE_OK <<<<"<<std::endl;

        
#ifdef WIN32
    system("PAUSE");
#endif

    Cdmw::OsSupport::OS::kill_process(group_manager_id);
    orb->destroy();
       	
    return result.wasSuccessful() ? 0 : 1;
}
    catch( const CORBA::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e << endl;
        return -1;
    }
    catch( const Cdmw::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e << endl;
        return -1;
    }
    catch(...)
    {
        cerr << "Uncaught exception" << endl;
        return -1;
        }       
 
}


#else

#include <cppunit/plugin/TestPlugIn.hpp>

// _implements all the plug-in stuffs, WinMain...
CPPUNIT_PLUGIN_IMPLEMENT();

#endif

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
    
