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

/**
 * @brief A Fault Tolerant Group Manager implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "LoadBalancing/lbgroupmanagerft/FTGroupManager_impl.hpp"
#include <iostream>

#include <sstream>

#define ECHO_HEADER() \
    "[CDMW FT Group Manager] (file: " << __FILE__ << ", line: " << __LINE__ << ")\n -->"


#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment:\
} while(0)

#endif

std::string FT_GROUP_MANAGER_NAME            ("LBGroupManager"    );

namespace Cdmw
{

namespace LB
{

    /**
     * Creates a group manager.
     *
     * @param lb_domain_id The load balancing domain id.
     */
    FTGroupManager_impl::FTGroupManager_impl(CORBA::ORB_ptr orb,
					    PortableServer::POA_ptr poa,
					    const char* lb_domain_id,					     
					    CdmwPlatformMngt::System_ptr system,
					    std::string observer_name)
        throw(CORBA::SystemException)
	:  Cdmw::LB::FTGroupManager_impl::LBGroupManager_impl(orb, poa, lb_domain_id),
	   m_orb(CORBA::ORB::_duplicate(orb))
    {
	m_system = CdmwPlatformMngt::System::_duplicate(system);
	m_observer_name = observer_name;
    }

    FTGroupManager_impl::~FTGroupManager_impl()
    {
    }

void
FTGroupManager_impl::activate()
{
    std::cout << "############################################################" << std::endl;
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>  Activating  FTGroupManager Replica" << std::endl;  
    CORBA::Object_var obj = m_orb->resolve_initial_references(FT_GROUP_MANAGER_NAME.c_str());
    CdmwLB::LBGroupManager_var group_manager_ref = CdmwLB::LBGroupManager::_narrow(obj.in());
       m_system->register_observer( m_observer_name.c_str(), group_manager_ref.in() );

    std::cout << ">>>>>>>>>>>>>>>>>>>>>>  Complete FTGroupManager_impl Initialization" << std::endl;
    std::cout << "############################################################" << std::endl;
}


}//close namespace LB

}// close nampespace Cdmw

