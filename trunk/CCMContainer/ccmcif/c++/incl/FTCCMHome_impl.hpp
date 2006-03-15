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


#ifndef INCL_CCM_CIF_FT_CCMHOME_IMPL_HPP
#define INCL_CCM_CIF_FT_CCMHOME_IMPL_HPP

#include <map>

#include <CCMContainer/ccmcif/CCMHome_impl.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>


namespace Cdmw {

namespace CCM {

namespace CIF {



/*
 * This class is the base class for home embedded in a Service or Session 
 * container (using a SessionContext) which creates FT replicated components.
 */
   
class FTCCMHome_impl : public CCMHome_impl
{
public:
    /**
    * Purpose:
    * <p>
    *   Constructor.
    *
    *@param rep_id       The home repository ID
    *@param comp_rep_id  The component repository ID
    *@param deactivator  The Object Deactivator responsible to 
    *                    deactivate home at its removal.
    *@param config       The config values used to specify if
    *                    the home is FT or not
    *                    
    *@exception Any CORBA Exception.
    */ 
    FTCCMHome_impl(const std::string&                         rep_id,
                   const std::string&                         comp_rep_id,
                   const Cdmw::LifeCycle::ObjectDeactivator&  deactivator,
                   const Components::ConfigValues&            config)
       throw(CORBA::SystemException);

    virtual ~FTCCMHome_impl() throw();
    
   /**
    * Purpose:
    * <p>This method is called by Container after Home creation.
    * 
    *@param ctxt Component context given by container.
    *
    *@exception Components::CCMException Any Component exception
    *@exception CORBA::SystemException Any CORBA system exception
    */
    void set_context(CdmwCcmContainer::CCM2Context_ptr ctxt)
       throw (Components::CCMException,
              CORBA::SystemException);

protected:
   /**
    * Purpose:
    * <p>
    *   This creates new component reference and save FT group info.
    * 
    *@return A reference on the new component.
    *@exception Components::CreateFailure if application errors.
    *@exception Any other CORBA Exception.
    *
    */ 
    CORBA::Object_ptr create_component_ref(
          const Components::ConfigValues& config, 
          Components::EnterpriseComponent_ptr  comp_exec = 
             Components::EnterpriseComponent::_nil())
        throw(Components::CreateFailure,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    *   This deletes FT group info for the specified component.
    * 
    *@param A component Object Id
    */ 
    virtual void delete_component_info(const std::string& comp_oid);

    /**
    * Purpose:
    * <p>
    *   This finds FT group info for the specified component.
    * 
    *@param A component Object Id
    */ 
    virtual CORBA::Object_ptr find_component_group(const std::string& comp_oid)
        throw (CORBA::SystemException);


private:
    // Hide copy constructor and =operator
    FTCCMHome_impl(const FTCCMHome_impl&);
    void operator=(const FTCCMHome_impl&);
    
    // Component group map with key = stringified oid.
    typedef std::map<std::string, 
                     ::FT::ObjectGroup_var, 
                     std::less<std::string> > ComponentGroupMap;

    ComponentGroupMap  m_comp_groups;
    
}; // end class FTCCMHome_impl

} // End of namespace CIF

} // End of namespace CCM

} // End of namespace Cdmw

#endif // INCL_CCM_CIF_FT_CCMHOME_IMPL_HPP
