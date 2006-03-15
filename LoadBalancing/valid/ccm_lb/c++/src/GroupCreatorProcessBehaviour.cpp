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


#include <fstream>
#include <string>

#include <ccm_lb/GroupCreatorProcessBehaviour.hpp>
#include "LoadBalancing/lbcommon/StateTransferConfig.hpp"
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"


static const char* SERVERWITHFACET_GROUP_IORFILE  = "ServerWithFacet.ior";


void
GroupCreatorProcessBehaviour::on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
{
    // This operation creates object group references for the external
    // interfaces of the component using the CDMW LB Group Manager.
    std::cout << "   -------- GroupCreator on_initialise --------"
              << std::endl;
    CORBA::Object_var lbgroup_mgr_obj = CORBA::Object::_nil();
    try{
	// Get the LB Group Manager
	lbgroup_mgr_obj
	    = m_orb->resolve_initial_references("LBGroupManager");
	m_lbgroup_mgr = CdmwLB::LBGroupManager::_narrow(lbgroup_mgr_obj.in());
    }catch(const CORBA::SystemException& e)
    {}
    if (CORBA::is_nil(m_lbgroup_mgr.in()))
    {
	// Get the Cdmw repository
	CdmwNamingAndRepository::Repository_var repository
	    = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
    
	// Get NamingInterface to object_groups (for reading)
	Cdmw::CommonSvcs::Naming::NamingInterface objGroupsInterface =
	    Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(REPOSITORY_GROUP_MANAGER_GROUP_NAME.c_str());
	lbgroup_mgr_obj = objGroupsInterface.resolve(GROUP_MANAGER_OBJECT_GROUP_NAME);  
    }

    m_lbgroup_mgr = CdmwLB::LBGroupManager::_narrow(lbgroup_mgr_obj.in());

    std::cout << "      -> create group for ServerWithFacet" << std::endl;

    ::PortableGroup::ObjectGroup_var serverWithFacetGroup =
        create_object_group("IDL:thalesgroup.com/Cdmw_CcmLbTest/ServerWithFacet:1.0");

    std::cout << "      -> save its IOR in " << SERVERWITHFACET_GROUP_IORFILE
              << std::endl;

    write_ior_file(serverWithFacetGroup.in(), SERVERWITHFACET_GROUP_IORFILE);

    std::cout << "   -------- GroupCreator on_initialise END --------"
              << std::endl;
}




::PortableGroup::ObjectGroup_ptr
GroupCreatorProcessBehaviour::create_object_group(const char* repositoryId)
    throw(CORBA::SystemException)
{
    std::string lb_policy = "default";

    ::PortableGroup::Properties prop;
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    prop[0].val <<=  lb_policy.c_str();

    ::PortableGroup::Criteria crit;
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;

    // create the component's IOGR
    try {
        ::PortableGroup::GenericFactory::FactoryCreationId_var component_creation_id;

        return m_lbgroup_mgr->create_object(repositoryId,
                                            crit,
                                            component_creation_id.out());
    } catch (CORBA::SystemException& e) {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
        throw;
    } catch (CORBA::Exception& e) {
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
