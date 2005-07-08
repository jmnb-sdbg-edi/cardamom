/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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

 
// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <Repository/naminginterface/NamingUtil.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

// Demo Files
#include "Time/ftclocksvcs/FTClockServiceProcessBehaviour.hpp"
#include "Time/clockservice/StateTransferConfig.hpp"

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

static size_t reactor_threads        = 10;
static size_t timer_wheel_spokes     = 8192;
static size_t timer_wheel_resolution = 1000; // 1ms
ACE_Reactor* theReactor = 0;

Cdmw::clock::svcs::FTClockServiceProcessBehaviour::FTClockServiceProcessBehaviour
(CORBA::ORB_ptr orb,
 bool primary)  
    throw()
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_set_has_primary = primary;

}

Cdmw::clock::svcs::FTClockServiceProcessBehaviour::~FTClockServiceProcessBehaviour()
    throw()
{
    
}



    
CORBA::ULong
Cdmw::clock::svcs::FTClockServiceProcessBehaviour::nb_steps()
    throw(CORBA::SystemException)
{
    return 1;
}


void Cdmw::clock::svcs::FTClockServiceProcessBehaviour::on_initialise
(const CdmwPlatformMngtBase::StartupKind& )
    throw(CORBA::SystemException)
{
    // Get process name
    m_process_name =
        Cdmw::PlatformMngt::PlatformInterface::getProcessName();
    std::cout << "   -------- Server " << m_process_name << " on_initialise --------" << std::endl;
    
    // get root POA            
    CORBA::Object_var obj =
        m_orb->resolve_initial_references( "RootPOA" );
    PortableServer::POA_var rootPOA =
        PortableServer::POA::_narrow( obj.in() );
    PortableServer::POAManager_var poaMgr =
        rootPOA->the_POAManager();
            
    // create a POA with USE_SERVANT_MANAGER & NON_RETAIN policy (to use ServantLocator)
    CORBA::PolicyList policies;
    policies.length(3);
    policies[0] = rootPOA->create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER); 
    policies[1] = rootPOA->create_servant_retention_policy(PortableServer::NON_RETAIN); 
    policies[2] = rootPOA->create_id_assignment_policy(PortableServer::USER_ID); 
        
    Cdmw::OrbSupport::StrategyList strategyList;
    PortableServer::POA_var myPOA = 
        Cdmw::OrbSupport::OrbSupport::create_POA(rootPOA.in(),
                                                 "myPOA",
                                                 poaMgr.in(),
                                                 policies,
                                                 strategyList);
        
    // create Executor servant (using myPOA)
    std::cout << "       ---- Server " << m_process_name << ": create Executor servant" << std::endl;

    ACE_TP_Reactor* tp_reactor = new ACE_TP_Reactor();
    ACE_Reactor* reactor = new ACE_Reactor(tp_reactor);
    ACE_Timer_Wheel* timer_queue = new ACE_Timer_Wheel(timer_wheel_spokes,
                                                       timer_wheel_resolution);
    reactor->timer_queue(timer_queue);
    ACE_Reactor::instance (reactor);
    
    int retval;
    Cdmw::clock::svcs::ReactorWorker* reactor_worker =
        new Cdmw::clock::svcs::ReactorWorker();
    retval =
        reactor_worker->activate(THR_NEW_LWP | THR_JOINABLE, reactor_threads);
    if (retval != 0)
        std::cerr << "EVoT:>> Cannot activate the Thread Pool\n" << std::endl;
    
    m_executor_impl =
        new Cdmw::clock::FTClock_impl(m_orb.in());
    

    // create activation handler
    std::cout << "       ---- Server " << m_process_name << ": create activation handler" << std::endl;
    m_activation_handler =
        new Cdmw::clock::FTClockActivationHandler(m_process_name,
                                                  m_executor_impl);

    // create ServantLocator
    std::cout << "       ---- Server " << m_process_name << ": create ServantLocator " << std::endl;
    std::string executor_oid = m_process_name;
    m_servant_locator = new Cdmw::clock::FTClockServantLocator(executor_oid,
                                                               m_activation_handler);

    // set ServantLocator to myPOA
    try {
        myPOA->set_servant_manager(m_servant_locator);
    } catch (const PortableServer::POA::WrongPolicy& e) {
        // shouldn't happen
        CDMW_ASSERT(false);
    }
        
    // create Executor object under myPOA
    std::cout << "       ---- Server " << m_process_name << ": create Executor reference " << std::endl;
    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(executor_oid.c_str());
    CORBA::Object_var executor_obj = 
        myPOA->create_reference_with_id(oid, "IDL:omg.org/CosClockService/PeriodicExecution/Executor:1.0");
    m_executor_obj =
        CosClockService::PeriodicExecution::Executor::_narrow(executor_obj.in());

    // add Executor object as member of Executor group
    this->add_executor_to_group();        
        
    // activate POAManager
    std::cout << "       ---- Server " << m_process_name << ": activate POAManager " << std::endl;
    poaMgr->activate();
        
    std::cout << "   -------- Server " << m_process_name << " on_initialise END --------" << std::endl;
}

    
void Cdmw::clock::svcs::FTClockServiceProcessBehaviour::on_next_step()
    throw( CdmwPlatformMngt::Process::InvalidStep,
           CORBA::SystemException )
{
}

void Cdmw::clock::svcs::FTClockServiceProcessBehaviour::on_run()
    throw( CdmwPlatformMngt::Process::NotReadyToRun,
           CORBA::SystemException )
{
    std::cout << "   -------- Server " << m_process_name << " running --------" << std::endl;
}

void Cdmw::clock::svcs::FTClockServiceProcessBehaviour::on_stop()
    throw( CORBA::SystemException )
{    
    std::cout << "   -------- Server " << m_process_name << " stopping --------" << std::endl;
    m_orb->shutdown();
}

    

void Cdmw::clock::svcs::FTClockServiceProcessBehaviour::add_executor_to_group() 
{
    // Get the group repository
    try
    {
        CORBA::Object_var obj = 
            m_orb->resolve_initial_references( "FTGroupRepository" );
        m_group_repository = 
            CdmwFT::Location::PrimaryBackupGroupRepository::_narrow(obj.in());
    }
    catch (const CORBA::ORB::InvalidName&)
    {
        std::cerr << "ERROR: Impossible to resolve FTGroupRepository! "
                  << "Name is invalid!"
                  << std::endl;
    }
    
    // Get this process' location
    ::FT::Location_var the_location = m_group_repository->the_location();
    m_location = Cdmw::NamingAndRepository::NamingInterface::to_string(
        the_location.in() );
    std::cout << "       ---- Server " << m_process_name << ": location is " << m_location << std::endl;
    
    // get Executor group reference
    CosClockService::PeriodicExecution::Executor_var executorGroup =
        getExecutorGroup();
    
    if (CORBA::is_nil(executorGroup.in())) {
        return;
    }

    // Get the Replication Manager
    std::cout << "       ---- Server " << m_process_name << ": Get the Replication Manager " << std::endl;
    CORBA::Object_var replication_mgr_obj
        = m_orb->resolve_initial_references("ReplicationManager");

    m_replication_mgr =
        CdmwFT::ReplicationManager::_narrow(replication_mgr_obj.in());

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

    // add Executor object as a new member of the Executor object group
    ::FT::ObjectGroup_var obj_group;
    try {
        std::cout << "       ---- Server " << m_process_name << ": add " << m_process_name << " to object group" << std::endl;
        obj_group = m_replication_mgr->add_member(executorGroup.in(), loc, m_executor_obj.in());
    }
    catch (const ::FT::ObjectGroupNotFound&)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "add_member " << m_process_name << " => FT::ObjectGroupNotFound raised!" << std::endl;
        return;
    }
    catch (const ::FT::MemberAlreadyPresent&)
   {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "add_member " << m_process_name << " => FT::MemberAlreadyPresent raised!" << std::endl;
        return;
    }
    catch (const ::FT::ObjectNotAdded&)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "add_member " << m_process_name << " => FT::ObjectNotAdded raised!" << std::endl;
        return;
    }        

    // Register activation handler
    std::cout << "       ---- Server " << m_process_name << ": Register activation handler" << std::endl;
    try
    {
        // get ActivationManager
        CORBA::Object_var obj
            = m_orb->resolve_initial_references( "FTActivationManager" );
        CdmwFT::Location::ActivationManager_var activation_manager
            = CdmwFT::Location::ActivationManager::_narrow( obj.in() );

        // Register activation handler into ActivationManager
        std::cout << "     Server " << m_process_name << ": register activation handler" << std::endl;
        activation_manager->register_activation_handler(
                executorGroup.in(), m_activation_handler );
    }
    catch(const ::FT::ObjectGroupNotFound& )
    {
        std::cerr << "ERROR: Impossible to register FTActivationHandler! "
                  << "Object Group not found!"
                  << std::endl;
    }
    catch( const ::CORBA::ORB::InvalidName& )
    {
        std::cerr << "ERROR: Impossible to register FTActivationManager! "
                  << "Name is invalid!"
                  << std::endl;
    }
    catch( const ::CORBA::SystemException& e )
    {
        std::cerr << "ERROR: Impossible to register FTActivationHandler!"
                  << std::endl;
        std::cerr << e << std::endl;
    }


    // Set Executor as primary if required
    if (m_set_has_primary) {
        try {
            std::cout << "       ---- Server " << m_process_name << ": set " << m_process_name << " as primary" << std::endl;
            obj_group = m_replication_mgr->set_primary_member( obj_group.in(), loc );
        } 
        catch (const ::FT::ObjectGroupNotFound& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "set_primary_member => FT::ObjectGroupNotFound raised!" << std::endl;
        }
        catch (const ::FT::MemberNotFound& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "set_primary_member => FT::MemberNotFound raised!" << std::endl;
        }
        catch (const ::FT::PrimaryNotSet& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "set_primary_member => FT::MemberNotFound raised!" << std::endl;
        }
        catch (const ::FT::BadReplicationStyle& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "set_primary_member => FT::BadReplicationStyle raised!" << std::endl;
        }
    }
}


/**
 * Get Executor group reference from Cardamom Repository
 * This reference was created by the group_creator process
 */
CosClockService::PeriodicExecution::Executor*
Cdmw::clock::svcs::FTClockServiceProcessBehaviour::getExecutorGroup() 
{

    // Get the Cdmw repository
    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
    
    // Get NamingInterface to object_groups (for reading)
    Cdmw::NamingAndRepository::NamingInterface objGroupsInterface =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(REPOSITORY_CLOCK_GROUP_NAME.c_str());

    // Get Executor Object Group
    std::cout << "       ---- Server " << m_process_name << ": Get Executor Object Group " << std::endl;
    CosClockService::PeriodicExecution::Executor_var executorGroup =
        CosClockService::PeriodicExecution::Executor::_nil();
    try {
        CORBA::Object_var obj = objGroupsInterface.resolve(CLOCK_GROUP_NAME);   
        executorGroup =
            CosClockService::PeriodicExecution::Executor::_narrow(obj.in());
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << CLOCK_GROUP_NAME << " => NamingContext::NotFound raised!" << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << CLOCK_GROUP_NAME << " => NamingContext::CannotProceed raised!" << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << CLOCK_GROUP_NAME << " => NamingContext::InvalidName raised!" << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << CLOCK_GROUP_NAME << " => CORBA::SystemException raised!" << std::endl;
    }

    return executorGroup._retn();     
    
}    

    


