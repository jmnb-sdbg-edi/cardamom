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


#ifndef INCL_PLATFORMMNGT_PROCESSMESSAGEBROKER_IMPL_HPP 
#define INCL_PLATFORMMNGT_PROCESSMESSAGEBROKER_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"

#include "idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "idllib/CdmwPlatformMngtProcessCallback.skel.hpp"

#include "SystemMngt/platformlibrary/MultipleIdServant_impl.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformagent/ApplicationAgentLogMessageId.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "FaultTolerance/idllib/FT.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#endif

namespace Cdmw
{
namespace PlatformMngt
{

#ifdef CDMW_USE_FAULTTOLERANCE
class ApplicationAgent_impl;
#endif
    
/**
* Purpose:
* <p>
* implements the
* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker:1.0
* interface
*
* @see MultipleIdServant_impl.
*/
class ProcessMessageBroker_impl : virtual public POA_CdmwPlatformMngt::ProcessMessageBroker,
    virtual public PortableServer::RefCountServantBase,
    virtual public MultipleIdServant_impl
{

public:


    /**
    * Purpose:
    * <p> Constructor.
    *
    * @param orb the initialised ORB used to create the specific POA.
    */ 
#ifdef CDMW_USE_FAULTTOLERANCE
    ProcessMessageBroker_impl(ApplicationAgent_impl* agent, CORBA::ORB_ptr orb );
#else
    ProcessMessageBroker_impl(CORBA::ORB_ptr orb);
#endif

    /**
    * Purpose:
    *  Sets the process observer.
    */ 
    void setProcessObserver(CdmwPlatformMngt::ProcessObserver_ptr processObserver)
        throw (BadParameterException);

    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_message:1.0
	* operation
	*/
    virtual void notify_message(const CdmwPlatformMngtBase::EventHeader& header,
                                const char* issuer, const char* message)
        throw(CORBA::SystemException);


    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_error:1.0
	* operation
	*/
    virtual void notify_error(const CdmwPlatformMngtBase::TimeStamp& time_stamp,
                              const char* issuer, const char* error_info)
        throw(CORBA::SystemException);


    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_user_event:1.0
	* operation
	*/
    virtual void notify_user_event(const char* issuer,
                                   const CORBA::Any& data)
        throw(CORBA::SystemException);


private:

    /**
     * The application's process observer.
     */
    CdmwPlatformMngt::ProcessObserver_var m_processObserver;
    
#ifdef CDMW_USE_FAULTTOLERANCE
    /**
     * the application agent
     */
    ApplicationAgent_impl *m_agent;
#endif    
};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PROCESSMESSAGEBROKER_IMPL_HPP

