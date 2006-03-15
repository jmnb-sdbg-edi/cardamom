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


#include "SystemMngt/tools/ProcessAdmin.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include <string>
#include <cctype>
#include <algorithm>
#include <fstream>


#define ECHO_CMD if (m_verbose) os 


namespace
{
    const char* INFO_MESSAGE = "Type h or help for a list of available commands.";
    const char* PROMPT_MESSAGE = "\nADMIN>";
    const char* UNKNOWN_COMMAND_MESSAGE = "Bad command!>";

    const CORBA::Long EXIT_ORDERED(-10);
    const CORBA::Long OP_SUCCESS = 0;
    const CORBA::Long OP_FAILURE = -1;

    
    bool not_space(char c)
    { return !isspace(c);}


}; // End anonymous namespace

namespace Cdmw
{
namespace Tools
{

//----------------------------------------------------------------------------
ProcessAdmin::ProcessAdmin(CORBA::ORB_ptr orb, 
                           CdmwPlatformMngt::ProcessDelegate_ptr proc)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_process_delegate(CdmwPlatformMngt::ProcessDelegate::_duplicate(proc)),
      m_verbose(false)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    CDMW_ASSERT(!CORBA::is_nil(proc));
}

//----------------------------------------------------------------------------
ProcessAdmin::~ProcessAdmin() throw()
{
}
//----------------------------------------------------------------------------
const CORBA::ULong ProcessAdmin::nb_commands = 16;

// WARNING:
// ProcessAdmin::commands should be sorted alphabetically (by operation name)
const ProcessAdmin::command_def ProcessAdmin::commands[] = 
{ 
    { "exit"               , &ProcessAdmin::exit                   },
    { "get_pull"           , &ProcessAdmin::get_pull_monitorable   },
    { "get_push"           , &ProcessAdmin::get_push_monitorable   },
    { "get_service"        , &ProcessAdmin::get_service            },
    { "h"                  , &ProcessAdmin::print_help             },
    { "help"               , &ProcessAdmin::print_help             },
    { "init"               , &ProcessAdmin::initialise             },    
    { "load"               , &ProcessAdmin::load_file              },
    { "nb_steps"           , &ProcessAdmin::get_nb_steps           },
    { "next"               , &ProcessAdmin::next_step              },
    { "run"                , &ProcessAdmin::run_order              },
    { "stop"               , &ProcessAdmin::stop_order             },    
    { "verbose"            , &ProcessAdmin::toggle_verbose         },
    { "x"                  , &ProcessAdmin::exit                   }    
};

    
//----------------------------------------------------------------------------
CORBA::Long ProcessAdmin::runOperation(const std::string & op,
                                          const std::string & arg, 
                                          std::ostream &      os) 
    throw()
{
    CORBA::Long l = 0;
    CORBA::Long r = (CORBA::Long)nb_commands;
    CORBA::Long m;
    CORBA::Long res;
    
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
void ProcessAdmin::print_info(std::ostream& os) const throw()
{
    os << "\n\n"
       << "\t\t--------------------------------------------------\n"
       << "\t\t PlatformManagement Process administration console \n"
       << "\t\t--------------------------------------------------\n"
       << "\n\n"
       << INFO_MESSAGE
       << "\nInfo: (verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
}
//----------------------------------------------------------------------------
void ProcessAdmin::print_prompt(std::ostream& os) const throw()
{
	os << PROMPT_MESSAGE << std::flush;
}
//----------------------------------------------------------------------------
void ProcessAdmin::unknown_command( std::ostream & os) const throw()
{
    os << UNKNOWN_COMMAND_MESSAGE << std::endl;
}
//----------------------------------------------------------------------------
void ProcessAdmin::run(std::istream& is,std::ostream& os) throw()
{
    print_info(os);
	print_prompt(os);

    std::string s;
    while(std::getline(is,s)) {  
        // ===============
        // Extract command        
        // ===============
        // skip leading whitespaces
        std::string::const_iterator i = s.begin();
        std::string::const_iterator end = s.end();
        i = std::find_if(i,end,not_space);
        // find end of command
        std::string::const_iterator j = std::find_if(i,end,isspace);
        // copy the characters in [i,j)
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
        
        ECHO_CMD << "@cmd = <" << command << "> @args = <" << arg << ">" << std::endl;
        if (runOperation(command,arg,os) == EXIT_ORDERED) break; 
        print_prompt(os); 
    } 
}
//----------------------------------------------------------------------------
CORBA::Long ProcessAdmin::exit(const std::string & arg, std::ostream & os) throw()
{	
	return EXIT_ORDERED;
}
//----------------------------------------------------------------------------
CORBA::Long ProcessAdmin::toggle_verbose(const std::string & arg, std::ostream & os) throw()
{	
    m_verbose = !m_verbose;
    os << "(verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
	return OP_SUCCESS;
}
//----------------------------------------------------------------------------
// Available commands:
//
CORBA::Long ProcessAdmin::print_help(const std::string & arg, std::ostream & os) throw()
{
    os << " +=====================+============================================+" << "\n"
       << " | COMMAND            | DESCRIPTION                                |" << "\n"
       << " +====================+============================================+" << "\n"
       << " | x | exit           | exit ADMIN mode                            |" << "\n"
       << " +--------------------+--------------------------------------------+" << "\n"        
       << " | get_pull [check]   | Returns the pull monitorable interface.    |" << "\n"
       << " |                    | check: Checks if the process is alive.     |" << "\n" 
       << " +--------------------+--------------------------------------------+" << "\n"
       << " | get_push           | Returns the push monitorable interface.    |" << "\n"
       << " +--------------------+--------------------------------------------+" << "\n"
       << " | get_service        | Returns embedded service.                  |" << "\n"
       << " +--------------------+--------------------------------------------+" << "\n"
       << " | h | help           | print available commands                   |" << "\n"
       << " +--------------------+--------------------------------------------+" << "\n"
       << " | init state_retrieval startup_mode [state_id] |                  |" << "\n"
       << " |                    |                                            |" << "\n"
       << " |                    |   state_retrieval= LAST|RECOVER|SPECIFIED  |" << "\n"
       << " |                    |   startup_mode: short                      |" << "\n"
       << " |                    |   state_id= state id for SPECIFIED         |" << "\n"
       << " +--------------------+--------------------------------------------+" << "\n"
       << " | load filename      | Load 'filename' batch file                 |" << "\n"
       << " +--------------------+--------------------------------------------+" << "\n"        
       << " | nb_steps           | Returns the number of initialisation steps |" << "\n"
       << " |                    | of the managed process.                    |" << "\n"        
       << " +--------------------+--------------------------------------------+" << "\n"
       << " | next               | Requests the process to perform its next   |" << "\n"
       << " |                    | step.                                      |" << "\n"        
       << " +--------------------+--------------------------------------------+" << "\n"
       << " | run                | Requests the process to run.               |" << "\n"        
       << " +--------------------+--------------------------------------------+" << "\n"
       << " | stop               | Requests the process to stop properly.     |" << "\n"        
       << " +--------------------+--------------------------------------------+" << "\n"
       << " | verbose            | toggle verbose mode on/off                 |" << "\n"
       << " +--------------------+--------------------------------------------+" << "\n"
       << std::endl;

    return OP_SUCCESS;
}
//----------------------------------------------------------------------------
CORBA::Long ProcessAdmin::load_file(const std::string & arg, std::ostream & os) throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {
        ECHO_CMD << " Loading batch file \"" << arg << "\"." << std::endl;
        std::ifstream in(arg.c_str());
        
        if (!in)
            os << "Could not open file \"" << arg << "\"." << std::endl;
        else {
            ProcessAdmin admin(m_orb.in(),m_process_delegate.in());
            admin.run(in,os);
            ECHO_CMD << "Batch file \"" << arg << "\" done." << std::endl;
            result = OP_SUCCESS;
        }
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}
//----------------------------------------------------------------------------
CORBA::Long ProcessAdmin::get_nb_steps(const std::string & arg, std::ostream & os) throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {        
        CORBA::ULong nb = m_process_delegate->nb_steps();
        os << "Number of initialisation steps :" << nb << std::endl;
        result = OP_SUCCESS;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}
//----------------------------------------------------------------------------
CORBA::Long ProcessAdmin::get_service(const std::string & arg, std::ostream & os)
    throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {
        CORBA::Object_var service = m_process_delegate->get_service();
        if (CORBA::is_nil(service.in()))
            os << "No embedded service." << std::endl;
        else {
            CORBA::String_var s = m_orb->object_to_string(service.in());
            os << "Embedded service : " << s.in() << std::endl;
        }
        result = OP_SUCCESS;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}
//----------------------------------------------------------------------------
CORBA::Long ProcessAdmin::get_pull_monitorable(const std::string & arg, std::ostream & os)
    throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {
        FT::PullMonitorable_var pull = m_process_delegate->get_pull_monitorable();
        if (CORBA::is_nil(pull.in()))
            os << "Pull monitoring not supported." << std::endl;
        else {
            CORBA::String_var s = m_orb->object_to_string(pull.in());
            os << "Pull monitoring object : " << s.in() << std::endl;
            if (arg == "check") {
                os << "Checking object ...";
                try {
                    pull->is_alive();
                    os << "OK." << std::endl;
                } catch (const CORBA::SystemException & e) {
                    os << "NON OK." << "\n\t<CORBA System Exception :" << e << ">" << std::endl;
                }
            }
        }
        result = OP_SUCCESS;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}

//----------------------------------------------------------------------------
CORBA::Long ProcessAdmin::get_push_monitorable(const std::string & arg, std::ostream & os)
    throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {
        CdmwPlatformMngt::PushMonitorable_var push = m_process_delegate->get_push_monitorable();
        if (CORBA::is_nil(push.in()))
            os << "Push monitoring not supported." << std::endl;
        else {
            CORBA::String_var s = m_orb->object_to_string(push.in());
            os << "Push monitoring object : " << s.in() << std::endl;
        }
        result = OP_SUCCESS;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}

// init state_retrieval startup_mode state_id)
// state_retrieval = LAST|RECOVER|SPECIFIED
CORBA::Long ProcessAdmin::initialise(const std::string & arg, std::ostream & os)
    throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {
        using namespace CdmwPlatformMngtBase;
        
        // arg contains the following:
        //   state_retrieval startup_mode state_id
        //
        StartupKind startup_kind;
        // =======================
        // Extract state_retrieval
        // =======================
        std::string::const_iterator i = arg.begin();
        std::string::const_iterator end = arg.end();
        // find end of ior
        std::string::const_iterator j = std::find_if(i,end,isspace);
        // copy the characters in [i,j)
        std::string retrieval;
        if (i != end) {
            retrieval = std::string(i,j);
            // Allowed values are : LAST|RECOVER|SPECIFIED
            if (retrieval == "LAST")
                startup_kind.state_retrieval = LAST_STATE;
            else if (retrieval == "RECOVER")
                startup_kind.state_retrieval = RECOVER_STATE;
            else if (retrieval == "SPECIFIED")
                startup_kind.state_retrieval = SPECIFIED_STATE;
            else {
                os << "Invalid value for state retrieval. "
                   << "Allowed values are LAST|RECOVER|SPECIFIED." << std::endl;
		return result;
	    }
        } else {
            os << "StateRetrieval is missing." << std::endl;
            return result;
        }
        // ====================
        // Extract startup_mode
        // ====================
        // skip leading whitespaces
        i = std::find_if(j,end,not_space);
        std::string mode;
        // copy arguments
        if (i != end) {
            mode = std::string(i,end);
            startup_kind.startup_mode = atoi(mode.c_str());
        } else {
            os << "StartupMode is missing." << std::endl;
            return result;
        }
        if (startup_kind.state_retrieval == SPECIFIED_STATE)
        {
            // ================
            // Extract state_id
            // ================
            // skip leading whitespaces
            i = std::find_if(j,end,not_space);
            std::string id;
            // copy arguments
            if (i != end) {
                id = std::string(i,end);
                startup_kind.specified_state_id = CORBA::string_dup(id.c_str());
            } else {
                os << "Specified State Id is missing." << std::endl;
                return result;
            }
        }
    
        os << "Sending <initialise> order...";
        m_process_delegate->initialise(startup_kind);
        os << "done." << std::endl;
        
        result = OP_SUCCESS;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) << std::endl;
    }

    return result;
}
//----------------------------------------------------------------------------

CORBA::Long ProcessAdmin::next_step(const std::string & arg, std::ostream & os)
    throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {
        os << "Sending <next_step> order...";
        m_process_delegate->next_step();
        os << "done." << std::endl;
        
        result = OP_SUCCESS;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}
//----------------------------------------------------------------------------

CORBA::Long ProcessAdmin::run_order(const std::string & arg, std::ostream & os)
    throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {
        os << "Sending <run> order...";
        m_process_delegate->run();
        os << "done." << std::endl;
        
        result = OP_SUCCESS;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}

//----------------------------------------------------------------------------

CORBA::Long ProcessAdmin::stop_order(const std::string & arg, std::ostream & os)
    throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {
        os << "Sending <stop> order...";
        m_process_delegate->stop();
        
        result = OP_SUCCESS;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }
    os << "done." << std::endl;

    return result;
}


//----------------------------------------------------------------------------

} // namespace Tools
} // namespace Cdmw

