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
#include <stdlib.h>
#include <string.h>

#include "teststdattributes_Application/TestStdAttributes_Application.hpp"

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

#include "SystemMngt/platformlibrary/GraphChecker.hpp"
#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"

using namespace std;

using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;
CPPUNIT_TEST_SUITE_REGISTRATION (TestStdAttributes_Application);


namespace Cdmw
{
namespace PlatformMngt
{



TestStdAttributes_Application::TestStdAttributes_Application()
{
}


TestStdAttributes_Application::~TestStdAttributes_Application()
{

}

/*
@brief   do_tests method for new navigability methods of IBB2
@brief   developments  .
*/

void
TestStdAttributes_Application::do_tests()                
{

    char flag_set_init_graph = 0;
    char flag_set_stop_graph = 0;

    CdmwPlatformMngt::System_var mysystem;

    try
    {
        CORBA::Object_var objref;

        const char *corbaloc_str = "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";

        m_orb = CORBA::ORB::_duplicate (Cdmw::TestUtils::CORBATestManager::instance ()->get_ORB ());

        objref = m_orb->string_to_object(corbaloc_str);

        /* resolve the System */
        mysystem = CdmwPlatformMngt::System::_narrow(objref.in());

        if (CORBA::is_nil(mysystem.in()))
        {
            TEST_INFO("Error in narrowing CdmwPlatformMngt::System");
            exit(-1);
        }

        std::string app_name = "HELLO_WORLD";
        
        /* from System, use a method to retrieve an application */
        CdmwPlatformMngt::Application_var app = mysystem->get_application (app_name.c_str());

       

        TEST_INFO ("Test to get application name");
        CORBA::String_var name = app->name();

        if (!strcmp (app_name.c_str(), name.in()))
        {
            CPPUNIT_ASSERT(1);  /* test ok */
        }
        else
        {
            CPPUNIT_ASSERT(0) ; /* test not ok */
        }

        TEST_INFO ("Test to get application status");

        CORBA::String_var status_info;
        
        CdmwPlatformMngt::ApplicationStatus app_status = app->get_status(status_info.out());
        
        if (app_status == CdmwPlatformMngt::APPLICATION_INITIAL ||
            app_status == CdmwPlatformMngt::APPLICATION_RUNNING)
        {
            CPPUNIT_ASSERT(1);
        }
        else
        {
            CPPUNIT_ASSERT(0);
        }
        
        TEST_INFO ("Test to get application mode");

        CORBA::String_var mode_info;
        
        CdmwPlatformMngt::FunctioningMode app_mode = app->get_mode(mode_info.out());
        
        if (app_mode == CdmwPlatformMngt::NORMAL_MODE)
        {
            CPPUNIT_ASSERT(1);
        }
        else
        {
            CPPUNIT_ASSERT(0);
        }
        
        TEST_INFO ("Test to set application init graph");

        // definition of process graph
        std::string process_name[2];
        
        process_name[0] = "HelloProcess1";
        process_name[1] = "HelloProcess2";
        unsigned short step = 0;
        unsigned int inx = 0;
        
        // the graph element factory
        Cdmw::PlatformMngt::GraphElementFactory graphElementFactory;
        
        // create the init graph element reference                         
        CdmwPlatformMngt::GraphElement_var init_graph_element1 =
             graphElementFactory.create (process_name[0].c_str(), step);
             
        CdmwPlatformMngt::GraphElement_var init_graph_element2 =
             graphElementFactory.create (process_name[1].c_str(), step);
        
        // Assign the successors's reference 
        inx = 0;     
        init_graph_element1->successors().length(1);    
        init_graph_element1->successors()[inx] = init_graph_element2;
        
        init_graph_element2->successors().length(0);
        
        // Determine the root elements of the init graph
        CdmwPlatformMngt::GraphElements_var init_root_elements = 
                       new CdmwPlatformMngt::GraphElements();
                       
        init_root_elements->length (1);
        init_root_elements[inx] = init_graph_element1;


        try
        {
            // set init graph to application
            app->set_init_graph (init_root_elements.in());
            flag_set_init_graph = 1;
        }
        catch (...)
        {
            flag_set_init_graph = 0;
        }
        
        if (flag_set_init_graph == 0)
        {
            CPPUNIT_ASSERT(0) ; /* test not ok */
        }
        else
        {
            CPPUNIT_ASSERT(1) ; /* test ok */
        }
        
        
        TEST_INFO ("Test to set application stop graph");
        
        // create the stop graph element reference                         
        CdmwPlatformMngt::GraphElement_var stop_graph_element1 =
             graphElementFactory.create (process_name[0].c_str(), 0);
             
        CdmwPlatformMngt::GraphElement_var stop_graph_element2 =
             graphElementFactory.create (process_name[1].c_str(), 0);
        
        // Assign the successors's reference     
        stop_graph_element1->successors().length(0);        
        stop_graph_element2->successors().length(0);
        
        // Determine the root elements of the stop graph
        CdmwPlatformMngt::GraphElements_var stop_root_elements = 
                       new CdmwPlatformMngt::GraphElements();
                       
        stop_root_elements->length (2);
        inx = 0; 
        stop_root_elements[inx] = stop_graph_element1;
        inx = 1; 
        stop_root_elements[inx] = stop_graph_element2;


        try
        {
            // set stop graph to application
            app->set_stop_graph (stop_root_elements.in());
            flag_set_stop_graph = 1;
        }
        catch (...)
        {
            flag_set_stop_graph = 0;
        }
        
        if (flag_set_stop_graph == 0)
        {
            CPPUNIT_ASSERT(0) ; /* test not ok */
        }
        else
        {
            CPPUNIT_ASSERT(1) ; /* test ok */
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

     // Call ORB cleanup
     Cdmw::OrbSupport::OrbSupport::ORB_cleanup(m_orb.in());

     // Destroy orb
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



} /* End namespace PlatformMngt */
} /* End namespace Cdmw */


