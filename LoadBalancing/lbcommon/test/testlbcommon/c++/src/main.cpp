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
 * @brief _implementation for TestLBCommon with cppUnit library.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 *
 */
#ifndef LBCOMMON_PLUGIN_EXPORTS
#include <string>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "testlbcommon/CORBAManager.hpp"

using namespace std;

int main( int argc, char* argv[] )
{
    char *outFileName = "res.xml";
  
    try
    {
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
        strategyList.add_multicast();
        // Initialises the ORB
        CORBA::ORB_var orb =
            Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        try
            {
                CORBAManager::instance()->init(orb.in());

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
        
        int total_fails = result.testFailuresTotal ();
        int total_tests = result.runTests () ;

        std::cerr<<"==================================================="<<std::endl;
        std::cerr<<"Ending test LoadBalancing --> lbcommon"<<std::endl;
        std::cerr<<"==================================================="<<std::endl;
        std::cerr<<"Summary:"<<std::endl;
        std::cerr<<"   Failed    : "<<total_fails<<std::endl;
        std::cerr<<"   Succeeded : "<<total_tests  - total_fails <<std::endl;
        std::cerr<<"   Expected successfull tests (CPP) :"<< total_tests <<std::endl;

        if( total_fails == 0 )
            std::cerr<<"   >>>> Test result is set as TESTABLE_OK <<<<"<<std::endl;
        
        
#ifdef WIN32
        system("PAUSE");
#endif
        return result.wasSuccessful() ? 0 : 1;   
        
    }catch( const CORBA::Exception &e )
        {
            cerr <<__FILE__<<" "<<__LINE__<<" "<< e << endl;
            return -1;
        }
    catch(...)
        {
        cerr << "Uncaught exception" << endl;
        return -1;
        }       
    
}


#else

#include <cppunit/plugin/TestPlugIn.hpp>

// _implements all the plug-in stuffs, WinMain...
CPPUNIT_PLUGIN_IMPLEMENT();

#endif

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
    
