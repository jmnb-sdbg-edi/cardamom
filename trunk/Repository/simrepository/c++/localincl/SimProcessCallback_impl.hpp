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


#ifndef INCL_TOOLS_SIM_PROCESS_CALLBACK_IMPL_HPP
#define INCL_TOOLS_SIM_PROCESS_CALLBACK_IMPL_HPP

#include "Foundation/common/System.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceBroker.stub.hpp"
#include "simrepository/MessageLogger.stub.hpp"
#include <string>

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
class SimProcessCallback_impl : 
        public POA_CdmwPlatformMngt::ProcessCallback,
        public virtual PortableServer::RefCountServantBase
{

public:

    /**
     * Purpose:
     * <p> Constructor
     * 
     */ 
    SimProcessCallback_impl(PortableServer::POA_ptr,
                            CosNaming::NamingContext_ptr,
                            CdmwPlatformMngt::ProcessMessageBroker_ptr,
                            CdmwPlatformMngtService::ServiceBroker_ptr,
                            CdmwSimulatedRepository::MessageLogger_ptr,
                            const std::string &)
        throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> Destructor
     * 
     */ 
    virtual 
    ~SimProcessCallback_impl()
        throw();
    
    PortableServer::POA_ptr _default_POA();

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/set_ready:1.0
    //
    /**
     * Not thread safe!
     */
    virtual
    CdmwPlatformMngt::ProcessCallback::ProcessStartingData*
    set_ready(CdmwPlatformMngt::ProcessDelegate_ptr the_process)
        throw(CORBA::SystemException);
        
    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/get_starting_info:1.0
    * operation
    */
    virtual CdmwPlatformMngt::ProcessCallback::ProcessStartingData* 
        get_starting_info(CdmwPlatformMngt::ProcessDelegate_ptr the_process)
            throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/set_creation_done:1.0
    * operation
    */
    virtual void set_creation_done()
        throw(CORBA::SystemException);
        

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/notify_error:1.0
    //
    virtual
    void
    notify_error(const char* issuer,
                 const char* error_info)
        throw(CORBA::SystemException);
    

protected:
    
private:
    
    /**
     * Purpose:
     * <p>  Copy constructor
     * 
     */ 
    SimProcessCallback_impl (const SimProcessCallback_impl& rhs)
        throw();
    
    
    /**
     * Purpose:
     * <p> Assignment operator
     * 
     */ 
    SimProcessCallback_impl&
    operator=(const SimProcessCallback_impl& rhs)
        throw();

    /**
     * Holds a reference to ...
     */
    PortableServer::POA_var                     m_poa;
    Cdmw::CommonSvcs::Naming::NamingInterface   m_root;
    CdmwPlatformMngt::ProcessMessageBroker_var  m_message_broker;
    CdmwPlatformMngtService::ServiceBroker_var  m_service_broker;
    CdmwSimulatedRepository::MessageLogger_var  m_logger;
    std::string                                 m_application_name;
    unsigned long                               m_process_number;

}; // End class SimProcessCallback_impl 

} // End namespace Tools
} // End namespace Cdmw

#endif // INCL_TOOLS_SIM_PROCESS_CALLBACK_IMPL_HPP

