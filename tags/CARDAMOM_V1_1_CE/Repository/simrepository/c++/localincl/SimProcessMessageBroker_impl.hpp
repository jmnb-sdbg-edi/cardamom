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


#ifndef INCL_TOOLS_SIM_PROCESS_MESSAGE_BROKER_IMPL_HPP
#define INCL_TOOLS_SIM_PROCESS_MESSAGE_BROKER_IMPL_HPP

#include "Foundation/common/System.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.skel.hpp"
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
class SimProcessMessageBroker_impl : 
        public POA_CdmwPlatformMngt::ProcessMessageBroker,
        public virtual PortableServer::RefCountServantBase
{

public:

    /**
     * Purpose:
     * <p> Constructor
     * 
     */ 
    SimProcessMessageBroker_impl(PortableServer::POA_ptr,
                                 CdmwSimulatedRepository::MessageLogger_ptr)
        throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> Destructor
     * 
     */ 
    virtual 
    ~SimProcessMessageBroker_impl()
        throw();
    
    PortableServer::POA_ptr _default_POA();

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_message:1.0
    //
    virtual
    void
    notify_message(const CdmwPlatformMngtBase::EventHeader& header,
                   const char* issuer,
                   const char* message)
        throw(CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_error:1.0
    //
    virtual
    void
    notify_error(const CdmwPlatformMngtBase::TimeStamp& time_stamp,
                 const char* issuer,
                 const char* error_info)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_user_event:1.0
    //
    virtual
    void
    notify_user_event(const char* issuer,
                      const CORBA::Any& data)
        throw(CORBA::SystemException);

protected:
    
private:
    
    /**
     * Purpose:
     * <p>  Copy constructor
     * 
     */ 
    SimProcessMessageBroker_impl (const SimProcessMessageBroker_impl& rhs)
        throw();
    
    
    /**
     * Purpose:
     * <p> Assignment operator
     * 
     */ 
    SimProcessMessageBroker_impl&
    operator=(const SimProcessMessageBroker_impl& rhs)
        throw();

    /**
     * Holds a reference to ...
     */
    PortableServer::POA_var                     m_poa;
    CdmwSimulatedRepository::MessageLogger_var m_logger;

}; // End class SimProcessMessageBroker_impl 

} // End namespace Tools
} // End namespace Cdmw

#endif // INCL_TOOLS_SIM_PROCESS_MESSAGE_BROKER_IMPL_HPP

