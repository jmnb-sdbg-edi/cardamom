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


#include <exception>
#include <iostream>
#include <fstream>

#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "testrepositoryext/TestProcessBehaviour.hpp"

#ifdef CDMW_POSIX

#include <sys/types.h>
#include <unistd.h>

#endif
#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"
#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.stub.hpp"

using namespace std;
using namespace Cdmw;
using namespace Cdmw::Common;
using namespace Cdmw::OsSupport;


namespace
{

    const int POA_THREAD_POOL_SIZE = 10;

    std::string NS_PORT_OPTION = "--orb-ns-port";
    std::string WITHOUT_REPOSITORY_OPTION = "--without-repository";
    std::string DEACTIVATE_TRACES_OPTION = "--deactivate-traces";

}


void exceptionHandler(void)
{
	std::cout << "Unexpected Exception : Test failed" << std::endl;
}


class InitializationError
{

};


class OrbThread : public OsSupport::Thread {

    public:
        
        OrbThread(const std::string& portNumber)
        {
            OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

            try
            {

            #if (CDMW_ORB_VDR == orbacus )
                std::string strArgv = "-ORBnaming corbaloc::localhost:" + portNumber + "/NameService";
            #elif (CDMW_ORB_VDR == tao )
                std::string strArgv = "-ORBInitRef NameService=corbaloc:iiop:localhost:" + portNumber + "/NameService";
            #endif
        
                char** myArgv = String::to_char_array( String::to_strings(strArgv) );
                int myArgc(2);

                m_orb =  OrbSupport::OrbSupport::ORB_init(myArgc,myArgv, strategyList); 

                // We free memory allocated by String::to_char_array
                for (int iArgc=0; iArgc < myArgc; ++iArgc) {
                    delete[] myArgv[iArgc];
                }
                delete[] myArgv;

                // And activate the root POA
                CORBA::Object_var obj = m_orb->resolve_initial_references("RootPOA");
                PortableServer::POA_ptr rootPOA = PortableServer::POA::_narrow(obj.in());

                PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
                poaMgr->activate();

            }
            catch(const CORBA::Exception&)
            {
                throw InitializationError();
            }

        }


        ~OrbThread() throw() {};


    protected:

        void run() throw()
        {
            m_orb->run();
        }

    public:
        CORBA::ORB_var  m_orb;

};



int main(int argc, char* argv[])
{

#ifdef CDMW_POSIX

    uid_t uid = ::getuid();
    uid_t euid = ::geteuid();

    gid_t gid = ::getgid();
    gid_t egid = ::getegid();

    std::cout << "Attributes of " << argv[0] << " :" << std::endl;
    std::cout << "uid=" << uid << " euid=" << euid << " gid=" << gid << " egid=" << egid << std::endl;

#endif

    // the return code 
    int ret_code = 0;

    std::set_unexpected(&exceptionHandler);


    bool platformManaged =
        PlatformInterface::isLaunchedByPlatformManagement(argc, argv);


    std::string portNumber = OS::get_option_value( argc, argv, NS_PORT_OPTION);
    if ( portNumber == "no" )
    {
        portNumber = "4999";
    }

    bool launchRepository = false;
    std::string withoutRepositoryOption = OS::get_option_value( argc, argv, WITHOUT_REPOSITORY_OPTION);

    if (!platformManaged && withoutRepositoryOption == "no")
    {
        launchRepository = true;
    }
 
    bool deactivateTraces = false;
    std::string deactivateTracesOption = OS::get_option_value( argc, argv, DEACTIVATE_TRACES_OPTION);
    if ( deactivateTracesOption != "no" )
    {
        deactivateTraces = true;
    }



    try
    {

        // initialises the ORB and activates the POA manager
        OrbThread orbThread(portNumber);


        // creates the process behaviour
        std::auto_ptr<TestProcessBehaviour> pProcessBehaviour(
            new TestProcessBehaviour(orbThread.m_orb.in(), platformManaged,
                portNumber, launchRepository, deactivateTraces));

        if (platformManaged)
        {
            // initialise the platform interface
            PlatformInterface::setup(orbThread.m_orb.in(), argc, argv);

            // acknowledge the creation of the process
            PlatformInterface::acknowledgeCreation(pProcessBehaviour.get());
            pProcessBehaviour.release();

            orbThread.m_orb->run();

            // cleanup the platform interface
            PlatformInterface::Cleanup();

            //orb->shutdown() is done by the stop() of the ProcessBehaviour
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orbThread.m_orb.in());
	    orbThread.m_orb->destroy();
        }
        else
        {
            // runs the ORB in another thread
            orbThread.start();
			int timescale = Cdmw::TestUtils::Testable::get_timescale();
            OS::sleep(timescale*3000);

            CdmwPlatformMngtBase::StartupKind startupKind;
            startupKind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
            startupKind.startup_mode = 0;
            startupKind.specified_state_id = CORBA::string_dup("");

            (pProcessBehaviour.get())->initialise(startupKind);

            (pProcessBehaviour.get())->run();

            (pProcessBehaviour.get())->stop();

            //orb->shutdown() is done by the stop() of the ProcessBehaviour
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orbThread.m_orb.in());
	    orbThread.m_orb->destroy();
            orbThread.join();

        }

    }
    catch(const InitializationError &)
    {
        std::cerr << "FAILURE : Initialization problem" << std::endl;
        ret_code = EXIT_FAILURE;
    }
    catch(const CORBA::Exception &e)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl << e._name() << endl;
        ret_code = EXIT_FAILURE;
    }

    return ret_code;

}


