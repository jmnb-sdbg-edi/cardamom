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

#include <memory> // for std::auto_ptr
#include <string>
#include <vector>


#include "platformsystem/CdmwInterface.hpp"
#include "platformsystemlibrary/InitUtils.hpp"
#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"
#include "SystemMngt/platformlibrary/DataStoreBaseDefinition.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"

#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/osthreads/Condition.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <Foundation/commonsvcs/datastore/StdMapDataStoreFactory.hpp>


#include "SystemMngt/idllib/CdmwPlatformMngtDataStore.stub.hpp"

   



namespace {
   
    const int MAX_RETRY_ON_FTMGR = 4;
    const int SLEEP_RETRY_ON_FTMGR_MS = 1000;

    const CORBA::ULong MANAGED_ELEMENT_DATA_STORE_ID = 1;
    const CORBA::ULong ENTITY_DATA_STORE_ID = 2;
    const CORBA::ULong HOST_DATA_STORE_ID = 3;
    const CORBA::ULong SYSTEM_DATA_STORE_ID = 4;
    const CORBA::ULong APPLICATION_DATA_STORE_ID = 5;
    const CORBA::ULong PROCESS_DATA_STORE_ID = 6;
    const CORBA::ULong SYSTEM_OBSERVER_REGISTRATION_DATA_STORE_ID = 7;
    const CORBA::ULong EVENT_REPORT_DATA_STORE_ID = 8;
    const CORBA::ULong EVENT_NOTIFIER_DATA_STORE_ID = 9;
    const CORBA::ULong SERVICE_DEFS_DATA_STORE_ID = 10;
    const CORBA::ULong RUNSET_DATA_STORE_ID = 11;

} // end of anonymous namespace



namespace Cdmw {

namespace PlatformMngt {


    void 
    CDMW_init(CORBA::ORB_ptr orb, 
              int & argc, 
              char** argv )
    throw (CORBA::SystemException)
    {

         CDMW_ASSERT(!CORBA::is_nil(orb));

         try {

         // *****************************************************************
         // *****************  CDMW root POA and servant manager ***********
         // *****************************************************************
         //
         // Create the CDMW root POA. It has its own POA Manager
         // May raise a CORBA::SystemException.
         //
         PortableServer::POA_var CDMW_rootPOA = InitUtils::Create_cdmw_root_POA(orb);
        
         PortableServer::POAManager_var poaManager = CDMW_rootPOA -> the_POAManager();
         //
         // Initialize the server so that incoming requests are
         // handled from this point on
         //
         // May raise PortableServer::AdapterInactive or CORBA::SystemException
         try {
            poaManager -> activate(); // TODO: maybe move it?
         } catch (const PortableServer::POAManager::AdapterInactive & ) {
            CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALORBAdapterInactive,
                                          CORBA::COMPLETED_NO) );
         } catch (const CORBA::SystemException & ex ) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                          ex, "Could not activate the CDMW POA Manager!" );
         }

         // *****************************************************************
         // *****************  create datastore of SystemMngt  **************
         // *****************************************************************
         //


         // define the datastore responsible to store ManagedElement
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngt::ManagedElementRecord> ManagedElementDataStoreFactory;

         ManagedElementDataStoreFactory::Create_data_store(MANAGED_ELEMENT_DATA_STORE_ID);
          
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_managed_element_datastore_id(MANAGED_ELEMENT_DATA_STORE_ID);
    

         // define the datastore responsible to store Entity
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngt::EntityRecord> EntityDataStoreFactory;

         EntityDataStoreFactory::Create_data_store(ENTITY_DATA_STORE_ID);

         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_entity_datastore_id(ENTITY_DATA_STORE_ID);
    

         // define the datastore responsible to store Host
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngt::HostRecord> HostDataStoreFactory;

         HostDataStoreFactory::Create_data_store(HOST_DATA_STORE_ID);

         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_host_datastore_id(HOST_DATA_STORE_ID);
    

         // define the datastore responsible to store System
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngt::SystemRecord> SystemDataStoreFactory;

         SystemDataStoreFactory::Create_data_store(SYSTEM_DATA_STORE_ID);

         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_system_datastore_id(SYSTEM_DATA_STORE_ID);
    

         // define the datastore responsible to store Application
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngt::ApplicationRecord> ApplicationDataStoreFactory;

         ApplicationDataStoreFactory::Create_data_store(APPLICATION_DATA_STORE_ID);
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_application_datastore_id(APPLICATION_DATA_STORE_ID);

         // define the datastore responsible to store Process
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngt::ProcessRecord> ProcessDataStoreFactory;

         ProcessDataStoreFactory::Create_data_store(PROCESS_DATA_STORE_ID);

         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_process_datastore_id(PROCESS_DATA_STORE_ID);
    

         // define the datastore responsible to store SystemObserverRegistration
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngt::SystemObserverRegistrationRecord> 
                    SystemObserverRegistrationDataStoreFactory;

         SystemObserverRegistrationDataStoreFactory::Create_data_store(
               SYSTEM_OBSERVER_REGISTRATION_DATA_STORE_ID);
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_system_observer_registration_datastore_id(
               SYSTEM_OBSERVER_REGISTRATION_DATA_STORE_ID);
    

         // define the datastore responsible to store EventHandler
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory <
                 CdmwPlatformMngtBase::EventKey, 
                 CdmwPlatformMngt::EventReportRecord> EventReportDataStoreFactory;

         EventReportDataStoreFactory::Create_data_store(EVENT_REPORT_DATA_STORE_ID);

         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_event_report_datastore_id(EVENT_REPORT_DATA_STORE_ID);
    

         // define the datastore responsible to store EventNotifier
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory <
                 CdmwPlatformMngtBase::EventKey, 
                 CdmwPlatformMngt::Event> EventNotifierDataStoreFactory;

         EventNotifierDataStoreFactory::Create_data_store(EVENT_NOTIFIER_DATA_STORE_ID);
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_event_notifier_datastore_id(EVENT_NOTIFIER_DATA_STORE_ID);
    
      
         // define the datastore responsible to store ServiceDefs
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngtService::ServiceDefs> ServiceDefsDataStoreFactory;

         ServiceDefsDataStoreFactory::Create_data_store(SERVICE_DEFS_DATA_STORE_ID);

         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_service_defs_datastore_id(SERVICE_DEFS_DATA_STORE_ID);
    

         // define the datastore responsible to store RunSet
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngt::RunSet> RunSetDataStoreFactory;

         RunSetDataStoreFactory::Create_data_store(RUNSET_DATA_STORE_ID);

         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_runset_datastore_id(RUNSET_DATA_STORE_ID);
    
   
         } catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex) {
            std::cerr << "CDMW_init: Fatal Error: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;
            ex.raise_SystemException();
         } catch (const CORBA::SystemException & ex) {
            std::cerr << "CDMW_init: Fatal Error: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;           
            throw;
         }
    }

    // CDMW Factory objects will be deactivated at ORB shutdown.
    void
    CDMW_cleanup(CORBA::ORB_ptr orb)
        throw ()
    {
        CDMW_ASSERT(!CORBA::is_nil(orb));
    }

   
} // end of namespace PlatformMngt

} // end of namespace Cdmw


