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


#ifndef ___TEST_FTCCMCIF_COMPONENT_WITH_FACET_IMPL_HPP__
#define ___TEST_FTCCMCIF_COMPONENT_WITH_FACET_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include "testftccmcif/user_ftccmcif.skel.hpp"


namespace TestFtCcmCif
{
//
// IDL:acme.com/TestFtCcmCif/ComponentsModule/CCM_ComponentWithFacet:1.0
//
class CCM_ComponentWithFacet_impl : 
    virtual public User_CCM_ComponentWithFacet,
    virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    
    CCM_ComponentWithFacet_impl(const CCM_ComponentWithFacet_impl&);
    void operator=(const CCM_ComponentWithFacet_impl&);

    ComponentsModule::CCM_ComponentWithFacet_Context_var m_session_context;

public:

    CCM_ComponentWithFacet_impl();
    ~CCM_ComponentWithFacet_impl();

    
      
    //
    // facet the_facet
    //
    virtual CCM_TestInterface_ptr get_the_facet ()
        throw (CORBA::SystemException);
    

    //
    // Session component specific operations
    //
    void set_session_context(Components::SessionContext_ptr ctx)
        throw(CORBA::SystemException);
    void ccm_activate()
        throw(CORBA::SystemException);
    void ccm_passivate()
        throw(CORBA::SystemException);
    void ccm_remove()
        throw(CORBA::SystemException);


    ComponentsModule::CCM_ComponentWithFacet_Context* get_context()
    {
        return ::TestFtCcmCif::ComponentsModule::CCM_ComponentWithFacet_Context::_duplicate(m_session_context.in());
    }
      
};


}; // End of namespace TestFtCcmCif

#endif // ___TEST_FTCCMCIF_COMPONENT_WITH_FACET_IMPL_HPP__

