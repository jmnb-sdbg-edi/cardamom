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
#include <sstream>
#include <string>
#include <vector>

#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "testorblogging/TestOrbLoggingDriver.hpp"
#include "testorblogging/TestHello_impl.hpp"


namespace Cdmw
{

    namespace Logging
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        TestOrbLoggingDriver::TestOrbLoggingDriver(CORBA::ORB_ptr orb, 
                                                   const std::string& process_name)
                : Testable("Cdmw::Logging"),
                  m_orb(CORBA::ORB::_duplicate(orb)),
                  m_process_name(process_name)
        {
            // Nothing to do
        }

        TestOrbLoggingDriver::~TestOrbLoggingDriver()
        throw()
        {
        }

        void
        TestOrbLoggingDriver::do_tests()
        throw()
        {
            set_nbOfRequestedTestOK (3);
            int timescale = Cdmw::TestUtils::Testable::get_timescale();

            TEST_INFO("Start the server process");
            OsSupport::OS::create_process( m_process_name, "--server --CdmwLogLevel=16 --CdmwLogFile=test.log -ORBDebugLevel 10");
            OsSupport::OS::sleep(timescale*5000);

            TEST_INFO( "Getting back the IOR from the file for the multi-thread POA" );
            std::ifstream iorFile( "TestHello.ior" );

            std::string iorTestHello;
            iorFile >> iorTestHello;
            iorFile.close();

            CORBA::Object_var objHello = m_orb->string_to_object( iorTestHello.c_str() );
            Cdmw::TestHello_var testHello = Cdmw::TestHello::_narrow( objHello.in() );
            
            TEST_CHECK( ! CORBA::is_nil(testHello.in()) );

            testHello->hello();
            OsSupport::OS::sleep(timescale*2000);
            
            testHello->shutdown();
            OsSupport::OS::sleep(timescale*2000);
            

            std::ifstream ifs1("test.log");
            std::string s;
            bool string_found = false;
            if (ifs1.good())
            {
                while(getline(ifs1, s))    
                {
                    if(!strncmp(s.c_str(),"==>HELLO", 8))
                    {
                        string_found=true;
                        break;
                    }
                }
                ifs1.close();
            }
            TEST_CHECK(string_found);
            
            std::ifstream ifs2("test.log");
            string_found = false;
            if (ifs2.good())
            {
                while(getline(ifs2, s))    
                {
                    if(!strncmp(s.c_str(),"==>TAO", 6))
                    {
                        string_found=true;
                        break;
                    }
                }
                ifs2.close();
            }
            TEST_CHECK(string_found);
            OsSupport::OS::unlink("test.log");
        }

    } // End namespace Logging
} // End namespace Cdmw

