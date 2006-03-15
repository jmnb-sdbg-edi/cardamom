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


#ifndef INCL_FTSYSTMNGT_SYSTEM_ACCESSOR_IMPL_HPP
#define INCL_FTSYSTMNGT_SYSTEM_ACCESSOR_IMPL_HPP


#include "SystemMngt/idllib/CdmwFTSystMngtSystem.skel.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtObservable.stub.hpp"


#include "SystemMngt/platformftaccessorlibrary/ManagedElementAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/HostContainerAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/RWEntityContainerAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/RWServiceDefContainerAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/HostAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/ApplicationAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/ProcessAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/EntityAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/FTConverter_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/FTProcessObserver_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/FTServiceBroker_impl.hpp"

#include "FaultTolerance/idllib/FT.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTFaultDetector.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"

#include "platformsystemlibrary/System_impl.hpp"


/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW PlatformMngt
*/
namespace PlatformMngt
{

/**
 *Purpose:
 *<p>
 */
class SystemAccessor_impl : public virtual POA_CdmwFTSystMngt::SystemAccessor,
                            public ManagedElementAccessor_impl,
                            public HostContainerAccessor_impl,
                            public RWEntityContainerAccessor_impl,
                            public RWServiceDefContainerAccessor_impl
{

    
private:

    /**
    * ORB reference
    */ 
    CORBA::ORB_var m_orb;
    
    /**
     * The specific POA in charge of servants.
     */
    PortableServer::POA_var m_poa;  

    /**
     * The system managed element
     */
    PlatformMngt::System_impl* m_system;
    
    /**
     * The fault notifier
     */
    ::FT::FaultNotifier_var m_fault_notifier;
    
    /**
     * The fault detector
     **/
    CdmwFT::FaultManagement::FaultDetector_var m_fault_detector;    

    /**
     * The replication manager
     */
    CdmwFT::ReplicationManager_var m_replication_mngr;
    
    /**
     * the replication manager IOGR string
     */
    std::string m_replication_manager_IOGR;


    /**
     * The fault tolerance converter object (ior).
     */
    CdmwPlatformMngt::FTConverter_var m_ft_converter_obj;
    
    /**
     * The fault tolerance process observer object (ior).
     */
    CdmwPlatformMngt::ProcessObserver_var m_ft_process_observer_obj;
    
    /**
     * The fault tolerance service broker object (ior).
     */
    CdmwPlatformMngtService::ServiceBroker_var m_ft_service_broker_obj;
    
    /**
     * The fault tolerance converter object (iogr).
     */
    CdmwPlatformMngt::FTConverter_var m_ft_converter_obj_group;
    
    /**
     * The fault tolerance process observer object (iogr).
     */
    CdmwPlatformMngt::ProcessObserver_var m_ft_process_observer_obj_group;
    
    /**
     * The fault tolerance service broker object (iogr).
     */
    CdmwPlatformMngtService::ServiceBroker_var m_ft_service_broker_obj_group;
    
    
    /**
     * The fault tolerance converter servant.
     */ 
    FTConverter_impl*  m_ft_converter;
    
    /**
     * The ft process observer servant.
     */ 
    FTProcessObserver_impl*  m_ft_process_observer;
    
    /**
     * The ft service broker servant.
     */ 
    FTServiceBroker_impl*  m_ft_service_broker;
    
    /**
     * The application accessor IOGR
     */
    CdmwFTSystMngt::ApplicationAccessor_var m_application_accessor_obj_group;
   
    /**
     * The process accessor IOGR
     */  
    CdmwFTSystMngt::ProcessAccessor_var m_process_accessor_obj_group;
      
    /**
     * The host accessor IOGR
     */   
    CdmwFTSystMngt::HostAccessor_var m_host_accessor_obj_group;
    
    /**
     * The entity accessor IOGR
     */     
    CdmwFTSystMngt::EntityAccessor_var m_entity_accessor_obj_group;

    /**
     * The host accessor servant
     */
    HostAccessor_impl*  m_host_accessor;
    
    /**
     * The application accessor servant
     */
    ApplicationAccessor_impl*  m_application_accessor;

    /**
     * The process accessor servant
     */
    ProcessAccessor_impl*  m_process_accessor;
    
    /**
     * The entity accessor servant
     */
    EntityAccessor_impl*  m_entity_accessor;


    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    SystemAccessor_impl (SystemAccessor_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    SystemAccessor_impl& operator = (const SystemAccessor_impl& rhs);
    

public:

    /**
     *Purpose:
     *<p> Constructor.
     *     
     */

    SystemAccessor_impl (CORBA::ORB_ptr orb,
                         PortableServer::POA_ptr poa,
                         PlatformMngt::System_impl* system,
                         CdmwPlatformMngt::ElementRepository_ptr element_repository,
                         const std::string& supervisionProcessName)    
        throw (Cdmw::Exception);

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~SystemAccessor_impl();
    
    
    /**
     *Purpose:
     *<p> Connect fault converter to fault notifier.
     */
    void connect_ft_converter();


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_application_accessor:1.0
     * operation
     */
    CdmwFTSystMngt::ApplicationAccessor_ptr get_application_accessor()
      throw (CORBA::SystemException);
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_process_accessor:1.0
     * operation
     */
    CdmwFTSystMngt::ProcessAccessor_ptr get_process_accessor()
      throw (CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_host_accessor:1.0
     * operation
     */
    CdmwFTSystMngt::HostAccessor_ptr get_host_accessor()
      throw (CORBA::SystemException);
        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_entity_accessor:1.0
     * operation
     */
    CdmwFTSystMngt::EntityAccessor_ptr get_entity_accessor()
      throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_system_element_path:1.0
     * operation
     */  
    char * get_system_element_path ()
      throw (CORBA::SystemException);
        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_status:1.0
     * operation
     */
    CdmwPlatformMngt::SystemStatus get_status(CORBA::String_out status_info)
          throw (CORBA::SystemException);
    

    /**
     *Purpose:
     *<p> Returns the system_mission
     *
     */

     char * get_system_mission()  throw (CORBA::SystemException);



    /**
     *Purpose:
     *<p> to set the system_mission attribute
     *
     */

     void set_system_mission( const char * system_mission )  throw (CORBA::SystemException);

/**     
     *Purpose:
     *<p> to get system_name
     *
     */

     char * get_system_name(  )  throw (CORBA::SystemException) ;


     /**     
     *Purpose:
     *<p> to set system_name
     *
     */
     void set_system_name( const char * system_name )  throw (CORBA::SystemException);

     /**     
     *Purpose:
     *<p> to get master_host
     *
     */

     char * get_master_host(  )  throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_mode:1.0
     * operation
     */
    CdmwPlatformMngt::FunctioningMode get_mode(CORBA::String_out mode_info)
      throw (CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_snapshot:1.0
     * operation
     */
    CdmwPlatformMngt::SystemSnapshot* get_snapshot()
      throw (CORBA::SystemException);

     /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/System/get_number_of_applications:1.0
     * operation
     */
    CORBA::ULong  get_number_of_applications()
          throw (CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_applications:1.0
     * operation
     */
    CdmwPlatformMngt::ElementPaths* get_applications (
                              CORBA::ULong how_many,
                              CdmwPlatformMngt::ElementPathsIterator_out applications_iterator)
      throw (CORBA::SystemException);
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_application:1.0
     * operation
     */
    char* get_application (const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/register_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr 
           register_observer(const char* observer_name,
                             CdmwPlatformMngt::SupervisionObserver_ptr observer)
      throw (CORBA::SystemException);
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/register_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr 
           register_proxy_observer(
                             const char* observer_name,
                             const char* host_name,
                             CdmwPlatformMngt::SupervisionObserver_ptr observer)
      throw (CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/unregister_observer:1.0
     * operation
     */
    CdmwPlatformMngt::SupervisionObserver_ptr 
           unregister_observer(const char* observer_name)
            throw (CdmwPlatformMngt::ObserverNotFound,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/define:1.0
     * operation
     */
    void define(const CdmwPlatformMngt::SystemDef& system_def)
            throw (CdmwPlatformMngt::DuplicateEntity,
                   CosPropertyService::MultipleExceptions,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateHost,
                   CdmwPlatformMngt::DuplicateHostEntity,
                   CdmwPlatformMngt::HostPropertyExceptions,
                   CdmwPlatformMngt::AlreadyDone,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/set_init_graph:1.0
     * operation
     */
    void set_init_graph (const CdmwPlatformMngt::GraphElements& application_graph)                  
            throw (CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/set_stop_graph:1.0
     * operation
     */
    void set_stop_graph (const CdmwPlatformMngt::GraphElements& application_graph)                  
            throw (CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/set_run_set:1.0
     * operation
     */
    void set_run_set(const CdmwPlatformMngt::RunSetDef& run_set_def)
          throw (CdmwPlatformMngt::DuplicateRunSetElement,
                 CdmwPlatformMngt::IncompatibleStatus,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/remove_run_set:1.0
     * operation
     */
    void remove_run_set(const char* run_set_name)
          throw (CdmwPlatformMngt::RunSetNotFound,
                 CdmwPlatformMngt::IncompatibleStatus,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/remove_all_run_sets:1.0
     * operation
     */
    void remove_all_run_sets()
          throw (CdmwPlatformMngt::IncompatibleStatus,
                 CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/add_host:1.0
     * operation
     */
    char* add_host(const CdmwPlatformMngt::HostDef& host_def)
           throw (CdmwPlatformMngt::HostAlreadyExists,
                  CdmwPlatformMngt::DuplicateEntity, 
                  CosPropertyService::MultipleExceptions,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/remove_host:1.0
     * operation
     */
    void remove_host(const char* host_name)
           throw (CdmwPlatformMngt::HostNotFound,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);
                  

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/add_application:1.0
     * operation
     */
    char* add_application(const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
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
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/remove_application:1.0
     * operation
     */
    void remove_application(const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
 
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/start:1.0
     * operation
     */
    void start (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/resume:1.0
     * operation
     */
    void resume()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/acknowledge:1.0
     * operation
     */
    void acknowledge()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/stop:1.0
     * operation
     */
    void stop (CORBA::Boolean emergency)
            throw (CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/cold_restart:1.0
     * operation
     */
    void cold_restart (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void get_attribute(  const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void set_attribute(  const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);


                   
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/hot_restart:1.0
     * operation
     */
    void hot_restart (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
 
 
protected:

                       

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_FTSYSTMNGT_SYSTEM_ACCESSOR_IMPL_HPP

