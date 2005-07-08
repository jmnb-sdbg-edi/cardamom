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

#ifndef INCL_DEMO_LB_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP 
#define INCL_DEMO_LB_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP 

// Standard Files 
#include <string>

// Cardamom Files
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

// TAO Specific Load balancing
#include <orbsvcs/orbsvcs/CosLoadBalancingC.h>
#include <orbsvcs/orbsvcs/PortableGroupC.h>
//#include "Cdmw_Hello_cif.stub.hpp"


class LBGroupCreatorProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    // Constructor
    LBGroupCreatorProcessBehaviour(CORBA::ORB_ptr orb)
        throw (CORBA::SystemException)
        : m_orb(CORBA::ORB::_duplicate( orb ))
    {
    }

    // Destructor
    virtual ~LBGroupCreatorProcessBehaviour() throw ()
    {
    }

    
    virtual CORBA::ULong nb_steps()
        throw( CORBA::SystemException )
    {
        return 1;
    }

    /**
     * This callback operation is called whenever the CARDAMOM supervision invokes
     * the initialise operation on the process behaviour object.
     */
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
        throw( CORBA::SystemException )
    {
        // This operation creates object group references for components's external
        // interfaces using the CARDAMOM Replicatoion Manager.
        std::cout << "   -------- LB GroupCreator on_initialise --------" << std::endl;

        // Get the LoadManeger reference
        CORBA::Object_var obj =
            m_orb->resolve_initial_references ("LoadManager");
        m_load_manager = CosLoadBalancing::LoadManager::_narrow (obj.in ());

        if (CORBA::is_nil (m_load_manager.in ())) {
            std::cerr << "Unable to get Load Manager Reference" << std::endl;
            throw CORBA::INTERNAL();
        }
        
        // I) Create the object group Load balancing properties
        //    NOTA: All component's ports shall share the same Load Balancing
        //    ptoperties if any.
        std::cout << "     LB GroupCreator: Create load balancing group" << std::endl;
        const char* rep_id_Server  = "IDL:acme.com/Hello/Server:1.0";
        // NOTA: Actual facets in CARDAMOM containers are in Cdmw_Hello IDL module
        const char* rep_id_Display = "IDL:acme.com/Hello/Display:1.0";

        PortableGroup::Criteria criteria (1);
        criteria.length (1);
        
        PortableGroup::Property &property = criteria[0];
        property.nam.length (1);
        
        property.nam[0].id =
            CORBA::string_dup ("omg.org.PortableGroup.MembershipStyle");
        
        PortableGroup::MembershipStyleValue msv =
            PortableGroup::MEMB_APP_CTRL;
        property.val <<= msv;
        
        // 2) Create both the component IOGR and the facet's IOGR.
        PortableGroup::GenericFactory::FactoryCreationId_var comp_fcid;
        CORBA::Object_var component_iogr;
        // create the component's IOGR
        try {
            component_iogr =
                m_load_manager->create_object (rep_id_Server,
                                               criteria,
                                               comp_fcid.out ());
        } catch( CORBA::SystemException& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw;
        } catch ( CORBA::Exception& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw CORBA::INTERNAL();
        }
        
        PortableGroup::GenericFactory::FactoryCreationId_var facet_fcid;
        CORBA::Object_var facet_iogr;
        
        // create the facet's IOGR
        try	{
            facet_iogr =
                m_load_manager->create_object (rep_id_Display,
                                               criteria,
                                               facet_fcid.out ());
        } catch( CORBA::SystemException& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw;
        } catch ( CORBA::Exception& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw CORBA::INTERNAL();
        } 

        // 3) Register the two IOGRs in the CARDAMOM naming and repository service
        // get Repository
        CdmwNamingAndRepository::Repository_var repository
            = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
        
        try {
            // get NameDomain to hello_servers (for writing)
            CdmwNamingAndRepository::NameDomain_var helloDomain =
                repository->resolve_name_domain ("demo_ccm_lb/hello_servers");
            
            helloDomain->register_new_object("hello_server.group", component_iogr.in());
            helloDomain->register_new_object("hello_server_facet.group", facet_iogr.in());
        } catch( CORBA::SystemException& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw;
        } catch ( CORBA::Exception& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw CORBA::INTERNAL();
        } 
        
        std::cout << "   -------- LB GroupCreator on_initialise END --------" << std::endl;
    }
    
    
    virtual void on_run()
    throw( CdmwPlatformMngt::Process::NotReadyToRun,
           CORBA::SystemException )
    {
        std::cout << "   -------- LB GroupCreator run --------" << std::endl;
        // nothing to do
    }

    
    virtual void on_stop()
    throw( CORBA::SystemException )
    {
        std::cout << "   -------- LB GroupCreator stop --------" << std::endl;
    }

    
private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    /**
     * TAO Load Manager
     */
    CosLoadBalancing::LoadManager_var m_load_manager;
    
};


#endif // INCL_DEMO_LB_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP
