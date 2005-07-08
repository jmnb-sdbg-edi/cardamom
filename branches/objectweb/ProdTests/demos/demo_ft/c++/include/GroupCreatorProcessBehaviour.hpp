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


#ifndef INCL_DEMO_FT_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP 
#define INCL_DEMO_FT_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP 

// Standard Files 
#include <string>

// Cdmw Files
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <FaultTolerance/ftcommon/FTLocation.hpp>
#include <FaultTolerance/ftcommon/FactoryInfos.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

#include "FaultTolerance/idllib/FT.stub.hpp"
#include "TestHello.stub.hpp"


class GroupCreatorProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    // Constructor
    GroupCreatorProcessBehaviour(CORBA::ORB_ptr orb)
    {
        m_orb = CORBA::ORB::_duplicate( orb );
    }

    // Destructor
    virtual ~GroupCreatorProcessBehaviour() throw ()
    {
    }

    
    virtual CORBA::ULong nb_steps()
    throw( CORBA::SystemException )
    {
        return 1;
    }

    
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CORBA::SystemException )
    {
        std::cout << "   -------- GroupCreator on_initialise --------" << std::endl;

        // Get the Replication Manager
        CORBA::Object_var replication_mgr_obj
            = m_orb->resolve_initial_references("ReplicationManager");

        m_replication_mgr = FT::ReplicationManager::_narrow(
            replication_mgr_obj.in() );

        // Create the object group properties
        std::cout << "     GroupCreator: Create group" << std::endl;
        const char* rep_id_HelloInterface
            = "IDL:thalesgroup.com/DemoFT/HelloInterface:1.0";

	std::string hostLocation = Cdmw::OsSupport::OS::get_hostname();

        ::FT::Properties prop;
            prop.length( 4 );
            prop[0].nam.length( 1 );
            prop[0].nam[0].id = "org.omg.ft.MinimumNumberReplicas";
            prop[0].val <<= (CORBA::UShort) 2;
    
            prop[1].nam.length( 1 );
            prop[1].nam[0].id = "org.omg.ft.ReplicationStyle";
            prop[1].val <<= (CORBA::UShort) 2;
    
            prop[2].nam.length( 1 );
            prop[2].nam[0].id = "org.omg.ft.MembershipStyle";
            prop[2].val <<= (CORBA::UShort) 0;
    
 	    prop[3].nam.length( 1 );
	    prop[3].nam[0].id = "org.omg.ft.Factories";
	    Cdmw::FT::FactoryInfos factoryInfos;
	    factoryInfos.add_ftLocation(hostLocation, "FTApplication", "hello1");
	    factoryInfos.add_ftLocation(::getenv("HOSTNAME2"), "FTApplication", "hello2");
	    prop[3].val <<= factoryInfos.get_factoryInfos();
    
	::FT::Criteria ft_criteria;
	ft_criteria.length(1);
	ft_criteria[0].nam.length(1);
	ft_criteria[0].nam[0].id="org.omg.ft.FTProperties";
	ft_criteria[0].val <<= prop;
	::FT::GenericFactory::FactoryCreationId_var factory_creation_id;
	::FT::ObjectGroup_var obj_group;
    
        // create the object group 
        try
        {
            obj_group =  m_replication_mgr->create_object(
                rep_id_HelloInterface, ft_criteria, factory_creation_id.out() );
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            throw;
        }

        m_group = DemoFT::HelloInterface::_narrow( obj_group.in() );

        // get Repository
        CdmwNamingAndRepository::Repository_var repository
            = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

        // get NameDomain to hello_servers (for writing)
        CdmwNamingAndRepository::NameDomain_var helloDomain =
            repository->resolve_name_domain ("demo_ft/hello_servers");
                
        // get NamingInterface to hello_servers (for reading)
        Cdmw::NamingAndRepository::NamingInterface helloNamingInterface =
            Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("demo_ft/hello_servers");

        
        // add hello1 to object group
        add_hello_member("hello1", hostLocation, helloNamingInterface, true);

        // add hello2 to object group
		  hostLocation = ::getenv("HOSTNAME2");
        add_hello_member("hello2", hostLocation, helloNamingInterface, false);

        // add hello3 to object group
		  //host_location = ::getenv("HOSTNAME3");
        //add_hello_member("hello3", host_location, helloNamingInterface, false);
        
        
        // register Hello ObjectGroup into repository
        std::cout << "     GroupCreator: register object group into repository to object group" << std::endl;
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId =
            helloDomain->new_name ("hello_group");
        helloDomain->register_object (regId.in(), m_group.in ());
        
        std::cout << "   -------- GroupCreator on_initialise END --------" << std::endl;
    }

    
    virtual void on_run()
    throw( CdmwPlatformMngt::Process::NotReadyToRun,
           CORBA::SystemException )
    {
        std::cout << "   -------- GroupCreator run --------" << std::endl;
        // nothing to do
    }

    
    virtual void on_stop()
    throw( CORBA::SystemException )
    {
        std::cout << "   -------- GroupCreator stop --------" << std::endl;
        // nothing to do : shutdown done in ProcessControl
    }

    
private:

    virtual void add_hello_member(std::string hello_name,
			                         std::string host_location,
                                  Cdmw::NamingAndRepository::NamingInterface& namingInterface,
                                  bool set_as_primary)
    {
        // Retrieve reference of member to add from NamingInterface
        DemoFT::HelloInterface_var hello;
        try
        {
            std::string full_name = hello_name + "_object";
            std::cout << "     GroupCreator: retrieve server " << hello_name << std::endl;
            CORBA::Object_var obj = namingInterface.resolve (full_name.c_str());

            // check if this is a HelloInterface
            hello =  DemoFT::HelloInterface::_narrow( obj.in() );
         }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "Resolve " << hello_name << " => NamingContext::NotFound raised!" << std::endl;
            return;
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "Resolve " << hello_name << " => NamingContext::CannotProceed raised!" << std::endl;
            return;
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "Resolve " << hello_name << " => NamingContext::InvalidName raised!" << std::endl;
            return;
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "Resolve " << hello_name << " => CORBA::SystemException raised!" << std::endl;
            return;
        }
        
        // create Location for this member
        Cdmw::FT::FTLocation ftLoc(host_location.c_str(), "FTApplication", hello_name.c_str());
        
        // add this reference as a new member of the object group
        ::FT::ObjectGroup_var obj_group;
        try {
            std::cout << "     GroupCreator: add " << hello_name << " to object group" << std::endl;
            obj_group = m_replication_mgr->add_member(m_group.in(), ftLoc.get_FT_Location(), hello.in());
        }
        catch (const ::FT::ObjectGroupNotFound&)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "add_member " << hello_name << " => FT::ObjectGroupNotFound raised!" << std::endl;
            return;
        }
        catch (const ::FT::MemberAlreadyPresent&)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "add_member " << hello_name << " => FT::MemberAlreadyPresent raised!" << std::endl;
            return;
        }
        catch (const ::FT::ObjectNotAdded&)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "add_member " << hello_name << " => FT::ObjectNotAdded raised!" << std::endl;
            return;
        }
                
        if (set_as_primary) {
            try {
                std::cout << "     GroupCreator: set " << hello_name << " as primary" << std::endl;
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
                
        // update group reference
        m_group = DemoFT::HelloInterface::_narrow(
            obj_group.in() );
    }
    

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    /**
     * The object group.
     */
    DemoFT::HelloInterface_var m_group;

    /**
     * The replication manager.
     */
    FT::ReplicationManager_var m_replication_mgr;

};


#endif // INCL_DEMO_FT_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP

