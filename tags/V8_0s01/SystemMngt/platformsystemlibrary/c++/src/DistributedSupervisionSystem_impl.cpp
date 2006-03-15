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


#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationControl_impl.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "platformsystemlibrary/DistributedSupervisionSystem_impl.hpp"
#include "platformsystemlibrary/DistributedSupervisionObserver_impl.hpp"



namespace Cdmw
{
namespace PlatformMngt
{



// ----------------------------------------------------------------------
// DistributedSupervisionSystem_impl class.
// ----------------------------------------------------------------------
DistributedSupervisionSystem_impl::DistributedSupervisionSystem_impl (
                                   SupervisionEventNotifier* eventNotifier,
                                   CORBA::ORB_ptr orb,
                                   PortableServer::POA_ptr poa,
                                   CdmwPlatformMngt::RWElementRepository_ptr element_repository)   

          throw (BadParameterException, AlreadyDoneException,
                 UnknownStateException, AlreadyExistsException,
                 OutOfResourcesException, OutOfMemoryException)                   
          : System_impl (eventNotifier, orb, poa, element_repository)
{
    // Create the SupervisionObserver servant
    m_supervisionObserver = new DistributedSupervisionObserver_impl (eventNotifier);				             
    // create an object var to take pointer ownership
    PortableServer::ServantBase_var supervisionObserverServant = m_supervisionObserver;        
    // activate object
    m_supervisionObserverObject = m_supervisionObserver->_this();
}



DistributedSupervisionSystem_impl::~DistributedSupervisionSystem_impl()
{
    try
    {
        CdmwPlatformMngt::ApplicationControl_var  applicationControlObject;
        Applications::iterator applicationIt;
        
        // remove applications referenced in map
        for (applicationIt = m_applications.begin(); 
             applicationIt != m_applications.end(); 
             applicationIt++)
        {
            // get application control
            applicationControlObject = (applicationIt->second).m_applicationControl;
            
            // request to control servant to destroy application
            applicationControlObject->destroy_application();
        }
    }
    catch (...)
    {
    }
}




CdmwPlatformMngt::Application_ptr 
      DistributedSupervisionSystem_impl::doAddApplication(
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
               CORBA::SystemException)
{
    std::string applicationName = platform_application_def.application_name.in();
    
    {
        CDMW_READER_LOCK_GUARD (m_rwSystemLock);
    
        Applications::iterator applicationIt = m_applications.find (applicationName);
             
        if (applicationIt != m_applications.end())
        {
            throw CdmwPlatformMngt::ApplicationAlreadyExists();
        }
    }

    try
    {
        // set application ack data
        CdmwPlatformMngt::ApplicationStartingData_var applicationStartData =
                             new CdmwPlatformMngt::ApplicationStartingData();
        
        applicationStartData->system_name = m_element_name;
        applicationStartData->system_port = this->systemPort();
        applicationStartData->application_name = platform_application_def.application_name;
        applicationStartData->host_container = m_hostContainerObject;
        applicationStartData->supervision_observer = m_supervisionObserverObject;
        applicationStartData->application_observer = m_applicationObserverObject;
        applicationStartData->service_binding = m_serviceBindingObject;
        applicationStartData->system_service_def_container = m_serviceDefContainerObject;
    
    
        CdmwPlatformMngt::Application_var  applicationObject;
        CdmwPlatformMngt::ApplicationControl_var  applicationControlObject;
        
        // get process observer for the application
        CdmwPlatformMngt::ProcessObserver_var processObserver;
        
        {
            CDMW_WRITER_LOCK_GUARD (m_rwSystemLock);
            
            // Add them into the applications map
            ApplicationRef applicationRef (applicationObject.in(), 
                                           applicationControlObject.in(),
                                           processObserver.in());
                                           
            m_applications.insert (Applications::value_type (applicationName,
                                                             applicationRef));
        }
      
        // return the application object
        return applicationObject._retn();
        
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch (const AlreadyDoneException&)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch (const InternalErrorException&)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


void DistributedSupervisionSystem_impl::doRemoveApplication (const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CORBA::SystemException)
{
    try
    {
        CdmwPlatformMngt::ApplicationControl_var  applicationControlObject;
        Applications::iterator applicationIt;
        
        {
            CDMW_READER_LOCK_GUARD (m_rwSystemLock);
        
            applicationIt = m_applications.find (application_name);
            if (applicationIt == m_applications.end())
            {
                throw CdmwPlatformMngt::ApplicationNotFound();
            }

            // check if application is still active
            // it can be removed only if it is inactive
            CORBA::String_var info;
            CdmwPlatformMngt::ApplicationStatus appStatus = 
                      ((applicationIt->second).m_application)->get_status(info.out());
        
            if ((appStatus == CdmwPlatformMngt::APPLICATION_INITIALISING) ||
                (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISATION_INCOMPLETE) ||
                (appStatus == CdmwPlatformMngt::APPLICATION_INITIALISED) ||
                (appStatus == CdmwPlatformMngt::APPLICATION_RUN_REQUEST) ||
                (appStatus == CdmwPlatformMngt::APPLICATION_RUNNING) ||
                (appStatus == CdmwPlatformMngt::APPLICATION_STOPPING))
            {
                throw CdmwPlatformMngt::ApplicationStillActive();
            }            
            
            // get application control
            applicationControlObject = (applicationIt->second).m_applicationControl;
        }

        {
           CDMW_WRITER_LOCK_GUARD (m_rwSystemLock);
            
           // request to control servant to destroy application
           applicationControlObject->destroy_application();
    
           // then erase object from the map : object is destroyed by orb if its
           // reference counter is 0
        
           applicationIt = m_applications.find (application_name);
           m_applications.erase(applicationIt);
        }
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}


} // End namespace PlatformMngt
} // End namespace Cdmw

