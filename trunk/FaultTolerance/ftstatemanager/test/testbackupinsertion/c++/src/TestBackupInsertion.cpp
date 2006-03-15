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


#include <exception>
#include <iostream>
#include <sstream>


#include "idllib/CdmwFTReplicationManager.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"

#include "testbackupinsertion/TestHello_impl.hpp"

#include "testbackupinsertion/TestBackupInsertion.hpp"
#include "testbackupinsertion/TestClientThread.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>
#include "testbackupinsertion/TestFaultNotifier.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

namespace {
  const char* LOC1 = "HOST1/APPL1/PROC1";
  const char* LOC2 = "HOST2/APPL2/PROC2";
  const char* LOC3 = "HOST3/APPL3/PROC3";
}; // End of anonymous namespace

namespace Cdmw {
namespace FT {

// constructor
TestBackupInsertion::TestBackupInsertion(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name,
    const std::string& process_name )
    : Testable( name ),
      m_process_name(process_name)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

//destructor
TestBackupInsertion::~TestBackupInsertion()
{
}

// determine of the process is into the list
bool TestBackupInsertion::contain(const std::string& process_name, const ::CdmwReplicationManager::LocationList* list)
{
    bool result = false;
    int size = list->length();

    for (int i = 0;
         i < size;
         i++)
    {
        if (strcmp((*list)[i], process_name.c_str()) == 0)
        {
            result = true;
            break;
        } else {
            // do nothing
        }
        
    }
    return result;
}



// do_tests
void TestBackupInsertion::do_tests()
{
    set_nbOfRequestedTestOK (45);

    try
    {
        std::string iorhello1APPL1 = "hello1APPL1.ior";
        std::string iorhello1APPL2 = "hello1APPL2.ior";
        std::string iorhello2APPL1 = "hello2APPL1.ior";
        std::string iorhello2APPL2 = "hello2APPL2.ior";
        
	::FT::Location_var ftloc1 = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(LOC1);
	::FT::Location_var ftloc2 = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(LOC2);
	::FT::Location_var ftloc3 = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(LOC3);
        
        // first delete the ior files for synchronisation
        try
        {
            OsSupport::OS::unlink (iorhello1APPL2);	
        }
        catch (...)
        {
        }

        try
        {
            OsSupport::OS::unlink (iorhello2APPL2);	
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

        const char * rid1 = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface1:1.0";

        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;

        CORBA::Object_var obj1;
        
        // set default properties
        TEST_INFO( "set default properties" );
    
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
	
	prop[3].nam.length(1);
	prop[3].nam[0].id="org.omg.ft.Factories";
	
	::FT::FactoryInfos factoryInfos;
	factoryInfos.length(3);
	factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
	factoryInfos[0].the_location = ftloc1.in();
	::FT::Criteria factoryCrit;        
	factoryCrit.length(0);
	factoryInfos[0].the_criteria = factoryCrit;
	
	factoryInfos[1].the_factory = ::FT::GenericFactory::_nil();
	factoryInfos[1].the_location = ftloc2.in();
	factoryInfos[1].the_criteria = factoryCrit;
	
	factoryInfos[2].the_factory = ::FT::GenericFactory::_nil();
	factoryInfos[2].the_location = ftloc3.in();
	factoryInfos[2].the_criteria = factoryCrit;
    
	prop[3].val <<= factoryInfos;



        ::CdmwFT::StateTransfer::DataStoreIDs dsids;
        dsids.length(2);
        dsids[0] = 1;
        dsids[1] = 2;

	::FT::Criteria crit1;       
	crit1.length(2);
	crit1[0].nam.length(1);
	crit1[0].nam[0].id="org.omg.ft.FTProperties";
	crit1[0].val <<= prop;
	crit1[1].nam.length(1);
	crit1[1].nam[0].id="com.thalesgroup.cdmw.ft.DataStoreIdentifiers";
	crit1[1].val <<= dsids;

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
        std::ostringstream appl2;
        appl1<<"--client APPL1 --object_group_id1="<<ogi1;
        appl2<<"--client APPL2 --object_group_id1="<<ogi1;

        OsSupport::OS::create_process( m_process_name, appl1.str());
        sleep(2);
        OsSupport::OS::ProcessId pid2  = 
        OsSupport::OS::create_process( m_process_name, appl2.str());   
        sleep(4);
        TEST_SUCCEED();

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*6000);
        

        
        // wait creation of ior files
        while (!(OsSupport::OS::file_exists(iorhello2APPL2) && OsSupport::OS::file_exists(iorhello1APPL2)))
        {
            OsSupport::OS::sleep(timescale*1000);
        }

        TEST_INFO("get the reference for the synchronisation object");
        std::string file_url_prefix = "file://";        
        std::string file_url = file_url_prefix;
        

        TEST_INFO("get the reference for object to insert  into the object group");
        file_url = file_url_prefix;
        file_url += iorhello1APPL1;
        
        CORBA::Object_var object_hello1APPL1 = m_orb->string_to_object(file_url.c_str());
        CdmwReplicationManager::HelloInterface1_var hello1APPL1 = CdmwReplicationManager::HelloInterface1::_narrow(object_hello1APPL1.in());

        file_url = file_url_prefix;
        file_url += iorhello1APPL2;
        
        CORBA::Object_var object_hello1APPL2 = m_orb->string_to_object(file_url.c_str());
        CdmwReplicationManager::HelloInterface1_var hello1APPL2 = CdmwReplicationManager::HelloInterface1::_narrow(object_hello1APPL2.in());

        file_url = file_url_prefix;
        file_url += iorhello2APPL1;
        
        CORBA::Object_var object_hello2APPL1 = m_orb->string_to_object(file_url.c_str());
        CdmwReplicationManager::HelloInterface2_var hello2APPL1 = CdmwReplicationManager::HelloInterface2::_narrow(object_hello2APPL1.in());

        file_url = file_url_prefix;
        file_url += iorhello2APPL2;
        
        CORBA::Object_var object_hello2APPL2 = m_orb->string_to_object(file_url.c_str());
        CdmwReplicationManager::HelloInterface2_var hello2APPL2 = CdmwReplicationManager::HelloInterface2::_narrow(object_hello2APPL2.in());


        TEST_SUCCEED();

        TEST_INFO("client started")
        
	// Create a test fault notification consumer
        TEST_INFO( "Register a test fault notifier" );

	Cdmw::FT::TestFaultNotifier *fault_notifier_impl =
            new Cdmw::FT::TestFaultNotifier();
        ::FT::FaultNotifier_var fn = fault_notifier_impl->_this();
        hello1APPL2->set_fault_notifier(fn.in());

        TEST_SUCCEED();

        Cdmw::FT::DataStoreBase::Set_Location_Name(Cdmw::CommonSvcs::Naming::NamingInterface::to_string(ftloc3.in()));

        ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primaryBackupGroupRepository_impl
          = ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();

		  // export GroupRepository in orb
		  CdmwFT::Location::PrimaryBackupGroupRepository_var primary_backup_rep_ref
			  = primaryBackupGroupRepository_impl;
		  
		  m_orb->register_initial_reference("FTGroupRepository", 
				                              primary_backup_rep_ref.in());


        // create the reference for the StatefullPrimaryBackupAdmin
        ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl* statefullPrimaryBackupAdmin_impl
          = new ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl(m_orb.in(), m_rootPOA.in(), ftloc3.in(), primaryBackupGroupRepository_impl, true);
        
        CdmwFT::Location::StatefullPrimaryBackupAdmin_var statefullPrimaryBackupAdmin = statefullPrimaryBackupAdmin_impl->_this();
        


        Cdmw::HelloInterface1_impl * helloInterface1_impl = new Cdmw::HelloInterface1_impl(m_orb.in(), m_rootPOA.in());
        CdmwReplicationManager::HelloInterface1_var hello1APPL3 =  helloInterface1_impl->_this();

        Cdmw::HelloInterface2_impl * helloInterface2_impl = new Cdmw::HelloInterface2_impl(m_orb.in(), m_rootPOA.in());
        CdmwReplicationManager::HelloInterface2_var hello2APPL3 =  helloInterface2_impl->_this();

        ::CdmwFT::StateTransfer::LocalDataStoreInfos infos;
        infos.length(2);
        infos[0] = *hello1APPL3->get_local_datastore_info();
        infos[1] = *hello2APPL3->get_local_datastore_info();

        rm->register_statefull_location(ftloc3.in(),
                                        statefullPrimaryBackupAdmin.in(),
                                        infos, false);

        TEST_INFO("");
        TEST_INFO("START TEST");
        TEST_INFO("");

        // Add new member on the group
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
                                  ftloc2.in(),
                                  hello1APPL2.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("Check the number of location in each datastore");
        {
            ::CdmwReplicationManager::LocationList* hello1APPL1LocationList1 =  hello1APPL1->get_location_list(1);
            TEST_CHECK(hello1APPL1LocationList1->length() == 0);
            ::CdmwReplicationManager::LocationList* hello1APPL1LocationList2 =  hello1APPL1->get_location_list(2);
            TEST_CHECK(hello1APPL1LocationList2->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL1LocationList3 =  hello2APPL1->get_location_list(3);
            TEST_CHECK(hello2APPL1LocationList3->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL1LocationList4 =  hello2APPL1->get_location_list(4);
            TEST_CHECK(hello2APPL1LocationList4->length() == 0);
            
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList1 =  hello1APPL2->get_location_list(1);
            TEST_CHECK(hello1APPL2LocationList1->length() == 0);
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList2 =  hello1APPL2->get_location_list(2);
            TEST_CHECK(hello1APPL2LocationList2->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL2LocationList3 =  hello2APPL2->get_location_list(3);
            TEST_CHECK(hello2APPL2LocationList3->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL2LocationList4 =  hello2APPL2->get_location_list(4);
            TEST_CHECK(hello2APPL2LocationList4->length() == 0);
            
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList1 =  hello1APPL3->get_location_list(1);
            TEST_CHECK(hello1APPL3LocationList1->length() == 0);
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList2 =  hello1APPL3->get_location_list(2);
            TEST_CHECK(hello1APPL3LocationList2->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList3 =  hello2APPL3->get_location_list(3);
            TEST_CHECK(hello2APPL3LocationList3->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList4 =  hello2APPL3->get_location_list(4);
            TEST_CHECK(hello2APPL3LocationList4->length() == 0);
        }
        
        TEST_INFO("Set info into data store");
        try
        {
            for (int i = 0; i < 100; i++)
            {
                hello1APPL2->set_info_into_datastore(1, i, "one");
                hello1APPL2->set_info_into_datastore(2, i, "two");
            }
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        // Add new member on the group
        TEST_INFO("add the new member HOST3/APPL3/PROC3 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  ftloc3.in(),
                                  hello1APPL3.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("Check the number of location in each datastore");
        {
            ::CdmwReplicationManager::LocationList* hello1APPL1LocationList1 =  hello1APPL1->get_location_list(1);
            TEST_CHECK(hello1APPL1LocationList1->length() == 0);
            ::CdmwReplicationManager::LocationList* hello1APPL1LocationList2 =  hello1APPL1->get_location_list(2);
            TEST_CHECK(hello1APPL1LocationList2->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL1LocationList3 =  hello2APPL1->get_location_list(3);
            TEST_CHECK(hello2APPL1LocationList3->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL1LocationList4 =  hello2APPL1->get_location_list(4);
            TEST_CHECK(hello2APPL1LocationList4->length() == 0);
            
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList1 =  hello1APPL2->get_location_list(1);
            TEST_CHECK(hello1APPL2LocationList1->length() == 1);
            TEST_CHECK(contain("HOST3/APPL3/PROC3", hello1APPL2LocationList1));
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList2 =  hello1APPL2->get_location_list(2);
            TEST_CHECK(hello1APPL2LocationList2->length() == 1);
            TEST_CHECK(contain("HOST3/APPL3/PROC3", hello1APPL2LocationList2));
            ::CdmwReplicationManager::LocationList* hello2APPL2LocationList3 =  hello2APPL2->get_location_list(3);
            TEST_CHECK(hello2APPL2LocationList3->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL2LocationList4 =  hello2APPL2->get_location_list(4);
            TEST_CHECK(hello2APPL2LocationList4->length() == 0);
            
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList1 =  hello1APPL3->get_location_list(1);
            TEST_CHECK(hello1APPL3LocationList1->length() == 1);
            TEST_CHECK(contain("HOST2/APPL2/PROC2", hello1APPL3LocationList1));
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList2 =  hello1APPL3->get_location_list(2);
            TEST_CHECK(hello1APPL3LocationList2->length() == 1);
            TEST_CHECK(contain("HOST2/APPL2/PROC2", hello1APPL3LocationList2));
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList3 =  hello2APPL3->get_location_list(3);
            TEST_CHECK(hello2APPL3LocationList3->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList4 =  hello2APPL3->get_location_list(4);
            TEST_CHECK(hello2APPL3LocationList4->length() == 0);
        }
        
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello1APPL3->get_info_from_datastore(1, 1)<<" -> expected \"one\"");
            TEST_CHECK(!strcmp(hello1APPL3->get_info_from_datastore(1, 1), "one"));
            TEST_INFO("data for oid 2 (datastore2):"<<hello1APPL3->get_info_from_datastore(2, 2)<<" -> expected \"two\"");
            TEST_CHECK(!strcmp(hello1APPL3->get_info_from_datastore(2, 2), "two"));
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Set info into data store");
        try
        {
            hello1APPL2->set_info_into_datastore(1, 1, "one2");
            hello1APPL2->set_info_into_datastore(2, 2, "two2");
        }
        catch(...)
        {
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello1APPL3->get_info_from_datastore(1, 1)<<" -> expected \"one2\"");
            TEST_CHECK(!strcmp(hello1APPL3->get_info_from_datastore(1, 1),"one2"));
            TEST_INFO("data for oid 2 (datastore2):"<<hello1APPL3->get_info_from_datastore(2, 2)<<" -> expected \"two2\"");
            TEST_CHECK(!strcmp(hello1APPL3->get_info_from_datastore(2, 2),"two2"));
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        Modifier modifier(hello1APPL2.in());
        modifier.start();
        OsSupport::OS::sleep(timescale*2000);

        // Add new member on the group
        TEST_INFO("add the new member HOST1/APPL1/PROC1 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  ftloc1.in(),
                                  hello1APPL1.in());
            TEST_FAILED();
        }
        catch( CORBA::Exception& e )
        {
            TEST_SUCCEED();
        }

        TEST_INFO("Checking Fault Notifier invoked");
        if (fault_notifier_impl->get_count() == 1)
        {
            TEST_SUCCEED();
        }
        else
        {
            std::cerr << "Fault Notifier invoked " <<
                fault_notifier_impl->get_count() << " times.";
            TEST_FAILED();
        }

        TEST_INFO( "Trying to use client1" );
        try
        {
            hello1APPL1->hello();
            TEST_FAILED();
        }
        catch (const CORBA::Exception& e)
        {
            TEST_SUCCEED();
        }

        TEST_INFO( "Killing the client2" );
        OsSupport::OS::kill_process(pid2);
        OsSupport::OS::sleep(timescale*2000);
    }
    catch(const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        std::cerr << e.what() << std::endl;
        TEST_FAILED();
    }


}

Modifier::Modifier(CdmwReplicationManager::HelloInterface1_ptr hello)
    throw (OutOfMemoryException,
           InternalErrorException,
           BadParameterException)
    : m_hello(CdmwReplicationManager::HelloInterface1::_duplicate(hello))
{
}

void Modifier::run() throw()
{
    for (int i = 0; i < 50; i++)
    {
      try {
          std::cerr << "Modifier " << i << std::endl;
        m_hello->set_info_into_datastore(1, i + 1000, "hello world");
      } catch (const CORBA::SystemException & ex) {
	std::cerr << "Modifier[" << i << "] " << __FILE__ << " line: " << __LINE__
		  << "\n Caught Exception : " << ex << std::endl;
      }
      Cdmw::OsSupport::OS::sleep(500);
    }
}

}; // namespace FT
}; // namespace Cdmw

