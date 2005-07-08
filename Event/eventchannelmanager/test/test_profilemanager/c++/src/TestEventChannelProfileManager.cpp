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


#include <iostream>
#include <string>
#include <sstream>
#include "test_profilemanager/TestEventChannelProfileManager.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

namespace
{
     const char* CHANNEL_1_NAME = "CHANNEL 1";
     const char* CHANNEL_2_NAME = "CHANNEL/2";
     const char* CHANNEL_3_NAME = "CHANNEL 3";

     const char* TYPE_NAME = "TYPE";
     const char* UNTYPED_FLAG = "Untyped";
     const char* TYPED_FLAG = "Typed";

     const char* URL_NAME = "URL";
     const char* URL_UNTYPED = "corbaloc::localhost:2098/DefaultEventChannelFactory";
     const char* URL_TYPED = "corbaloc::localhost:2098/DefaultTypedEventChannelFactory";

     const int   NB_PROFILE = 3;
     const char* XML_CHANNEL_NAME[NB_PROFILE] = {"TestEventChannel1","TestEventChannel2","TestEventChannel3"};

     const size_t XML_CHANNEL_NB_PROPERTY[NB_PROFILE] = {2, 2, 1};

     const char* XML_CHANNEL_TYPE_FLAG[NB_PROFILE] = {"Typed", "Untyped", "Typed"};

     const char* XML_URL[NB_PROFILE] = {URL_TYPED, URL_UNTYPED, ""};
};

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


TestEventChannelProfileManager::TestEventChannelProfileManager(CdmwEventAdmin::EventChannelProfileManager_ptr manager)
    throw()
    :Testable("Cdmw::EventAdmin::EventChannelProfileManager"),
      m_profileManager(CdmwEventAdmin::EventChannelProfileManager::_duplicate(manager))
{
    // does nothing
}


TestEventChannelProfileManager::~TestEventChannelProfileManager()
    throw()
{
    // does nothing
}


TestEventChannelProfileManager::TestEventChannelProfileManager(const TestEventChannelProfileManager& rhs)
    throw()
    :Testable("Cdmw::EventAdmin::EventChannelProfileManager"),
     m_profileManager(CdmwEventAdmin::EventChannelProfileManager::_duplicate(rhs.m_profileManager.in()))
{
    // does nothing
}
        
TestEventChannelProfileManager&
TestEventChannelProfileManager::operator=(const TestEventChannelProfileManager& rhs)
    throw()
{
    if (this != &rhs) {
        m_profileManager = CdmwEventAdmin::EventChannelProfileManager::_duplicate(rhs.m_profileManager.in());
    }
    return *this;
}

void TestEventChannelProfileManager::do_tests()        
{
     // set number of requested successfull tests
    set_nbOfRequestedTestOK (12);
    
    // This tests TestEventChannelProfileManager class
    //-------------------------------------------------
    TEST_INFO("Testing TestEventChannelProfileManager");

    using namespace Cdmw::EventAdmin;

    bool test_ok = true;

    try 
    {
         TEST_INFO("Listing existing event channel profile");
         CdmwEventAdmin::EventChannelNameList_var  eventChannelNames;
         try
         {
              int i_channel;
              int nbNames = 0;
              
              eventChannelNames = m_profileManager->managed_event_channels();
              nbNames = eventChannelNames->length();

              if (nbNames == NB_PROFILE)  
              {
                  for (i_channel=0; i_channel<nbNames; i_channel++)
                  {
                       CORBA::String_var name = 
                             CORBA::string_dup((*eventChannelNames)[i_channel]);
                       
                        if (strcmp(name.in(), XML_CHANNEL_NAME[i_channel]) != 0)
                        {
                            // get out the loop, test failed!
                            std::ostringstream os;
                            os << "Profile[" << i_channel << "]: " << name.in()
                               << ", instead of " << XML_CHANNEL_NAME[i_channel];
                       
                            TEST_INFO(os.str());
                            test_ok = false;
                            break;
                        }
                    }
              }
              else
              {
                  std::ostringstream os;
                  os << "XML file contains " << nbNames << " channel profiles, waiting "
                     << NB_PROFILE;
                   
                  TEST_INFO(os);
                  test_ok = false;
              }
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());

             test_ok = false;
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");

             test_ok = false;
         }
         TEST_CHECK(test_ok);
         
         // Check now the EventChannelProfileManager correctly read the XML file
         TEST_INFO("Find profile defined in XML file and check it");
         test_ok = false;
         int channel_i = 0;
         for (channel_i=0; channel_i<NB_PROFILE; channel_i++)
         {
             try
             {
                  CdmwEventAdmin::EventChannelProfile_var pReturnProfile;
                 pReturnProfile = m_profileManager->find_profile(XML_CHANNEL_NAME[channel_i]);
                
                if (pReturnProfile->length() == XML_CHANNEL_NB_PROPERTY[channel_i])
                {
                    test_ok = true;

                    std::string name = (*pReturnProfile)[0].name.in();
                    if (name != TYPE_NAME)
                    {
                        std::ostringstream os;
                         os << "Profile for " << XML_CHANNEL_NAME[channel_i];
                        os << ": Prop1 Name is " << name << " instead of " << TYPE_NAME;
                        TEST_INFO(os.str());
                        
                        test_ok = false;
                        break;
                    }

                    std::string value = (*pReturnProfile)[0].value.in();
                    if (value != XML_CHANNEL_TYPE_FLAG[channel_i])
                    {
                        std::ostringstream os;
                        os << "Profile for " << XML_CHANNEL_NAME[channel_i];
                        os << ": Prop1 Value is " << value << " instead of " 
                             << XML_CHANNEL_TYPE_FLAG[channel_i];
                        TEST_INFO(os.str());
                        test_ok = false;
                        break;
                    }

                    if (XML_CHANNEL_NB_PROPERTY[channel_i] > 1)
                    {
                        name = (*pReturnProfile)[1].name.in();
                        if (name != URL_NAME)
                        {
                            std::ostringstream os;
                            os << "Profile for " << XML_CHANNEL_NAME[channel_i];
                            os << ": Prop2 Name is " << name << " instead of " << URL_NAME;
                            TEST_INFO(os.str());
                            test_ok = false;
                            break;
                        }

                       value = (*pReturnProfile)[1].value.in();
                       if (value != XML_URL[channel_i])
                          {
                            std::ostringstream os;
                            os << "Profile for " << XML_CHANNEL_NAME[channel_i];
                            os << ": Prop2 Value is " << value << " instead of " 
                                 << XML_URL[channel_i];
                            TEST_INFO(os.str());
                               test_ok = false;
                               break;
                           }
                    }
              }
            }
           catch(CORBA::Exception& e)
           {
                 // test failed! get out loop!
               std::ostringstream os;
               os << "CORBA exception: " << e;
               TEST_INFO(os.str());

               test_ok = false;
               break; 
           }
           catch(...)
           {
                 // test failed! get out loop!
               TEST_INFO("Unexpected exception raised!");
               test_ok = false;
               break; 
           }
         }  // end for loop
         TEST_CHECK(test_ok);


         TEST_INFO("Creating a new profile");
         test_ok = false;
         CdmwEventAdmin::EventChannelProfile  profile(5);
         profile.length(2);
         profile[0].name = TYPE_NAME;
         profile[0].value = TYPED_FLAG;
         profile[1].name = URL_NAME;
         profile[1].value = URL_TYPED;
             
         try
         {
             m_profileManager->create_profile(CHANNEL_1_NAME, profile);
             test_ok = true;
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");
         }
         TEST_CHECK(test_ok);
         
         
         TEST_INFO("Creating an existant profile, raising AlreadyExistant exception");
         test_ok = false;
         try
         {
             m_profileManager->create_profile(CHANNEL_1_NAME, profile);
         }
         catch(CdmwEventAdmin::AlreadyExistant& ae)
         {
             test_ok = true;
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");
         }
         TEST_CHECK(test_ok);

         TEST_INFO("Creating a profile with an invalid channel name, raise InvalidName exception");
         test_ok = false;
         try
         {
             m_profileManager->create_profile(CHANNEL_2_NAME, profile);
         }
         catch(CdmwEventAdmin::InvalidName&)
         {
             test_ok = true;
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");
         }
         TEST_CHECK(test_ok);

         TEST_INFO("Remove the created profile");
         test_ok = false;
         try
         {
              m_profileManager->remove_profile(CHANNEL_1_NAME);
              test_ok = true;
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");
         }
         TEST_CHECK(test_ok);

         TEST_INFO("Try to remove it again! Raise NotExist exception.");
         test_ok = false;
         try
         {
             m_profileManager->remove_profile(CHANNEL_1_NAME);

             // No exception raised, test failed
             TEST_INFO("Unexpected successfull remove operation");
         }
         catch (CdmwEventAdmin::NotExist& ne)
         {
             test_ok = true;
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");
         }
         TEST_CHECK(test_ok);

         TEST_INFO("Update inexistant profile!");
         test_ok = false;
         try
         {
             m_profileManager->update_profile(CHANNEL_1_NAME, profile);
         }
         catch (CdmwEventAdmin::NotExist& ne)
         {
             test_ok = true;
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");
         }
         TEST_CHECK(test_ok);

         TEST_INFO("Create this profile again!");
         test_ok = false;
         try
         {
             m_profileManager->create_profile(CHANNEL_1_NAME, profile);
             test_ok = true;
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");
         }
         TEST_CHECK(test_ok);

         TEST_INFO("Update this profile");
         test_ok = false;
         profile.length(1);
         profile[0].value = UNTYPED_FLAG;
         try
         {
             m_profileManager->update_profile(CHANNEL_1_NAME, profile);
             test_ok = true;
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");
         }
         TEST_CHECK(test_ok);
         
         TEST_INFO("Find a profile");
         test_ok = false;
         try
         {
              
             CdmwEventAdmin::EventChannelProfile_var pReturnProfile;
             pReturnProfile = m_profileManager->find_profile(CHANNEL_1_NAME);
              
             if (pReturnProfile->length() == profile.length())
             {
                size_t i = 0;
                  
                test_ok = true;
                for (i=0; i<profile.length(); i++)
                {
                      std::string name = (*pReturnProfile)[i].name.in();
                      std::string refName = profile[i].name.in();
                      if (name != refName)
                      {
                          test_ok = false;
                          break;
                      }
                      
                      std::string value = (*pReturnProfile)[i].value.in();
                      std::string refValue = profile[i].value.in();
                      if (value != refValue)
                      {
                          test_ok = false;
                          break;
                      }
                }
            }
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");
         }
         TEST_CHECK(test_ok);

         TEST_INFO("Find an inexistant profile");
         test_ok = false;
         try
         {
             m_profileManager->find_profile(CHANNEL_3_NAME);

             // no exception raised, error
             TEST_INFO("Unexpected profile found!");
         }
         catch (CdmwEventAdmin::NotExist& e)
         {
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
             test_ok = true;
         }
         catch(CORBA::Exception& e)
         {
             // test failed!
             std::ostringstream os;
             os << "CORBA exception: " << e;
             TEST_INFO(os.str());
         }
         catch(...)
         {
             // test failed!
             TEST_INFO("Unexpected exception raised!");
         }
         TEST_CHECK(test_ok);

     } 
     catch (const CORBA::Exception & e) 
     {
          std::ostringstream os;
          os << "CORBA exception: " << e;
          TEST_INFO(os.str());
     }
     catch (...)
     {
          TEST_INFO("Unexpected exception raised!");
     }
}
        
}; // End namespace LifeCyle
}; // End namespace Cdmw


