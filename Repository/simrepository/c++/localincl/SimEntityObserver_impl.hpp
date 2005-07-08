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


#ifndef INCL_TOOLS_SIM_ENTITY_OBSERVER_IMPL_HPP
#define INCL_TOOLS_SIM_ENTITY_OBSERVER_IMPL_HPP

#include "Foundation/common/System.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntityObserver.skel.hpp"
#include "simrepository/MessageLogger.stub.hpp"

namespace Cdmw
{
namespace Tools
{

/**
*Purpose:
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class SimEntityObserver_impl : 
        public POA_CdmwPlatformMngtEntity::EntityObserver,
        public virtual PortableServer::RefCountServantBase
{

public:

    /**
     * Purpose:
     * <p> Constructor
     * 
     */ 
    SimEntityObserver_impl(PortableServer::POA_ptr,                          
                           CdmwSimulatedRepository::MessageLogger_ptr)
        throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> Destructor
     * 
     */ 
    virtual 
    ~SimEntityObserver_impl()
        throw();
    
    PortableServer::POA_ptr _default_POA();

    //
    // IDL:thalesgroup.com/CdmwPlatformMngtEntity/SystemEntityObserver/set_system_entity_status:1.0
    //
    virtual
    void
    set_system_entity_status(const char* entity_name,
                             CdmwPlatformMngtEntity::EntityStatus entity_status,
                             const char* entity_info)
        throw(CdmwPlatformMngtEntity::EntityNotFound,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityObserver/set_application_entity_status:1.0
    //
    virtual
    void
    set_application_entity_status(const char* entity_name,
                                  CdmwPlatformMngtEntity::EntityStatus entity_status,
                                  const char* entity_info)
        throw(CdmwPlatformMngtEntity::EntityNotFound,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityObserver/set_process_entity_status:1.0
    //
    virtual
    void
    set_process_entity_status(const char* entity_name,
                              CdmwPlatformMngtEntity::EntityStatus entity_status,
                              const char* entity_info)
        throw(CdmwPlatformMngtEntity::EntityNotFound,
              CORBA::SystemException);
    


protected:
    
private:
    
    /**
     * Purpose:
     * <p>  Copy constructor
     * 
     */ 
    SimEntityObserver_impl (const SimEntityObserver_impl& rhs)
        throw();
    
    
    /**
     * Purpose:
     * <p> Assignment operator
     * 
     */ 
    SimEntityObserver_impl&
    operator=(const SimEntityObserver_impl& rhs)
        throw();

    /**
     * Holds a reference to ...
     */
    PortableServer::POA_var                     m_poa;
    CdmwSimulatedRepository::MessageLogger_var m_logger;

}; // End class SimEntityObserver_impl 

} // End namespace Tools
} // End namespace Cdmw

#endif // INCL_TOOLS_SIM_ENTITY_OBSERVER_IMPL_HPP

