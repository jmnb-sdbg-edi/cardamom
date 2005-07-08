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

/**
 *
 * @author Fabrizio Morciano
 * Runner for tests. 
 * Please use following configuration string:
 * 
 * -ORBInitRef ClockService=file://ClockService.ior -ORBInitRef LocalClock=file://LocalClock.ior
 */

#ifndef TESTCLOCKSERVICE_PLUGIN_EXPORTS

#include <iostream>
#include <fstream>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>

#include "Time/testclockservice/CORBAManager.hpp"
#include "Time/testclockservice/PerformanceManager.hpp"

#include "tao/PortableServer/ORB_Manager.h"
#include "ace/Get_Opt.h"
#include "ace/Task.h"


using namespace std;


    class ORBWorker : public ACE_Task_Base
    {
    public:
        ORBWorker (CORBA::ORB_ptr orb);
        virtual int svc (void);
    private:
        CORBA::ORB_var m_ORB;
    };

    ORBWorker::ORBWorker (CORBA::ORB_ptr orb)
        :  m_ORB (orb)
    {
    }

    int
    ORBWorker::svc (void)
    {
        try
        {
          this->m_ORB->run ();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout << "ORBWorker:>> " << e << std::endl;
        }
        return 0;
    }


///////////////////////////////////////////////////////////////////////////////
	
static
void parse_args( int argc, char* argv[] )
{
    if(argc<2)
    {
        std::cerr << "usage:  "
                    << argv[0]
                    << " [-p (enable performace tests)]\n"
                    <<argv[0]
                    <<"-ORBInitRef ClockService=file://ClockService.ior "
                    <<"-ORBInitRef LocalClock=file://LocalClock.ior"
                    << std::endl;
    }

    string  tmp(argv[1]);
    if( tmp == "-p" )
        PerformanceManager::get_instance()->activate();
}


///////////////////////////////////////////////////////////////////////////////

int 
main(int argc, 
     char **argv)
{
    parse_args(argc,argv);
    try
    {
        CORBAManager::instance()->init(argc, argv);

        int retval;
        ORBWorker orb_worker( CORBAManager::instance()->get_orb().in() );
        retval = orb_worker.activate (THR_NEW_LWP | THR_JOINABLE);
        if(retval<0)
        {
            std::cerr<<"Error on starting orb thread!"<<std::endl;
            return -1;
        }

        CORBAManager::instance()->activate_poa();
    }
    catch(std::logic_error& e)
    {
        std::cerr<<e.what()<<std::endl;
        return -1;
    }
    catch(...)
    {
        return -1;
    }
    
    // Create the event manager and test controller
    CppUnit::TestResult controller;
    
    // Add a listener that colllects test result
    CppUnit::TestResultCollector result;
    controller.addListener( &result );        
    
    // Add a listener that print info about running test.
    CppUnit::BriefTestProgressListener progress;
    controller.addListener( &progress );      
    
    // Add the top suite to the test runner
    CppUnit::TestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );           
   
    char *outFileName = "res.xml";
    try
    {
        std::ofstream outFile(outFileName);    
        runner.run( controller );        

        // Print test in xml format on outFile.
        CppUnit::XmlOutputter outputter( &result, outFile );
        outputter.write();                      
        outFile.close();
    }
    catch ( std::invalid_argument &e )  // Test path not resolved
    {
        std::cerr  <<  std::endl  
            <<  "ERROR: "  <<  e.what()
            << std::endl;
        return -1;
    }
           
    //   CORBAManager::instance()->get_orb()->shutdown();
    int total_fails = result.testFailuresTotal ();
    int total_tests = result.runTests () ;

    std::cout<<"==================================================="<<std::endl;
    std::cout<<"Ending test Time"<<std::endl;
    std::cout<<"==================================================="<<std::endl;
    std::cout<<"Summary:"<<std::endl;
    std::cout<<"   Failed    : "<<total_fails<<std::endl;
    std::cout<<"   Succeeded : "<<total_tests  - total_fails <<std::endl;
    std::cout<<"   Expected successfull tests (CPP) :"<< total_tests <<std::endl;
    
    if( total_fails == 0 ) 
        std::cout<<"   >>>> Test result is set as TESTABLE_OK <<<<"<<std::endl;

#ifdef WIN32
    system("PAUSE");
#endif

    return result.wasSuccessful() ? 0 : 1;
}

#else

#include <cppunit/plugin/TestPlugIn.hpp>

// _implements all the plug-in stuffs, WinMain...
CPPUNIT_PLUGIN_IMPLEMENT();

#endif

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
