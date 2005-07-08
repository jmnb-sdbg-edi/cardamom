/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/testutils/Testable.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "SystemMngt/platformlibrary/Iterator.hpp"
#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"

#include "testplatformlibrary/TestLogMngr.hpp"

#include "testplatformlibrary/TestMessageQueue.hpp"
#include "testplatformlibrary/TestStateMachine.hpp"
#include "testplatformlibrary/TestEventHandler.hpp"
#include "testplatformlibrary/TestGraphUtility.hpp"
#include "testplatformlibrary/TestGraphChecker.hpp"
#include "testplatformlibrary/TestSupervisionEventNotifier.hpp"
#include "testplatformlibrary/TestStarter.hpp"
#include "testplatformlibrary/TestIterators.hpp"
#include "testplatformlibrary/TestSequencer.hpp"
#include "testplatformlibrary/TestContainer.hpp"
#include "testplatformlibrary/TestEntity.hpp"
#include "testplatformlibrary/TestServiceDef.hpp"
#include "testplatformlibrary/TestPullMonitor.hpp"

#include <exception>
#include <iostream>

namespace
{

    const int POA_THREAD_POOL_SIZE = 10;

    std::string NS_PORT_OPTION = "--observer-port";

}


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
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        OrbThread orbThread(orb.in(), rootPOA.in());
        orbThread.start();
		int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*3000);
        
        // initialise the log manager
        Cdmw::PlatformMngt::TestLogMngr::initialise();

        // init iterator manager with root poa
        IteratorManager::initialize(rootPOA.in());

        TestMessageQueue test1("MessageQueue<std::string>");
		test1.start();

        TestStateMachine test2("StateMachine");
		test2.start();

        TestEventHandler test3("EventHandler");
		test3.start();
        TestGraphUtility test4("GraphUtility");
		test4.start();

        TestGraphChecker test5("GraphChecker");
		 test5.start();

        TestSupervisionEventNotifier test6("SupervisionEventNotifier", orb.in(), observerPortNumber);
		test6.start();

        TestStarter test7("Starter");
		test7.start();

        TestIterators test8("Iterators", rootPOA.in());
		test8.start();

        TestSequencer test9("Sequencer");
		test9.start();

        TestContainer test10("Container", rootPOA.in());
		test10.start();

        TestEntity test11("Entity", rootPOA.in());
		test11.start();

        TestServiceDef test12("ServiceDef", rootPOA.in());
        test12.start();

        TestPullMonitor test13("PullMonitor", orb.in(), rootPOA.in());
        test13.start();

        orbThread.shutdown();
        orbThread.join();

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


