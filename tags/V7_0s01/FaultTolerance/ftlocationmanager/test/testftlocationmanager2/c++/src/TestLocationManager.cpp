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

#include "testftlocationmanager2/TestHello_impl.hpp"
#include "testftlocationmanager2/TestFTActivationHandler_impl.hpp"

#include "testftlocationmanager2/TestLocationManager.hpp"
#include "testftlocationmanager2/TestClientThread.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>


namespace Cdmw {
namespace FT {

// constructor
TestLocationManager::TestLocationManager(
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
TestLocationManager::~TestLocationManager()
{
}

// determine of the process is into the list
bool TestLocationManager::contain(const std::string& process_name, const ::CdmwReplicationManager::LocationList* list)
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
void TestLocationManager::do_tests()
{
    set_nbOfRequestedTestOK (245);

    try
    {
        std::string iorhello1APPL1 = "hello1APPL1.ior";
        std::string iorhello1APPL2 = "hello1APPL2.ior";
        std::string iorhello2APPL1 = "hello2APPL1.ior";
        std::string iorhello2APPL2 = "hello2APPL2.ior";
        
        
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
                      << "could not _narrow object to type HelloInterface" << std::endl;
            abort();
        }
        
        TEST_SUCCEED();




        TEST_SUCCEED();
        
        // create a first object with an invalid criteria
        // the invalid criteria is the FaultMonitoringStyle 
        TEST_INFO("Create objects groups");

        const char * rid1 = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface1:1.0";
        const char * rid2 = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface2:1.0";

        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id2;

        CORBA::Object_var obj1;
        CORBA::Object_var obj2;
        
        // set default properties
        TEST_INFO( "set_default_properties" );


        ::FT::Properties prop;
    
        prop.length(2);
        prop[0].nam.length(1);
        prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop[0].val <<= (CORBA::UShort)2;

        std::vector<std::string> slocvect(3);
        slocvect[0] = "HOST1.hostname/APPL1.applicationname/PROC1.processname";
        slocvect[1] = "HOST2.hostname/APPL2.applicationname/PROC2.processname";
        slocvect[2] = "HOST3.hostname/APPL3.applicationname/PROC3.processname";


        const CORBA::ULong MAX_LOCS=slocvect.size();
        ::FT::FactoryInfos factoryInfos;
        factoryInfos.length(MAX_LOCS);
        for (CORBA::ULong i = 0; i < MAX_LOCS; ++i) {
            factoryInfos[i].the_factory = ::FT::GenericFactory::_nil();
            ::FT::Location_var loc = 
                  Cdmw::CommonSvcs::Naming::NamingInterface::to_name(slocvect[i]);
            std::cerr << '[' << i << "] " << slocvect[i] << " --- " 
                      << Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc.in()) << std::endl;

            factoryInfos[i].the_location = loc.in();
            ::FT::Criteria factoryCrit;        
            factoryCrit.length(0);
            factoryInfos[i].the_criteria = factoryCrit;
        }
    
        prop[1].nam.length(1);
        prop[1].nam[0].id="org.omg.ft.Factories";
        prop[1].val <<= factoryInfos;

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
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        

        dsids[0] = 3;
        dsids[1] = 4;

        ::FT::Criteria crit2;        
        crit2.length(2);
        crit2[0].nam.length(1);
        crit2[0].nam[0].id="org.omg.ft.FTProperties";
        crit2[0].val <<= prop;

        crit2[1].nam.length(1);
        crit2[1].nam[0].id="com.thalesgroup.cdmw.ft.DataStoreIdentifiers";
        crit2[1].val <<= dsids;

        try
        {
            obj2 =  rm->create_object(rid2,
                                      crit2,
                                      factory_creation_id2.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
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
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        
        ::FT::ObjectGroupId ogi2 = 0;
        try
        {
            ogi2 = rm->get_object_group_id(obj2.in());
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }


        TEST_INFO("start clients");
        std::ostringstream appl1;
        std::ostringstream appl2;
        appl1<<"--client APPL1 --object_group_id1="<<ogi1<<" --object_group_id2="<<ogi2;
        appl2<<"--client APPL2 --object_group_id1="<<ogi1<<" --object_group_id2="<<ogi2;

        OsSupport::OS::ProcessId pid1  = 
        OsSupport::OS::create_process( m_process_name, appl1.str());
        sleep(2);
        OsSupport::OS::ProcessId pid2  = 
        OsSupport::OS::create_process( m_process_name, appl2.str());   
        sleep(4);
        TEST_SUCCEED();

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*1000);
        

        
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
        


        // create a reference for the PrimaryBackupGroupRepository
        ::FT::Location loc;
        loc.length(3);
        loc[0].kind = "hostname";
        loc[1].kind = "applicationname";
        loc[2].kind = "processname";
 
        loc[0].id = "HOST3";
        loc[1].id = "APPL3";
        loc[2].id = "PROC3";

        Cdmw::FT::DataStoreBase::Set_Location_Name(Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc));

        ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primaryBackupGroupRepository_impl
          = ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();

		  // export GroupRepository in orb
		  CdmwFT::Location::PrimaryBackupGroupRepository_var primary_backup_rep_ref
			  = primaryBackupGroupRepository_impl;
		  
		  m_orb->register_initial_reference("FTGroupRepository", 
				                              primary_backup_rep_ref.in());


        // create the reference for the StatefullPrimaryBackupAdmin
        ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl* statefullPrimaryBackupAdmin_impl
          = new ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl(m_orb.in(), m_rootPOA.in(), loc, primaryBackupGroupRepository_impl, true);
        
        CdmwFT::Location::StatefullPrimaryBackupAdmin_var statefullPrimaryBackupAdmin = statefullPrimaryBackupAdmin_impl->_this();
        


        Cdmw::HelloInterface1_impl * helloInterface1_impl = new Cdmw::HelloInterface1_impl(m_orb.in(), m_rootPOA.in());
        CdmwReplicationManager::HelloInterface1_var hello1APPL3 =  helloInterface1_impl->_this();

        Cdmw::HelloInterface2_impl * helloInterface2_impl = new Cdmw::HelloInterface2_impl(m_orb.in(), m_rootPOA.in());
        CdmwReplicationManager::HelloInterface2_var hello2APPL3 =  helloInterface2_impl->_this();

        // register the Member Admin (the PrimaryBackupAdmin) on the Replication Manager
        //rm->register_location(loc, primaryBackupAdmin.in());
        ::CdmwFT::StateTransfer::LocalDataStoreInfos infos;
        infos.length(2);
        infos[0] = *hello1APPL3->get_local_datastore_info();
        infos[1] = *hello2APPL3->get_local_datastore_info();

        rm->register_statefull_location(loc,
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
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }


        loc[0].id = "HOST2";
        loc[1].id = "APPL2";
        loc[2].id = "PROC2";


        // Add new member on the group
        TEST_INFO("add the new member HOST2/APPL2/PROC2 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1APPL2.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("Check is the datastore is activated or not");
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(4));

        TEST_CHECK(hello1APPL2->datastore_is_acticated(1));
        TEST_CHECK(hello1APPL2->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL2->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL2->datastore_is_acticated(4));

        TEST_CHECK(!hello1APPL3->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL3->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(4));
         
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

            TEST_INFO(hello1APPL1LocationList1->length());
            TEST_INFO(hello1APPL1LocationList2->length());
            TEST_INFO(hello2APPL1LocationList3->length());
            TEST_INFO(hello2APPL1LocationList4->length());
            TEST_INFO(hello1APPL2LocationList1->length());
            TEST_INFO(hello1APPL2LocationList2->length());
            TEST_INFO(hello2APPL2LocationList3->length());
            TEST_INFO(hello2APPL2LocationList4->length());
            TEST_INFO(hello1APPL3LocationList1->length());
            TEST_INFO(hello1APPL3LocationList2->length());
            TEST_INFO(hello2APPL3LocationList3->length());
            TEST_INFO(hello2APPL3LocationList4->length());

        }
        
        TEST_INFO("Set info into data store");
        try
        {
            hello1APPL2->set_info_into_datastore(1, 1, "one");
            hello1APPL2->set_info_into_datastore(2, 2, "two");
            hello1APPL2->set_info_into_datastore(1, 10, "one");
            hello1APPL2->set_info_into_datastore(2, 20, "two");
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }






        loc[0].id = "HOST3";
        loc[1].id = "APPL3";
        loc[2].id = "PROC3";

        // Add new member on the group
        TEST_INFO("add the new member HOST3/APPL3/PROC3 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1APPL3.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Check is the datastore is activated or not");
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(4));

        TEST_CHECK(hello1APPL2->datastore_is_acticated(1));
        TEST_CHECK(hello1APPL2->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL2->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL2->datastore_is_acticated(4));

        TEST_CHECK(!hello1APPL3->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL3->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(4));
         
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
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList2 =  hello1APPL2->get_location_list(2);
            TEST_CHECK(hello1APPL2LocationList2->length() == 1);
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

            TEST_INFO(hello1APPL1LocationList1->length());
            TEST_INFO(hello1APPL1LocationList2->length());
            TEST_INFO(hello2APPL1LocationList3->length());
            TEST_INFO(hello2APPL1LocationList4->length());
            TEST_INFO(hello1APPL2LocationList1->length());
            TEST_INFO(hello1APPL2LocationList2->length());
            TEST_INFO(hello2APPL2LocationList3->length());
            TEST_INFO(hello2APPL2LocationList4->length());
            TEST_INFO(hello1APPL3LocationList1->length());
            TEST_INFO(hello1APPL3LocationList2->length());
            TEST_INFO(hello2APPL3LocationList3->length());
            TEST_INFO(hello2APPL3LocationList4->length());
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



        // Add new member on the group
        TEST_INFO("get the reference of the object group 2");
        try
        {
            obj2 = rm->get_object_group_ref(obj2.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }


        loc[0].id = "HOST2";
        loc[1].id = "APPL2";
        loc[2].id = "PROC2";


        // Add new member on the group
        TEST_INFO("add the new member HOST2/APPL2/PROC2 in group 2");
        try
        {
            obj2 = rm->add_member(obj2.in(),
                                  loc,
                                  hello2APPL2.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("Check is the datastore is activated or not");
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(4));

        TEST_CHECK(hello1APPL2->datastore_is_acticated(1));
        TEST_CHECK(hello1APPL2->datastore_is_acticated(2));
        TEST_CHECK(hello2APPL2->datastore_is_acticated(3));
        TEST_CHECK(hello2APPL2->datastore_is_acticated(4));

        TEST_CHECK(!hello1APPL3->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL3->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(4));
         
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
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList2 =  hello1APPL2->get_location_list(2);
            TEST_CHECK(hello1APPL2LocationList2->length() == 1);
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

            TEST_INFO(hello1APPL1LocationList1->length());
            TEST_INFO(hello1APPL1LocationList2->length());
            TEST_INFO(hello2APPL1LocationList3->length());
            TEST_INFO(hello2APPL1LocationList4->length());
            TEST_INFO(hello1APPL2LocationList1->length());
            TEST_INFO(hello1APPL2LocationList2->length());
            TEST_INFO(hello2APPL2LocationList3->length());
            TEST_INFO(hello2APPL2LocationList4->length());
            TEST_INFO(hello1APPL3LocationList1->length());
            TEST_INFO(hello1APPL3LocationList2->length());
            TEST_INFO(hello2APPL3LocationList3->length());
            TEST_INFO(hello2APPL3LocationList4->length());
        }
        
        TEST_INFO("Set info into data store");
        try
        {
            hello2APPL2->set_info_into_datastore(3, 1, "one");
            hello2APPL2->set_info_into_datastore(4, 2, "two");
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }




        loc[0].id = "HOST1";
        loc[1].id = "APPL1";
        loc[2].id = "PROC1";

        // Add new member on the group
        TEST_INFO("add the new member HOST1/APPL1/PROC1 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1APPL1.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("Check is the datastore is activated or not");
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(4));

        TEST_CHECK(hello1APPL2->datastore_is_acticated(1));
        TEST_CHECK(hello1APPL2->datastore_is_acticated(2));
        TEST_CHECK(hello2APPL2->datastore_is_acticated(3));
        TEST_CHECK(hello2APPL2->datastore_is_acticated(4));

        TEST_CHECK(!hello1APPL3->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL3->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(4));
         
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
            TEST_CHECK(hello1APPL2LocationList1->length() == 2);
            TEST_CHECK(contain("HOST1.hostname/APPL1.applicationname/PROC1.processname", hello1APPL2LocationList1));
            TEST_CHECK(contain("HOST3.hostname/APPL3.applicationname/PROC3.processname", hello1APPL2LocationList1));
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList2 =  hello1APPL2->get_location_list(2);
            TEST_CHECK(hello1APPL2LocationList2->length() == 2);
            TEST_CHECK(contain("HOST1.hostname/APPL1.applicationname/PROC1.processname", hello1APPL2LocationList2));
            TEST_CHECK(contain("HOST3.hostname/APPL3.applicationname/PROC3.processname", hello1APPL2LocationList2));
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

            TEST_INFO(hello1APPL1LocationList1->length());
            TEST_INFO(hello1APPL1LocationList2->length());
            TEST_INFO(hello2APPL1LocationList3->length());
            TEST_INFO(hello2APPL1LocationList4->length());
            TEST_INFO(hello1APPL2LocationList1->length());
            TEST_INFO(hello1APPL2LocationList2->length());
            TEST_INFO(hello2APPL2LocationList3->length());
            TEST_INFO(hello2APPL2LocationList4->length());
            TEST_INFO(hello1APPL3LocationList1->length());
            TEST_INFO(hello1APPL3LocationList2->length());
            TEST_INFO(hello2APPL3LocationList3->length());
            TEST_INFO(hello2APPL3LocationList4->length());
        }
        

        TEST_INFO("Get info from data store");
        try
        { 
            TEST_INFO("data for oid 1 (datastore1):"<<hello1APPL3->get_info_from_datastore(1, 1)<<" -> expected \"one2\"");
            TEST_CHECK(!strcmp(hello1APPL3->get_info_from_datastore(1, 1),"one2"));
            TEST_INFO("data for oid 2 (datastore2):"<<hello1APPL3->get_info_from_datastore(2, 2)<<" -> expected \"two2\"");
            TEST_CHECK(!strcmp(hello1APPL3->get_info_from_datastore(2, 2),"two2"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello1APPL1->get_info_from_datastore(1, 1)<<" -> expected \"one2\"");
            TEST_CHECK(!strcmp(hello1APPL1->get_info_from_datastore(1, 1),"one2"));
            TEST_INFO("data for oid 2 (datastore2):"<<hello1APPL1->get_info_from_datastore(2, 2)<<" -> expected \"two2\"");
            TEST_CHECK(!strcmp(hello1APPL1->get_info_from_datastore(2, 2),"two2"));
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
            hello1APPL2->set_info_into_datastore(1, 1, "one3");
            hello1APPL2->set_info_into_datastore(2, 2, "two3");
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello1APPL3->get_info_from_datastore(1, 1)<<" -> expected \"one3\"");
            TEST_CHECK(!strcmp(hello1APPL3->get_info_from_datastore(1, 1),"one3"));
            TEST_INFO("data for oid 2 (datastore2):"<<hello1APPL3->get_info_from_datastore(2, 2)<<" -> expected \"two3\"");
            TEST_CHECK(!strcmp(hello1APPL3->get_info_from_datastore(2, 2),"two3"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello1APPL1->get_info_from_datastore(1, 1)<<" -> expected \"one3\"");
            TEST_CHECK(!strcmp(hello1APPL1->get_info_from_datastore(1, 1),"one3"));
            TEST_INFO("data for oid 2 (datastore2):"<<hello1APPL1->get_info_from_datastore(2, 2)<<" -> expected \"two3\"");
            TEST_CHECK(!strcmp(hello1APPL1->get_info_from_datastore(2, 2),"two3"));
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }








        loc[0].id = "HOST3";
        loc[1].id = "APPL3";
        loc[2].id = "PROC3";

        // Add new member on the group
        TEST_INFO("add the new member HOST3/APPL3/PROC3 in group 2");
        try
        {
            obj2 = rm->add_member(obj2.in(),
                                  loc,
                                  hello2APPL3.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Check is the datastore is activated or not");
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(4));

        TEST_CHECK(hello1APPL2->datastore_is_acticated(1));
        TEST_CHECK(hello1APPL2->datastore_is_acticated(2));
        TEST_CHECK(hello2APPL2->datastore_is_acticated(3));
        TEST_CHECK(hello2APPL2->datastore_is_acticated(4));

        TEST_CHECK(!hello1APPL3->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL3->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(4));
         
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
            TEST_CHECK(hello1APPL2LocationList1->length() == 2);
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList2 =  hello1APPL2->get_location_list(2);
            TEST_CHECK(hello1APPL2LocationList2->length() == 2);
            ::CdmwReplicationManager::LocationList* hello2APPL2LocationList3 =  hello2APPL2->get_location_list(3);
            TEST_CHECK(hello2APPL2LocationList3->length() == 1);
            ::CdmwReplicationManager::LocationList* hello2APPL2LocationList4 =  hello2APPL2->get_location_list(4);
            TEST_CHECK(hello2APPL2LocationList4->length() == 1);
            
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList1 =  hello1APPL3->get_location_list(1);
            TEST_CHECK(hello1APPL3LocationList1->length() == 2);
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList2 =  hello1APPL3->get_location_list(2);
            TEST_CHECK(hello1APPL3LocationList2->length() == 2);
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList3 =  hello2APPL3->get_location_list(3);
            TEST_CHECK(hello2APPL3LocationList3->length() == 1);
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList4 =  hello2APPL3->get_location_list(4);
            TEST_CHECK(hello2APPL3LocationList4->length() == 1);

            TEST_INFO(hello1APPL1LocationList1->length());
            TEST_INFO(hello1APPL1LocationList2->length());
            TEST_INFO(hello2APPL1LocationList3->length());
            TEST_INFO(hello2APPL1LocationList4->length());
            TEST_INFO(hello1APPL2LocationList1->length());
            TEST_INFO(hello1APPL2LocationList2->length());
            TEST_INFO(hello2APPL2LocationList3->length());
            TEST_INFO(hello2APPL2LocationList4->length());
            TEST_INFO(hello1APPL3LocationList1->length());
            TEST_INFO(hello1APPL3LocationList2->length());
            TEST_INFO(hello2APPL3LocationList3->length());
            TEST_INFO(hello2APPL3LocationList4->length());
        }
        

        
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore3):"<<hello2APPL3->get_info_from_datastore(3, 1)<<" -> expected \"one\"");
            TEST_CHECK(!strcmp(hello2APPL3->get_info_from_datastore(3, 1),"one"));
            TEST_INFO("data for oid 2 (datastore4):"<<hello2APPL3->get_info_from_datastore(4, 2)<<" -> expected \"two\"");
            TEST_CHECK(!strcmp(hello2APPL3->get_info_from_datastore(4, 2),"two"));
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
            hello2APPL2->set_info_into_datastore(3, 1, "one2");
            hello2APPL2->set_info_into_datastore(4, 2, "two2");
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore3):"<<hello2APPL3->get_info_from_datastore(3, 1)<<" -> expected \"one2\"");
            TEST_CHECK(!strcmp(hello2APPL3->get_info_from_datastore(3, 1),"one2"));
            TEST_INFO("data for oid 2 (datastore4):"<<hello2APPL3->get_info_from_datastore(4, 2)<<" -> expected \"two2\"");
            TEST_CHECK(!strcmp(hello2APPL3->get_info_from_datastore(4, 2),"two2"));
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        loc[0].id = "HOST1";
        loc[1].id = "APPL1";
        loc[2].id = "PROC1";

        // Add new member on the group
        TEST_INFO("add the new member HOST1/APPL1/PROC1 in group 2");
        try
        {
            obj2 = rm->add_member(obj2.in(),
                                  loc,
                                  hello2APPL1.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("Check is the datastore is activated or not");
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL1->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL1->datastore_is_acticated(4));

        TEST_CHECK(hello1APPL2->datastore_is_acticated(1));
        TEST_CHECK(hello1APPL2->datastore_is_acticated(2));
        TEST_CHECK(hello2APPL2->datastore_is_acticated(3));
        TEST_CHECK(hello2APPL2->datastore_is_acticated(4));

        TEST_CHECK(!hello1APPL3->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL3->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(4));
         
        TEST_INFO("Check the number of location in each datastore");
        {
            ::CdmwReplicationManager::LocationList* hello1APPL1LocationList1 =  hello1APPL1->get_location_list(1);
            TEST_CHECK(hello1APPL1LocationList1->length() == 2);
            ::CdmwReplicationManager::LocationList* hello1APPL1LocationList2 =  hello1APPL1->get_location_list(2);
            TEST_CHECK(hello1APPL1LocationList2->length() == 2);
            ::CdmwReplicationManager::LocationList* hello2APPL1LocationList3 =  hello2APPL1->get_location_list(3);
            TEST_CHECK(hello2APPL1LocationList3->length() == 2);
            TEST_CHECK(contain("HOST2.hostname/APPL2.applicationname/PROC2.processname", hello2APPL1LocationList3));
            TEST_CHECK(contain("HOST3.hostname/APPL3.applicationname/PROC3.processname", hello2APPL1LocationList3));
            ::CdmwReplicationManager::LocationList* hello2APPL1LocationList4 =  hello2APPL1->get_location_list(4);
            TEST_CHECK(contain("HOST2.hostname/APPL2.applicationname/PROC2.processname", hello2APPL1LocationList4));
            TEST_CHECK(contain("HOST3.hostname/APPL3.applicationname/PROC3.processname", hello2APPL1LocationList4));
            TEST_CHECK(hello2APPL1LocationList4->length() == 2);
            
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList1 =  hello1APPL2->get_location_list(1);
            TEST_CHECK(hello1APPL2LocationList1->length() == 2);
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList2 =  hello1APPL2->get_location_list(2);
            TEST_CHECK(hello1APPL2LocationList2->length() == 2);
            ::CdmwReplicationManager::LocationList* hello2APPL2LocationList3 =  hello2APPL2->get_location_list(3);
            TEST_CHECK(hello2APPL2LocationList3->length() == 2);
            TEST_CHECK(contain("HOST1.hostname/APPL1.applicationname/PROC1.processname", hello2APPL2LocationList3));
            TEST_CHECK(contain("HOST3.hostname/APPL3.applicationname/PROC3.processname", hello2APPL2LocationList3));
            ::CdmwReplicationManager::LocationList* hello2APPL2LocationList4 =  hello2APPL2->get_location_list(4);
            TEST_CHECK(hello2APPL2LocationList4->length() == 2);
            TEST_CHECK(contain("HOST1.hostname/APPL1.applicationname/PROC1.processname", hello2APPL2LocationList4));
            TEST_CHECK(contain("HOST3.hostname/APPL3.applicationname/PROC3.processname", hello2APPL2LocationList4));
            
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList1 =  hello1APPL3->get_location_list(1);
            TEST_CHECK(hello1APPL3LocationList1->length() == 2);
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList2 =  hello1APPL3->get_location_list(2);
            TEST_CHECK(hello1APPL3LocationList2->length() == 2);
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList3 =  hello2APPL3->get_location_list(3);
            TEST_CHECK(hello2APPL3LocationList3->length() == 2);
            TEST_CHECK(contain("HOST1.hostname/APPL1.applicationname/PROC1.processname", hello2APPL3LocationList3));
            TEST_CHECK(contain("HOST2.hostname/APPL2.applicationname/PROC2.processname", hello2APPL3LocationList3));
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList4 =  hello2APPL3->get_location_list(4);
            TEST_CHECK(hello2APPL3LocationList4->length() == 2);
            TEST_CHECK(contain("HOST1.hostname/APPL1.applicationname/PROC1.processname", hello2APPL3LocationList4));
            TEST_CHECK(contain("HOST2.hostname/APPL2.applicationname/PROC2.processname", hello2APPL3LocationList4));

            TEST_INFO(hello1APPL1LocationList1->length());
            TEST_INFO(hello1APPL1LocationList2->length());
            TEST_INFO(hello2APPL1LocationList3->length());
            TEST_INFO(hello2APPL1LocationList4->length());
            TEST_INFO(hello1APPL2LocationList1->length());
            TEST_INFO(hello1APPL2LocationList2->length());
            TEST_INFO(hello2APPL2LocationList3->length());
            TEST_INFO(hello2APPL2LocationList4->length());
            TEST_INFO(hello1APPL3LocationList1->length());
            TEST_INFO(hello1APPL3LocationList2->length());
            TEST_INFO(hello2APPL3LocationList3->length());
            TEST_INFO(hello2APPL3LocationList4->length());
        }
        

        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore3):"<<hello2APPL3->get_info_from_datastore(3, 1)<<" -> expected \"one2\"");
            TEST_CHECK(!strcmp(hello2APPL3->get_info_from_datastore(3, 1),"one2"));
            TEST_INFO("data for oid 2 (datastore4):"<<hello2APPL3->get_info_from_datastore(4, 2)<<" -> expected \"two2\"");
            TEST_CHECK(!strcmp(hello2APPL3->get_info_from_datastore(4, 2),"two2"));
            TEST_INFO("data for oid 1 (datastore3):"<<hello2APPL1->get_info_from_datastore(3, 1)<<" -> expected \"one2\"");
            TEST_CHECK(!strcmp(hello2APPL1->get_info_from_datastore(3, 1),"one2"));
            TEST_INFO("data for oid 2 (datastore4):"<<hello2APPL1->get_info_from_datastore(4, 2)<<" -> expected \"two2\"");
            TEST_CHECK(!strcmp(hello2APPL1->get_info_from_datastore(4, 2),"two2"));
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
            hello2APPL2->set_info_into_datastore(3, 1, "one3");
            hello2APPL2->set_info_into_datastore(4, 2, "two3");
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore3):"<<hello2APPL3->get_info_from_datastore(3, 1)<<" -> expected \"one3\"");
            TEST_CHECK(!strcmp(hello2APPL3->get_info_from_datastore(3, 1),"one3"));
            TEST_INFO("data for oid 2 (datastore4):"<<hello2APPL3->get_info_from_datastore(4, 2)<<" -> expected \"two3\"");
            TEST_CHECK(!strcmp(hello2APPL3->get_info_from_datastore(4, 2),"two3"));
            TEST_INFO("data for oid 1 (datastore3):"<<hello2APPL1->get_info_from_datastore(3, 1)<<" -> expected \"one3\"");
            TEST_CHECK(!strcmp(hello2APPL1->get_info_from_datastore(3, 1),"one3"));
            TEST_INFO("data for oid 2 (datastore4):"<<hello2APPL1->get_info_from_datastore(4, 2)<<" -> expected \"two3\"");
            TEST_CHECK(!strcmp(hello2APPL1->get_info_from_datastore(4, 2),"two3"));
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }


        TEST_INFO("Realise an on_failure on the replication manager for a backup process");

        loc[0].id = "HOST1";
        loc[1].id = "APPL1";
        loc[2].id = "PROC1";

        CosNotification::StructuredEvent fault_event;
        fault_event.header.fixed_header.event_type.domain_name = "FT_CORBA";
        fault_event.header.fixed_header.event_type.type_name = "ObjectCrashFault";
        fault_event.filterable_data.length(2);
        fault_event.filterable_data[0].name = "FTDomainId";
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = "Location";
        fault_event.filterable_data[1].value <<= loc;
        
        ::FT::FaultNotifier_var fn = rm->get_fault_notifier();

        fn->push_structured_fault( fault_event );
        
        OsSupport::OS::sleep(timescale*2000);

        
        TEST_INFO("Check is the datastore is activated or not");
        TEST_CHECK(hello1APPL2->datastore_is_acticated(1));
        TEST_CHECK(hello1APPL2->datastore_is_acticated(2));
        TEST_CHECK(hello2APPL2->datastore_is_acticated(3));
        TEST_CHECK(hello2APPL2->datastore_is_acticated(4));

        TEST_CHECK(!hello1APPL3->datastore_is_acticated(1));
        TEST_CHECK(!hello1APPL3->datastore_is_acticated(2));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(3));
        TEST_CHECK(!hello2APPL3->datastore_is_acticated(4));
         
        TEST_INFO("Check the number of location in each datastore");
        {
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList1 =  hello1APPL2->get_location_list(1);
            TEST_CHECK(hello1APPL2LocationList1->length() == 1);
            TEST_CHECK(contain("HOST3.hostname/APPL3.applicationname/PROC3.processname", hello1APPL2LocationList1));
            ::CdmwReplicationManager::LocationList* hello1APPL2LocationList2 =  hello1APPL2->get_location_list(2);
            TEST_CHECK(hello1APPL2LocationList2->length() == 1);
            TEST_CHECK(contain("HOST3.hostname/APPL3.applicationname/PROC3.processname",hello1APPL2LocationList2 ));
            ::CdmwReplicationManager::LocationList* hello2APPL2LocationList3 =  hello2APPL2->get_location_list(3);
            TEST_CHECK(hello2APPL2LocationList3->length() == 1);
            TEST_CHECK(contain("HOST3.hostname/APPL3.applicationname/PROC3.processname", hello2APPL2LocationList3));
            ::CdmwReplicationManager::LocationList* hello2APPL2LocationList4 =  hello2APPL2->get_location_list(4);
            TEST_CHECK(hello2APPL2LocationList4->length() == 1);
            TEST_CHECK(contain("HOST3.hostname/APPL3.applicationname/PROC3.processname", hello2APPL2LocationList4));
            
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList1 =  hello1APPL3->get_location_list(1);
            TEST_CHECK(hello1APPL3LocationList1->length() == 1);
            TEST_CHECK(contain("HOST2.hostname/APPL2.applicationname/PROC2.processname", hello1APPL3LocationList1));
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList2 =  hello1APPL3->get_location_list(2);
            TEST_CHECK(hello1APPL3LocationList2->length() == 1);
            TEST_CHECK(contain("HOST2.hostname/APPL2.applicationname/PROC2.processname", hello1APPL3LocationList2));
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList3 =  hello2APPL3->get_location_list(3);
            TEST_CHECK(hello2APPL3LocationList3->length() == 1);
            TEST_CHECK(contain("HOST2.hostname/APPL2.applicationname/PROC2.processname", hello2APPL3LocationList3));
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList4 =  hello2APPL3->get_location_list(4);
            TEST_CHECK(hello2APPL3LocationList4->length() == 1);
            TEST_CHECK(contain("HOST2.hostname/APPL2.applicationname/PROC2.processname", hello2APPL3LocationList4));

            TEST_INFO(hello1APPL2LocationList1->length());
            TEST_INFO(hello1APPL2LocationList2->length());
            TEST_INFO(hello2APPL2LocationList3->length());
            TEST_INFO(hello2APPL2LocationList4->length());
            TEST_INFO(hello1APPL3LocationList1->length());
            TEST_INFO(hello1APPL3LocationList2->length());
            TEST_INFO(hello2APPL3LocationList3->length());
            TEST_INFO(hello2APPL3LocationList4->length());
        }



        TEST_INFO("Realise an on_failure on the replication manager for a primary process");

        loc[0].id = "HOST2";
        loc[1].id = "APPL2";
        loc[2].id = "PROC2";

        fault_event.header.fixed_header.event_type.domain_name = "FT_CORBA";
        fault_event.header.fixed_header.event_type.type_name = "ObjectCrashFault";
        fault_event.filterable_data.length(2);
        fault_event.filterable_data[0].name = "FTDomainId";
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = "Location";
        fault_event.filterable_data[1].value <<= loc;
        
        fn->push_structured_fault( fault_event );

        OsSupport::OS::sleep(timescale*2000);

        
        TEST_INFO("Check is the datastore is activated or not");
        TEST_CHECK(hello1APPL3->datastore_is_acticated(1));
        TEST_CHECK(hello1APPL3->datastore_is_acticated(2));
        TEST_CHECK(hello2APPL3->datastore_is_acticated(3));
        TEST_CHECK(hello2APPL3->datastore_is_acticated(4));
         
        TEST_INFO("Check the number of location in each datastore");
        {
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList1 =  hello1APPL3->get_location_list(1);
            TEST_CHECK(hello1APPL3LocationList1->length() == 0);
            ::CdmwReplicationManager::LocationList* hello1APPL3LocationList2 =  hello1APPL3->get_location_list(2);
            TEST_CHECK(hello1APPL3LocationList2->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList3 =  hello2APPL3->get_location_list(3);
            TEST_CHECK(hello2APPL3LocationList3->length() == 0);
            ::CdmwReplicationManager::LocationList* hello2APPL3LocationList4 =  hello2APPL3->get_location_list(4);
            TEST_CHECK(hello2APPL3LocationList4->length() == 0);

            TEST_INFO(hello1APPL3LocationList1->length());
            TEST_INFO(hello1APPL3LocationList2->length());
            TEST_INFO(hello2APPL3LocationList3->length());
            TEST_INFO(hello2APPL3LocationList4->length());
        }

        TEST_INFO( "Killing the client1" );
        OsSupport::OS::kill_process( pid1);

        TEST_INFO( "Killing the client2" );
        OsSupport::OS::kill_process( pid2);

        
        TEST_INFO("Create a statefull_group_infos sequence");

        loc[0].id = "HOST3";
        loc[1].id = "APPL3";
        loc[2].id = "PROC3";
        
        TEST_INFO("Create a data store");
        typedef Cdmw::FT::DataStore<int, std::string> TestDataStore;
        Cdmw::OsSupport::OS::Timeval cohort_timeout;
        cohort_timeout.seconds=10;
        cohort_timeout.microseconds=0;
        Cdmw::OsSupport::OS::Timeval coordinator_timeout;
        coordinator_timeout.seconds=10;
        coordinator_timeout.microseconds=0;

        Cdmw::FT::DataStoreContext context(m_orb.in(),
                                           m_rootPOA.in(),
                                           100, 100,
                                           cohort_timeout,
                                           coordinator_timeout,
                                           1);
        
        TestDataStore*  dataStoreObj = new TestDataStore(5,
                                                         m_orb.in(),
                                                         m_rootPOA.in(),
                                                         context);
        dataStoreObj->activate();

        CdmwFT::StateTransfer::DataStoreGroup_var  datastore_group
            = (new Cdmw::FT::DataStoreGroup_impl(m_rootPOA.in(),context))->_this();

        {
            CdmwFT::StateTransfer::LocalDataStoreInfo localdatastoreinfo;
            localdatastoreinfo.dsids.length(1);
            localdatastoreinfo.dsids[0] = 50;
            localdatastoreinfo.coordinator = NULL;
            localdatastoreinfo.cohort = NULL;
            localdatastoreinfo.local_data_store = NULL;
            
            
            CdmwFT::StateTransfer::LocalDataStoreInfos localdatastoreinfos;
            localdatastoreinfos.length(1);
            localdatastoreinfos[0] = localdatastoreinfo;
            
            
            CdmwFT::StateTransfer::DataStoreInfo datastoreinfo;
            datastoreinfo.the_location = loc;
            datastoreinfo.local_data_store_infos = localdatastoreinfos;
            
            
            CdmwFT::StateTransfer::DataStoreInfos datastoreinfos;
            datastoreinfos.length(1);
            datastoreinfos[0] = datastoreinfo;
            
            CdmwFT::Location::StatefullGroupInfo statefullGroupInfo;
            statefullGroupInfo.data_store_infos = datastoreinfos;
            
            CdmwFT::Location::StatefullGroupInfos statefullGroupInfos;
            statefullGroupInfos.length(1);
            statefullGroupInfos[0] = statefullGroupInfo;
            
            TEST_INFO("update the statefullPrimaryBackupAdmin with a statefullGroupInfos sequences where an unknow dsid is inserted");
            try
            {
                statefullPrimaryBackupAdmin->update_statefull_group_view(statefullGroupInfos);
                TEST_FAILED();
            }
            catch(const CdmwFT::Location::UnknownDataStoreID&)
            {
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
        }

        
        {
            CdmwFT::StateTransfer::LocalDataStoreInfo localdatastoreinfo;
            localdatastoreinfo.dsids.length(1);
            localdatastoreinfo.dsids[0] = 5;
            localdatastoreinfo.coordinator = NULL;
            localdatastoreinfo.cohort = NULL;
            localdatastoreinfo.local_data_store = NULL;
            
            
            CdmwFT::StateTransfer::LocalDataStoreInfos localdatastoreinfos;
            localdatastoreinfos.length(1);
            localdatastoreinfos[0] = localdatastoreinfo;
            
            
            CdmwFT::StateTransfer::DataStoreInfo datastoreinfo;
            datastoreinfo.the_location = loc;
            datastoreinfo.local_data_store_infos = localdatastoreinfos;
            
            
            CdmwFT::StateTransfer::DataStoreInfos datastoreinfos;
            datastoreinfos.length(1);
            datastoreinfos[0] = datastoreinfo;
            
            CdmwFT::Location::StatefullGroupInfo statefullGroupInfo;
            statefullGroupInfo.data_store_infos = datastoreinfos;
            
            CdmwFT::Location::StatefullGroupInfos statefullGroupInfos;
            statefullGroupInfos.length(1);
            statefullGroupInfos[0] = statefullGroupInfo;
            
            TEST_INFO("update the statefullPrimaryBackupAdmin with a statefullGroupInfos sequences where an unknow dsid is inserted");
            try
            {
                statefullPrimaryBackupAdmin->update_statefull_group_view(statefullGroupInfos);
                TEST_FAILED();
            }
            catch(const CdmwFT::StateTransfer::InvalidDataStoreState&)
            {
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
        }
        
        {
            CdmwFT::StateTransfer::LocalDataStoreInfo localdatastoreinfo;
            localdatastoreinfo.dsids.length(1);
            localdatastoreinfo.dsids[0] = 50;
            localdatastoreinfo.coordinator = NULL;
            localdatastoreinfo.cohort = NULL;
            localdatastoreinfo.local_data_store = NULL;
            
            
            CdmwFT::StateTransfer::LocalDataStoreInfos localdatastoreinfos;
            localdatastoreinfos.length(1);
            localdatastoreinfos[0] = localdatastoreinfo;
            
            
            CdmwFT::StateTransfer::DataStoreInfo datastoreinfo;
            datastoreinfo.the_location = loc;
            datastoreinfo.local_data_store_infos = localdatastoreinfos;
            
            
            CdmwFT::StateTransfer::DataStoreInfos datastoreinfos;
            datastoreinfos.length(1);
            datastoreinfos[0] = datastoreinfo;
            
            CdmwFT::Location::StatefullGroupInfo statefullGroupInfo;
            statefullGroupInfo.data_store_infos = datastoreinfos;
            
            CdmwFT::Location::StatefullGroupInfos statefullGroupInfos;
            statefullGroupInfos.length(1);
            statefullGroupInfos[0] = statefullGroupInfo;
            
            TEST_INFO("update the statefullPrimaryBackupAdmin with a statefullGroupInfos sequences where an unknow dsid is inserted");
            try
            {
                statefullPrimaryBackupAdmin->update_statefull_primary(statefullGroupInfos, loc);
                TEST_FAILED();
            }
            catch(const CdmwFT::Location::UnknownDataStoreID&)
            {
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
        }

        {
            CdmwFT::StateTransfer::LocalDataStoreInfo localdatastoreinfo;
            localdatastoreinfo.dsids.length(1);
            localdatastoreinfo.dsids[0] = 5;
            localdatastoreinfo.coordinator = NULL;
            localdatastoreinfo.cohort = NULL;
            localdatastoreinfo.local_data_store = NULL;
            
            
            CdmwFT::StateTransfer::LocalDataStoreInfos localdatastoreinfos;
            localdatastoreinfos.length(1);
            localdatastoreinfos[0] = localdatastoreinfo;
            
            
            CdmwFT::StateTransfer::DataStoreInfo datastoreinfo;
            datastoreinfo.the_location = loc;
            datastoreinfo.local_data_store_infos = localdatastoreinfos;
            
            
            CdmwFT::StateTransfer::DataStoreInfos datastoreinfos;
            datastoreinfos.length(1);
            datastoreinfos[0] = datastoreinfo;
            
            CdmwFT::Location::StatefullGroupInfo statefullGroupInfo;
            statefullGroupInfo.data_store_infos = datastoreinfos;
            
            CdmwFT::Location::StatefullGroupInfos statefullGroupInfos;
            statefullGroupInfos.length(1);
            statefullGroupInfos[0] = statefullGroupInfo;
            
            TEST_INFO("update the statefullPrimaryBackupAdmin with a statefullGroupInfos sequences where an unknow dsid is inserted");
            try
            {
                statefullPrimaryBackupAdmin->update_statefull_primary(statefullGroupInfos, loc);
                TEST_FAILED();
            }
            catch(const CdmwFT::StateTransfer::InvalidDataStoreState&)
            {
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
        }


        {
            CdmwFT::StateTransfer::LocalDataStoreInfo localdatastoreinfo;
            localdatastoreinfo.dsids.length(1);
            localdatastoreinfo.dsids[0] = 5;
            localdatastoreinfo.coordinator = NULL;
            localdatastoreinfo.cohort = NULL;
            localdatastoreinfo.local_data_store = NULL;
            
            
            CdmwFT::StateTransfer::LocalDataStoreInfos localdatastoreinfos;
            localdatastoreinfos.length(1);
            localdatastoreinfos[0] = localdatastoreinfo;
            
            CdmwFT::StateTransfer::DataStoreInfo datastoreinfo;
            datastoreinfo.the_location = loc;
            datastoreinfo.local_data_store_infos = localdatastoreinfos;
            
            CdmwFT::StateTransfer::DataStoreInfos datastoreinfos;
            datastoreinfos.length(1);
            datastoreinfos[0] = datastoreinfo;
            
            CdmwFT::GroupInfo group_info;
            group_info.object_group_ref = CORBA::Object::_nil();
            group_info.object_group_id = 10;
            group_info.object_group_ref_version = 10;
            
            CdmwFT::Location::StatefullGroupInfo statefullGroupInfo;
            statefullGroupInfo.data_store_infos = datastoreinfos;
            statefullGroupInfo.group_info = group_info;

            CdmwFT::Location::StatefullGroupInfos statefullGroupInfos;
            statefullGroupInfos.length(1);
            statefullGroupInfos[0] = statefullGroupInfo;
            



            OsSupport::OS::ProcessId pid1  = 
            OsSupport::OS::create_process( m_process_name, "--init");
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            OsSupport::OS::sleep(timescale*3000);

            TEST_INFO("get the reference for the synchronisation object");
            std::string file_url = "file://init.ior";
            
            CORBA::Object_var object = m_orb->string_to_object(file_url.c_str());
            CdmwReplicationManager::HelloInterface3_var hello3 = CdmwReplicationManager::HelloInterface3::_narrow(object.in());
            
            ::CdmwFT::StateTransfer::LocalDataStoreInfo info;

            info = *hello3->get_local_datastore_info();




            CdmwFT::StateTransfer::LocalDataStoreInfos infos;
            infos.length(1);
            infos[0].dsids.length(1);
            infos[0].dsids[0] = 5;
            infos[0].coordinator = info.coordinator;
            infos[0].cohort = info.cohort;
            infos[0].local_data_store = info.local_data_store;
            CdmwFT::StateTransfer::DataStoreInfo primarydatastoreinfos;
            primarydatastoreinfos.the_location = loc;
            primarydatastoreinfos.local_data_store_infos = infos;


            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                statefullPrimaryBackupAdmin->init_backup_data_stores(
                      primarydatastoreinfos);
                TEST_SUCCEED();
            }
            catch (...)
            {
                TEST_FAILED();
            }

            TEST_INFO("update the statefullPrimaryBackupAdmin with a statefullGroupInfos sequences where an unknow dsid is inserted");
            try
            {
                statefullPrimaryBackupAdmin->update_statefull_primary(statefullGroupInfos, loc);
                TEST_FAILED();
            }
            catch(const ::FT::ObjectGroupNotFound&)
            {
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
            TEST_INFO("update the statefullPrimaryBackupAdmin with a statefullGroupInfos sequences where an unknow dsid is inserted");
            try
            {
                statefullPrimaryBackupAdmin->update_statefull_primary(statefullGroupInfos, loc);
                TEST_FAILED();
            }
            catch(const ::FT::ObjectGroupNotFound&)
            {
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
            TEST_INFO("update the statefullPrimaryBackupAdmin with a statefullGroupInfos sequences where an unknow dsid is inserted");
            try
            {
                statefullPrimaryBackupAdmin->update_statefull_group_view(statefullGroupInfos);
                TEST_FAILED();
            }
            catch(const ::FT::ObjectGroupNotFound&)
            {
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }

            OsSupport::OS::kill_process(pid1);
            OsSupport::OS::sleep(timescale*1500);
        }

        OsSupport::OS::sleep(timescale*2000);
    }
    
    catch(const CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        std::cerr << e.what() << std::endl;
        TEST_FAILED();
    }


}

}; // namespace FT
}; // namespace Cdmw

