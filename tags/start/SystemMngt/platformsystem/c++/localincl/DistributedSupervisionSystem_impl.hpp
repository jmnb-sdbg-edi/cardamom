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


#ifndef INCL_PLATFORMMNGT_DISTRIBUTED_SUPERVISION_SYSTEM_IMPL_HPP
#define INCL_PLATFORMMNGT_DISTRIBUTED_SUPERVISION_SYSTEM_IMPL_HPP

#include "platformsystem/System_impl.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSupervisionObserver.stub.hpp"



namespace Cdmw
{
namespace PlatformMngt
{

class Application;
class DistributedSupervisionObserver_impl;

/**
 *Purpose:
 *<p>
 */
class DistributedSupervisionSystem_impl : public System_impl
{


public:



    /**
     *Purpose:
     *<p> Default constructor.
     *     
     *@param eventNotifier the supervision observer to be notified of events.
     *@param orb           the orb reference
     *@param poa           the poa in charge of servants
     *
     *@exception BadParameterException if the parameter is invalid.
     *@exception AlreadyDoneException if the state machine is already running.
     *@exception UnknownStateException if stateName doesn't refer to a known
     * state.
     *@exception AlreadyExistException if the system container already exist.
     *@exception OutOfRessourcesException Lack of ressource.
     *@exception OutOfMemoryException Lack of memory.
     */
#ifdef CDMW_USE_FAULTTOLERANCE
    DistributedSupervisionSystem_impl (SupervisionEventNotifier* eventNotifier,
                                       CORBA::ORB_ptr orb,
                                       PortableServer::POA_ptr poa,
                                       std::string faultManCorbaloc)
            throw (BadParameterException,
                   AlreadyDoneException,
                   UnknownStateException,
                   AlreadyExistsException,
                   OutOfResourcesException,
                   OutOfMemoryException);
#else
    DistributedSupervisionSystem_impl (SupervisionEventNotifier* eventNotifier,
                                       CORBA::ORB_ptr orb,
                                       PortableServer::POA_ptr poa)
            throw (BadParameterException,
                   AlreadyDoneException,
                   UnknownStateException,
                   AlreadyExistsException,
                   OutOfResourcesException,
                   OutOfMemoryException);

#endif

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~DistributedSupervisionSystem_impl();


    
                   
    /**
     *Purpose:
     *<p> Actually adds the specified application.
     *
     *@param platform_application_def the definition of platform application
     *@param application_def          the definition of the application.
     *
     *@return the CORBA reference of the application.
     *
     * @exception ApplicationAlreadyExists if the application name is already
     *            used within the system.
     * @exception HostNotFound if the host where the application should run
     *            is unknown.
     * @exception DuplicateEntity if there is a duplicate entity name among
     *            the application's entities
     * @exception DuplicateService if there is a duplicate service name among
     *            the application's service definitions
     * @exception DuplicateProcess if there is a duplicate process name among
     *            the application's processes
     * @exception ProcessHostNotFound if an unknown host is used by one of
     *            the application's processes
     * @exception DuplicateProcessEntity if there is a duplicate entity name
     *            among the entities of one of the application processes
     * @exception DuplicateProcessService if there is a duplicate service
     *            name among the services of one of the application processes
     * @exception DuplicateProcessStep if there is a duplicate step label
     *            among the steps of one of the application processes
     * @exception DuplicateProcessActivityPoint if there is a duplicate
     *            name among the activity points of one of the application processes
     * @exception IncompatibleStatus if application status incompatible
     */
     CdmwPlatformMngt::Application_ptr 
            doAddApplication(
                   const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
                   const CdmwPlatformMngt::ApplicationDef& application_def)
            throw (CdmwPlatformMngt::ApplicationAlreadyExists,
                   CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
                   CdmwPlatformMngt::DuplicateProcessActivityPoint,
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
 

private:
  
    /**
     * The system supervision observer object.
     */
    CdmwPlatformMngt::SupervisionObserver_var m_supervisionObserverObject;
    
    
    /**
     * The centralised system supervision observer servant.
     */ 
    DistributedSupervisionObserver_impl* m_supervisionObserver;

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_DISTRIBUTED_SUPERVISION_SYSTEM_IMPL_HPP

