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


#ifndef INCL_PLATFORMMNGT_PROXY_SIM_APPLICATION_ACCESSOR_IMPL_HPP
#define INCL_PLATFORMMNGT_PROXY_SIM_APPLICATION_ACCESSOR_IMPL_HPP


#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>

#include "testbasic/SIM_ManagedElementAccessor_impl.hpp"
#include "testbasic/SIM_RWEntityContainerAccessor_impl.hpp"
#include "testbasic/SIM_RWServiceDefContainerAccessor_impl.hpp"



#include <SystemMngt/idllib/CdmwFTSystMngtApplication.skel.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp>

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

/**
 *Purpose:
 *<p>
 */
class SIM_ApplicationAccessor_impl 
	: public virtual POA_CdmwFTSystMngt::ApplicationAccessor,
	  public SIM_ManagedElementAccessor_impl,
	  public SIM_RWEntityContainerAccessor_impl,
	  public SIM_RWServiceDefContainerAccessor_impl                                                        
{
private:
// 	TestSamples   


    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    SIM_ApplicationAccessor_impl(
        SIM_ApplicationAccessor_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    SIM_ApplicationAccessor_impl& operator=(
        const SIM_ApplicationAccessor_impl& rhs);

public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     */
    SIM_ApplicationAccessor_impl()
            throw();
    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~SIM_ApplicationAccessor_impl();

public:

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/get_restart_type:1.0
     * operation
     */         
    CdmwPlatformMngt::RestartType get_restart_type (const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/get_restart_attempt:1.0
     * operation
     */    
    CORBA::Long get_restart_attempt (const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
        
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/is_manual_starting:1.0
     * operation
     */    
    CORBA::Boolean is_manual_starting(const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
     
     
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/set_manual_starting:1.0
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
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/get_status:1.0
     * operation
     */
    CdmwPlatformMngt::ApplicationStatus get_status(
                 const char* target_element_path,
                 CORBA::String_out status_info)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/get_mode:1.0
     * operation
     */
    CdmwPlatformMngt::FunctioningMode get_mode(
                 const char* target_element_path,
                 CORBA::String_out mode_info)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/get_snapshot:1.0
     * operation
     */
    CdmwPlatformMngt::ApplicationSnapshot* get_snapshot(
                 const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/get_number_of_processes:1.0
     * operation
     */
    CORBA::ULong get_number_of_processes( const char* target_element_path) throw(CdmwPlatformMngt::ManagedElementNotFound, CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/get_processes:1.0
     * operation
     */
    CdmwPlatformMngt::ElementPaths* get_processes(
                 const char* target_element_path,
                 CORBA::ULong how_many,
                 CdmwPlatformMngt::ElementPathsIterator_out processes_iterator)
              throw(CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void get_attribute( const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr  , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void set_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/get_process:1.0
     * operation
     */
    char* get_process(const char* target_element_path,              
		      const char* process_name,
		      const char* host_name)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::ProcessNotFound,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/define:1.0
     * operation
     */
    void define (const char* target_element_path,            
                 const CdmwPlatformMngt::ApplicationDef& application_def)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::DuplicateEntity,
                  CdmwPlatformMngt::DuplicateService,
                  CdmwPlatformMngt::DuplicateProcess,
                  CdmwPlatformMngt::ProcessHostNotFound,
                  CdmwPlatformMngt::DuplicateProcessEntity,
                  CdmwPlatformMngt::DuplicateProcessService,
                  CdmwPlatformMngt::DuplicateProcessStep,
                  CdmwPlatformMngt::AlreadyDone,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/add_process:1.0
     * operation
     */
    char* add_process(const char* target_element_path,
                     CdmwPlatformMngt::ProcessDef* process_def)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::ProcessAlreadyExists,
                  CdmwPlatformMngt::HostNotFound,
                  CdmwPlatformMngt::DuplicateEntity,
                  CdmwPlatformMngt::DuplicateService,
                  CdmwPlatformMngt::DuplicateStep,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/remove_process:1.0
     * operation
     */
    void remove_process(
                  const char* target_element_path,
                  const char* process_name,
                  const char* host_name)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::ProcessNotFound,
                  CdmwPlatformMngt::ProcessStillActive,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/set_init_graph:1.0
     * operation
     */
    void set_init_graph(
                  const char* target_element_path,
                  const CdmwPlatformMngt::GraphElements& process_graph)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::ReferenceNotFound,
                  CdmwPlatformMngt::CircularReference,
                  CdmwPlatformMngt::StepOutOfBound,
                  CdmwPlatformMngt::StepJump,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/set_stop_graph:1.0
     * operation
     */
    void set_stop_graph(
                  const char* target_element_path,
                  const CdmwPlatformMngt::GraphElements& process_graph)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::ReferenceNotFound,
                  CdmwPlatformMngt::CircularReference,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/get_autorestart_info:1.0
     * operation
     */              
    CdmwPlatformMngt::ApplicationAutoRestartInfo get_autorestart_info(
                   const char* target_element_path)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/set_autorestart_info:1.0
     * operation
     */
    void set_autorestart_info(
                   const char* target_element_path,
                   const CdmwPlatformMngt::ApplicationAutoRestartInfo& autorestart_info)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/initialise:1.0
     * operation
     */
    void initialise(const char* target_element_path,
                    const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/initialise_and_wait:1.0
     * operation
     */
    CdmwPlatformMngt::LongRequestStatus initialise_and_wait(
                   const char* target_element_path,
                   const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/resume:1.0
     * operation
     */
    void resume(const char* target_element_path)
        throw(CdmwPlatformMngt::ManagedElementNotFound,
              CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/resume_and_wait:1.0
     * operation
     */
    CdmwPlatformMngt::LongRequestStatus resume_and_wait(
                    const char* target_element_path)
        throw(CdmwPlatformMngt::ManagedElementNotFound,
              CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/acknowledge:1.0
     * operation
     */
    void acknowledge(const char* target_element_path)
        throw(CdmwPlatformMngt::ManagedElementNotFound,
              CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/run:1.0
     * operation
     */
    void run(const char* target_element_path)
        throw(CdmwPlatformMngt::ManagedElementNotFound,
              CdmwPlatformMngt::IncompatibleStatus,
              CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/stop:1.0
     * operation
     */
    void stop(const char* target_element_path,
              CORBA::Boolean emergency)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/stop_and_wait:1.0
     * operation
     */
    CdmwPlatformMngt::LongRequestStatus stop_and_wait(
              const char* target_element_path,
              CORBA::Boolean emergency)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CORBA::SystemException);
            
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/cold_restart:1.0
     * operation
     */
    void cold_restart(const char* target_element_path,
                      const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException);        
            

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/ApplicationAccessor/hot_restart:1.0
     * operation
     */
    void hot_restart(const char* target_element_path,
                     const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CdmwPlatformMngt::ManagedElementNotFound,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException); 

};

} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_PROXY_APPLICATION_ACCESSOR_IMPL_HPP

