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


// Standard Files 
#include <string>
#include <fstream>

// Cdmw Files
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include <testftreplicationmanagerinit/TestFTInit2.hpp>
#include <testftreplicationmanagerinit/TestUtils.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>
#include <FaultTolerance/ftcommon/FTConfiguration.hpp>

#include <idllib/FT.stub.hpp>

using namespace Cdmw;
using namespace std;

namespace
{
    const char * rep_id_hello = "IDL:thalesgroup.com/CdmwFT/Test/HelloInterface:1.0";
    const char * rep_id_fault_detector = "IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector:1.0";
}
                               
int main( int argc, char* argv[] )
{
    try
    {
        // Initialise FT service
        Cdmw::FT::FTServiceInit::Init(argc, argv, true);

        // Initialises the ORB
        OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();
        
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        
        // TO be removed walk around!!!!!
        //Cdmw::OrbSupport::OrbSupport::set_endpoint_selector_factory(Cdmw::FT::FTConfiguration::get_FT_endpoint_selector_factory());

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        
        std::cout << "Obtain the ORB�s PolicyManager " << std::endl;
        obj = orb->resolve_initial_references("ORBPolicyManager");
        
        CORBA::PolicyManager_var policy_manager 
            = CORBA::PolicyManager::_narrow(obj.in());
        
        // Create and apply an ORB-wide Routed policy
        std::cout << "Create and apply an ORB-wide Routed policy " << std::endl;
        std::cout << "Creating REQUEST_DURATION_POLICY ..." << std::endl;
        CORBA::Any any;
        TimeBase::TimeT duration = 20000000;
        any <<= duration;
        CORBA::PolicyList policies(1);
        policies.length(1);
        policies[0] =
            orb->create_policy(::FT::REQUEST_DURATION_POLICY, any);
        
        policy_manager->set_policy_overrides(policies, CORBA::SET_OVERRIDE);
        
        using namespace Cdmw::OsSupport;
        OS::ProcessId replMngt_id
        = OS::create_process ("cdmw_ft_manager"," --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml");
        //= OS::create_process ("replMngt_process"," ");

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*20000);
        

        // Get the fault Manager IOR
        std::cout << "Obtain the fault_manager " << std::endl;
        obj = orb->string_to_object("corbaloc::localhost:4555/fault_manager");

        CdmwFT::FTManager_var ftManager = CdmwFT::FTManager::_narrow(obj.in());
        obj = ftManager->get_replication_manager();
        CdmwFT::ReplicationManager_var replication_manager = CdmwFT::ReplicationManager::_narrow(obj.in());
        std::string replication_ior_str = orb->object_to_string(replication_manager.in());
        
        // export the object reference to a file
        std::cout << " export the replication_manager object reference to a file"<<std::endl;
        std::ofstream os("replication_manager.ior");
        os << replication_ior_str;
        os.close();
        {
            
             // Create Server process
            std::cout << "Create test_server process" << std::endl;

            OS::ProcessId test_server_id1 
            = OS::create_process ("test_server2", 
                                  "--FilePath1=hello1 --HostName=A -ORBInitRef ReplicationManager=" 
                                  + replication_ior_str);
            
            OS::ProcessId test_server_id2 
            = OS::create_process ("test_server2", 
                                  "--FilePath1=hello2 --HostName=B -ORBInitRef ReplicationManager=" 
                                  + replication_ior_str);
            
            OS::ProcessId test_server_id3 
            = OS::create_process ("test_server2", 
                                  "--FilePath1=hello3 --HostName=C -ORBInitRef ReplicationManager=" 
                                  + replication_ior_str);
            

            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            OsSupport::OS::sleep(timescale*15000);


            //
            // Create group
            //
            std::cout << "Create group" << std::endl;
            ::FT::ObjectGroup_var obj_group_fd = ::FT::ObjectGroup::_nil();
            //= Cdmw::FT::TestUtils::create_group(orb.in(), replication_manager.in(), rep_id_fault_detector);
            
            ::FT::ObjectGroup_var obj_group_h
              = Cdmw::FT::TestUtils::create_group(orb.in(), replication_manager.in(), rep_id_hello);
            
            OsSupport::OS::sleep(timescale*15000);
            
            // Do test
            std::cout<<"start test"<<std::endl;
            
            Cdmw::FT::TestFTInit2 test(orb.in(), 
                                       rootPOA.in(), 
                                       obj_group_fd.in(), 
                                       obj_group_h.in(),
                                       replication_manager.in(), 
                                       "TestFTInit2");
            test.start();
            OsSupport::OS::sleep(timescale*2000);
            
            // clean created file
            OsSupport::OS::unlink("hello1");
            OsSupport::OS::unlink("hello2");
            OsSupport::OS::unlink("hello3");
            OsSupport::OS::unlink("replication_manager.ior");
            OsSupport::OS::unlink("A_primary_backup_admin.ior");
            OsSupport::OS::unlink("B_primary_backup_admin.ior");
            OsSupport::OS::unlink("C_primary_backup_admin.ior");
            
            
            OS::kill_process(test_server_id2);
            OS::kill_process(test_server_id3);
            OS::kill_process(test_server_id1);
        }
        
        OS::kill_process(replMngt_id );
    }
    catch( const CORBA::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << endl;
        return EXIT_FAILURE;
    }
    catch(...)
    {
        cerr << "Unexpected exception" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

