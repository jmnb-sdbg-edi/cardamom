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


#ifndef INCL_PLATFORMMNGT_PLATFORMINTERFACE_HPP 
#define INCL_PLATFORMMNGT_PLATFORMINTERFACE_HPP 

#include <string>

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"

#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "SystemMngt/platforminterface/UserCommandCallback.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommonBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommandBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCmdResponseCallback.stub.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
* Purpose:
* <p>
* The interface to the Platform Management service.
*/
class PlatformInterface
{

public:

    /**
    * Purpose:
    * <p>
    * Returns true if the process has been launched by the platform management.
    * In this case, the command line contains a stringified ProcessCallback
    * reference that is used for the creation acknowledgement.
    */
    static bool Is_launched_by_PlatformManagement (int argc, char* argv[]);


    /**
    * Purpose:
    * <p>
    * Sets the interface to the Platform Management service up.
    *
    * @param orb the initialised ORB.
    * @param argc the number of arguments of the command line.
    * @param argv the array of arguments of the command line.
    * @exception BadOrderException if the setup has already been performed.
    * @exception BadParameterException if orb is NIL or the command line doesn't
    * contain the stringified ProcessCallback reference.
    */
    static void Setup (CORBA::ORB_ptr orb, int argc, char* argv[])
        throw (BadOrderException, BadParameterException);

    /**
    * Purpose:
    * <p>
    * Releases all static object references initialised
    * by previous call to <I>Setup()</I>. This should be the last 
    * operation on this class.
    *
    */
    static void Cleanup ();

    //// For backward compatibility
    ////
    /**
    * Purpose:
    * <p>
    * Acknowledges the creation of the user supplied managed process.
    *
    * @param process the process.
    * @exception BadOrderException if the setup has not been performed.
    * @exception BadParameterException if process is NIL.
    */
    static void Acknowledge_creation(CdmwPlatformMngt::ProcessDelegate_ptr process)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Creates the default implementation of a managed process and
    * acknowledges the latter.
    * If successful, the method takes ownership of the process behaviour
    * which will be deallocated when the default implementation
    * will be deallocated itself.
    *
    * @param behaviour the actual behaviour of the managed process.
    * @param pullMonitorable the actual pullMonitorable CORBA object of managed process.
    * @exception BadOrderException if the setup has not been performed.
    * @exception BadParameterException if behaviour is NULL.
    */
    static void Acknowledge_creation(ProcessBehaviour *behaviour)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);
    
    ////
    
    /**
    * Purpose:
    * <p>
    * Notify the creation starting of the user supplied managed process.
    *
    * @param process the process delegate.
    * @exception BadOrderException if the setup has not been performed.
    * @exception BadParameterException if process is NIL.
    */
    static void Notify_creation_starting (CdmwPlatformMngt::ProcessDelegate_ptr process)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Creates the default implementation of a managed process and
    * acknowledges the latter.
    * If successful, the method takes ownership of the process behaviour
    * which will be deallocated when the default implementation
    * will be deallocated itself.
    *
    * @param behaviour the actual behaviour of the managed process.
    * @exception BadOrderException if the setup has not been performed.
    * @exception BadParameterException if behaviour is NULL.
    */
    static void Notify_creation_starting (ProcessBehaviour *behaviour)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);
        
        
    /**
    * Purpose:
    * <p>
    * Indicate to SystemMngt that the process creation is done
    *
    * @exception BadOrderException if the setup has not been performed.
    */
    static void Notify_creation_done()
        throw (CORBA::SystemException, BadOrderException);


    /**
    * Purpose:
    * <p>
    * Notifies an applicative message.
    *
    * @param level the level of the message.
    * @param issuer the issuer of the message.
    * @param message the message to notify.
    * @exception CORBA::SystemException the CORBA system exception raised
    *     by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if level is CdmwPlatformMngtBase::FTL,
    *     or issuer is NULL or is CdmwPlatformMngtBase::PFMMNG,
    *     or message is NULL.
    */
    static void Notify_message (CdmwPlatformMngtBase::EventLevel level,
                                const char* issuer, const char* message)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Notifies a fatal error.
    * If the acknowledgement has not been performed yet, this method indicates
    * that the process cannot be actually created.
    * The process will be killed by the Platform Management service.
    *
    * @param issuer the issuer of the error.
    * @param error_info the information about the error.
    * @exception CORBA::SystemException the CORBA system exception raised
    *     by the operation call.
    * @exception BadOrderException if the setup has not been performed.
    * @exception BadParameterException if issuer is NULL or is
    *     CdmwPlatformMngtBase::PFMMNG, or error_info is NULL.
    */
    static void Notify_fatal_error (const char* issuer, const char* error_info)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);
        
       
       
      
    /**
    * Purpose:
    * <p>
    * Notifies a process fatal error.
    * The process will be killed by the Platform Management service.
    *
    * @param process_name the name of faulty process
    * @param issuer the issuer of the error.
    * @param error_info the information about the error.
    * @exception CORBA::SystemException the CORBA system exception raised
    *     by the operation call.
    * @exception BadOrderException if the setup has not been performed.
    * @exception BadParameterException if issuer is NULL or is
    *     CdmwPlatformMngtBase::PFMMNG, or error_info is NULL.
    * @exception CdmwPlatformMngt::ProcessNotFound if faulty process is not found
    */
    static void Notify_process_fatal_error (const char* process_name,
        const char* issuer, const char* error_info)
        throw (CORBA::SystemException, BadOrderException, BadParameterException,
               CdmwPlatformMngt::ProcessNotFound);


    /**
    * Purpose:
    * <p>
    * Notifies an applicative user event.
    *
    * @param issuer the issuer of the event.
    * @param data the data to notify.
    * @exception CORBA::SystemException the CORBA system exception raised
    *     by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if level is CdmwPlatformMngtBase::FTL,
    *    or issuer is NULL or is CdmwPlatformMngtBase::PFMMNG,
    *    or message is NULL.
    */
    static void Notify_user_event (const char* issuer,
                                   const CORBA::Any& data)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Returns the name of the application owning the process.
    *
    * @exception BadOrderException if the acknowledgement has not been performed.
    */
    static const char* Get_application_name()
        throw (BadOrderException);


    /**
    * Purpose:
    * <p>
    * Returns the process name.
    *
    * @exception BadOrderException if the acknowledgement has not been performed.
    */
    static const char* Get_process_name()
        throw (BadOrderException);


    /**
    * Purpose:
    * <p>
    * Returns the process configuration data.
    *
    * @exception BadOrderException if the acknowledgement has not been performed.
    *
    */     
    static CdmwPlatformMngt::ProcessConfigurationData Get_process_config_data()
        throw (BadOrderException);
         
         
    /**
    * Purpose:
    * <p>
    * Returns the specified service.
    *
    * @param service_name the name of the service.
    * @exception CdmwPlatformMngtService::ServiceNotFound if service_name
    *     doesn't denote a service that has been defined for the process.
    * @exception CdmwPlatformMngtService::ServiceUnavailable if the specified
    *      service is unavailable.
    * @exception CORBA::SystemException the CORBA system exception raised
    *      by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if service_name is NULL.
    */
    static CORBA::Object_ptr Get_service (const char* service_name)
        throw (CdmwPlatformMngtService::ServiceNotFound,
               CdmwPlatformMngtService::ServiceUnavailable,
               CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Get the object reference of the managed element specified
    * by its path
    *
    * @param element_path the path of the managed element.
    *
    * @return the object reference.
    *
    * @exception CdmwPlatformMngt::ManagedElementNotFound if path
    *     doesn't denote an existing element.
    * @exception CORBA::SystemException the CORBA system exception raised
    *     by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if element path is NULL.
    */
    static CdmwPlatformMngt::ManagedElement_ptr Get_managed_element (const char* element_path)
        throw (CdmwPlatformMngt::ManagedElementNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);
               
    /**
    * Purpose:
    * <p>
    * Gets the status of the specified entity of the system.
    *
    * @param entity_name the name of the entity.
    * @param entity_info the returned additional information.
    * @return the status of the entity.
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name is NULL.
    */
    static char* Get_system_entity_status (const char* entity_name,
                                           CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);
               
    /**
    * Purpose:
    * <p>
    * Gets the status of the specified entity of the host.
    *
    * @param entity_name the name of the entity.
    * @param entity_info the returned additional information.
    * @return the status of the entity.
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name is NULL.
    */
    static char* Get_host_entity_status (const char* entity_name,
                                         CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);


    
    /**
    * Purpose:
    * <p>
    * Gets the status of the specified entity of the application.
    *
    * @param entity_name the name of the entity.
    * @param entity_info the returned additional information.
    * @return the status of the entity.
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name is NULL.
    */
    static char* Get_application_entity_status (const char* entity_name,
                                                CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);



    /**
    * Purpose:
    * <p>
    * Gets the status of the specified entity of the process.
    *
    * @param entity_name the name of the entity.
    * @param entity_info the returned additional information.
    * @return the status of the entity.
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name is NULL.
    */
    static char* Get_process_entity_status (const char* entity_name,
                                            CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);
               
    /**
    * Purpose:
    * <p>
    * Sets the status of the specified entity of the system.
    *
    * @param entity_name the name of the entity.
    * @param entity_status the new status of the entity.
    * @param entity_info the additional information.
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name, entity_status or entity_info is NULL.
    */
    static void Set_system_entity_status (const char* entity_name,
                                          const char* entity_status,
                                          const char* entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);
     
               
    /**
    * Purpose:
    * <p>
    * Sets the status of the specified entity of the host.
    *
    * @param entity_name the name of the entity.
    * @param entity_status the new status of the entity.
    * @param entity_info the additional information.
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name, entity_status or entity_info is NULL.
    */
    static void Set_host_entity_status (const char* entity_name,
                                        const char* entity_status,
                                        const char* entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);


    
    /**
    * Purpose:
    * <p>
    * Sets the status of the specified entity of the application.
    *
    * @param entity_name the name of the entity.
    * @param entity_status the new status of the entity.
    * @param entity_info the additional information.
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name, entity_status or entity_info is NULL.
    */
    static void Set_application_entity_status (const char* entity_name,
                                               const char* entity_status,
                                               const char* entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);



    /**
    * Purpose:
    * <p>
    * Sets the status of the specified entity of the process.
    *
    * @param entity_name the name of the entity.
    * @param entity_status the new status of the entity.
    * @param entity_info the additional information.
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name, entity_status or entity_info is NULL.
    */
    static void Set_process_entity_status (const char* entity_name,
                                           const char* entity_status,
                                           const char* entity_info)
        throw (CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);
               
       

    /**
    * Purpose:
    * <p>
    * Gets the status of the specified host entity.
    *    
    * @param host_name the name of the host.
    * @param entity_name the name of the entity.
    * @param entity_info the returned additional information.
    * @return the status of the entity.
    * @exception HostNotFound if host has not been found
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name is NULL.
    */
    static char* Get_host_entity_status (const char* host_name,
                                         const char* entity_name,
                                         CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);
               
               
    /**
    * Purpose:
    * <p>
    * Gets the status of the specified application entity.
    *
    * @param application_name the name of the application.
    * @param entity_name the name of the entity.
    * @param entity_info the returned additional information.
    * @return the status of the entity.
    * @exception ApplicationNotFound if application has not been found
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name is NULL.
    */
    static char* Get_application_entity_status (const char* application_name,
                                                const char* entity_name,
                                                CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::ApplicationNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);



    /**
    * Purpose:
    * <p>
    * Gets the status of the specified process entity.
    *    
    * @param application_name the name of the application.
    * @param process_name the name of the process.
    * @param host_name the name of the host.
    * @param entity_name the name of the entity.
    * @param entity_info the returned additional information.
    * @return the status of the entity.
    * @exception ProcessNotFound if process has not been found
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name is NULL.
    */
    static char* Get_process_entity_status (const char* application_name,
                                            const char* process_name,
                                            const char* host_name,
                                            const char* entity_name,
                                            CORBA::String_out entity_info)
        throw (CdmwPlatformMngt::ProcessNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);
               

        
    /**
    * Purpose:
    * <p>
    * Sets the status of the specified entity of the host.
    *
    * @param host_name the name of the host.
    * @param entity_name the name of the entity.
    * @param entity_status the new status of the entity.
    * @param entity_info the additional information.
    * @exception HostNotFound if host has not been found
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name, entity_status or entity_info is NULL.
    */
    static void Set_host_entity_status (const char* host_name,
                                        const char* entity_name,
                                        const char* entity_status,
                                        const char* entity_info)
        throw (CdmwPlatformMngt::HostNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);
                          
               
    /**
    * Purpose:
    * <p>
    * Sets the status of the specified entity of the application.
    *
    * @param application_name the name of the application.
    * @param entity_name the name of the entity.
    * @param entity_status the new status of the entity.
    * @param entity_info the additional information.
    * @exception ApplicationNotFound if application has not been found
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name, entity_status or entity_info is NULL.
    */
    static void Set_application_entity_status (const char* application_name,
                                               const char* entity_name,
                                               const char* entity_status,
                                               const char* entity_info)
        throw (CdmwPlatformMngt::ApplicationNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);



    /**
    * Purpose:
    * <p>
    * Sets the status of the specified entity of the process.
    *
    * @param application_name the name of the application.
    * @param process_name the name of the process.
    * @param host_name the name of the host.
    * @param entity_name the name of the entity.
    * @param entity_status the new status of the entity.
    * @param entity_info the additional information.
    * @exception ProcessNotFound if process has not been found
    * @exception CdmwPlatformMngt::EntityNotFound if entity_name
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name, entity_status or entity_info is NULL.
    */
    static void Set_process_entity_status (const char* application_name,
                                           const char* process_name,
                                           const char* host_name,
                                           const char* entity_name,
                                           const char* entity_status,
                                           const char* entity_info)
        throw (CdmwPlatformMngt::ProcessNotFound,
               CdmwPlatformMngt::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);
               
               
    
                   
     /**
     * Purpose:
     * <p>
     * Returns the value of property associated to the specified element.
     *
     * @param element_path   the element object path.
     * @param property_name  the name of the property.
     * @param property_value the value of the property.
     *
     * @exception CosPropertyService::PropertyNotFound if property is not found
     * @exception CORBA::SystemException the CORBA system exception raised
     *     by the operation call.
     * @exception BadSequenceOrder if calling sequence is incorrect
     *        
     */
     static void Get_property_value(const char* element_path,
                                    const char* property_name,
                                    CORBA::Any* property_value)             
            throw (CosPropertyService::PropertyNotFound,
                   CORBA::SystemException, BadOrderException);
        
                    
     /**
     * Purpose:
     * <p>
     * Set the value of property associated to the specified element.
     *
     * @param element_path   the element object path.
     * @param property_name  the name of the property.
     * @param property_value the value of the property.
     *
     * @exception CosPropertyService::PropertyNotFound if property is not found
     * @exception CosPropertyService::ReadOnlyProperty if the property value change is not allowed
     * @exception CORBA::SystemException the CORBA system exception raised
     *     by the operation call.
     * @exception BadSequenceOrder if calling sequence is incorrect
     */
     static void Set_property_value(const char* element_path,
                                    const char* property_name,
                                    const CORBA::Any& property_value)             
            throw (CosPropertyService::PropertyNotFound,
                   CosPropertyService::ReadOnlyProperty,
                   CORBA::SystemException, BadOrderException);
    
    
     /**
     * Purpose:
     * <p>
     * Call command on specified Process.
     *
     * @param process_path the path of the process
     * @param command_name the name of the command
     * @param command_args the arguments of the command
     * @param response_callback the callback called after command processing
     *
     * @exception ProcessNotFound if process is not found
     * @exception CommandNotFound if command name has not been registered
     * @exception CommandArgumentInvalid if the argument value is invalid
     * @exception CommandRejected if the command is rejected
     * @exception CORBA::SystemException the CORBA system exception raised
     *     by the operation call.
     * @exception BadSequenceOrder if calling sequence is incorrect
     */
     static void Call_command (
                          const char* process_path,
                          const char* command_name,
                          const CdmwPlatformMngt::CmdParameterValues & command_args,
                          CdmwPlatformMngt::CommandResponseCallback_ptr response_callback)
            throw (CdmwPlatformMngt::ProcessNotFound,
                   CdmwPlatformMngt::CommandNotFound,
                   CdmwPlatformMngt::CommandArgumentInvalid,
                   CdmwPlatformMngt::CommandRejected,
                   CORBA::SystemException, BadOrderException);
              
     /**
     * Purpose:
     * <p>
     * Call command on specified Entity of a Process.
     *
     * @param entity_path the path of the entity
     * @param command_name the name of the command
     * @param command_args the arguments of the command
     * @param response_callback the callback called after command processing
     *
     * @exception EntityNotFound if entity is not found
     * @exception CommandNotFound if command name has not been registered
     * @exception CommandArgumentInvalid if the argument value is invalid
     * @exception CommandRejected if the command is rejected
     * @exception CORBA::SystemException the CORBA system exception raised
     *     by the operation call.
     * @exception BadSequenceOrder if calling sequence is incorrect
     */
     static void Call_entity_command(
                          const char* entity_path,
                          const char* command_name,
                          const CdmwPlatformMngt::CmdParameterValues & command_args,
                          CdmwPlatformMngt::CommandResponseCallback_ptr response_callback)
            throw (CdmwPlatformMngt::EntityNotFound,
                   CdmwPlatformMngt::CommandNotFound,
                   CdmwPlatformMngt::CommandArgumentInvalid,
                   CdmwPlatformMngt::CommandRejected,
                   CORBA::SystemException, BadOrderException);                        
    /**
    * Purpose:
    * <p>
    * Set the command callback to be associated to the command name of the process
    *
    * @param command_name the name of the command.
    * @param command_callback the callback to associate
    *
    * @exception BadParameterException if callaback is NULL.
    */
    static void Set_command_callback (const char* command_name,
                                      UserCommandCallbackBase* command_callback)
        throw (BadParameterException);


    /**
    * Purpose:
    * <p>
    * Set the command callback to be associated to the command name of the entity
    *
    * @param entity_name the name of the entity.
    * @param command_name the name of the command.
    * @param command_callback the callback to associate
    *
    * @exception BadParameterException if callaback is NULL.
    */
    static void Set_entity_command_callback(
                                   const char* entity_name,
                                   const char* command_name,
                                   UserCommandCallbackBase* command_callback)
        throw (BadParameterException);

    
private:

    enum InterfaceStatus
    {
        INITIAL,
        SETUP_PERFORMED,
        CREATION_STARTING_PERFORMED,
        CREATION_DONE_PERFORMED
    };

    /**
    * The status of the Platform Management interface.
    */
    static InterfaceStatus M_status;
    
    /**
    * This mutex protects the interface status
    */
    static OsSupport::Mutex M_status_mtx;

    /**
    * The callback provided by the Platform Management service used
    * for the acknowledgement of the process creation.
    */
    static CdmwPlatformMngt::ProcessCallback_var M_process_callback;

    /**
    * The name of the application owning the process.
    */
    static std::string M_application_name;

    /**
    * The name of the process.
    */
    static std::string M_process_name;
    
    /**
    * The configuration data of the process.
    */
    static CdmwPlatformMngt::ProcessConfigurationData M_configuration_data;

    /**
    * The message broker that forwards messages and fatal error to the Platform
    * Management service.
    */
    static CdmwPlatformMngt::ProcessMessageBroker_var M_process_message_broker;

    /**
    * The orb.

    */
    static CORBA::ORB_var M_orb;

    /**
    * Purpose:
    * <p>
    * Checks the required status.
    *
    * @param required_status the required interface status.
    * @exception BadOrderException if the status of the interface
    * is not the required status.
    */
    static void Check_status(InterfaceStatus required_status)
        throw (BadOrderException);

    /**
    * Purpose:
    * <p>
    * Checks whether the issuer is valid.
    *
    * @param issuer the issuer.
    * @exception BadParameterException if issuer is NULL or is PFMMNG.
    */
    static void Check_issuer(const char* issuer) throw (BadParameterException);

    /**
    * Purpose:
    * <p>
    * Checks whether the entity name is valid and whether the
    * acknowledgement has been performed.
    *
    * @param entity_name the name of the entity.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name or entity_info is NULL.
    */
    static void Check_entity_name (const char* entity_name)
        throw (BadOrderException, BadParameterException);
        
    /**
    * Purpose:
    * <p>
    * Checks whether the entity parameters are valid and whether the
    * acknowledgement has been performed.
    *
    * @param entity_name the name of the entity.
    * @param entity_status the status of the entity.
    * @param entity_info the additional information.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entity_name or entity_info is NULL.
    */
    static void Check_entity_status_parameters(const char* entity_name,
                                               const char* entity_status,
                                               const char* entity_info)
        throw (BadOrderException, BadParameterException);

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PLATFORMINTERFACE_HPP

