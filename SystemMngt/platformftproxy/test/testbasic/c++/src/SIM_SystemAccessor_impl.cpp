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


#include "testbasic/SIM_SystemAccessor_impl.hpp"
#include "testbasic/SIM_ApplicationAccessor_impl.hpp"
#include "testbasic/SIM_ProcessAccessor_impl.hpp"
#include "testbasic/SIM_HostAccessor_impl.hpp"
#include "testbasic/SIM_EntityAccessor_impl.hpp"

#include <SystemMngt/platformlibrary/ElementPathsIterator_impl.hpp>

#include "testbasic/TestModus.hpp"
#include "testbasic/TestSamples.hpp"
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
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

SIM_SystemAccessor_impl::SIM_SystemAccessor_impl (
			    SIM_SystemAccessor_impl& rhs) 
{ // FIXME tbd
}

SIM_SystemAccessor_impl&
SIM_SystemAccessor_impl::operator = (const SIM_SystemAccessor_impl& rhs) 
{ 
    CDMW_NEVER_HERE();
    return *this;
}


SIM_SystemAccessor_impl::SIM_SystemAccessor_impl()    
        throw ()
{
    try {
	m_appl_accessor   = createApplicationAccessor();
	m_proc_accessor   = createProcessAccessor();
	m_host_accessor   = createHostAccessor();
	m_entity_accessor = createEntityAccessor();    
    }
    catch(...) {
	    CDMW_NEVER_HERE();
    }
}


SIM_SystemAccessor_impl::~SIM_SystemAccessor_impl()
{
}

CdmwFTSystMngt::ApplicationAccessor_ptr 
SIM_SystemAccessor_impl::createApplicationAccessor()
    throw (CORBA::SystemException)
{ 
    Cdmw::PlatformMngt::Proxy::SIM_ApplicationAccessor_impl *appl_accessor_impl = 
	new Cdmw::PlatformMngt::Proxy::SIM_ApplicationAccessor_impl();

    CORBA::Object_var obj = appl_accessor_impl->_this();

    CdmwFTSystMngt::ApplicationAccessor_ptr appl_accessor =
	CdmwFTSystMngt::ApplicationAccessor::_narrow(obj.in());

    return appl_accessor;
}
      
CdmwFTSystMngt::ProcessAccessor_ptr 
SIM_SystemAccessor_impl::createProcessAccessor()
      throw (CORBA::SystemException)
{ 
    Cdmw::PlatformMngt::Proxy::SIM_ProcessAccessor_impl *proc_accessor_impl = 
	new Cdmw::PlatformMngt::Proxy::SIM_ProcessAccessor_impl();

    CORBA::Object_var obj = proc_accessor_impl->_this();

    CdmwFTSystMngt::ProcessAccessor_ptr proc_accessor =
	CdmwFTSystMngt::ProcessAccessor::_narrow(obj.in());

    return proc_accessor;
}

    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void SIM_SystemAccessor_impl::get_attribute( const char * target_element_path, const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

     CORBA::ULong SIM_SystemAccessor_impl::get_number_of_applications() throw (CORBA::SystemException )
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);


}

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void SIM_SystemAccessor_impl::set_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);

}

      
///
char* SIM_SystemAccessor_impl::get_system_name()throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

void SIM_SystemAccessor_impl::set_system_name(const char * path)throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

char * SIM_SystemAccessor_impl::get_master_host()throw(CORBA::SystemException)
{
throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

      
CdmwFTSystMngt::HostAccessor_ptr 
SIM_SystemAccessor_impl::createHostAccessor()
      throw (CORBA::SystemException)
{
    Cdmw::PlatformMngt::Proxy::SIM_HostAccessor_impl *host_accessor_impl = 
	new Cdmw::PlatformMngt::Proxy::SIM_HostAccessor_impl();

    CORBA::Object_var obj = host_accessor_impl->_this();

    CdmwFTSystMngt::HostAccessor_ptr host_accessor =
	CdmwFTSystMngt::HostAccessor::_narrow(obj.in());

    return host_accessor;
}
        
CdmwFTSystMngt::EntityAccessor_ptr 
SIM_SystemAccessor_impl::createEntityAccessor()
      throw (CORBA::SystemException)
{
    Cdmw::PlatformMngt::Proxy::SIM_EntityAccessor_impl *entity_accessor_impl = 
	new Cdmw::PlatformMngt::Proxy::SIM_EntityAccessor_impl();

    CORBA::Object_var obj = entity_accessor_impl->_this();

    CdmwFTSystMngt::EntityAccessor_ptr entity_accessor =
	CdmwFTSystMngt::EntityAccessor::_narrow(obj.in());

    return entity_accessor;
}
  

CdmwFTSystMngt::ApplicationAccessor_ptr 
SIM_SystemAccessor_impl::get_application_accessor()
    throw (CORBA::SystemException)
{ 
    raiseExceptionIfExceptionModus();

    return CdmwFTSystMngt::ApplicationAccessor::_duplicate(m_appl_accessor.in());
}
      
CdmwFTSystMngt::ProcessAccessor_ptr 
SIM_SystemAccessor_impl::get_process_accessor()
      throw (CORBA::SystemException)
{ 
    raiseExceptionIfExceptionModus();

    return CdmwFTSystMngt::ProcessAccessor::_duplicate(m_proc_accessor.in());
}
      
      
CdmwFTSystMngt::HostAccessor_ptr 
SIM_SystemAccessor_impl::get_host_accessor()
      throw (CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    return CdmwFTSystMngt::HostAccessor::_duplicate(m_host_accessor.in());
}
        
CdmwFTSystMngt::EntityAccessor_ptr 
SIM_SystemAccessor_impl::get_entity_accessor()
      throw (CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    return CdmwFTSystMngt::EntityAccessor::_duplicate(m_entity_accessor.in());
}
  
        
CdmwPlatformMngt::SystemStatus 
SIM_SystemAccessor_impl::get_status(CORBA::String_out status_info)
          throw (CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();


//     return one of         
// 	enum SystemStatus
//         {
//           SYSTEM_INITIAL,
//           SYSTEM_STARTING,
//           SYSTEM_STARTUP_SUSPENDED,
//           SYSTEM_STARTED,
//           SYSTEM_STOPPING,
//           SYSTEM_STOPPED
//         };

    return CdmwPlatformMngt::SYSTEM_STOPPED;
}

 char * SIM_SystemAccessor_impl::get_system_mission()  throw (CORBA::SystemException)
           {
            throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
           };

     /**     
     *Purpose:
     *<p> to set the system_mission attribute
     *
     */

     void SIM_SystemAccessor_impl::set_system_mission( const char * system_mission )  throw (CORBA::SystemException)
       {
        throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
       };




CdmwPlatformMngt::ElementPath 
SIM_SystemAccessor_impl::get_system_element_path ()
    throw(CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

 //    return TestSamples::getElementPath();
    return TestSamples::getElementPath();
}

    
CdmwPlatformMngt::FunctioningMode 
SIM_SystemAccessor_impl::get_mode(CORBA::String_out mode_info)
      throw (CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

//  return one of
//   enum FunctioningMode
//   {
//     NORMAL_MODE,
//     DEGRADED_MODE
//   };
  
    return TestSamples::getFunctioningMode();
}
  
    
CdmwPlatformMngt::SystemSnapshot* SIM_SystemAccessor_impl::get_snapshot()
      throw (CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    return TestSamples::getSystemSnapshot();
}

CdmwPlatformMngt::ElementPaths* 
SIM_SystemAccessor_impl::get_applications (
                              CORBA::ULong how_many,
                              CdmwPlatformMngt::ElementPathsIterator_out applications_iterator)
      throw (CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    CdmwPlatformMngt::ElementPaths_var elems = 
	TestSamples::getElementPaths();

    CdmwPlatformMngt::ElementPaths_var head =
	new CdmwPlatformMngt::ElementPaths();
    CdmwPlatformMngt::ElementPaths_var tail =
	new CdmwPlatformMngt::ElementPaths();
	
    split(elems, how_many, head, tail);

    // create the path iterator
    CdmwPlatformMngt::ElementPathsIterator_var elementsRemainingIt =
	ElementPathsIterator_impl::create (seq2list(tail));

    applications_iterator = elementsRemainingIt;

    return head._retn();
}
      
char* 
SIM_SystemAccessor_impl::get_application (const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    return  TestSamples::getElementPath();
}
  


CdmwPlatformMngt::SupervisionObserver_ptr 
SIM_SystemAccessor_impl::register_observer(const char* observer_name,
					   CdmwPlatformMngt::SupervisionObserver_ptr observer)
      throw (CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    return TestSamples::getSupervisionObserver();
}
  
      
CdmwPlatformMngt::SupervisionObserver_ptr 
SIM_SystemAccessor_impl::register_proxy_observer(
                             const char* observer_name,
                             const char* host_name,
                             CdmwPlatformMngt::SupervisionObserver_ptr observer)
      throw (CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    return TestSamples::getSupervisionObserver();
}
  
      
      
CdmwPlatformMngt::SupervisionObserver_ptr 
SIM_SystemAccessor_impl::unregister_observer(const char* observer_name)
            throw (CdmwPlatformMngt::ObserverNotFound,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
 
    return TestSamples::getSupervisionObserver();
}

void 
SIM_SystemAccessor_impl::define(const CdmwPlatformMngt::SystemDef& system_def)
            throw (CdmwPlatformMngt::DuplicateEntity,
                   CdmwPlatformMngt::DuplicateService,
                   CdmwPlatformMngt::DuplicateHost,
                   CdmwPlatformMngt::AlreadyDone,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    // FIXME verify argument
}



void 
SIM_SystemAccessor_impl::set_init_graph (const CdmwPlatformMngt::GraphElements& application_graph)                  
            throw (CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    // FIXME verify argument
}



void 
SIM_SystemAccessor_impl::set_stop_graph (const CdmwPlatformMngt::GraphElements& application_graph)                  
            throw (CdmwPlatformMngt::ReferenceNotFound,
                   CdmwPlatformMngt::CircularReference,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    // FIXME verify argument
}

    
void 
SIM_SystemAccessor_impl::set_run_set(const CdmwPlatformMngt::RunSetDef& run_set_def)
          throw (CdmwPlatformMngt::DuplicateRunSetElement,
                 CdmwPlatformMngt::IncompatibleStatus,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
}

void 
SIM_SystemAccessor_impl::remove_run_set(const char* run_set_name)
          throw (CdmwPlatformMngt::RunSetNotFound,
                 CdmwPlatformMngt::IncompatibleStatus,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    // FIXME verify argument
}


void 
SIM_SystemAccessor_impl::remove_all_run_sets()
          throw (CdmwPlatformMngt::IncompatibleStatus,
                 CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    // FIXME verify argument
}

char*
SIM_SystemAccessor_impl::add_host(const CdmwPlatformMngt::HostDef& host_def)
           throw (CdmwPlatformMngt::HostAlreadyExists,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();

    return TestSamples::getElementPath();
}


void 
SIM_SystemAccessor_impl::remove_host(const char* host_name)
           throw (CdmwPlatformMngt::HostNotFound,
                  CdmwPlatformMngt::IncompatibleStatus,
                  CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
}

                  

char*
SIM_SystemAccessor_impl::add_application(const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
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
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
    
    return TestSamples::getElementPath();

}


void 
SIM_SystemAccessor_impl::remove_application(const char* application_name)
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CdmwPlatformMngt::ApplicationStillActive,
                   CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
}

 
void 
SIM_SystemAccessor_impl::start (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
}



    
void 
SIM_SystemAccessor_impl::resume()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
   raiseExceptionIfExceptionModus();
}


void 
SIM_SystemAccessor_impl::acknowledge()
        throw (CdmwPlatformMngt::IncompatibleStatus,
               CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
}



void 
SIM_SystemAccessor_impl::stop (CORBA::Boolean emergency)
            throw (CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
}



void 
SIM_SystemAccessor_impl::cold_restart (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{
   raiseExceptionIfExceptionModus();
}

                   
void 
SIM_SystemAccessor_impl::hot_restart (const CdmwPlatformMngtBase::StartupKind& startup_kind)      
            throw (CdmwPlatformMngt::IncompatibleStatus,
                   CORBA::SystemException)
{
    raiseExceptionIfExceptionModus();
}


} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

