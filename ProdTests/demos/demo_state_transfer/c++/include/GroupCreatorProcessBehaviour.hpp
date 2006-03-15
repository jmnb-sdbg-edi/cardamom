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


#ifndef INCL_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP 
#define INCL_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP 



// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>


class GroupCreatorProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    // Constructor
    GroupCreatorProcessBehaviour(CORBA::ORB_ptr orb,  bool point_to_point);

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

        
        
private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    /**
     * The replication manager.
     */
    FT::ReplicationManager_var m_replication_mgr;

    /**
     * The object group.
     */
    ::FT::ObjectGroup_var m_obj_group;

    /**
     * The object_groups NameDomain.
     */
    CdmwNamingAndRepository::NameDomain_var m_objGroupsDomain;
            
    /**
     * True if point-to-point state transfer will be used.
     * In this case, the DataStore identifiers have to be specified
     * into Criterias at object group creation. 
     */
    bool m_point_to_point;
};

#endif // INCL_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP

