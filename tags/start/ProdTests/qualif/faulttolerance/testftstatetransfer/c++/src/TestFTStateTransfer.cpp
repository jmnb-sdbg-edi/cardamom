/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
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
 * =========================================================================== */


#include <exception>
#include <iostream>
#include <sstream>


#include "idllib/CdmwFTReplicationManager.stub.hpp"

#include "testftstatetransfer/TestHello_impl.hpp"
//#include "testftlocationmanager2/TestFTActivationHandler_impl.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"

#include "testftstatetransfer/TestFTStateTransfer.hpp"
#include <Repository/naminginterface/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>

namespace Cdmw {
namespace FT {

// constructor
TestFTStateTransfer::TestFTStateTransfer(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name,
    const std::string& process_name,
    const std::string& host1,
    const std::string& host2,
    const std::string& host3 )
    : Testable( name ),
      m_process_name(process_name),
      m_host1(host1),
      m_host2(host2),
      m_host3(host3)
      
    
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

//destructor
TestFTStateTransfer::~TestFTStateTransfer()
{
}

// determine if the process is into the list
bool TestFTStateTransfer::contain(const std::string& process_name, const ::CdmwReplicationManager::LocationList* list)
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
void TestFTStateTransfer::do_tests()
{
    set_nbOfRequestedTestOK (206);

    try
    {
 
        
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
        slocvect[0] =  m_host1 +".hostname/APPL1.applicationname/P11.processname";
        slocvect[1] =  m_host2 +".hostname/APPL2.applicationname/P21.processname";
        slocvect[2] =  m_host3 +".hostname/APPL3.applicationname/P31.processname";


        const CORBA::ULong MAX_LOCS=slocvect.size();
        ::FT::FactoryInfos factoryInfos;
        factoryInfos.length(MAX_LOCS);
        for (CORBA::ULong i = 0; i < MAX_LOCS; ++i) {
            factoryInfos[i].the_factory = ::FT::GenericFactory::_nil();
            ::FT::Location_var loc = 
                  Cdmw::NamingAndRepository::NamingInterface::to_name(slocvect[i]);
            std::cerr << '[' << i << "] " << slocvect[i] << " --- " 
                      << Cdmw::NamingAndRepository::NamingInterface::to_string(loc.in()) << std::endl;

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


        CdmwReplicationManager::HelloInterface1_var hello1ObjectGroup = CdmwReplicationManager::HelloInterface1::_narrow(obj1.in());
        CdmwReplicationManager::HelloInterface2_var hello2ObjectGroup = CdmwReplicationManager::HelloInterface2::_narrow(obj2.in());


        std::cout << "//" << std::endl;
        std::cout << "// ###########################################################" << std::endl;
        std::cout << "// ##### FIRST STEP" << std::endl;
        std::cout << "//" << std::endl;

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*1000);


        TEST_INFO("Start server processes");
        
        std::ostringstream proc_init11;
        std::ostringstream proc_init21;
        std::ostringstream proc_init31;
        proc_init11<< " --proc-initialise APPL1 P11";
        proc_init21<< " --proc-initialise APPL2 P21";
        proc_init31<< " --proc-initialise APPL3 P31";

        OsSupport::OS::create_process( "platform_admin.sh" , proc_init11.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init21.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init31.str());   

        OsSupport::OS::sleep(timescale*20000);
        
        std::ostringstream proc_run11;
        std::ostringstream proc_run21;
        std::ostringstream proc_run31;
        proc_run11<< " --proc-run APPL1 P11";
        proc_run21<< " --proc-run APPL2 P21";
        proc_run31<< " --proc-run APPL3 P31";

        OsSupport::OS::create_process( "platform_admin.sh" , proc_run11.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run21.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run31.str());   

        OsSupport::OS::sleep(timescale*5000);

        TEST_INFO("client started")


        CdmwNamingAndRepository::Repository_var repository =
                                  CdmwNamingAndRepository::Repository::_nil();

        CORBA::Object_var obj_repository = m_orb->string_to_object("corbaloc::localhost:42562/CdmwRepository");
        repository = CdmwNamingAndRepository::Repository::_narrow(obj_repository.in());

        
        Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                              repository.in());
        
        Cdmw::NamingAndRepository::NamingInterface ni =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");
    

        std::string full_name = "dom3/P11HelloInterface1";
        
        CORBA::Object_var obj;
        
        try
        {
            obj = ni.resolve (full_name);
            TEST_SUCCEED();
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        
        CdmwReplicationManager::HelloInterface1_var hello1P11 = CdmwReplicationManager::HelloInterface1::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello1P11.in()) );


        full_name = "dom3/P11HelloInterface2";
        try
        {
            obj = ni.resolve (full_name);
            TEST_SUCCEED();
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }

        CdmwReplicationManager::HelloInterface2_var hello2P11 = CdmwReplicationManager::HelloInterface2::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello2P11.in()) );
 

        full_name = "dom3/P21HelloInterface1";
        
        
        try
        {
            obj = ni.resolve (full_name);
            TEST_SUCCEED();
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        
        CdmwReplicationManager::HelloInterface1_var hello1P21 = CdmwReplicationManager::HelloInterface1::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello1P21.in()) );


        full_name = "dom3/P21HelloInterface2";
        try
        {
            obj = ni.resolve (full_name);
            TEST_SUCCEED();
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }

        CdmwReplicationManager::HelloInterface2_var hello2P21 = CdmwReplicationManager::HelloInterface2::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello2P21.in()) );

        full_name = "dom3/P31HelloInterface1";
        
        
        try
        {
            obj = ni.resolve (full_name);
            TEST_SUCCEED();
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        
        CdmwReplicationManager::HelloInterface1_var hello1P31 = CdmwReplicationManager::HelloInterface1::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello1P11.in()) );


        full_name = "dom3/P31HelloInterface2";
        try
        {
            obj = ni.resolve (full_name);
            TEST_SUCCEED();
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }

        CdmwReplicationManager::HelloInterface2_var hello2P31 = CdmwReplicationManager::HelloInterface2::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello2P11.in()) );



//         TEST_INFO("start clients");
//         std::ostringstream appl1;
//         std::ostringstream appl2;
//         appl1<<"--client APPL1 --object_group_id1="<<ogi1<<" --object_group_id2="<<ogi2;
//         appl2<<"--client APPL2 --object_group_id1="<<ogi1<<" --object_group_id2="<<ogi2;

//         OsSupport::OS::ProcessId pid1  = 
//         OsSupport::OS::create_process( m_process_name, appl1.str());
//         sleep(2);
//         OsSupport::OS::ProcessId pid2  = 
//         OsSupport::OS::create_process( m_process_name, appl2.str());   
//         sleep(4);
//         TEST_SUCCEED();

//         int timescale = Cdmw::TestUtils::Testable::get_timescale();
//         OsSupport::OS::sleep(timescale*1000);
        

        
//         // wait creation of ior files
//         while (!(OsSupport::OS::file_exists(iorhello2APPL2) && OsSupport::OS::file_exists(iorhello1APPL2)))
//         {
//             OsSupport::OS::sleep(timescale*1000);
//         }

//         TEST_INFO("get the reference for the synchronisation object");
//         std::string file_url_prefix = "file://";        
//         std::string file_url = file_url_prefix;
        

//         TEST_INFO("get the reference for object to insert  into the object group");
//         file_url = file_url_prefix;
//         file_url += iorhello1APPL1;
        
//         CORBA::Object_var object_hello1APPL1 = m_orb->string_to_object(file_url.c_str());
//         CdmwReplicationManager::HelloInterface1_var hello1APPL1 = CdmwReplicationManager::HelloInterface1::_narrow(object_hello1APPL1.in());

//         file_url = file_url_prefix;
//         file_url += iorhello1APPL2;
        
//         CORBA::Object_var object_hello1APPL2 = m_orb->string_to_object(file_url.c_str());
//         CdmwReplicationManager::HelloInterface1_var hello1APPL2 = CdmwReplicationManager::HelloInterface1::_narrow(object_hello1APPL2.in());

//         file_url = file_url_prefix;
//         file_url += iorhello2APPL1;
        
//         CORBA::Object_var object_hello2APPL1 = m_orb->string_to_object(file_url.c_str());
//         CdmwReplicationManager::HelloInterface2_var hello2APPL1 = CdmwReplicationManager::HelloInterface2::_narrow(object_hello2APPL1.in());

//         file_url = file_url_prefix;
//         file_url += iorhello2APPL2;
        
//         CORBA::Object_var object_hello2APPL2 = m_orb->string_to_object(file_url.c_str());
//         CdmwReplicationManager::HelloInterface2_var hello2APPL2 = CdmwReplicationManager::HelloInterface2::_narrow(object_hello2APPL2.in());


//         TEST_SUCCEED();

//         TEST_INFO("client started")
        


//         // create a reference for the PrimaryBackupGroupRepository
//         ::FT::Location loc;
//         loc.length(3);
//         loc[0].id = "HOST3";
//         loc[1].id = "APPL3";
//         loc[2].id = "PROC3";

//         Cdmw::FT::DataStoreBase::Set_Location_Name(Cdmw::NamingAndRepository::NamingInterface::to_string(loc));

//         ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primaryBackupGroupRepository_impl
//           = ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::get_instance();

// 		  // export GroupRepository in orb
// 		  CdmwFT::Location::PrimaryBackupGroupRepository_var primary_backup_rep_ref
// 			  = primaryBackupGroupRepository_impl;
		  
// 		  m_orb->register_initial_reference("FTGroupRepository", 
// 				                              primary_backup_rep_ref.in());


//         // create the reference for the StatefullPrimaryBackupAdmin
//         ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl* statefullPrimaryBackupAdmin_impl
//           = new ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl(m_orb.in(), m_rootPOA.in(), loc, primaryBackupGroupRepository_impl, true);
        
//         CdmwFT::Location::StatefullPrimaryBackupAdmin_var statefullPrimaryBackupAdmin = statefullPrimaryBackupAdmin_impl->_this();
        


//         Cdmw::HelloInterface1_impl * helloInterface1_impl = new Cdmw::HelloInterface1_impl(m_orb.in(), m_rootPOA.in(), "APPL3");
//         CdmwReplicationManager::HelloInterface1_var hello1APPL3 =  helloInterface1_impl->_this();

//         Cdmw::HelloInterface2_impl * helloInterface2_impl = new Cdmw::HelloInterface2_impl(m_orb.in(), m_rootPOA.in(), "APPL3");
//         CdmwReplicationManager::HelloInterface2_var hello2APPL3 =  helloInterface2_impl->_this();

//         // register the Member Admin (the PrimaryBackupAdmin) on the Replication Manager
//         //rm->register_location(loc, primaryBackupAdmin.in());
//         ::CdmwFT::StateTransfer::LocalDataStoreInfos infos;
//         infos.length(4);
//         for (int i = 0; i< 2; i++)
//         {
//             ::CdmwFT::StateTransfer::LocalDataStoreInfo* info = hello1APPL3->get_local_datastore_info(i+1);
//             ::CdmwFT::StateTransfer::LocalDataStoreInfo local_info;
//             local_info.dsid = info->dsid;
//             local_info.coordinator = info->coordinator;
//             local_info.cohort = info->cohort;
//             local_info.local_data_store = info->local_data_store;
            
//             infos[i]= local_info;

//             info = hello2APPL3->get_local_datastore_info(i+3);
//             local_info.dsid = info->dsid;
//             local_info.coordinator = info->coordinator;
//             local_info.cohort = info->cohort;
//             local_info.local_data_store = info->local_data_store;
            
//             infos[i+2]= local_info;
//         }

//         rm->register_statefull_location(loc,
//                                         statefullPrimaryBackupAdmin.in(),
//                                         infos);






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

        ::FT::Location loc;
        loc.length(3);
        loc[0].id = m_host2.c_str();
        loc[0].kind = "hostname";
        loc[1].id = "APPL2";
        loc[1].kind = "applicationname";        
        loc[2].id = "P21";
        loc[2].kind = "processname";


        // Add new member on the group
        TEST_INFO("add the new member "<<m_host2<<"/APPL2/P21 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1P21.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        


        TEST_INFO("Set info into data store");
        try
        {
            hello1P21->trigger_on_insert(1, 1, "one1-1");
            hello1P21->trigger_on_insert(1, 2, "one2-1");
            hello1P21->trigger_on_insert(1, 3, "one3-1");
            hello1P21->trigger_on_remove(1, 3);

            hello1P21->trigger_on_insert(2, 1, "two1-1");
            hello1P21->trigger_on_insert(2, 2, "two2-1");
            hello1P21->trigger_on_insert(2, 3, "two3-1");
            hello1P21->trigger_on_remove(2, 3);

            TEST_SUCCEED();
            TEST_CHECK(!strcmp(hello1P21->trigger_on_insert(1, 1, "one1-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_remove(1, 3), "NotFoundException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_update(1, 10, "ten"), "NotFoundException"));

            TEST_CHECK(!strcmp(hello1P21->trigger_on_insert(2, 1, "two1-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_remove(2, 3), "NotFoundException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_update(2, 10, "ten"), "NotFoundException"));


            // to do 
            // realise an operation where a answer cohort in no turing firt phase commit
        }
        catch(...)
        {
            TEST_FAILED();
        }


        TEST_CHECK(hello1P21->callback_called(1, "insert") == 3);
        TEST_CHECK(hello1P21->callback_called(1, "update") == 0);
        TEST_CHECK(hello1P21->callback_called(1, "remove") == 1);
        TEST_CHECK(hello1P21->callback_called(2, "insert") == 3);
        TEST_CHECK(hello1P21->callback_called(2, "update") == 0);
        TEST_CHECK(hello1P21->callback_called(2, "remove") == 1);
        


        loc[0].id = m_host3.c_str();
        loc[0].kind = "hostname";
        loc[1].id = "APPL3";
        loc[1].kind = "applicationname";        
        loc[2].id = "P31";
        loc[2].kind = "processname";

        // Add new member on the group
        TEST_INFO("add the new member"<<m_host3<<"/APPL3/P31 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1P31.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P31->get_info_from_datastore(1, 1)<<" -> expected \"one1-1\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 1), "one1-1"));
            TEST_INFO("data for oid 2 (datastore1):"<<hello1P31->get_info_from_datastore(1, 2)<<" -> expected \"one2-1\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 2), "one2-1"));
            TEST_INFO("data for oid 2 (datastore1):"<<hello1P31->get_info_from_datastore(1, 3)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 3), "NotFoundException"));
            
            
            TEST_INFO("data for oid 1 (datastore2):"<<hello1P31->get_info_from_datastore(2, 1)<<" -> expected \"two1-1\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 1), "two1-1"));
            TEST_INFO("data for oid 2 (datastore2):"<<hello1P31->get_info_from_datastore(2, 2)<<" -> expected \"two2-1\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 2), "two2-1"));
            TEST_INFO("data for oid 2 (datastore2):"<<hello1P31->get_info_from_datastore(2, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 3), "NotFoundException"));
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Set info into data store");
        try
        {
            hello1P21->trigger_on_update(1, 1, "one1-2");
            hello1P21->trigger_on_remove(1, 2);
            hello1P21->trigger_on_insert(1, 4, "one4-1");
            
            hello1P21->trigger_on_update(2, 1, "two1-2");
            hello1P21->trigger_on_remove(2, 2);
            hello1P21->trigger_on_insert(2, 4, "two4-1");

            TEST_CHECK(!strcmp(hello1P21->trigger_on_insert(1, 4, "one4-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_remove(1, 2), "NotFoundException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_update(1, 10, "ten"), "NotFoundException"));

            TEST_CHECK(!strcmp(hello1P21->trigger_on_insert(2, 4, "two4-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_remove(2, 2), "NotFoundException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_update(2, 10, "ten"), "NotFoundException"));

            // to do 
            // realise an operation where a answer cohort in no turing firt phase commit

        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_CHECK(hello1P21->callback_called(1, "insert") == 1);
        TEST_CHECK(hello1P21->callback_called(1, "update") == 1);
        TEST_CHECK(hello1P21->callback_called(1, "remove") == 1);
        TEST_CHECK(hello1P21->callback_called(2, "insert") == 1);
        TEST_CHECK(hello1P21->callback_called(2, "update") == 1);
        TEST_CHECK(hello1P21->callback_called(2, "remove") == 1);

        TEST_CHECK(hello1P31->callback_called(1, "insert") == 3);
        TEST_CHECK(hello1P31->callback_called(1, "update") == 1);
        TEST_CHECK(hello1P31->callback_called(1, "remove") == 1);
        TEST_CHECK(hello1P31->callback_called(2, "insert") == 3);
        TEST_CHECK(hello1P31->callback_called(2, "update") == 1);
        TEST_CHECK(hello1P31->callback_called(2, "remove") == 1);


        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P31->get_info_from_datastore(1, 1)<<" -> expected \"one1-2\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 1), "one1-2"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P31->get_info_from_datastore(1, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore1):"<<hello1P31->get_info_from_datastore(1, 4)<<" -> expected \"one4-1\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 4), "one4-1"));


            TEST_INFO("data for oid 1 (datastore2):"<<hello1P31->get_info_from_datastore(2, 1)<<" -> expected \"two1-2\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 1), "two1-2"));
            TEST_INFO("data for oid 1 (datastore2):"<<hello1P31->get_info_from_datastore(2, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore2):"<<hello1P31->get_info_from_datastore(2, 4)<<" -> expected \"two4-1\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 4), "two4-1"));
        }
        catch(...)
        {
            TEST_FAILED();
        }


        loc[0].id = m_host1.c_str();
        loc[0].kind = "hostname";
        loc[1].id = "APPL1";
        loc[1].kind = "applicationname";        
        loc[2].id = "P11";
        loc[2].kind = "processname";

        // Add new member on the group
        TEST_INFO("add the new member"<<m_host1<<"/APPL1/P11 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1P11.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        


        TEST_INFO("Get info from data store");
        try
        { 

            TEST_INFO("data for oid 1 (datastore1):"<<hello1P31->get_info_from_datastore(1, 1)<<" -> expected \"one1-2\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 1), "one1-2"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P31->get_info_from_datastore(1, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore1):"<<hello1P31->get_info_from_datastore(1, 4)<<" -> expected \"one4-1\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 4), "one4-1"));

            TEST_INFO("data for oid 1 (datastore2):"<<hello1P31->get_info_from_datastore(2, 1)<<" -> expected \"two1-2\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 1), "two1-2"));
            TEST_INFO("data for oid 1 (datastore2):"<<hello1P31->get_info_from_datastore(2, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore2):"<<hello1P31->get_info_from_datastore(2, 4)<<" -> expected \"two4-1\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 4), "two4-1"));




            TEST_INFO("data for oid 1 (datastore1):"<<hello1P11->get_info_from_datastore(1, 1)<<" -> expected \"one1-2\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 1), "one1-2"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P11->get_info_from_datastore(1, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore1):"<<hello1P11->get_info_from_datastore(1, 4)<<" -> expected \"one4-1\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 4), "one4-1"));


            TEST_INFO("data for oid 1 (datastore2):"<<hello1P11->get_info_from_datastore(2, 1)<<" -> expected \"two1-2\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(2, 1), "two1-2"));
            TEST_INFO("data for oid 1 (datastore2):"<<hello1P11->get_info_from_datastore(2, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(2, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore2):"<<hello1P11->get_info_from_datastore(2, 4)<<" -> expected \"two4-1\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(2, 4), "two4-1"));
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Set info into data store");
        try
        {
            hello1P21->trigger_on_update(1, 1, "one1-3");
            hello1P21->trigger_on_remove(1, 4);
            hello1P21->trigger_on_insert(1, 5, "one5-1");

            hello1P21->trigger_on_update(2, 1, "two1-3");
            hello1P21->trigger_on_remove(2, 4);
            hello1P21->trigger_on_insert(2, 5, "two5-1");
            TEST_SUCCEED();

            TEST_CHECK(!strcmp(hello1P21->trigger_on_insert(1, 5, "one5-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_remove(1, 4), "NotFoundException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_update(1, 10, "ten"), "NotFoundException"));

            TEST_CHECK(!strcmp(hello1P21->trigger_on_insert(2, 5, "two5-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_remove(2, 4), "NotFoundException"));
            TEST_CHECK(!strcmp(hello1P21->trigger_on_update(2, 10, "ten"), "NotFoundException"));

            // to do 
            // realise an operation where a answer cohort in no turing firt phase commit

        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_CHECK(hello1P21->callback_called(1, "insert") == 1);
        TEST_CHECK(hello1P21->callback_called(1, "update") == 1);
        TEST_CHECK(hello1P21->callback_called(1, "remove") == 1);
        TEST_CHECK(hello1P21->callback_called(2, "insert") == 1);
        TEST_CHECK(hello1P21->callback_called(2, "update") == 1);
        TEST_CHECK(hello1P21->callback_called(2, "remove") == 1);

        TEST_CHECK(hello1P31->callback_called(1, "insert") == 1);
        TEST_CHECK(hello1P31->callback_called(1, "update") == 1);
        TEST_CHECK(hello1P31->callback_called(1, "remove") == 1);
        TEST_CHECK(hello1P31->callback_called(2, "insert") == 1);
        TEST_CHECK(hello1P31->callback_called(2, "update") == 1);
        TEST_CHECK(hello1P31->callback_called(2, "remove") == 1);

        TEST_CHECK(hello1P11->callback_called(1, "insert") == 3);
        TEST_CHECK(hello1P11->callback_called(1, "update") == 1);
        TEST_CHECK(hello1P11->callback_called(1, "remove") == 1);
        TEST_CHECK(hello1P11->callback_called(2, "insert") == 3);
        TEST_CHECK(hello1P11->callback_called(2, "update") == 1);
        TEST_CHECK(hello1P11->callback_called(2, "remove") == 1);

        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P31->get_info_from_datastore(1, 1)<<" -> expected \"one1-3\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 1), "one1-3"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P31->get_info_from_datastore(1, 4)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 4), "NotFoundException"));
            TEST_INFO("data for oid 5 (datastore1):"<<hello1P31->get_info_from_datastore(1, 5)<<" -> expected \"one5-1\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 5), "one5-1"));


            TEST_INFO("data for oid 1 (datastore2):"<<hello1P31->get_info_from_datastore(2, 1)<<" -> expected \"two1-3\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 1), "two1-3"));
            TEST_INFO("data for oid 1 (datastore2):"<<hello1P31->get_info_from_datastore(2, 4)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 4), "NotFoundException"));
            TEST_INFO("data for oid 5 (datastore2):"<<hello1P31->get_info_from_datastore(2, 5)<<" -> expected \"two5-1\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(2, 5), "two5-1"));




            TEST_INFO("data for oid 1 (datastore1):"<<hello1P11->get_info_from_datastore(1, 1)<<" -> expected \"one1-3\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 1), "one1-3"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P11->get_info_from_datastore(1, 4)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 4), "NotFoundException"));
            TEST_INFO("data for oid 5 (datastore1):"<<hello1P11->get_info_from_datastore(1, 5)<<" -> expected \"one5-1\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 5), "one5-1"));


            TEST_INFO("data for oid 1 (datastore2):"<<hello1P11->get_info_from_datastore(2, 1)<<" -> expected \"two1-3\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(2, 1), "two1-3"));
            TEST_INFO("data for oid 1 (datastore2):"<<hello1P11->get_info_from_datastore(2, 4)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(2, 4), "NotFoundException"));
            TEST_INFO("data for oid 5 (datastore2):"<<hello1P11->get_info_from_datastore(2, 5)<<" -> expected \"two5-1\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(2, 5), "two5-1"));

            
        }
        
        catch(...)
        {
            TEST_FAILED();
        }




        //
        // second part
        //



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


        loc[0].id = m_host2.c_str();
        loc[0].kind = "hostname";
        loc[1].id = "APPL2";
        loc[1].kind = "applicationname";        
        loc[2].id = "P21";
        loc[2].kind = "processname";


        // Add new member on the group
        TEST_INFO("add the new member "<<m_host2<<"/APPL2/P21 in group 2");
        try
        {
            obj2 = rm->add_member(obj2.in(),
                                  loc,
                                  hello2P21.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        


        
        TEST_INFO("Set info into data store");
        try
        {
            hello2P21->trigger_on_insert(3, 1, "three1-1");
            hello2P21->trigger_on_insert(3, 2, "three2-1");
            hello2P21->trigger_on_insert(3, 3, "three3-1");
            hello2P21->trigger_on_remove(3, 3);

            hello2P21->trigger_on_insert(4, 1, "four1-1");
            hello2P21->trigger_on_insert(4, 2, "four2-1");
            hello2P21->trigger_on_insert(4, 3, "four3-1");
            hello2P21->trigger_on_remove(4, 3);
            TEST_SUCCEED();

            TEST_CHECK(!strcmp(hello2P21->trigger_on_insert(3, 1, "three1-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_remove(3, 3), "NotFoundException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_update(3, 10, "ten"), "NotFoundException"));

            TEST_CHECK(!strcmp(hello2P21->trigger_on_insert(4, 1, "four1-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_remove(4, 3), "NotFoundException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_update(4, 10, "ten"), "NotFoundException"));

            // to do 
            // realise an operation where a answer cohort in no turing firt phase commit
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_CHECK(hello2P21->callback_called(3, "insert") == 3);
        TEST_CHECK(hello2P21->callback_called(3, "update") == 0);
        TEST_CHECK(hello2P21->callback_called(3, "remove") == 1);
        TEST_CHECK(hello2P21->callback_called(4, "insert") == 3);
        TEST_CHECK(hello2P21->callback_called(4, "update") == 0);
        TEST_CHECK(hello2P21->callback_called(4, "remove") == 1);


        loc[0].id = m_host3.c_str();
        loc[0].kind = "hostname";
        loc[1].id = "APPL3";
        loc[1].kind = "applicationname";        
        loc[2].id = "P31";
        loc[2].kind = "processname";


        // Add new member on the group
        TEST_INFO("add the new member "<<m_host3<<"/APPL3/P31 in group 2");
        try
        {
            obj2 = rm->add_member(obj2.in(),
                                  loc,
                                  hello2P31.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello2P31->get_info_from_datastore(3, 1)<<" -> expected \"three1-1\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 1), "three1-1"));
            TEST_INFO("data for oid 2 (datastore1):"<<hello2P31->get_info_from_datastore(3, 2)<<" -> expected \"three2-1\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 2), "three2-1"));
            TEST_INFO("data for oid 2 (datastore1):"<<hello2P31->get_info_from_datastore(3, 3)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 3), "NotFoundException"));
            
            
            TEST_INFO("data for oid 1 (datastore2):"<<hello2P31->get_info_from_datastore(4, 1)<<" -> expected \"four1-1\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 1), "four1-1"));
            TEST_INFO("data for oid 2 (datastore2):"<<hello2P31->get_info_from_datastore(4, 2)<<" -> expected \"four2-1\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 2), "four2-1"));
            TEST_INFO("data for oid 2 (datastore2):"<<hello2P31->get_info_from_datastore(4, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 3), "NotFoundException"));
        }
        catch(...)
        {
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Set info into data store");
        try
        {
            hello2P21->trigger_on_update(3, 1, "three1-2");
            hello2P21->trigger_on_remove(3, 2);
            hello2P21->trigger_on_insert(3, 4, "three4-1");
            
            hello2P21->trigger_on_update(4, 1, "four1-2");
            hello2P21->trigger_on_remove(4, 2);
            hello2P21->trigger_on_insert(4, 4, "four4-1");

            TEST_CHECK(!strcmp(hello2P21->trigger_on_insert(3, 4, "three4-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_remove(3, 2), "NotFoundException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_update(3, 10, "ten"), "NotFoundException"));

            TEST_CHECK(!strcmp(hello2P21->trigger_on_insert(4, 4, "four4-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_remove(4, 2), "NotFoundException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_update(4, 10, "ten"), "NotFoundException"));
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_CHECK(hello2P21->callback_called(3, "insert") == 1);
        TEST_CHECK(hello2P21->callback_called(3, "update") == 1);
        TEST_CHECK(hello2P21->callback_called(3, "remove") == 1);
        TEST_CHECK(hello2P21->callback_called(4, "insert") == 1);
        TEST_CHECK(hello2P21->callback_called(4, "update") == 1);
        TEST_CHECK(hello2P21->callback_called(4, "remove") == 1);

        TEST_CHECK(hello2P31->callback_called(3, "insert") == 3);
        TEST_CHECK(hello2P31->callback_called(3, "update") == 1);
        TEST_CHECK(hello2P31->callback_called(3, "remove") == 1);
        TEST_CHECK(hello2P31->callback_called(4, "insert") == 3);
        TEST_CHECK(hello2P31->callback_called(4, "update") == 1);
        TEST_CHECK(hello2P31->callback_called(4, "remove") == 1);

        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello2P31->get_info_from_datastore(3, 1)<<" -> expected \"three1-2\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 1), "three1-2"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello2P31->get_info_from_datastore(3, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore1):"<<hello2P31->get_info_from_datastore(3, 4)<<" -> expected \"three4-1\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 4), "three4-1"));


            TEST_INFO("data for oid 1 (datastore2):"<<hello2P31->get_info_from_datastore(4, 1)<<" -> expected \"four1-2\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 1), "four1-2"));
            TEST_INFO("data for oid 1 (datastore2):"<<hello2P31->get_info_from_datastore(4, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore2):"<<hello2P31->get_info_from_datastore(4, 4)<<" -> expected \"four4-1\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 4), "four4-1"));
        }
        catch(...)
        {
            TEST_FAILED();
        }

        loc[0].id = m_host1.c_str();
        loc[0].kind = "hostname";
        loc[1].id = "APPL1";
        loc[1].kind = "applicationname";        
        loc[2].id = "P11";
        loc[2].kind = "processname";


        // Add new member on the group
        TEST_INFO("add the new member "<<m_host1<<"/APPL1/P11 in group 2");
        try
        {
            obj2 = rm->add_member(obj2.in(),
                                  loc,
                                  hello2P11.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello2P31->get_info_from_datastore(3, 1)<<" -> expected \"three1-2\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 1), "three1-2"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello2P31->get_info_from_datastore(3, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore1):"<<hello2P31->get_info_from_datastore(3, 4)<<" -> expected \"three4-1\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 4), "three4-1"));

            TEST_INFO("data for oid 1 (datastore2):"<<hello2P31->get_info_from_datastore(4, 1)<<" -> expected \"four1-2\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 1), "four1-2"));
            TEST_INFO("data for oid 1 (datastore2):"<<hello2P31->get_info_from_datastore(4, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore2):"<<hello2P31->get_info_from_datastore(4, 4)<<" -> expected \"four4-1\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 4), "four4-1"));




            TEST_INFO("data for oid 1 (datastore1):"<<hello2P11->get_info_from_datastore(3, 1)<<" -> expected \"three1-2\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(3, 1), "three1-2"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello2P11->get_info_from_datastore(3, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(3, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore1):"<<hello2P11->get_info_from_datastore(3, 4)<<" -> expected \"three4-1\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(3, 4), "three4-1"));


            TEST_INFO("data for oid 1 (datastore2):"<<hello2P11->get_info_from_datastore(4, 1)<<" -> expected \"four1-2\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(4, 1), "four1-2"));
            TEST_INFO("data for oid 1 (datastore2):"<<hello2P11->get_info_from_datastore(4, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(4, 2), "NotFoundException"));
            TEST_INFO("data for oid 4 (datastore2):"<<hello2P11->get_info_from_datastore(4, 4)<<" -> expected \"four4-1\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(4, 4), "four4-1"));
        }
        catch(...)
        {
            TEST_FAILED();
        }

        
        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Set info into data store");
        try
        {
            hello2P21->trigger_on_update(3, 1, "three1-3");
            hello2P21->trigger_on_remove(3, 4);
            hello2P21->trigger_on_insert(3, 5, "three5-1");

            hello2P21->trigger_on_update(4, 1, "four1-3");
            hello2P21->trigger_on_remove(4, 4);
            hello2P21->trigger_on_insert(4, 5, "four5-1");
            TEST_SUCCEED();

            TEST_CHECK(!strcmp(hello2P21->trigger_on_insert(3, 5, "three5-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_remove(3, 4), "NotFoundException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_update(3, 10, "ten"), "NotFoundException"));

            TEST_CHECK(!strcmp(hello2P21->trigger_on_insert(4, 5, "four5-1"), "AlreadyExistException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_remove(4, 4), "NotFoundException"));
            TEST_CHECK(!strcmp(hello2P21->trigger_on_update(4, 10, "ten"), "NotFoundException"));
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_CHECK(hello2P21->callback_called(3, "insert") == 1);
        TEST_CHECK(hello2P21->callback_called(3, "update") == 1);
        TEST_CHECK(hello2P21->callback_called(3, "remove") == 1);
        TEST_CHECK(hello2P21->callback_called(4, "insert") == 1);
        TEST_CHECK(hello2P21->callback_called(4, "update") == 1);
        TEST_CHECK(hello2P21->callback_called(4, "remove") == 1);

        TEST_CHECK(hello2P31->callback_called(3, "insert") == 1);
        TEST_CHECK(hello2P31->callback_called(3, "update") == 1);
        TEST_CHECK(hello2P31->callback_called(3, "remove") == 1);
        TEST_CHECK(hello2P31->callback_called(4, "insert") == 1);
        TEST_CHECK(hello2P31->callback_called(4, "update") == 1);
        TEST_CHECK(hello2P31->callback_called(4, "remove") == 1);

        TEST_CHECK(hello2P11->callback_called(3, "insert") == 3);
        TEST_CHECK(hello2P11->callback_called(3, "update") == 1);
        TEST_CHECK(hello2P11->callback_called(3, "remove") == 1);
        TEST_CHECK(hello2P11->callback_called(4, "insert") == 3);
        TEST_CHECK(hello2P11->callback_called(4, "update") == 1);
        TEST_CHECK(hello2P11->callback_called(4, "remove") == 1);

        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello2P31->get_info_from_datastore(3, 1)<<" -> expected \"three1-3\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 1), "three1-3"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello2P31->get_info_from_datastore(3, 4)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 4), "NotFoundException"));
            TEST_INFO("data for oid 5 (datastore1):"<<hello2P31->get_info_from_datastore(3, 5)<<" -> expected \"three5-1\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(3, 5), "three5-1"));


            TEST_INFO("data for oid 1 (datastore2):"<<hello2P31->get_info_from_datastore(4, 1)<<" -> expected \"four1-3\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 1), "four1-3"));
            TEST_INFO("data for oid 1 (datastore2):"<<hello2P31->get_info_from_datastore(4, 4)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 4), "NotFoundException"));
            TEST_INFO("data for oid 5 (datastore2):"<<hello2P31->get_info_from_datastore(4, 5)<<" -> expected \"four5-1\"");
            TEST_CHECK(!strcmp(hello2P31->get_info_from_datastore(4, 5), "four5-1"));




            TEST_INFO("data for oid 1 (datastore1):"<<hello2P11->get_info_from_datastore(3, 1)<<" -> expected \"three1-3\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(3, 1), "three1-3"));
            TEST_INFO("data for oid 1 (datastore1):"<<hello2P11->get_info_from_datastore(3, 4)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(3, 4), "NotFoundException"));
            TEST_INFO("data for oid 5 (datastore1):"<<hello2P11->get_info_from_datastore(3, 5)<<" -> expected \"three5-1\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(3, 5), "three5-1"));


            TEST_INFO("data for oid 1 (datastore2):"<<hello2P11->get_info_from_datastore(4, 1)<<" -> expected \"four1-3\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(4, 1), "four1-3"));
            TEST_INFO("data for oid 1 (datastore2):"<<hello2P11->get_info_from_datastore(4, 4)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(4, 4), "NotFoundException"));
            TEST_INFO("data for oid 5 (datastore2):"<<hello2P11->get_info_from_datastore(4, 5)<<" -> expected \"four5-1\"");
            TEST_CHECK(!strcmp(hello2P11->get_info_from_datastore(4, 5), "four5-1"));

            
        }
        catch(...)
        {
            TEST_FAILED();
        }

        CdmwReplicationManager::HelloInterface1_var temp1;
        CdmwReplicationManager::HelloInterface2_var temp2;

        loc[0].id = m_host2.c_str();
        loc[0].kind = "hostname";
        loc[1].id = "APPL2";
        loc[1].kind = "applicationname";        
        loc[2].id = "P21";
        loc[2].kind = "processname";

        std::string location_string = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);

        TEST_INFO("Who is the primary");
        try
        {

            ::FT::ObjectGroup_var object_group = rm->get_object_group_ref(hello1ObjectGroup.in());
            temp1 = CdmwReplicationManager::HelloInterface1::_narrow(object_group.in());
            std::cout<<temp1->who_is_called()<<std::endl;
            TEST_CHECK(temp1->who_is_called() == location_string);

            object_group = rm->get_object_group_ref(hello2ObjectGroup.in());
            temp2 = CdmwReplicationManager::HelloInterface2::_narrow(object_group.in());
            std::cout<<temp2->who_is_called()<<std::endl;
            TEST_CHECK(temp2->who_is_called() == location_string);
 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;;
            TEST_FAILED();
        }

        CORBA::Object_var fm_obj
        = m_orb->string_to_object("corbaloc::localhost:4555/fault_manager");
        
        if ( CORBA::is_nil( fm_obj.in() ) )
            CDMW_THROW2(Exception,
                        Exception::FATAL,
                        "Invalid corbaloc" );
        
        ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );
        
//         fault_manager->dump_datastore(::CdmwFT::REPLICATION_MANAGER);
        

//         TEST_INFO("Check on the replication manager if the location "<<m_host2<<"/APPL2/P21");
//         TEST_INFO("is primary for all the object group");
//         std::cout<<"Press any key and Enter"<<std::endl;
//         std::string dummy;
//         std::cin >> dummy;


        // Add new member on the group
        TEST_INFO("remove the member "<<m_host2<<"/APPL2/P21 in group 1");
        try
        {
            obj1 = rm->remove_member(obj1.in(),
                                    loc);
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

//         fault_manager->dump_datastore(::CdmwFT::REPLICATION_MANAGER);

//         TEST_INFO("Check on the replication manager if the location "<<m_host1<<"/APPL1/P11");
//         TEST_INFO("is primary for all the object group");
//         std::cout<<"Press any key and Enter"<<std::endl;
//         std::cin >> dummy;

        loc[0].id = m_host1.c_str();
        loc[0].kind = "hostname";
        loc[1].id = "APPL1";
        loc[1].kind = "applicationname";        
        loc[2].id = "P11";
        loc[2].kind = "processname";

        location_string = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);

        TEST_INFO("Who is the primary");
        try
        {

            ::FT::ObjectGroup_var object_group = rm->get_object_group_ref(hello1ObjectGroup.in());
            temp1 = CdmwReplicationManager::HelloInterface1::_narrow(object_group.in());
            std::cout<<temp1->who_is_called()<<std::endl;
            TEST_CHECK(temp1->who_is_called() == location_string);

            object_group = rm->get_object_group_ref(hello2ObjectGroup.in());
            temp2 = CdmwReplicationManager::HelloInterface2::_narrow(object_group.in());
             std::cout<<temp2->who_is_called()<<std::endl;
           TEST_CHECK(temp2->who_is_called() == location_string);
            
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;;
            TEST_FAILED();
        }

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

