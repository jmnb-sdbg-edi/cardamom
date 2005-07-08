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
#include "ClientExecutor_impl.hpp"
#include "HelloInfo_impl.hpp"


namespace Hello
{


ClientActivationThread::ClientActivationThread(ClientExecutor_impl* p_client)
		 : m_client(p_client)
{
}

ClientActivationThread::~ClientActivationThread() throw() 
{
}


  
void ClientActivationThread::run() throw()
{
    try
    {
        // ========================================================
        // get the display connection to the server
        // ========================================================    
        std::cout << "Connect to the display facet..." << std::endl;
    
        Display_var display = 
            m_client->m_session_context->get_connection_client_receptacle();
            
        // ========================================================
        // call server display
        // ======================================================== 
        std::cout << "Call the server display..." << std::endl;
        
        display->display_hello();
    }
    
    catch (const CORBA::UserException & e) 
    {
        std::cout << "CORBA User Exception : \n" << e << std::endl;
    }
    
    catch (const CORBA::SystemException & e) 
    {
        std::cout << "CORBA System Exception : \n" << e << std::endl;
    }
    
    catch (...)
	{
	    std::cout << "Unexpected exception raised!" << std::endl;
	}
}


    
//
// IDL:thalesgroup.com/Hello/CCM_Client:1.0
//
ClientExecutor_impl::ClientExecutor_impl()
	: m_session_context(CCM_Client_Context::_nil()),
	  m_is_active(false)
{
    // create thread
	m_activation_thread = new ClientActivationThread(this);
}

ClientExecutor_impl::~ClientExecutor_impl()
{
    std::cout << "===> ClientExecutor_impl::~ClientExecutor_impl called!" << std::endl;
    
    delete m_activation_thread;
}


//
// set_session_context function : it fixes the session context of the component (call by the container)
//
void ClientExecutor_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "===> set_session_context called!" << std::endl;
   
    m_session_context = CCM_Client_Context::_narrow(ctx); 
}


//
// ccm_activate function : it actives the component (call by the container)
//
void ClientExecutor_impl::ccm_activate()
   throw(CORBA::SystemException)
{
    std::cout << "===> ccm_activate called!" << std::endl;
    
    // if thread not yet started, start it
    if (! m_is_active)
    {
        std::cout << "===> start activation thread!" << std::endl;
        
        m_is_active = true;
      
        m_activation_thread->start();
    }
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//

void ClientExecutor_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
   std::cout << "===> ccm_passivate called!" << std::endl;
   
   if (m_is_active) 
   {
       // thread has been started so stop it then unlock mutex
       m_is_active = false;

       // join the thread
       try
       {
	       m_activation_thread->join();
	       
	       std::cout  << "===> thread has join" << std::endl;
       }
       catch (...)
       {
           std::cout << "Unexpected exception raised!" << std::endl;
       }
    }
       
} 



//
// ccm_remove function : it destroys the component (call by the container)
//
void ClientExecutor_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout << "===> ccm_remove called!" << std::endl;
}


//
// IDL:thalesgroup.com/Hello/CCM_Server/push_received_info:1.0
//
void ClientExecutor_impl::push_received_info(Hello::HelloInfo* event)
        throw(CORBA::SystemException)
{
    std::cout << "===> push_received_info called!" << std::endl;
           
    event->display_text();
}



}; // End of namespace Hello
