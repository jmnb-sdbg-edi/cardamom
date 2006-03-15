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

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include "testexceptions/TestException.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "Foundation/common/String.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION( Cdmw::TestException ) ;

namespace Cdmw 
{



// constructor
/*
TestException::TestException(CORBA::ORB_ptr orb,
                             std::string daemon,
                             std::string system)
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_daemon(daemon),
      m_system(system)
{

}
*/

TestException::TestException()
{
}

// destructor
TestException::~TestException()
{
}

// do_tests
void TestException::do_tests()
{

    using namespace Cdmw::PlatformMngt;

// //     set_nbOfRequestedTestOK (4);
    TEST_INFO("Check if the supervision use CARDAMOM exception");
    try
    {
        char** argv__ = Cdmw::Common::String::to_char_array
        ( Cdmw::Common::String::to_strings(PlatformInterface::Get_process_name()));
        int argc__(1);
        
        PlatformInterface::Setup(m_orb.in(), argc__, argv__);
        CPPUNIT_ASSERT(false);
    }
    catch(const BadOrderException& )
    {
        CPPUNIT_ASSERT(true);
    }
    catch(...)
    {
        CPPUNIT_ASSERT(false);
    }

    TEST_INFO("Check if the supervision use CORBA exception");
    try
    {
        CORBA::Object_var service = PlatformInterface::Get_service ("My_Unknown_Service");
        CPPUNIT_ASSERT(false);
    }
    catch(const CdmwPlatformMngtService::ServiceNotFound &)
    {
        CPPUNIT_ASSERT(true); 
    }
    catch(...)
    {
        CPPUNIT_ASSERT(false);
    }
    
    TEST_INFO("Check if the daemon use CORBA exception");
    CORBA::Object_var obj = m_orb->string_to_object (m_daemon.c_str());
            
    if (!CORBA::is_nil (obj.in()))
    {
        CdmwPlatformMngt::Daemon_var daemon =
            CdmwPlatformMngt::Daemon::_narrow (obj.in());
        
        try
        {
            daemon->kill_process ("My_Unknown_Process");
            CPPUNIT_ASSERT(false);
        }
        catch(const CdmwPlatformMngt::ProcessNotFound&)
        {
            CPPUNIT_ASSERT(true);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
        
    } else {
        CPPUNIT_ASSERT(false);
    }
    
        
    TEST_INFO("Check if the supervision use CORBA exception");
    obj = m_orb->string_to_object (m_system.c_str());
            
    if (!CORBA::is_nil (obj.in()))
    {
        CdmwPlatformMngt::System_var _system = 
            CdmwPlatformMngt::System::_narrow(obj.in());
        
        try
        {
            _system->remove_host("My_Unknown_Host");
            CPPUNIT_ASSERT(false);
        }
        catch(const CdmwPlatformMngt::HostNotFound &)
        {
            CPPUNIT_ASSERT(true);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
        
    } else {
        CPPUNIT_ASSERT(false);
    }
     
        
}


} // end namespace Cdmw

