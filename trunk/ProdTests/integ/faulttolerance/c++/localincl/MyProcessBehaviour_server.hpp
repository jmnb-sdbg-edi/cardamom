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

#ifndef INCL_MY_PROCESS_BEHAVIOUR_HPP 
#define INCL_MY_PROCESS_BEHAVIOUR_HPP 

#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <Foundation/commonsvcs/naming/NamingUtil.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include <FaultTolerance/idllib/CdmwFTActivation.skel.hpp>
#include <faulttolerance/TestHello_impl.hpp>

#include <FaultTolerance/idllib/FT.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>
#include <Repository//idllib/CdmwNamingAndRepository.stub.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

class HelloActivationHandler
: virtual public CdmwFT::Location::ActivationHandler,
  virtual public Cdmw::OrbSupport::RefCountLocalObject
{

public:

    HelloActivationHandler(
        const std::string& name,
        PortableServer::ServantBase*  hello)
    : m_name( name )
    { 
        CDMW_ASSERT(hello);
        m_hello = dynamic_cast<Cdmw::Test::HelloInterface_impl*> (hello);
    };
    
    virtual void activate()
    throw( CORBA::SystemException )
    {
        std::cout
            << m_name
            <<" <####### HELLO ACTIVATION ######>"
            << std::endl;

        std::ifstream is("hello.stt");
        if (is.good())
        {
            unsigned long nb_hello;
            is >> nb_hello;
            
            m_hello->set_nb_hello(nb_hello);
                
            is.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: /tmp/hello.stt" << std::endl;
            throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        }
    };
    
    
    virtual void deactivate()
    throw( CORBA::SystemException )
    {
        std::cout
            << m_name
            <<" <####### HELLO DEACTIVATION ######>"
            << std::endl;
    };

private:

    /**
    * The application name.
    */
    std::string m_name;

    // hello impl  
    Cdmw::Test::HelloInterface_impl*  m_hello;
};



/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class MyProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    /**
     * Purpose:
     * <p> The constructor.
     */
    MyProcessBehaviour(
        CORBA::ORB_ptr orb )
    {
        m_orb = CORBA::ORB::_duplicate( orb );
        m_nb_init_step = 2;
        m_current_step = 0;
    }

    /**
     * Purpose:
     * <p> The destructor.
     */
    virtual ~MyProcessBehaviour() throw ()
    {
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

        // Get the group repository
        try
        {
            CORBA::Object_var obj
                = m_orb->resolve_initial_references( "FTGroupRepository" );

            m_group_repository
                = CdmwFT::Location::PrimaryBackupGroupRepository::_narrow(
                    obj.in() );
        }
        catch( const CORBA::ORB::InvalidName& )
        {
            std::cerr
                << "ERROR: Impossible to resolve FTGroupRepository! "
                << "Name is invalid!"
                << std::endl;
        }

        CdmwNamingAndRepository::Repository_var repository
            = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
        
        // ======================================================
        // Get HELLO name domain from repository
        // ======================================================
        CdmwNamingAndRepository::NameDomain_var helloDomain =
            repository->resolve_name_domain ("dom1/dom2/dom3");
       
        // get process name
        std::string process_name 
            = Cdmw::PlatformMngt::PlatformInterface::Get_process_name();


        // Get this process' location
		  std::cout << "test_server: get this process' location " << std::endl;
        ::FT::Location_var the_location = m_group_repository->the_location();
        m_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(
            the_location.in() );

        // Create the object
        std::cout
            << m_name
            << " test_server: create HelloInterface_impl"
            << std::endl;

        Cdmw::Test::HelloInterface_impl* hello_interface
            = new Cdmw::Test::HelloInterface_impl( m_orb.in(), m_name );
        m_hello_servant = hello_interface;
        m_hello_obj = hello_interface->_this();
        
        //
        // Register the HelloInterface object in the HELLO name domain
        //
        // reserve the name "HelloInterface" in the domain
        m_full_name = process_name + "HelloInterface";
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId 
            = helloDomain->new_name (m_full_name.c_str());

        // register the object
        helloDomain->register_object (regId, m_hello_obj.in ());

    }

    void on_next_step()
    throw( CdmwPlatformMngt::ProcessDelegate::InvalidStep,
           CORBA::SystemException )
    {
        if( m_current_step < m_nb_init_step )
            ++m_current_step;
        else
            throw CdmwPlatformMngt::ProcessDelegate::InvalidStep();

        // Get the object group reference
        ::FT::ObjectGroup_var group_ref = ::FT::ObjectGroup::_nil();

        try 
        {
            std::cout
                << m_name
                << " test_server: get object group ref"
                <<std::endl;

            group_ref = m_group_repository->get_object_group(
                m_hello_obj.in() );
        }
        catch( const FT::MemberNotFound& )
        {
            std::cerr << "ERROR: Impossible to get the group! " << std::endl;
        }

        // Create activation handler and register it
        std::cout
            << m_name
            << " test_server: create activation handler and register it"
            <<std::endl;

        CdmwFT::Location::ActivationHandler_var activation_handler
            = new HelloActivationHandler( m_full_name, m_hello_servant.in() );

        try
        {
            CORBA::Object_var obj
                = m_orb->resolve_initial_references( "FTActivationManager" );
            CdmwFT::Location::ActivationManager_var activation_manager
                = CdmwFT::Location::ActivationManager::_narrow( obj.in() );

            activation_manager->register_activation_handler(
                    group_ref.in(), activation_handler.in() );
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

        std::cout<< m_name << " test_server: orb run"<<std::endl;
        ++m_current_step;
    }

    virtual void on_run()
    throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
           CORBA::SystemException )
    {
    }

    virtual void on_stop()
    throw( CORBA::SystemException )
    {    
        // nothing to do : shutdown done in ProcessControl
    }

private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    /**
     * The application name.
     */
    std::string m_name;

    /**
     * The full application name.
     */
    std::string m_full_name;

    /**
     * The number of init step.
     */
    unsigned int m_nb_init_step;

    /**
     * The current init step.
     */
    unsigned int m_current_step;

    /**
     * The group repository.
     */
    CdmwFT::Location::GroupRepository_var m_group_repository;

    /**
     * The object reference.
     */
    Cdmw::Test::HelloInterface_var m_hello_obj;
    PortableServer::ServantBase_var m_hello_servant;
};


#endif // INCL_MY_PROCESS_BEHAVIOUR_HPP

