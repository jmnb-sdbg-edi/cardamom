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


#ifndef INCL_FTSYSTMNGT_PROCESS_ACCESSOR_HPP
#define INCL_FTSYSTMNGT_PROCESS_ACCESSOR_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/platformftaccessorlibrary/ManagedElementAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/RWEntityContainerAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/RWServiceDefContainerAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwFTSystMngtProcess.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
 *Purpose: Provides the process access.
 *<p>
 */
class ProcessAccessor_impl : public virtual POA_CdmwFTSystMngt::ProcessAccessor,
                             public ManagedElementAccessor_impl,
                             public RWEntityContainerAccessor_impl,
                             public RWServiceDefContainerAccessor_impl
{
    
private:

    /**
     *Purpose:
     *<p> Get the Process object.
     */
    CdmwPlatformMngt::Process_ptr get_process (const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound);

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ProcessAccessor_impl(const ProcessAccessor_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ProcessAccessor_impl& operator=(const ProcessAccessor_impl& rhs);

public:
    /**
     *Purpose:
     *<p> constructor.
     */
    ProcessAccessor_impl(
        CdmwPlatformMngt::ElementRepository_ptr element_repository)
            throw();

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual
    ~ProcessAccessor_impl();


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_type:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessType get_type (const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_embedded_service:1.0
     * operation
     */
    CORBA::Object_ptr get_embedded_service(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
              
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_pid:1.0
     * operation
     */          
    CdmwPlatformMngt::ProcessPID get_pid(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_created_dynamically:1.0
     * operation
     */
    CORBA::Boolean get_created_dynamically(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
     
    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void get_attribute(  const char* target_element_path , const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void set_attribute(  const char* target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_restart_type:1.0
     * operation
     */   
    CdmwPlatformMngt::RestartType get_restart_type(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_restart_attempt:1.0
     * operation
     */     
    CORBA::Long get_restart_attempt(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_run_as_root:1.0
     * operation
     */     
    bool get_run_as_root(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_start_time:1.0
     * operation
     */     
    CdmwPlatformMngtBase::TimeStamp get_start_time(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_stop_time:1.0
     * operation
     */     
    CdmwPlatformMngtBase::TimeStamp get_stop_time(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/is_manual_starting:1.0
     * operation
     */ 
    CORBA::Boolean is_manual_starting(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/set_manual_starting:1.0
     * operation
     */     
    void set_manual_starting(const char* target_element_path,
                             CORBA::Boolean manual_starting)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_status:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessStatus get_status(
                              const char* target_element_path,
                              CORBA::String_out status_info)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
              
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_process_mode:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessMode get_process_mode(
                              const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_memory_resident:1.0
     * operation
     */
    bool get_memory_resident( const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/set_memory_resident:1.0
     * operation
     */
    void set_memory_resident( const char* target_element_path, bool memory_resident)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_core_dump_size:1.0
     * operation
     */
    CORBA::Long get_core_dump_size( const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);




    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/is_auto_ending:1.0
     * operation
     */
    CORBA::Boolean is_auto_ending(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
              
              
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/set_auto_ending:1.0
     * operation
     */
    void set_auto_ending(const char* target_element_path,
                         CORBA::Boolean auto_ending)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_host:1.0
     * operation
     */          
    char* get_host(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_process_info:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessInfo* get_process_info(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/set_process_info:1.0
     * operation
     */
    void set_process_info(const char* target_element_path,
                          const CdmwPlatformMngt::ProcessInfo& process_info )
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_life_cycle:1.0
     * operation
     */
    CdmwPlatformMngt::LifeCycleDef* get_life_cycle(const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::IncompatibleType,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/set_life_cycle:1.0
     * operation
     */
    void set_life_cycle(const char* target_element_path,
                        const CdmwPlatformMngt::LifeCycleDef& life_cycle)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::DuplicateStep,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_monitoring_info:1.0
     * operation
     */
    CdmwPlatformMngt::CompleteMonitoringInfo get_monitoring_info(
                         const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::IncompatibleType,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/set_monitoring_info:1.0
     * operation
     */
    void set_monitoring_info(
                         const char* target_element_path,
                         const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_info)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);
                   
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_exit_autorestart_info:1.0
     * operation
     */               
    CdmwPlatformMngt::ExitAutoRestartInfo* get_exit_autorestart_info(
                         const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/set_exit_autorestart_info:1.0
     * operation
     */ 
    void set_exit_autorestart_info(
                         const char* target_element_path,
                         const CdmwPlatformMngt::ExitAutoRestartInfo& exit_autorestart_info)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/get_snapshot:1.0
     * operation
     */
    CdmwPlatformMngt::ProcessSnapshot* get_snapshot(const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException);
             
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/initialise:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus initialise(
                          const char* target_element_path,
                          const CdmwPlatformMngtBase::StartupKind& startup_kind)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/next_step:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus next_step(const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::InvalidStep,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/run:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus run(const char* target_element_path)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CdmwPlatformMngt::NotReadyToRun,
                   CdmwPlatformMngt::IncompatibleType,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/stop:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus stop(const char* target_element_path,
                                         CORBA::Boolean emergency)
             throw(CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException);
             
             
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/cold_restart:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus cold_restart(
                          const char* target_element_path,
                          const CdmwPlatformMngtBase::StartupKind& startup_kind)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException);
             

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ProcessAccessor/hot_restart:1.0
     * operation
     */
    CdmwPlatformMngt::RequestStatus hot_restart(
                          const char* target_element_path,
                          const CdmwPlatformMngtBase::StartupKind& startup_kind)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CdmwPlatformMngt::IncompatibleStatus,
                    CORBA::SystemException);
                    
                    

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_FTSYSTMNGT_PROCESS_ACCESSOR_HPP

