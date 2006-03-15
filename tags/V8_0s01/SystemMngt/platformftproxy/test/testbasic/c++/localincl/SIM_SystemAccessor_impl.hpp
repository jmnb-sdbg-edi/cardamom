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


#ifndef INCL_PLATFORMMNGT_PROXY_SIM_SYSTEM_ACCESSOR_IMPL_HPP
#define INCL_PLATFORMMNGT_PROXY_SIM_SYSTEM_ACCESSOR_IMPL_HPP


#include <SystemMngt/idllib/CdmwFTSystMngtSystem.skel.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtApplication.skel.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtProcess.skel.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtEntity.skel.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtObservable.stub.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp>

#include "testbasic/SIM_ManagedElementAccessor_impl.hpp"
#include "testbasic/SIM_HostContainerAccessor_impl.hpp"
#include "testbasic/SIM_RWEntityContainerAccessor_impl.hpp"
#include "testbasic/SIM_RWServiceDefContainerAccessor_impl.hpp"





// #include "testbasic/SIM_ApplicationAccessor_impl.hpp"
// #include "testbasic/SIM_HostAccessor_impl.hpp"
// #include "testbasic/SIM_ProcessAccessor_impl.hpp"
// #include "testbasic/SIM_EntityAccessor_impl.hpp"


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

namespace Proxy
{

/**
 *Purpose:
 *<p>
 */
class SIM_SystemAccessor_impl : 
                            public virtual POA_CdmwFTSystMngt::SystemAccessor,
                            public SIM_ManagedElementAccessor_impl,
                            public SIM_HostContainerAccessor_impl,
                            public SIM_RWEntityContainerAccessor_impl,
                            public SIM_RWServiceDefContainerAccessor_impl
{
private:
    CdmwFTSystMngt::ApplicationAccessor_var m_appl_accessor;
    CdmwFTSystMngt::ProcessAccessor_var     m_proc_accessor;
    CdmwFTSystMngt::HostAccessor_var        m_host_accessor;
    CdmwFTSystMngt::EntityAccessor_var      m_entity_accessor;
    
private:

    /**
     * The system managed element
     */
//     CdmwPlatformMngt::System_var m_system;

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    SIM_SystemAccessor_impl (SIM_SystemAccessor_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    SIM_SystemAccessor_impl& operator = (const SIM_SystemAccessor_impl& rhs);
    

public:

    /**
     *Purpose:
     *<p> Constructor.
     *     
     */

    SIM_SystemAccessor_impl ()
        throw ();

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~SIM_SystemAccessor_impl();

    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void get_attribute( const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void set_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);


    ///
     char* get_system_name()throw(CORBA::SystemException);
     void set_system_name(const char*)throw(CORBA::SystemException);
     char* get_master_host()throw(CORBA::SystemException);



    CdmwFTSystMngt::ApplicationAccessor_ptr 
    SIM_SystemAccessor_impl::createApplicationAccessor()
	throw (CORBA::SystemException);
    
    CdmwFTSystMngt::ProcessAccessor_ptr 
    SIM_SystemAccessor_impl::createProcessAccessor()
	throw (CORBA::SystemException);
    
    
    CdmwFTSystMngt::HostAccessor_ptr 
    SIM_SystemAccessor_impl::createHostAccessor()
	throw (CORBA::SystemException);
    
    CdmwFTSystMngt::EntityAccessor_ptr 
    SIM_SystemAccessor_impl::createEntityAccessor()
	throw (CORBA::SystemException);
    
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
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/SystemAccessor/get_system_element_path:1.0
     * operation
     */
    CdmwPlatformMngt::ElementPath get_system_element_path ()
	throw(CORBA::SystemException);

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
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateHost,
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
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateProcess,
                   CdmwPlatformMngt::ProcessHostNotFound,
                   CdmwPlatformMngt::DuplicateProcessEntity,
                   CdmwPlatformMngt::DuplicateProcessService,
                   CdmwPlatformMngt::DuplicateProcessStep,
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

} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_PROXY_SIM_SYSTEM_ACCESSOR_IMPL_HPP

