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


#include "testplatformlibrary/TestEventHandler.hpp"
#include "SystemMngt/platformlibrary/EventHandler.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/testutils/TestManager.hpp"

#include <memory>
#include <iostream>

using namespace std;
using namespace Cdmw::PlatformMngt;

CPPUNIT_TEST_SUITE_REGISTRATION( TestEventHandler );

// ----------------------------------------------------------------------
// ConcreteEvent class.
// ----------------------------------------------------------------------
ConcreteEvent::ConcreteEvent( const string& name )
    : Event()
{
    m_name = name;
    
    m_record.header = EventHeaderFactory::createHeader(CdmwPlatformMngtBase::INF);
}

void ConcreteEvent::execute()
{
    int timescale = Cdmw::TestUtils::get_timescale();
    Cdmw::OsSupport::OS::sleep(timescale*1000);
    cout << "execute( " << m_name << " )" << endl;
}

// ----------------------------------------------------------------------
// TestEventHandler class.
// ----------------------------------------------------------------------
TestEventHandler::TestEventHandler(  )
{
}

TestEventHandler::~TestEventHandler()
{
}

void TestEventHandler::do_tests()
{
    // set number of requested successfull tests
// //     set_nbOfRequestedTestOK (1);
    
    EventHandler eventHandler;
    eventHandler.start();

    TEST_INFO( "Try to add the events E0, E1, E2, E3, E4, E5, E6, E8 and E9" );
    try
    {
        auto_ptr< ConcreteEvent > e0( new ConcreteEvent( "E0" ) );
        auto_ptr< ConcreteEvent > e1( new ConcreteEvent( "E1" ) );
        auto_ptr< ConcreteEvent > e2( new ConcreteEvent( "E2" ) );
        auto_ptr< ConcreteEvent > e3( new ConcreteEvent( "E3" ) );
        auto_ptr< ConcreteEvent > e4( new ConcreteEvent( "E4" ) );
        auto_ptr< ConcreteEvent > e5( new ConcreteEvent( "E5" ) );
        auto_ptr< ConcreteEvent > e6( new ConcreteEvent( "E6" ) );
        auto_ptr< ConcreteEvent > e7( new ConcreteEvent( "E7" ) );
        auto_ptr< ConcreteEvent > e8( new ConcreteEvent( "E8" ) );
        auto_ptr< ConcreteEvent > e9( new ConcreteEvent( "E9" ) );

        eventHandler.addEvent( e0.get() ); e0.release();
        eventHandler.addEvent( e1.get() ); e1.release();
        eventHandler.addEvent( e2.get() ); e2.release();
        eventHandler.addEvent( e3.get() ); e3.release();
        eventHandler.addEvent( e4.get() ); e4.release();
        eventHandler.addEvent( e5.get() ); e5.release();
        eventHandler.addEvent( e6.get() ); e6.release();
        eventHandler.addEvent( e7.get() ); e7.release();
        eventHandler.addEvent( e8.get() ); e8.release();
        eventHandler.addEvent( e9.get() ); e9.release();

        CPPUNIT_ASSERT(true);
    }
    catch( const Cdmw::Exception& e )
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(false);
    }

    /*
    TEST_INFO( "" );
    try
    {
        CPPUNIT_ASSERT(true);
    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(false);
    }
    */

	int timescale = Cdmw::TestUtils::get_timescale();
    Cdmw::OsSupport::OS::sleep(timescale*5000);
    eventHandler.stop();
}

