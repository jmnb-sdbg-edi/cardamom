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


#ifndef INCL_PLATFORMMNGT_FTPROCESS_OBSERVER_IMPL_HPP
#define INCL_PLATFORMMNGT_FTPROCESS_OBSERVER_IMPL_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessObserver.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessDelegateWrapper.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformlibrary/EventHandler.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class System_impl;


/**
 *Purpose:
 *<p> Provides the event notifications to the processes of an application.
 */
class FTProcessObserver_impl : public virtual POA_CdmwPlatformMngt::ProcessObserver,
                               public virtual PortableServer::RefCountServantBase,
                               public DeactivableServant_impl
{
private:

    /**
     * The associated system.
     */
    System_impl* m_system;
    
    /**
     * The repository of the managed elements.
     */
    CdmwPlatformMngt::ElementRepository_var m_element_repository;

private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    FTProcessObserver_impl(
        FTProcessObserver_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    FTProcessObserver_impl& operator=(
        const FTProcessObserver_impl& rhs);

public:

    /**
     *Purpose:
     *<p> Constructor.
     */
    FTProcessObserver_impl(
        PortableServer::POA_ptr poa,
        System_impl* system,
        CdmwPlatformMngt::ElementRepository_ptr element_repository);

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_message:1.0
     * operator
     */
    virtual
    void notify_message(
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const CdmwPlatformMngtBase::EventHeader& header,
        const char* issuer,
        const char* message)
            throw (CdmwPlatformMngt::ProcessNotFound ,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_error:1.0
     * operator
     */
    void notify_error(
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp,
        const char* issuer,
        const char* error_info)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_starting:1.0
     * operator
     */
    void notify_starting(
        const char* application_name,
        const char* process_name,
        const char* host_name,
        CdmwPlatformMngt::ProcessPID pid,
        const CdmwPlatformMngtBase::TimeStamp & time_stamp,
        CORBA::Long restart_attempt)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException);
                   
                   
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_ending:1.0
     * operator
     */
    void notify_ending(
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp,
        CORBA::Long exit_status)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_monitoring_failure:1.0
     * operator
     */
    void notify_monitoring_failure(
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const CdmwPlatformMngtBase::TimeStamp& time_stamp )
            throw( CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/notify_user_event:1.0
     * operator
     */
    void notify_user_event(
        const char* application_name,
        const char* process_name,
        const char* host_name,
        const char* issuer,
        const CORBA::Any& data )
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException);
                   
                   
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/get_managed_element:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/get_system_entity_status:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/get_host_entity_status:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/get_application_entity_status:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/get_process_entity_status:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/set_system_entity_status:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/set_host_entity_status:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/set_application_entity_status:1.0
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/set_process_entity_status:1.0
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
               
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/set_create_process_result:1.0
     * operator
     */
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

     
     /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/set_initialisation_result:1.0
     * operator
     */
     void set_initialisation_result (
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/set_next_step_result:1.0
     * operator
     */
     void set_next_step_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result,
        CORBA::ULong step_nbr)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException);
     
     /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/set_run_result:1.0
     * operator
     */
     void set_run_result (
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
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessObserver/set_stop_result:1.0
     * operator
     */
     void set_stop_result (
        const char * application_name,
        const char * process_name,
        const char * host_name,
        CdmwPlatformMngt::ProcessCommandResult result)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CORBA::SystemException);
     
     
};



} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_FTPROCESS_OBSERVER_IMPL_HPP

