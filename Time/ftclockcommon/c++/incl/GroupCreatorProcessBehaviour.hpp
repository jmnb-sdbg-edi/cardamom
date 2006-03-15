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


#ifndef INCL_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP 
#define INCL_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP 


// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>

#include <map>

namespace Cdmw {
    namespace clock {
        namespace svcs {
            class GroupCreatorProcessBehaviour;
        }
    }
}

class Cdmw::clock::svcs::GroupCreatorProcessBehaviour
    : public Cdmw::CdmwInit::ProcessControl
{

public:

    // Constructor
    GroupCreatorProcessBehaviour(CORBA::ORB_ptr orb, 
                                 bool point_to_point, 
                                 const std::string& h1, 
                                 const std::string& h2, 
                                 const std::string& h3);

    // Destructor
    virtual ~GroupCreatorProcessBehaviour() throw ();

    
    virtual CORBA::ULong nb_steps()
        throw( CORBA::SystemException );
    
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
        throw( CORBA::SystemException );
    
    virtual void on_run()
        throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
               CORBA::SystemException );
    
    virtual void on_stop()
        throw( CORBA::SystemException );

	void  link(const std::string& process,
	           const std::string& host);        
        
private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    /**
     * The replication manager.
     */
    ::FT::ReplicationManager_var m_replication_mgr;

    /**
     * The object group.
     */
    ::FT::ObjectGroup_var m_obj_group;

    /**
     * The object_groups NameDomain.
     */
    CdmwNamingAndRepository::NameDomain_var m_objGroupsDomain;

	/**
	 * Association between processes and hosts.
	 */
    std::multimap<std::string,std::string>	m_db;
	
	typedef	std::multimap<std::string,std::string>::iterator	iterator;
	
	typedef	std::multimap<std::string,std::string>::const_iterator	const_iterator;
	
	std::string	m_application_name;
    bool m_point_to_point;
    std::string m_host1;
    std::string m_host2;
    std::string m_host3;
};

#endif // INCL_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP

