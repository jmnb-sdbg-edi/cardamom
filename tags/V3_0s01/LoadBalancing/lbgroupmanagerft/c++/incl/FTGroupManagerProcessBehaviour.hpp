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
#ifndef _CARDAMOM_FT_GROUP_MANAGER_PROCESS_BEHAVIOUR_H_
#define _CARDAMOM_FT_GROUP_MANAGER_PROCESS_BEHAVIOUR_H_

/**
 * @file
 * @brief Process Behaviour for the FTGroupManager
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "Foundation/orbsupport/CORBA.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include "FaultTolerance/idllib/CdmwFTStateTransfer.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp"

#include "LoadBalancing/lbgroupmanagerft/LBGroupManagerServantLocator.hpp"
#include "LoadBalancing/lbgroupmanagerft/LBGroupManagerActivationHandler.hpp"

#include <LoadBalancing/lbgroupmanager/ConfigurationMngr.hpp>

#include "LoadBalancing/lbgroupmanagerft/FTGroupManager_impl.hpp"

#include "ace/Get_Opt.h"
#include "ace/Thread.h"
#include "ace/Thread_Manager.h"
#include "ace/TP_Reactor.h"
#include "ace/Get_Opt.h"
#include "ace/Task.h"
#include "ace/Timer_Wheel.h"

namespace Cdmw {
    namespace LB {
	class FTGroupManagerProcessBehaviour;
    }
}

class Cdmw::LB::FTGroupManagerProcessBehaviour :
    public Cdmw::CdmwInit::ProcessControl
{
public:
    FTGroupManagerProcessBehaviour(CORBA::ORB_ptr orb,
				   PortableServer::POA_ptr poa,
                                   bool primary,
				   CdmwPlatformMngt::System_ptr system,
				   ConfigurationMngr& conf_mngr)  
        throw();
    
    ~FTGroupManagerProcessBehaviour() throw();

public:
    virtual CORBA::ULong nb_steps()
        throw( CORBA::SystemException );
    
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
        throw( CORBA::SystemException );
    
    virtual void on_next_step()
        throw( CdmwPlatformMngt::ProcessDelegate::InvalidStep,
               CORBA::SystemException );
    
    virtual void on_run()
        throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
               CORBA::SystemException );
    
    virtual void on_stop()
        throw( CORBA::SystemException );
    
private:
    
    void add_group_manager_to_group()
	throw(::FT::ObjectGroupNotFound,
	      ::FT::MemberNotFound,
	      ::FT::PrimaryNotSet,
	      ::FT::BadReplicationStyle,
	      ::FT::MemberAlreadyPresent,
	      ::FT::ObjectNotAdded,
	      ::CORBA::ORB::InvalidName,
	      CORBA::SystemException);
    
    
    CdmwLB::LBGroupManager* getLBGroupManagerGroup(); 
private:
    
    CORBA::ORB_var m_orb;

    PortableServer::POA_var m_poa;
    
    CdmwLB::LBGroupManager_var m_group_manager_obj;
    Cdmw::LB::FTGroupManager_impl*  m_group_manager_impl;
    
    CdmwPlatformMngt::System_var m_system;
    
    // -- FT Related  Attributes

    std::string m_process_name;
    std::string m_location;

    CdmwFT::ReplicationManager_var m_replication_mgr;    
    
    CdmwFT::Location::GroupRepository_var m_group_repository;

    Cdmw::LB::LBGroupManagerActivationHandler*  m_activation_handler;
    Cdmw::LB::LBGroupManagerServantLocator*     m_servant_locator;

    bool m_set_has_primary;

    ConfigurationMngr m_conf_mngr;
};



#endif /* _CARDAMOM_FT_GROUP_MANAGER_PROCESS_BEHAVIOUR_H_ */


