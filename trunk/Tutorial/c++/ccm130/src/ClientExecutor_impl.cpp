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
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>


#include "ClientExecutor_impl.hpp"
#include "HelloInfo_impl.hpp"


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
        // ========================================================
        // Get the naming and repository
        // ========================================================
        std::cout << "Get the repository..." << std::endl;
        CdmwNamingAndRepository::Repository_var repository = 
                   Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
                   
                   
        // ======================================================  
        // get the default root context
        // ======================================================
        CosNaming::NamingContext_var def_root_context =
             repository->resolve_root_context(
                             CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
        
        // ======================================================                      
        // create a naming interface on default root context
        // ====================================================== 
        Cdmw::NamingAndRepository::NamingInterface ni_defRoot (
                                                         def_root_context.in());
        
        
        
	    
        // ========================================================
        // get server component from name service
        // ======================================================== 
        std::cout << "Get the server reference..." << std::endl;
        CORBA::Object_var objref1 = ni_defRoot.resolve ("MyNaming/Tutorial/HelloServer");

	    Server_var helloServer = Server::_narrow(objref1.in());
             
	    // ========================================================
        // get client component from name service
        // ========================================================
        std::cout << "Get the client reference..." << std::endl; 
        CORBA::Object_var objref2 = ni_defRoot.resolve ("MyNaming/Tutorial/HelloClient");

	    Client_var helloClient = Client::_narrow(objref2.in());
	    
	    
	    
	    // ========================================================
        // Get the server event sink
        // ========================================================
        std::cout << "Get the server event sink..." << std::endl;
        HelloInfoConsumer_var consumer = helloServer->get_consumer_received_info();
        
        // ========================================================
        // Subscribe to server consumer
        // ========================================================
        std::cout << "Subscribe to the server..." << std::endl;
        Components::Cookie_var cookie = 
                  helloClient->subscribe_info_to_send (consumer.in());
        
        
        // ===============================================================
        // push the hello info event to the server
        // ===============================================================    
        std::cout << std::endl;
        std::cout << ">>>> Push the hello info event ..." << std::endl;
    
        // create the hello info event
        HelloInfo_var ev = new HelloInfo_impl();
        ev->text (">>>>--- Hello from client ---<<<<");
    
        // then push it
        m_session_context->push_info_to_send (ev.in());            
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
