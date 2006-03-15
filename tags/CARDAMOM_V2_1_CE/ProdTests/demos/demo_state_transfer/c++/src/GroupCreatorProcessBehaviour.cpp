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


// Demo Files
#include <GroupCreatorProcessBehaviour.hpp>
#include <StateTransferConfiguration.hpp>

// Cdmw Files
#include <Foundation/ossupport/OS.hpp>
#include <FaultTolerance/idllib/CdmwFTStateTransfer.stub.hpp>
#include <FaultTolerance/ftcommon/FactoryInfos.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include <iostream>




GroupCreatorProcessBehaviour::GroupCreatorProcessBehaviour(
    CORBA::ORB_ptr orb,
    bool point_to_point)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_point_to_point = point_to_point;
}

GroupCreatorProcessBehaviour::~GroupCreatorProcessBehaviour() throw ()
{
}

    
CORBA::ULong GroupCreatorProcessBehaviour::nb_steps()
    throw( CORBA::SystemException )
{
    return 1;
}

    
void GroupCreatorProcessBehaviour::on_initialise(
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CORBA::SystemException )
{
    std::cout << "   -------- GroupCreator on_initialise --------" << std::endl;

    // Get the Replication Manager
    std::cout << "       ---- GroupCreator: get Replication Manager" << std::endl;
    CORBA::Object_var replication_mgr_obj
        = m_orb->resolve_initial_references("ReplicationManager");

    m_replication_mgr = FT::ReplicationManager::_narrow(
        replication_mgr_obj.in() );

    std::string hostLocation = Cdmw::OsSupport::OS::get_hostname();

    // Create the object group properties
    ::FT::Properties prop;
    prop.length( 4 );
    prop[0].nam.length( 1 );
    prop[0].nam[0].id = "org.omg.ft.MinimumNumberReplicas";
    prop[0].val <<= (CORBA::UShort) MINIMUM_NUMBER_REPLICAS;
    
    prop[1].nam.length( 1 );
    prop[1].nam[0].id = "org.omg.ft.ReplicationStyle";
    prop[1].val <<= (CORBA::UShort) REPLICATION_STYLE;
    
    prop[2].nam.length( 1 );
    prop[2].nam[0].id = "org.omg.ft.MembershipStyle";
    prop[2].val <<= (CORBA::UShort) MEMBERSHIP_STYLE;
    
    prop[3].nam.length( 1 );
    prop[3].nam[0].id = "org.omg.ft.Factories";
    Cdmw::FT::FactoryInfos factoryInfos;
    factoryInfos.add_ftLocation(hostLocation, "FTApplication", "database1");
    factoryInfos.add_ftLocation(::getenv("HOSTNAME2"), "FTApplication", "database2");
    factoryInfos.add_ftLocation(::getenv("HOSTNAME3"), "FTApplication", "database3");
    prop[3].val <<= factoryInfos.get_factoryInfos();
    
    ::FT::Criteria ft_criteria;
    ft_criteria.length(1);
    ft_criteria[0].nam.length(1);
    ft_criteria[0].nam[0].id="org.omg.ft.FTProperties";
    ft_criteria[0].val <<= prop;

    if (m_point_to_point) {
        // For point-to-point State Transfer, the identifiers of 
        // the used DataStores have to be added to the criteria
        std::cout << "       ---- GroupCreator: add dsids into criteria" << std::endl;
        
        ::CdmwFT::StateTransfer::DataStoreIDs dsids;
        dsids.length(3);
        dsids[0] = DATA_STORE_1_IDENTIFIER;
        dsids[1] = DATA_STORE_2_IDENTIFIER;
        dsids[2] = DATA_STORE_3_IDENTIFIER;
        
        ft_criteria.length( ft_criteria.length()+1 );
        ft_criteria[ft_criteria.length()-1].nam.length( 1 );
        ft_criteria[ft_criteria.length()-1].nam[0].id = "com.thalesgroup.cdmw.ft.DataStoreIdentifiers";
        ft_criteria[ft_criteria.length()-1].val <<= dsids;
    
    }
    
    // create the object group 
    std::cout << "       ---- GroupCreator: create the object group" << std::endl;
    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id;
    try
    {
        m_obj_group =  m_replication_mgr->create_object(DATABASE_REPOSITORY_ID.c_str(), 
                                                        ft_criteria, 
                                                        factory_creation_id.out() );
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;
        throw;
    }

    // get Repository
    std::cout << "       ---- GroupCreator: get Repository" << std::endl;
    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

    // get NameDomain to object_groups (for writing)
    m_objGroupsDomain = repository->resolve_name_domain ("demo/object_groups");

    // register Database ObjectGroup into repository
    std::cout << "       ---- GroupCreator: register object group into repository" << std::endl;
    CdmwNamingAndRepository::NameDomain::RegistrationId_var regId =
        m_objGroupsDomain->new_name (DATABASE_GROUP_NAME.c_str());
    m_objGroupsDomain->register_object (regId.in(), m_obj_group.in ());

    std::cout << "   -------- GroupCreator on_initialise END --------" << std::endl;
}

    
void GroupCreatorProcessBehaviour::on_run()
    throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
           CORBA::SystemException )
{
}

    
void GroupCreatorProcessBehaviour::on_stop()
    throw( CORBA::SystemException )
{
    m_orb->shutdown(true);
}

    
