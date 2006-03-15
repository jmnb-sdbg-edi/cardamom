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


#ifndef INCL_PLATFORMMNGT_PROXY_SIM_HOST_PROXY_IMPL_HPP
#define INCL_PLATFORMMNGT_PROXY_SIM_HOST_PROXY_IMPL_HPP

#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>

#include "testbasic/SIM_ManagedElementAccessor_impl.hpp"



#include <SystemMngt/idllib/CdmwFTSystMngtHost.skel.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp>
#include "Foundation/common/Exception.hpp"
#include <SystemMngt/idllib/CdmwPlatformMngtManagedElement.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtPropertyManager.skel.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtManagedElement.skel.hpp>


namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{

/**
 *Purpose:
 *<p> Provides the host access.
 */
class SIM_HostAccessor_impl 
	: public virtual POA_CdmwFTSystMngt::HostAccessor,
	  public SIM_ManagedElementAccessor_impl
{
private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    SIM_HostAccessor_impl(
        SIM_HostAccessor_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    SIM_HostAccessor_impl& operator=(
        const SIM_HostAccessor_impl& rhs);
        
        
public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     */
    SIM_HostAccessor_impl()
            throw();

    /**
     *Purpose:
     *<p> Destructor.
     */
    ~SIM_HostAccessor_impl();
    
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
    virtual char * get_host_name(const char* path)throw(CORBA::SystemException)
    {
    //throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    return NULL;
    }

    virtual char * get_host_status_info(const char* path)throw(CORBA::SystemException)
    {
    //throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    return NULL;
    }

    virtual CdmwPlatformMngt::CompleteMonitoringInfo get_monitoring_parameters(const char*)throw(CORBA::SystemException)
      {
      CdmwPlatformMngt::CompleteMonitoringInfo comp;
      return comp;
      }

    virtual void set_monitoring_parameters(const CdmwPlatformMngt::CompleteMonitoringInfo&, const char*)throw(CORBA::SystemException)
      {

      }

    virtual CORBA::Boolean get_global_supervision_indicator(const char*)throw(CORBA::SystemException)
      {

      return true;
      }
    void set_global_supervision_indicator(bool, const char*)throw(CORBA::SystemException)
    {

    }
    

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/HostAccessor/status:1.0
     * operation
     */
    CdmwPlatformMngt::HostStatus get_status(const char* target_element_path)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/HostAccessor/platform_application_exe:1.0
     * operation
     */
    char* get_platform_application_exe(const char* target_element_path)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/HostAccessor/get_processes:1.0
     * operation
     */
    CdmwPlatformMngt::ElementPaths* get_processes(
                     const char* target_element_path,
                     CORBA::ULong how_many,
                     CdmwPlatformMngt::ElementPathsIterator_out processes_iterator)
              throw (CdmwPlatformMngt::ManagedElementNotFound,
                     CORBA::SystemException);
            
  
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/HostAccessor/shutdown:1.0
     * operation
     */        
    void shutdown (const char* target_element_path)
              throw (CdmwPlatformMngt::ManagedElementNotFound,
                     CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/HostAccessor/cold_reboot:1.0
     * operation
     */ 
    void cold_reboot (const char* target_element_path)
              throw (CdmwPlatformMngt::ManagedElementNotFound,
                     CORBA::SystemException);
              
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/HostAccessor/hot_reboot:1.0
     * operation
     */ 
    void hot_reboot (const char* target_element_path)
              throw (CdmwPlatformMngt::ManagedElementNotFound,
                     CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/get_number_of_entities:1.0
     * operation
     */ 
    CORBA::ULong get_number_of_entities(const char* target_element_path)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException)
    {
        return 0;
    }
    
    

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/get_all_entity_names:1.0
     * operation
     */
    void get_all_entity_names (const char* target_element_path,
                               CORBA::ULong how_many,
                               CdmwPlatformMngt::ElementNames_out entity_names,
                               CdmwPlatformMngt::ElementNamesIterator_out rest)
        throw(CdmwPlatformMngt::ManagedElementNotFound,
              CORBA::SystemException)
    {
    }
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/get_entity:1.0
     * operation
     */
    char* get_entity(const char* target_element_path,
                     const char* entity_name)
        throw(CdmwPlatformMngt::ManagedElementNotFound,
              CdmwPlatformMngt::EntityNotFound,
              CORBA::SystemException)
    {
        return "";
    }
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/get_entities:1.0
     * operation
     */
    CORBA::Boolean get_entities (const char* target_element_path,
                                 const CdmwPlatformMngt::ElementNames& entity_names,
                                 CdmwPlatformMngt::ElementPaths_out entities)
        throw(CdmwPlatformMngt::ManagedElementNotFound,
              CORBA::SystemException)
    {
        return false;
    }
    

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/get_all_entities:1.0
     * operation
     */
    void get_all_entities(const char* target_element_path,
                          CORBA::ULong how_many,
                          CdmwPlatformMngt::ElementPaths_out entities,
                          CdmwPlatformMngt::ElementPathsIterator_out rest)
        throw(CdmwPlatformMngt::ManagedElementNotFound,
              CORBA::SystemException)
    {
    }    



    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/add_entity:1.0
     * operation
     */  
    char* add_entity (const char* target_element_path,
                      const CdmwPlatformMngt::EntityDef & entity_def)
        throw(CdmwPlatformMngt::ManagedElementNotFound,
              CdmwPlatformMngt::EntityAlreadyExists,
              CORBA::SystemException)
    {
        return "";
    }
    


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/EntityContainerAccessor/remove_entity:1.0
     * operation
     */
    void remove_entity(const char* target_element_path,
                       const char* entity_name)
        throw(CdmwPlatformMngt::ManagedElementNotFound,
              CdmwPlatformMngt::EntityNotFound,
              CORBA::SystemException)
    {
    }
    
};
    
} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_PROXY_SIM_HOST_PROXY_IMPL_HPP

