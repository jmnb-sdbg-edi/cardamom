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


#include <iostream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>

// Cdmw CCM Files
#include <CCMContainer/idllib/CdmwDeployment.stub.hpp>
#include <CCMContainer/ccmcommon/CCMUtils.hpp>
#include <CCMContainer/ccmcif/FTCCMHome_impl.hpp>

namespace Cdmw {

namespace CCM {

namespace CIF {


/* This class inherits from CCMHome_impl and manages FT aspects for any CCM 
 * home. This is the base class for CARDAMOM generated CCM home in an FT 
 * environement.
 */
FTCCMHome_impl::FTCCMHome_impl(
      const std::string&                         rep_id,
      const std::string&                         comp_rep_id,
      const Cdmw::LifeCycle::ObjectDeactivator&  deactivator,
      const Components::ConfigValues&            config)
  throw(CORBA::SystemException)
  : CCMHome_impl(rep_id, comp_rep_id, deactivator, config, true)
{
    CORBA::Any_var ft_rep_style_any;
    bool ft_config_value_found = 
       read_config_value(CdmwDeployment::FAULT_TOLERANCE_REPLICATION_STYLE, 
                         config, 
                         ft_rep_style_any); 

    if (ft_config_value_found) 
    {
        CdmwDeployment::FTReplicationStyleValue ft_rep_style;
        if (ft_rep_style_any.in() >>= ft_rep_style) 
        {       
            if (ft_rep_style != CdmwDeployment::WARM_PASSIVE)  
            {
                _set_ref_count(0);
                PRINT_ERROR("Unsupported FT Configuration values for FT home creation");
                throw CORBA::BAD_PARAM(
                      OrbSupport::BAD_PARAMInvalidConfigValues, 
                      CORBA::COMPLETED_NO);
            }
        }
        else 
        {
            _set_ref_count(0);
            PRINT_ERROR("Cannot read FT Configuration values for FT home creation");
            throw CORBA::BAD_PARAM(
                  OrbSupport::BAD_PARAMInvalidConfigValues, 
                  CORBA::COMPLETED_NO);
        }
    }
}

FTCCMHome_impl::~FTCCMHome_impl() 
    throw()
{
    PRINT_INFO("FTCCMHome_impl destructor called!");
}

void 
FTCCMHome_impl::set_context(CdmwCcmContainer::CCM2Context_ptr ctxt)
throw (Components::CCMException,
       CORBA::SystemException)
{
    // call 
    CCMHome_impl::set_context(ctxt);

    try
    {
        CORBA::Object_var obj_ref 
             = m_context->resolve_initial_references("FTCurrent");
    }
    catch (const CdmwCcmContainer::CCM2Context::InvalidName& )
    {
        // Home is FT, process should be FT
        // This pb should be detected when installing the Home. If the user use
        // CDMW Code generator, all is ok (no FT allocator) but if he modified
        // the generated code...
        // Then the container shall check the FTCurrent before creating the home.
        PRINT_ERROR("set_context: FT Home is waiting for a FT context from a FT process");
        throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMWrongContextType,
                               CORBA::COMPLETED_NO);
    }
}


CORBA::Object_ptr
FTCCMHome_impl::create_component_ref(const Components::ConfigValues& config, Components::EnterpriseComponent_ptr  comp_exec)
    throw(Components::CreateFailure,
          CORBA::SystemException)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    CDMW_ASSERT(m_context);

    CORBA::Any_var ft_group_ref_any;
    bool ft_config_value_found 
       = read_config_value(CdmwDeployment::FAULT_TOLERANCE_GROUP_REF, 
                           config, 
                           ft_group_ref_any); 
    
    ::FT::ObjectGroup_var ft_group_ref = ::FT::ObjectGroup::_nil();

    if (ft_config_value_found)
    {
        if (ft_group_ref_any >>= CORBA::Any::to_object(ft_group_ref))
        {
            if (CORBA::is_nil(ft_group_ref.in()))
            {
                PRINT_ERROR("Wrong FT Configuration values for FT component creation: group_ref is nil!");
                throw Components::CreateFailure
                   (CdmwDeployment::WRONG_FT_CONFIG_VALUES);
            }
            // all is ok ft_group_ref will be saved below in comp_info
        }
        else
        {
            PRINT_ERROR("Wrong FT Configuration values for FT component creation");
            throw Components::CreateFailure
               (CdmwDeployment::WRONG_FT_CONFIG_VALUES);
        }
    }
    else
    {
        PRINT_ERROR("Missing FT Configuration values for FT component creation");
        throw Components::CreateFailure
           (CdmwDeployment::MISSING_FT_CONFIG_VALUES);
    }

    std::string comp_oid;
    CORBA::Object_var result = create_component_ref_i(comp_oid, comp_exec);

    m_comp_groups.insert(ComponentGroupMap::value_type(comp_oid, 
                                                       ft_group_ref));
    PRINT_INFO("Add group ref in map for component " << comp_oid);

    return result._retn();
}

void 
FTCCMHome_impl::delete_component_info(const std::string& comp_oid)
{
    PRINT_INFO("delete group ref in map for component " << comp_oid);
    m_comp_groups.erase(comp_oid);

    CCMHome_impl::delete_component_info(comp_oid);
}

CORBA::Object_ptr 
FTCCMHome_impl::find_component_group(const std::string& comp_oid)
   throw (CORBA::SystemException)
{
    PRINT_INFO("find group ref in map for component " << comp_oid);
    ComponentGroupMap::iterator comp_pos = m_comp_groups.find(comp_oid);
        
    if (comp_pos  == m_comp_groups.end()) 
    {
        PRINT_ERROR("No group found for this component");
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                      CORBA::COMPLETED_NO); 
    }

    ComponentGroupMap::value_type& comp_value = *comp_pos;
    
    return CORBA::Object::_duplicate(comp_value.second.in());
}

} // End of namespace CIF

} // End of namespace CCM

} // End of namespace Cdmw

