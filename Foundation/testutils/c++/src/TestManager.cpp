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

#include "Foundation/testutils/TestManager.hpp"
#include "Foundation/common/Assert.hpp"
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>

// FIXME: find a way to discover the targer
#undef sparc
#undef sun

static int
m_timescale = -1;

///////////////////////////////////////////////////////////////////////////////


//
// Return the multiplier coefficient of time
//
int
Cdmw::TestUtils::get_timescale ()
{
  if (m_timescale == -1)
  {
    char *p_env =::getenv ("CDMW_TIMESCALE");
    if (p_env == NULL)
     return (m_timescale = 1);
	          
    m_timescale = atoi (p_env);
  }
  return m_timescale;
}
    
///////////////////////////////////////////////////////////////////////////////

Cdmw::TestUtils::TestManager::TestManager()
{
  m_is_initialized = false;
}

///////////////////////////////////////////////////////////////////////////////

Cdmw::TestUtils::TestManager::~TestManager()
{
}

///////////////////////////////////////////////////////////////////////////////

int
Cdmw::TestUtils::TestManager::init(int      argc,
                                   char***  argv )
{
    std::string    
    	arg_value; 
    int
        out=0;
    if( argc>0 && argv )
    {
        // get basename of program
        char* base_test_program = basename((*argv)[0]);
        m_base_test_program_string = base_test_program; 
                
        arg_value = Cdmw::OsSupport::OS::get_option_value(argc,
                                                          *argv,
                                                          "--help");
        if (arg_value == "yes") 
    {
            std::cout << "Options:\n"
                      << "\t --do-tests=\"test_name test_name ...\"..start selected test case only\n"
                      << std::endl;
            out = -1;
    }
        else
        {   	
            m_is_initialized=true;

            // -- multiple tests selection.
            arg_value = Cdmw::OsSupport::OS::get_option_value(argc,
                                                              *argv,
                                                              "--do-tests");
            if (arg_value != "no")
    {
            	// std::cout << ":>> do tests: " << arg_value << std::endl;
                std::istringstream iss(arg_value);
                std::string method;
                while (iss >> method)
                   m_test_list.push_back(method);
    }
        }
    }
    return out;
}

///////////////////////////////////////////////////////////////////////////////

Cdmw::TestUtils::TestManager* 
Cdmw::TestUtils::TestManager::instance()
{
    static Cdmw::TestUtils::TestManager instance; 
    return &instance; 
}

///////////////////////////////////////////////////////////////////////////////

void
Cdmw::TestUtils::TestManager::summary (std::ostream& os)
{
    std::string test_name = m_base_test_program_string;
    int total_tests = m_result.runTests();
    int number_of_fails=m_result.testFailuresTotal()  ;
    
  os << "===================================================" << std::endl;
  os << "Ending test " << test_name << std::endl;
  os << "===================================================" << std::endl;
  os << "Summary:" << std::endl;
  os << "   Failed    : " << number_of_fails << std::endl;
  os << "   Succeeded : " << total_tests - number_of_fails << std::endl;
  os << "   Expected successfull tests (CPP) :" << total_tests << std::endl;

  if (number_of_fails == 0)
    os << "   >>>> Test result is set as TESTABLE_OK <<<<" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

void
Cdmw::TestUtils::TestManager::set_aux_info (const std::string & info)
{
  m_aux_info = info;
}

///////////////////////////////////////////////////////////////////////////////

std::string 
Cdmw::TestUtils::TestManager::get_aux_info () const
{
   return m_aux_info;
}

///////////////////////////////////////////////////////////////////////////////

int
Cdmw::TestUtils::TestManager::run_tests()
{
    int out = 0;
    try
    {
        // Create the event manager and test controller.
        CppUnit::TestResult controller;
        
        // Add a listener that collects test result.
        controller.addListener( &m_result );        
        
        // Add a listener that print info about running test.
        CppUnit::BriefTestProgressListener progress;
        controller.addListener( &progress );      

        // Add the top suite to the test runner.
        CppUnit::TestRunner runner;
        if( this->is_registration_used())
           runner.addTest( CppUnit::TestFactoryRegistry::getRegistry(m_base_test_program_string.c_str()).makeTest() );
        else
            runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
        
        std::string 
    	  outFileName = m_base_test_program_string+std::string("_test_results.xml");    
           
        std::ofstream outFile(outFileName.c_str());    
        if(m_test_list.empty())
           runner.run( controller );        
        else
           for (unsigned int i = 0; i < m_test_list.size(); i++)
                runner.run( controller, m_test_list[i]);
         
        m_was_successful = m_result.wasSuccessful();

        // Print test in xml format on outFile.
        CppUnit::XmlOutputter outputter( &m_result, outFile );
        outputter.write();                      
        outFile.close();
        
    }
    catch ( std::invalid_argument &e )  // Test path not resolved.
    {
        std::cerr << ":>> std exception caught: " << e.what() << std::endl;
        out = -1;
    }      
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
