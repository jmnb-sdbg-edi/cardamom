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



#include <iostream>
#include <limits>

#include "testentitynavigability/TestEntityNav.hpp"

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "idllib/CdmwPlatformMngtSupervisionObserver.skel.hpp"
#include "idllib/CdmwPlatformMngtSystem.stub.hpp"
#include "idllib/CdmwPlatformMngtApplication.stub.hpp"

using namespace std;

using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;

CPPUNIT_TEST_SUITE_REGISTRATION (TestEntityNav);


namespace Cdmw
{
namespace PlatformMngt
{



TestEntityNav::TestEntityNav() 
{

}


TestEntityNav::~TestEntityNav()
{

}


void
TestEntityNav::do_tests()                
{
//     set_nbOfRequestedTestOK (19);

    try
    {
      /*get the orb */
        m_orb =
          CORBA::ORB::_duplicate (Cdmw::TestUtils::CORBATestManager::
                                  instance ()->get_ORB ());

        CORBA::Object_var objref;

        /*define the corbaloc*/
        const char *corbaloc_str = 
             "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";

        objref = m_orb->string_to_object(corbaloc_str);

        /* resolve the System */
        CdmwPlatformMngt::System_var system = 
             CdmwPlatformMngt::System::_narrow(objref.in());

        if (CORBA::is_nil(system.in()))
        {
            TEST_INFO("Error in narrowing CdmwPlatformMngt::System_var");
            exit(-1);
        }

        /* test: from system to entity */
        CdmwPlatformMngt::Entity_var sys_entity1 = CdmwPlatformMngt::Entity::_nil();        
        CdmwPlatformMngt::Entity_var sys_entity2 = CdmwPlatformMngt::Entity::_nil();
        
        TEST_INFO ("Test getting System Entity 1 from System");
        
        try
        {
            sys_entity1 = system->get_entity ("SYSTEM_ENTITY1");
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::EntityNotFound &ex)
        {
            TEST_INFO("Error: SYSTEM ENTITY 1 not found");
            CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
    
        TEST_INFO ("Test getting System Entity 2 from System");
        
        try
        {
            sys_entity2 = system->get_entity ("SYSTEM_ENTITY2");
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::EntityNotFound &ex)
        {
            TEST_INFO("Error: SYSTEM ENTITY 2 not found");
            CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
        
        /* tests: from entity to system */
        
        TEST_INFO ("Test retrieving System from System Entity 1");
        
        CdmwPlatformMngt::System_var system1 = CdmwPlatformMngt::System::_nil();
        
        if (!CORBA::is_nil(sys_entity1.in()))
        {
            try
            {
	            system1 = CdmwPlatformMngt::System::_narrow(sys_entity1->get_parent());

                // if System object is retrieved
                if (system->_is_equivalent(system1.in()))
	            {
	                CPPUNIT_ASSERT(true);
	            }
                else
	            {
                    CPPUNIT_ASSERT(false);
	            }
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        TEST_INFO ("Test retrieving System from System Entity 2");
        
        CdmwPlatformMngt::System_var system2 = CdmwPlatformMngt::System::_nil();
        
        if (!CORBA::is_nil(sys_entity2.in()))
        {
            try
            {
	            system2 = CdmwPlatformMngt::System::_narrow(sys_entity2->get_parent());

                // if System object is retrieved
                if (system->_is_equivalent(system2.in()))
	            {
	                CPPUNIT_ASSERT(true);
	            }
                else
	            {
                    CPPUNIT_ASSERT(false);
	            }
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        //////////////////////////
        
        /* test: from host to entity */
        CdmwPlatformMngt::Host_var host = CdmwPlatformMngt::Host::_nil(); 
        CdmwPlatformMngt::Entity_var host_entity1 = CdmwPlatformMngt::Entity::_nil();        
        CdmwPlatformMngt::Entity_var host_entity2 = CdmwPlatformMngt::Entity::_nil();
                
        TEST_INFO ("Test getting Host from System");
 
	    CdmwPlatformMngt::ElementNames_var elems;
 	    CdmwPlatformMngt::ElementNamesIterator_var it;

        // get number of hosts
        CORBA::ULong nbr_of_host = system->get_number_of_hosts();
        
        // get the name of hosts
	    system->get_all_host_names(nbr_of_host, elems.out(), it.out());
	    
	    const CdmwPlatformMngt::ElementName_var host_name = 
	          CORBA::string_dup ((elems.in())[0]);
	    
	    // for the first host get the entity
        try
        {
            host = system->get_host (host_name.in());
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::HostNotFound &ex)
        {
            TEST_INFO("Error: Host not found");
                CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
        
        TEST_INFO ("Test getting Host Entity 1 from Host");
        
        try
        {
            host_entity1 = host->get_entity ("HOST_ENTITY1");
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::EntityNotFound &ex)
        {
            TEST_INFO("Error: HOST ENTITY 1 not found");
            CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
    
        TEST_INFO ("Test getting Host Entity 2 from Host");
        
        try
        {
            host_entity2 = host->get_entity ("HOST_ENTITY2");
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::EntityNotFound &ex)
        {
            TEST_INFO("Error: HOST ENTITY 2 not found");
            CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
        
        /* tests: from entity to host */
        
        TEST_INFO ("Test retrieving Host from Host Entity 1");
        
        CdmwPlatformMngt::Host_var host1 = CdmwPlatformMngt::Host::_nil();
        
        if (!CORBA::is_nil(host_entity1.in()))
        {
            try
            {
	            host1 = CdmwPlatformMngt::Host::_narrow(host_entity1->get_parent());

                // if Host object is retrieved
                if (host->_is_equivalent(host1.in()))
	            {
	                CPPUNIT_ASSERT(true);
	            }
                else
	            {
                    CPPUNIT_ASSERT(false);
	            }
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        
        TEST_INFO ("Test retrieving Host from Host Entity 2");
        
        CdmwPlatformMngt::Host_var host2 = CdmwPlatformMngt::Host::_nil();
        
        if (!CORBA::is_nil(host_entity2.in()))
        {
            try
            {
	            host2 = CdmwPlatformMngt::Host::_narrow(host_entity2->get_parent());

                // if Host object is retrieved
                if (host->_is_equivalent(host2.in()))
	            {
	                CPPUNIT_ASSERT(true);
	            }
                else
	            {
                    CPPUNIT_ASSERT(false);
	            }
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }

        //////////////////////////
        
        /* test: from application to entity */
        CdmwPlatformMngt::Application_var application = CdmwPlatformMngt::Application::_nil(); 
        CdmwPlatformMngt::Entity_var app_entity1 = CdmwPlatformMngt::Entity::_nil();        
        CdmwPlatformMngt::Entity_var app_entity2 = CdmwPlatformMngt::Entity::_nil();
                
        TEST_INFO ("Test getting Application APPL1 from System");
        
        try
        {
            application = system->get_application ("APPL1");
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::ApplicationNotFound &ex)
        {
            TEST_INFO("Error: APPLICATION APPL1 not found");
            CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
        
        TEST_INFO ("Test getting Application Entity 1 from Application APPL1");
        
        try
        {
            app_entity1 = application->get_entity ("APPLICATION_ENTITY1");
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::EntityNotFound &ex)
        {
            TEST_INFO("Error: APPLICATION ENTITY 1 not found");
            CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
    
        TEST_INFO ("Test getting Application Entity 2 from Application APPL1");
        
        try
        {
            app_entity2 = application->get_entity ("APPLICATION_ENTITY2");
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::EntityNotFound &ex)
        {
            TEST_INFO("Error: APPLICATION ENTITY 2 not found");
            CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
        
        /* tests: from entity to application */
        
        TEST_INFO ("Test retrieving Application from Application Entity 1");
        
        CdmwPlatformMngt::Application_var application1 = CdmwPlatformMngt::Application::_nil();
        
        if (!CORBA::is_nil(app_entity1.in()))
        {
            try
            {
	            application1 = CdmwPlatformMngt::Application::_narrow(app_entity1->get_parent());

                // if Application object is retrieved
                if (application->_is_equivalent(application1.in()))
	            {
	                CPPUNIT_ASSERT(true);
	            }
                else
	            {
                    CPPUNIT_ASSERT(false);
	            }
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        
        TEST_INFO ("Test retrieving Application from Application Entity 2");
        
        CdmwPlatformMngt::Application_var application2 = CdmwPlatformMngt::Application::_nil();
        
        if (!CORBA::is_nil(app_entity2.in()))
        {
            try
            {
	            application2 = CdmwPlatformMngt::Application::_narrow(app_entity2->get_parent());

                // if Application object is retrieved
                if (application->_is_equivalent(application2.in()))
	            {
	                CPPUNIT_ASSERT(true);
	            }
                else
	            {
                    CPPUNIT_ASSERT(false);
	            }
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        
        //////////////////////////
        
        /* test: from process to entity */
        CdmwPlatformMngt::Process_var process = CdmwPlatformMngt::Process::_nil(); 
        CdmwPlatformMngt::Entity_var proc_entity1 = CdmwPlatformMngt::Entity::_nil();        
        CdmwPlatformMngt::Entity_var proc_entity2 = CdmwPlatformMngt::Entity::_nil();
                
        TEST_INFO ("Test getting Process PROC11 from Application APPL1");
        
        try
        {
            process = application->get_process ("PROC11", host_name.in());
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::ProcessNotFound &ex)
        {
            TEST_INFO("Error: PROCESS PROC11 not found");
            CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
        
        TEST_INFO ("Test getting Process Entity 1 from Process PROC11");
        
        try
        {
            proc_entity1 = process->get_entity ("PROCESS_ENTITY1");
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::EntityNotFound &ex)
        {
            TEST_INFO("Error: PROCESS ENTITY 1 not found");
            CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
    
        TEST_INFO ("Test getting Process Entity 2 from Process PROC11");
        
        try
        {
            proc_entity2 = process->get_entity ("PROCESS_ENTITY2");
            CPPUNIT_ASSERT(true);
        }
        catch (CdmwPlatformMngt::EntityNotFound &ex)
        {
            TEST_INFO("Error: PROCESS ENTITY 2 not found");
            CPPUNIT_ASSERT(false);
        }
        catch(...)
        {
            CPPUNIT_ASSERT(false);
        }
        
        /* tests: from entity to process */
        
        TEST_INFO ("Test retrieving Process from Process Entity 1");
        
        CdmwPlatformMngt::Process_var process1 = CdmwPlatformMngt::Process::_nil();
        
        if (!CORBA::is_nil(proc_entity1.in()))
        {
            try
            {
	            process1 = CdmwPlatformMngt::Process::_narrow(proc_entity1->get_parent());

                // if Process object is retrieved
                if (process->_is_equivalent(process1.in()))
	            {
	                CPPUNIT_ASSERT(true);
	            }
                else
	            {
                    CPPUNIT_ASSERT(false);
	            }
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
        
        TEST_INFO ("Test retrieving Process from Process Entity 2");
        
        CdmwPlatformMngt::Process_var process2 = CdmwPlatformMngt::Process::_nil();
        
        if (!CORBA::is_nil(proc_entity2.in()))
        {
            try
            {
	            process2 = CdmwPlatformMngt::Process::_narrow(proc_entity2->get_parent());

                // if Process object is retrieved
                if (process->_is_equivalent(process2.in()))
	            {
	                CPPUNIT_ASSERT(true);
	            }
                else
	            {
                    CPPUNIT_ASSERT(false);
	            }
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
        }
        else
        {
            CPPUNIT_ASSERT(false);
        }
        
    }
    catch (const Cdmw::Exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        exit(-1);
    }
    catch(const CORBA::Exception& ex)
    {        
        std::cerr << ex << std::endl;
        exit(-1);
    }
    catch(...)
    {
        std::cerr << "unexpected exception" << std::endl;
        exit(-1);
    }

    /*
    ========================================================
    program stopping
    ========================================================
    */

    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(m_orb.in());

    if (!CORBA::is_nil(m_orb.in()))
    {
        try
        {     
            m_orb->destroy();
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << ex << std::endl;
            exit(-1);
        }
    }
}    



} /* End namespace PlatformMngt*/
} /* End namespace Cdmw*/


