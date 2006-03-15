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


#ifndef INCL_DATABASE_PROCESS_BEHAVIOUR_HPP 
#define INCL_DATABASE_PROCESS_BEHAVIOUR_HPP 


// Demos Files
#include <DatabaseActivationHandler.hpp>
#include <DatabaseServantLocator.hpp>
#include <Database_impl.hpp>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>




/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class DatabaseProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    DatabaseProcessBehaviour(CORBA::ORB_ptr orb,
                             bool point_to_point,
                             bool set_has_primary);

    
    virtual ~DatabaseProcessBehaviour() throw ();
    
    virtual CORBA::ULong nb_steps()
        throw( CORBA::SystemException );
    
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
        throw( CORBA::SystemException );
    
    void on_next_step()
        throw( CdmwPlatformMngt::ProcessDelegate::InvalidStep,
               CORBA::SystemException );

    virtual void on_run()
        throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
               CORBA::SystemException );

    virtual void on_stop()
        throw( CORBA::SystemException );

private:

    void add_database_to_group(); 
    
    Demo::Database* getDatabaseGroup(); 

    
    // The ORB.
    CORBA::ORB_var m_orb;
    
    // true if the point-to-point State Transfer has to be used 
    // false if the multicast State Transfer has to be used 
    bool m_point_to_point;

    // if the Database object has to be set as primary
    bool m_set_has_primary;

    // The process name.
    std::string m_process_name;
    
    // The process location
    std::string m_location;

    // The group repository.
    CdmwFT::Location::GroupRepository_var m_group_repository;

    // The Replication Manager
    FT::ReplicationManager_var m_replication_mgr;
    
    // The Activation Handler
    DatabaseActivationHandler* m_activation_handler;
    
    // The Servant Locator
    DatabaseServantLocator* m_servant_locator;
    
    // The Database object reference
    Demo::Database_var m_database_obj;
    
    // The Database servant
    Database_impl* m_database_impl;
    
};


#endif // INCL_DATABASE_PROCESS_BEHAVIOUR_HPP


