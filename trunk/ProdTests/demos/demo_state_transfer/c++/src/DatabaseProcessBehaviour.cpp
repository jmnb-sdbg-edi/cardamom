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

 
// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <Repository/naminginterface/NamingUtil.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <FaultTolerance/ftcommon/FTLocation.hpp>

// Demo Files
#include <DatabaseProcessBehaviour.hpp>
#include <StateTransferConfiguration.hpp>



DatabaseProcessBehaviour::DatabaseProcessBehaviour(CORBA::ORB_ptr orb,
                                                   bool point_to_point,
                                                   bool set_has_primary)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_point_to_point = point_to_point;
    m_set_has_primary = set_has_primary;
    m_activation_handler = NULL;
    m_servant_locator = NULL;
    m_database_obj = Demo::Database::_nil();
    m_database_impl = NULL;
}

    
DatabaseProcessBehaviour::~DatabaseProcessBehaviour() 
    throw ()
{
    if (m_servant_locator != NULL)
        m_servant_locator->_remove_ref();
    if (m_activation_handler != NULL)
        m_activation_handler->_remove_ref();
    if (m_database_impl != NULL)
        m_database_impl->_remove_ref();
}

    
CORBA::ULong DatabaseProcessBehaviour::nb_steps()
    throw( CORBA::SystemException )
{
    return 1;
}

    
void DatabaseProcessBehaviour::on_initialise(
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CORBA::SystemException )
{
    // Get process name
    m_process_name = Cdmw::PlatformMngt::PlatformInterface::getProcessName();
    std::cout << "   -------- Server " << m_process_name << " on_initialise --------" << std::endl;

    // get root POA            
    CORBA::Object_var obj = m_orb->resolve_initial_references( "RootPOA" );
    PortableServer::POA_var rootPOA
        = PortableServer::POA::_narrow( obj.in() );
    PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
            
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
        
    // create Database servant (using myPOA)
    std::cout << "       ---- Server " << m_process_name << ": create Database servant" << std::endl;
    m_database_impl = new Database_impl(m_orb.in(), myPOA.in(), m_point_to_point);
        

    // create activation handler
    std::cout << "       ---- Server " << m_process_name << ": create activation handler" << std::endl;
    m_activation_handler = new DatabaseActivationHandler(m_process_name,
                                                         m_database_impl);

    // create ServantLocator
    std::cout << "       ---- Server " << m_process_name << ": create ServantLocator " << std::endl;
    std::string database_oid = m_process_name;
    m_servant_locator = new DatabaseServantLocator(database_oid,
                                                   m_activation_handler);

    // set ServantLocator to myPOA
    try {
        myPOA->set_servant_manager(m_servant_locator);
    } catch (const PortableServer::POA::WrongPolicy& e) {
        // shouldn't happen
        CDMW_ASSERT(false);
    }
        
    // create Database object under myPOA
    std::cout << "       ---- Server " << m_process_name << ": create Database reference " << std::endl;
    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(database_oid.c_str());
    CORBA::Object_var database_obj = 
        myPOA->create_reference_with_id(oid, "IDL:thalesgroup.com/Demo/Database:1.0");
    m_database_obj = Demo::Database::_narrow(database_obj.in());

    // add Database object as member of Database group
    add_database_to_group();        
        
    // activate POAManager
    std::cout << "       ---- Server " << m_process_name << ": activate POAManager " << std::endl;
    poaMgr->activate();
        
    std::cout << "   -------- Server " << m_process_name << " on_initialise END --------" << std::endl;
}

    
void DatabaseProcessBehaviour::on_next_step()
    throw( CdmwPlatformMngt::Process::InvalidStep,
           CORBA::SystemException )
{
}

void DatabaseProcessBehaviour::on_run()
    throw( CdmwPlatformMngt::Process::NotReadyToRun,
           CORBA::SystemException )
{
    std::cout << "   -------- Server " << m_process_name << " running --------" << std::endl;
}

void DatabaseProcessBehaviour::on_stop()
    throw( CORBA::SystemException )
{    
    std::cout << "   -------- Server " << m_process_name << " stopping --------" << std::endl;
    // Same pb than in PCR-0046 was done again..
    // m_orb->shutdown();
}

    

void DatabaseProcessBehaviour::add_database_to_group() 
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

    // get Database group reference
    Demo::Database_var databaseGroup = getDatabaseGroup();
    if (CORBA::is_nil(databaseGroup.in())) {
        return;
    }

    // Get the Replication Manager
    std::cout << "       ---- Server " << m_process_name << ": Get the Replication Manager " << std::endl;
    CORBA::Object_var replication_mgr_obj
        = m_orb->resolve_initial_references("ReplicationManager");

    m_replication_mgr = FT::ReplicationManager::_narrow(
        replication_mgr_obj.in() );

    // create Location for this member
    std::cout << "       ---- Server " << m_process_name << ": Create Location for this member " << std::endl;
    Cdmw::FT::FTLocation ftLoc(Cdmw::OsSupport::OS::get_hostname().c_str(), "FTApplication", m_process_name.c_str() );

    // add Database object as a new member of the Database object group
    ::FT::ObjectGroup_var obj_group;
    try {
        std::cout << "       ---- Server " << m_process_name << ": add " << m_process_name << " to object group" << std::endl;
        obj_group = m_replication_mgr->add_member(databaseGroup.in(), ftLoc.get_FT_Location(), m_database_obj.in());
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
                databaseGroup.in(), m_activation_handler );
    }
    catch( const FT::ObjectGroupNotFound& )
    {
        std::cerr << "ERROR: Impossible to register FTActivationHandler! "
                  << "Object Group not found!"
                  << std::endl;
    }
    catch( const CORBA::ORB::InvalidName& )
    {
        std::cerr << "ERROR: Impossible to register FTActivationManager! "
                  << "Name is invalid!"
                  << std::endl;
    }
    catch( const CORBA::SystemException& e )
    {
        std::cerr << "ERROR: Impossible to register FTActivationHandler!"
                  << std::endl;
        std::cerr << e << std::endl;
    }


    // Set Database as primary if required
    if (m_set_has_primary) {
        try {
            std::cout << "       ---- Server " << m_process_name << ": set " << m_process_name << " as primary" << std::endl;
            obj_group = m_replication_mgr->set_primary_member( obj_group.in(), ftLoc.get_FT_Location() );
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
 * Get Database group reference from Cardamom Repository
 * This reference was created by the group_creator process
 */
Demo::Database* DatabaseProcessBehaviour::getDatabaseGroup() 
{

    // Get the Cdmw repository
    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
        
    // Get NamingInterface to object_groups (for reading)
    Cdmw::NamingAndRepository::NamingInterface objGroupsInterface =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface (REPOSITORY_DATABASE_GROUP_NAME);

    // Get Database Object Group
    std::cout << "       ---- Server " << m_process_name << ": Get Database Object Group " << std::endl;
    Demo::Database_var databaseGroup = Demo::Database::_nil();
    try {
        CORBA::Object_var obj = objGroupsInterface.resolve(DATABASE_GROUP_NAME);   
        databaseGroup = Demo::Database::_narrow(obj.in());
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << DATABASE_GROUP_NAME << " => NamingContext::NotFound raised!" << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << DATABASE_GROUP_NAME << " => NamingContext::CannotProceed raised!" << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << DATABASE_GROUP_NAME << " => NamingContext::InvalidName raised!" << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << DATABASE_GROUP_NAME << " => CORBA::SystemException raised!" << std::endl;
    }

    return databaseGroup._retn();     
    
}    

    


