/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */
#include <iostream>
#include <fstream>

#include "testftccmcif/CCM_ComponentWithFacet_impl.hpp"

#include <Foundation/ossupport/OS.hpp>


//
// IDL:thalesgroup.com/TestFtCcmCif/CCM_ComponentWithFacet:1.0
//
TestFtCcmCif::CCM_ComponentWithFacet_impl::CCM_ComponentWithFacet_impl()
{
}

TestFtCcmCif::CCM_ComponentWithFacet_impl::~CCM_ComponentWithFacet_impl()
{
    std::cout << "CCM_ComponentWithFacet_impl::~CCM_ComponentWithFacet_impl called!" << std::endl;
}


//
// facet the_facet
//
TestFtCcmCif::CCM_TestInterface_ptr 
TestFtCcmCif::CCM_ComponentWithFacet_impl::get_the_facet ()
    throw (CORBA::SystemException)
{
    return TestFtCcmCif::CCM_TestInterface::_duplicate(this);
}



//
// set_session_context function : it fixes the session context of the component (call by the container)
//
void
TestFtCcmCif::CCM_ComponentWithFacet_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "Set_session_context called!" << std::endl;
   
    m_session_context = ::TestFtCcmCif::CCM_ComponentWithFacet_Context::_narrow(ctx); 
}


//
// ccm_activate function : it actives the component (call by the container)
//
void
TestFtCcmCif::CCM_ComponentWithFacet_impl::ccm_activate()
   throw(CORBA::SystemException)
{
    std::ofstream os("CCM_ComponentWithFacet.stt");
    os << "Ccm_activate called!";
    os.close();

    std::cout << "Ccm_activate called!" << std::endl;
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//

void
TestFtCcmCif::CCM_ComponentWithFacet_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
    std::ofstream os("CCM_ComponentWithFacet.stt");
    os << "Ccm_passivate called!";
    os.close();

    std::cout << "Ccm_passivate called!" << std::endl;
} 



//
// ccm_remove function : it destroys the component (call by the container)
void
TestFtCcmCif::CCM_ComponentWithFacet_impl::ccm_remove()
   throw(CORBA::SystemException)
{
    std::ofstream os("CCM_ComponentWithFacet.stt");
    os << "Ccm_remove called!";
    os.close();

    std::cout << "Ccm_remove called!" << std::endl;
}


