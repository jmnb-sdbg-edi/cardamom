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


#include <limits>
#include <iostream>
#include "platformsystemlibrary/Host_impl.hpp"
#include "platformsystemlibrary/System_impl.hpp"
#include "platformsystemlibrary/SystemLogMessageId.hpp"

#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"

#include "SystemMngt/platformelementlibrary/EntityContainer_impl.hpp"
#include "SystemMngt/platformelementlibrary/RWEntityContainer_impl.hpp"
#include "SystemMngt/platformelementlibrary/EntityContainer.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "platformapplicationlibrary/ProcessesIterator_impl.hpp"
#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

using namespace std;

namespace Cdmw
{
namespace PlatformMngt
{

class Host_impl;
class System_impl;



// ----------------------------------------------------------------------
// HostRespondingCallback class.
// ----------------------------------------------------------------------
HostRespondingCallback::HostRespondingCallback (Host_impl* host)
    : Callback()
{
    m_host = host;
}


HostRespondingCallback::~HostRespondingCallback ()
{
}


void HostRespondingCallback::execute()
  throw()
{
    CdmwPlatformMngtBase::TimeStamp time_stamp =
    PlatformMngt::EventHeaderFactory::createTimeStamp();
    m_host->setStatus (CdmwPlatformMngt::HOST_RESPONDING, time_stamp);
    
}

// ----------------------------------------------------------------------
// HostNotRespondingCallback class.
// ----------------------------------------------------------------------
HostNotRespondingCallback::HostNotRespondingCallback (Host_impl* host)
    : Callback()
{
    m_host = host;
}


HostNotRespondingCallback::~HostNotRespondingCallback ()
{
}


void HostNotRespondingCallback::execute()
  throw()
{
    CdmwPlatformMngtBase::TimeStamp time_stamp =
    PlatformMngt::EventHeaderFactory::createTimeStamp();
    m_host->setStatus (CdmwPlatformMngt::HOST_UNREACHABLE, time_stamp);
    
}




// ----------------------------------------------------------------------
// Host_impl class.
// ----------------------------------------------------------------------


#ifdef CDMW_USE_FAULTTOLERANCE
Host_impl::Host_impl (PortableServer::POA_ptr poa,
                      const CdmwPlatformMngt::HostDef& host_def,
                      const char* host_path,
                      System_impl* system,
                      CdmwPlatformMngt::RWElementRepository_ptr element_repository,                      
                      CdmwFT::FaultManagement::FaultDetector_ptr faultDetector)
#else
Host_impl::Host_impl (PortableServer::POA_ptr poa,
                      const CdmwPlatformMngt::HostDef& host_def,
                      const char* host_path,
                      System_impl* system,
                      CdmwPlatformMngt::RWElementRepository_ptr element_repository)
#endif
    throw (BadParameterException,
           OutOfMemoryException,
   	   CosPropertyService::MultipleExceptions)
    : DeactivableServant_impl (poa),
      ManagedElement_impl (element_repository)
                             
{
    m_hostEntityStatusChangeFactory = NULL;
    m_actualEntityContainer = NULL;
    
    try
    { 
        if (system == NULL)
        {
            CDMW_THROW2 (BadParameterException, "system", "NULL");
        }
        
        // Set the internal data
        m_system = system; 
        m_monitor = NULL;
        
        // create the host entity status change factory
        std::auto_ptr <HostEntityStatusChangeFactory> hostEntityStatusChangeFactory(
            new HostEntityStatusChangeFactory());

        try
        {
            Cdmw::CommonSvcs::DataStore::DataStoreBase* ds =
               Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(
                   Configuration::Get_host_datastore_id());
            m_host_ds = 
               dynamic_cast<Cdmw::PlatformMngt::HostDataStore*>(ds);
        }
        catch (const NotExistException& e)
        {
            CDMW_ASSERT(false);
        }
       
        
	    // store element path in managed element
	    set_element_path (host_path,
	                      host_def.host_name.in());
	                      
	    // create entity container               
        std::auto_ptr <PlatformMngt::EntityContainer> actualEntityContainer (
            new PlatformMngt::EntityContainer(poa, m_element_repository.in(),
                                              m_element_path.in()));
                                              
        m_rwEntityContainerServant = new RWEntityContainer_impl (
                    poa, actualEntityContainer.get(), m_element_repository.in());
                    
        m_rwEntityContainer
            = dynamic_cast <RWEntityContainer_impl*> (m_rwEntityContainerServant.in());
                                              
        // Complete the internal data
        m_hostEntityStatusChangeFactory = hostEntityStatusChangeFactory.get();
        m_actualEntityContainer = actualEntityContainer.get();
	                      
	    // initialise data store of the object
        create_record_in_datastore(host_def);


#ifdef CDMW_USE_FAULTTOLERANCE
        m_faultDetector = CdmwFT::FaultManagement::FaultDetector::_duplicate(faultDetector);

        m_monitoringID = 0;        
#else
        // create Daemon PullMonitor
        std::auto_ptr <DaemonMonitor> 
           monitor(new DaemonMonitor(   
                           m_system, 
                           m_element_name.in(),                   
                           host_def.daemon_monitoring_info.monitoring_interval,
                           host_def.daemon_monitoring_info.monitoring_timeout));
        
        
        // create Callbacks                        
        std::auto_ptr <HostRespondingCallback>  responseCallback (
                                new HostRespondingCallback (this));
                                
        std::auto_ptr <HostNotRespondingCallback>  noResponseCallback (
                                new HostNotRespondingCallback (this));
    
                                
        m_monitor = monitor.release();
        
        // set callbacks in monitor
        m_monitor->setResponseCallback (responseCallback.release());
        m_monitor->setNoResponseCallback (noResponseCallback.release());
#endif

        // Release the ownership of the temporary objects
        hostEntityStatusChangeFactory.release();
        actualEntityContainer.release();
    
	//Add the Properties on Host                                                                                                                                       
	unsigned int propCount = host_def.properties.length();
	for (unsigned int propIndex=0 ; propIndex < propCount ; propIndex++)
        {
            const CdmwPlatformMngt::PropertyDef& prop_def =
                                      host_def.properties[propIndex];
            try
            {
                std::string propertyName(host_def.properties[propIndex].property_name);
                CosPropertyService::PropertyModeType propertyMode = CosPropertyService::PropertyModeType(0);
                CdmwPlatformMngt::PropertyModeType propertyModeCdmw = host_def.properties[propIndex].mode_type;
                if (propertyModeCdmw == 1) propertyMode = CosPropertyService::PropertyModeType(1);
                define_property_with_mode(propertyName.c_str() , prop_def.initial_value, propertyMode);
  	    }
            catch (const CosPropertyService::InvalidPropertyName&)
                {
                        throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
                }
            catch (const CosPropertyService::ConflictingProperty&)
                {
                        throw  CosPropertyService::MultipleExceptions();/* TO-DO */
                }
            catch (const CosPropertyService::UnsupportedTypeCode&)
                {
                        throw  CosPropertyService::MultipleExceptions();/* TO-DO */
                }
            catch (const CosPropertyService::UnsupportedProperty&)
                {
                        throw  CosPropertyService::MultipleExceptions();/* TO-DO */
                }
            catch (const CosPropertyService::UnsupportedMode&)
                {
                        throw  CosPropertyService::MultipleExceptions();/* TO-DO */
                }
            catch (const CosPropertyService::ReadOnlyProperty&)
                {
                        throw  CosPropertyService::MultipleExceptions();/* TO-DO */
                }
	}
    }
    catch (const std::bad_alloc&)
    {
        if (m_actualEntityContainer != NULL)
            delete m_actualEntityContainer;
            
        CDMW_THROW (OutOfMemoryException);
    } 

}



Host_impl::~Host_impl ()
{  
 
#ifndef CDMW_USE_FAULTTOLERANCE
    if (m_monitor != NULL)
    {
        // terminate pull monitor thread if not stopped
        m_monitor->stop();
        
        delete m_monitor;
    }
#endif

    if (m_hostEntityStatusChangeFactory != NULL)
    {
        m_hostEntityStatusChangeFactory->_remove_ref();
    }
    
    if (m_actualEntityContainer != NULL)
    {
    	delete m_actualEntityContainer;
    }

	remove_record_in_datastore();
}


void Host_impl::end_creation(const CdmwPlatformMngt::HostDef& host_def)
       throw (CdmwPlatformMngt::DuplicateEntity,
              OutOfMemoryException,
              CORBA::SystemException)
{
    // Add the entities of the process
	// if duplicated throw DuplicateEntity exception
	unsigned int entityCount = host_def.entities.length();
	
    for (unsigned int entityIndex=0;
         entityIndex < entityCount;
         entityIndex++)
    {  
    	// get entity definition (EntityDef is a structure, a reference on the
    	// structure is returned by the [] operator)
    	const CdmwPlatformMngt::EntityDef& entity_def = host_def.entities[entityIndex];
                                         
        try
        {
            m_rwEntityContainer->add_entity(entity_def);
        }
        catch(const CdmwPlatformMngt::EntityAlreadyExists&)
        {
        	// in this case the servant will be deleted by application
            throw CdmwPlatformMngt::DuplicateEntity (entity_def.entity_name.in());
        }
    }
}


void 
Host_impl::create_record_in_datastore(const CdmwPlatformMngt::HostDef& host_def)
    throw (OutOfMemoryException)
{
    // test if the supervision is not fault tolerant or if it is, is it the primary?
    if (!Configuration::Is_fault_tolerant() || Configuration::Is_primary())
    {
        // set managed element
	    set_element_in_datastore();
	
        CdmwPlatformMngt::HostRecord record;
        record.status = CdmwPlatformMngt::HOST_RESPONDING;
        record.platform_application_exe = host_def.platform_application_exe;
        record.daemon_monitoring_info = host_def.daemon_monitoring_info;
        
        try
        {
            m_host_ds->insert(m_element_path.in(), record);
        }
        catch (Common::AlreadyExistException&)
        {
            std::cout << "host record already exists in datastore : " 
                      << m_element_path.in() << std::endl;
            CDMW_ASSERT(false);
        }  
    }
}

void 
Host_impl::remove_record_in_datastore()
{
   // inhibition if backup
   if (!Configuration::Is_fault_tolerant() || Configuration::Is_primary())
   {
       try
       {
           m_host_ds->remove(m_element_path.in());
       }
       catch (Common::NotFoundException&)
       {
       }
   }
}

CdmwPlatformMngt::HostRecord* Host_impl::get_record_from_datastore()
{
    CdmwPlatformMngt::HostRecord* host_record = NULL;
    
    try
    {
        host_record = m_host_ds->select(m_element_path.in());
    }
    catch (Common::NotFoundException&)
    {
        std::cout << "host record cannot be found in datastore : " 
                  << m_element_path.in() << std::endl;
        CDMW_ASSERT(false);
    }   
    
    return host_record; 
}

    

void Host_impl::update_record_in_datastore(const CdmwPlatformMngt::HostRecord& host_record)
{
    try
    {
        m_host_ds->update(m_element_path.in(), host_record);
    }
    catch (Common::NotFoundException&)
    {
        std::cout << "host record cannot be found in datastore : " 
                  << m_element_path.in() << std::endl;
        CDMW_ASSERT(false);
    } 
}




CdmwPlatformMngt::HostStatus Host_impl::status()
throw( CORBA::SystemException )
{
    CdmwPlatformMngt::HostRecord_var host_record = get_record_from_datastore();
    
    return host_record->status;
}

char * Host_impl::host_status_info() throw( CORBA::SystemException )
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    
}
    
CdmwPlatformMngt::ManagedElement_ptr Host_impl::get_parent() throw (CORBA::SystemException)
{

    return m_system->_this() ;
}
    
    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void Host_impl::get_attribute(const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
        {
        char flag_found=0;
        std::string tmp1(attr_name);
        /*active the right methods*/

        if (flag == 0)
        { /*case of stardard attribute*/
        if ( tmp1 == "status" )
           {
           flag_found=1;
	   CORBA::Any_var tempany = new CORBA::Any;
           CdmwPlatformMngt::HostStatus status_out;
           status_out = status() ; 
	   tempany.inout() <<= status_out;
	   ret_attr = tempany._retn();
           }
 
        if (flag_found==0)
            throw CdmwPlatformMngt::AttributeNotFound(); 
        }
       else
        { /*case of user defined attribute*/
           try
           {       
           CORBA::Any* outany = get_property_value( attr_name );

           CORBA::Any_var tempany = new CORBA::Any;

           ret_attr = outany; 
           }
           catch ( CosPropertyService::PropertyNotFound )
             {
             throw CdmwPlatformMngt::AttributeNotFound();
             }       
        }
      }

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void Host_impl::set_attribute(const char * attr_name, const CORBA::Any & attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
        {
        if ( flag == 0)
	 { /*no attribute to set at the moment*/
           throw CdmwPlatformMngt::AttributeNotFound();
	 }
	else
         {
         /* case of user defined attributes*/
         define_property(attr_name, attr);  
         }
        }


CdmwPlatformMngt::Processes * Host_impl::get_processes ( CORBA::ULong how_many, CdmwPlatformMngt::ProcessesIterator_out processes_iterator)
throw (CORBA::SystemException)
{

  CdmwPlatformMngt::Application_var  app;

  CORBA::ULong how_many_apps; //number of applications
                         //in this system
  CORBA::ULong i;

  CdmwPlatformMngt::Applications * apps ;

  CdmwPlatformMngt::ApplicationsIterator_var app_iter;

  //numeric_limits <CORBA::ULong>  big_number;
  CORBA::ULong max_ulong;
  //max_ulong = big_number.max();  

  max_ulong = m_system->get_number_of_applications();
  apps = m_system->get_applications(max_ulong, app_iter) ;

  //number of applications
  how_many_apps = apps->length() ;

  CdmwPlatformMngt::Processes*  processes;

  CORBA::ULong j,k;
  CdmwPlatformMngt::Host_ptr actual_host;
  CORBA::ULong size_seq;

  CdmwPlatformMngt::ProcessesIterator_var process_iter;

  CdmwPlatformMngt::Processes * seq_processes_out = new CdmwPlatformMngt::Processes();

  std::list<CdmwPlatformMngt::Process_var> process_remaining;

//use a cycle with the single get_application
  k=0;
  for (i=0 ; i<how_many_apps; i++)
    {

    max_ulong = (*apps)[i]->get_number_of_processes();
    processes = (*apps)[i]->get_processes( max_ulong, process_iter) ;

    size_seq = processes->length();


    for (j=0; j<size_seq; j++)
      {
       //use the get_host to know if it is us desired host
      actual_host = ((*processes)[j])->get_host();

      if ( actual_host->_is_equivalent(this->_this() ) )
        {
	if ( k <= (how_many-1) )
           {
           seq_processes_out->length(k+1);
           (*seq_processes_out)[k] =  (*processes)[j] ;
	   }
        else
	  {
          /*put the processes to the iterator out ... */
          process_remaining.push_back ( (*processes)[j] );

	  }
        k++;
        }
       }

    }

  CdmwPlatformMngt::ProcessesIterator_var process_remaining_iter = ProcessesIterator_impl::create( process_remaining) ;

  processes_iterator = process_remaining_iter._retn() ;

  return (seq_processes_out) ;

}
 


CdmwPlatformMngt::CompleteMonitoringInfo Host_impl::get_monitoring_parameters() throw (CORBA::SystemException)
  {
  throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
  }

void Host_impl::set_monitoring_parameters(const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_parameters) throw (CORBA::SystemException)
  {
  throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
  }

CORBA::Boolean Host_impl::get_global_supervision_indicator() throw (CORBA::SystemException)
  {
  throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
  }

char * Host_impl::get_host_name() throw (CORBA::SystemException)
  {
  throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
  }

 //void Host_impl::set_global_supervision_indicator(const CORBA::Boolean & global_supervision_indicator) throw (CORBA::SystemException)
 void Host_impl::set_global_supervision_indicator(bool global_supervision_indicator) throw (CORBA::SystemException)
  {
   throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
  } 



 CdmwPlatformMngt::Process_ptr Host_impl::get_process(const char *process_name, const char *application_name ) throw (CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException) 
   {
   CdmwPlatformMngt::Application_var  app;
   app = m_system->get_application(application_name);


   CdmwPlatformMngt::Host_ptr actual_host;

   char flag_found=0; //flag to indicate if the desired process was found
    CdmwPlatformMngt::Process_ptr myprocess;

    CdmwPlatformMngt::ProcessesIterator_var proc_iter;

    myprocess = app->get_process( process_name, "" );

    actual_host = myprocess->get_host() ;

    if ( actual_host->_is_equivalent(this->_this() ) )
        {
        flag_found=1;
        //break;
        }

    if (flag_found == 0)
      {
      //cout <<"Desired host not found"<<endl;
      //process not found
      throw CdmwPlatformMngt::ProcessNotFound();
      }


    //until here it means that the functions has a success  
    return CdmwPlatformMngt::Process::_duplicate(myprocess) ;
  }


            
        
void Host_impl::shutdown ()
throw (CORBA::SystemException)
{
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


 
void Host_impl::cold_reboot ()
throw (CORBA::SystemException)
{
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

               
void Host_impl::hot_reboot ()
throw (CORBA::SystemException)
{
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

    

char* Host_impl::platform_application_exe()
throw( CORBA::SystemException )
{
    CdmwPlatformMngt::HostRecord_var host_record = get_record_from_datastore();
    
    // Note : assignment to a const char* return by .in() leads to make a copy of the string.
    CORBA::String_var exe = (host_record->platform_application_exe).in();
    
    return exe._retn();
}
    

void Host_impl::setStatus (CdmwPlatformMngt::HostStatus status, CdmwPlatformMngtBase::TimeStamp time_stamp)
{
    CdmwPlatformMngt::HostRecord_var host_record;
    CdmwPlatformMngt::HostStatus current_status;
    
    {
        // lock for concurent access
        CDMW_MUTEX_GUARD(m_mutex_host_record);
    
        // get current status
        host_record = get_record_from_datastore();
        current_status = host_record->status;
    
        // if status change
        if (status != current_status)
        {
            // store new status
            host_record->status = status;
            update_record_in_datastore(host_record.in());
        }
    }
    
    // if status has been changed
    if (status != current_status)
    {
        // notify status change
        m_system->notifyHostStatusChangeEvent (
                               get_element_name(),
                               status, time_stamp);
               
        if (status == CdmwPlatformMngt::HOST_UNREACHABLE)
        {
            // update applications of system with unavailable host name
            m_system->updateApplications (get_element_name());
        }
    }
}


void Host_impl::start()
{
    // start pull monitor thread
#ifdef CDMW_USE_FAULTTOLERANCE
    ::FT::Location location;
    location.length(1);
    location[0].kind = "hostname";
    location[0].id = get_element_name();
    
    ::FT::FaultMonitoringIntervalAndTimeoutValue time;
    time.monitoring_interval=0;
    time.timeout=0;    
      
    m_monitoringID = m_faultDetector->start_monitoring(::FT::PullMonitorable::_nil(),
                                                       location,
                                                       time);
#else
    m_monitor->start();
#endif
}
    
    

void Host_impl::stop()
{
    // terminate pull monitor thread
#ifdef CDMW_USE_FAULTTOLERANCE
    m_faultDetector->stop_monitoring(m_monitoringID);    
#else
    m_monitor->stop();
#endif
}


void Host_impl::notifyEntityStatusChangeEvent(
    const char* entity_name,
    const char* entity_status,
    const char* entity_info )
{
    try
    {
        CdmwPlatformMngtBase::EventHeader header
            = EventHeaderFactory::createHeader(CdmwPlatformMngtBase::INF);

        CdmwPlatformMngt::HostEntityStatusChange_var event
            = m_hostEntityStatusChangeFactory->create(
                header,
                m_system->get_element_name(),
                m_element_name.in(),
                entity_name,
                entity_status,
                entity_info);

        CORBA::String_var eventStr = event->to_string();
        LogMngr::logMessage(eventStr.in());

        m_system->get_event_notifier()->addEvent(event);
        event._retn();
    }
    catch(const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(...)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

CORBA::ULong Host_impl::get_number_of_entities()
throw(CORBA::SystemException)
{
    return m_rwEntityContainer->get_number_of_entities();
}

void Host_impl::get_all_entity_names(
    CORBA::ULong how_many,
    CdmwPlatformMngt::ElementNames_out entity_names,
    CdmwPlatformMngt::ElementNamesIterator_out rest)
throw(CORBA::SystemException)
{
    m_rwEntityContainer->get_all_entity_names(
        how_many, entity_names, rest);
}

CdmwPlatformMngt::Entity_ptr Host_impl::get_entity (
    const char* entity_name)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    return m_rwEntityContainer->get_entity(entity_name);
}

CORBA::Boolean Host_impl::get_entities(
    const CdmwPlatformMngt::ElementNames& entity_names,
    CdmwPlatformMngt::Entities_out nentities)
throw(CORBA::SystemException)
{
    return m_rwEntityContainer->get_entities(
        entity_names, nentities);
}

void Host_impl::get_all_entities(
    CORBA::ULong how_many,
    CdmwPlatformMngt::Entities_out nentities,
    CdmwPlatformMngt::EntitiesIterator_out rest)
throw(CORBA::SystemException)
{
    m_rwEntityContainer->get_all_entities(
        how_many, nentities, rest);
}

    
CdmwPlatformMngt::Entity_ptr Host_impl::add_entity (
     const CdmwPlatformMngt::EntityDef & entity_def)
throw(CdmwPlatformMngt::EntityAlreadyExists,
      CORBA::SystemException)
{
    std::string entityName = entity_def.entity_name.in();
    
    LogMngr::logMessage(INF, MSG_ID_ADD_HOST_ENTITY, 
                        entityName.c_str(), m_element_name.in());
    
    return m_rwEntityContainer->add_entity(entity_def);

    // TODO: Notify the configuration change
}

void Host_impl::remove_entity(
    const char* entity_name)
throw (CdmwPlatformMngt::EntityNotFound,
       CORBA::SystemException)
{
    LogMngr::logMessage(INF, MSG_ID_REMOVE_SYSTEM_ENTITY, 
                        entity_name, m_element_name.in());
    
    m_rwEntityContainer->remove_entity(entity_name);

    // TODO: Notify the configuration change
}


} // End namespace PlatformMngt
} // End namespace Cdmw


