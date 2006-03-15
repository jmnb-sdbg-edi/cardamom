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


#include "platformftsystem/CdmwInterface.hpp"
#include "platformsystemlibrary/InitUtils.hpp"
#include "platformsystemlibrary/CdmwInitConfiguration.hpp"
#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"
#include "SystemMngt/platformlibrary/DataStoreBaseDefinition.hpp"
#include "SystemMngt/platformlibrary/ClientIDGenerator.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"

#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/osthreads/Condition.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>



#include <FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp>
#include <FaultTolerance/ftlocationmanager/PrimaryBackupAdmin_impl.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>
#include <FaultTolerance/ftcommon/FTConfiguration.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreGroup_impl.hpp>
      
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>
#include <FaultTolerance/ftlocationmanager/LocationDataStoreTimeoutHandler_impl.hpp>

#include "SystemMngt/idllib/CdmwPlatformMngtDataStore.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"

namespace {
   
    const int MAX_RETRY_ON_FTMGR = 4;
    const int SLEEP_RETRY_ON_FTMGR_MS = 1000;

    const int NB_SYSMNGT_DATASTORE = 11;

} // end of anonymous namespace



namespace Cdmw {

namespace PlatformMngt {


void CDMW_init(CORBA::ORB_ptr orb, 
               int & argc, 
               char** argv, 
               const char* supervisionProcessName,
               const char* fault_manager_corbaloc)
throw (CORBA::SystemException)
{

     CDMW_ASSERT(!CORBA::is_nil(orb));

     try {
        
         // Get the XML init filename. This may raise CORBA::BAD_PARAM, CORBA::NO_MEMORY,
         // or CORBA::INTERNAL    
         std::string xml_file = InitUtils::Get_xml_initialisation_file(argc, argv);
        
         // Get the XML documents validation option. This may raise CORBA::NO_MEMORY,
         // or CORBA::INTERNAL
         bool validate_xml = InitUtils::Get_xml_validation_option(argc, argv);
        
         // Parsing xml
         std::auto_ptr<CdmwInitConfiguration> xml_data(0);
         try {
            std::auto_ptr<CdmwInitConfiguration> init
               (CdmwInitConfiguration::ReadConfiguration(xml_file,
                                                         validate_xml));
            
            xml_data = init;
         } catch (const Cdmw::OutOfMemoryException &) {
            CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                          CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY, 
                                           CORBA::COMPLETED_NO) );
         } catch (const ConfigurationException & ex) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                           CORBA::COMPLETED_NO),
                          ex.what());
         } catch(const Cdmw::InternalErrorException & ex){
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALXMLLibraryError,
                                          CORBA::COMPLETED_NO),
                          ex.what() );
         }

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
         // *************  setup FaultTolerance Policies   ******************
         // *****************************************************************

         Cdmw::OrbSupport::OrbSupport::set_endpoint_selector_factory(::Cdmw::FT::FTConfiguration::Get_FT_endpoint_selector_factory());

         size_t duration_time;
         std::string client_id_str;
         std::string hostname = Cdmw::OsSupport::OS::get_hostname();
            
         try {
             std::string res;
             try {
                 res = xml_data->getServiceAttributeValue("fault-tolerance", "request-duration-policy"); 
                 duration_time = atoi(res.c_str());
             } catch (const Cdmw::BadParameterException& ex) {
                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                CORBA::COMPLETED_NO),
                               ex.what() );
             }

             try  {
                 //client_id_str = xml_data->getServiceAttributeValue("fault-tolerance", "client-id");
                 client_id_str = 
                    ClientIDGenerator::generate_supervision_client_id(hostname.c_str());
                     Cdmw::FT::FTConfiguration::Set_client_id(orb, 
                                                          client_id_str.c_str());
             } catch (const Cdmw::BadParameterException& ) {
                 // do nothing : parameter is optional
             }
         } catch (const XMLErrorException & ex) {
             CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                           CORBA::COMPLETED_NO),
                          ex.what() );
         } catch (const OutOfMemoryException & ex) {
             CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                          CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                           CORBA::COMPLETED_NO),
                          ex.what() );
         } 

         CORBA::Object_var obj;
         obj = orb->resolve_initial_references("ORBPolicyManager");
        
         CORBA::PolicyManager_var policy_manager 
            = CORBA::PolicyManager::_narrow(obj.in());
        

      // Create and apply an ORB-wide Routed policy
         CORBA::Any any;
         TimeBase::TimeT duration = duration_time;
         any <<= duration;
         CORBA::PolicyList policies(2);
         policies.length(2);
         policies[0] =
            orb->create_policy(::FT::REQUEST_DURATION_POLICY, any);

      // connection timeout policy
         CORBA::Any timeout_as_any;
         timeout_as_any <<= static_cast<TimeBase::TimeT>(250000);
         policies[1] =
            orb->create_policy(TAO::CONNECTION_TIMEOUT_POLICY_TYPE, timeout_as_any);
      
         policy_manager->set_policy_overrides(policies, CORBA::SET_OVERRIDE);
      

         // *****************************************************************
         // ***********  setup FaultTolerance ReplicationManager  ***********
         // *****************************************************************

         // Get the reference of the Replication Manager and register it in the ORB
         
         CdmwFT::ReplicationManager_var replication_manager = CdmwFT::ReplicationManager::_nil();
         
         try
         {
             // if corbaloc has been defined
             if (strcmp(fault_manager_corbaloc, "") != 0)
             {    
                 // get the FT manager
                 CORBA::Object_var fault_manager_obj = 
                     orb->string_to_object(fault_manager_corbaloc);
                 if (CORBA::is_nil (fault_manager_obj.in()))
                     CDMW_THROW2(Cdmw::Exception,
                                 Cdmw::Exception::FATAL,
                                 "Invalid fault manager corbaloc");    
         
                 CdmwFT::FTManager_var fault_manager;
             
                 try 
                 {
                     fault_manager = ::CdmwFT::FTManager::_narrow(fault_manager_obj.in());
                 } catch (const CORBA::SystemException & ex) {
                     std::cerr << "Exception while contacting FTManager: "
                               << ex << "\n"
                               << fault_manager_corbaloc << " is probably invalid"
                               << std::endl;
                     CDMW_THROW2(Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                                 ex,
                                 "Invalid fault manager corbaloc" );
                 }
     
                 if (CORBA::is_nil (fault_manager.in()))
                     CDMW_THROW2(Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                                 CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO),
                                 "Invalid fault manager reference");


                 bool ftmgr_not_active = true;
                 int nbRetries = 0;
                 
                 do 
                 {
                     try 
                     {
                         // get the replication manager from the FTManager
                         CORBA::Object_var rm_obj = 
                         fault_manager->get_replication_manager();
                         replication_manager = 
                         CdmwFT::ReplicationManager::_narrow(rm_obj.in());
                         ftmgr_not_active = false;
                     } catch (const CdmwFT::FTManager::NoAlreadyActivated&) {
                         if (++nbRetries > MAX_RETRY_ON_FTMGR) 
                         {
                             std::cerr << "ERROR: FTManager not yet activated." 
                                       << std::endl;
                             
                             CDMW_THROW2(Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                                         CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO),
                                         "FTManager not yet activated" );
                         }
                         
                         std::cerr << "Retry to contact FTManager in " 
                                   << SLEEP_RETRY_ON_FTMGR_MS 
                                   << " ms ..." 
                                   << std::endl;
                         Cdmw::OsSupport::OS::sleep(SLEEP_RETRY_ON_FTMGR_MS);
                     }
                 } while (ftmgr_not_active);
             }
             else
             {
                 CdmwPlatformMngt::Daemon_var daemon = CdmwPlatformMngt::Daemon::_nil();
                 try
                 {
                     // get the daemon reference
                     daemon = 
                     Configuration::Get_daemon ("localhost", 
                                                Cdmw::PlatformMngt::Configuration::Get_daemon_port());
                 }
                 catch (...)
                 {
                     std::cerr << "ERROR: Local Daemon cannot be reachable on port " 
                               << Cdmw::PlatformMngt::Configuration::Get_daemon_port()
                               << std::endl;
                     
                     CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                                  CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO),
                                  "Local Daemon cannot be reachable.");
                 }
                 
                 try
                 {  
                     CORBA::Object_var rm_obj = daemon->discover_global_service ("ReplicationManager");
                     replication_manager = 
                     CdmwFT::ReplicationManager::_narrow(rm_obj.in());
                 }
                 catch (const CORBA::SystemException& ex)
                 {
                     std::cerr << "ERROR: ReplicationManager not discovered by daemon." 
                               << std::endl;
                     
                     CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                                  ex,
                                  "Cannot found ReplicationManager reference.");
                 }
                 
             }
             
             
             orb->register_initial_reference("ReplicationManager", 
                                             replication_manager.in());      

         }
         catch (const CORBA::SystemException& ex)
         {
             CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                          ex,
                          "Cannot register ReplicationManager.");
         }
         catch (const CORBA::ORB::InvalidName& )
         {
         CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
                                      CORBA::COMPLETED_NO),
                      "InvalidName => Cannot register ReplicationManager.");
         }
         
         
         // *****************************************************************
         // *************  setup FaultTolerance LocationManager  ************
         // *****************************************************************
         
         // Location information shall be obtain from supervision
         ::FT::Location loc;
         loc.length(2);

         loc[0].id 
            = CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
         loc[0].kind = "hostname";
         loc[1].id = supervisionProcessName;
         loc[1].kind = "processname";
         
         std::string location_str 
             = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
         std::cout << "setup FaultTolerance LocationManager for location: " 
                    << location_str << std::endl;
            
         // create a reference for the PrimaryBackupGroupRepository
         ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * 
            primary_backup_repository =
                 ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();

         // get the fault notifier reference
         ::FT::FaultNotifier_var fault_notifier = 
            replication_manager->get_fault_notifier();

         // create the DataStoreTimeoutHandler
         Cdmw::FT::LocationDataStoreTimeoutHandler* 
            locationDataStoreTimeoutHandler = 
               new Cdmw::FT::LocationDataStoreTimeoutHandler(
                     fault_notifier.in());

         // Register the timeoutHandler into the DataStoreBase
         Cdmw::FT::DataStoreBase::Register_Timeout_Handler(
               locationDataStoreTimeoutHandler);


         // *****************************************************************
         // *************  setup DataStore                       ************
         // *****************************************************************
         
         // FaultTolerance use datastore
         CORBA::ULong datastore_id;
         std::string multicast_port;
         std::string multicast_domain;
         std::string multicast_IP_address;
         int max_transaction_in_progress;
         int max_transaction_done;
         OsSupport::OS::Timeval cohort_timeout;
         OsSupport::OS::Timeval coordinator_timeout;
         OsSupport::OS::Timeval freeze_timeout;
         long number_of_scheduler_thread;
         long chunk_size;
         long freeze_size;

      
         
         // Get dataStore parameters from xml file
         try
         {
             InitUtils::Get_parameters(*(xml_data.get()),
                                       multicast_port,
                                       multicast_domain,
                                       multicast_IP_address,
                                       max_transaction_in_progress,
                                       max_transaction_done,
                                       cohort_timeout,
                                       coordinator_timeout,
                                       freeze_timeout,
                                       number_of_scheduler_thread,
                                       chunk_size,
                                       freeze_size);
         } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                           CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                           CORBA::COMPLETED_NO),ex.what() );
         }
          
         // create the dataStore context
         Cdmw::FT::DataStoreContext* context =
             new Cdmw::FT::DataStoreContext
             (orb,
              CDMW_rootPOA.in(),
              max_transaction_in_progress,
              max_transaction_done,
              cohort_timeout,
              coordinator_timeout,
              number_of_scheduler_thread,
              chunk_size,
              freeze_size);
         
         // create the local datastore infos     
         CORBA::ULong ds_inc = 0;
         CdmwFT::StateTransfer::LocalDataStoreInfos_var localDataStoreInfos = 
            new CdmwFT::StateTransfer::LocalDataStoreInfos();
         localDataStoreInfos->length(1);
      
      
         // create local datastore info with datastore group
         CdmwFT::StateTransfer::LocalDataStoreInfo localDataStoreInfo;
         
         CdmwFT::StateTransfer::DataStoreGroup_var
              managedelementdatastore_group_ref = 
                (new Cdmw::FT::DataStoreGroup_impl(
                        CDMW_rootPOA.in(),
                        *context))->_this();
    
         localDataStoreInfo.dsids.length(NB_SYSMNGT_DATASTORE);
         localDataStoreInfo.local_data_store = 
                CdmwFT::StateTransfer::DataStoreGroup::_duplicate(
                                      managedelementdatastore_group_ref.in());
          
         localDataStoreInfo.coordinator = context->get_tpc_coordinator();
         localDataStoreInfo.cohort = context->get_tpc_cohort();
    
         (*localDataStoreInfos)[0] = localDataStoreInfo;
          
          
          
          
         // define the DataStores used by System Management
         // The order of creation is significant for backup starting and must not be changed
         // (due to record dependancy)
         // ManagedElement, System, Host, Application, Process, Entity,
         // SystemObserverRegistration, EventHandler, EventNotifier,
         // ServiceDef, Runset
         
         //
         // define the datastore responsible to store ManagedElement
         //
         typedef Cdmw::FT::DataStore<std::string,
                                     CdmwPlatformMngt::ManagedElementRecord> 
                 ManagedElementFTDataStore;

         // get ManagedElement datastore parameters from xml
         try
         {
             InitUtils::Get_datastore_parameters
                 (*(xml_data.get()),
                  "ManagedElementDataStore.datastore",
                  1,
                  datastore_id);
         } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                           CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                           CORBA::COMPLETED_NO),ex.what());
         }
    
    
         // create data store (registered itself)
         new ManagedElementFTDataStore (datastore_id,
                                        orb,
                                        CDMW_rootPOA.in(),
                                        *context,
                                        freeze_size,
                                        chunk_size,
                                        freeze_timeout); 
          
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_managed_element_datastore_id(datastore_id);

         // store datastore id in local datastore info
         (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
         ds_inc++;
          


         //
         // define the datastore responsible to store System
         //
         typedef Cdmw::FT::DataStore<std::string,
                                     CdmwPlatformMngt::SystemRecord> 
                 SystemFTDataStore;
      
         // get System datastore parameters from xml
         try
         {
             InitUtils::Get_datastore_parameters
             (*(xml_data.get()),
              "SystemDataStore.datastore",
              1,
              datastore_id);
         } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                           CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                           CORBA::COMPLETED_NO),ex.what() );
         }
    
         // create data store (registered itself)
         new SystemFTDataStore (datastore_id,
                                orb,
                                CDMW_rootPOA.in(),
                                *context,
                                freeze_size,
                                chunk_size,
                                freeze_timeout); 
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_system_datastore_id(datastore_id);

         // store datastore id in local datastore info
         (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
         ds_inc++;
         
         // 
         // define the datastore responsible to store Host
         //
         typedef Cdmw::FT::DataStore<std::string,
                                     CdmwPlatformMngt::HostRecord> 
                 HostFTDataStore;
         
         // get Host datastore parameters from xml
         try
         {
             InitUtils::Get_datastore_parameters
             (*(xml_data.get()),
              "HostDataStore.datastore",
              1,
              datastore_id);
         } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                           CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                           CORBA::COMPLETED_NO),ex.what() );
         }
    
    
         // create data store (registered itself)
         new HostFTDataStore (datastore_id,
                              orb,
                              CDMW_rootPOA.in(),
                              *context,
                              freeze_size,
                              chunk_size,
                              freeze_timeout); 
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_host_datastore_id(datastore_id);
    
         // store datastore id in local datastore info
         (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
         ds_inc++;
          


         //
         // define the datastore responsible to store Application
         //
         typedef Cdmw::FT::DataStore<std::string,
                            CdmwPlatformMngt::ApplicationRecord> 
                 ApplicationFTDataStore;
                 
         // get Application datastore parameters from xml        
         try
         {
             InitUtils::Get_datastore_parameters
             (*(xml_data.get()),
              "ApplicationDataStore.datastore",
              1,
              datastore_id);
         } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2(Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                          CORBA::COMPLETED_NO),ex.what() );
         }
    
         // create data store (registered itself)
         new ApplicationFTDataStore (datastore_id,
                                     orb,
                                     CDMW_rootPOA.in(),
                                     *context,
                                     freeze_size,
                                     chunk_size,
                                     freeze_timeout); 
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_application_datastore_id(datastore_id);
    
         // store datastore id in local datastore info
         (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
         ds_inc++;


         //
         // define the datastore responsible to store Process
         //
         typedef Cdmw::FT::DataStore<std::string,
                                     CdmwPlatformMngt::ProcessRecord> 
                 ProcessFTDataStore;

         // get Process datastore parameters from xml
         try
         {
             InitUtils::Get_datastore_parameters
             (*(xml_data.get()),
              "ProcessDataStore.datastore",
              1,
              datastore_id);
         } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                           CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                           CORBA::COMPLETED_NO),ex.what() );
         }
    
         // create data store (registered itself)
         new ProcessFTDataStore (datastore_id,
                                 orb,
                                 CDMW_rootPOA.in(),
                                 *context,
                                 freeze_size,
                                 chunk_size,
                                 freeze_timeout); 
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_process_datastore_id(datastore_id);
    
         // store datastore id in local datastore info
         (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
         ds_inc++;
          
         //
         // define the datastore responsible to store Entity
         //
         typedef Cdmw::FT::DataStore<std::string,
                                     CdmwPlatformMngt::EntityRecord> 
                 EntityFTDataStore;

         // get Entity datastore parameters from xml
         try
         {
             InitUtils::Get_datastore_parameters
                 (*(xml_data.get()),
                  "EntityDataStore.datastore",
                  1,
                  datastore_id);
         } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                           CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                           CORBA::COMPLETED_NO),ex.what() );
         }
    
    
         // create data store (registered itself)
         new EntityFTDataStore (datastore_id,
                                orb,
                                CDMW_rootPOA.in(),
                                *context,
                                freeze_size,
                                chunk_size,
                                freeze_timeout); 
                                
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_entity_datastore_id(datastore_id);
    
         // store datastore id in local datastore info
         (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
         ds_inc++;
         


         //
         // define the datastore responsible to store SystemObserverRegistration
         //
         typedef Cdmw::FT::DataStore<std::string,
                                     CdmwPlatformMngt::SystemObserverRegistrationRecord> 
                 SystemObserverRegistrationFTDataStore;
                 
         // get SystemObserverRegistration datastore parameters from xml
         try
         {
             InitUtils::Get_datastore_parameters
             (*(xml_data.get()),
              "SystemObserverRegistrationDataStore.datastore",
              1,
              datastore_id);
          } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                           CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                           CORBA::COMPLETED_NO),ex.what() );
          }
    
          // create data store (registered itself)
          new SystemObserverRegistrationFTDataStore (datastore_id,
                                                     orb,
                                                     CDMW_rootPOA.in(),
                                                     *context,
                                                     freeze_size,
                                                     chunk_size,
                                                     freeze_timeout);
    
          // Store datastore_id in the Configuration class to have access inside
          // the whole SystemMngt CSCI
          PlatformMngt::Configuration::Set_system_observer_registration_datastore_id(datastore_id);
    
          // store datastore id in local datastore info
          (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
          ds_inc++;


          //
          // define the datastore responsible to store EventHandler
          //
          typedef Cdmw::FT::DataStore<CdmwPlatformMngtBase::EventKey,
                                      CdmwPlatformMngt::EventReportRecord>
                  EventReportFTDataStore;

          // get EventHandler datastore parameters from xml
          try
          {
              InitUtils::Get_datastore_parameters
              (*(xml_data.get()),
               "EventReportDataStore.datastore",
               1,
               datastore_id);
          } catch (const Cdmw::BadParameterException& ex) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                            CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                            CORBA::COMPLETED_NO),ex.what() );
          }
    
          // create data store (registered itself)
          new EventReportFTDataStore (datastore_id,
                                      orb,
                                      CDMW_rootPOA.in(),
                                      *context,
                                      freeze_size,
                                      chunk_size,
                                      freeze_timeout); 
    
          // Store datastore_id in the Configuration class to have access inside
          // the whole SystemMngt CSCI
          PlatformMngt::Configuration::Set_event_report_datastore_id(datastore_id);
     
          // store datastore id in local datastore info
          (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
          ds_inc++;

 
          //
          // define the datastore responsible to store EventNotifier
          //
         typedef Cdmw::FT::DataStore<CdmwPlatformMngtBase::EventKey,
                                     CdmwPlatformMngt::Event>
                 EventNotifierFTDataStore;
                 
         // get EventNotifier datastore parameters from xml
         try
         {
             InitUtils::Get_datastore_parameters
             (*(xml_data.get()),
              "EventNotifierDataStore.datastore",
              1,
              datastore_id);
         } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                           CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                           CORBA::COMPLETED_NO),ex.what() );
         }
    
    
         // create data store (registered itself)
         new EventNotifierFTDataStore (datastore_id,
                                       orb,
                                       CDMW_rootPOA.in(),
                                       *context,
                                       freeze_size,
                                       chunk_size,
                                       freeze_timeout); 
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_event_notifier_datastore_id(datastore_id);
    
         // store datastore id in local datastore info
         (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
         ds_inc++;
      
         //
         // define the datastore responsible to store ServiceDefs
         //
         typedef Cdmw::FT::DataStore<std::string,
                                     CdmwPlatformMngtService::ServiceDefs> 
                 ServiceDefsFTDataStore;
                
         // get serviceDefs datastore parameters from xml
         try
         {
             InitUtils::Get_datastore_parameters
             (*(xml_data.get()),
              "ServiceDefsDataStore.datastore",
              1,
              datastore_id);
         } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                           CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                           CORBA::COMPLETED_NO),ex.what() );
         }
    
         // create data store (registered itself)
         new ServiceDefsFTDataStore (datastore_id,
                                     orb,
                                     CDMW_rootPOA.in(),
                                     *context,
                                     freeze_size,
                                     chunk_size,
                                     freeze_timeout); 
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_service_defs_datastore_id(datastore_id);
    
         // store datastore id in local datastore info
         (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
         ds_inc++;

         //
         // define the datastore responsible to store RunSet
         //
         typedef Cdmw::FT::DataStore<std::string,
                                     CdmwPlatformMngt::RunSet> 
                 RunSetFTDataStore;
                 
         // get Runset datastore parameters from xml
         try
         {
             InitUtils::Get_datastore_parameters
             (*(xml_data.get()),
              "RunSetDataStore.datastore",
              1,
              datastore_id);
         } catch (const Cdmw::BadParameterException& ex) {
              CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                           CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                           CORBA::COMPLETED_NO),ex.what() );
         }
    
         // create data store (registered itself)
         new RunSetFTDataStore (datastore_id,
                                orb,
                                CDMW_rootPOA.in(),
                                *context,
                                freeze_size,
                                chunk_size,
                                freeze_timeout); 
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_runset_datastore_id(datastore_id);
    
         // store datastore id in local datastore info
         (*localDataStoreInfos)[0].dsids[ds_inc] = datastore_id;
         ds_inc++;


      
         
         
         // create the StatefullPrimaryBackupAdmin_impl and register it
         std::string string_arg = 
             ::Cdmw::OsSupport::OS::get_option_value( argc, argv, "--HotRestart");
             
         bool cold_restart= true;
         if (strcmp(string_arg.c_str(), "yes")==0)
             cold_restart = false;
      
         ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl * 
             statefull_primary_backup_admin
                = new ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl(
                                            orb, 
                                            CDMW_rootPOA.in(), 
                                            loc, 
                                            primary_backup_repository, 
                                            cold_restart);
               
         ::CdmwFT::Location::StatefullPrimaryBackupAdmin_var statefull_primary_backup_admin_ref 
             = statefull_primary_backup_admin->_this();
      
         replication_manager->register_statefull_location(
                     loc, 
                     statefull_primary_backup_admin_ref.in(),
                     localDataStoreInfos.in(), false);
   
   
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
void CDMW_cleanup(CORBA::ORB_ptr orb)
     throw ()
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
}

   
} // end of namespace PlatformMngt

} // end of namespace Cdmw


