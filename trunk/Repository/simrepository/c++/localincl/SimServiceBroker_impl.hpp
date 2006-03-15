/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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


#ifndef INCL_TOOLS_SIM_SERVICE_BROKER_IMPL_HPP
#define INCL_TOOLS_SIM_SERVICE_BROKER_IMPL_HPP

#include "Foundation/common/System.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceBroker.skel.hpp"
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
class SimServiceBroker_impl : 
        public POA_CdmwPlatformMngtService::ServiceBroker,
        public virtual PortableServer::RefCountServantBase
{

public:

    /**
     * Purpose:
     * <p> Constructor
     * 
     */ 
    SimServiceBroker_impl(PortableServer::POA_ptr,
                          CosNaming::NamingContext_ptr,                          
                          CdmwSimulatedRepository::MessageLogger_ptr)
        throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> Destructor
     * 
     */ 
    virtual 
    ~SimServiceBroker_impl()
        throw();
    
    PortableServer::POA_ptr _default_POA();    
    
    //
    // IDL:thalesgroup.com/CdmwPlatformMngtService/ServiceBroker/get_service:1.0
    //
    virtual
    CORBA::Object_ptr
    get_service(const char* application_name,
                const char* process_name,
                const char* host_name,
                const char* service_name)
        throw(CdmwPlatformMngtService::ServiceNotFound,
              CdmwPlatformMngtService::ServiceUnavailable,
              CORBA::SystemException);
    void
    set_service(const char* service_name, CORBA::Object_ptr object_ref)
        throw (CORBA::SystemException);
    
#ifdef CDMW_USE_FAULTTOLERANCE
    /**
     *Purpose:
     *<p>
     * Implements the 
     * IDL:thalesgroup.com/CdmwPlatformMngt/ServiceBroker/get_global_service:1.0
     * operator
     */
    void set_global_service(const char* service_name, CORBA::Object_ptr service_object)
        throw( CdmwPlatformMngtService::ServiceAlreadyExists,
               CORBA::SystemException );
#endif

protected:
    
private:
    
    /**
     * Purpose:
     * <p>  Copy constructor
     * 
     */ 
    SimServiceBroker_impl (const SimServiceBroker_impl& rhs)
        throw();
    
    
    /**
     * Purpose:
     * <p> Assignment operator
     * 
     */ 
    SimServiceBroker_impl&
    operator=(const SimServiceBroker_impl& rhs)
        throw();

    /**
     * Holds a reference to ...
     */
    PortableServer::POA_var                    m_poa;
    Cdmw::CommonSvcs::Naming::NamingInterface  m_root;
    CdmwSimulatedRepository::MessageLogger_var m_logger;

}; // End class SimServiceBroker_impl 

} // End namespace Tools
} // End namespace Cdmw

#endif // INCL_TOOLS_SIM_SERVICE_BROKER_IMPL_HPP

