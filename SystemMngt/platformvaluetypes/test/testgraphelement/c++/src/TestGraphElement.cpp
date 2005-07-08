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
#include "testgraphelement/TestGraphElement.hpp"
#include "idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/platformvaluetypes/GraphElement_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"

using namespace std;
using namespace CdmwPlatformMngt;
using namespace Cdmw::PlatformMngt;

void subElementNamesToString(
    string str,
    GraphElement* graphElement,
    string& elementNames )
{
    // Print the current graph element
    char step[10] = {0};
    sprintf( step, "%u", graphElement->step() );
    elementNames += str + "|  " + "\n";
    elementNames += str + "+- " + graphElement->name() + step + "\n";

    // Print all the current graph element's successors
    for( unsigned int eltIndex=0;
         eltIndex < graphElement->successors().length();
         eltIndex++ )
        subElementNamesToString(
            str+"|  ",
            graphElement->successors()[ eltIndex ],
            elementNames );
}

TestGraphElement::TestGraphElement( const string& name ) : Testable( name )
{
}

TestGraphElement::~TestGraphElement()
{
}

void TestGraphElement::do_tests()
{
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

    TEST_INFO( "Create a graph" );
    try
    {
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

        // Get the actual response
        string expectedResult = string() +
            "|  "            + "\n"
            "+- a0"          + "\n"
            "|  |  "         + "\n"
            "|  +- a1"       + "\n"
            "|  |  |  "      + "\n"
            "|  |  +- b1"    + "\n"
            "|  |  |  "      + "\n"
            "|  |  +- c1"    + "\n"
            "|  |  |  |  "   + "\n"
            "|  |  |  +- d0" + "\n"
            "|  "            + "\n"
            "+- b0"          + "\n"
            "|  |  "         + "\n"
            "|  +- a1"       + "\n"
            "|  |  |  "      + "\n"
            "|  |  +- b1"    + "\n"
            "|  |  |  "      + "\n"
            "|  |  +- c1"    + "\n"
            "|  |  |  |  "   + "\n"
            "|  |  |  +- d0" + "\n"
            "|  "            + "\n"
            "+- c0"          + "\n"
            "|  |  "         + "\n"
            "|  +- c1"       + "\n"
            "|  |  |  "      + "\n"
            "|  |  +- d0"    + "\n";
        string actualResult   = "";

        for( unsigned int i=0; i<roots.length(); i++ )
            subElementNamesToString( "", roots[i], actualResult );

        // Print out the actual response
        cout << "The whole graph: " << endl
             << actualResult << endl;

        // The actual result must match the expected result
        if( actualResult != expectedResult )
            CDMW_THROW2(
                Cdmw::Exception,
                Cdmw::Exception::FATAL,
                "Unexpected response" );
        
        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        std::cerr << e.what() << endl;
        TEST_FAILED();
    }
}

