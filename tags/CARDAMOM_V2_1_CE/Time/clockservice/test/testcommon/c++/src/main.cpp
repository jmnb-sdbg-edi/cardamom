/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/**
 *
 * @author Fabrizio Morciano
 * Runner for tests. 
 * Please use a string like,
 * -ORBInitRef ClockService=file://ClockService.ior -ORBInitRef LocalClock=file://LocalClock.ior
 * to configure properely.
 * See also --help.
 */

#ifndef TESTCLOCKSERVICE_PLUGIN_EXPORTS

#include <libgen.h> 
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>

#include <Time/clockservice/CdmwCosClock.hpp>
#include <Time/clockservice/Util.hpp>

#include "Time/clockservice/test/testcommon/TimeTestManager.hpp"
#include "Time/clockservice/test/testcommon/PerformanceManager.hpp"

#ifndef NOT_LOCALISATION
#   include <Foundation/common/Options.hpp>
#   include <Foundation/common/String.hpp>
#   include <Foundation/orbsupport/OrbSupport.hpp>
#   include <Foundation/ossupport/OS.hpp>
#else
#   include "tao/PortableServer/ORB_Manager.h"
#endif

#include <Foundation/osthreads/Barrier.hpp>

using namespace std;
using namespace Cdmw::clock;
using namespace Cdmw;



#include <cppunit/TestListener.h>
#include <cppunit/Test.h>
 
////////////////////////////////////////////////////////////////////////////////

namespace
{    
    
    bool    
        clientOnly = false;
        
    bool    
        serverOnly = false;
    
    int
        returnError = -1;

    int
        returnSuccess = 0;

    std::vector<std::string> do_tests;
        
    class ORBWorker : public Cdmw::OsSupport::Thread
    {
    public:
            ORBWorker(Cdmw::OsSupport::Barrier& barrier, CORBA::ORB_ptr orb); 
            virtual ~ORBWorker() throw ();
            virtual void run() throw ();
    private:
        Cdmw::OsSupport::Barrier& m_barrier;
        CORBA::ORB_var            m_ORB;
    };
    
    ORBWorker::ORBWorker(Cdmw::OsSupport::Barrier& barrier, CORBA::ORB_ptr orb)
        : m_barrier(barrier), 
          m_ORB(orb)
    {
    }
    
    ORBWorker::~ORBWorker() throw ()
    {
        try
        {
            m_ORB->shutdown(1);
            m_ORB->destroy(); 
            this->join();     
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout << ":>> CORBA system exception caught: " << e._info().rep() << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << ":>> std exception caught: " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cout << ":>> unexpected exception caught" << std::endl;
        }
    }
    
    void ORBWorker::run() throw ()
    {
        try
        {
            m_barrier.wait(); 
            this->m_ORB->run();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout << ":>> CORBA system exception caught: " << e._info().rep() << std::endl;
        }
        catch (...)
        {
            std::cout << ":>> unexpected exception caught" << std::endl;
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////     
    
    void 
    parse_args(int argc, 
               char* argv[] )
    {

        // -- enable performance assert.
        std::string arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                  argv,
                                                  "-p");
        if (arg_value == "yes") 
        {
            std::cout << ":>> performance assert are: on." << std::endl;
            PerformanceManager::get_instance()->activate();
        }

        // -- client only mode.
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                  argv,
                                                  "--client");
        if (arg_value != "no")
        {            
            std::cout << ":>> client only selection is: on." << std::endl;
            clientOnly = true;
        }

        // -- server only mode.
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                  argv,
                                                  "--server");
        if (arg_value != "no")
        {
            if( clientOnly )
            {
                std::cout << ":>> server and client at the same time???\n"
                          << ":>> are you sure?\n";
                exit(returnError);
            }
            std::cout << ":>> server only selection." << std::endl;
            serverOnly = true;
        }
        
        if( clientOnly ) 
        {
            arg_value =
                Cdmw::OsSupport::OS::get_option_value(argc,
                                                      argv,
                                                      "-ORBInitRef");
            if (arg_value == "no")
            {
                std::cout << ":>> please provide an: -ORBInitRef <parameters>!!!"
                          << std::endl;
                exit(returnError);
            }
            
            arg_value =
                Cdmw::OsSupport::OS::get_option_value(argc,
                                                      argv,
                                                      "--CdmwLocalisationService");
            if (arg_value != "no")
            {
                std::cout<<":>> Please remove option: --CdmwLocalisationService!!!"
                         <<std::endl;
                exit(returnError);
            }
            
        }
               
        // -- show help informations
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                  argv,
                                                  "--help");
        if (arg_value == "yes") 
        {
            std::cout << "Options:\n"
                      << "\t --help................................Show this message\n"
                      << "\t --p...................................Enable performance assert\n"
                      << "\t --client..............................Start test without "\
                         "using the collocated service\n"
                      << "\t --server .............................Start the Time service only\n"
                      << "\t --do-tests=\"test_name test_name ...\"..start selected test case only\n"
                      << std::endl;
            exit(returnError);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{

    try
    {
        parse_args(argc,argv);

        // initialises the ORB.
        CORBA::ORB_var orb;

#ifndef NOT_LOCALISATION
        OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();
        strategyList.add_multicast();
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
#else
        orb = CORBA::ORB_init (argc, argv, "");
#endif
        PortableServer::POA_var rootPOA;
        PortableServer::POAManager_var poa_manager;

        if (clientOnly == false)
        {
            // initializes the CosClockServices.
            if (CosClock::init(orb.in(), argc, argv) == false)
            {
                std::cerr << ":>> can not init clock service!" << std::endl;
                return returnError;
            }

            CORBA::Object_var obj;
            try
            {
                obj = orb->resolve_initial_references("RootPOA");
            }
            catch(const CORBA::SystemException& e)
            {
                std::cerr << "ClockService:>> " << e << std::endl;
            }
            catch(std::exception& e)
            {
                std::cerr << "ClockService:>> " << e.what() << std::endl;
            }

            //CDMW_ASSERT(!CORBA::is_nil(obj.in()));
        
            rootPOA = PortableServer::POA::_narrow(obj.in());
        
            poa_manager = rootPOA->the_POAManager();

            poa_manager->activate();
        }

        // initializes the time test manager.
        Cdmw::TestUtils::CORBATestManager::instance()->init(orb.in(), 
	                                               rootPOA.in(),
						       argc,
						       &argv);

        Cdmw::TestUtils::CORBATestManager::instance()->use_registration();


        if (serverOnly == true)
        {
            // the server process enters in a never ending loop
            orb->run();
        }
        else
        {
            // create the barrier.
            Cdmw::OsSupport::Barrier barrier(2);

            // create an ORBWorker object.
            ORBWorker* orb_worker = new ORBWorker(barrier, orb.in());
            if (orb_worker == 0)
            {
                std::cerr << "Could not create ORBWorkwer object: aborting" << std::endl;
                if (clientOnly == false)
                {
                    // close CosClockService.
                    CosClock::close();
                }
                return returnError;
            }

            // start the ORBWorker thread.
            orb_worker->start();

            // wait 3 seconds for the ORBWorker thread to start execution.
            Cdmw::OsSupport::Barrier::WaitReturn status = barrier.wait(3000);
            if (status == Cdmw::OsSupport::Barrier::TIMEDOUT)
            {
                std::cerr << "Could not synchronize main thread with ORBWorkwer thread: aborting" << std::endl;
                if (clientOnly == false)
                {
                    // close CosClockService.
                    CosClock::close();
                }

                // delete the ORBWorker object.
                delete orb_worker;

                // end program.
                return returnError;
            }

            // wait for the ORBWorker thread to run the ORB.
            Cdmw::OsSupport::OS::sleep(1,0);

            // run the tests.
            Cdmw::TestUtils::CORBATestManager::instance()->run_tests(); 
            Cdmw::TestUtils::CORBATestManager::instance()->summary();
 
            if (clientOnly == false)
            {
                // close CosClockService
                CosClock::close();
            }

            // delete the ORBWorker object.
            delete orb_worker;
        }
 
#ifdef	WIN32	
        orb->shutdown(1);
        CosClock::get_POA()->destroy(1,1);
        orb->destroy();
#endif

    }
    catch (std::logic_error& e)
    {
        std::cerr << ":>> std exeception caught: " << e.what() << std::endl;
        return returnError;
    }
    catch (const CORBA::Exception& e)
    {
        std::cerr << ":>> CORBA exception caught: " << e._info().rep() << std::endl;
        return returnError;
    }
    catch(...)
    {
        std::cerr << ":>> unknown exception caught" << std::endl;
        return returnError;
    }
    
    return returnSuccess;
}

#else

#include <cppunit/plugin/TestPlugIn.hpp>

// _implements all the plug-in stuffs, WinMain...
CPPUNIT_PLUGIN_IMPLEMENT();

#endif

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
