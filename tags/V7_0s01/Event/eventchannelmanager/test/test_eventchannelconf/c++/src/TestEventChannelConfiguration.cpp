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


#include <sstream>

#include "test_eventchannelconf/TestEventChannelConfiguration.hpp"

namespace Cdmw
{
namespace EventAdmin
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestEventChannelConfiguration::TestEventChannelConfiguration(std::string XMLFileName, bool do_validate)
    throw()
    :Testable("Cdmw::EventAdmin::EventChannelConfiguration"),
      m_conf(NULL),
      m_xmlFile(XMLFileName),
      m_doValidate(do_validate)
{
    // does nothing
}


TestEventChannelConfiguration::~TestEventChannelConfiguration()
    throw()
{
    // does nothing
     if (m_conf)
         delete m_conf;
}


TestEventChannelConfiguration::TestEventChannelConfiguration(const TestEventChannelConfiguration& rhs)
    throw()
    :Testable("Cdmw::EventAdmin::EventChannelConfiguration")
{
    // does nothing
}
        
TestEventChannelConfiguration&
TestEventChannelConfiguration::operator=(const TestEventChannelConfiguration& rhs)
    throw()
{
    if (this != &rhs) {
        m_conf = rhs.m_conf;
    }
    return *this;
}

void TestEventChannelConfiguration::do_tests()        
{
     // set number of requested successfull tests
    set_nbOfRequestedTestOK (3);
    
    
    // This tests EventChannelConfiguration class
    //-------------------------------------------------
    TEST_INFO("Testing EventChannelConfiguration");


    bool test_ok=false;

    try 
    {
         TEST_INFO("Instantiate an EventChannelConfiguration object");
         
         try
         {
             m_conf = EventChannelConfiguration::readConfiguration(m_xmlFile, m_doValidate);

             if (m_conf)
                 test_ok = true;
         }
         catch (const ConfigurationException& e)
         {
             // test failed!
             TEST_INFO("ConfigurationException raised");
             TEST_INFO(e.what());
         }
         catch (const OutOfMemoryException& e)
         {
             // test failed!
             TEST_INFO("OutOfMemoryException raised");
         }
         catch (const InternalErrorException& e)
         {
             // test failed!
             TEST_INFO("InternalErrorException raised");
         }
         TEST_CHECK(test_ok);
         
         
         TEST_INFO("Get number of ChannelProfile");
         test_ok = false;
         size_t nbProfile = 0;
         try
         {
             nbProfile = m_conf->getNbEventChannelProfile();
             std::cout  << "nbProfile = " << nbProfile << std::endl;
             test_ok = true;
         }
         catch (...)
         {
             // test failed!
         }
         TEST_CHECK(test_ok);

         TEST_INFO("get each channel name and properties");
         test_ok = false;
         try
         {
             std::string name;
             size_t channel_i =0;
             ChannelProperties* p_properties = NULL;

             for (channel_i = 0; channel_i < nbProfile; channel_i++)
             {
                  name = m_conf->getNextEventChannelName(channel_i);
                  std::cout << "Channel " << channel_i << " = " <<name << std::endl;
                  TEST_INFO(name);

                  p_properties = m_conf->getEventChannelProfileValue(name);
                  if (p_properties)
                  {
                      size_t nbProperty = p_properties->size();
                      size_t prop_i;

                      for (prop_i=0; prop_i < nbProperty; prop_i++)
                      {
                          TEST_INFO((*p_properties)[prop_i].name);
                          TEST_INFO((*p_properties)[prop_i].value);
                      }
                  }
                  else
                  {
                     TEST_INFO("No Properties!");
                  }
             }
             test_ok = true;
         }
         catch (...)
         {
         }
         TEST_CHECK(test_ok);

    } 
    catch (...) 
    {
        std::cerr << "ERROR: Unexpected exception" << std::endl;
    }
}
        
}; // End namespace LifeCyle
}; // End namespace Cdmw

