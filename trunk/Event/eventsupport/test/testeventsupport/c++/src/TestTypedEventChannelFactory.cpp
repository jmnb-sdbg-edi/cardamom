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
#include <string>
#include <sstream>
#include "testeventsupport/TestTypedEventChannelFactory.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

namespace
{
    const char* CHANNEL_1_NAME = "TestTypedEventChannel1";
    const char* CHANNEL_2_NAME = "TestTypedEventChannel2";
     const char* MIS_URL_CHANNEL_NAME = "TestTypedEventChannel3";
    const char* UNDEF_XML_CHANNEL_NAME = "TestTypedEventChannel4";
     const char* INV_CHANNEL_NAME = "Test/EventChannel4";
};

namespace Cdmw
{
namespace Event
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestTypedEventChannelFactory::TestTypedEventChannelFactory(CdmwEvent::TypedEventChannelFactory_ptr factory)
    throw()
    :Testable("Cdmw::Event::TypedEventChannelFactory"),
      m_factory(CdmwEvent::TypedEventChannelFactory::_duplicate(factory)),
     m_test_number(1)
{
    // does nothing
}


TestTypedEventChannelFactory::~TestTypedEventChannelFactory()
    throw()
{
    // does nothing
}


TestTypedEventChannelFactory::TestTypedEventChannelFactory(const TestTypedEventChannelFactory& rhs)
    throw()
    :Testable("Cdmw::Event::TypedEventChannelFactory"),
     m_factory(CdmwEvent::TypedEventChannelFactory::_duplicate(rhs.m_factory.in()))
{
    // does nothing
}
        
TestTypedEventChannelFactory&
TestTypedEventChannelFactory::operator=(const TestTypedEventChannelFactory& rhs)
    throw()
{
    if (this != &rhs) {
        m_factory = CdmwEvent::TypedEventChannelFactory::_duplicate(rhs.m_factory.in());
    }
    return *this;
}

void
TestTypedEventChannelFactory::next_test()
{
    m_test_number++;
    start();
}

void TestTypedEventChannelFactory::do_tests()        
{
#if CDMW_ORB_VDR == orbacus        
    // set number of requested successfull tests
    // start/do_tests is recalled several times
    set_nbOfRequestedTestOK (0);
     
    bool test_ok=false;
    CosTypedEventChannelAdmin::TypedEventChannel_var channel;


    // This tests EventChannelFactory class
    //-------------------------------------------------
    TEST_INFO("Testing TypedEventChannelFactory");

    try 
    {        
        switch (m_test_number)
        {
          case 1 :
          
            // add number of requested successfull tests    
            add_nbOfRequestedTestOK (1);
   
            TEST_INFO("Creating a new typed channel CHANNEL 1 while EventchannelManager is not running");
            try
            {
                channel = m_factory->create_event_channel(CHANNEL_1_NAME);
            }
            catch (CdmwEvent::CreationError& e)
            {
                test_ok = true;
                std::ostringstream os;
                os << "CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::AlreadyExistant& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.name.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::InvalidName& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CORBA::Exception& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e << std::ends;
                TEST_INFO(os.str());
            }
            catch (...)
            {
                // test failed!
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);
            break;
            
          case 2:
          
            // add number of requested successfull tests    
            add_nbOfRequestedTestOK (1);
            
            TEST_INFO("Creating a new typed channel CHANNEL 1 while EventServer is not running");
            test_ok = false;
            try
            {
                channel = m_factory->create_event_channel(CHANNEL_1_NAME);
            }
            catch (CdmwEvent::CreationError& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::AlreadyExistant& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.name.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::InvalidName& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CORBA::TRANSIENT& e)
            {
                test_ok = true;
                std::ostringstream os;
                os << "CORBA exception raised: " << e << std::ends;
                TEST_INFO(os.str());
            }
            catch (CORBA::Exception& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e << std::ends;
                TEST_INFO(os.str());
            }
            catch (...)
            {
                // test failed!
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);
            break;
            
          case 3:
          
            // add number of requested successfull tests    
            add_nbOfRequestedTestOK (5);
            
            TEST_INFO("Creating a new typed channel CHANNEL 4 undefined in the XML file, should raised Creation error");
            test_ok = false;
            try
            {
                channel = m_factory->create_event_channel(UNDEF_XML_CHANNEL_NAME);
            }
            catch (CdmwEvent::CreationError& e)
            {
                std::ostringstream os;
                os << e.reason.in() << std::ends;
                TEST_INFO(os.str());
                test_ok = true;
            }
            catch (CdmwLifeCycle::InvalidName& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::AlreadyExistant& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.name.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CORBA::Exception& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e << std::ends;
                TEST_INFO(os.str());
            }
            catch (...)
            {
                // test failed!
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);

            test_ok = false;
            TEST_INFO("Creating a new typed channel CHANNEL 1");
            try
            {
                channel = m_factory->create_event_channel(CHANNEL_1_NAME);
                test_ok = true;
            }
            catch (CdmwEvent::CreationError& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::AlreadyExistant& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.name.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::InvalidName& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CORBA::Exception& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e << std::ends;
                TEST_INFO(os.str());
            }
            catch (...)
            {
                // test failed!
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);
            
            
            TEST_INFO("Creating a new typed channel CHANNEL 2");
            test_ok = false;
            try
            {
                channel = m_factory->create_event_channel(CHANNEL_2_NAME);
                test_ok = true;
            }
            catch (CdmwEvent::CreationError& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::AlreadyExistant& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.name.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::InvalidName& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CORBA::Exception& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e << std::ends;
                TEST_INFO(os.str());
            }
            catch (...)
            {
                // test failed!
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);
            
            TEST_INFO("Creating a new typed channel CHANNEL 1 => should raise AlreadyExistant exception ");
            test_ok = false;
            try
            {
                channel = m_factory->create_event_channel(CHANNEL_1_NAME);
            }
            catch (CdmwEvent::CreationError& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::InvalidName& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::AlreadyExistant&)
            {
                test_ok = true;
            }
            catch (CORBA::Exception& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e << std::ends;
                TEST_INFO(os.str());
            }
            catch (...)
            {
                // test failed!
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);
            
            TEST_INFO("Creating a new typed channel CHANNEL 3 with no URL defined => should raise CreationError exception ");
            test_ok = false;
            try
            {
                channel = m_factory->create_event_channel(MIS_URL_CHANNEL_NAME);
            }
            catch (CdmwEvent::CreationError& e)
            {
                test_ok = true;
            }
            catch (CdmwLifeCycle::InvalidName& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.reason.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CdmwLifeCycle::AlreadyExistant& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e
                    << e.name.in() << std::ends;
                TEST_INFO(os.str());
            }
            catch (CORBA::Exception& e)
            {
                std::ostringstream os;
                os << "Unexpected CORBA exception raised: " << e << std::ends;
                TEST_INFO(os.str());
            }
            catch (...)
            {
                // test failed!
                TEST_INFO("Unexpected exception raised!");
            }
            TEST_CHECK(test_ok);
            

            break;

        default:
            TEST_INFO("**** NO MORE TEST AVAILABLE!");
            break;
        }
    } 
    catch (const CORBA::Exception & e) 
    {
        std::cerr << "ERROR: " << e << std::endl;
    }
#endif  // orbacus
}
        
}; // End namespace Event
}; // End namespace Cdmw


