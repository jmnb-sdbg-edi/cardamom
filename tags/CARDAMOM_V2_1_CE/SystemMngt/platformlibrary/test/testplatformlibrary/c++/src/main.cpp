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


#include <fstream>
#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/testutils/CORBATestManager.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include <Foundation/commonsvcs/datastore/StdMapDataStoreFactory.hpp>


#include "SystemMngt/idllib/CdmwPlatformMngtDataStore.stub.hpp"

#include "SystemMngt/platformlibrary/Iterator.hpp"
#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/DataStoreBaseDefinition.hpp"

#include "testplatformlibrary/TestLogMngr.hpp"

#include "testplatformlibrary/TestMessageQueue.hpp"
#include "testplatformlibrary/TestStateMachine.hpp"
#include "testplatformlibrary/TestEventHandler.hpp"
#include "testplatformlibrary/TestGraphUtility.hpp"
#include "testplatformlibrary/TestGraphChecker.hpp"
#include "testplatformlibrary/TestSupervisionEventNotifier.hpp"
#include "testplatformlibrary/TestStarter.hpp"
#include "testplatformlibrary/TestSequencer.hpp"
#include "testplatformlibrary/TestContainer.hpp"
#include "testplatformlibrary/TestServiceDef.hpp"
#include "testplatformlibrary/TestPullMonitor.hpp"
#include "testplatformlibrary/TestElement.hpp"
#include "testplatformlibrary/TestProcessKillMngr.hpp"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>

#include <exception>
#include <iostream>

namespace
{

    const int POA_THREAD_POOL_SIZE = 10;

    std::string NS_PORT_OPTION = "--observer-port";

    const int MAX_RETRY_ON_FTMGR = 4;
    const int SLEEP_RETRY_ON_FTMGR_MS = 1000;

    const CORBA::ULong EVENT_REPORT_DATA_STORE_ID = 8;
    const CORBA::ULong EVENT_NOTIFIER_DATA_STORE_ID = 9;
    const CORBA::ULong SERVICE_DEFS_DATA_STORE_ID = 10;

} // end of anonymous namespace


void exceptionHandler(void)
{
	std::cout << "Unexpected Exception : Test failed" << std::endl;
}


class OrbThread : public OsSupport::Thread {

    public:
        
        OrbThread(CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA)
        {
            m_orb = CORBA::ORB::_duplicate(orb);

            PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
            poaMgr->activate();
        }
        
        ~OrbThread() throw() {} ;


        void
        shutdown() 
        {
            m_orb->shutdown(true);
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(m_orb.in());
	    m_orb->destroy();
        }

    protected:

        void
        run() throw() 
        {
            m_orb->run();   
        }

        
    public:

        CORBA::ORB_var  m_orb;

};


int main(int argc, char* argv[])
{
    using namespace Cdmw::PlatformMngt;  
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    
    
    // the return code 
    int ret_code = 0;

    std::set_unexpected(&exceptionHandler);

    CORBA::ORB_var orb;

    std::string observerPortNumber = OS::get_option_value( argc, argv, NS_PORT_OPTION);
    if (observerPortNumber == "no")
    {
        observerPortNumber = "5024";
    }

    try
    {
        StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerRequest();

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        //Cdmw::TestUtils::CORBATestManager::instance()->init(orb.in(),true,argc,&argv);

	PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
	Cdmw::TestUtils::CORBATestManager::instance()->init(orb.in(),rootPOA.in(),argc,&argv);
	Cdmw::TestUtils::CORBATestManager::instance()->get_POA();
	//PortableServer::POA::_narrow(obj.in());

 //       Cdmw::TestUtils::CORBATestManager::instance()->init(orb.in(),rootPOA.in());
        Cdmw::TestUtils::CORBATestManager::instance()->set_aux_info(observerPortNumber);

        OrbThread orbThread(orb.in(), rootPOA.in());
        orbThread.start();
		int timescale = Cdmw::TestUtils::get_timescale();
        OsSupport::OS::sleep(timescale*3000);
        
        
        // create the datastore
        
        // define the datastore responsible to store EventHandler
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngt::EventReportRecord> EventReportDataStoreFactory;

         EventReportDataStoreFactory::Create_data_store(EVENT_REPORT_DATA_STORE_ID);

         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_event_report_datastore_id(EVENT_REPORT_DATA_STORE_ID);
    

         // define the datastore responsible to store EventNotifier
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngt::Event> EventNotifierDataStoreFactory;

         EventNotifierDataStoreFactory::Create_data_store(EVENT_NOTIFIER_DATA_STORE_ID);
    
         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_event_notifier_datastore_id(EVENT_NOTIFIER_DATA_STORE_ID);
    
      
         // define the datastore responsible to store ServiceDefs
         typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory<std::string, 
                 CdmwPlatformMngtService::ServiceDefs> ServiceDefsDataStoreFactory;

         ServiceDefsDataStoreFactory::Create_data_store(SERVICE_DEFS_DATA_STORE_ID);

         // Store datastore_id in the Configuration class to have access inside
         // the whole SystemMngt CSCI
         PlatformMngt::Configuration::Set_service_defs_datastore_id(SERVICE_DEFS_DATA_STORE_ID);
    
        
        // initialise the log manager
        Cdmw::PlatformMngt::TestLogMngr::initialise();

        // init iterator manager with root poa
        IteratorManager::initialize(rootPOA.in());

       Cdmw::TestUtils::CORBATestManager::instance()->run_tests();

       Cdmw::TestUtils::CORBATestManager::instance()->summary();
       orbThread.shutdown();
       orbThread.join();

    //return result.wasSuccessful() ? 0 : 1;
    //return ret_code;
    return Cdmw::TestUtils::CORBATestManager::instance()->was_successful();

    }
    catch(const std::exception &e)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl
            << e.what() << std::endl;
        ret_code = EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl;
        ret_code = EXIT_FAILURE;
    }
    
    // destroy the log manager
    Cdmw::PlatformMngt::LogMngr::destroy();


    return ret_code;

}


