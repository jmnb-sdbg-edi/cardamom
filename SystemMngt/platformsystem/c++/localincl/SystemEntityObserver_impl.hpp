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


#ifndef INCL_PLATFORMMNGT_SYSTEM_ENTITY_OBSERVER_HPP
#define INCL_PLATFORMMNGT_SYSTEM_ENTITY_OBSERVER_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntityObserver.skel.hpp"



namespace Cdmw
{
namespace PlatformMngt
{

class System_impl;
class RWEntityContainer_impl;

/**
 * This interface allows application software to change the status
 * of :
 * - one of the entities of the system.
 */
class SystemEntityObserver_impl : 
             virtual public POA_CdmwPlatformMngtEntity::SystemEntityObserver,
             virtual public PortableServer::RefCountServantBase
{


public:
    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param system          the system responsible for the host
     *@param entityContainer the entity container for the system
     *
     */
    SystemEntityObserver_impl (System_impl* system, RWEntityContainer_impl* entityContainer)      
            throw (BadParameterException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/SystemEntityObserver/set_system_entity_status:1.0
     * operator
     */
    void set_system_entity_status (const char* entity_name,        
                                   CdmwPlatformMngtEntity::EntityStatus entity_status,
                                   const char* entity_info)
            throw (CdmwPlatformMngtEntity::EntityNotFound,
                   CORBA::SystemException);

    
                   
private:

    /**
     * The system responsible for the entity.
     */
    System_impl* m_system;
  
    /**
     * The entity container.
     */
    RWEntityContainer_impl* m_entityContainer;


    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    SystemEntityObserver_impl()
    {};

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    SystemEntityObserver_impl (SystemEntityObserver_impl& rhs)
    {};

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    SystemEntityObserver_impl& operator= (const SystemEntityObserver_impl& rhs)
    {
        return *this;
    };
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_SYSTEM_ENTITY_OBSERVER_HPP

