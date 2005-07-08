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


#ifndef INCL_EVENT_SERVICE_ADMIN_TOOL_HPP
#define INCL_EVENT_SERVICE_ADMIN_TOOL_HPP


#include "Foundation/orbsupport/CORBA.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Event/idllib/CdmwEventAdmin.stub.hpp"
#include "Event/idllib/CdmwEvent.stub.hpp"
#include "Foundation/common/Exception.hpp"

#include <iostream>
#include <string>

namespace Cdmw
{
namespace EventAdmin
{



class InitException  : public Exception 
{    
    public:
        InitException(const std::string& reason) 
            : Exception( Exception::SAFE, "Init exception: " + reason )
        {
        }

    protected:

    private:

};
    
//--------------------------------------------------------------------------------------//
class EventServiceAdminTool
{
  public:

    EventServiceAdminTool(CORBA::ORB_ptr, CosNaming::NamingContext_ptr nc, const std::string& commandFile);
    
    ~EventServiceAdminTool() throw();

    void run(std::istream&,std::ostream&,bool commandFile);

    void init() throw (InitException);
    
  protected:
    
    // Available commands
    CORBA::Long print_help (const std::string &, std::ostream &);
    CORBA::Long exit (const std::string &, std::ostream &);
    CORBA::Long toggle_verbose(const std::string &, std::ostream &);
    
    CORBA::Long create_event_channel(const std::string &, std::ostream &);
    CORBA::Long create_typed_event_channel(const std::string &, std::ostream &);
    CORBA::Long destroy_event_channel(const std::string &, std::ostream &);
    CORBA::Long connect_channels(const std::string &, std::ostream &);
    CORBA::Long list_managed_channel(const std::string &, std::ostream &);
    CORBA::Long list_managed_channel_profile(const std::string &, std::ostream &);
    CORBA::Long list_created_channel(const std::string &, std::ostream &);
    CORBA::Long display_channel_profile(const std::string &, std::ostream &);
    CORBA::Long create_profile(const std::string &, std::ostream &);
    CORBA::Long remove_profile(const std::string &, std::ostream &);
    CORBA::Long save_profile(const std::string &, std::ostream &);
    CORBA::Long update_profile(const std::string &, std::ostream &);
    CORBA::Long stop_ec_manager(const std::string &, std::ostream &);

private:
    
    typedef CORBA::Long (EventServiceAdminTool::* operation)(const std::string &, std::ostream &);
    
    struct command_def
    {
        const char* name;
        operation   op;
    };
    
    static const command_def commands[];
    static const CORBA::ULong nb_commands;
    
    CORBA::Long runOperation(const std::string &, const std::string &, std::ostream &);
    
    EventServiceAdminTool();
    EventServiceAdminTool(const EventServiceAdminTool&);
    EventServiceAdminTool& operator = (const EventServiceAdminTool&);
    
    void unknown_command(std::ostream &);    
    void print_prompt(std::ostream&); 
    void print_info(std::ostream&);

    CORBA::Long connect_generic_channels(const std::string &, const std::string &, std::ostream &);
    CORBA::Long connect_typed_channels(const std::string &, const std::string &, const std::string &, std::ostream &);
    CORBA::Long extract_channel_and_profile(const std::string &, std::string &, CdmwEventAdmin::EventChannelProfile&,  std::ostream &);

    // orb reference
   CORBA::ORB_var  m_orb;

    // Repository object reference
   CosNaming::NamingContext_var m_namingcontext;

    // command file name
    std::string m_commandFile;

    // verbose flag
    bool m_verbose;

    // Root context
    CosNaming::NamingContext_var m_defaultRootContext;
    CosNaming::NamingContext_var m_adminRootContext;

    // Servant context
    std::string m_factory_context;
    std::string m_eventchannel_context;
    std::string m_typedeventchannel_context;

    // Servant object reference
   CdmwEvent::EventChannelFactory_var m_factory;
   CdmwEvent::TypedEventChannelFactory_var m_typedFactory;
    CdmwEventAdmin::EventChannelProfileManager_var m_profileManagerRef;
};


}; // namespace EventAdmin
}; // namespace Cdmw

#endif // INCL_EVENT_SERVICE_ADMIN_TOOL_HPP


