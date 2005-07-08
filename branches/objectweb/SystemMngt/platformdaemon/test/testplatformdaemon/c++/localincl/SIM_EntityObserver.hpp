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


#ifndef INCL_PLATFORMMNGT_SIM_ENTITY_OBSERVER_HPP
#define INCL_PLATFORMMNGT_SIM_ENTITY_OBSERVER_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntityObserver.skel.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


class SIM_EntityObserver : public virtual POA_CdmwPlatformMngtEntity::EntityObserver,
                            public virtual PortableServer::RefCountServantBase
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     */
    SIM_EntityObserver()
    {
    }

    /**
     *Purpose:
     *<p> Destructor.
     */
    virtual ~SIM_EntityObserver()
    {
    }

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
                   CORBA::SystemException )
    {

    }


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
                   CORBA::SystemException )
    {

    }


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
                   CORBA::SystemException )
    {

    }


};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_SIM_ENTITY_OBSERVER_HPP

