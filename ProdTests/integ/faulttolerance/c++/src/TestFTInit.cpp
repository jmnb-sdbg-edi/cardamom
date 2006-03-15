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

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "faulttolerance/TestFTInit.hpp"
#include "faulttolerance/TestUtils.hpp"

#include "faulttolerance/TestHello.stub.hpp"

#include "Foundation/commonsvcs/naming/NamingInterface.hpp"

namespace Cdmw {
namespace FT {

// constructor
TestFTInit::TestFTInit(
    CORBA::ORB_ptr orb,
    CdmwFT::ReplicationManager_ptr replication_manager,
    Cdmw::Test::HelloInterface_ptr group,
    const std::string& name)
: Testable( name ),
  m_orb( CORBA::ORB::_duplicate( orb ) ),
  m_replication_manager( CdmwFT::ReplicationManager::_duplicate(
      replication_manager ) ),
  m_group( Cdmw::Test::HelloInterface::_duplicate( group ) )
{    
}

// destructor
TestFTInit::~TestFTInit()
{
}

// do_tests
void TestFTInit::do_tests()
{
    std::string host_name = Cdmw::OsSupport::OS::get_hostname();

    try
    {
        TEST_INFO("Calling hello on HelloInterface while no primary elected...");
        m_group->hello();
        TEST_INFO("No CORBA System Exception raised as expected!");
        TEST_FAILED();
    }
    catch (const CORBA::TRANSIENT& e)
    {
        TEST_INFO("Expected CORBA::TRANSIENT Exception raised" << e);
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("Unexpected CORBA System Exception raised" << e);
        TEST_FAILED();  
    }

    try
    {
        TEST_INFO("Setting the primary...");
        ::FT::ObjectGroup_var object_group 
            = TestUtils::change_primary(m_group.in(), 
                                        m_replication_manager.in(), 
                                        host_name.c_str(), 
                                        "FTApplication", 
                                        "hello1" );

        //std::string object_group_str = m_orb->object_to_string(object_group.in());
        //TEST_INFO("New object_group_str = " << object_group_str);

        if (!CORBA::is_nil(object_group.in()))
        {
            m_group = Cdmw::Test::HelloInterface::_narrow(object_group.in());

            if (!CORBA::is_nil(m_group.in()))
            {
                TEST_SUCCEED();  
            }
            else
            {
                TEST_FAILED();
            }
        }
        else 
        {
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface...");
        m_group->hello();
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Changing the primary...");
        ::FT::ObjectGroup_var object_group 
            = TestUtils::change_primary(m_group.in(),
                                        m_replication_manager.in(), 
                                        host_name.c_str(), 
                                        "FTApplication", 
                                        "hello3" );

        //std::string object_group_str = m_orb->object_to_string(object_group.in());
        //TEST_INFO("New object_group_str = " << object_group_str);

        if (!CORBA::is_nil(object_group.in()))
        {
            m_group = Cdmw::Test::HelloInterface::_narrow(object_group.in());

            if (!CORBA::is_nil(m_group.in()))
            {
                TEST_SUCCEED();  
            }
            else
            {
                TEST_FAILED();
            }
        }
        else 
        {
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface...");
        m_group->hello();
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Changing the primary again...");
        TestUtils::change_primary(m_group.in(), 
                                        m_replication_manager.in(), 
                                        host_name.c_str(), 
                                        "FTApplication", 
                                        "hello1" );
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface with the old group ref...");
        m_group->hello();
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface with the old group ref...");
        m_group->hello();
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    Cdmw::Test::HelloInterface_var ext_hello_group;
    try
    {
        TEST_INFO("Create a new version outside CDMW context...");
        TEST_INFO("Neither the Replication Manager nor the Location Manager is aware!");

        ::FT::ObjectGroup_var object_group 
            = TestUtils::update_group_with_primary_outside_cdmw_ft
                          (m_orb.in(), 
                           m_group.in(), 
                           m_replication_manager.in(), 
                           host_name.c_str(), 
                           "FTApplication", 
                           "hello1" );

        if (!CORBA::is_nil(object_group.in()))
        {
            ext_hello_group = Cdmw::Test::HelloInterface::_narrow(object_group.in());

            if (!CORBA::is_nil(ext_hello_group.in()))
            {
                TEST_SUCCEED();  
            }
            else
            {
                TEST_FAILED();
            }
        }
        else 
        {
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface with the new external group ref...");
        ext_hello_group->hello();
        TEST_INFO("No CORBA System Exception raised as expected!");
        TEST_FAILED();
    }
    catch (const CORBA::INV_OBJREF& e)
    {
        TEST_INFO("Expected CORBA::INV_OBJREF Exception raised");
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("Unexpected CORBA System Exception raised" << e);
        TEST_FAILED();  
    }

    try
    {
        TEST_INFO("Changing the primary...");
        ::FT::ObjectGroup_var object_group 
            = TestUtils::change_primary(m_group.in(), 
                                        m_replication_manager.in(),
                                        host_name.c_str(), 
                                        "FTApplication", 
                                        "hello2" );

        if (!CORBA::is_nil(object_group.in()))
        {
            m_group = Cdmw::Test::HelloInterface::_narrow(object_group.in());

            if (!CORBA::is_nil(m_group.in()))
            {
                TEST_SUCCEED();  
            }
            else
            {
                TEST_FAILED();
            }
        }
        else 
        {
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface with the old group ref...");
        m_group->hello();
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    Cdmw::Test::process_info_var process_info;
    try
    {
        TEST_INFO("Calling who_is_called on HelloInterface to get ProcessInfo...");
        process_info = m_group->who_is_called();
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("killing current primary...");
        OsSupport::OS::kill_process(process_info->processID);
        TEST_SUCCEED();  
    }
    catch (const BadParameterException& e)
    {
        TEST_INFO("BadParameterException raised" << e);
        TEST_FAILED();
    }    
    catch (const PermissionDeniedException& e)
    {
        TEST_INFO("PermissionDeniedException raised" << e);
        TEST_FAILED();
    }    
    catch (const InternalErrorException& e)
    {
        TEST_INFO("InternalErrorException raised" << e);
        TEST_FAILED();
    }    

    OsSupport::OS::sleep(2000);
                    
    Cdmw::Test::process_info_var new_process_info;
    try
    {
        TEST_INFO("Calling who_is_called on HelloInterface to get ProcessInfo...");
        new_process_info = m_group->who_is_called();
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }

    TEST_INFO("Comparing both ProcessId, should be different...");
    if (process_info->processID == new_process_info->processID)
    {
        TEST_FAILED();
    }
    else
    {
        TEST_SUCCEED();
    }
}


} // end namespace FT

} // end namespace Cdmw

