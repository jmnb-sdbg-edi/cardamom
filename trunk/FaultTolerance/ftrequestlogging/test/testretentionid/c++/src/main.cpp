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
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/CORBASystemExceptionWrapper.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
// #include <testftreplicationmanagerinit/TestFTInit2.hpp>
// #include <testftreplicationmanagerinit/TestUtils.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>
#include <FaultTolerance/ftcommon/FTConfiguration.hpp>
#include <idllib/FT.stub.hpp>

#include <testretentionid/TestRetentionId.hpp>
#include <testretentionid/RetentionIdTester.stub.hpp>


using namespace Cdmw;
using namespace std;

namespace
{
    const char * TESTER_REP_ID = "IDL:thalesgroup.com/CdmwFT/Test/RetentionIdTester:1.0";
    const char * TESTER_IOR_FILE = "tester.ior";
}
      



CdmwFT::Test::RetentionIdTester_ptr
create_group(CORBA::ORB_ptr orb, 
             CdmwFT::ReplicationManager_ptr replication_manager)
    throw (CORBA::Exception)
{
    ::FT::Properties prop;
    prop.length(4);

    prop[0].nam.length(1);
    prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
    prop[0].val <<= (CORBA::UShort)2;
    
    prop[1].nam.length(1);
    prop[1].nam[0].id="org.omg.ft.ReplicationStyle";
    prop[1].val <<= (CORBA::UShort)2;
    
    prop[2].nam.length(1);
    prop[2].nam[0].id="org.omg.ft.MembershipStyle";
    prop[2].val <<= (CORBA::UShort)0;
    
    // set default properties
    ::FT::Location loc1;
    loc1.length(3);
    loc1[0].id = Cdmw::OsSupport::OS::get_hostname().c_str();
    loc1[0].kind = "hostname";
    loc1[1].id = "APPL";
    loc1[1].kind = "application";
    loc1[2].id = "PROC";
    loc1[2].kind = "process";
    
    ::FT::FactoryInfos factoryInfos;
    ::FT::Criteria factoryCrit;        
    factoryCrit.length(0);
    factoryInfos.length(1);
    factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
    factoryInfos[0].the_location = loc1;
    factoryInfos[0].the_criteria = factoryCrit;

    prop[3].nam.length(1);
    prop[3].nam[0].id="org.omg.ft.Factories";
    prop[3].val <<= factoryInfos;
    
    ::FT::Criteria ft_criteria;       
    ft_criteria.length(1);
    ft_criteria[0].nam.length(1);
    ft_criteria[0].nam[0].id="org.omg.ft.FTProperties";
    ft_criteria[0].val <<= prop;

    // create the object group 
    ::FT::ObjectGroup_var obj_group;
    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id;
    CdmwFT::Test::RetentionIdTester_var tester; 
    try
    {
        std::cout << "Create object group" << std::endl;
        obj_group = replication_manager->create_object(TESTER_REP_ID,
                                                       ft_criteria,
                                                       factory_creation_id.out());

        
        std::cout << "Resolve ior file: " << TESTER_IOR_FILE << std::endl;
        std::string corbaloc = "file://";
        corbaloc += TESTER_IOR_FILE;
        CORBA::Object_var obj = orb->string_to_object(corbaloc.c_str());
        tester = CdmwFT::Test::RetentionIdTester::_narrow(obj.in());
                                                       
        std::cout << "Add new member on the group" << std::endl;
        obj_group = replication_manager->add_member(obj_group.in(),
                                                    loc1,
                                                    tester.in());
                                                    
        tester = CdmwFT::Test::RetentionIdTester::_narrow(obj_group.in());
            
    }      
    catch( const ::FT::InvalidProperty& e )
    {
        std::cerr << "InvalidProperty [" << e.nam[0].id << "]" << std::endl;;
        throw;
    } 
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;;
        throw;
    }

    return tester._retn();
}






int main( int argc, char* argv[] )
{
    try
    {
        int timescale = Cdmw::TestUtils::Testable::get_timescale();

        // Initialise FT service
        Cdmw::FT::FTServiceInit::Init(argc, argv, true);

        // Initialises the ORB
        OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPool(4);
        
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        
        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        std::cout << "Obtain the ORB’s PolicyManager " << std::endl;
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
        
        // start ReplicationManager
        using namespace Cdmw::OsSupport;
        OS::ProcessId replMngt_id =
            OS::create_process ("cdmw_ft_manager",
                                " --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml");

        // Get reference to the ReplicationManager
        std::cout << "Obtain the ReplicationManager " << std::endl;
        CdmwFT::ReplicationManager_var replication_manager = 
            CdmwFT::ReplicationManager::_nil(); 
        bool ftmgr_not_active = true;
        int nbRetries = 0, maxRetries = 10;
        do {
            OsSupport::OS::sleep(timescale*5000);
            try {
                obj = orb->string_to_object("corbaloc::localhost:4555/fault_manager");
                CdmwFT::FTManager_var ftManager = CdmwFT::FTManager::_narrow(obj.in());
                replication_manager = ftManager->get_replication_manager();
                ftmgr_not_active = false;
            } catch (const CORBA::SystemException& e) {
                // ReplicationManager not yet started
                if (++nbRetries > maxRetries) {
                    std::cerr << "ERROR: FTManager not started in time" << std::endl;
                    CDMW_THROW2(Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                                CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO),
                                "FTManager not yet activated" );
                }
                std::cout << "ReplicationManager not yet started. Retrying..." 
                          << std::endl;
            }
        } while (ftmgr_not_active);
        
        
        std::string replication_ior_str = 
            orb->object_to_string(replication_manager.in());
        
        
        {
             // Create Server process
            std::cout << "Create retention_id_tester process" << std::endl;

            std::string args = "--FilePath=";
            args += TESTER_IOR_FILE; 
            args += " -ORBInitRef ReplicationManager=";
            args += replication_ior_str; 
            OS::ProcessId retention_id_tester =
                OS::create_process ("retention_id_tester", 
                                    args.c_str());
            
            OsSupport::OS::sleep(timescale*10000);

            //
            // Create group
            //
            std::cout << "Create group" << std::endl;
            CdmwFT::Test::RetentionIdTester_var tester =
                create_group(orb.in(), 
                             replication_manager.in());
            
            // Do test
            std::cout<<"start test"<<std::endl;
            
            CdmwFT::Test::TestRetentionId test(orb.in(), 
                                               tester.in());
            test.start();
            
            // clean created file
            OsSupport::OS::unlink(TESTER_IOR_FILE);
            
            OS::kill_process(retention_id_tester);
        }
        
        OS::kill_process(replMngt_id);
        
        orb->shutdown(false);
        orb->destroy();
        
    }
    catch( const CORBA::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << std::endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "
                  << "Unexpected exception" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

