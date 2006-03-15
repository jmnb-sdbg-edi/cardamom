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

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "platformftaccessorlibrary/HostAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"

#include "SystemMngt/platformlibrary/ElementPathsIterator_impl.hpp"


namespace Cdmw
{
namespace PlatformMngt
{




// ----------------------------------------------------------------------
// HostAccessor_impl class.
// ----------------------------------------------------------------------

HostAccessor_impl::HostAccessor_impl(
    CdmwPlatformMngt::ElementRepository_ptr element_repository)
throw()
    : ManagedElementAccessor_impl(element_repository),
      RWEntityContainerAccessor_impl(element_repository)                   
{
}



HostAccessor_impl::~HostAccessor_impl ()
{
}

CdmwPlatformMngt::Host_ptr HostAccessor_impl::get_host (
          const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound)
{
    CdmwPlatformMngt::Host_var host_obj;
    
    try
    {
        CORBA::Object_var element_obj =
              m_element_repository->get_element(target_element_path);
              
        host_obj = CdmwPlatformMngt::Host::_narrow (element_obj.in());
    }
    catch (const CdmwPlatformMngt::ElementPathInvalid&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::ManagedElementNotFound(target_element_path);
    }
    
    return host_obj._retn();                   
}


void HostAccessor_impl::get_attribute( const char * target_element_path,
const char * attr_name, CORBA::Any_out ret_attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
    CdmwPlatformMngt::Host_var host_obj = 
                        get_host (target_element_path);
                        
    host_obj->get_attribute( attr_name , ret_attr , flag);

}

void HostAccessor_impl::set_attribute( const char * target_element_path,
const char * attr_name, const CORBA::Any & attr,CORBA::Char flag )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
    CdmwPlatformMngt::Host_var host_obj = 
                        get_host (target_element_path);
                        
    host_obj->set_attribute( attr_name , attr , flag);


}

CdmwPlatformMngt::HostStatus HostAccessor_impl::get_status(const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
    CdmwPlatformMngt::Host_var host_obj = 
                        get_host (target_element_path);
                        
    return host_obj->status();
}
    
char * HostAccessor_impl::get_host_status_info(const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound, CORBA::SystemException)
{
    CdmwPlatformMngt::Host_var host_obj = 
                        get_host (target_element_path);
                        
    return host_obj->host_status_info();
    
}



    CdmwPlatformMngt::CompleteMonitoringInfo HostAccessor_impl::get_monitoring_parameters(const char* target_element_path) throw (CdmwPlatformMngt::ManagedElementNotFound,CORBA::SystemException)    
     {
     CdmwPlatformMngt::Host_var host_obj = get_host (target_element_path);
                        
     return host_obj->get_monitoring_parameters();


     }

    void HostAccessor_impl::set_monitoring_parameters(const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_parameters, const char* target_element_path) throw (CdmwPlatformMngt::ManagedElementNotFound, CORBA::SystemException)
     {
     CdmwPlatformMngt::Host_var host_obj = get_host (target_element_path);
                        
     host_obj->set_monitoring_parameters(monitoring_parameters);

     }

bool HostAccessor_impl::get_global_supervision_indicator(const char* target_element_path) throw (CdmwPlatformMngt::ManagedElementNotFound, CORBA::SystemException)
   {
     CdmwPlatformMngt::Host_var host_obj = get_host (target_element_path);
                        
     return(host_obj->get_global_supervision_indicator());
   } 

   void HostAccessor_impl::set_global_supervision_indicator(bool global_supervision_indicator  ,  const char* target_element_path)  throw (CdmwPlatformMngt::ManagedElementNotFound, CORBA::SystemException )
   {
     CdmwPlatformMngt::Host_var host_obj = get_host (target_element_path);
                        
     host_obj->set_global_supervision_indicator(global_supervision_indicator);

   }

char * HostAccessor_impl::get_host_name(const char* target_element_path) throw (CdmwPlatformMngt::ManagedElementNotFound, CORBA::SystemException)
   {
     CdmwPlatformMngt::Host_var host_obj = get_host (target_element_path);
                        
     return(host_obj->get_host_name());
   } 

char* HostAccessor_impl::get_platform_application_exe(const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
    CdmwPlatformMngt::Host_var host_obj = 
                        get_host (target_element_path);
                        
    return host_obj->platform_application_exe();
}
    
    
CdmwPlatformMngt::ElementPaths* HostAccessor_impl::get_processes(
                     const char* target_element_path,
                     CORBA::ULong how_many,
                     CdmwPlatformMngt::ElementPathsIterator_out processes_iterator)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
    CdmwPlatformMngt::Host_var host_obj = 
                        get_host (target_element_path);
                        
    // get the list of process references
    CdmwPlatformMngt::ProcessesIterator_var processesRemainingIt;
    
	CdmwPlatformMngt::Processes_var processes_seq =
	            host_obj->get_processes (how_many, processesRemainingIt.out());
	            
    // convert each process reference in element path
    
    // create sequence of element path
    CdmwPlatformMngt::ElementPaths_var elements =
                          new CdmwPlatformMngt::ElementPaths();
    
    // set element sequence length 
    size_t seq_len = processes_seq->length();
    elements->length(seq_len);
    
    // fill element sequence
    unsigned int count;
    for (count=0 ; count < seq_len ; count++)
    {
        // Store process path in sequence
        // path is allocated by element_path()
        if (!CORBA::is_nil(processes_seq[count]))
        elements[count] = (processes_seq[count])->element_path();                                           
    }
        
    // Get an iterator on the rest of the elements
    std::list<CdmwPlatformMngt::ElementPath_var> elementsRemaining;
    CdmwPlatformMngt::Process_var process;
    
    // fore each element contained in reference iterator
    while (processesRemainingIt->next_one (process.out()))
    { 
        elementsRemaining.push_back (process->element_path());
    }
    
    // create the path iterator
    CdmwPlatformMngt::ElementPathsIterator_var elementsRemainingIt =
              ElementPathsIterator_impl::create (elementsRemaining);

    // Assign the iterator to the processes_iterator
    processes_iterator = elementsRemainingIt._retn();

    return elements._retn();
}

            
        
void HostAccessor_impl::shutdown (const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Host_var host_obj = 
                        get_host (target_element_path);
                        
    host_obj->shutdown();
}


 
void HostAccessor_impl::cold_reboot (const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Host_var host_obj = 
                        get_host (target_element_path);
                        
    host_obj->cold_reboot();
}

               
void HostAccessor_impl::hot_reboot (const char* target_element_path)
throw(CdmwPlatformMngt::ManagedElementNotFound,
      CORBA::SystemException)
{
	CdmwPlatformMngt::Host_var host_obj = 
                        get_host (target_element_path);
                        
    host_obj->hot_reboot();
}




} // End namespace PlatformMngt
} // End namespace Cdmw


