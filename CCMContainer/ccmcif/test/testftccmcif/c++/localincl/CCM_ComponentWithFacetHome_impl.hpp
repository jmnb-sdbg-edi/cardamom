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

#ifndef ___TEST_FTCCMCIF_COMPONENT_WITH_FACET_HOME_IMPL_HPP__
#define ___TEST_FTCCMCIF_COMPONENT_WITH_FACET_HOME_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include "testftccmcif/ftccmcif_cif.skel.hpp"

namespace TestFtCcmCif
{

//
// IDL:acme.com/TestFtCcmCif/ComponentsModule/CCM_ComponentWithFacetHome:1.0
//
class CCM_ComponentWithFacetHome_impl : 
    virtual public ComponentsModule::CCM_ComponentWithFacetHome,
    virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    
    CCM_ComponentWithFacetHome_impl(const CCM_ComponentWithFacetHome_impl&);
    void operator=(const CCM_ComponentWithFacetHome_impl&);

public:

    CCM_ComponentWithFacetHome_impl();
    ~CCM_ComponentWithFacetHome_impl();

    //
    // IDL:acme.com/TestFtCcmCif/ComponentsModule/CCM_ComponentWithFacetHomeImplicit/create:1.0
    //
    virtual Components::EnterpriseComponent_ptr create()
        throw(Components::CCMException,
              CORBA::SystemException);
};

}; // End of namespace CcmFtTest 


#endif // ___TEST_FTCCMCIF_COMPONENT_WITH_FACET_HOME_IMPL_HPP__


