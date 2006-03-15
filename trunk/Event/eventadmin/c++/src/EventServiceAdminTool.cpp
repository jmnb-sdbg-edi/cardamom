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


#include "eventadmin/EventServiceAdminTool.hpp"

#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <algorithm>

#include "Foundation/common/System.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "Foundation/orbsupport/CosNaming.stub.hpp"

#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingUtil.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"

#include "Event/eventinterface/EventChannel.hpp"
#include "Event/eventinterface/TypedEventChannel.hpp"


#define PRINT if (m_verbose) os


using namespace Cdmw;
using namespace Cdmw::OsSupport;
using namespace Cdmw::OrbSupport;

namespace
{
    const int MAX_RETRY=100;
    const int NB_PROPERTIES = 5;
    const char* INFO_MESSAGE = "Type h or help for a list of available commands.";
    const char* PROMPT_MESSAGE = "\nEVENT SERVICE ADMIN>";
    const char* UNKNOWN_COMMAND_MESSAGE = "Bad command!>";

    const CORBA::Long EXIT_ORDERED(-10);
    const CORBA::Long OP_SUCCESS = 0;
    const CORBA::Long OP_FAILURE = -1;

    const int MAX_PROPERTY=50;
    const std::string URL_PROPERTY_TAG = "URL";
    const std::string IDL_PROPERTY_TAG = "IDL";

    bool not_space(char c)
    { return !isspace(c); }

    bool isequalchar(char c)
    { return c=='=';}

    bool not_equalchar(char c)
    { return !isequalchar(c);}

}; // End anonymous namespace

namespace Cdmw
{
namespace EventAdmin
{

//----------------------------------------------------------------------------
EventServiceAdminTool::EventServiceAdminTool(CORBA::ORB_ptr orb, 
                                             CosNaming::NamingContext_ptr nc,
                                                    const std::string& commandFile)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_namingcontext(CosNaming::NamingContext::_duplicate(nc)),
       m_commandFile(commandFile),
      m_verbose(false)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    CDMW_ASSERT(!CORBA::is_nil(nc));

    m_factory_context = Common::Locations::CDMW_EVENT_SERVICE_NAME_DOMAIN;
    m_factory_context += "/";
    m_factory_context += "FACTORIES";

    m_eventchannel_context = Common::Locations::CDMW_EVENT_SERVICE_NAME_DOMAIN;
    m_eventchannel_context += "/";
    m_eventchannel_context += "EVENT_CHANNELS";

    m_typedeventchannel_context = Common::Locations::CDMW_EVENT_SERVICE_NAME_DOMAIN;
    m_typedeventchannel_context += "/";
    m_typedeventchannel_context += "EVENT_CHANNELS";

} // End constructor EventServiceAdminTool

    using namespace Cdmw::Common;

//----------------------------------------------------------------------------
void
EventServiceAdminTool::init()
    throw (InitException)
{
    CommonSvcs::Naming::NamingInterface ni(m_namingcontext.in());
    std::string name;

    std::cout << "cdmw_event_admin intialisation ..." << std::endl;
    
    try 
    {   
        int nb_retry = MAX_RETRY;

        // wait for cdmw_event_channel_manager to be launched and initialised
        while (nb_retry)
        {
            try
            {
                name = m_factory_context + "/DefaultEventChannelFactory";
                
                //Get a reference on the generic EventChannelFactory
                CORBA::Object_var obj = ni.resolve(name);
                m_factory = CdmwEvent::EventChannelFactory::_narrow(obj.in());
                
                if (CORBA::is_nil(m_factory.in())) 
                {
                    throw InitException("DefaultEventChannelFactory is a nil object reference.");
                }
                
                name = m_factory_context + "/DefaultTypedEventChannelFactory";
                //Get a reference on the generic EventChannelFactory
                obj = ni.resolve(name);
                m_typedFactory = CdmwEvent::TypedEventChannelFactory::_narrow(obj.in());
                
                if (CORBA::is_nil(m_typedFactory.in())) 
                {
                    throw InitException("DefaultTypedEventChannelFactory is a nil object reference.");
                }
                
                //Get a reference on the EventChannelProfileManager
                name = Common::Locations::CDMW_EVENT_SERVICE_NAME_DOMAIN;
                name += "/";
                name += CdmwEventAdmin::EventChannelProfileManagerName;
                obj = ni.resolve(name);
                m_profileManagerRef = CdmwEventAdmin::EventChannelProfileManager::_narrow(obj.in());
                
                if (CORBA::is_nil(m_profileManagerRef.in())) 
                {
                    throw InitException("EventChannelProfileManager is a nil object reference.");
                }

                // No NotFound exception raised, get out from loop
                break;
            }
            catch (CosNaming::NamingContext::NotFound&)
            {
              OsSupport::OS::sleep(300);
            }
        } // end while loop
    }
    catch (CdmwNamingAndRepository::NoNameDomain&)
    {
        throw InitException("Cdmw event service domain not specified.");
    }
    catch (CdmwNamingAndRepository::InvalidName&)
    {
        throw InitException("Cdmw event service domain invalid.");
    }
    catch (CosNaming::NamingContext::NotFound&)
    {
        throw InitException("cdmw_event_channel_manager not found.");
    }
    catch (CosNaming::NamingContext::CannotProceed&)
    {
        throw InitException("Can not proceed resolve.");
    }
    catch (CosNaming::NamingContext::InvalidName&)
    {       
        throw InitException("Invalid name for factory or EventChannelProfileManager.");
    }

    std::cout << "cdmw_event_admin intialisation done" << std::endl;

}
    

//----------------------------------------------------------------------------
EventServiceAdminTool::~EventServiceAdminTool() throw()
{
}

//----------------------------------------------------------------------------
const CORBA::ULong EventServiceAdminTool::nb_commands = 28;

// WARNING:
// EventServiceAdminTool::commands should be sorted alphabetically (by operation name)
const EventServiceAdminTool::command_def EventServiceAdminTool::commands[] = 
{ 
    { "cg"                           , &EventServiceAdminTool::create_event_channel         },
    { "connect"                      , &EventServiceAdminTool::connect_channels             },
    { "create_channel"               , &EventServiceAdminTool::create_event_channel         },
    { "create_profile"               , &EventServiceAdminTool::create_profile               },  
    { "create_typed_channel"         , &EventServiceAdminTool::create_typed_event_channel   },
    { "ct"                           , &EventServiceAdminTool::create_typed_event_channel   },
    { "destroy"                      , &EventServiceAdminTool::destroy_event_channel        },
    { "display_channel_profile"      , &EventServiceAdminTool::display_channel_profile      },
    { "dp"                           , &EventServiceAdminTool::display_channel_profile      },
    { "exit"                         , &EventServiceAdminTool::exit                         },
    { "h"                            , &EventServiceAdminTool::print_help                   },
    { "help"                         , &EventServiceAdminTool::print_help                   },
    { "lc"                           , &EventServiceAdminTool::list_created_channel         },
    { "list_created_channel"         , &EventServiceAdminTool::list_created_channel         },
    { "list_managed_channel"         , &EventServiceAdminTool::list_managed_channel         },
    { "list_managed_channel_profile" , &EventServiceAdminTool::list_managed_channel_profile },
    { "lm"                           , &EventServiceAdminTool::list_managed_channel         },
    { "lp"                           , &EventServiceAdminTool::list_managed_channel_profile },
    { "remove_profile"               , &EventServiceAdminTool::remove_profile               },  
    { "rm"                           , &EventServiceAdminTool::remove_profile               },  
    { "s"                            , &EventServiceAdminTool::save_profile                 },  
   { "save_profile"                 , &EventServiceAdminTool::save_profile                 },  
   { "stop_ec_manager"              , &EventServiceAdminTool::stop_ec_manager              },
   { "u"                            , &EventServiceAdminTool::update_profile               },  
    { "update_profile"               , &EventServiceAdminTool::update_profile               },  
    { "v"                            , &EventServiceAdminTool::toggle_verbose               },
    { "verbose"                      , &EventServiceAdminTool::toggle_verbose               },
    { "x"                            , &EventServiceAdminTool::exit                         }
};


    
//----------------------------------------------------------------------------
CORBA::Long EventServiceAdminTool::runOperation(const std::string &op,
                                                const std::string &arg, 
                                                std::ostream &os) 
{
    CORBA::Long l = 0;
    CORBA::Long r = (CORBA::Long)nb_commands;
    CORBA::Long m;
    CORBA::Long res;
    
    // Do nothing if command begin with character '#' (commented line in cmd file)
    if (op[0] == '#') 
        return OP_SUCCESS;

    while(l < r)
    {
      m = (l + r) / 2;
      res = strcmp(commands[m].name, op.c_str());
      if(res == 0)
        return (this->*commands[m].op)(arg,os);
      
      if(res > 0)
        r = m;
      else
        l = m + 1;
    }
    
    if(op != "") unknown_command(os);
    
    return OP_FAILURE;
}

//----------------------------------------------------------------------------
void EventServiceAdminTool::print_info(std::ostream& os)
{
    os << "\n\n"
       << "\t\t--------------------------------------------------\n"
       << "\t\t Cdmw Event service administration console       \n"
       << "\t\t--------------------------------------------------\n"
       << "\n\n"
       << INFO_MESSAGE
       << "\nInfo: (verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
}

//----------------------------------------------------------------------------
void EventServiceAdminTool::print_prompt(std::ostream& os)
{
    os << PROMPT_MESSAGE << std::flush;
}

//----------------------------------------------------------------------------
void EventServiceAdminTool::unknown_command( std::ostream & os)
{
    os << UNKNOWN_COMMAND_MESSAGE << std::endl;
}

//----------------------------------------------------------------------------
void EventServiceAdminTool::run(std::istream& is,std::ostream& os,bool commandFile)
{
   print_info(os);
    print_prompt(os);
    
    bool loop_flag = true;

    // get input stream
    std::istream *p_is = &is;

    while (loop_flag)
    {
      std::string s;
      while(std::getline(*p_is,s)) 
      {  
        // ===============
        // Extract command        
        // ===============
        // skip leading whitespaces
        std::string::const_iterator i = s.begin();
        std::string::const_iterator end = s.end();
        i = std::find_if(i,end,not_space);

        // find end of command
        std::string::const_iterator j = std::find_if(i,end,isspace);

        // copy the characters in [i,j]
        std::string command;
        if (i != end)
            command = std::string(i,j);

        // =================
        // Extract arguments
        // =================
        // skip leading whitespaces
        i = std::find_if(j,end,not_space);
        std::string arg;

        // copy arguments
        if (i != end)
            arg = std::string(i,end);
        
        PRINT << "@cmd = <" << command << "> @args = <" << arg << ">" << std::endl;
        if (runOperation(command,arg,os) == EXIT_ORDERED) 
        {
          loop_flag = false;
          break; 
        }

        print_prompt(os); 
      } 

      // if end of command file switch on cin
      if (commandFile)
      {
        p_is = &std::cin;
      }
    } 
}  // End run()

//----------------------------------------------------------------------------
CORBA::Long EventServiceAdminTool::exit(const std::string & arg, std::ostream & os)
{    
    return EXIT_ORDERED;
}

//----------------------------------------------------------------------------
CORBA::Long EventServiceAdminTool::toggle_verbose(const std::string & arg, std::ostream & os)
{    
    m_verbose = !m_verbose;
    os << "(verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
    return OP_SUCCESS;
}

//----------------------------------------------------------------------------
// Available commands:
//
CORBA::Long EventServiceAdminTool::print_help(const std::string & arg, std::ostream & os)
{
     os << " +===================================+============================================+" << "\n"
        << " | COMMAND                           | DESCRIPTION                                |" << "\n"
        << " +===================================+============================================+" << "\n"
        << " | x | exit                          | exit ClientThread mode                     |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n"
        << " | h | help                          | print available commands                   |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n" 
        << " | v | verbose                       | toggle verbose mode on/off                 |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n"
            << " | cg | create_channel <channel_name>| create a new generic event channel         |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n"
            << " | ct | create_typed_channel         | create a new typed event channel           |" << "\n"
          << " |      <channel_name>               |                                            |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n"
            << " | destroy <channel_name>            | destroy an event channel                   |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n"
          << " | connect                           | connect channel_1 to channel_2.            |" << "\n"
          << " |      <channel_name1>              | channel_1 becomes the consumer of channel_2|" << "\n"
          << " |      <channel_name2>              | interface_ref is used when connecting two  |" << "\n"
          << " |      [<interface_ref>]            | typed event channel                        |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
          << " | lm | list_managed_channel         | list managed channel                       |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
          << " | lp | list_managed_channel_profile | list managed channel and their properties  |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
          << " | lc | list_created_channel         | list already created channel               |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
          << " | dp | display_channel_profile      | display specified event channel profile    |" << "\n"
          << " |      <channel_name>               |                                            |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
            << " | create_profile                    | create a profile for a channel             |" << "\n"
          << " |      <channel_name>               |                                            |" << "\n"
          << " |      <factory_url>                | factory_url is a mandatory property.       |" << "\n"
          << " |      [<IDL>]                      | IDL is used only for typed event channel.  |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
            << " | rm | remove_profile               | remove a profile for a channel             |" << "\n"
          << " |      <channel_name>               |                                            |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
            << " | s | save_profile                  | save all channel profile in a xml file     |" << "\n"
          << " |      [<xml_file_name>]            | if no file name is specified, profiles are |" << "\n"
          << " |                                   | saved in the current loaded xml file       |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
            << " | add_property                      | add a property into a profile for a channel|" << "\n"
          << " |      <channel_name>               |                                            |" << "\n"
          << " |      <factory_url>                |                                            |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
            << " | u | update_profile                | update a profile for a channel             |" << "\n"
          << " |      <channel_name>               |                                            |" << "\n"
          << " |      <factory_url>                | factory_url is a mandatory property.       |" << "\n"
          << " |      [<IDL>]                      | IDL is used only for typed event channel.  |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
          << " | stop_ec_manager                   | stop cdmw_event_channel_manager           |" << "\n"
          << " +-----------------------------------+--------------------------------------------+" << "\n"
        << std::endl;

     return OP_SUCCESS;
}  // End print_help()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::create_event_channel(const std::string &arg, std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;
    const char *channel_name = arg.c_str();
    
    // if undefined context name set error
    if (*channel_name == '\0')                
    {
        os << "channel name is missing" << std::endl;
    }
    else
    {
        try
        {
            m_factory->create_event_channel(channel_name);
        }
        catch (const CdmwLifeCycle::AlreadyExistant& )
        {
            os << "channel already exist!" << std::endl;
            return result;
        }
        catch (const CdmwLifeCycle::InvalidName& )
        {
            os << "channel name has an invalid format!" << std::endl;
            return result;
        }
        catch (const CdmwEvent::CreationError& e)
        {
            os << "Creation error: " << e.reason.in() << std::endl;
            return result;
        }
        catch (const CORBA::SystemException& e)
        {
            os << "cdmw_event_admin: " << e << std::endl;
            return result;
        }
        
        os << "generic event channel " << channel_name << " created!" << std::endl;
        
        result = OP_SUCCESS;
    }
    
    return result;
}

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::create_typed_event_channel(const std::string &arg, std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;
    std::string channel_name;
    std::string interface_ref;
    
    // extract channel name
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();
    
    // find channel 1 name
    std::string::const_iterator j = std::find_if(i,end,isspace);
    
    // copy the characters in [i,j)
    if (i != j)
        channel_name = std::string(i,j);
    
    if (channel_name == "")
    {
        os << "channel name is missing!" << std::endl;
        return OP_FAILURE;
    }
    
    try
    {
        m_typedFactory->create_event_channel(channel_name.c_str());
    }
    catch (const CdmwLifeCycle::AlreadyExistant& )
    {
        os << "channel already exist!" << std::endl;
        return result;
    }
    catch (const CdmwLifeCycle::InvalidName& )
    {
        os << "channel name has an invalid format!" << std::endl;
        return result;
    }
    catch (const CdmwEvent::CreationError& e)
    {
        os << "Creation error: " << e.reason.in() << std::endl;
        return result;
    }
    catch (const CORBA::SystemException& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return result;
    }
    
    os << "typed event channel " << channel_name << " created!" << std::endl;
    
     result = OP_SUCCESS;  
    return result;
}

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::destroy_event_channel(const std::string &arg, std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;
    const char* channel_name = arg.c_str();
    
    // if undefined channel name set error
    if (*channel_name == '\0')                
    {
        os << "channel name is missing" << std::endl;
    }
    else
    {
        std::string whole_channel_name = m_eventchannel_context + "/" + arg;

        try
        {
            // try with an un-typed event channel
            Event::EventChannel eventchannel(whole_channel_name.c_str(), m_namingcontext.in());
            eventchannel.destroy();
            os << "event channel " << channel_name << " destroyed!" << std::endl;
            result = OP_SUCCESS;
            
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            // Channel can be a typed one
            try
            {
                Event::TypedEventChannel typedeventchannel(whole_channel_name.c_str(), m_namingcontext.in());
                typedeventchannel.destroy();
                os << "typed event channel " << channel_name << " destroyed!" << std::endl;
                result = OP_SUCCESS;
            }
            catch (const CORBA::BAD_PARAM& e)
            {
                os << "Invalid channel name: " << e << std::endl;
            }
            catch (const CORBA::INTERNAL& e)
            {
                std::cerr << "Internal Error Exception" << std::endl;
            }
            catch (const CORBA::SystemException& e)
            {
                std::cerr << e << std::endl;
            }
        }
        catch (const CORBA::INTERNAL& e)
        {
            std::cerr << "Internal Error Exception" << std::endl;
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << e << std::endl;
        }
    }
    
    return result;
}

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::connect_channels(const std::string &arg, std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;
    std::string channel_name_1;
    std::string channel_name_2;
    std::string interface_ref;
  
    // extract channel 1 name
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();

    // find channel 1 name
    std::string::const_iterator j = std::find_if(i,end,isspace);

    // copy the characters in [i,j)
    if (i != j)
        channel_name_1 = std::string(i,j);

    if (channel_name_1 == "")
    {
        os << "channel name 1 is missing!" << std::endl;
        return OP_FAILURE;
    }

    if (j != end)
    {
        // extract channel 2 name 
        // skip leading whitespaces
        i = std::find_if(j,end,not_space);

        // find end of name
        j = std::find_if(i,end,isspace);

        // copy the characters in [i,j)
        if (i != j)
            channel_name_2 = std::string(i,j);
    }

    if (channel_name_2 == "")
    {
        os << "channel name 2 is missing!" << std::endl;
        return OP_FAILURE;
    }

    if (j != end)
    {
        // extract interface_ref name 
        // skip leading whitespaces
        i = std::find_if(j,end,not_space);

        // find end of name
        j = std::find_if(i,end,isspace);

        // copy the characters in [i,j)
        if (i != j)
            interface_ref = std::string(i,j);
    }

    if (interface_ref != "")
    {
        result = connect_typed_channels(channel_name_1, 
                                        channel_name_2, 
                                        interface_ref, 
                                        os);
    }
    else
    {
        result = connect_generic_channels(channel_name_1, 
                                          channel_name_2, 
                                          os);
    }

    return result;
}

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::connect_generic_channels(const std::string& channel_name_1, 
                                                const std::string& channel_name_2, 
                                                std::ostream& os)
{
    CORBA::Long result = OP_FAILURE;
    Event::EventChannel* eventchannel2;
    
    std::string complete_channel_name_1 = m_eventchannel_context + "/" + channel_name_1;
    std::string complete_channel_name_2 = m_eventchannel_context + "/" + channel_name_2;

    try
    {
        eventchannel2 = new Event::EventChannel(complete_channel_name_2.c_str(),
                                                m_namingcontext.in());
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << e << std::endl;
        return OP_FAILURE;
    }

    try
    {
        eventchannel2->connect_to_channel(complete_channel_name_1.c_str());
    }
    catch (const CosTypedEventChannelAdmin::InterfaceNotSupported& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return OP_FAILURE;
    }
    catch (const CosTypedEventChannelAdmin::NoSuchImplementation& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return OP_FAILURE;
    }
    catch (const CosEventChannelAdmin::AlreadyConnected& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return OP_FAILURE;
    }
    catch (const CORBA::SystemException& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return OP_FAILURE;
    }

    os << "generic event channel " << channel_name_1 << " connected to " 
       << channel_name_2 << std::endl;
    result = OP_SUCCESS;

    return result;

} // End connect_generic_channels()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::connect_typed_channels(const std::string& channel_name_1, 
                                              const std::string& channel_name_2,
                                              const std::string& interface_ref,
                                              std::ostream& os)
{
    CORBA::Long result = OP_FAILURE;
    Cdmw::Event::TypedEventChannel* typedeventchannel2;

    std::string complete_channel_name_1 = m_typedeventchannel_context + "/" + channel_name_1;
    std::string complete_channel_name_2 = m_typedeventchannel_context + "/" + channel_name_2;
    
    try
    {
        typedeventchannel2 = new Event::TypedEventChannel(complete_channel_name_2.c_str(),
                                                m_namingcontext.in());
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << e << std::endl;
        return OP_FAILURE;
    }

    try
    {
        typedeventchannel2->connect_to_channel(interface_ref.c_str(),
                                               complete_channel_name_1.c_str());
    }
    catch (const CosTypedEventChannelAdmin::InterfaceNotSupported& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return OP_FAILURE;
    }
    catch (const CosTypedEventChannelAdmin::NoSuchImplementation& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return OP_FAILURE;
    }
    catch (const CosEventChannelAdmin::AlreadyConnected& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return OP_FAILURE;
    }
    catch (const CORBA::SystemException& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return OP_FAILURE;
    }
    
    os << "typed event channel " << channel_name_1 << " connected to " 
       << channel_name_2 << std::endl;
    
    result = OP_SUCCESS;
    return result;

} // End connect_typed_channels()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::list_managed_channel(const std::string &arg, 
                                            std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;
    CdmwEventAdmin::EventChannelNameList*  managed_channel_list;
 
    try
    {
        managed_channel_list = m_profileManagerRef->managed_event_channels();
    }
    catch (const CORBA::SystemException& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return OP_FAILURE;
    }
    catch (...)
    {
        os << "cdmw_event_admin: unexpected exception raised!" << std::endl;
        return OP_FAILURE;
    }

    os << "list of managed event channel:" << std::endl;

    for (unsigned long i=0; i < managed_channel_list->length(); i++)
    {
         os << "\t" << ((*managed_channel_list)[i]).in() << std::endl;
    }

    result = OP_SUCCESS;
    return result;
} // End list_managed_channel

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::list_managed_channel_profile(const std::string &arg, 
                                                    std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;
    CdmwEventAdmin::EventChannelNameList*  managed_channel_list;
 
    try
    {
        managed_channel_list = m_profileManagerRef->managed_event_channels();
    }
    catch (const CORBA::SystemException& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        return OP_FAILURE;
    }
    catch (...)
    {
        os << "cdmw_event_admin: unexpected exception raised!" << std::endl;
        return OP_FAILURE;
    }

    os << "list of managed event channel:" << std::endl;

    for (unsigned long i=0; i < managed_channel_list->length(); i++)
    {
        result = display_channel_profile(std::string((*managed_channel_list)[i]), os);
         if (result == OP_FAILURE)
             break;
    }

    return result;
} // End list_managed_channel_profile()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::list_created_channel(const std::string &arg, 
                                            std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;
    CosNaming::BindingIterator_var it;          // Iterator reference
    CosNaming::BindingList_var     bl;          // Binding list
    std::string ec_context[2] = {m_eventchannel_context, m_typedeventchannel_context};
    int nb_context = 2;
    CommonSvcs::Naming::NamingInterface ni(m_namingcontext.in());
    CORBA::Object_var obj;

    // List event channels

    if (m_eventchannel_context == m_typedeventchannel_context)
        nb_context = 1;

    for (int i_ctx=0; i_ctx<nb_context; i_ctx++)
    {
        // Get NamingInterface object 
        obj = ni.resolve(ec_context[i_ctx].c_str());    
        CosNaming::NamingContext_var ecNContext = CosNaming::NamingContext::_narrow(obj.in());
        
        if (CORBA::is_nil(ecNContext.in())) 
        {
            os << "cdmw_event_admin: error when getting event channel naming context!" << std::endl;
            break;
        }

        CommonSvcs::Naming::NamingInterface ec_ni(ecNContext.in());

        try 
        {       
            int nb_createdChannel = 0;
            // Get first chunk
            ec_ni.list(CommonSvcs::Naming::NamingInterface::LIST_CHUNK_SIZE,bl.out(),it.out()); 
            
            do
            {
                for (CORBA::ULong i_bl=0; i_bl<bl->length(); i_bl++)
                {
                    if (bl[i_bl].binding_type == CosNaming::nobject)
                    {
                        nb_createdChannel++;

                        if (nb_createdChannel == 1)
                        {
                            os << "list of created event channel:" << std::endl;
                        }

                        os << "\t" << (bl[i_bl].binding_name[0].id).in();
                        
                        if ( bl[i_bl].binding_name[0].id[0] == '\0'
                            || bl[i_bl].binding_name[0].kind[0] != '\0')
                        {
                            os << "." << (bl[i_bl].binding_name[0].kind).in();
                        }
                        os << std::endl;
                    }
                }
                
                if (CORBA::is_nil(it.in())) 
                    break;
            } 
            while (it->next_n(CommonSvcs::Naming::NamingInterface::LIST_CHUNK_SIZE, bl));

            if (nb_createdChannel == 0)
            {
                os << "no channel created!" << std::endl;
            }
        } 
        catch (const CORBA::SystemException &e) 
        {
        os << "cdmw_event_admin: " << e << std::endl;
        }
    }

    return result;
} // End list_created_channel()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::display_channel_profile(const std::string &arg, 
                                               std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;
    std::string channel_name;
  
    // extract channel 1 name
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();

    // find channel 1 name
    std::string::const_iterator j = std::find_if(i,end,isspace);

    // copy the characters in [i,j)
    if (i != j)
        channel_name = std::string(i,j);

    if (channel_name == "")
    {
        os << "channel name is missing!" << std::endl;
        return OP_FAILURE;
    }

    try
    {
         CdmwEventAdmin::EventChannelProfile* profile =
             m_profileManagerRef->find_profile(channel_name.c_str());
         os << channel_name << ": "<< std::endl;
         for (unsigned long k=0; k<profile->length(); k++)
         {
             os << "\t" << (*profile)[k].name.in() << "=" << (*profile)[k].value.in() << std::endl;
         }
             
         result = OP_SUCCESS;
    }
    catch (const CdmwEventAdmin::NotExist& )
    {
        os << "no profile exist for this channel: " << channel_name << std::endl;
        result = OP_FAILURE;
    }
    catch (const CORBA::SystemException& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        result = OP_FAILURE;
    }

    return result;
} // End display_channel_profile()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::create_profile(const std::string &arg, 
                                      std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;

    std::string channel_name;
    CdmwEventAdmin::EventChannelProfile  profile(NB_PROPERTIES);
  
    // extract channel name and profile
    result = extract_channel_and_profile(arg, channel_name, profile, os);

    if (result == OP_SUCCESS)
    {
        try
        {
            m_profileManagerRef->create_profile(channel_name.c_str(), profile);
            os << "profile created for channel: " 
               << channel_name << std::endl;
        }
        catch (const CdmwEventAdmin::AlreadyExistant& )
        {
            os << "channel already exist!" << std::endl;
            result = OP_FAILURE;
        }
        catch (const CdmwEventAdmin::InvalidName& )
        {
            os << "channel name has an invalid format!" << std::endl;
            result = OP_FAILURE;
        }
        catch (const CORBA::SystemException& e)
        {
        os << "cdmw_event_admin: " << e << std::endl;
            result = OP_FAILURE;
        }
    }

    return result;
} // End create_profile()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::remove_profile(const std::string &arg, 
                                      std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;
    std::string channel_name;
  
    // extract channel 1 name
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();

    // find channel 1 name
    std::string::const_iterator j = std::find_if(i,end,isspace);

    // copy the characters in [i,j)
    if (i != j)
        channel_name = std::string(i,j);

    if (channel_name == "")
    {
        os << "channel name is missing!" << std::endl;
        return OP_FAILURE;
    }

    try
    {
         m_profileManagerRef->remove_profile(channel_name.c_str());
         os << "profile removed from configuration for channel: " 
            << channel_name << std::endl;
             
         result = OP_SUCCESS;
    }
    catch (const CdmwEventAdmin::NotExist& )
    {
        os << "no profile exist for this channel!" << std::endl;
        result = OP_FAILURE;
    }
    catch (const CORBA::SystemException& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        result = OP_FAILURE;
    }

    return result;
} // End  remove_profile()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::save_profile(const std::string &arg, 
                                    std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;

    os << "operation not implemented!" << std::endl;

    result = OP_SUCCESS;
    return result;
} // End save_profile()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::update_profile(const std::string &arg, 
                                      std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;

    std::string channel_name;
    CdmwEventAdmin::EventChannelProfile  profile(NB_PROPERTIES);
  
    // extract channel name and profile
    result = extract_channel_and_profile(arg, channel_name, profile, os);

    if (result == OP_SUCCESS)
    {
        try
        {
            m_profileManagerRef->update_profile(channel_name.c_str(), profile);
        }
        catch (const CdmwEventAdmin::NotExist& )
        {
            os << "channel doesn't exist! Create it!" << std::endl;
            result = OP_FAILURE;
        }
        catch (const CORBA::SystemException& e)
        {
            os << "cdmw_event_admin: " << e << std::endl;
            result = OP_FAILURE;
        }
    }

    result = OP_SUCCESS;
    return result;
} // End update_profile()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::extract_channel_and_profile(const std::string &arg,
                                                   std::string &channel_name, 
                            CdmwEventAdmin::EventChannelProfile& profile,
                            std::ostream &os)
{
    std::string property_tag[MAX_PROPERTY];
    std::string url;             // used to save url property
    std::string property[MAX_PROPERTY];
    int nb_property = 0;         // number of property defined in profile
    int property_index = 0;      // index of the current property
    bool end_extraction = false;
  
    // extract channel 1 name
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();

    // find channel 1 name
    std::string::const_iterator j = std::find_if(i,end,isspace);

    // copy the characters in [i,j)
    if (i != j)
        channel_name = std::string(i,j);

    if (channel_name == "")
    {
        os << "channel name is missing!" << std::endl;
        return OP_FAILURE;
    }

    while ((!end_extraction) && (property_index < MAX_PROPERTY))
    {
        if (j != end)
        {
            // extract URL tag
            // skip leading whitespaces
            i = std::find_if(j,end,not_space);
            
            // find end of name
            j = std::find_if(i,end,isequalchar);
            
            // copy the characters in [i,j)
            if (i != j)
                property_tag[property_index] = std::string(i,j);
        }

        if (property_tag[property_index] == "")
        {
            // nor more property, check mandatory field is defined!
            if (url == "") 
            { 
                os << "URL is missing!" << std::endl;
                return OP_FAILURE;
            }           
            end_extraction = true;
        }
        else
        {
            if ((property_tag[property_index] != URL_PROPERTY_TAG) 
                && (property_tag[property_index] != IDL_PROPERTY_TAG))
            {
                os << "invalid profile description!" << std::endl;
                return OP_FAILURE;
            }

            if (j != end)
            {
                // extract property
                // skip leading whitespaces
                i = std::find_if(j,end,not_equalchar);
                
                // find end of name
                j = std::find_if(i,end,isspace);
                
                // copy the characters in [i,j)
                if (i != j)
                    property[property_index] = std::string(i,j);
            }
            
            if (property[property_index] == "")
            {
                os << "invalid profile description!" << std::endl;
                return OP_FAILURE;
            }
            
            if (property_tag[property_index] == URL_PROPERTY_TAG)
            {
                url = property[property_index];
            }

            nb_property++;
            property_index++;
        }
    }

    profile.length(nb_property);

    for (int i=0; i<nb_property; i++)
    {
        profile[i].name = property_tag[i].c_str();
        profile[i].value = property[i].c_str();
    }

    return OP_SUCCESS;

} // extract_channel_and_profile()

//----------------------------------------------------------------------------
CORBA::Long 
EventServiceAdminTool::stop_ec_manager(const std::string &arg, 
                                       std::ostream &os)
{
    CORBA::Long result = OP_FAILURE;


    try
    {
        m_profileManagerRef->destroy();

        os << "cdmw_event_channel_manager stopped!" << std::endl;
        result = OP_SUCCESS;
    }
    catch (const CORBA::SystemException& e)
    {
        os << "cdmw_event_admin: " << e << std::endl;
        result = OP_FAILURE;
    }

    return result;
} // End update_profile()


}  // End namespace EventAdmin
}  // End namespace Cdmw


