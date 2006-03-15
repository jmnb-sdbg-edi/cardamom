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


#ifndef INCL_CCM_CIF_FTCCMOBJECT_IMPL_HPP
#define INCL_CCM_CIF_FTCCMOBJECT_IMPL_HPP

// Cdmw CCM files
#include <CCMContainer/ccmcif/CCMObject_impl.hpp>

// Cdmw FT Files
#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTCurrent.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>


namespace Cdmw {

namespace CCM {

namespace CIF {


/**
*Purpose:
*<p>    [class description]
*    This class implement the FT specificities for CCM component.
*    It will be inherited by all FT components servant implementation.
*
*/
class FTCCMObject_impl :  public CCMObject_impl
{
public:

   /**
    * Purpose:
    * <p>
    *   Constructor.
    *
    *@param comp_oid       The component oid
    *@param ctx            The component extended context
    *@param rep_id         The component repository ID
    *@param comp_ref       The component reference.
    *@param is_monolithic  True if the component, its facets and consumers 
    *                      have a single object reference.
    */ 
    FTCCMObject_impl(const std::string&  comp_oid,
                     Context*            ctx, 
                     const std::string&  rep_id,
                     CORBA::Object_ptr   comp_ref,
                     bool                is_monolithic=false);

    ~FTCCMObject_impl();

protected:
    /**
    * Purpose:
    * <p>
    *   returns the corresponding reference of the reference given as parameter. This 
    *   operation shall return group reference of the member given as parameter.
    * 
    *@param obj_ref       The facet/consumer/component reference.
    *@return  The group reference to be exported to clients
    */ 
    CORBA::Object_ptr get_object_ref(const CORBA::Object_ptr obj_ref)
       throw (CORBA::SystemException);

    /**
     * Purpose:
    * <p>
    *   Return true if configuration complete has already been called
    */
    inline bool is_configured() { return m_configured;}

private:
    FTCCMObject_impl(const FTCCMObject_impl&);
    void operator=(const FTCCMObject_impl&);

    CdmwFT::Current_var                   m_ft_current;
    CdmwFT::Location::GroupRepository_var m_ft_group_rep;
    CdmwFT::ReplicationManager_var        m_ft_rep_mng;
    
};  // End CCMObject_impl class

} // End namespace CIF

} // End namespace CCM

} // End namespce Cdmw



#endif // INCL_CCM_CIF_FTCCMOBJECT_IMPL_HPP
