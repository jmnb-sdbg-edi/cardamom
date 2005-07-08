/* =========================================================================== *
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
 * =========================================================================== */

#include "ServerExecutor_impl.hpp"
#include "ServerProcessControl.hpp"

#include "TestPingFacetExecutor_impl.hpp"


namespace Marshalling
{
    
//
// IDL:thalesgroup.com/Marshalling/CCM_Server:1.0
//
ServerExecutor_impl::ServerExecutor_impl()
{
    m_testCall = new TestPingFacetExecutor_impl;
}

ServerExecutor_impl::~ServerExecutor_impl()
{
    std::cout << "===> ServerExecutor_impl::~ServerExecutor_impl called!" << std::endl;
}

//
// IDL:thalesgroup.com/Marshalling/CCM_Server/get_server_facet:1.0
//
CCM_TestPing_ptr ServerExecutor_impl::get_server_facet()
    throw(CORBA::SystemException)
{
    //std::cout << "===> get_server_facet called!" << std::endl;
    
    return CCM_TestPing::_duplicate(m_testCall.in());
}

// From Session Component

//
// set_session_context function : it fixes the session context of the component (call by the container)
//
void ServerExecutor_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "===> set_session_context called!" << std::endl;
   
    m_session_context = CCM_Server_Context::_narrow(ctx); 
}

//
// ccm_activate function : it actives the component (call by the container)
//
void ServerExecutor_impl::ccm_activate()
   throw(CORBA::SystemException)
{
    std::cout << "===> ccm_activate called!" << std::endl;
    ServerProcessControl::ccm_activated();
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//

void ServerExecutor_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
   std::cout << "===> ccm_passivate called!" << std::endl;
} 


//
// ccm_remove function : it destroys the component (call by the container)
//
void ServerExecutor_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout << "===> ccm_remove called!" << std::endl;
}

}; // End of namespace Marshalling
