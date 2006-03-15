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


#ifndef INCL_PLATFORMMNGT_PROCESSMESSAGEBROKER_IMPL_HPP 
#define INCL_PLATFORMMNGT_PROCESSMESSAGEBROKER_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"

#include "idllib/CdmwPlatformMngtDaemon.stub.hpp"
#include "idllib/CdmwPlatformMngtManagedElement.stub.hpp"
#include "idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "idllib/CdmwPlatformMngtService.stub.hpp"
#include "idllib/CdmwPlatformMngtProcessCallback.skel.hpp"
#include "idllib/CdmwPlatformMngtProcessObserver.stub.hpp"
#include "idllib/CdmwPlatformMngtServiceBroker.stub.hpp"


#include "SystemMngt/platformlibrary/MultipleIdServant_impl.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformdaemonlibrary/DaemonLogMessageId.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
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
    */ 
    /* @param orb the initialised ORB used to create the specific POA,
    *  @param poaName the poa name used to create the specific POA.
    *  @param daemon the daemon reference
    */
    ProcessMessageBroker_impl(CORBA::ORB_ptr orb, const char* poaName, CdmwPlatformMngt::Daemon_ptr daemon);

    /**
    * Purpose:
    *  Sets the process observer.
    */ 
    void setProcessObserver(CdmwPlatformMngt::ProcessObserver_ptr processObserver)
        throw (BadParameterException);

            
    /**
    * Purpose:
    *  Sets the service broker.
    */ 
    void setServiceBroker(CdmwPlatformMngtService::ServiceBroker_ptr serviceBroker)
        throw (BadParameterException);
        
    /**
    * Purpose:
    *  Sets the application name.
    */ 
    void setApplicationName(const char* applicationName)
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
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_process_error:1.0
	* operation
	*/    
    virtual void notify_process_error (const char * failed_process_name,
                                       const CdmwPlatformMngtBase::TimeStamp & time_stamp,
                                       const char * issuer,
                                       const char * error_info)
        throw(CdmwPlatformMngt::ProcessNotFound,
              CORBA::SystemException);
              
              
    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_user_event:1.0
	* operation
	*/
    virtual void notify_user_event(const char* issuer,
                                   const CORBA::Any& data)
        throw(CORBA::SystemException);
        
        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_service:1.0
     * operator
     */
    CORBA::Object_ptr get_service(const char* service_name)
        throw (CdmwPlatformMngtService::ServiceNotFound,
               CdmwPlatformMngtService::ServiceUnavailable,
               CORBA::SystemException);
               
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_managed_element:1.0
     * operator
     */  
    CdmwPlatformMngt::ManagedElement_ptr get_managed_element (
        const char * element_path)
        throw (CdmwPlatformMngt::ElementPathInvalid,
               CdmwPlatformMngt::ManagedElementNotFound,
               CORBA::SystemException);
               
               
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_system_entity_status:1.0
     * operator
     */           
    char* get_system_entity_status (
        const char* entity_name,
        CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException);
               
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_host_entity_status:1.0
     * operator
     */           
    char* get_host_entity_status (
        const char* host_name,
        const char* entity_name,
        CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_application_entity_status:1.0
     * operator
     */           
    char* get_application_entity_status (
        const char* application_name,
        const char* entity_name,
        CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::ApplicationNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException);
               
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_process_entity_status:1.0
     * operator
     */
    char* get_process_entity_status (
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const char* entity_name,
        CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::ProcessNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/set_system_entity_status:1.0
     * operator
     */
    void set_system_entity_status (
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException);
               
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/set_host_entity_status:1.0
     * operator
     */
    void set_host_entity_status (
        const char* host_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException);
               
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/set_application_entity_status:1.0
     * operator
     */
    void set_application_entity_status (
        const char* application_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
        throw (CdmwPlatformMngt::ApplicationNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/set_process_entity_status:1.0
     * operator
     */
    void set_process_entity_status (
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
        throw (CdmwPlatformMngt::ProcessNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException);


private:

    /**
     * The application's process observer.
     */
    CdmwPlatformMngt::ProcessObserver_var m_processObserver;

    /**
     * The application's service broker.
     */
    CdmwPlatformMngtService::ServiceBroker_var m_serviceBroker;
    
    
    /**
    * The name of the application.
    */
    std::string m_applicationName;
    
    /**
    * The name of the host.
    */
    std::string m_hostName;


    /**
    * The daemon.
    */
    CdmwPlatformMngt::Daemon_var m_daemon;
 
};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PROCESSMESSAGEBROKER_IMPL_HPP

