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

#include <testftlocationmanager2/TestStatefullPrimaryBackupAdmin.hpp>
#include <testftlocationmanager2/SIM_PrimaryBackupGroupRepository.hpp>
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreGroup_impl.hpp>
#include "testftlocationmanager2/TestHello_impl.hpp"

namespace Cdmw {
namespace FT {


    // constructor
    TestStatefullPrimaryBackupAdmin::TestStatefullPrimaryBackupAdmin(
        CORBA::ORB_ptr orb,
        PortableServer::POA_ptr rootPOA,
        const std::string& name,
        const std::string& process_name)
        : Testable( name ),
          m_process_name(process_name)
    {
        m_orb = CORBA::ORB::_duplicate( orb );
        m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    }
    

    // destructor
    TestStatefullPrimaryBackupAdmin::~TestStatefullPrimaryBackupAdmin()
    {


    }
    


    void TestStatefullPrimaryBackupAdmin::do_tests()
    {
        set_nbOfRequestedTestOK (19);
        
        try
        {
            TEST_INFO("Create the sim group repository");        
            Location::SIM_PrimaryBackupGroupRepository_impl* group_repository = new Location::SIM_PrimaryBackupGroupRepository_impl();
            group_repository->the_location();
            
   
            ::FT::Location loc;
            loc.length(3);
            loc[0].kind = "hostname";
            loc[1].kind = "applicationname";
            loc[2].kind = "processname";
 
            loc[0].id = "HOST0";
            loc[1].id = "APPL0";
            loc[2].id = "PROC0";
            
            TEST_INFO("Create the statefullPrimaryBackupAdmin");
            Location::StatefullPrimaryBackupAdmin_impl* sfpba_obj = new Location::StatefullPrimaryBackupAdmin_impl(m_orb.in(),
                                                                                                               m_rootPOA.in(), 
                                                                                                               loc,
                                                                                                               group_repository,
                                                                                                               true,    
                                                                                                               true);
            
            CdmwFT::Location::StatefullPrimaryBackupAdmin_var sfpba = sfpba_obj->_this();

            TEST_INFO("Create a DataStore");
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

            TEST_SUCCEED();
            TestDataStore* dataStoreObj1 = new TestDataStore(11,
                                                             m_orb.in(),
                                                             m_rootPOA.in(),
                                                             context);

            dataStoreObj1->is_activated();
            TEST_SUCCEED();

            TestDataStore* dataStoreObj2 = new TestDataStore(12,
                                                             m_orb.in(),
                                                             m_rootPOA.in(),
                                                             context);
            
            dataStoreObj2->is_activated();
            TEST_SUCCEED();

            TestDataStore* dataStoreObj3 = new TestDataStore(13,
                                                             m_orb.in(),
                                                             m_rootPOA.in(),
                                                             context);
            
            dataStoreObj3->is_activated();
            TEST_SUCCEED();

            CdmwFT::StateTransfer::DataStoreGroup_var datastore_group
                = (new Cdmw::FT::DataStoreGroup_impl(m_rootPOA.in(),context))->_this();




            
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


            TEST_INFO("Create LocalDataStoreInfos for init_backup"); 

            CdmwFT::StateTransfer::LocalDataStoreInfos infos;
            infos.length(1);
            infos[0].dsids.length(2);
            infos[0].dsids[0] = 11;
            infos[0].dsids[1] = 12;
            infos[0].coordinator = info.coordinator;
            infos[0].cohort = info.cohort;
            infos[0].local_data_store = info.local_data_store;

            CdmwFT::StateTransfer::DataStoreInfo primarydatastoreinfos;
            primarydatastoreinfos.the_location = loc;
            primarydatastoreinfos.local_data_store_infos = infos;

            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                sfpba->init_backup_data_stores(primarydatastoreinfos);
                TEST_SUCCEED();
            }
            catch (const CdmwFT::Location::InitFailed& ex)
            {
                std::cout<<ex<<std::endl;
                std::cout<<ex.reason<<std::endl;
                TEST_FAILED();
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            
            // call init with the same infos => thorw alreadyDone exception
            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                sfpba->init_backup_data_stores(primarydatastoreinfos);
                TEST_FAILED();
            }
            catch (const CdmwFT::Location::InitFailed& ex)
            {
                std::cout<<ex<<std::endl;
                std::cout<<ex.reason<<std::endl;
                TEST_FAILED();
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_SUCCEED();
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            
            
            infos.length(1);
            infos[0].dsids.length(2);
            infos[0].dsids[0] = 11;
            infos[0].dsids[1] = 13;
            infos[0].coordinator = info.coordinator;
            infos[0].cohort = info.cohort;
            infos[0].local_data_store = info.local_data_store;
            primarydatastoreinfos.the_location = loc;
            primarydatastoreinfos.local_data_store_infos = infos;

            // call init with one datastore already init and one not initialised => unstable state => InitFailed exception
            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                sfpba->init_backup_data_stores(primarydatastoreinfos);
                TEST_FAILED();
            }
            catch (const CdmwFT::Location::InitFailed& ex)
            {
                std::cout<<ex<<std::endl;
                std::cout<<ex.reason<<std::endl;
                TEST_SUCCEED();
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }

            infos.length(1);

            infos[0].dsids.length(1);
            infos[0].dsids[0] = 13;
            infos[0].coordinator = info.coordinator;
            infos[0].cohort = info.cohort;
            infos[0].local_data_store = info.local_data_store;
            primarydatastoreinfos.the_location = loc;
            primarydatastoreinfos.local_data_store_infos = infos;

            // call init_backup
            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                sfpba->init_backup_data_stores(primarydatastoreinfos);
                TEST_SUCCEED();
            }
            catch (const CdmwFT::Location::InitFailed& ex)
            {
                std::cout<<ex<<std::endl;
                std::cout<<ex.reason<<std::endl;
                TEST_FAILED();
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }

            infos.length(1);

            infos[0].dsids.length(1);
            infos[0].dsids[0] = 14;
            infos[0].coordinator = NULL;
            infos[0].cohort =NULL;
            infos[0].local_data_store = 
               CdmwFT::StateTransfer::DataStoreGroup::_nil();
            primarydatastoreinfos.the_location = loc;
            primarydatastoreinfos.local_data_store_infos = infos;

            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                sfpba->init_backup_data_stores(primarydatastoreinfos);
                TEST_FAILED();
            }
            catch (const CdmwFT::Location::InitFailed& ex)
            {
                std::cout<<ex<<std::endl;
                std::cout<<ex.reason<<std::endl;
                TEST_SUCCEED();
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }



            TEST_INFO("Create a DataStore");
            TestDataStore* dataStoreObj4 = new TestDataStore(14,
                                                             m_orb.in(),
                                                             m_rootPOA.in(),
                                                             context);
            
            TestDataStore* dataStoreObj5 = new TestDataStore(15,
                                                             m_orb.in(),
                                                             m_rootPOA.in(),
                                                             context);
            
            TestDataStore* dataStoreObj6 = new TestDataStore(16,
                                                             m_orb.in(),
                                                             m_rootPOA.in(),
                                                             context);
            

            TEST_INFO("Create LocalDataStoreInfos for init_backup"); 

            infos.length(1);
            infos[0].dsids.length(2);
            infos[0].dsids[0] = 14;
            infos[0].dsids[1] = 15;
            infos[0].coordinator = info.coordinator;
            infos[0].cohort = info.cohort;
            infos[0].local_data_store = info.local_data_store;

            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                sfpba->init_primary_data_stores(infos);
                TEST_SUCCEED();
            }
            catch (const CdmwFT::Location::InitFailed& ex)
            {
                std::cout<<ex<<std::endl;
                std::cout<<ex.reason<<std::endl;
                TEST_FAILED();
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }

            TEST_INFO("check if the datastore is activated");
            TEST_CHECK(dataStoreObj4->is_activated());
            TEST_CHECK(dataStoreObj5->is_activated());

            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                sfpba->init_primary_data_stores(infos);
                TEST_FAILED();
            }
            catch (const CdmwFT::Location::InitFailed& ex)
            {
                std::cout<<ex<<std::endl;
                std::cout<<ex.reason<<std::endl;
                TEST_FAILED();
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_SUCCEED();
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            
            
            infos.length(1);
            infos[0].dsids.length(2);
            infos[0].dsids[0] = 14;
            infos[0].dsids[1] = 16;
            infos[0].coordinator = info.coordinator;
            infos[0].cohort = info.cohort;
            infos[0].local_data_store = info.local_data_store;

            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                sfpba->init_primary_data_stores(infos);
                TEST_FAILED();
            }
            catch (const CdmwFT::Location::InitFailed& ex)
            {
                std::cout<<ex<<std::endl;
                std::cout<<ex.reason<<std::endl;
                TEST_SUCCEED();
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }

            TEST_INFO("check if the datastore is activated");
            TEST_CHECK(dataStoreObj4->is_activated());
            TEST_CHECK(!dataStoreObj6->is_activated());

            infos.length(1);

            infos[0].dsids.length(1);
            infos[0].dsids[0] = 16;
            infos[0].coordinator = info.coordinator;
            infos[0].cohort = info.cohort;
            infos[0].local_data_store = info.local_data_store;

            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                sfpba->init_primary_data_stores(infos);
                TEST_SUCCEED();
            }
            catch (const CdmwFT::Location::InitFailed& ex)
            {
                std::cout<<ex<<std::endl;
                std::cout<<ex.reason<<std::endl;
                TEST_FAILED();
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }

            TEST_INFO("check if the datastore is activated");
            TEST_CHECK(dataStoreObj6->is_activated());

            infos.length(1);

            infos[0].dsids.length(1);
            infos[0].dsids[0] = 17;
            infos[0].coordinator = NULL;
            infos[0].cohort =NULL;
            infos[0].local_data_store = 
               CdmwFT::StateTransfer::DataStoreGroup::_nil();

            TEST_INFO("Call init_backup_data_stores"); 
            try
            {
                sfpba->init_primary_data_stores(infos);
                TEST_FAILED();
            }
            catch (const CdmwFT::Location::InitFailed& ex)
            {
                std::cout<<ex<<std::endl;
                std::cout<<ex.reason<<std::endl;
                TEST_SUCCEED();
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }
            catch(const CORBA::SystemException& ex)
            {
                std::cout<<ex<<std::endl;
                TEST_FAILED();
            }

            OsSupport::OS::sleep(timescale*1500);

            OsSupport::OS::kill_process(pid1);
        }
        catch(...)
        {
            TEST_FAILED();
        }    
    }
    

}; // namespace FT
}; // namespace Cdmw

