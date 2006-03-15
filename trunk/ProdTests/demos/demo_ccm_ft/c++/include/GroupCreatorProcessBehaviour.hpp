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
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include <FaultTolerance/ftcommon/FactoryInfos.hpp>
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include "FaultTolerance/idllib/FT.stub.hpp"


class GroupCreatorProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    // Constructor
    GroupCreatorProcessBehaviour(CORBA::ORB_ptr orb)
		throw (CORBA::SystemException)
		: m_orb(CORBA::ORB::_duplicate( orb ))
    {
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

    /**
     * This callback operation is called whenever the CDMW supervision invokes
     * the initialise operation on the process behaviour object.
     */
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
        throw( CORBA::SystemException )
    {
        // This operation creates object group references for components's external
        // interfaces using the CDMW Replicatoion Manager.
        std::cout << "   -------- GroupCreator on_initialise --------" << std::endl;
        
        // Get the Replication Manager
        CORBA::Object_var replication_mgr_obj
            = m_orb->resolve_initial_references("ReplicationManager");
        
        m_replication_mgr = FT::ReplicationManager::_narrow(
            replication_mgr_obj.in() );
        
        // I) Create the object group Fault Tolerance properties
        //    NOTA: All component's ports shall share the same Fault Tolerance
        //          properties.
        std::cout << "     GroupCreator: Create group" << std::endl;
        const char* rep_id_Server  = "IDL:acme.com/Hello/Server:1.0";
        // NOTA: Actual facets in CDMW containers are in Cdmw_Hello IDL module
        const char* rep_id_Display = "IDL:acme.com/Hello/Display:1.0";

	std::string hostLocation = Cdmw::OsSupport::OS::get_hostname();

        ::FT::Properties prop;
        prop.length( 4 );
        prop[0].nam.length( 1 );
        prop[0].nam[0].id = "org.omg.ft.MinimumNumberReplicas";
        prop[0].val <<= (CORBA::UShort) 3;
        
        prop[1].nam.length( 1 );
        prop[1].nam[0].id = "org.omg.ft.ReplicationStyle";
        prop[1].val <<= (CORBA::UShort) 2; //::FT::WARM_PASSIVE; //(CORBA::UShort) 2;
        
        prop[2].nam.length( 1 );
        prop[2].nam[0].id = "org.omg.ft.MembershipStyle";
        prop[2].val <<= (CORBA::UShort) 0; //::FT::MEMB_APP_CTRL; // (CORBA::UShort) 0;
        
	prop[3].nam.length( 1 );
	prop[3].nam[0].id = "org.omg.ft.Factories";
	Cdmw::FT::FactoryInfos factoryInfos;
	factoryInfos.add_ftLocation(hostLocation, "FTApplication", "ServerProcess1");
	factoryInfos.add_ftLocation(hostLocation, "FTApplication", "ServerProcess2");
	factoryInfos.add_ftLocation(hostLocation, "FTApplication", "ServerProcess3");
	prop[3].val <<= factoryInfos.get_factoryInfos();
    
        // 2) Create both the component IOGR and the facet's IOGR.
        
        ::FT::Criteria ft_criteria;
	ft_criteria.length(1);
	ft_criteria[0].nam.length(1);
	ft_criteria[0].nam[0].id="org.omg.ft.FTProperties";
	ft_criteria[0].val <<= prop;

        ::FT::GenericFactory::FactoryCreationId_var component_creation_id;
        ::FT::ObjectGroup_var component_iogr;
        
        // create the component's IOGR
        try	{
            component_iogr =  m_replication_mgr->create_object(rep_id_Server, 
                                                               ft_criteria, 
                                                               component_creation_id.out() );
        } catch( CORBA::SystemException& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw;
        } catch ( CORBA::Exception& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw CORBA::INTERNAL();
        } 
        
        ::FT::GenericFactory::FactoryCreationId_var facet_creation_id;
        ::FT::ObjectGroup_var facet_iogr;
        
        // create the facet's IOGR
        try  {
            facet_iogr =  m_replication_mgr->create_object(rep_id_Display, 
                                                           ft_criteria, 
                                                           facet_creation_id.out() );
        } catch( CORBA::SystemException& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw;
        } catch ( CORBA::Exception& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw CORBA::INTERNAL();
        } 

        // 3) Register the two IOGRs in the CDMW naming and repository service
        // get Repository
        CdmwNamingAndRepository::Repository_var repository
            = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
        
        try {
            // get NameDomain to hello_servers (for writing)
            CdmwNamingAndRepository::NameDomain_var helloDomain =
                repository->resolve_name_domain ("demo_ccm_ft/hello_servers");
            
            helloDomain->register_new_object("hello_server.group", component_iogr.in());
            helloDomain->register_new_object("hello_server_facet.group", facet_iogr.in());
        } catch( CORBA::SystemException& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw;
        } catch ( CORBA::Exception& e ) {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
            throw CORBA::INTERNAL();
        } 
        
        std::cout << "   -------- GroupCreator on_initialise END --------" << std::endl;
    }
    
    
    virtual void on_run()
    throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
           CORBA::SystemException )
    {
        std::cout << "   -------- GroupCreator run --------" << std::endl;
        // nothing to do
    }

    
    virtual void on_stop()
    throw( CORBA::SystemException )
    {
        std::cout << "   -------- GroupCreator stop --------" << std::endl;
    }

    
private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    /**
     * The Server component IOGR.
     */
    //    Hello::Server_var m_component_group;
    
    /**
     * The Server component's facet IOGR.
     * NOTA: Actual facets in CDMW containers are in Cdmw_Hello IDL module
     */
    //    Cdmw_Hello::Display_var m_facet_group;

    /**
     * The replication manager.
     */
    FT::ReplicationManager_var m_replication_mgr;

};


#endif // INCL_DEMO_FT_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP


