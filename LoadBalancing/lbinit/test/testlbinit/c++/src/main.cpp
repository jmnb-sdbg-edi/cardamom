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
 * @brief The main for test lbinit.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 *
 */

#ifndef LBINIT_PLUGIN_EXPORTS
#include <string>
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
#include "testlbinit/CORBAManager.hpp"
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
#include <testlbinit/TestUtils.hpp>

#include <iostream>
using namespace std;

using namespace Cdmw;

namespace
{
    const std::string GROUP_MANAGER_PORT = "5040";
    const char * rep_id_hello = "IDL:CdmwLBInit/HelloInterface:1.0";
}

int main( int argc, char* argv[] )
{
    char *outFileName = "res.xml";
    try
    {
        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();


        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

        using namespace Cdmw::OsSupport;
        OS::ProcessId groupMngt_id
            = OS::create_process ("cdmw_lb_group_manager","--CdmwLBXMLFile=CdmwLBGroupManager_conf.xml ");

        OsSupport::OS::sleep(5000);

        // Get the LB Group Manager IOR
        CORBA::Object_var obj = orb->string_to_object("corbaloc::localhost:5040/group_mgr");
        CdmwLB::LBGroupManager_var group_manager
            = CdmwLB::LBGroupManager::_narrow(obj.in());

        // Create Server process
        
        OS::ProcessId test_server_id1
            = OS::create_process ("test_server_init",
                                  "--FilePath=hello1 --HostName=host1 -ORBInitRef LBGroupManager=corbaloc::localhost:5040/group_mgr");
        OsSupport::OS::sleep(1000);
        OS::ProcessId test_server_id2
            = OS::create_process ("test_server_init",
                                  "--FilePath=hello2 --HostName=host2 -ORBInitRef LBGroupManager=corbaloc::localhost:5040/group_mgr");

        OsSupport::OS::sleep(1000);
        
        OS::ProcessId test_server_id3
            = OS::create_process ("test_server_init",
                                  "--FilePath=hello3 --HostName=host3 -ORBInitRef LBGroupManager=corbaloc::localhost:5040/group_mgr");
        
        OsSupport::OS::sleep(1000);
        OS::ProcessId test_server_id4
            = OS::create_process ("test_server_init",
                                  "--FilePath=hello4 --HostName=host4 -ORBInitRef LBGroupManager=corbaloc::localhost:5040/group_mgr");

        OsSupport::OS::sleep(5000);

        const char * rep_id_hello = "IDL:CdmwLBInit/HelloInterface:1.0";
        std::string lb_policy = "default";
        //
        // Create a group with default load balancing policy (RoundRobin)
        //
        ::PortableGroup::Properties prop;
        
        prop.length(1);
        prop[0].nam.length(1);
        prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
        prop[0].val <<=  lb_policy.c_str();
        
        ::PortableGroup::Criteria crit;
        crit.length(1);
        crit[0].nam.length(1);
        crit[0].nam[0].id="cdmw.lb.LBProperties";
        crit[0].val <<= prop;
        ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
        
        obj = group_manager->create_object(rep_id_hello,
                                           crit,
                                           factory_creation_id1.out());

        // Add replicas
        CORBA::Object_var hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello1");
        ::PortableGroup::Location loc;
        loc.length(3);
        loc[0].id="HOST1";
        loc[0].kind="hostname1";
        loc[1].id="APPL1";
        loc[1].kind="applicationname1";
        loc[2].id="PROC1";
        loc[2].kind="processname1";
        
        
        obj = group_manager->add_member(obj.in(),
                                        loc,
                                        hello_object.in());

        hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello2");
        loc[0].id = "HOST2";
        obj = group_manager->add_member(obj.in(),
                                        loc,
                                        hello_object.in());
        
        hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello3");
        loc[0].id = "HOST3";
        obj = group_manager->add_member(obj.in(),
                                        loc,
                                        hello_object.in());
    
	     hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello4");
        loc[0].id = "HOST4";
        obj = group_manager->add_member(obj.in(),
                                        loc,
                                        hello_object.in());
        
        // export the object reference to a file
        std::string file_name = "hello_group_default";
        CORBA::String_var ref_string = orb->object_to_string(obj.in());
        std::ofstream os1(file_name.c_str());
        os1 << ref_string.in();
        os1.close();
        
        lb_policy = "Random";
        
        //
        // Create a group with Random policy
        //
        ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id2;

        prop[0].val <<=  lb_policy.c_str();

        crit[0].val <<= prop;
        
        obj = group_manager->create_object(rep_id_hello,
                                           crit,
                                           factory_creation_id2.out());
        
        // Add replicas
        hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello1");
        loc[0].id = "HOST1";
        obj = group_manager->add_member(obj.in(),
                                        loc,
                                        hello_object.in());
        
        hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello2");
        loc[0].id = "HOST2";
        obj = group_manager->add_member(obj.in(),
                                        loc,
                                        hello_object.in());
        
	       hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_file(orb.in(), "hello3");
        loc[0].id = "HOST3";
        obj = group_manager->add_member(obj.in(),
                                        loc,
                                        hello_object.in());
        
	// export the object reference to a file
        file_name = "hello_group_random";
        ref_string = orb->object_to_string(obj.in());
        std::ofstream os2(file_name.c_str());
        os2 << ref_string.in();
        os2.close();
        
        
        
        try
        {
            CORBAManager::instance()->init(orb.in());
            
        }
        catch(std::logic_error& e)
        {
            std::cerr<<e.what()<<std::endl;
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
            std::cerr  <<  std::endl
                       <<  "ERROR: "  <<  e.what()
                       << std::endl;
            return -1;
        }
        int total_fails = result.testFailuresTotal ();
        int total_tests = result.runTests () ;
        
        std::cerr<<"==================================================="<<std::endl;
        std::cerr<<"Ending test LoadBalancing --> lbinit"<<std::endl;
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
        // clean created file
        OsSupport::OS::unlink("hello1");
        OsSupport::OS::unlink("hello2");
        OsSupport::OS::unlink("hello3");
        OsSupport::OS::unlink("hello4");
        OsSupport::OS::unlink("hello_group_default");
        OsSupport::OS::unlink("hello_group_random");
        
        OS::kill_process(test_server_id2);
        OS::kill_process(test_server_id3);
        OS::kill_process(test_server_id4);
        OS::kill_process(test_server_id1);
        
        OS::kill_process(groupMngt_id );
	orb->shutdown(1);
	orb->destroy();
        return result.wasSuccessful() ? 0 : 1;
        
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
#else

#include <cppunit/plugin/TestPlugIn.hpp>

// _implements all the plug-in stuffs, WinMain...
CPPUNIT_PLUGIN_IMPLEMENT();

#endif

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////

