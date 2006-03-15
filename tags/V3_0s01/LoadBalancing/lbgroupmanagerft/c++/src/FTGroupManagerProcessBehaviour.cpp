/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/**
 * @brief A Fault Tolerant Group Manager process behaviour 
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <Foundation/commonsvcs/naming/NamingUtil.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include "LoadBalancing/lbcommon/StateTransferConfig.hpp"
#include "LoadBalancing/lbgroupmanagerft/FTGroupManagerProcessBehaviour.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"
#include "FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp"
#include "FaultTolerance/ftlocationmanager/PrimaryBackupAdmin_impl.hpp"
#include "FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp"
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include "FaultTolerance/ftstatemanager/DataStoreContext.hpp"
#include "FaultTolerance/ftstatemanager/DataStoreGroup_impl.hpp"
#include <LoadBalancing/lbgroupmanager/TypeRepository_impl.hpp>

#include "ace/Thread.h"
#include "ace/Thread_Manager.h"
#include "ace/TP_Reactor.h"
#include "ace/Select_Reactor.h"
#include "ace/Get_Opt.h"
#include "ace/Task.h"
#include "ace/Timer_Queue.h"
#include "ace/Timer_Heap.h"
#include "ace/Timer_Wheel.h"
#include "ace/Timer_Hash.h"
#include "ace/Timer_List.h"

#define ECHO_HEADER() \
    "[CDMW LB Group Manager FT] (file: " << __FILE__ << ", line: " << __LINE__ << ")\n -->"

#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment:\
} while(0)

#endif


namespace Cdmw {
  namespace LB {
    typedef Cdmw::FT::DataStore<PortableGroup::ObjectGroupId,
				CdmwLB::ObjectGroupData>
    FTObjectGroupDataStore;
  }
}



Cdmw::LB::FTGroupManagerProcessBehaviour::FTGroupManagerProcessBehaviour
(CORBA::ORB_ptr orb,
 PortableServer::POA_ptr poa,
 bool primary,
 CdmwPlatformMngt::System_ptr system,
 ConfigurationMngr& conf_mngr) 
    throw()
  : m_conf_mngr(conf_mngr)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_poa = PortableServer::POA::_duplicate(poa);
    m_set_has_primary = primary;
    m_system = CdmwPlatformMngt::System::_duplicate(system);
}

Cdmw::LB::FTGroupManagerProcessBehaviour::~FTGroupManagerProcessBehaviour()
    throw()
{
}

CORBA::ULong
Cdmw::LB::FTGroupManagerProcessBehaviour::nb_steps()
    throw(CORBA::SystemException)
{
    return 1;
}


void Cdmw::LB::FTGroupManagerProcessBehaviour::on_initialise
(const CdmwPlatformMngtBase::StartupKind& )
    throw(CORBA::SystemException)
{
     // Get process name
    m_process_name =
        Cdmw::PlatformMngt::PlatformInterface::Get_process_name();
    std::string application_name = Cdmw::PlatformMngt::PlatformInterface::Get_application_name();
    std::cout << "   -------- Server " << m_process_name << " on_initialise --------" << std::endl;
    
    PortableServer::POAManager_var poaMgr =
        m_poa->the_POAManager();
    
    // create a POA with USE_SERVANT_MANAGER & NON_RETAIN policy (to use ServantLocator)
    CORBA::PolicyList policies;
    policies.length(3);
    policies[0] = m_poa->create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER); 
    policies[1] = m_poa->create_servant_retention_policy(PortableServer::NON_RETAIN); 
    policies[2] = m_poa->create_id_assignment_policy(PortableServer::USER_ID); 
        
    Cdmw::OrbSupport::StrategyList strategyList;
    PortableServer::POA_var myPOA = 
        Cdmw::OrbSupport::OrbSupport::create_POA(m_poa.in(),
                                                 "myPOA",
                                                 poaMgr.in(),
                                                 policies,
                                                 strategyList);
        
    // create Executor servant (using myPOA)
    std::cout << "       ---- Server " << m_process_name << ": create Executor servant" << std::endl;

    CORBA::Object_var objref;
    
    objref =  m_orb->resolve_initial_references("ReplicationManager");  
	 

    CdmwFT::ReplicationManager_var replication_manager = CdmwFT::ReplicationManager::_narrow(objref.in());

    long localDataStoreInfosSize = 1;
    CdmwFT::StateTransfer::LocalDataStoreInfos_var localDataStoreInfos = new CdmwFT::StateTransfer::LocalDataStoreInfos();
    localDataStoreInfos->length(localDataStoreInfosSize);

    long number_of_scheduler_thread = 5;
    long chunk_size = 200;
    long freeze_size =150;
    OsSupport::OS::Timeval freeze_timeout;
    unsigned long res_long = 500;


    freeze_timeout.seconds = res_long/1000;
    freeze_timeout.microseconds = (res_long%1000)*1000;


    Cdmw::FT::DataStoreContext* datastore_context = new Cdmw::FT::DataStoreContext( m_orb.in(),
										    m_poa.in(),
										    m_conf_mngr.getMaxTransactionInProgress(),
										    m_conf_mngr.getMaxTransactionDone(),
										    m_conf_mngr.getCohortTimeOut(),
										    m_conf_mngr.getCoordinatorTimeOut(),
										    number_of_scheduler_thread,
										    chunk_size,
										    freeze_size);

    CdmwFT::StateTransfer::LocalDataStoreInfo localDataStoreInfo ;
    CdmwFT::StateTransfer::DataStoreGroup_var objectgroupDataStore_ref = 	  
      (new Cdmw::FT::DataStoreGroup_impl(m_poa.in(),*datastore_context))->_this();
  
    long dataStore_inc=0;
    
    localDataStoreInfo.local_data_store = CdmwFT::StateTransfer::DataStoreGroup::_duplicate(objectgroupDataStore_ref.in());
    
    
    
    localDataStoreInfo.coordinator = 
      datastore_context->get_tpc_coordinator();
    localDataStoreInfo.cohort = 
      datastore_context->get_tpc_cohort();
    localDataStoreInfo.dsids.length(localDataStoreInfosSize);
    localDataStoreInfo.dsids[0] = OBJECT_GROUP_DATA_IDENTIFIER;

    (*localDataStoreInfos)[dataStore_inc] = localDataStoreInfo;

    new Cdmw::LB::FTObjectGroupDataStore(OBJECT_GROUP_DATA_IDENTIFIER,
					 m_orb.in(),
					 m_poa.in(),
					 *datastore_context,
					 freeze_size,
					 chunk_size,
					 freeze_timeout);

    // Location information shall be obtain from supervision
    ::FT::Location loc;
    loc.length(3);
    
    loc[0].id = CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
    loc[0].kind = "hostname";
    loc[1].id = application_name.c_str();
    loc[1].kind = "applicationname";
    loc[2].id = m_process_name.c_str();
    loc[2].kind = "processname";
    
    std::string location_str 
      = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    std::cout << "setup LocationManager for location: " << location_str << std::endl;
    
    
    // create a reference for the PrimaryBackupGroupRepository
    ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primaryBackupGroupRepository_impl
	= ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();
    
    ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl * statefull_primary_backup_admin
	= new ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl(m_orb.in(), 
								     m_poa.in(), 
								     loc, 
								     primaryBackupGroupRepository_impl, 
								     true);
    
    std::string observer_name = m_conf_mngr.getObserverName() + std::string("_") + Cdmw::OsSupport::OS::get_hostname();
    ::CdmwFT::Location::StatefullPrimaryBackupAdmin_var statefull_primary_backup_admin_ref 
	= statefull_primary_backup_admin->_this();
    
    
    replication_manager->register_statefull_location(loc, 
						     statefull_primary_backup_admin_ref.in(),
                                                     localDataStoreInfos.in(), false);
    
    
    // Create the TypeRepository object and register it
    CdmwLB::TypeRepository_var typeRepository =
      new Cdmw::LB::TypeRepository_impl(m_conf_mngr.getSupportedInterfaceMap());
    m_orb->register_initial_reference("LBTypeRepository", typeRepository.in());

    
    m_group_manager_impl =
      new Cdmw::LB::FTGroupManager_impl(m_orb.in(), 
					m_poa.in(), 
					m_conf_mngr.getDomainID(),
					m_system.in(),                                        
				        observer_name);
    
    
    // create activation handler
    std::cout << "       ---- Server " << m_process_name << ": create activation handler" << std::endl;
    m_activation_handler =
      new Cdmw::LB::LBGroupManagerActivationHandler(m_process_name,
						    m_group_manager_impl);
    
    // create ServantLocator
    std::cout << "       ---- Server " << m_process_name << ": create ServantLocator " << std::endl;
    std::string executor_oid = m_process_name;
    m_servant_locator = new Cdmw::LB::LBGroupManagerServantLocator(executor_oid,
								   m_activation_handler);

    // set ServantLocator to myPOA
    try {
        myPOA->set_servant_manager(m_servant_locator);
    } catch (const PortableServer::POA::WrongPolicy& e) {
        // shouldn't happen
        CDMW_ASSERT(false);
    }
        
    // create Executor object under myPOA
    std::cout << "       ---- Server " << m_process_name << ": create LBGroupManager reference " << std::endl;
    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(executor_oid.c_str());
    CORBA::Object_var group_manager_obj = 
        myPOA->create_reference_with_id(oid, "IDL:CdmwLB/LBGroupManager:1.0");
    m_group_manager_obj = CdmwLB::LBGroupManager::_narrow(group_manager_obj.in());
    m_orb->register_initial_reference(GROUP_MANAGER_NAME.c_str(),
				      m_group_manager_obj.in());
        

    // add Executor object as member of Executor group
    this->add_group_manager_to_group();        
        
    // activate POAManager
    std::cout << "       ---- Server " << m_process_name << ": activate POAManager " << std::endl;
    poaMgr->activate();
        
    std::cout << "   -------- Server " << m_process_name << " on_initialise END --------" << std::endl;
}

    
void Cdmw::LB::FTGroupManagerProcessBehaviour::on_next_step()
    throw( CdmwPlatformMngt::ProcessDelegate::InvalidStep,
           CORBA::SystemException )
{
}

void Cdmw::LB::FTGroupManagerProcessBehaviour::on_run()
    throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
           CORBA::SystemException )
{
    std::cout << "   -------- Server " << m_process_name << " running --------" << std::endl;
}

void Cdmw::LB::FTGroupManagerProcessBehaviour::on_stop()
    throw( CORBA::SystemException )
{    
    std::cout << "   -------- Server " << m_process_name << " stopping --------" << std::endl;
    m_activation_handler->deactivate();
}

    

void Cdmw::LB::FTGroupManagerProcessBehaviour::add_group_manager_to_group()
    	throw(::FT::ObjectGroupNotFound,
	      ::FT::MemberNotFound,
	      ::FT::PrimaryNotSet,
	      ::FT::BadReplicationStyle,
	      ::FT::MemberAlreadyPresent,
	      ::FT::ObjectNotAdded,
	      ::CORBA::ORB::InvalidName,
	      CORBA::SystemException)
{
    // Get the group repository
    CORBA::Object_var obj = 
	m_orb->resolve_initial_references( "FTGroupRepository" );
    m_group_repository = 
	CdmwFT::Location::PrimaryBackupGroupRepository::_narrow(obj.in());
        
    // Get this process' location
    ::FT::Location_var the_location = m_group_repository->the_location();
    m_location = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(
        the_location.in() );
    std::cout << "       ---- Server " << m_process_name << ": location is " << m_location << std::endl;
    
  
    // Get the Replication Manager
    std::cout << "       ---- Server " << m_process_name << ": Get the Replication Manager " << std::endl;
    CORBA::Object_var replication_mgr_obj
        = m_orb->resolve_initial_references("ReplicationManager");

    m_replication_mgr =
        CdmwFT::ReplicationManager::_narrow(replication_mgr_obj.in());
    
    // get Executor group reference
    CdmwLB::LBGroupManager_var lbGroupManagerGroup =
        getLBGroupManagerGroup();
    
    if (CORBA::is_nil(lbGroupManagerGroup.in())) {
	return; 
    }

    // create Location for this member
    std::cout << "       ---- Server " << m_process_name << ": Create Location for this member " << std::endl;
    ::FT::Location loc;
    loc.length( 3 );
    loc[0].id = CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
    loc[0].kind = "hostname";
    loc[1].id = "FTApplication";
    loc[1].kind = "applicationname";
    loc[2].id = m_process_name.c_str();
    loc[2].kind = "processname";        

    // add LBGroupManager object as a new member of the LBGroupManager object group
    ::FT::ObjectGroup_var obj_group;
    
    std::cout << "       ---- Server " << m_process_name << ": add " << m_process_name << " to object group" << std::endl;
    obj_group = m_replication_mgr->add_member(lbGroupManagerGroup.in(), loc, m_group_manager_obj.in());
    

    // Register activation handler
    std::cout << "       ---- Server " << m_process_name << ": Register activation handler" << std::endl;
    // get ActivationManager
    obj	= m_orb->resolve_initial_references( "FTActivationManager" );
    CdmwFT::Location::ActivationManager_var activation_manager
	= CdmwFT::Location::ActivationManager::_narrow( obj.in() );
    
    // Register activation handler into ActivationManager
    std::cout << "     Server " << m_process_name << ": register activation handler" << std::endl;
    activation_manager->register_activation_handler(
	lbGroupManagerGroup.in(), m_activation_handler );
    
    // Set LBGroupManager as primary if required
    if (m_set_has_primary) {
	std::cout << "       ---- Server " << m_process_name << ": set " << m_process_name << " as primary" << std::endl;
	obj_group = m_replication_mgr->set_primary_member( obj_group.in(), loc );
    } 
        
    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

    // get NameDomain to object_groups (for writing)
    CdmwNamingAndRepository::NameDomain_var objGroupsDomain = repository->resolve_name_domain (REPOSITORY_GROUP_MANAGER_GROUP_NAME.c_str());

    // register Executor ObjectGroup into repository
    std::cout << "       ---- GroupCreator: register object group into repository" << std::endl;
    try{
	objGroupsDomain->release_name(GROUP_MANAGER_OBJECT_GROUP_NAME.c_str());
    }catch(const CdmwNamingAndRepository::InvalidName& e)
    {
        DEBUG_DUMP(__FILE__ << "  " << __LINE__);
	DEBUG_DUMP("Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NamingContext::InvalidName raised!");
    }
    catch(const CdmwNamingAndRepository::NameDomain::NotRegistered& e)
    {
        DEBUG_DUMP(__FILE__ << "  " << __LINE__);
	DEBUG_DUMP("Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NameDomain::NotRegistered raised!");
    }
    
    
    CdmwNamingAndRepository::NameDomain::RegistrationId_var regId =
        objGroupsDomain->new_name (GROUP_MANAGER_OBJECT_GROUP_NAME.c_str());
    
    objGroupsDomain->register_object (regId.in(), obj_group.in ());
}


/**
 * Get LBGroupManager group reference from Cardamom Repository
 * This reference was created by the group_creator process
 */
CdmwLB::LBGroupManager*
Cdmw::LB::FTGroupManagerProcessBehaviour::getLBGroupManagerGroup() 
{

    // Get the Cdmw repository
    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
    
    // Get NamingInterface to object_groups (for reading)
    Cdmw::CommonSvcs::Naming::NamingInterface objGroupsInterface =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(REPOSITORY_GROUP_MANAGER_GROUP_NAME.c_str());

    // Get LB GroupManager Object Group
    std::cout << "       ---- Server " << m_process_name << ": Get LB GroupManager Object Group " << std::endl;
    CdmwLB::LBGroupManager_var lbGroupManagerGroup = CdmwLB::LBGroupManager::_nil();
    try {
        CORBA::Object_var obj = objGroupsInterface.resolve(GROUP_MANAGER_OBJECT_GROUP_NAME);   
        lbGroupManagerGroup = CdmwLB::LBGroupManager::_narrow(obj.in());
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        DEBUG_DUMP(__FILE__ << "  " << __LINE__);
        DEBUG_DUMP("Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NamingContext::NotFound raised!") ;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
	DEBUG_DUMP(__FILE__ << "  " << __LINE__) ;
	DEBUG_DUMP("Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NamingContext::CannotProceed raised!") ;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        DEBUG_DUMP(__FILE__ << "  " << __LINE__);
	DEBUG_DUMP("Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NamingContext::InvalidName raised!");
    }
    catch (const CORBA::SystemException& e)
    {
        DEBUG_DUMP(__FILE__ << "  " << __LINE__);
        DEBUG_DUMP("Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => CORBA::SystemException raised!");
    }
    if (CORBA::is_nil(lbGroupManagerGroup.in()))
    {
	::FT::Name group_manager_og_name;
	group_manager_og_name.length(1);
	group_manager_og_name[0].id=GROUP_MANAGER_OBJECT_GROUP_NAME.c_str();
	std::cout << "       ---- Server " << m_process_name << ": Get LB GroupManager empty Object Group from replication manager" << std::endl;
	CORBA::Object_var obj = m_replication_mgr->get_object_group_ref_from_name(group_manager_og_name);
	lbGroupManagerGroup = CdmwLB::LBGroupManager::_narrow(obj.in());
    }
    return lbGroupManagerGroup._retn();     
    
}    

    


