/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */


#include "ServerProcessControl.hpp"

#include "Foundation/common/Assert.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"


namespace Marshalling
{
  
  ServerProcessControl::ServerProcessControl (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_applicationName (""),
      m_processName ("")
  {
  }
  
  
  ServerProcessControl::~ServerProcessControl()
    throw()
  {
  }
  
  // process initialisation called by platformmngt 
  void ServerProcessControl::on_initialise (const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
  {
    // ===================================================
    // Get the process name and application name
    // ===================================================
    m_processName = Cdmw::CdmwInit::InitUtils::get_cdmw_process_name();
    
    m_applicationName = Cdmw::CdmwInit::InitUtils::get_cdmw_application_name();
    
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(), 
                                                         ">>>>>>>>>>>>>> Initialisation requested by supervision");
    
    // Create the object
    Marshalling::TestPing_i* testping_i = new Marshalling::TestPing_i();
    
    // Activate it to obtain the object reference
    Marshalling::TestPing_var m_testping = testping_i->_this();
    
    
    // ===================================
    // register Tester into Repository
    // ===================================
    // Get the Cdmw repository
    CdmwNamingAndRepository::Repository_var repository
      = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();    
    
    // Get DemoPerf name domain from repository
    CdmwNamingAndRepository::NameDomain_var marshallingNameDomain;
    try {
      marshallingNameDomain = repository->resolve_name_domain ("perf_test");
    } catch (const CdmwNamingAndRepository::NoNameDomain&) {
      std::cerr << "Can't find NameDomain perf_test" << std::endl;
      CDMW_ASSERT(false);
    } catch (const CdmwNamingAndRepository::InvalidName&) {
      std::cerr << "NameDomain perf_test is invalid !!" << std::endl;
      CDMW_ASSERT(false);
    }

    // Register the TestPing object in the "perf_test" name domain
    std::string registration_name = "TestPing";
    try {
      // reserve the name "TestPing" in the domain
      CdmwNamingAndRepository::NameDomain::RegistrationId_var regId 
        = marshallingNameDomain->new_name (registration_name.c_str());
      
      // register the object
      marshallingNameDomain->register_object (regId.in(), m_testping.in ());

    } catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists&) {
        std::cerr << "Name " << registration_name << " already exist into NameDomain perf_test" << std::endl;
        CDMW_ASSERT(false);
    } catch (const CdmwNamingAndRepository::InvalidName&) {
        std::cerr << "Name " << registration_name << " is an invalid name !!" << std::endl;
        CDMW_ASSERT(false);
    } catch (const CdmwNamingAndRepository::NameDomain::InvalidRegistration&) {
        std::cerr << "Invalid Registration of TestPing reference as " << registration_name << std::endl;
        CDMW_ASSERT(false);
    }        
    
  }
  
  
  // process to run called by platformmngt    
  void ServerProcessControl::on_run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,CORBA::SystemException)
  {
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(),
                                                         ">>>>>>>>>>>>>> Run requested by supervision");
                
    // do nothing
  }


  // process to stop called by platformmngt 
  void ServerProcessControl::on_stop()
    throw(CORBA::SystemException)
  {
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(), 
                                                         ">>>>>>>>>>>>>> Stop requested by supervision");
    
  }    
  

}; // End namespace Invocation
