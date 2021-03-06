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


#include "TestSessionStrategy.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingUtil.hpp"
#include <string>
#include <cctype>
#include <algorithm>
#include "TestData.hpp"
#include "ResourceFactory.stub.hpp"
#include "AnonymousResourceFactory.stub.hpp"
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


TestSessionStrategy::TestSessionStrategy(const std::string & args, 
                                       std::ostream &      os)
    throw()
    :Testable("Cdmw::LifeCycle::TestSessionStrategy",os),
     m_args(args)
{
    // does nothing
}


TestSessionStrategy::~TestSessionStrategy()
    throw()
{
    // does nothing
}


TestSessionStrategy::TestSessionStrategy(const TestSessionStrategy& rhs)
    throw()
    :Testable("Cdmw::LifeCycle::SessionCleanupNotifier"),
     m_args(rhs.m_args)
{
    // does nothing
}
        
TestSessionStrategy&
TestSessionStrategy::operator=(const TestSessionStrategy& rhs)
    throw()
{
    if (this != &rhs) {
        m_args    = rhs.m_args;
    }
    return *this;
}

void TestSessionStrategy::do_tests()        
{
    //-------------------------------------------------
    // This tests Session strategy
    //-------------------------------------------------
    TEST_INFO("Testing Session strategy");

    using namespace Cdmw::LifeCycle;

    try {
        std::string factory_domaine_name = get_FACTORIES_DOMAIN_NAME();
        std::string factory_name         = get_SESSION_FACTORY_NAME();
        std::string dev_label            = get_SESSION_DEVICE_LABEL();
        std::string appl_1_name          = get_PROGRAM_EXECUTABLE_1();
        std::string appl_2_name          = get_PROGRAM_EXECUTABLE_2();


        // NOTA: - Device.stop() deactivates Device Object, while
        //         Device.start() or any other operation will reactivate it.
        //       - Application.start() will launch a process, while
        //         Application.stop() will kill the process and deactivate
        //         the Application object.
        

        Cdmw::CommonSvcs::Naming::NamingInterface ni
            = Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface
            (factory_domaine_name);
        
        TEST_INFO( "Testing " << factory_name << " factory.");

        
        typedef Cdmw::CommonSvcs::Naming::NamingUtil<CFFactories::AnonymousResourceFactory> 
            Util;
    
        
        CFFactories::AnonymousResourceFactory_var fac 
            = Util::resolve_name(ni,factory_name);
        
        CDMW_ASSERT(!CORBA::is_nil(fac.in()));

        CF::Device_var dev = CF::Device::_nil();
        try {
            TEST_INFO("Creating Device <" << dev_label << ">");
            dev = fac->create_device(dev_label.c_str());
            TEST_SUCCEED();
        } catch (const CdmwLifeCycle::AlreadyExistant & ex) {
            TEST_FAILED(); return;
        } catch (const CdmwLifeCycle::InvalidName & ex) {
            TEST_FAILED(); return;
        } catch (const CF::Device::InvalidLocation & ex) {
            TEST_FAILED(); return;
        } catch (const CORBA::SystemException & ex) {
            TEST_FAILED(); return;
        }
        
        try {
            TEST_INFO("Starting it");
            dev->start();
            TEST_SUCCEED();
        } catch (const CF::Resource::StartError & ex) {
            TEST_FAILED(); return;
        } catch (const CORBA::SystemException & ex) {
          std::cerr << ex << std::endl;
            TEST_FAILED(); return;
        }
        
        CF::Application_var appli = CF::Application::_nil();
        try {
            TEST_INFO("Creating Application I <" << appl_1_name << ">" );
            appli = fac->create_application (appl_1_name.c_str());
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

        try {
            TEST_INFO("Starting it");
            appli->start();
            TEST_SUCCEED();
        } catch (const CF::Resource::StartError & ex) {
            TEST_FAILED(); return;
        } catch (const CORBA::SystemException & ex) {
            TEST_FAILED(); return;
        }
        
        CF::Application_var appli2 = CF::Application::_nil();
        try {
            TEST_INFO("Creating Application I <" << appl_2_name << ">");
            appli2 = fac->create_application (appl_2_name.c_str());
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

        try {
            TEST_INFO("Starting it");
            appli2->start();
            TEST_SUCCEED();
        } catch (const CF::Resource::StartError & ex) {
            TEST_FAILED(); return;
        } catch (const CORBA::SystemException & ex) {
            TEST_FAILED(); return;
        }
        

        try {
            TEST_INFO("Stopping Application I");
            appli->stop();
            TEST_SUCCEED();
        } catch (const CF::Resource::StopError & ex) {
            TEST_FAILED(); return;
        } catch (const CORBA::SystemException & ex) {
            TEST_FAILED(); return;
        }

        try {
            TEST_INFO("Deleting Application II");
            appli2->remove();
            TEST_SUCCEED();
        } catch (const CosLifeCycle::NotRemovable & ex){
            TEST_FAILED(); return;
        } catch (const CORBA::SystemException & ex) {
            TEST_FAILED(); return;
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

