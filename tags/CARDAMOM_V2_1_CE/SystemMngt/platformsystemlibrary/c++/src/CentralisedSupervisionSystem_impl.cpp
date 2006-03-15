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

#include "platformsystemlibrary/CentralisedSupervisionSystem_impl.hpp"
#include "platformsystemlibrary/CentralisedSupervisionObserver_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtServiceBroker.stub.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"


namespace Cdmw
{
namespace PlatformMngt
{



// ----------------------------------------------------------------------
// CentralisedSupervisionSystem_impl class.
// ----------------------------------------------------------------------
CentralisedSupervisionSystem_impl::CentralisedSupervisionSystem_impl (
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
    m_supervisionObserver = new CentralisedSupervisionObserver_impl ();				             
    // create an object var to take pointer ownership
    PortableServer::ServantBase_var supervisionObserverServant = m_supervisionObserver;        
    // activate object
    m_supervisionObserverObject = m_supervisionObserver->_this();
    
    // clear application servant map
    m_application_servants.clear();
}



CentralisedSupervisionSystem_impl::~CentralisedSupervisionSystem_impl()
{
 
}




CdmwPlatformMngt::Application_ptr 
CentralisedSupervisionSystem_impl::doAddApplication(
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
        // Create the application servant
        // ------------------------------
        
        // build the application path        
        std::string application_path = 
                  ManagedElementKey::Get_application_path (
                                         m_element_name.in(), applicationName);
                                         
                                            
#ifdef CDMW_USE_FAULTTOLERANCE
        Application_impl* p_application = 
				             new Application_impl (m_poa.in(), m_eventNotifier,
				                                   m_element_repository.in(),
				                                   m_replicationManagerIOGR, 
				                                   m_FTConverterObject.in());
#else
        Application_impl* p_application = 
				             new Application_impl (m_poa.in(), m_eventNotifier,
				                                   m_element_repository.in());
#endif	    

        // set application ack data
        CdmwPlatformMngt::ApplicationStartingData_var applicationStartData =
                             new CdmwPlatformMngt::ApplicationStartingData();
        
        applicationStartData->system_name = m_element_name;
        applicationStartData->system_port = m_systemPort;
        applicationStartData->application_name = platform_application_def.application_name;
        applicationStartData->host_container = m_hostContainerObject;
        applicationStartData->supervision_observer = m_supervisionObserverObject;
        applicationStartData->application_observer = m_applicationObserverObject;
        applicationStartData->service_binding = m_serviceBindingObject;
        applicationStartData->system_service_def_container = m_serviceDefContainerObject;

#ifdef CDMW_USE_FAULTTOLERANCE
        
        applicationStartData->process_observer = m_FTProcessObserverGroup;
        applicationStartData->service_broker = m_FTServiceBrokerGroup;
        
#else
        applicationStartData->process_observer = 
           CdmwPlatformMngt::ProcessObserver::_nil();
        applicationStartData->service_broker = 
           CdmwPlatformMngtService::ServiceBroker::_nil();
#endif
        
               
        // process end of application creation
        p_application->end_creation (applicationStartData.in(), application_path.c_str());

        // create an object var to take pointer ownership
        PortableServer::ServantBase_var applicationServant = p_application;
        
        
        // Create the application control servant
        // --------------------------------------
        ApplicationControl_impl* p_applicationControl = 
				             new ApplicationControl_impl (m_poa.in(), p_application);
				             
	    // create an object var to take pointer ownership
        PortableServer::ServantBase_var applicationControlServant = p_applicationControl;
				     
#ifdef CDMW_USE_FAULTTOLERANCE

        // set global service
        // ----------------
        CdmwPlatformMngtService::ServiceBroker_var serviceBroker =
                p_application->get_service_broker();
                
                
        serviceBroker->set_global_service(
                       "ReplicationManager", m_replicationMngt.in());

#endif

	    // activate objects
	    // ----------------
	    p_application->activate();
        CdmwPlatformMngt::Application_var  applicationObject = p_application->_this();
        
        p_applicationControl->activate();
        CdmwPlatformMngt::ApplicationControl_var  applicationControlObject = 
                                                   p_applicationControl->_this();
        
        
        // get process observer for the application
        CdmwPlatformMngt::ProcessObserver_var processObserver =
                                                   p_application->get_process_observer();
        
        // Add them and servants into the applications maps
        {
            CDMW_WRITER_LOCK_GUARD (m_rwSystemLock);
            ApplicationRef applicationRef (applicationObject.in(), 
                                           applicationControlObject.in(),
                                           processObserver.in());
            m_applications.insert (Applications::value_type (applicationName,
                                                             applicationRef));
            
            ApplicationServantSet app_servant_set;
            app_servant_set.application = p_application;
            app_servant_set.application_control = p_applicationControl;
                                                             
            m_application_servants.insert (ApplicationServants::value_type (
                                                              applicationName,            
                                                              app_servant_set));
        }
        
        try
        {
        	// add in repository the application object
            m_element_repository->add_element (application_path.c_str(),
                                               CdmwPlatformMngt::APPLICATION_TYPE,
                                               applicationObject.in());
        }
        catch(const CdmwPlatformMngt::ElementPathInvalid)
        {
       	   throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const CdmwPlatformMngt::ElementAlreadyExists)
        {
       	   throw CdmwPlatformMngt::ApplicationAlreadyExists();
        }
      
        try
        {
            // Call Application define to create processes
            applicationObject->define (application_def);
        }
        catch (const CdmwPlatformMngt::AlreadyDone &)
        {
            throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
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


void CentralisedSupervisionSystem_impl::doRemoveApplication (const char* application_name)
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
        	CdmwPlatformMngt::Application_var  applicationObject;
        	
            CDMW_WRITER_LOCK_GUARD (m_rwSystemLock);
            
            // get application
            applicationIt = m_applications.find (application_name);
            applicationObject = (applicationIt->second).m_application;
            
            // remove from repository the application object
            CORBA::String_var application_path = applicationObject->element_path();
            m_element_repository->remove_element (application_path.in());
            
            // request to control servant to destroy application
            applicationControlObject->destroy_application();
    
            // then erase object from the map : object is destroyed by orb if its
            // reference counter is 0        
           
            m_applications.erase(applicationIt);
            
            // remove the application servant from map
            ApplicationServants::iterator applicationServantIt = 
                           m_application_servants.find (application_name);
                           
            if (applicationServantIt != m_application_servants.end())
            {
                m_application_servants.erase(applicationServantIt);
            }
        }          
    }
    catch(const CdmwPlatformMngt::ElementPathInvalid)
    {
       	throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const CdmwPlatformMngt::ManagedElementNotFound)
    {
        throw CdmwPlatformMngt::ApplicationNotFound();
    }  
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}

void CentralisedSupervisionSystem_impl::getApplicationServants (
                                           const char* application_name,
                                           Application_impl*& application_servant,
                                           ApplicationControl_impl*& app_control_servant)
throw (CdmwPlatformMngt::ApplicationNotFound)
{
	application_servant = 0;
	app_control_servant = 0;
	
	try
	{	    
	    CDMW_READER_LOCK_GUARD(m_rwSystemLock);

        ApplicationServants::iterator applicationServantIt = 
                                      m_application_servants.find(application_name);
        if (applicationServantIt == m_application_servants.end())
        {
            throw CdmwPlatformMngt::ApplicationNotFound();
        }

        application_servant = (applicationServantIt->second).application;
        app_control_servant = (applicationServantIt->second).application_control;	    
    }
    catch (...)
    {
    	throw CdmwPlatformMngt::ApplicationNotFound();
    }
}

void CentralisedSupervisionSystem_impl::synchro_applications_with_datastore()
{
    ApplicationServants::iterator applicationIt;
    
    for (applicationIt = m_application_servants.begin();
         applicationIt != m_application_servants.end();
         applicationIt++)
    {
        Application_impl* application = (applicationIt->second).application;
        
        // synchro the application
        application->synchro_with_datastore();
        
        // synchro the processes of the application
        application->synchro_processes_with_datastore();
    }
}

void CentralisedSupervisionSystem_impl::synchro_processing_of_applications()
{
    ApplicationServants::iterator applicationIt;
    
    for (applicationIt = m_application_servants.begin();
         applicationIt != m_application_servants.end();
         applicationIt++)
    {
        Application_impl* application = (applicationIt->second).application;
        
        // synchro the application
        application->synchro_processing();
    }
}

void CentralisedSupervisionSystem_impl::synchro_processing_of_processes()
{
    ApplicationServants::iterator applicationIt;
    
    for (applicationIt = m_application_servants.begin();
         applicationIt != m_application_servants.end();
         applicationIt++)
    {
        Application_impl* application = (applicationIt->second).application;
        
        // synchro the processes of the application
        application->synchro_processing_of_processes();
    }
}

void CentralisedSupervisionSystem_impl::freeze_event_handlers()
{
    // freeze the event handler of system
    get_event_handler()->freeze_handler();
    
    // freeze the event handler of each application
    ApplicationServants::iterator applicationIt;
    
    for (applicationIt = m_application_servants.begin();
         applicationIt != m_application_servants.end();
         applicationIt++)
    {
        Application_impl* application = (applicationIt->second).application;
        
        application->get_event_handler()->freeze_handler();
    }
}

void CentralisedSupervisionSystem_impl::unfreeze_event_handlers()
{
    // unfreeze the event handler of system
    get_event_handler()->unfreeze_handler();
    
    // unfreeze the event handler of each application
    ApplicationServants::iterator applicationIt;
    
    for (applicationIt = m_application_servants.begin();
         applicationIt != m_application_servants.end();
         applicationIt++)
    {
        Application_impl* application = (applicationIt->second).application;
        
        application->get_event_handler()->unfreeze_handler();
    }
}



} // End namespace PlatformMngt
} // End namespace Cdmw

