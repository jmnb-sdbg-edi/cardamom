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


#ifndef INCL_PLATFORMMNGT_CENTRALISED_SUPERVISION_SYSTEM_IMPL_HPP
#define INCL_PLATFORMMNGT_CENTRALISED_SUPERVISION_SYSTEM_IMPL_HPP

#include "platformsystemlibrary/System_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtSupervisionObserver.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"



namespace Cdmw
{
namespace PlatformMngt
{

class Application;
class Application_impl;
class ApplicationControl_impl;
class ProcessObserver_impl;
class CentralisedSupervisionObserver_impl;

/**
 *Purpose:
 *<p>
 */
struct ApplicationServantSet                   
{
    /**
     * The application servant.
     */
    Application_impl* application;
    
    /**
     * The application control servant.
     */
    ApplicationControl_impl* application_control;
};


/**
 *Purpose:
 *<p>
 */
class CentralisedSupervisionSystem_impl : public System_impl
{


public:



    /**
     *Purpose:
     *<p> Default constructor.
     *     
     *@param eventNotifier the supervision observer to be notified of events.
     *@param orb           the orb reference
     *@param poa           the poa in charge of servants
     *@param element_repository the element repository
     *
     *@exception BadParameterException if the parameter is invalid.
     *@exception AlreadyDoneException if the state machine is already running.
     *@exception UnknownStateException if stateName doesn't refer to a known
     * state.
     *@exception AlreadyExistException if the system container already exist.
     *@exception OutOfRessourcesException Lack of ressource.
     *@exception OutOfMemoryException Lack of memory.
     */
    CentralisedSupervisionSystem_impl (SupervisionEventNotifier* eventNotifier,
                                       CORBA::ORB_ptr orb,
                                       PortableServer::POA_ptr poa,
                                       CdmwPlatformMngt::RWElementRepository_ptr element_repository)
            throw (BadParameterException,
                   AlreadyDoneException,
                   UnknownStateException,
                   AlreadyExistsException,
                   OutOfResourcesException,
                   OutOfMemoryException);

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~CentralisedSupervisionSystem_impl();


    
                   
    /**
     *Purpose:
     *<p> Actually adds the specified application.
     *
     *@param platform_application_def the definition of platform application
     *@param application_def the definition of the application.
     *
     *@return the CORBA reference of the application.
     *
     * @exception ApplicationAlreadyExists if the application name is already
     *            used within the system.
     * @exception HostNotFound if the host where the application should run
     *            is unknown.
     * @exception DuplicateEntity if there is a duplicate entity name among
     *            the application's entities
     * @exception CosPropertyService::MultipleExceptions if there is a bad property among
     *            the application's properties
     * @exception DuplicateService if there is a duplicate service name among
     *            the application's service definitions
     * @exception DuplicateProcess if there is a duplicate process name among
     *            the application's processes
     * @exception ProcessHostNotFound if an unknown host is used by one of
     *            the application's processes
     * @exception DuplicateProcessEntity if there is a duplicate entity name
     *            among the entities of one of the application processes
     * @exception ProcessPropertyExceptions if there is a bad property among
     *            the properties of one of the application processes
     * @exception DuplicateProcessService if there is a duplicate service
     *            name among the services of one of the application processes
     * @exception DuplicateProcessStep if there is a duplicate step label
     *            among the steps of one of the application processes
     * @exception DuplicateProcessCommand if there is a duplicate command name
     *            among the commands of one of the application processes        
     * @exception DuplicateProcessCommandArg if there is a duplicate command arg name
     *            among the commands of one of the application processes
     * @exception DuplicateEntityCommand if there is a duplicate command name
     *            among the commands of one of the entities of the application processes
     * @exception DuplicateEntityCommandArg if there is a duplicate command arg name
     *            among the commands of one of the entities of the application processes
     * @exception IncompatibleStatus if the status of the system doesn't
     *            allow the operation.
     */
     CdmwPlatformMngt::Application_ptr 
            doAddApplication(
                   const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                   const CdmwPlatformMngt::ApplicationDef& application_def)
            throw (CdmwPlatformMngt::ApplicationAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::ProcessPropertyExceptions,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessCommand, 
                   CdmwPlatformMngt::DuplicateProcessCommandArg,
                   CdmwPlatformMngt::DuplicateEntityCommand, 
                   CdmwPlatformMngt::DuplicateEntityCommandArg,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
    
    /**
     *Purpose:
     *<p> Actually removes the specified application.
     *
     *@param application_name the name identifying the application.
     *
     *@exception ApplicationNotFound if application_name doesn't denote
     *           an existing application.
     *@exception ApplicationStillActive if the application is still active
     */
    void doRemoveApplication (const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CORBA::SystemException);
                   
    /**
     *Purpose:
     *<p> Get the application servants.
     *
     *@param application_name the name identifying the application.
     *
     *@param application_servant the returned application servant address
     *@param app_control_servant the returned application control servant address
     *
     *@exception ApplicationNotFound if application_name doesn't denote
     *           an existing application.
     */               
    void getApplicationServants (const char* application_name,
                                 Application_impl*& application_servant,
                                 ApplicationControl_impl*& app_control_servant)
            throw (CdmwPlatformMngt::ApplicationNotFound);
            
            
    /**
     *Purpose:
     *<p> Synchronise the applications with their datastore
     *
     */
    void synchro_applications_with_datastore();
    
    /**
     *Purpose:
     *<p> Synchronise the processing of applications
     *
     */
    void synchro_processing_of_applications();
    
    /**
     *Purpose:
     *<p> Synchronise the processing of system processes
     *
     */
    void synchro_processing_of_processes();
    
    /**
     *Purpose:
     *<p> Freeze all event handlers
     *
     */
    void freeze_event_handlers();
    
    /**
     *Purpose:
     *<p> Unfreeze all event handlers
     *
     */
    void unfreeze_event_handlers();
 

private:
  
    /**
     * The system supervision observer object.
     */
    CdmwPlatformMngt::SupervisionObserver_var m_supervisionObserverObject;
    
    
    /**
     * The centralised system supervision observer servant.
     */ 
    CentralisedSupervisionObserver_impl* m_supervisionObserver;
    
    /**
     * The list of application servants managed by this system.
     */
    typedef std::map <std::string, ApplicationServantSet> ApplicationServants;
    
    ApplicationServants m_application_servants;
    
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_CENTRALISED_SUPERVISION_SYSTEM_IMPL_HPP

