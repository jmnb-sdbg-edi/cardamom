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


#ifndef INCL_TOOLS_SIM_PROCESS_MESSAGE_BROKER_IMPL_HPP
#define INCL_TOOLS_SIM_PROCESS_MESSAGE_BROKER_IMPL_HPP

#include "Foundation/common/System.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtManagedElement.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessObserver.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceBroker.stub.hpp"
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
                                 CdmwPlatformMngtService::ServiceBroker_ptr service_broker,
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
    virtual
    void
    notify_user_event(const char* issuer,
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

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_system_entity_status:1.0
    //           
    char* get_system_entity_status (
        const char* entity_name,
        CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException)
    {
        std::cout<<"ProcessMessageBroker::get_system_entity_status"<<"\n"<<"("<< entity_name << ")"<<std::endl;
        
        CORBA::String_var entity_status = CORBA::string_dup("");
        entity_info = CORBA::string_dup("");
        
        return entity_status._retn();
    }
    
    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_host_entity_status:1.0
    //           
    char* get_host_entity_status (
        const char* host_name,
        const char* entity_name,
        CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException)
    {
        if (host_name == NULL)
        {
            std::cout<<"ProcessMessageBroker::get_host_entity_status"<<"\n"<<"("<< entity_name <<")"<<std::endl;
        }
        else
        {
            std::cout<<"ProcessMessageBroker::get_host_entity_status"<<"\n"<<"(" << host_name << "," << entity_name <<")"<<std::endl;
        }
        
        CORBA::String_var entity_status = CORBA::string_dup("");
        entity_info = CORBA::string_dup("");
        
        return entity_status._retn();
    }
    
    
    
    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_application_entity_status:1.0
    //           
    char* get_application_entity_status (
        const char* application_name,
        const char* entity_name,
        CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::ApplicationNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException)
    {
        if (application_name == NULL)
        {
            std::cout<<"ProcessMessageBroker::get_application_entity_status"<<"\n"<<"(" << entity_name <<")"<<std::endl;
        }
        else
        {
            std::cout<<"ProcessMessageBroker::get_application_entity_status"<<"\n"<<"(" << application_name << "," << entity_name <<")"<<std::endl;
        }
        
        CORBA::String_var entity_status = CORBA::string_dup("");
        entity_info = CORBA::string_dup("");
        
        return entity_status._retn();
    }
               
    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_process_entity_status:1.0
    //
    char* get_process_entity_status (
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const char* entity_name,
        CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException)
    {
        if (application_name == NULL)
        {
            std::cout<<"ProcessMessageBroker::get_process_entity_status"<<"\n"<<"("<< entity_name << ")"<<std::endl;
        }
        else
        {
            std::cout<<"ProcessMessageBroker::get_process_entity_status"<<"\n"<<"("
                     << application_name << "," << process_name << "," << host_name << "," 
                     << entity_name << ")"<<std::endl;
        }
        
        CORBA::String_var entity_status = CORBA::string_dup("");
        entity_info = CORBA::string_dup("");
        
        return entity_status._retn();
    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/set_system_entity_status:1.0
    //
    void set_system_entity_status (
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException)
    {
        std::cout<<"ProcessMessageBroker::set_system_entity_status"<<"\n"
                 <<"("<< entity_name << "," << entity_status << "," << entity_info << ")" <<std::endl;
    }
    
    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/set_host_entity_status:1.0
    //
    void set_host_entity_status (
        const char* host_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException)
    {
        if (host_name == NULL)
        {
            std::cout<<"ProcessMessageBroker::set_host_entity_status"<<"\n"
                     <<"(" << entity_name << "," << entity_status << "," << entity_info << ")" <<std::endl;
        }
        else
        {
            std::cout<<"ProcessMessageBroker::set_host_entity_status"<<"\n"
                     <<"(" << host_name << "," << entity_name << "," << entity_status << "," << entity_info << ")" <<std::endl;
        }
    }
               
    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/set_application_entity_status:1.0
    //
    void set_application_entity_status (
        const char* application_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
        throw (CdmwPlatformMngt::ApplicationNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException)
    {
        if (application_name == NULL)
        {
            std::cout<<"ProcessMessageBroker::set_application_entity_status"<<"\n"
                     <<"(" << entity_name << "," << entity_status << "," << entity_info << ")" <<std::endl;
        }
        else
        {
            std::cout<<"ProcessMessageBroker::set_application_entity_status"<<"\n"
                     <<"(" << application_name << "," << entity_name << "," << entity_status << "," << entity_info << ")" <<std::endl;
        }
    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/set_process_entity_status:1.0
    //
    void set_process_entity_status (
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const char* entity_name,
        const char* entity_status,
        const char* entity_info)
        throw (CdmwPlatformMngt::ProcessNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException)
    {
        if (application_name == NULL)
        {
            std::cout<<"ProcessMessageBroker::set_process_entity_status"<<"\n"
                     <<"(" << entity_name << "," << entity_status << "," << entity_info << ")" <<std::endl;
        }
        else
        {
            std::cout<<"ProcessMessageBroker::set_process_entity_status"<<"\n"
                     <<"(" << application_name << "," << process_name << "," << host_name << "," 
                     << entity_name << "," << entity_status << "," << entity_info << ")" <<std::endl;
        }
    }
    

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
    PortableServer::POA_var m_poa;
    
    /**
     * The application's service broker.
     */
    CdmwPlatformMngtService::ServiceBroker_var m_service_broker;
    
    /**
     * The message logger.
     */
    CdmwSimulatedRepository::MessageLogger_var m_logger;
    

    
    

    /**
    * The name of the host.
    */
    std::string m_hostName;

}; // End class SimProcessMessageBroker_impl 

} // End namespace Tools
} // End namespace Cdmw

#endif // INCL_TOOLS_SIM_PROCESS_MESSAGE_BROKER_IMPL_HPP

