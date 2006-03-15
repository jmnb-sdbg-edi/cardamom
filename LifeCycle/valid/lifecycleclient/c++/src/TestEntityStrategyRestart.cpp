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

#include "lifecycleclient/TestEntityStrategyRestart.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingUtil.hpp"
#include <string>
#include <cctype>
#include <algorithm>
#include "lifecycleclient/TestData.hpp"
#include "lifecycleclient/ResourceFactory.stub.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"


namespace Cdmw
{
namespace LifeCycle
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestEntityStrategyRestart::TestEntityStrategyRestart(const std::string & args, 
                                                     std::ostream &      os)
    throw()
    :Testable("Cdmw::LifeCycle::TestEntityStrategyRestart",os),
     m_args(args)
{
    // does nothing
}


TestEntityStrategyRestart::~TestEntityStrategyRestart()
    throw()
{
    // does nothing
}


TestEntityStrategyRestart::TestEntityStrategyRestart(const TestEntityStrategyRestart& rhs)
    throw()
    :Testable("Cdmw::LifeCycle::EntityCleanupNotifier"),
     m_args(rhs.m_args)
{
    // does nothing
}
        
TestEntityStrategyRestart&
TestEntityStrategyRestart::operator=(const TestEntityStrategyRestart& rhs)
    throw()
{
    if (this != &rhs) {
        m_args    = rhs.m_args;
    }
    return *this;
}

void TestEntityStrategyRestart::do_tests()        
{
    //-------------------------------------------------
    // This tests Entity strategy
    //-------------------------------------------------
    
    set_nbOfRequestedTestOK (0);
    
    TEST_INFO("Testing Entity strategy");

    using namespace Cdmw::LifeCycle;

    try {
        std::string factory_domain_name = get_FACTORIES_DOMAIN_NAME();
        std::string factory_name        = get_ENTITY_FACTORY_NAME(); 
        std::string appl_1_key          = get_ENTITY_APPLICATION_I_KEY();
        std::string appl_1_name         = get_PROGRAM_EXECUTABLE_1();

        std::string resource_domain_name = get_RESOURCES_DOMAIN_NAME();


        // NOTA: - Application.start() will launch a process, while
        //         Application.stop() will kill the process and deactivate
        //         the Application object.
        
        
        TEST_INFO( "Testing " << factory_name << " factory.");

        
        CFFactories::ResourceFactory_var fac = CFFactories::ResourceFactory::_nil();

        {
            using Cdmw::NamingAndRepository::RepositoryInterface;
            Cdmw::CommonSvcs::Naming::NamingInterface ni
                = RepositoryInterface::get_domain_naming_interface
                (factory_domain_name);
            typedef Cdmw::CommonSvcs::Naming::NamingUtil<CFFactories::ResourceFactory> 
                Util;
            
            fac = Util::resolve_name(ni,factory_name);
        }


        CDMW_ASSERT(!CORBA::is_nil(fac.in()));

        CF::Application_var appli = CF::Application::_nil();

        if ((m_args == "create")||
            (m_args == "all")) {
            add_nbOfRequestedTestOK (1);
            try {
                TEST_INFO("Creating Application I <" 
                          << appl_1_key << "," << appl_1_name << ">");
                appli = fac->create_application (appl_1_key.c_str(), 
                                                 appl_1_name.c_str());
                TEST_SUCCEED();
            } catch (const CdmwLifeCycle::AlreadyExistant & ex) {
                TEST_FAILED(); return;
            } catch (const CdmwLifeCycle::InvalidName & ex) {
                TEST_FAILED(); return;
            } catch (const CF::CreateApplicationError & ex) {
                TEST_FAILED(); return;
            } catch (const CORBA::SystemException & ex) {
                TEST_FAILED(); return;
            }
        } else {
            using Cdmw::NamingAndRepository::RepositoryInterface;
            Cdmw::CommonSvcs::Naming::NamingInterface ni
                = RepositoryInterface::get_domain_naming_interface
                (resource_domain_name);
            typedef Cdmw::CommonSvcs::Naming::NamingUtil<CF::Application> 
                Util;
            
            appli = Util::resolve_name(ni,appl_1_key);
            
        }
        
        if ((m_args == "start")||
            (m_args == "all")) {
            try {
                add_nbOfRequestedTestOK (1);
                TEST_INFO("Starting it");
                appli->start();
                TEST_SUCCEED();
            } catch (const CF::Resource::StartError & ex) {
                TEST_FAILED(); return;
            } catch (const CORBA::SystemException & ex) {
                TEST_FAILED(); return;
            }
        }
        
        if ((m_args == "stop")||
            (m_args == "all")) {
            try {
                add_nbOfRequestedTestOK (1);
                TEST_INFO("Stopping Application I");
                appli->stop();
                TEST_SUCCEED();
            } catch (const CF::Resource::StopError & ex) {
                TEST_FAILED(); return;
            } catch (const CORBA::SystemException & ex) {
                TEST_FAILED(); return;
            }
        }
        
        if ((m_args == "remove")||
            (m_args == "all")) {
            try {
                add_nbOfRequestedTestOK (1);
                TEST_INFO("Deleting Application I");
                appli->remove();
                TEST_SUCCEED();
            } catch (const CosLifeCycle::NotRemovable & ex){
                TEST_FAILED(); return;
            } catch (const CORBA::SystemException & ex) {
                TEST_FAILED(); return;
            }
        }
        
    } catch (const CORBA::Exception & ex) {
        TEST_INFO("ERROR:" << ex);
        TEST_FAILED();
    } catch (const Cdmw::Common::TypeMismatchException & ex) {
        TEST_INFO("ERROR:" << ex.what());
        TEST_FAILED();
    }
}
        
}; // End namespace LifeCyle
}; // End namespace Cdmw

