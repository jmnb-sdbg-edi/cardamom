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
 
#ifndef INCL_CDMW_FT_GROUP_MANAGER_H_
#define INCL_CDMW_FT_GROUP_MANAGER_H_
/**
 * @brief A Fault Tolerant Group Manager implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "Foundation/osthreads/Mutex.hpp"

#include <memory>
#include "FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp"
#include "LoadBalancing/lbgroupmanagerft/LBGroupManagerActivationHandler.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp"
#include <LoadBalancing/lbgroupmanager/LBGroupManager_impl.hpp>


namespace Cdmw
{

namespace LB {

  class FTGroupManager_impl : public virtual Cdmw::LB::LBGroupManager_impl
    {
    public:
      FTGroupManager_impl (CORBA::ORB_ptr orb, 
			   PortableServer::POA_ptr poa,
			   const char*,
			   CdmwPlatformMngt::System_ptr system,
			   std::string observer_name)
	    throw(CORBA::SystemException);
        
      virtual ~FTGroupManager_impl();
	/**
	 * This method is called by the framework to activate the clock.
	 */ 
	virtual void activate();
	friend class ::Cdmw::LB::LBGroupManagerActivationHandler;
        
    private:
	CORBA::ORB_var m_orb;
	
	CdmwPlatformMngt::System_var m_system;

	std::string m_observer_name;
    };
    
} /* LB*/
    
} /* Cdmw */

#endif /* INCL_CDMW_FT_GROUP_MANAGER_H_ */
