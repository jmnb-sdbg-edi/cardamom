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

// Standard include files
#include <iostream>
#include <cstdlib>
#include <string>



// Cdmw include files
#include "Foundation/common/Locations.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.stub.hpp"

#include "eventclient/TestCdmwEventService.hpp"
#include "eventclient/TestCdmwTypedEventService.hpp"

#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"


namespace {

    const std::string TEST_PROC_1 = "TestProcess1";
    const std::string TEST_PROC_2 = "TestProcess2";
    const std::string TEST_PROC_3 = "TestProcess3";
    const std::string TEST_PROC_4 = "TestProcess4";
    
    const int SUCCESS = 0;
    const int FAILURE = 1;  
    
    const int POA_THREAD_POOL_SIZE = 5;   
    
    const char* FACTORY_SERVER_IOR = "FactoryServerIor";



    class TestProcessControl : public Cdmw::CdmwInit::ProcessControl
    {
    public:
        
        TestProcessControl(CORBA::ORB_ptr orb, bool typed_evt_test)
            throw(CORBA::SystemException)
            : m_orb(CORBA::ORB::_duplicate(orb))
        {
            m_typed_evt_test = typed_evt_test;
            m_test_proc_nb = 0;
        }
               
	    ~TestProcessControl() throw() 
        {
            std::cout << "Destructor of TestProcessControl called" << std::endl;
        }
            
        /**
        * Purpose:
        * <p>
        * the behaviour for the
        * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
        * operation
        */
        virtual 
            void 
            on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CORBA::SystemException)
        {
            using namespace Cdmw::NamingAndRepository;
            using namespace Cdmw::CommonSvcs::Naming;
            using namespace Cdmw::PlatformMngt;
            using namespace Cdmw::Common;

            PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                              PlatformInterface::Get_process_name(),
                                              "Initialisation requested");
                        
            std::string testProcName 
                = PlatformInterface::Get_process_name();
            
            if (testProcName == TEST_PROC_1)
            {
                m_test_proc_nb = 1;
            }
            else if (testProcName == TEST_PROC_2)
            {
                m_test_proc_nb = 2;
            }
            else if (testProcName == TEST_PROC_3)
            {
                m_test_proc_nb = 3;
            }
            else if (testProcName == TEST_PROC_4)
            {
                m_test_proc_nb = 4;
            }

            try 
            {
                //
                // Get NamingInterface object from RepositoryInterface
                //
                NamingInterface ni
                    = RepositoryInterface::get_domain_naming_interface(Locations::CDMW_SERVICES_NAME_DOMAIN);
              
                //
                //Get a reference on the EventChannel1 and EventChannel2
                //
                CORBA::Object_var obj = ni.resolve("EVENT");
                m_nc = CosNaming::NamingContext::_narrow(obj.in());
                
                if (CORBA::is_nil(m_nc.in())) 
                {
                    std::cerr << "NamingContext is a nil object reference"
                        << std::endl;
                    std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                        << " IN FILE: " << __FILE__ << std::endl;
                    
                    PlatformInterface::Notify_fatal_error(testProcName.c_str(),
                        "NamingContext is a nil object reference!");
                    return;
                }
            }
            catch (CORBA::SystemException &ex)
            {
                std::cerr << ex << std::endl;
                std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                    << " IN FILE: " << __FILE__ << std::endl;
                PlatformInterface::Notify_fatal_error(testProcName.c_str(),
                        "Can not get NamingInterface!");
                throw;
            }
        }
        
        
        /**
        * Purpose:
        * <p>
        * the behaviour for the
        * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
        * operation
        */
        virtual 
            void 
            on_run()
            throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
            CORBA::SystemException)
        {
            using namespace Cdmw::PlatformMngt;
            using namespace Cdmw::Event;

            PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                PlatformInterface::Get_process_name(),
                "Run requested");
                
            //Testing TestCdmwEventService
            if (m_typed_evt_test)
            {
                TestCdmwTypedEventService testCdmwTypedEvent(m_orb.in(), m_nc.in(), m_test_proc_nb);
                testCdmwTypedEvent.start();
            }
            else
            {
                TestCdmwEventService testCdmwEvent(m_orb.in(), m_nc.in(), m_test_proc_nb);
                testCdmwEvent.start();
            }
        }
        
        /**
        * Purpose:
        * <p>
        * the behaviour for the
        * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
        * operation
        */
        virtual 
            void 
            on_stop()
            throw(CORBA::SystemException)
        {   
            using namespace Cdmw::PlatformMngt;

            PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                              PlatformInterface::Get_process_name(),
                                              "Stop requested");
        }
        
    private:
        CORBA::ORB_var                 m_orb;
        CosNaming::NamingContext_var   m_nc;
        bool                           m_typed_evt_test;
        int                            m_test_proc_nb;
    };

};  // end namespace


int main(int argc, char* argv[])
{
	 using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;

    int status = SUCCESS;
    CORBA::ORB_var orb;
    CdmwPlatformMngt::ProcessCallback_var callback;
    bool typedEvtTest = false;


    std::string typedTestOption
        = OS::get_option_value( argc, argv, "-t");

    if ( typedTestOption == "yes")
    {
       typedEvtTest = true;
    }

    std::string callbackIOR = OS::get_option_value(argc, argv,
                                                   Options::CALLBACK_IOR_OPTION);

    // ---- Is it launched by platform management for validation tests? ----
    if (callbackIOR == "no" || callbackIOR == "yes")
    {
        // error: can not be used as a unit test!
        std::cerr << "This test is a validation test and shall be launched by platform management"
                  << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        return FAILURE;
    }

    try
    {
		  Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);
        
        // Get the root POA 
        CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        // Activate the POA manager
        PortableServer::POAManager_var poaManager = rootPOA -> the_POAManager();
        poaManager -> activate();
 
        Cdmw::CdmwInit::ProcessControl_var ctrl 
            = new TestProcessControl(orb.in(), typedEvtTest);
        
        // init Cdmw
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, ctrl.in());            

        // Run implementation
        orb->run();
    }
    catch(const CORBA::Exception& ex)
    {
	    std::cerr << ex << std::endl;
       std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                 << " IN FILE: " << __FILE__ << std::endl;
	    status = FAILURE;
    }


    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
    orb->destroy();

    return status;    
}

