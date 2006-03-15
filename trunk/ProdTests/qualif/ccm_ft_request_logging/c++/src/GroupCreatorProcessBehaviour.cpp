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

#include <fstream>
#include <ccm_ft_request_logging/GroupCreatorProcessBehaviour.hpp>
#include <FaultTolerance/ftcommon/FactoryInfos.hpp>


static const char* APPLICATION_NAME = "CcmFtApplication";
static const char* SERVER1_NAME = "ServerProcess1";
static const char* SERVER2_NAME = "ServerProcess2";
static const char* SERVER3_NAME = "ServerProcess3";

static const char* SERVERWITHFACET_GROUP_IORFILE  = "ServerWithFacet.ior";


void 
GroupCreatorProcessBehaviour::on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CORBA::SystemException )
{
    // This operation creates object group references for components's external
    // interfaces using the CDMW Replication Manager.
    std::cout << "   -------- GroupCreator on_initialise --------" << std::endl;
    
    // Get the Replication Manager
    CORBA::Object_var replication_mgr_obj
        = m_orb->resolve_initial_references("ReplicationManager");
    
    m_replication_mgr = FT::ReplicationManager::_narrow(
        replication_mgr_obj.in() );
    
    std::cout << "      -> create group for ServerWithFacet" << std::endl;
    ::FT::ObjectGroup_var serverWithFacetGroup = 
        create_object_group("IDL:thalesgroup.com/Cdmw_CcmFtTest/ServerWithFacet:1.0", 3);
    std::cout << "      -> write it's IOR in " << SERVERWITHFACET_GROUP_IORFILE << std::endl;
    write_ior_file(serverWithFacetGroup.in(), SERVERWITHFACET_GROUP_IORFILE);
    
    std::cout << "   -------- GroupCreator on_initialise END --------" << std::endl;
}




::FT::ObjectGroup_ptr
GroupCreatorProcessBehaviour::create_object_group(
        const char* repositoryId,
        CORBA::UShort minumumNumberReplicas
    )
    throw( CORBA::SystemException )
{
    
    ::FT::Properties prop;
    prop.length( 4 );
    prop[0].nam.length( 1 );
    prop[0].nam[0].id = "org.omg.ft.MinimumNumberReplicas";
    prop[0].val <<= minumumNumberReplicas;
    
    prop[1].nam.length( 1 );
    prop[1].nam[0].id = "org.omg.ft.ReplicationStyle";
    prop[1].val <<= (CORBA::UShort) ::FT::WARM_PASSIVE; //(CORBA::UShort) 2;
    
    prop[2].nam.length( 1 );
    prop[2].nam[0].id = "org.omg.ft.MembershipStyle";
    prop[2].val <<= (CORBA::UShort) ::FT::MEMB_APP_CTRL; // (CORBA::UShort) 0;
    
    Cdmw::FT::FactoryInfos factoryInfos;
    factoryInfos.add_ftLocation(m_host1, APPLICATION_NAME, SERVER1_NAME);
    factoryInfos.add_ftLocation(m_host2, APPLICATION_NAME, SERVER2_NAME);
    factoryInfos.add_ftLocation(m_host3, APPLICATION_NAME, SERVER3_NAME);    
    
    prop[3].nam.length( 1 );
    prop[3].nam[0].id = "org.omg.ft.Factories";
    prop[3].val <<= factoryInfos.get_factoryInfos();
    
    ::FT::Criteria ft_criteria;       
    ft_criteria.length(1);
    ft_criteria[0].nam.length(1);
    ft_criteria[0].nam[0].id="org.omg.ft.FTProperties";
    ft_criteria[0].val <<= prop;
    
    // create the component's IOGR
    try	{
        ::FT::GenericFactory::FactoryCreationId_var component_creation_id;
        return m_replication_mgr->create_object(repositoryId, 
                                                ft_criteria, 
                                                component_creation_id.out() );
    } catch( CORBA::SystemException& e ) {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
        throw;
    } catch ( CORBA::Exception& e ) {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
        throw CORBA::INTERNAL();
    } 
    
}


void
GroupCreatorProcessBehaviour::write_ior_file(CORBA::Object_ptr obj,
                                             const char* filename)
{
    CORBA::String_var ior = m_orb->object_to_string(obj);
    
    std::ofstream file(filename);
    file << ior.in();
    file.close();
}





