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


#ifndef INCL_FTSYSTMNGT_HOST_PROXY_IMPL_HPP
#define INCL_FTSYSTMNGT_HOST_PROXY_IMPL_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/platformftaccessorlibrary/ManagedElementAccessor_impl.hpp"
#include "SystemMngt/platformftaccessorlibrary/RWEntityContainerAccessor_impl.hpp"

#include "SystemMngt/idllib/CdmwFTSystMngtHost.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
 *Purpose:
 *<p> Provides the host access.
 */
class HostAccessor_impl : public virtual POA_CdmwFTSystMngt::HostAccessor,
                          public ManagedElementAccessor_impl,
                          public RWEntityContainerAccessor_impl
{

private:

    /**
     *Purpose:
     *<p> Get the Host object.
     */
    CdmwPlatformMngt::Host_ptr get_host (const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound);

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    HostAccessor_impl(
        HostAccessor_impl& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    HostAccessor_impl& operator=(
        const HostAccessor_impl& rhs);
        
        
public:

    /**
     *Purpose:
     *<p> Constructor.
     *
     */
    HostAccessor_impl(
        CdmwPlatformMngt::ElementRepository_ptr element_repository)
            throw();

    /**
     *Purpose:
     *<p> Destructor.
     */
    ~HostAccessor_impl();
    
    

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
     *<p> facade to get an attribute
     *
     */

     void get_attribute( const char * target_element_path, const char * attr_name, const CORBA::Any_out ret_attr , CORBA::Char flag )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void set_attribute( const char *  target_element_path, const char * attr_name, const CORBA::Any & attr, CORBA::Char flag  )  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFTSystMngt/HostAccessor/get_host_status_info:1.0
     * operation;
     * implemented in Selex in date 20-7-2005
     */
    char * get_host_status_info(const char* target_element_path)
             throw (CdmwPlatformMngt::ManagedElementNotFound,
                    CORBA::SystemException);
    

     //get_monitoring_parameters()
    CdmwPlatformMngt::CompleteMonitoringInfo get_monitoring_parameters(const char* target_element_path) throw (CdmwPlatformMngt::ManagedElementNotFound,CORBA::SystemException);

    //set_monitoring_parameters()
    void set_monitoring_parameters(const CdmwPlatformMngt::CompleteMonitoringInfo& monitoring_parameters, const char* target_element_path) throw (CdmwPlatformMngt::ManagedElementNotFound,CORBA::SystemException);

  //get_global_supervision_indicator()
    bool get_global_supervision_indicator(const char* target_element_path)  throw (CdmwPlatformMngt::ManagedElementNotFound,CORBA::SystemException); 

    //set_global_supervision_indicator method
    void set_global_supervision_indicator(bool global_supervision_indicator,const char* target_element_path)  throw (CdmwPlatformMngt::ManagedElementNotFound,CORBA::SystemException);

  //get_host_name
    char * get_host_name(const char* target_element_path)  throw (CdmwPlatformMngt::ManagedElementNotFound,CORBA::SystemException); 

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
        

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_FTSYSTMNGT_HOST_PROXY_IMPL_HPP

