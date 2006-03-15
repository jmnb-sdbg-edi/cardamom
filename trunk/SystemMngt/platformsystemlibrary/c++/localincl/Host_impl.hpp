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


#ifndef INCL_PLATFORMMNGT_HOST_IMPL_HPP
#define INCL_PLATFORMMNGT_HOST_IMPL_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "SystemMngt/platformlibrary/Callback.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformelementlibrary/ManagedElement_impl.hpp"
#include "SystemMngt/platformelementlibrary/EntityContainer.hpp"
#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtHost.skel.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "platformsystemlibrary/DaemonMonitor.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "FaultTolerance/idllib/CdmwFTFaultDetector.stub.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#endif

namespace Cdmw
{
namespace PlatformMngt
{

class Host_impl;
class System_impl;
class RWEntityContainer_impl;
class HostEntityStatusChangeFactory;

/**
 * The callback for the responding host.
 */
class HostRespondingCallback : public virtual Callback
{
    

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  host  the host associated to callback.
     */
    HostRespondingCallback (Host_impl* host);
        

    /**
     *Purpose:
     *<p> Destructor.
     */
    ~HostRespondingCallback ();
        
    /**
     *Purpose:
     *<p> Callback execution. notifies the host is responding
     */
    void execute() throw();
 
    
private:

    /**
     * The host.
     */
    Host_impl* m_host;

};



/**
 * The callback for the not responding host.
 */
class HostNotRespondingCallback : public virtual Callback
{
    

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param  host  the host associated to callback.
     */
    HostNotRespondingCallback (Host_impl* host);
        
   
    /**
     *Purpose:
     *<p> Destructor.
     */
    ~HostNotRespondingCallback ();
    
    
    /**
     *Purpose:
     *<p> Callback execution. notifies the host is not responding
     */
    void execute() throw();
 
    
private:

    /**
     * The host.
     */
    Host_impl* m_host;

};



/**
 *Purpose:
 *<p> Provides the host monitoring.
 */
class Host_impl : virtual public POA_CdmwPlatformMngt::Host,
                  public DeactivableServant_impl,
                  public ManagedElement_impl                  
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     * 
     *@param poa       the poa in charge of servants.
     *@param host_def  the host definition.
     *@param host_path the path of the Host.
     *@param system    the system responsible for the host.        
     *@param element_repository the element repository   
     *
     *@exception BadParameterException
     *@exception OutOfMemoryException
     *
     */
#ifdef CDMW_USE_FAULTTOLERANCE
    Host_impl (PortableServer::POA_ptr poa,
               const CdmwPlatformMngt::HostDef& host_def,
               const char* host_path,               
               System_impl* system,
               CdmwPlatformMngt::RWElementRepository_ptr element_repository,
               CdmwFT::FaultManagement::FaultDetector_ptr faultDetector)
#else
    Host_impl (PortableServer::POA_ptr poa,
               const CdmwPlatformMngt::HostDef& host_def,
               const char* host_path,
               System_impl* system,
               CdmwPlatformMngt::RWElementRepository_ptr element_repository)      
#endif
          throw (BadParameterException,
                 OutOfMemoryException,
 		 CosPropertyService::MultipleExceptions
		);
                    



    /**
     *Purpose:
     *<p> Destructor.
     */
    ~Host_impl();
    
    
    /**
     *Purpose:
     *<p> Ends the creation of the host servant
     *
     *@exception DuplicateEntity if there is a duplicate entity name among
     * the entities of the process
     */
    void end_creation(const CdmwPlatformMngt::HostDef& host_def)
            throw (CdmwPlatformMngt::DuplicateEntity,
                   OutOfMemoryException,
                   CORBA::SystemException);
    
    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void get_attribute(const char * attr_name, CORBA::Any_out ret_attr , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */
     void set_attribute(const char * attr_name, const CORBA::Any & attr , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);
            
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Host/status:1.0
     * operation
     */
    CdmwPlatformMngt::HostStatus status()
             throw (CORBA::SystemException);

    /**
     * to get the host_status_info attribute : added in Selex
     */
    char * host_status_info()
             throw (CORBA::SystemException);

    //get_monitoring_parameters()
    CdmwPlatformMngt::CompleteMonitoringInfo get_monitoring_parameters() throw (CORBA::SystemException);    

    //set_monitoring_parameters()
    void set_monitoring_parameters(const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_parameters) throw (CORBA::SystemException);

    //get_global_supervision_indicator()
    CORBA::Boolean get_global_supervision_indicator() throw (CORBA::SystemException);    

    //set_global_supervision_indicator method
//    void set_global_supervision_indicator(const CORBA::Boolean & global_supervision_indicator) throw (CORBA::SystemException);
    virtual void set_global_supervision_indicator(bool ) throw (CORBA::SystemException);

    //get_host_name
    char * get_host_name() throw (CORBA::SystemException);    
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Host/platform_application_exe:1.0
     * operation
     */
    char* platform_application_exe()
             throw (CORBA::SystemException);
     /**
     *Purpose:
     *<p>
     * Implements the
     * get_parent() function from Host to System
     * required for navigability
     */
     CdmwPlatformMngt::ManagedElement_ptr  get_parent()  throw (CORBA::SystemException);
 
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Host/get_processes:1.0
     * operation
     */
    CdmwPlatformMngt::Processes * get_processes (
        CORBA::ULong how_many,
        CdmwPlatformMngt::ProcessesIterator_out processes_iterator)
              throw (CORBA::SystemException);

     /**
     *Purpose:
     *<p>
     * Implements the
     * get_process(const char * , const char *) function from Host to Process
     * required for navigability
     */

     CdmwPlatformMngt::Process_ptr get_process(const char *process_name, const char *application_name) 
              throw (CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * getprocesses() function from Host to Process
     * required for navigability
     */

     CdmwPlatformMngt::Processes * getprocesses() 
              throw (CORBA::SystemException);
 
   
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Host/shutdown:1.0
     * operation
     */        
    void shutdown ()
         throw (CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Host/cold_reboot:1.0
     * operation
     */ 
    void cold_reboot ()
         throw (CORBA::SystemException);
              
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Host/hot_reboot:1.0
     * operation
     */ 
    void hot_reboot ()
         throw (CORBA::SystemException);
              
              
        
    /**
     *Purpose:
     *<p> change host status
     *
     *@param  status  the new status of host.
     */
    void setStatus(CdmwPlatformMngt::HostStatus status, CdmwPlatformMngtBase::TimeStamp time_stamp);
    
    
    /**
     *Purpose:
     *<p> start the pull monitoring
     *
     */
    void start();
    
    
    /**
     *Purpose:
     *<p> stop the pull monitoring
     *
     */
    void stop();
    
    /**
     *<p> Notifies the supervision observer that an entity of this
     * host has its status changed.
     *
     *@param entity_name the name of the entity.
     *@param entity_status the status of the entity.
     *@param entity_info some additional information.
     */
    void notifyEntityStatusChangeEvent(
        const char* entity_name,
        const char* entity_status,
        const char* entity_info );
        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_number_of_entities:1.0
     * operation
     */
    CORBA::ULong get_number_of_entities()
        throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_all_entity_names:1.0
     * operation
     */
    void get_all_entity_names(
        CORBA::ULong how_many,
        CdmwPlatformMngt::ElementNames_out entity_names,
        CdmwPlatformMngt::ElementNamesIterator_out rest )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_entity:1.0
     * operation
     */
    CdmwPlatformMngt::Entity_ptr get_entity (const char* entity_name)
            throw( CdmwPlatformMngt::EntityNotFound,
                   CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_entities:1.0
     * operation
     */
    CORBA::Boolean get_entities(
        const CdmwPlatformMngt::ElementNames& entity_names,
        CdmwPlatformMngt::Entities_out nentities )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/EntityContainer/get_all_entities:1.0
     * operation
     */
    void get_all_entities(
        CORBA::ULong how_many,
        CdmwPlatformMngt::Entities_out nentities,
        CdmwPlatformMngt::EntitiesIterator_out rest )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWEntityContainer/add_entity:1.0
     * operation
     */
    CdmwPlatformMngt::Entity_ptr add_entity (
                const CdmwPlatformMngt::EntityDef & entity_def)
                throw(CdmwPlatformMngt::EntityAlreadyExists,
		              CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/RWEntityContainer/remove_entity:1.0
     * operation
     */
    void remove_entity(
        const char* entity_name )
            throw( CdmwPlatformMngt::EntityNotFound,
                   CORBA::SystemException );

 

private:

    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    Host_impl();

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    Host_impl (Host_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    Host_impl& operator= (const Host_impl& rhs);
    
    
    /**
     *Purpose:
     *<p> Create the host record in data store
     *
     *@param hostDef the data for host to start.
     *
     *@exception OutOfMemoryException Lack of memory.
     */
    void create_record_in_datastore(
                  const CdmwPlatformMngt::HostDef& hostDef)
            throw (OutOfMemoryException);
            
    /**
     *Purpose:
     *<p> Remove the host record in data store
     *
     */
    void remove_record_in_datastore();
    
    /**
     *Purpose:
     *<p> Get the host record from data store
     *
     */
    CdmwPlatformMngt::HostRecord* get_record_from_datastore();
    
    /**
     *Purpose:
     *<p> Update the host record in data store
     *
     */
    void update_record_in_datastore(const CdmwPlatformMngt::HostRecord& host_record);

    /**
     * The system responsible for the entity.
     */
    System_impl* m_system;
    
    /**
     * The Daemon Pull Monitor.
     */
    DaemonMonitor* m_monitor;
    

#ifdef CDMW_USE_FAULTTOLERANCE

    CdmwFT::FaultManagement::FaultDetector_var m_faultDetector;

    CdmwFT::FaultManagement::MonitoringId m_monitoringID; 
       
#endif

    /**
     * The RW Entity Container object var to take pointer ownership
     */
    PortableServer::ServantBase_var m_rwEntityContainerServant;
    
    /**
     * The actual entity container.
     */
    PlatformMngt::EntityContainer* m_actualEntityContainer;
    
    /**
     * The read/write entity container.
     */
    RWEntityContainer_impl* m_rwEntityContainer;
    
    /**
     * The host entity status change factory.
     */
    HostEntityStatusChangeFactory* m_hostEntityStatusChangeFactory;

    /**
     * The mutex used for changing the host record.
     */
    OsSupport::Mutex m_mutex_host_record;
    
    /**
     * The host datastore.
     */
    HostDataStore* m_host_ds;

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_HOST_IMPL_HPP

