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

#include "Time/ftclockcommon/GroupCreatorProcessBehaviour.hpp"
#include "Time/clockservice/StateTransferConfig.hpp"

//#include <FaultTolerance/idllib/CdmwFTStateTransfer.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>

#include <FaultTolerance/idllib/FT.stub.hpp>
#include <FaultTolerance/ftcommon/FactoryInfos.hpp>
#include <FaultTolerance/ftcommon/FTConfiguration.hpp>

#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include <iostream>

using namespace std;


Cdmw::clock::svcs::GroupCreatorProcessBehaviour::GroupCreatorProcessBehaviour(
    CORBA::ORB_ptr orb,
    bool point_to_point,
    const std::string &h1,
    const std::string &h2,
    const std::string &h3)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_point_to_point = point_to_point;
    m_host1 = h1;
    m_host2 = h2; 
    m_host3 = h3;
    m_application_name = "FTApplication";
}

Cdmw::clock::svcs::GroupCreatorProcessBehaviour::~GroupCreatorProcessBehaviour() throw ()
{
}

    
CORBA::ULong
Cdmw::clock::svcs::GroupCreatorProcessBehaviour::nb_steps()
    throw( CORBA::SystemException )
{
    return 1;
}

    
void
Cdmw::clock::svcs::GroupCreatorProcessBehaviour::on_initialise(
    const CdmwPlatformMngtBase::StartupKind& )
    throw( CORBA::SystemException )
{
    std::cout << "   -------- GroupCreator on_initialise --------" << std::endl;

    // Get the Replication Manager
    std::cout << "       ---- GroupCreator: get Replication Manager" << std::endl;
    CORBA::Object_var replication_mgr_obj
        = m_orb->resolve_initial_references("ReplicationManager");

    m_replication_mgr = CdmwFT::ReplicationManager::_narrow(
        replication_mgr_obj.in() );
    std::cout << "   -------- GroupCreator on_initialise --------" << std::endl;

 
    ::FT::Properties prop;

    prop.length(4);
    prop[0].nam.length(1);
    prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
    prop[0].val <<= (CORBA::UShort)2;
    
    prop[1].nam.length(1);
    prop[1].nam[0].id="org.omg.ft.ReplicationStyle";
    prop[1].val <<= (CORBA::UShort)2;
    
    prop[2].nam.length(1);
    prop[2].nam[0].id="org.omg.ft.MembershipStyle";
    prop[2].val <<= (CORBA::UShort)0;
 
    prop[3].nam.length(1);
    prop[3].nam[0].id = "org.omg.ft.Factories";
    Cdmw::FT::FactoryInfos factoryInfos1;
    
    std::cout << "m_host1 " << m_host1 << endl;
    std::cout << "m_host2 " << m_host2 << endl;
    std::cout << "m_host3 " << m_host3 << endl;
    
    GroupCreatorProcessBehaviour::const_iterator
    	cit = m_db.begin(),
    	end = m_db.end();
    
    for(; cit != end ; ++cit )
    {
    	const std::string& process = cit->first;
    	const std::string& host = cit->second;
    	if( process.empty() ||
    	    host.empty() )
		{
			std::cerr<<"process or host are empty!!!"<<std::cerr;
			continue;
		}
		/*
	    char c1[1000];
	    strcpy(c1,host.c_str());
    	factoryInfos1.add_ftLocation(host, 
    	                             m_application_name, 
                                     process);
    	*/
    	factoryInfos1.add_ftLocation(host.c_str(),
    	                             m_application_name.c_str(), 
                                     process.c_str());
    }    	
        
    prop[3].val <<= factoryInfos1.get_factoryInfos();

    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::FT::Criteria ft_criteria;
    ft_criteria.length(2);
    ft_criteria[0].nam.length(1);
    ft_criteria[0].nam[0].id="org.omg.ft.FTProperties";
    ft_criteria[0].val <<= prop;

    
    if (m_point_to_point) {
        // For point-to-point State Transfer, the identifiers of 
        // the used DataStores have to be added to the criteria
        std::cout << "       ---- GroupCreator: add dsids into criteria" << std::endl;
        
        ::CdmwFT::StateTransfer::DataStoreIDs dsids;
        dsids.length(1);
        dsids[0] = CONTROLLER_DATA_IDENTIFIER;
        
        ft_criteria[1].nam.length( 1 );
        ft_criteria[1].nam[0].id = "com.thalesgroup.cdmw.ft.DataStoreIdentifiers";
        ft_criteria[1].val <<= dsids;
    
    }
    
    // create the object group 
    std::cout << "       ---- GroupCreator: create the object group" << std::endl;
    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id;
    try
    {
        m_obj_group =  m_replication_mgr->create_object(EXECUTOR_REPOSITORY_ID.c_str(), 
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
    m_objGroupsDomain = repository->resolve_name_domain (REPOSITORY_CLOCK_GROUP_NAME.c_str());

    // register Executor ObjectGroup into repository
    std::cout << "       ---- GroupCreator: register object group into repository" << std::endl;
    CdmwNamingAndRepository::NameDomain::RegistrationId_var regId =
        m_objGroupsDomain->new_name (CLOCK_GROUP_NAME.c_str());
    m_objGroupsDomain->register_object (regId.in(), m_obj_group.in ());

    std::cout << "   -------- GroupCreator on_initialise END --------" << std::endl;
}

    
void
Cdmw::clock::svcs::GroupCreatorProcessBehaviour::on_run()
    throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
           CORBA::SystemException )
{
}

    
void
Cdmw::clock::svcs::GroupCreatorProcessBehaviour::on_stop()
    throw( CORBA::SystemException )
{
    m_orb->shutdown(true);
}


void  
Cdmw::clock::svcs::GroupCreatorProcessBehaviour::link(const std::string& process,
                                                      const std::string& host)
{
    m_db.insert(make_pair(process,host));	
}
    
