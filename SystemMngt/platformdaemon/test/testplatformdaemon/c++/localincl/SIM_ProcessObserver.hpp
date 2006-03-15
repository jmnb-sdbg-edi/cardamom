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


#ifndef INCL_PLATFORMMNGT_SIM_PROCESSOBSERVER_HPP 
#define INCL_PLATFORMMNGT_SIM_PROCESSOBSERVER_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessObserver.skel.hpp"
#include <fstream>
#include <iostream>

namespace Cdmw
{
namespace PlatformMngt
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
class SIM_ProcessObserver : public virtual POA_CdmwPlatformMngt::ProcessObserver,
    virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        */ 
        SIM_ProcessObserver(const char* logFile);


        /**
        * Purpose:
        * <p> Stops the observer.
        * Closes the log file.
        */ 
        void stop();


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~SIM_ProcessObserver()
                throw();


        void notify_message(
            const char* application_name,
            const char* process_name,
            const char* host_name,
            const CdmwPlatformMngtBase::EventHeader& header,
            const char* issuer,
            const char* message)
                throw (CdmwPlatformMngt::ProcessNotFound ,
                       CORBA::SystemException);

        void notify_error(
            const char* application_name,
            const char* process_name,
            const char* host_name,
            const CdmwPlatformMngtBase::TimeStamp& time_stamp,
            const char* issuer,
            const char* error_info)
                throw (CdmwPlatformMngt::ProcessNotFound,
                       CORBA::SystemException);

        void notify_starting(
            const char* application_name,
            const char* process_name,
            const char* host_name,
            CdmwPlatformMngt::ProcessPID pid,
            const CdmwPlatformMngtBase::TimeStamp & time_stamp,
            CORBA::Long restart_attempt)
                throw (CdmwPlatformMngt::ProcessNotFound,
                       CORBA::SystemException);
                   
                   
        void notify_ending(
            const char* application_name,
            const char* process_name,
            const char* host_name,
            const CdmwPlatformMngtBase::TimeStamp& time_stamp,
            CORBA::Long exit_status)
                throw (CdmwPlatformMngt::ProcessNotFound,
                       CORBA::SystemException);

    
    void notify_monitoring_failure(
            const char* application_name,
            const char* process_name,
            const char* host_name,
            const CdmwPlatformMngtBase::TimeStamp& time_stamp )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException );


    void notify_user_event(
                           const char* application_name,
                           const char* process_name,
                           const char* host_name,
                           const char* issuer,
                           const CORBA::Any& data )
        throw (CdmwPlatformMngt::ProcessNotFound,
               CORBA::SystemException);

    void set_create_process_result (
                                    const char * application_name,
                                    const char * process_name,
                                    const char * host_name,
                                    CdmwPlatformMngt::ProcessCommandResult result,
                                    const char* error_info,
                                    const char* error_issuer,
                                    CdmwPlatformMngt::ProcessDelegateWrapper_ptr process_delegate_wrap)
        throw (CdmwPlatformMngt::ProcessNotFound,
               CORBA::SystemException);


    void set_initialisation_result (
                                    const char * application_name,
                                    const char * process_name,
                                    const char * host_name,
                                    CdmwPlatformMngt::ProcessCommandResult result)
        throw (CdmwPlatformMngt::ProcessNotFound,
               CORBA::SystemException);
     

    void set_next_step_result (
                               const char * application_name,
                               const char * process_name,
                               const char * host_name,
                               CdmwPlatformMngt::ProcessCommandResult result,
                               CORBA::ULong step_nbr)
        throw (CdmwPlatformMngt::ProcessNotFound,
               CORBA::SystemException);
     

    void set_run_result (
                         const char * application_name,
                         const char * process_name,
                         const char * host_name,
                         CdmwPlatformMngt::ProcessCommandResult result)
        throw (CdmwPlatformMngt::ProcessNotFound,
               CORBA::SystemException);
     

    void set_stop_result (
                          const char * application_name,
                          const char * process_name,
                          const char * host_name,
                          CdmwPlatformMngt::ProcessCommandResult result)
        throw (CdmwPlatformMngt::ProcessNotFound,
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
               CORBA::SystemException)
    {
        CdmwPlatformMngt::ManagedElement_var element;
        return element._retn();
    }
     
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
         * The log file stream
         */
        std::ofstream m_fs;


}; // End class SIM_ProcessObserver 

} // End namespace PlatformMngt
} // End namespace Cdmw
#endif


