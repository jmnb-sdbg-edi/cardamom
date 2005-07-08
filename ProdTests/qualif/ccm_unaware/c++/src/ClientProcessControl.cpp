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
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>


#include "ccm_unaware/ClientProcessControl.hpp"
#include "ccm_unaware/SimpleInterfaces.stub.hpp"



namespace SimpleCcmServer
{


ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : Testable("Test access to component by ccm-unaware client"),
      m_orb (CORBA::ORB::_duplicate(orb))
{
}

ClientProcessControl::~ClientProcessControl()
    throw()
{
}
  


// process initialisation called by platformmngt     
void ClientProcessControl::on_initialise (
        const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
{
}
  
// process to run called by platformmngt    
void ClientProcessControl::on_run()
    throw(CdmwPlatformMngt::Process::NotReadyToRun, 
          CORBA::SystemException)
{
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
    set_nbOfRequestedTestOK(4);    
    
    // Get the naming and repository from Repository interface
    CdmwNamingAndRepository::Repository_var repository =
        CdmwNamingAndRepository::Repository::_nil();  
    repository = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
     
    // Get Naming Interface of root name domain
    CosNaming::NamingContext_var root =
        repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
    Cdmw::NamingAndRepository::NamingInterface ni(root.in());
        
    // 
    // Retrive NamedServer
    //
    TEST_INFO("Retrive NamedServer object from Repository");
    bool namedServerFound = false;
    CORBA::Object_var serverObj;
    while (!namedServerFound) {
        try {
            serverObj = ni.resolve ("NamedServer");
            namedServerFound = true;
        } catch (const CosNaming::NamingContext::NotFound& e) {
            TEST_INFO("   waiting for ccm deployment to finish...");
            // retry in 3 sec
            Cdmw::OsSupport::OS::sleep(3000);
        }
    }
    
    // get the Server proxy
    SimpleCcmServer::Named_var namedServer = SimpleCcmServer::Named::_narrow(serverObj.in());

    if (CORBA::is_nil(namedServer.in()))
    {
        std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
                  << "could not _narrow object to type Named" << std::endl;
        TEST_FAILED();
        return;
    }
    TEST_SUCCEED();


    // 
    // Retrive NamedServer
    //
    TEST_INFO("Retrive Display object from Repository");
    bool displayFound = false;
    CORBA::Object_var displayObj;
    while (!displayFound) {
        try {
            displayObj = ni.resolve ("Display");
            displayFound = true;
        } catch (const CosNaming::NamingContext::NotFound& e) {
            TEST_INFO("   waiting for ccm deployment to finish...");
            // retry in 3 sec
            Cdmw::OsSupport::OS::sleep(3000);
        }
    }
    
    // get the Server proxy
    SimpleCcmServer::Display_var display = SimpleCcmServer::Display::_narrow(displayObj.in());

    if (CORBA::is_nil(display.in()))
    {
        std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
                  << "could not _narrow object to type Display" << std::endl;
        TEST_FAILED();
        return;
    }
    TEST_SUCCEED();


    //
    // Test request to Named
    //
    TEST_INFO("Test request to Named object");
    try {
        CORBA::String_var name = namedServer->get_name();
        
        if (strcmp("TestName", name.in()) == 0) {
            TEST_SUCCEED();
        } else {
            std::cout << "Name is not the one expected: " 
                      << name.in() << std::endl;		   
            TEST_FAILED();
        }
        
    } catch (const CORBA::Exception& e) {
        std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
                  << "Exception during namedServer->get_name():\n" 
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e)
                  << std::endl;
        TEST_FAILED();
    }

    //
    // Test request to Display
    //
    TEST_INFO("Test request to Display object");
    try {
        display->print("Hello World...");
        
        TEST_SUCCEED();
        
    } catch (const CORBA::Exception& e) {
        std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
                  << "Exception during display->print():\n" 
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e)
                  << std::endl;
        TEST_FAILED();
    }

    
}





}; // End namespace SimpleCcmServer
