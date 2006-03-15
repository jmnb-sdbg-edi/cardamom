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


#include <iostream>
#include <ClientExecutor_impl.hpp>

#include "Foundation/commonsvcs/naming/NamingInterface.hpp"


namespace Hello
{
    
//
// IDL:acme.com/Hello/CCM_Client:1.0
//
ClientExecutor_impl::ClientExecutor_impl()
    : m_session_context(CCM_Client_Context::_nil()),
      m_client_thread(NULL)
{
}

ClientExecutor_impl::~ClientExecutor_impl()
{
    std::cout << "===> ClientExecutor_impl::~ClientExecutor_impl called!" << std::endl;
}


//
// set_session_context function : it fixes the session context of the component 
// (called by the container)
//
void ClientExecutor_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "===> set_session_context called!" << std::endl;
    
    m_session_context = CCM_Client_Context::_narrow(ctx); 
}


//
// ccm_activate function : it actives the component (called by the container)
//
void ClientExecutor_impl::ccm_activate()
    throw(CORBA::SystemException)
{
    std::cout << "===> ccm_activate called!" << std::endl;
    
    try
    {
        // Launch the test in a thread
        Display_var display = m_session_context->get_connection_client_receptacle();
        m_client_thread = new ClientThread(display.in());
        
        m_client_thread->start();
        
    } catch (const CORBA::UserException & e) {
        std::cout << "CORBA User Exception : \n" << e << std::endl;
    } catch (const CORBA::SystemException & e) {
        std::cout << "CORBA System Exception : \n" << e << std::endl;
    } catch (...) {
        std::cout << "Unexpected exception raised!" << std::endl;
    }
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//

void ClientExecutor_impl::ccm_passivate()
    throw(CORBA::SystemException)
{
    std::cout << "===> ccm_passivate called!" << std::endl;
    m_client_thread->stop();
    m_client_thread->join();
    
    delete m_client_thread;
    m_client_thread=NULL;
} 



//
// ccm_remove function : it destroys the component (call by the container)
//
void ClientExecutor_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout << "===> ccm_remove called!" << std::endl;
}


}; // End of namespace Hello


