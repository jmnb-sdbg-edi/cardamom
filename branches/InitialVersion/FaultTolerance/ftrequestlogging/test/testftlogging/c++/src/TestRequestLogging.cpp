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

#include <iostream>
#include <sstream>
#include <string.h>

#include "testftlogging/TestRequestLogging.hpp"
#include "idllib/CdmwFTReplicationManager.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

// Warning DIRE HACK
// This is done to modify the ORBs record of the retention id
// The alternative is to synthesise all of the FT context information
// via a portable interceptor.
// The existing approach uses the standard TAO FT mechanism.
#include <tao/ORB_Core.h>
#define private public
#include <orbsvcs/orbsvcs/FaultTolerance/FT_ClientRequest_Interceptor.h>
#undef private

namespace {
  const char* LOC1 = "HOST1/APPL1/PROC1";
  const char* LOC2 = "HOST2/APPL2/PROC2";
  const char* LOC3 = "HOST3/APPL3/PROC3";
}; // End of anonymous namespace

namespace Cdmw {
namespace FT {

TestRequestLogging::TestRequestLogging (
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name,
    DsIdentifier ds_id)
    : Testable( name )
{
    m_orb = CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(rootPOA);
}

TestRequestLogging::~TestRequestLogging ()
{
}
	
void 
TestRequestLogging::do_tests()
{
    set_nbOfRequestedTestOK(22);

    TAO_ORB_Core* orb_core = m_orb->orb_core();
    TEST_INFO( "number of interceptors = " << orb_core->client_request_interceptors().size());
    PortableInterceptor::ClientRequestInterceptor* client_interceptor
        = orb_core->client_request_interceptors()[0].in();
    TAO::FT_ClientRequest_Interceptor* ft_client_interceptor
        = dynamic_cast<TAO::FT_ClientRequest_Interceptor*>(client_interceptor);
    TEST_INFO( "interceptor = " << ft_client_interceptor);

    try
    {
        std::string iorhello1APPL1 = "hello1APPL1.ior";
        std::string iorhello2APPL1 = "hello2APPL1.ior";
        
        ::FT::Location_var ftloc1 = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(LOC1);
        // first delete the ior files for synchronisation
        try
        {
            OsSupport::OS::unlink (iorhello1APPL1);	
        }
        catch (...)
        {
        }
        
        TEST_INFO( "Resolve the Replication Manager reference" );

        CORBA::Object_var objref = m_orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());
        
        if (CORBA::is_nil(rm.in()))
        {
            std::cout << "File : " << __FILE__ << " Line : " << __LINE__
                      << "could not _narrow object to type ReplicationManager" << std::endl;
            abort();
        }
        
        TEST_SUCCEED();

        objref = m_orb->string_to_object("corbaloc::localhost:4555/fault_manager");
        ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow(objref.in());

        if (CORBA::is_nil(rm.in()))
        {
            std::cout << "File : " << __FILE__ << " Line : " << __LINE__
                      << "could not _narrow object to type FTManager" << std::endl;
            abort();
        }
        
        TEST_SUCCEED();
        
        // create a first object with an invalid criteria
        // the invalid criteria is the FaultMonitoringStyle 
        TEST_INFO("Create objects groups");

        const char * rid1 = "IDL:thalesgroup.com/CdmwRequestLogging/HelloInterface:1.0";

        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;

        CORBA::Object_var obj1;
        
        // set default properties
        TEST_INFO( "set default properties" );
    
        ::FT::Properties prop;
    
        prop.length(4);
        prop[0].nam.length(1);
        prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop[0].val <<= (CORBA::UShort)1;
        
        prop[1].nam.length(1);
        prop[1].nam[0].id="org.omg.ft.ReplicationStyle";
        prop[1].val <<= (CORBA::UShort)2;
        
        prop[2].nam.length(1);
        prop[2].nam[0].id="org.omg.ft.MembershipStyle";
        prop[2].val <<= (CORBA::UShort)0;
        
        prop[3].nam.length(1);
        prop[3].nam[0].id="org.omg.ft.Factories";
        
        ::FT::FactoryInfos factoryInfos;
        factoryInfos.length(1);
        factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
        factoryInfos[0].the_location = ftloc1.in();
        ::FT::Criteria factoryCrit;        
        factoryCrit.length(0);
        factoryInfos[0].the_criteria = factoryCrit;
	
        prop[3].val <<= factoryInfos;
        
        ::FT::Criteria crit1;       
        crit1.length(1);
        crit1[0].nam.length(1);
        crit1[0].nam[0].id="org.omg.ft.FTProperties";
        crit1[0].val <<= prop;
        
        try
        {
            obj1 =  rm->create_object(rid1,
                                      crit1,
                                      factory_creation_id1.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        
        ::FT::ObjectGroupId ogi1 = 0;
        try
        {
            ogi1 = rm->get_object_group_id(obj1.in());
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        TEST_INFO("start clients");
        std::ostringstream appl1;

        appl1<<"--client APPL1 --object_group_id1="<<ogi1;

        OsSupport::OS::ProcessId pid1 =
            OsSupport::OS::create_process("cdmw_test_ft_logging", appl1.str());
        sleep(2);
        TEST_SUCCEED();

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*6000);
        

        
        // wait creation of ior files
        while (!OsSupport::OS::file_exists(iorhello1APPL1))
        {
            OsSupport::OS::sleep(timescale*1000);
        }

        TEST_INFO("get the reference for the synchronisation object");
        std::string file_url_prefix = "file://";        
        std::string file_url = file_url_prefix;
        

        TEST_INFO("get the reference for object to insert  into the object group");
        file_url = file_url_prefix;
        file_url += iorhello1APPL1;
        
        CORBA::Object_var object_hello1APPL1 =
            m_orb->string_to_object(file_url.c_str());

        CdmwRequestLogging::HelloInterface_var hello1APPL1 =
            CdmwRequestLogging::HelloInterface::_narrow
            (object_hello1APPL1.in());
        TEST_SUCCEED();

        TEST_INFO("get the reference of the object group 1");
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        // Add new member on the group
        TEST_INFO("add the new member HOST2/APPL2/PROC2 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  ftloc1.in(),
                                  hello1APPL1.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        m_hello = CdmwRequestLogging::HelloInterface::_narrow(obj1.in());

        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("client started")
        
        TEST_INFO("Invoke a simple operation");
        m_hello->hello();
        TEST_SUCCEED();

        TEST_INFO("Check log size");
        TEST_CHECK(m_hello->get_log_size() == 1);

        TEST_INFO("Invoke an operation with a return value");
        TEST_CHECK(m_hello->do_something(4) == 16);

        TEST_INFO("Check log size");
        TEST_CHECK(m_hello->get_log_size() == 3);

        TEST_INFO("Invoke an operation with a known return value");
        TEST_CHECK(m_hello->do_something(5) == 25);

        TEST_INFO("Reset retention id");
        CORBA::Long old_retention_id = ft_client_interceptor->retention_id_--;
        
        TEST_INFO("Invoke an operation with expecting old return value");
        TEST_CHECK(m_hello->do_something(0) == 25);

        TEST_INFO("Check log size");
        TEST_CHECK(m_hello->get_log_size() == 5);
        while (true) {
            TEST_INFO("Invoke an operation with an inout and an out value");
            char* val = CORBA::string_dup("hello");
            ::CdmwRequestLogging::MyStruct_var sval;
            CORBA::String_var ret = m_hello->do_something_more(val, sval.out());
            TEST_CHECK(strcmp(ret.in(), "hello") == 0);
            TEST_CHECK(strcmp(val, "olleh") == 0);
            TEST_CHECK(strcmp(sval->val.in(), "olleh|hello") == 0);
            break;
        }

        TEST_INFO("Reset retention id");
        ft_client_interceptor->retention_id_--;

        TEST_INFO("Repeat an operation with an inout and an out value");
        char* val = CORBA::string_dup("world");
        ::CdmwRequestLogging::MyStruct_var sval2;
        TEST_CHECK(strcmp(m_hello->do_something_more(val, sval2.out()), "hello") == 0);
        TEST_CHECK(strcmp(val, "olleh") == 0);
        TEST_CHECK(strcmp(sval2->val.in(), "olleh|hello") == 0);

        TEST_INFO("Check log size");
        TEST_CHECK(m_hello->get_log_size() == 7);

        TEST_INFO("Wait for timeouts to fire");
        OsSupport::OS::sleep(14000);

        TEST_INFO("Check log size");
        CORBA::ULong size = m_hello->get_log_size();
        TEST_INFO("log size is " << size);
        TEST_CHECK(size == 0);

        TEST_INFO("Check of an expired request");
        try {
            ft_client_interceptor->retention_id_ = old_retention_id;

            TimeBase::TimeT duration_time = 1; // This should to have an expired request!

            CORBA::Any any;
            any <<= duration_time;
            CORBA::PolicyList policies(1);
            policies.length(1);
            policies[0] =
                m_orb->create_policy(::FT::REQUEST_DURATION_POLICY, any);
            CORBA::Object_var obj = 
                m_hello->_set_policy_overrides(policies, CORBA::SET_OVERRIDE);

            CdmwRequestLogging::HelloInterface_var m_hello2 
                = CdmwRequestLogging::HelloInterface::_narrow(obj.in());
            m_hello2->do_something(0);
            TEST_FAILED();
        } catch (const CORBA::BAD_CONTEXT & ) {
            TEST_SUCCEED();
        }
//       TEST_INFO("Test size of TX DataStore");
//       TEST_CHECK(myDataStore1.size() == 0);

//       try
//       {
//           manager.begin();
//           TEST_FAILED();
//       }
//       catch (const InTransactionException&)
//       {
//           TEST_SUCCEED();
//       }

        TEST_INFO( "Killing the client" );
        OsSupport::OS::kill_process(pid1);
        OsSupport::OS::sleep(timescale*2000);
    }
    catch (const CORBA::UserException& ex)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test TestRequestLogging Exception caught : " << ex
                     << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& ex)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test TestRequestLogging Exception caught : " << ex
                     << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    catch (...)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test TestRequestLogging Exception caught : " << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
}

}; // namespace FT
}; // namespace Cdmw
