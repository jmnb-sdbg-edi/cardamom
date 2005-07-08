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


#ifndef INCL_DEMO_FT_SERVER_PROCESS_BEHAVIOUR_HPP 
#define INCL_DEMO_FT_SERVER_PROCESS_BEHAVIOUR_HPP 

#include <Foundation/common/Assert.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <Repository/naminginterface/NamingUtil.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>

#include <string>
#include <iostream>

#include "LoggingMechanism.hpp"
#include "RecoveryMechanism.hpp"
#include "HelloActivationHandler.hpp"
#include "HelloServantLocator.hpp"


/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class ServerProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    ServerProcessBehaviour(
        CORBA::ORB_ptr orb, int argc, char *argv[] )
    {
        m_orb = CORBA::ORB::_duplicate( orb );
        m_logging_mechanism = NULL;
        m_activation_handler = NULL;
        m_servant_locator = NULL;
        m_hello = DemoFT::HelloInterface::_nil();
        m_nb_init_step = 2;
        m_current_step = 0;
		  m_argc = argc;
		  m_argv = argv;
    }

    
    virtual ~ServerProcessBehaviour() throw ()
    {
        if (m_logging_mechanism != NULL)
            delete m_logging_mechanism;
        if (m_servant_locator != NULL)
            m_servant_locator->_remove_ref();
        if (m_activation_handler != NULL)
            m_activation_handler->_remove_ref();
        if (m_hello_impl != NULL)
            m_hello_impl->_remove_ref();
    }

    
    virtual CORBA::ULong nb_steps()
    throw( CORBA::SystemException )
    {
        return m_nb_init_step;
    }

    
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CORBA::SystemException )
    {
        ++m_current_step;

        // Get process name
        m_process_name = Cdmw::PlatformMngt::PlatformInterface::getProcessName();
        std::cout << "   -------- Server " << m_process_name << " on_initialise --------" << std::endl;

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

        
        // Get the Cdmw repository
        CdmwNamingAndRepository::Repository_var repository
            = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
        
        // Get HELLO name domain from repository
        CdmwNamingAndRepository::NameDomain_var helloDomain;
        try {
            helloDomain = repository->resolve_name_domain ("demo_ft/hello_servers");
        } catch (const CdmwNamingAndRepository::NoNameDomain&) {
            std::cerr << "Can't find NameDomain demo_ft/hello_servers" << std::endl;
            CDMW_ASSERT(false);
        } catch (const CdmwNamingAndRepository::InvalidName&) {
            std::cerr << "NameDomain demo_ft/hello_servers is invalid !!" << std::endl;
            CDMW_ASSERT(false);
        }


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
        PortableServer::POA_var helloPOA = 
            Cdmw::OrbSupport::OrbSupport::create_POA(rootPOA.in(),
                                                      "helloPOA",
                                                      poaMgr.in(),
                                                      policies,
                                                      strategyList);
        
        // create HelloInterface servant (using helloPOA)
        m_hello_impl = new HelloInterface_impl(m_process_name, helloPOA.in());
        
        std::cout << "       ---- Server " << m_process_name << ": create Logging & Recovery Mechanisms " << std::endl;

        // create LoggingMechanism
		  // workaround of a splice dcps bug
		  char** argv_tmp = NULL;
        int argc_tmp = m_argc;
		  
		  argv_tmp = new char*[argc_tmp+1];
		  argv_tmp[0] = new char[7];
		  strncpy(argv_tmp[0], "server", 6);
		  
		  for (int i=1; i<argc_tmp ; i++)
		  {
			  argv_tmp[i] = m_argv[i];
		  }
		  argv_tmp[argc_tmp] = NULL;

        m_logging_mechanism = new LoggingMechanism(argc_tmp, argv_tmp);
        
        // Create activation handler which is also the RecoveryMechanism
        std::cout << "       ---- Server " << m_process_name << ": create activation handler" << std::endl;
        m_activation_handler = new HelloActivationHandler(argc_tmp,
				                                              argv_tmp,
																			 m_process_name, 
                                                          m_hello_impl);

        // create ServantLocator
        std::cout << "       ---- Server " << m_process_name << ": create ServantLocator " << std::endl;
        std::string hello_oid = m_process_name;
        m_servant_locator = new HelloServantLocator(hello_oid,
                                                    m_logging_mechanism,
                                                    m_activation_handler);

        // set ServantLocator to helloPOA
        try {
            helloPOA->set_servant_manager(m_servant_locator);
        } catch (const PortableServer::POA::WrongPolicy& e) {
            // shouldn't happen
            CDMW_ASSERT(false);
        }
        
        // activate POAManager
        std::cout << "       ---- Server " << m_process_name << ": activate POAManager " << std::endl;
        poaMgr->activate();
        
        // create HelloInterface object under helloPOA
        std::cout << "       ---- Server " << m_process_name << ": create Hello reference " << std::endl;
        PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(hello_oid.c_str());
        CORBA::Object_var hello_obj = 
            helloPOA->create_reference_with_id(oid, "IDL:thalesgroup.com/DemoFT/HelloInterface:1.0");
        m_hello = DemoFT::HelloInterface::_narrow(hello_obj.in());
        
        
        // Register the HelloInterface object in the "hello_servers" name domain
        std::string registration_name = m_process_name + "_object";
        try {
            // reserve the name "HelloInterface" in the domain
            CdmwNamingAndRepository::NameDomain::RegistrationId_var regId 
                = helloDomain->new_name (registration_name.c_str());
    
            // register the object
            std::cout << "       ---- Server " << m_process_name 
                      << ": register Hello reference as " << registration_name << std::endl;
            helloDomain->register_object (regId.in(), m_hello.in ());

        } catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists&) {
            std::cerr << "Name " << registration_name << " already exist into NameDomain demo_ft/hello_servers" << std::endl;
            CDMW_ASSERT(false);
        } catch (const CdmwNamingAndRepository::InvalidName&) {
            std::cerr << "Name " << registration_name << " is an invalid name !!" << std::endl;
            CDMW_ASSERT(false);
        } catch (const CdmwNamingAndRepository::NameDomain::InvalidRegistration&) {
            std::cerr << "Invalid Registration of Hello reference as " << registration_name << std::endl;
            CDMW_ASSERT(false);
        }
        
        std::cout << "   -------- Server " << m_process_name << " on_initialise END --------" << std::endl;
    }

    
    void on_next_step()
    throw( CdmwPlatformMngt::Process::InvalidStep,
           CORBA::SystemException )
    {
        std::cout << "   -------- Server " << m_process_name << " on_next_step --------" << std::endl;

        if( m_current_step < m_nb_init_step )
            ++m_current_step;
        else
            throw CdmwPlatformMngt::Process::InvalidStep();

        // Get the object group reference
        ::FT::ObjectGroup_var group_ref = ::FT::ObjectGroup::_nil();
        try 
        {
            group_ref = m_group_repository->get_object_group(
                m_hello.in() );
        }
        catch( const FT::MemberNotFound& )
        {
            std::cerr << "ERROR: Impossible to get the group! " << std::endl;
            // shouldn't happen
            CDMW_ASSERT(false);
        }
        
        // Register activation handler
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
                    group_ref.in(), m_activation_handler );
        }
        catch( const FT::ObjectGroupNotFound& )
        {
            std::cerr
                << "ERROR: Impossible to register FTActivationHandler! "
                << "Object Group not found!"
                << std::endl;
        }
        catch( const CORBA::ORB::InvalidName& )
        {
            std::cerr
                << "ERROR: Impossible to register FTActivationManager! "
                << "Name is invalid!"
                << std::endl;
        }
        catch( const CORBA::SystemException& e )
        {
            std::cerr
                << "ERROR: Impossible to register FTActivationHandler!"
                << std::endl;
            std::cerr << e << std::endl;
        }

        ++m_current_step;

        std::cout << "   -------- Server " << m_process_name << " on_next_step END --------" << std::endl;
    }

    virtual void on_run()
    throw( CdmwPlatformMngt::Process::NotReadyToRun,
           CORBA::SystemException )
    {
        std::cout << "   -------- Server " << m_process_name << " running --------" << std::endl;
    }

    virtual void on_stop()
    throw( CORBA::SystemException )
    {    
        std::cout << "   -------- Server " << m_process_name << " stopping --------" << std::endl;
        // nothing to do : shutdown done in ProcessControl 
    }

private:

    // The ORB.
    CORBA::ORB_var m_orb;

    // The process name.
    std::string m_process_name;
    
    // The process location
    std::string m_location;

    // The number of init step.
    unsigned int m_nb_init_step;

    // The current init step.
    unsigned int m_current_step;

    // The group repository.
    CdmwFT::Location::GroupRepository_var m_group_repository;

    // The Logging Mechanism
    LoggingMechanism*  m_logging_mechanism;
    
    // The Recovery Mechanism
    // The Activation Handler which is the Recovery Mechanism
    HelloActivationHandler* m_activation_handler;
    
    // The Servant Locator
    HelloServantLocator* m_servant_locator;
    
    // The HelloInterface object reference
    DemoFT::HelloInterface_var m_hello;
    
    // The HelloInterface servant
    HelloInterface_impl* m_hello_impl;

	 int m_argc;
	 char **m_argv;
    
};


#endif // INCL_DEMO_FT_SERVER_PROCESS_BEHAVIOUR_HPP


