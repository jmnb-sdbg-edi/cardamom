/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef INCL_PLATFORMMNGT_ENTITY_OBSERVER_HPP
#define INCL_PLATFORMMNGT_ENTITY_OBSERVER_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
#include "SystemMngt/idllib//CdmwPlatformMngtEntityObserver.skel.hpp"
#include "SystemMngt/platformlibrary/MultipleIdServant_impl.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class Application_impl;

/**
 * This interface allows application software to change the status
 * of :
 * - one of its entity
 * - one of its application's entities
 * - one of the entities of the system.
 */
class EntityObserver_impl : public virtual POA_CdmwPlatformMngtEntity::EntityObserver,
                            public virtual MultipleIdServant_impl,
                            public virtual PortableServer::RefCountServantBase
{
private:
    /**
     * The application reponsible for the entity.
     */
    Application_impl* m_application;

private:
    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    EntityObserver_impl()
    : MultipleIdServant_impl( Configuration::M_orb.in(), "EntityObserverPOA" )
    {};

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    EntityObserver_impl( EntityObserver_impl& rhs )
    : MultipleIdServant_impl( Configuration::M_orb.in(), "EntityObserverPOA" )
    {};

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    EntityObserver_impl& operator=(
        const EntityObserver_impl& rhs )
    {
        return *this;
    };

public:
    /**
     *Purpose:
     *<p> Constructor.
     */
    EntityObserver_impl(
        Application_impl* application )
            throw( BadParameterException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/SystemEntityObserver/set_system_entity_status:1.0
     * operator
     */
    void set_system_entity_status(
        const char* entity_name,
        CdmwPlatformMngtEntity::EntityStatus entity_status,
        const char* entity_info )
            throw( CdmwPlatformMngtEntity::EntityNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityObserver/set_application_entity_status:1.0
     * operator
     */
    void set_application_entity_status(
        const char* entity_name,
        CdmwPlatformMngtEntity::EntityStatus entity_status,
        const char* entity_info )
            throw( CdmwPlatformMngtEntity::EntityNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityObserver/set_process_entity_status:1.0
     * operator
     */
    void set_process_entity_status(
        const char* entity_name,
        CdmwPlatformMngtEntity::EntityStatus entity_status,
        const char* entity_info )
            throw( CdmwPlatformMngtEntity::EntityNotFound,
                   CORBA::SystemException );
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_ENTITY_OBSERVER_HPP

