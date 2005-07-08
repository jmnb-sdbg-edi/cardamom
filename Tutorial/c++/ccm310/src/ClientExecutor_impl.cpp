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


#include <Repository/naminginterface/NamingInterface.hpp>

#include "ClientExecutor_impl.hpp"
#include "StatusInfo_impl.hpp"
#include <iostream>


namespace Hello
{
    
//
// IDL:thalesgroup.com/Hello/CCM_Client:1.0
//
ClientExecutor_impl::ClientExecutor_impl()
	: m_session_context(CCM_Client_Context::_nil())
{
}

ClientExecutor_impl::~ClientExecutor_impl()
{
    std::cout << "===> ClientExecutor_impl::~ClientExecutor_impl called!" << std::endl;
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
    
    try
    {
		  // ===============================================================
        // push the status info event to the server
        // ===============================================================    
        std::cout << std::endl;
        std::cout << ">>>> Push the status info event ..." << std::endl;
    
        // create the status info event
        StatusInfo_var ev = new StatusInfo_impl();
        ev->text (">>>>--- client activated ---<<<<");
    
        // then push it
        m_session_context->push_info(ev.in());            

        // ========================================================
        // get the display connection to the server
        // ========================================================    
        std::cout << "Get the display facet from server 1..." << std::endl;
    
        Display_var display = m_session_context->get_connection_client_receptacle();
            
        // ========================================================
        // call server 1 display
        // ======================================================== 
        std::cout << "Call the server 1 display..." << std::endl;
        
        display->display_hello();
        
        
        // ========================================================
        // Get the naming and repository
        // ========================================================
        std::cout << "Get the NameService connection..." << std::endl;
        CosNaming::NamingContext_var nc_root = 
                      m_session_context->get_connection_to_naming();
        
        
        
        // ======================================================                      
        // create a naming interface on root context
        // ====================================================== 
        Cdmw::NamingAndRepository::NamingInterface ni_root (nc_root.in());

   
        // ========================================================
        // get server 1 component from name service
        // ======================================================== 
        std::cout << "Get the server 1 reference..." << std::endl;
        CORBA::Object_var objref1 = ni_root.resolve ("MyNaming/Tutorial/HelloServer1");

	    Server_var helloServer1 = Server::_narrow(objref1.in());
        
        // ========================================================
        // get server 2 component from name service
        // ======================================================== 
        std::cout << "Get the server 2 reference..." << std::endl;
        CORBA::Object_var objref2 = ni_root.resolve ("MyNaming/Tutorial/HelloServer2");

	    Server_var helloServer2 = Server::_narrow(objref2.in());
			     
	    // ========================================================
        // get client component from name service
        // ========================================================
        std::cout << "Get the client reference..." << std::endl; 
        CORBA::Object_var objref3 = ni_root.resolve ("MyNaming/Tutorial/HelloClient");

	    Client_var helloClient = Client::_narrow(objref3.in());
			     
	    // ========================================================
        // Disconnect from server 1
        // ========================================================
        std::cout << "Disconnect from the server 1..." << std::endl;
        helloClient->disconnect_client_receptacle ();

		  // ===============================================================
        // push the status info event to the server
        // ===============================================================    
        std::cout << std::endl;
        std::cout << ">>>> Push the status info event ..." << std::endl;
    
        // create the status info event
        ev->text (">>>>--- server1 disconnected ---<<<<");
    
        // then push it
        m_session_context->push_info(ev.in());       
        
        // ========================================================
        // Get the server 2 facet
        // ========================================================
        std::cout << "Get the server 2 facet..." << std::endl;
        Display_var display2 = helloServer2->provide_server_facet();
        
        // ========================================================
        // Connect to server 2
        // ========================================================
        std::cout << "Connect to the server 2..." << std::endl;
        helloClient->connect_client_receptacle (display2.in());

		  // ===============================================================
        // push the status info event to the server
        // ===============================================================    
        std::cout << std::endl;
        std::cout << ">>>> Push the status info event ..." << std::endl;
    
        // create the status info event
        ev->text (">>>>--- server2 connected ---<<<<");
    
        // then push it
        m_session_context->push_info(ev.in());   
        
        
        // ==========================================================
        // get the new display connection to the server using context
        // ==========================================================    
        std::cout << "Get the display facet from context..." << std::endl;
    
        display = m_session_context->get_connection_client_receptacle();
            
        // ========================================================
        // call server 2 display
        // ======================================================== 
        std::cout << "Call the server 2 display..." << std::endl;
        
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
// ccm_passivate function : it makes stop the component (call by the container)
//

void ClientExecutor_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
   std::cout << "===> ccm_passivate called!" << std::endl;
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
