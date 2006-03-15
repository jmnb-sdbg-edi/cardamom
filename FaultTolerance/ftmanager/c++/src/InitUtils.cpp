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

#include <FaultTolerance/ftmanager/InitUtils.hpp>
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/String.hpp"
#include <Foundation/common/Assert.hpp> 
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/ossupport/OS.hpp"
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include "FaultTolerance/ftreplicationmanager/PrimaryBackupGroupRepositoryFTManager_impl.hpp"
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <FaultTolerance/ftcommon/FTConfiguration.hpp>
#include <ftgroupcreator/GroupConfigurationMngr.hpp>
#include "ftmanager/ServiceLocator_impl.hpp"
#include <FaultTolerance/ftcommon/FTManagerDSID.hpp>
#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/LogManager.hpp>
// include XML library utils
#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>



namespace 
{
    const char* FT_MANAGER_NAME = "fault_manager";
    std::string HELP_OPTION = "--help";
    std::string GROUP_CONFIG_OPTION = "--groupConf";
    void notifyPreInitialisationError(bool platformManaged, const char* message)
    {
        CDMW_ASSERT(0); // Let's see if this used!
        //        CDMW_FT_ERROR << "[FTManager] " << message << std::endl;
    }
} // end anonymous namespace


void print_syntax(char* program_name)
{
    CDMW_ERROR(Cdmw::FT::FTLogger::GetLogger(),
               "Syntax :\n"
               << program_name <<" " << Cdmw::Common::Options::PROCESS_XML_FILE_OPTION << "=XMLConfigFile\n"
               << "\n"
               << "Options:\n"
               << " [" << GROUP_CONFIG_OPTION <<"=XMLGroupConfigurationFile]"
               << " [" << Cdmw::Common::Options::VALIDATE_XML_OPTION<< "]");
}


namespace Cdmw {
namespace FT {
namespace FTManager {
    FTManagerDataStore * InitUtils::M_ftManagerDataStore = NULL;
    IOGRInfoDataStore * InitUtils::M_iogrInfoDataStore = NULL;
    ConsumerIdDataStore * InitUtils::M_consumerIdDataStore = NULL;
    SequencePushConsumerDataStore * InitUtils::M_sequencePushConsumerDataStore = NULL;
    StructuredPushConsumerDataStore * InitUtils::M_structuredPushConsumerDataStore = NULL;
    HostDataStore * InitUtils::M_hostDataStore = NULL;
    MonitoringIdDataStore * InitUtils::M_monitoringIdDataStore = NULL;
    HostStatusDataStore * InitUtils::M_hostStatusDataStore = NULL;
    LocInfoDataStore * InitUtils::M_locInfoDataStore = NULL;
    OGIDDataStore * InitUtils::M_ogidDataStore = NULL;
    OGDDataStore * InitUtils::M_ogdDataStore = NULL;
    PropertiesDataStore * InitUtils::M_propertiesDataStore = NULL;
    Cdmw::FT::DataStoreContext * InitUtils::M_context = NULL;
    std::string InitUtils::M_xml_conf = "";
    std::string InitUtils::M_xml_group_conf = "";
    bool InitUtils::M_validate = false;


/**
 * Purpose:
 * <p> pre_init
 * 
 *@exception CORBA::SystemException
 */        
    
std::string
InitUtils::Pre_init(int& argc, char** argv)
    throw(ConfigurationException, CORBA::SystemException)
{

    // Initialize the XML4C2 system
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch(const XMLException& toCatch)
    {
       std::string xmlExceptionMsg (XMLString::transcode(toCatch.getMessage()));
       std::string msg = "Error during Xerces-c Initialization: " + xmlExceptionMsg;

       CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                    CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO),
                    msg);
    }


    // First initialise the CARDAMOM logging library is initialised.
    try {
        Cdmw::Logging::LogManager::Init(argc, argv);
        // From now on, CARDAMOM logging library is initialised.
    } catch (const std::exception & e ) {
        std::cerr << "Cannot initialise CARDAMOM Logging library! \n"
                  << "Caught exception : " << e.what() << std::endl;
        ::exit(EXIT_FAILURE);
    }
    std::string help_option =
    Cdmw::OsSupport::OS::get_option_value( argc, argv, HELP_OPTION);
    if (help_option == "yes")
    {  
        print_syntax (argv[0]);
    }
  
    // checks the xml configuration file option
    std::string xml_conf_option =
    Cdmw::OsSupport::OS::get_option_value( argc, argv, Cdmw::Common::Options::PROCESS_XML_FILE_OPTION);
    
    if ( xml_conf_option != "no" )
    {
        if (xml_conf_option != "yes")
        {
            if (Cdmw::OsSupport::OS::file_exists(xml_conf_option))
            {
                M_xml_conf = xml_conf_option;  		
                
                // checks the optional validate option
                std::string validateXML_option =
                Cdmw::OsSupport::OS::get_option_value( argc, argv, Cdmw::Common::Options::VALIDATE_XML_OPTION);

                if (validateXML_option != "no")
                {
                    M_validate = true;
                }

            }
            else
            {
                CDMW_ERROR(Cdmw::FT::FTLogger::GetLogger(),"The xml configuration file doesn't exist");
                ::exit(EXIT_FAILURE);
            }
                
        }
        else
        {
            CDMW_ERROR(Cdmw::FT::FTLogger::GetLogger(),"The xml configuration file must be specified");
            ::exit(EXIT_FAILURE);
        }
    }
    else
    {
        CDMW_ERROR(Cdmw::FT::FTLogger::GetLogger(),"The xml configuration file must be specified");
        ::exit(EXIT_FAILURE);
    }


    // checks the xml group configuration file option
    CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"Checking the xml configuration file option.");
    std::string xml_group_conf_option =
    Cdmw::OsSupport::OS::get_option_value(argc, argv, GROUP_CONFIG_OPTION);

    if (xml_group_conf_option != "no")
    {
        if (xml_group_conf_option != "yes")
        {
            if (Cdmw::OsSupport::OS::file_exists(xml_group_conf_option))
            {
                M_xml_group_conf = xml_group_conf_option;  		

                // checks the optional validate option
                std::string validateXML_option =
                Cdmw::OsSupport::OS::get_option_value( argc, argv, GROUP_CONFIG_OPTION);

                if (validateXML_option != "no")
                {
                    M_validate = true;
                }
            }
            else
            {
                CDMW_ERROR(Cdmw::FT::FTLogger::GetLogger(),"No group configuration file present in the current directory");
                ::exit(EXIT_FAILURE);
            }
        }
        // else this option is not mandatory!
    }

    // The default configuration
    Cdmw::FT::ConfigurationMngr conf;
    // If an XML configuration file has been transmit, we use it

    if (!M_xml_conf.empty())
    {
        conf.parse(M_xml_conf, M_validate);
    }


    // pre_init orb for ReplicationManager purpose
    Cdmw::FT::ReplicationManager::InitUtils::Pre_init(argc, argv);

    return conf.getSystemPortString();
}
    
    
    
/** Purpose:
 * <p> post_init 
 * 
 *@exception CORBA::SystemException
 */    
    
void
InitUtils::Post_init (int& argc, char** argv, CORBA::ORB_ptr orb)
    throw(ConfigurationException, CORBA::SystemException)
{
    
    // The default configuration
    Cdmw::FT::ConfigurationMngr conf;
    // If an XML configuration file has been transmit, we use it
    if (!M_xml_conf.empty())
    {
        conf.parse(M_xml_conf, M_validate);
    }


    // If an XML Group configuration file has been transmited, we use it to
    // create default group needed by the system
    if (!M_xml_group_conf.empty() && M_validate)
    {
        CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"Validate default Group configuration..." );
        // The default Group configuration
        CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"create GroupConfigurationMngr " );
        Cdmw::FT::GroupConfigurationMngr group_conf(true);
        CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"GroupConfigurationMngr parse"<< M_xml_group_conf );
        group_conf.validate(M_xml_group_conf);
    }
    else
        CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"GroupConfigurationMngr parse"<< M_xml_group_conf );


    // TO be removed walk around!!!!!
    Cdmw::OrbSupport::OrbSupport::set_endpoint_selector_factory(Cdmw::FT::FTConfiguration::Get_FT_endpoint_selector_factory());


    CORBA::Object_var obj;
    obj = orb->resolve_initial_references("ORBPolicyManager");

    CORBA::PolicyManager_var policy_manager 
    = CORBA::PolicyManager::_narrow(obj.in());
    TimeBase::TimeT duration_time = conf.getRequestDurationTime();        
    
    CORBA::Any timeout_as_any;
    timeout_as_any <<= static_cast<TimeBase::TimeT>(500000);
    CORBA::Any any;
    any <<= duration_time;
    CORBA::PolicyList policies(2);
    policies.length(2);
    policies[0] =
    orb->create_policy(::FT::REQUEST_DURATION_POLICY, any);
    policies[1] =
    orb->create_policy (TAO::CONNECTION_TIMEOUT_POLICY_TYPE,
                        timeout_as_any);
    
    policy_manager->set_policy_overrides(policies, CORBA::SET_OVERRIDE);
    
    // Activate the POA manager
    obj = orb->resolve_initial_references( "RootPOA" );
    PortableServer::POA_var poa = PortableServer::POA::_narrow( obj.in() );
    
    PortableServer::POAManager_var mgr = poa->the_POAManager();
    mgr->activate();

    
    // Create datastore Context


    //create the properties data store and storage home
    M_context = new Cdmw::FT::DataStoreContext(orb,
                                               poa.in(),
                                               //                                                    conf.getSystemPortString(),
                                               //                                                    conf.getDomainID(),
                                               //                                                    conf.getStateTransfertIPAddress(),
                                               conf.getMaxTransactionInProgress(),
                                               conf.getMaxTransactionDone(),
                                               conf.getCohortTimeOut(),
                                               conf.getCoordinatorTimeOut());


    Cdmw::FT::TXManager* manager = new Cdmw::FT::TXManager(*M_context);

    // create datastore
    M_propertiesDataStore = new PropertiesDataStore(*manager,
                                                    PROPERTIES_DATA_STORE,
                                                    orb,
                                                    poa.in());
        
    OsSupport::OS::Timeval time;
    time.seconds = 10;
    time.microseconds = 0;
        
    M_ogdDataStore = new OGDDataStore (*manager,
                                       OGD_DATA_STORE,
                                       orb,
                                       poa.in(), 20, 100, time);
        
        
    M_ogidDataStore = new OGIDDataStore (*manager,
                                         OGID_DATA_STORE,
                                         orb,
                                         poa.in());
        
        
    M_locInfoDataStore = new LocInfoDataStore(*manager,
                                              LOCATIONS_DATA_STORE,
                                              orb,
                                              poa.in());
        
        
    //simple monitoring observer
    M_hostStatusDataStore = new HostStatusDataStore (*manager,
                                                     HOST_DATA_STORE2,
                                                     orb,
                                                     poa.in());
        
    // fault detector
    M_monitoringIdDataStore = new MonitoringIdDataStore(*manager,
                                                        MONITORING_ID_DATA_STORE,
                                                        orb,
                                                        poa.in());
        
    M_hostDataStore = new HostDataStore(*manager,
                                        HOST_DATA_STORE,
                                        orb,
                                        poa.in());
        
    // fault notifier
    M_structuredPushConsumerDataStore = new StructuredPushConsumerDataStore(*manager,
                                                                            STRUCTURED_PUSH_CONSUMER_DATA_STORE,
                                                                            orb,
                                                                            poa.in());
        
        
    M_sequencePushConsumerDataStore = new SequencePushConsumerDataStore(*manager,
                                                                        SEQUENCE_PUSH_CONSUMER_DATA_STORE,
                                                                        orb,
                                                                        poa.in());
        
    M_consumerIdDataStore = new ConsumerIdDataStore(*manager,
                                                    CONSUMER_ID_DATA_STORE,
                                                    orb,
                                                    poa.in()); 
        
        
    //ftmanager
    M_iogrInfoDataStore = new IOGRInfoDataStore(*manager,
                                                IOGR_INFOS_DATA_STORE,
                                                orb,
                                                poa.in());
        
        
        
    M_ftManagerDataStore = new FTManagerDataStore(*manager,
                                                  FT_MANAGER_DATA_STORE,
                                                  orb,
                                                  poa.in());
        

    // Create the TypeRepository object and register it
    CdmwFT::TypeRepository_var typeRepository =
    new Cdmw::FT::TypeRepository_impl(conf.getSupportedInterfaceMap());
    orb->register_initial_reference("FTTypeRepository", typeRepository.in());

    // Create a replication manager
    CdmwFT::ReplicationManager_var replication_manager
    = Cdmw::FT::ReplicationManager::InitUtils::Create_replication_manager(argc,
                                                                          argv,
                                                                          orb,
                                                                          poa.in(),
                                                                          conf.getDomainID(),
                                                                          conf.getSecurityLevel(),
                                                                          conf.getReplicationLevel(),
                                                                          std::cout);

    // post_init orb for ReplicationManager purpose
    Cdmw::FT::ReplicationManager::InitUtils::Post_init(replication_manager.in());

    std::string str_opt_mt("");
    str_opt_mt += "--SIMPLE_PULL_CORBALOC ";
    str_opt_mt += conf.getSimplePullCorbaloc();
    str_opt_mt += " --SIMPLE_PUSH_CORBALOC ";
    str_opt_mt += conf.getSimplePushCorbaloc();
    str_opt_mt += " --MUTUAL_PULL_CORBALOC ";
    str_opt_mt += conf.getMutualPullCorbaloc();
    str_opt_mt += " --MUTUAL_PUSH_CORBALOC ";
    str_opt_mt += conf.getMutualPushCorbaloc();
    int argc_mt(8);

    if (conf.getMasterInterrogationTime() != "0")
    {            
        str_opt_mt += " --MASTER_INTERROGATION ";
        str_opt_mt += conf.getMasterInterrogationTime();
        argc_mt = argc_mt + 2;
    }
    if (conf.getSlaveAcknowledgementTime() != "0")
    {
        str_opt_mt += " --SLAVE_ACKNOWLEDGEMENT ";
        str_opt_mt += conf.getSlaveAcknowledgementTime();            
        argc_mt = argc_mt + 2;
    }
    if (conf.getMasterConfirmationTime() != "0")
    {
        str_opt_mt += " --MASTER_CONFIRMATION ";
        str_opt_mt += conf.getMasterConfirmationTime();            
        argc_mt = argc_mt + 2;
    }
    if (conf.getMasterElectionTime() != "0")
    {
        str_opt_mt += " --MASTER_ELECTION ";
        str_opt_mt += conf.getMasterElectionTime();            
        argc_mt = argc_mt + 2;
    }
    if (conf.getDaemonAcknowledgementTime() != "0")
    {
        str_opt_mt += " --DAEMON_ACKNOWLEDGEMENT ";
        str_opt_mt += conf.getDaemonAcknowledgementTime();
        argc_mt = argc_mt + 2;
    }

    char** argv_mt = Cdmw::Common::String::to_char_array
    (Cdmw::Common::String::to_strings(str_opt_mt));

    CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"Creating the monitoringLibraryInterface");
    CdmwFT::MonitoringLibrary::MonitoringLibraryInterface_var monitoringLibraryInterface = 
    Cdmw::FT::InitUtils::Create_monitoring_library( argc_mt, argv_mt, orb);

    // Create a supervision fault detector 
    CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"Creating the fault detector");    
    Cdmw::FT::FaultManagement::FaultDetector_impl* observer_impl
    = new Cdmw::FT::FaultManagement::FaultDetector_impl(orb,
                                                        poa.in(),
                                                        monitoringLibraryInterface.in());

    CdmwFT::FaultManagement::FaultDetector_var observer = observer_impl->_this();
    // observer_impl will be managed by the POA
    observer_impl->_remove_ref();

    // Create a fault notifier 
    CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"Creating the fault notifier");
    Cdmw::FT::FaultNotifier_impl* notifier_impl
    = new Cdmw::FT::FaultNotifier_impl(orb,
                                       poa.in());
        
    ::FT::FaultNotifier_var notifier = notifier_impl->_this();
        
    // register a local onsumer into the faultNotifier
    CosNotifyComm::StructuredPushConsumer_var rm_consumer
    = CosNotifyComm::StructuredPushConsumer::_narrow(replication_manager.in());
    notifier_impl->register_local_consumer(rm_consumer.in());
        
    //register the fault notifier to the fault detector
    CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"Set the fault notifier");
    observer->set_fault_notifier( notifier.in() );

    // create the primary backup admin 
    CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"Creating the PrimaryBackupGroupRepository");
    ::Cdmw::FT::ReplicationManager::PrimaryBackupGroupRepositoryFTManager_impl * primary_backup_repository
      = ::Cdmw::FT::ReplicationManager::PrimaryBackupGroupRepositoryFTManager_impl::Get_instance();
        
    ::FT::Location loc;
    loc.length(1);
    loc[0].id = CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
        
         
         
    // create the ft manger object
    CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"Creating the FTManager");
    Cdmw::FT::FTManager_impl* ftManagerObj 
    = new Cdmw::FT::FTManager_impl(orb,
                                   poa.in(),
                                   notifier_impl,
                                   observer_impl,
                                   replication_manager.in(),
                                   conf.getDomainID(),
                                   conf.getSystemPortString(),
                                   M_xml_group_conf);
        
    CdmwFT::FTManager_var ftManager = ftManagerObj->_this();
    // ftManagerObj is kept by POA, thus remove local ref
    ftManagerObj->_remove_ref();
        

    std::string multicast_addr = conf.getLocalisationServiceAddress();
    CORBA::Object_var group = orb->string_to_object(multicast_addr.c_str());
    PortableServer::ObjectId_var  service_locator_id = poa->create_id_for_reference(group.in());
    Cdmw::FT::ServiceLocator_impl* service_locator_obj = new  Cdmw::FT::ServiceLocator_impl(orb, ftManagerObj->get_IOGR_dsid());
    poa->activate_object_with_id (service_locator_id.in (),service_locator_obj);
        

    // initialise the primary_backup_repository
    primary_backup_repository->init(orb,
                                    loc,
                                    ftManager.in());

    //bind the ft manager to the corbaloc
    OrbSupport::OrbSupport::bind_object_to_corbaloc(orb, FT_MANAGER_NAME, ftManager.in());

        
    //create the mutual monitoring object

    CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"Create the locations");
    CdmwFT::MonitoringLibrary::Locations_var locations = new CdmwFT::MonitoringLibrary::Locations;
    locations->length(1);
    locations[0L]=OsSupport::OS::get_hostname().c_str();
        
        
    CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"Create the mutualMonitoring");        
    CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoring_var mutualMonitoring 
    = monitoringLibraryInterface->create_mutual_monitoring(locations.in());

    CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"Create the mutualMonitoringObserver");        
    Cdmw::FT::MutualMonitoringObserver_impl* mutualMonitoringObserver 
    = new Cdmw::FT::MutualMonitoringObserver_impl(notifier.in(), ftManagerObj);
        
    mutualMonitoring->attach_observer(mutualMonitoringObserver);
    ftManagerObj->register_mutual_monitoring(mutualMonitoring.in());        
    mutualMonitoring->start_monitoring();
    // Print out the corbalocs
    CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"corbaloc::" << OsSupport::OS::get_hostname()
              << ":" << conf.getSystemPortString()
              << "/" << FT_MANAGER_NAME );
}

       
    
/** Purpose:
 * <p> cleanup
 * 
 *@exception CORBA::SystemException
 */    
    
void
InitUtils::Cleanup()
throw(CORBA::SystemException)
{

    delete M_ftManagerDataStore;
    delete M_iogrInfoDataStore;
    delete M_consumerIdDataStore;
    delete M_sequencePushConsumerDataStore;
    delete M_structuredPushConsumerDataStore;
    delete M_hostDataStore;
    delete M_monitoringIdDataStore;
    delete M_hostStatusDataStore;
    delete M_locInfoDataStore;
    delete M_ogidDataStore;
    delete M_ogdDataStore;
    delete M_propertiesDataStore;

    delete M_context;

    try
    {
        XMLPlatformUtils::Terminate();
    }
    catch(const XMLException& toCatch)
    {
       std::string xmlExceptionMsg (XMLString::transcode(toCatch.getMessage()));
       std::string msg = "Error during Xerces-c Termination: " + xmlExceptionMsg;

       CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                    CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO),
                    msg);
    }
}

}; // namespace FTManager
}; // namespace FT
}; // namespace Cdmw

