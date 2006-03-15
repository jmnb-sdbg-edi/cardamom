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
#include <sstream>
#include "Foundation/orbsupport/CORBA.hpp"
#include "testplatformlibrary/TestGraphUtility.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "SystemMngt/platformlibrary/GraphUtility.hpp"

using namespace std;
using namespace CdmwPlatformMngt;
using namespace Cdmw::PlatformMngt;

CPPUNIT_TEST_SUITE_REGISTRATION( TestGraphUtility );

TestGraphUtility::TestGraphUtility()
{
}

TestGraphUtility::~TestGraphUtility()
{
}

void TestGraphUtility::do_tests()
{
    // set number of requested successfull tests
// //     set_nbOfRequestedTestOK (3);
    
    #if CDMW_ORB_VDR == tao
    GraphElementFactory factory;
    #else
    CORBA::ValueFactoryBase_var factory;
    #endif
    CdmwPlatformMngt::GraphElements roots;
    GraphElement_var a0;
    GraphElement_var a1;
    GraphElement_var b0;
    GraphElement_var b1;
    GraphElement_var c0;
    GraphElement_var c1;
    GraphElement_var d0;
    SynchronisableTaskDefMap taskMap;
    TaskIdList rootTasks;

    cout << "Creates the following graph :    " << endl;
    cout << endl;
    cout << "a0 -->|         +--> b1            " << endl;
    cout << "      |--> a1 --|                  " << endl;
    cout << "b0 -->|         +-->|              " << endl;
    cout << "                    |--> c1 --> d0 " << endl;
    cout << "c0 ---------------->|              " << endl;
    cout << endl;

    #if CDMW_ORB_VDR == tao
    a0 = factory.create("a",0);
    a1 = factory.create("a",1);
    b0 = factory.create("b",0);
    b1 = factory.create("b",1);
    c0 = factory.create("c",0);
    c1 = factory.create("c",1);
    d0 = factory.create("d",0);
    #else
    factory = new GraphElementFactory();
    a0 = dynamic_cast<GraphElementFactory*>(factory.in())->create("a",0);
    a1 = dynamic_cast<GraphElementFactory*>(factory.in())->create("a",1);
    b0 = dynamic_cast<GraphElementFactory*>(factory.in())->create("b",0);
    b1 = dynamic_cast<GraphElementFactory*>(factory.in())->create("b",1);
    c0 = dynamic_cast<GraphElementFactory*>(factory.in())->create("c",0);
    c1 = dynamic_cast<GraphElementFactory*>(factory.in())->create("c",1);
    d0 = dynamic_cast<GraphElementFactory*>(factory.in())->create("d",0);
    #endif

    a0->successors().length( 1 );
    a0->successors()[ 0 ] = a1;
    b0->successors().length( 1 );
    b0->successors()[ 0 ] = a1;
    a1->successors().length( 2 );
    a1->successors()[ 0 ] = b1;
    a1->successors()[ 1 ] = c1;
    c0->successors().length( 1 );
    c0->successors()[ 0 ] = c1;
    c1->successors().length( 1 );
    c1->successors()[ 0 ] = d0;

    roots.length( 3 );
    roots[0] = a0;
    roots[1] = b0;
    roots[2] = c0;

    TEST_INFO( "Get the task map" );
    try
    {
        GraphUtility::getTaskDefinitions( roots, false, taskMap, rootTasks );

        // Get the actual response
        string expectedResult = string()
             + "a\\0 : 0 : a\\1, "       + "\n"
             + "a\\1 : 2 : b\\1, c\\1, " + "\n"
             + "b\\0 : 0 : a\\1, "       + "\n"
             + "b\\1 : 1 : "             + "\n"
             + "c\\0 : 0 : c\\1, "       + "\n"
             + "c\\1 : 2 : d\\0, "       + "\n"
             + "d\\0 : 1 : "             + "\n";

        // Go throug each element of the map
        ostringstream actualResult;
        SynchronisableTaskDefMap::iterator taskIt;

        for( taskIt=taskMap.begin(); taskIt!=taskMap.end(); taskIt++ )
        {
            // display the task id and the amount of predecessors
            actualResult << taskIt->first << " : "
                         << taskIt->second.m_nbPredecessors << " : ";
            
            // display the successors' task id
            TaskIdList::iterator successorIt;
            for( successorIt =  taskIt->second.m_successors_id.begin();
                 successorIt != taskIt->second.m_successors_id.end();
                 successorIt++ )
                actualResult << *successorIt << ", ";

            actualResult << endl;
        }

        // Print out the actual response
        cout << actualResult.str() << endl;

        // The actual result must match the expected result
        if( expectedResult != actualResult.str() )
            CDMW_THROW2(
                Cdmw::Exception,
                Cdmw::Exception::FATAL,
                "Unexpected response" );
        
        CPPUNIT_ASSERT(true);
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        CPPUNIT_ASSERT(false);
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        CPPUNIT_ASSERT(false);
    }

    TEST_INFO( "Get the root tasks" );
    try
    {
        // Get the actual response
        string expectedResult = "a\\0, b\\0, c\\0, ";
        string actualResult   = "";

        TaskIdList::iterator rootTaskIt;
        for( rootTaskIt =  rootTasks.begin();
             rootTaskIt != rootTasks.end();
             rootTaskIt++ )
            actualResult += *rootTaskIt + ", ";

        // Print the actual response
        cout << actualResult << endl;

        // The actual result must match the expected result
        if( actualResult != expectedResult )
            CDMW_THROW2(
                Cdmw::Exception,
                Cdmw::Exception::FATAL,
                "Unexpected response" );
        
        CPPUNIT_ASSERT(true);
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        CPPUNIT_ASSERT(false);
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        CPPUNIT_ASSERT(false);
    }

    TEST_INFO( "Get the graph element names" );
    try
    {
        GraphElementNames elementNames
            = GraphUtility::getElementNames( roots );

        // Get the actual response
        string expectedResult = "a, b, c, d, ";
        string actualResult   = "";

        GraphElementNames::iterator elementNameIt;
        for( elementNameIt =  elementNames.begin();
             elementNameIt != elementNames.end();
             elementNameIt++ )
            actualResult += *elementNameIt + ", ";

        // Print the actual response
        cout << actualResult << endl;

        // The actual result must match the expected result
        if( actualResult != expectedResult )
            CDMW_THROW2(
                Cdmw::Exception,
                Cdmw::Exception::FATAL,
                "Unexpected response" );
        
        CPPUNIT_ASSERT(true);
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        CPPUNIT_ASSERT(false);
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        CPPUNIT_ASSERT(false);
    }
}

