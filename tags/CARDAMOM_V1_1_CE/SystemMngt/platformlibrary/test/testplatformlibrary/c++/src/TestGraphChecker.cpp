/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include "Foundation/orbsupport/CORBA.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"

#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "SystemMngt/platformlibrary/GraphChecker.hpp"

#include "testplatformlibrary/TestGraphChecker.hpp"

using namespace std;
using namespace CdmwPlatformMngt;
using namespace Cdmw::PlatformMngt;

TestGraphChecker ::TestGraphChecker ( const string& name )
: Testable( name )
{
}

TestGraphChecker ::~TestGraphChecker ()
{
}

void TestGraphChecker ::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (8);
    
    
    ExistingElementMap existingElementMap;
    #if CDMW_ORB_VDR == tao
    GraphElementFactory factory;
    #else
    CORBA::ValueFactoryBase_var factory;
    #endif
    CdmwPlatformMngt::GraphElements roots;
    GraphElement_var a0;
    GraphElement_var a1;
    GraphElement_var a2;
    GraphElement_var a3;
    GraphElement_var b0;
    GraphElement_var b1;
    GraphElement_var c0;
    GraphElement_var c1;
    GraphElement_var d0;
    GraphElement_var d1;
    GraphElement_var e0;

    existingElementMap.insert( ExistingElementMap::value_type( "a", 4 ) );
    existingElementMap.insert( ExistingElementMap::value_type( "b", 2 ) );
    existingElementMap.insert( ExistingElementMap::value_type( "c", 2 ) );
    existingElementMap.insert( ExistingElementMap::value_type( "d", 1 ) );

    #if CDMW_ORB_VDR == tao
    a0 = factory.create( "a", 0 );
    a1 = factory.create( "a", 1 );
    a2 = factory.create( "a", 2 );
    a3 = factory.create( "a", 3 );
    b0 = factory.create( "b", 0 );
    b1 = factory.create( "b", 1 );
    c0 = factory.create( "c", 0 );
    c1 = factory.create( "c", 1 );
    d0 = factory.create( "d", 0 );
    d1 = factory.create( "d", 1 );
    e0 = factory.create( "e", 0 );
    #else
    factory = new GraphElementFactory();
    a0 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "a", 0 );
    a1 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "a", 1 );
    a2 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "a", 2 );
    a3 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "a", 3 );
    b0 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "b", 0 );
    b1 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "b", 1 );
    c0 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "c", 0 );
    c1 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "c", 1 );
    d0 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "d", 0 );
    d1 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "d", 1 );
    e0 = dynamic_cast<GraphElementFactory*>( factory.in() )->create( "e", 0 );
    #endif

    TEST_INFO( "Check the following graph is valid" );
    try
    {
        a0->successors().length( 0 );
        a1->successors().length( 0 );
        a2->successors().length( 0 );
        a3->successors().length( 0 );
        b0->successors().length( 0 );
        b1->successors().length( 0 );
        c0->successors().length( 0 );
        c1->successors().length( 0 );
        d0->successors().length( 0 );
        d1->successors().length( 0 );
        e0->successors().length( 0 );

        cout << "--> a0 -->|          |--> b1           " << endl;
        cout << "          |--> a1 -->|                 " << endl;
        cout << "--> b0 -->|          |-->|             " << endl;
        cout << "                         |--> c1 --> d0" << endl;
        cout << "--> c0 ----------------->|             " << endl;

        a0->successors().length( 1 );
        a0->successors()[ 0 ] = a1;
        a1->successors().length( 2 );
        a1->successors()[ 0 ] = b1;
        a1->successors()[ 1 ] = c1;
        c1->successors().length( 1 );
        c1->successors()[ 0 ] = d0;
        b0->successors().length( 1 );
        b0->successors()[ 0 ] = a1;
        c0->successors().length( 1 );
        c0->successors()[ 0 ] = c1;

        roots.length( 3 );
        roots[0] = a0;
        roots[1] = b0;
        roots[2] = c0;

        GraphChecker processGraph( roots, NULL, false );
        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        TEST_FAILED();
    }

    TEST_INFO( "Check the following graph is valid" );
    try
    {
        a0->successors().length( 0 );
        a1->successors().length( 0 );
        a2->successors().length( 0 );
        a3->successors().length( 0 );
        b0->successors().length( 0 );
        b1->successors().length( 0 );
        c0->successors().length( 0 );
        c1->successors().length( 0 );
        d0->successors().length( 0 );
        d1->successors().length( 0 );
        e0->successors().length( 0 );
		roots.length( 0 );

        cout << "--> a0 -->|          |--> b1 ----------------->|      " << endl;
        cout << "          |--> a1 -->|                         |      " << endl;
        cout << "--> b0 -->|          |-->|          |--> d0    |--> a3" << endl;
        cout << "                         |--> c1 -->|          |      " << endl;
        cout << "--> c0 ----------------->|          |--> a2 -->|      " << endl;

        a0->successors().length( 1 );
        a0->successors()[ 0 ] = a1;
        a1->successors().length( 2 );
        a1->successors()[ 0 ] = b1;
        a1->successors()[ 1 ] = c1;
        b1->successors().length( 1 );
        b1->successors()[ 0 ] = a3;
        c1->successors().length( 2 );
        c1->successors()[ 0 ] = d0;
        c1->successors()[ 1 ] = a2;
        a2->successors().length( 1 );
        a2->successors()[ 0 ] = a3;
        b0->successors().length( 1 );
        b0->successors()[ 0 ] = a1;
        c0->successors().length( 1 );
        c0->successors()[ 0 ] = c1;

        roots.length( 3 );
        roots[0] = a0;
        roots[1] = b0;
        roots[2] = c0;

        GraphChecker processGraph( roots, NULL, false );
        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        TEST_FAILED();
    }

    TEST_INFO( "Check the following graph contains a step jump" );
    try
    {
        a0->successors().length( 0 );
        a1->successors().length( 0 );
        a2->successors().length( 0 );
        a3->successors().length( 0 );
        b0->successors().length( 0 );
        b1->successors().length( 0 );
        c0->successors().length( 0 );
        c1->successors().length( 0 );
        d0->successors().length( 0 );
        d1->successors().length( 0 );
        e0->successors().length( 0 );
		roots.length( 0 );


        cout << "--> a0 -->|          |--> b1           " << endl;
        cout << "          |--> a1 -->|                 " << endl;
        cout << "--> b0 -->|          |-->|             " << endl;
        cout << "                         |--> c1 --> d0" << endl;
        cout << "--> c0 ----------------->|             " << endl;
        cout << "                                       " << endl;
        cout << "--> d1                                 " << endl;
        cout << "    ^^                                 " << endl;

        a0->successors().length( 1 );
        a0->successors()[ 0 ] = a1;
        a1->successors().length( 2 );
        a1->successors()[ 0 ] = b1;
        a1->successors()[ 1 ] = c1;
        c1->successors().length( 1 );
        c1->successors()[ 0 ] = d0;
        b0->successors().length( 1 );
        b0->successors()[ 0 ] = a1;
        c0->successors().length( 1 );
        c0->successors()[ 0 ] = c1;

        roots.length( 4 );
        roots[0] = a0;
        roots[1] = b0;
        roots[2] = c0;
        roots[3] = d1;

        GraphChecker processGraph( roots, NULL, false );
        TEST_FAILED();
    }
    catch( CdmwPlatformMngt::StepJump& e )
    {
        cerr << e._name() << endl;
        cerr << "The element '" << e.name.in()
             << "' jumps from the step " << e.origin_step
             << " to the step " << e.target_step << endl;
        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        TEST_FAILED();
    }

    TEST_INFO( "Check the following graph contains a step jump" );
    try
    {
        a0->successors().length( 0 );
        a1->successors().length( 0 );
        a2->successors().length( 0 );
        a3->successors().length( 0 );
        b0->successors().length( 0 );
        b1->successors().length( 0 );
        c0->successors().length( 0 );
        c1->successors().length( 0 );
        d0->successors().length( 0 );
        d1->successors().length( 0 );
        e0->successors().length( 0 );
		roots.length( 0 );

        cout << "--> a0 -->|          |--> b1 --> a3    " << endl;
        cout << "          |--> a1 -->|           ^^    " << endl;
        cout << "--> b0 -->|    ^^    |-->|             " << endl;
        cout << "                         |--> c1 --> d0" << endl;
        cout << "--> c0 ----------------->|             " << endl;

        a0->successors().length( 1 );
        a0->successors()[ 0 ] = a1;
        a1->successors().length( 2 );
        a1->successors()[ 0 ] = b1;
        a1->successors()[ 1 ] = c1;
        b1->successors().length( 1 );
        b1->successors()[ 0 ] = a3;
        c1->successors().length( 1 );
        c1->successors()[ 0 ] = d0;
        b0->successors().length( 1 );
        b0->successors()[ 0 ] = a1;
        c0->successors().length( 1 );
        c0->successors()[ 0 ] = c1;

        roots.length( 3 );
        roots[0] = a0;
        roots[1] = b0;
        roots[2] = c0;

        GraphChecker processGraph( roots, NULL, false );
        TEST_FAILED();
    }
    catch( CdmwPlatformMngt::StepJump& e )
    {
        cerr << e._name() << endl;
        cerr << "The element '" << e.name.in()
             << "' jumps from the step " << e.origin_step
             << " to the step " << e.target_step << endl;
        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        TEST_FAILED();
    }

    TEST_INFO( "Check the following graph is cyclic" );
    try
    {
        a0->successors().length( 0 );
        a1->successors().length( 0 );
        a2->successors().length( 0 );
        a3->successors().length( 0 );
        b0->successors().length( 0 );
        b1->successors().length( 0 );
        c0->successors().length( 0 );
        c1->successors().length( 0 );
        d0->successors().length( 0 );
        d1->successors().length( 0 );
        e0->successors().length( 0 );
		roots.length( 0 );

        cout << "               +----------------------------+" << endl;
        cout << "               |                            |" << endl;
        cout << "--> a0 -->|    v     |--> b1                |" << endl;
        cout << "          |--> a1 -->|                      |" << endl;
        cout << "--> b0 -->|    ^^    |-->|          |--> d0 |" << endl;
        cout << "                         |--> c1 -->|       |" << endl;
        cout << "--> c0 ----------------->|          |-------+" << endl;

        a0->successors().length( 1 );
        a0->successors()[ 0 ] = a1;
        a1->successors().length( 2 );
        a1->successors()[ 0 ] = b1;
        a1->successors()[ 1 ] = c1;
        c1->successors().length( 2 );
        c1->successors()[ 0 ] = d0;
        c1->successors()[ 1 ] = a1;
        b0->successors().length( 1 );
        b0->successors()[ 0 ] = a1;
        c0->successors().length( 1 );
        c0->successors()[ 0 ] = c1;

        roots.length( 3 );
        roots[0] = a0;
        roots[1] = b0;
        roots[2] = c0;

        GraphChecker processGraph( roots, NULL, false );
        TEST_FAILED();
    }
    catch( CdmwPlatformMngt::CircularReference& e )
    {
        cerr << e._name() << endl;
        cerr << "The graph contains a circular reference: '"
             << e.name.in() << e.step << "'" << endl;
        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        TEST_FAILED();
    }
    
    TEST_INFO( "Check the following graph is cyclic" );
    try
    {
        a0->successors().length( 0 );
        a1->successors().length( 0 );
        a2->successors().length( 0 );
        a3->successors().length( 0 );
        b0->successors().length( 0 );
        b1->successors().length( 0 );
        c0->successors().length( 0 );
        c1->successors().length( 0 );
        d0->successors().length( 0 );
        d1->successors().length( 0 );
        e0->successors().length( 0 );
		roots.length( 0 );

        cout << "--> a0 -->|          |--> b1                 " << endl;
        cout << "          |--> a1 -->|                       " << endl;
        cout << "--> b0 -->|          |-->|          |--> d0  " << endl;
        cout << "                         |--> c1 -->|        " << endl;
        cout << "--> c0 ----------------->|          |-------+" << endl;
        cout << "--> e0                                      |" << endl;
        cout << "    ^^                                      |" << endl;
        cout << "    |                                       |" << endl;
        cout << "    +---------------------------------------+" << endl;

        a0->successors().length( 1 );
        a0->successors()[ 0 ] = a1;
        a1->successors().length( 2 );
        a1->successors()[ 0 ] = b1;
        a1->successors()[ 1 ] = c1;
        c1->successors().length( 2 );
        c1->successors()[ 0 ] = d0;
        c1->successors()[ 1 ] = e0;
        b0->successors().length( 1 );
        b0->successors()[ 0 ] = a1;
        c0->successors().length( 1 );
        c0->successors()[ 0 ] = c1;

        roots.length( 4 );
        roots[0] = a0;
        roots[1] = b0;
        roots[2] = c0;
        roots[3] = e0;

        GraphChecker processGraph( roots, NULL, false );
        TEST_FAILED();
    }
    catch( CdmwPlatformMngt::CircularReference& e )
    {
        cerr << e._name() << endl;
        cerr << "The graph contains a circular reference: '"
             << e.name.in() << e.step << "'" << endl;
        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        TEST_FAILED();
    }

    TEST_INFO( "Check the step range of all the graph element" );
    try
    {
        a0->successors().length( 0 );
        a1->successors().length( 0 );
        a2->successors().length( 0 );
        a3->successors().length( 0 );
        b0->successors().length( 0 );
        b1->successors().length( 0 );
        c0->successors().length( 0 );
        c1->successors().length( 0 );
        d0->successors().length( 0 );
        d1->successors().length( 0 );
        e0->successors().length( 0 );
		roots.length( 0 );

        cout << "--> a0 -->|          |--> b1                  " << endl;
        cout << "          |--> a1 -->|                        " << endl;
        cout << "--> b0 -->|          |-->|                    " << endl;
        cout << "                         |--> c1 --> d0 --> d1" << endl;
        cout << "--> c0 ----------------->|                  ^^" << endl;

        a0->successors().length( 1 );
        a0->successors()[ 0 ] = a1;
        a1->successors().length( 2 );
        a1->successors()[ 0 ] = b1;
        a1->successors()[ 1 ] = c1;
        c1->successors().length( 1 );
        c1->successors()[ 0 ] = d0;
        d0->successors().length( 1 );
        d0->successors()[ 0 ] = d1;
        b0->successors().length( 1 );
        b0->successors()[ 0 ] = a1;
        c0->successors().length( 1 );
        c0->successors()[ 0 ] = c1;

        roots.length( 3 );
        roots[0] = a0;
        roots[1] = b0;
        roots[2] = c0;

        GraphChecker processGraph( roots, &existingElementMap, false );
        TEST_FAILED();
    }
    catch( CdmwPlatformMngt::StepOutOfBound& e )
    {
        cerr << e._name() << endl;
        cerr << "The step " << e.step
             << " of the element '" << e.name.in()
             << "' is out of bound." << endl;

        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        TEST_FAILED();
    }

    TEST_INFO( "Check the existence of all the steps of the following graph" );
    try
    {
        a0->successors().length( 0 );
        a1->successors().length( 0 );
        a2->successors().length( 0 );
        a3->successors().length( 0 );
        b0->successors().length( 0 );
        b1->successors().length( 0 );
        c0->successors().length( 0 );
        c1->successors().length( 0 );
        d0->successors().length( 0 );
        d1->successors().length( 0 );
        e0->successors().length( 0 );
		roots.length( 0 );

        cout << "--> a0 -->|          |--> b1 --> e0    " << endl;
        cout << "          |--> a1 -->|           ^^    " << endl;
        cout << "--> b0 -->|          |-->|             " << endl;
        cout << "                         |--> c1 --> d0" << endl;
        cout << "--> c0 ----------------->|             " << endl;

        a0->successors().length( 1 );
        a0->successors()[ 0 ] = a1;
        a1->successors().length( 2 );
        a1->successors()[ 0 ] = b1;
        a1->successors()[ 1 ] = c1;
        b1->successors().length( 1 );
        b1->successors()[ 0 ] = e0;
        c1->successors().length( 1 );
        c1->successors()[ 0 ] = d0;
        b0->successors().length( 1 );
        b0->successors()[ 0 ] = a1;
        c0->successors().length( 1 );
        c0->successors()[ 0 ] = c1;

        roots.length( 3 );
        roots[0] = a0;
        roots[1] = b0;
        roots[2] = c0;

        GraphChecker processGraph( roots, &existingElementMap, false );
        TEST_FAILED();
    }
    catch( CdmwPlatformMngt::ReferenceNotFound& e )
    {
        cerr << e._name() << endl;
        cerr << "The element '" << e.name.in()
             << "' doesn't refer to an existing element." << endl;
        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        cerr << e._name() << endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        cerr << e.what() << endl;
        TEST_FAILED();
    }
}

