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


#include <Foundation/common/Exception.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>


#include "testami/ClientProcessControl.hpp"



namespace Cdmw
{
namespace TestAMI
{


ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : Testable("Test AMI"),
      m_orb (CORBA::ORB::_duplicate(orb)),
      pServerHandler_impl(NULL)
{
}

ClientProcessControl::~ClientProcessControl()
    throw()
{
    if (pServerHandler_impl != NULL)
        delete pServerHandler_impl;
}
  


// process initialisation called by platformmngt     
void ClientProcessControl::on_initialise (
        const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
{
    // create the ServerHandler servant
    pServerHandler_impl = new Cdmw::TestAMI::ServerHandler_impl(m_orb.in());
    
    // Get the root POA 
    CORBA::Object_var rootPOAobj = m_orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(rootPOAobj.in());

    // activate servant on POA (ref count is incremented)
    PortableServer::ObjectId_var oid = 
        rootPOA->activate_object(pServerHandler_impl);
                   
    CORBA::Object_var object = rootPOA->id_to_reference(oid.in());
    m_serverHandler = Cdmw::TestAMI::AMI_ServerHandler::_narrow(object.in());
    
}
  
// process to run called by platformmngt    
void ClientProcessControl::on_run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
          CORBA::SystemException)
{
    // 
    // Retrive Server
    //
    std::cout << "Retrive Server object from Repository" << std::endl;
    
    // Get the naming and repository from Repository interface
    CdmwNamingAndRepository::Repository_var repository =
        CdmwNamingAndRepository::Repository::_nil();  
    repository = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
     
    // Get Naming Interface of dom1 name domain
    Cdmw::CommonSvcs::Naming::NamingInterface ni =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface (
                        "dom1");
         
    // Get the Server object from repository
    CORBA::Object_var objref = ni.resolve ("Server");

    // get the Server proxy
    m_server = Cdmw::TestAMI::Server::_narrow(objref.in());

    if (CORBA::is_nil(m_server.in()))
    {
        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
                  << "could not _narrow object to type Server" << std::endl;
        throw CdmwPlatformMngt::ProcessDelegate::NotReadyToRun();
    }

    // 
    // start tests
    //
    start();
}


// process to stop called by platformmngt
void ClientProcessControl::on_stop()
    throw(CORBA::SystemException)
{
}



void ClientProcessControl::do_tests()
{
    set_nbOfRequestedTestOK(2);
    
    TEST_INFO("send asynchronous request request_sleeping() to server");
    m_server->sendc_request_sleeping(m_serverHandler.in(), 3000, 3);

    TEST_INFO("waiting result...");
    Cdmw::OsSupport::OS::sleep(6000);

    if (pServerHandler_impl->check_request_sleeping_result() == RESULT_CORRECT) {
        TEST_SUCCEED();
    } else {
        TEST_FAILED();
    }
      
    TEST_INFO("send asynchronous request request_sending_exception() to server");
    m_server->sendc_request_sending_exception(m_serverHandler.in());
    
    TEST_INFO("waiting result...");
    Cdmw::OsSupport::OS::sleep(3000);
    
    if (pServerHandler_impl->check_request_sending_exception_result() == RESULT_CORRECT) {
        TEST_SUCCEED();
    } else {
        TEST_FAILED();
    }
      
    
}





}; // End namespace TestAMI
}; // End namespace Cdmw
