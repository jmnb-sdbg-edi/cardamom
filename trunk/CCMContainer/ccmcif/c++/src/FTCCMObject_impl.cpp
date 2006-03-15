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

// Cdmw files
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/common/Assert.hpp>

// Cdmw CCM files
#include <CCMContainer/ccmcif/FTCCMObject_impl.hpp>
#include <CCMContainer/ccmcommon/CCMUtils.hpp>


namespace Cdmw {

namespace CCM {

namespace CIF {

FTCCMObject_impl::FTCCMObject_impl(const std::string&  comp_oid,
                                   Context*            ctx,
                                   const std::string&  rep_id,
                                   CORBA::Object_ptr   comp_ref,
                                   bool                is_monolithic)
   : CCMObject_impl(comp_oid, ctx, rep_id, comp_ref, is_monolithic)
{
    try
    {
        CORBA::Object_var ft_current_obj = 
           m_context->resolve_initial_references("FTCurrent");

        m_ft_current = CdmwFT::Current::_narrow(ft_current_obj.in());
        CDMW_ASSERT(!CORBA::is_nil(m_ft_current.in()));

        CORBA::Object_var ft_group_rep_obj =
           m_context->resolve_initial_references("FTGroupRepository");
    
        m_ft_group_rep = 
           CdmwFT::Location::GroupRepository::_narrow(ft_group_rep_obj.in());
        CDMW_ASSERT(!CORBA::is_nil(m_ft_group_rep.in()));

        CORBA::Object_var ft_rep_mng_obj = 
           m_context->resolve_initial_references("ReplicationManager");

        m_ft_rep_mng = CdmwFT::ReplicationManager::_narrow(ft_rep_mng_obj.in());
        CDMW_ASSERT(!CORBA::is_nil(m_ft_rep_mng.in()));
    }
    catch (const CdmwCcmContainer::CCM2Context::InvalidName& )
    {
        // FTCurrent/FTGroupRepository not defined, we are not in FT process, 
        CDMW_ASSERT(false);
    }
    catch (const CORBA::SystemException& e)
    {
        PRINT_ERROR("CORBA::SystemException raised : " << e);
        CDMW_ASSERT(false);
    }

}
 
FTCCMObject_impl::~FTCCMObject_impl()
{
    PRINT_INFO("FTCCMObject_impl destructor called!");
}

/**
* Purpose:
* <p>
*   return a group reference if the current request is invoked on a FT group reference.
*   Otherwise return the same reference.
* 
*@param obj_ref       The facet/consumer/component reference.
*
*@return  The reference to be exported to clients
*/ 
CORBA::Object_ptr 
FTCCMObject_impl::get_object_ref(const CORBA::Object_ptr obj_ref)
   throw (CORBA::SystemException)
{
    CORBA::Object_var result = CORBA::Object::_duplicate(obj_ref);

    ::FT::ObjectGroupId group_id = 0;
    try
    {
        CDMW_ASSERT(!CORBA::is_nil(m_ft_current.in()));
        
        // test if we are able to get group id: if OK it significates it is a FT 
        // request on a group ref. Otherwise, it will raise NoContext exception.
        group_id = m_ft_current->get_object_group_id();
    
        // No NoContext exception raised: this is a FT request on a group ref.
        // Find and return the corresponding group reference of the given object ref.
        // To do so, we use the FTGroupRepository to fing the group from the object 
        // ref instead from the group_id on which the request is done. It may be
        // different in segmented case.
        if (!CORBA::is_nil(m_ft_group_rep.in()))
        {
            result = m_ft_group_rep->get_object_group(obj_ref);
        }
    }
    catch (const CdmwFT::Current::NoContext&)
    {
        // This is not a FT request, return the object ref
    }
    catch (const FT::MemberNotFound& )
    {
        // Object ref is not a member of a group
        // May be the location manager has not been advised by the replication 
        // yet. We can check replciation manager.
        // Currently, component are monolithic if FT. So the group id of the 
        // request is the one of the specified object ref.
        // TODO add an operation on replication manager to find the group ref 
        // from a member ref.
        try 
        {
           if (!CORBA::is_nil(m_ft_rep_mng.in())) 
           {
               result = m_ft_rep_mng->get_object_group_ref_from_gid(group_id);
           }
        }
        catch (const ::FT::ObjectGroupNotFound&)
        {
            // Shall not happen: it means the group has not been created by this 
            // replication manager.
            PRINT_ERROR("FT::ObjectGroupNotFound raised!");
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch (const CORBA::SystemException& e)
        {
            PRINT_ERROR("CORBA::SystemException raised : " << e);
            throw;
        }
    }
    catch (const CORBA::SystemException& e)
    {
        PRINT_ERROR("CORBA::SystemException raised : " << e);
        throw;
    }

    return result._retn();
}

} // end namespace CIF

} // end namespace CCM

} // end namespace Cdmw
