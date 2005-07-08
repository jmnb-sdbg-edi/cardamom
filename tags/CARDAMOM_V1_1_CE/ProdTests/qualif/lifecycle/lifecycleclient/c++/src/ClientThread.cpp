/* =========================================================================== *
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
 * =========================================================================== */

#include "lifecycleclient/ClientThread.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"
#include "Repository/naminginterface/NamingUtil.hpp"
#include <string>
#include <cctype>
#include <algorithm>

#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"

#include "lifecycleclient/TestEntityStrategy.hpp"
#include "lifecycleclient/TestEntityStrategyRestart.hpp"
#include "lifecycleclient/TestUnManagedEntityStrategy.hpp"
#include "lifecycleclient/TestUnManagedEntityStrategyRestart.hpp"
#include "lifecycleclient/TestSessionStrategy.hpp"
#include "lifecycleclient/TestServiceStrategy.hpp"

#include "lifecycleclient/ProcessAgent.stub.hpp"

#include <sstream>

#define PRINT_EXCEPTION(f,l,e,os) \
do { \
    os << "File : " << f << " Line : " << l << "\n";\
    print_exception(e,os);\
    CDMW_ASSERT(0); \
} while(0) \

namespace
{
    const char* INFO_MESSAGE = "Type h or help for a list of available commands.";
    const char* PROMPT_MESSAGE = "\n%>";
    const char* UNKNOWN_COMMAND_MESSAGE = "Bad command!>";

    const CORBA::Long EXIT_ORDERED(-10);
    const CORBA::Long OP_SUCCESS = 0;
    const CORBA::Long OP_FAILURE = -1;
       

    void print_exception(const CdmwLifeCycle::AlreadyExistant & ex, std::ostream & os)
    {
        os << "Exception CdmwLifeCycle::AlreadyExistant \n"
           << ex.name.in() << std::endl;
    }

    void print_exception(const CdmwLifeCycle::InvalidName & ex, std::ostream & os)
    {
        os << "Exception CdmwLifeCycle::InvalidName \n"
           << ex.reason.in() << std::endl;
    }

    void print_exception(const Cdmw::Common::TypeMismatchException & ex, 
                         std::ostream & os)
    {
        os << "Exception Cdmw::Common::TypeMismatchException " 
            << ex.what() << std::endl;
    }

    void print_exception(const CORBA::SystemException & ex, std::ostream & os)
    {
        os << ex << std::endl;
    }

    void print_exception(const CORBA::Exception & ex, std::ostream & os)
    {
        os << ex << std::endl;
    }

    
    bool not_space(char c)
    { return !isspace(c);}

}; // End anonymous namespace

namespace Cdmw
{
namespace LifeCycle
{

//----------------------------------------------------------------------------
ClientThread::ClientThread(CORBA::ORB_ptr orb, 
                           CdmwNamingAndRepository::Repository_ptr rep,
                           std::istream & is,
                           std::ostream & os)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_repository(CdmwNamingAndRepository::Repository::_duplicate(rep)),
      m_verbose(false),
      m_istream(is),
      m_ostream(os),
      m_exit_required(false)
{
}

//----------------------------------------------------------------------------
ClientThread::~ClientThread() throw()
{
}

//----------------------------------------------------------------------------
void ClientThread::run() throw()
{
    run (m_istream,m_ostream);
}
//----------------------------------------------------------------------------
const CORBA::ULong ClientThread::nb_commands = 14;
const ClientThread::command_def ClientThread::commands[] =  // should be sorted alphabetically (by operation name)
{ 
    { "all"               , &ClientThread::test_all                    },
    { "entity"            , &ClientThread::test_Entity                 },
    { "entity_restart"    , &ClientThread::test_EntityRestart          },
    { "exit"              , &ClientThread::exit                        },
    { "h"                 , &ClientThread::print_help                  },
    { "help"              , &ClientThread::print_help                  },
    { "service"           , &ClientThread::test_Service                },
    { "session"           , &ClientThread::test_Session                },
    { "sleep"             , &ClientThread::sleep                       },
    { "stop"              , &ClientThread::stop_proc                   },
    { "unmanaged"         , &ClientThread::test_UnManagedEntity        },
    { "unmanaged_restart" , &ClientThread::test_UnManagedEntityRestart },
    { "verbose"           , &ClientThread::toggle_verbose              },
    { "x"                 , &ClientThread::exit                        }
};

    
//----------------------------------------------------------------------------
CORBA::Long ClientThread::runOperation(const std::string & op,
                                const std::string & arg, 
                                std::ostream &      os) throw()
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
void ClientThread::print_info(std::ostream& os) const throw()
{
    os << "\n\n"
       << "\t\t---------------------------------------\n"
       << "\t\t  Cdmw LifeCycle Service Test Driver  \n"
       << "\t\t---------------------------------------\n"
       << "\n\n"
       << INFO_MESSAGE
       << "\nInfo: (verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
}
//----------------------------------------------------------------------------
void ClientThread::print_prompt(std::ostream& os) const throw()
{
    os << PROMPT_MESSAGE << std::flush;
}
//----------------------------------------------------------------------------
void ClientThread::unknown_command( std::ostream & os) const throw()
{
    os << UNKNOWN_COMMAND_MESSAGE << std::endl;
}
//----------------------------------------------------------------------------
void ClientThread::run(std::istream& is,std::ostream& os) throw()
{
    print_info(os);
    print_prompt(os);
    std::string s;
    while(!m_exit_required && std::getline(is,s)) {
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
        
        if (runOperation(command,arg,os) == EXIT_ORDERED) break; 
        print_prompt(os); 
    } 
}
//----------------------------------------------------------------------------
CORBA::Long ClientThread::exit(const std::string & arg, std::ostream & os) 
    throw()
{    
//     try  {
//         if (m_verbose) os << "Initiating ORB shutdown." << std::endl;
//         m_orb->shutdown(false);
//     } catch (const CORBA::SystemException & ex) {
//         PRINT_EXCEPTION(__FILE__,__LINE__,ex,os);
//     }
    return EXIT_ORDERED;
}
//----------------------------------------------------------------------------
CORBA::Long ClientThread::toggle_verbose(const std::string & arg, std::ostream & os) 
    throw()
{    
    m_verbose = !m_verbose;
    os << "(verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
    return OP_SUCCESS;
}
//----------------------------------------------------------------------------
// Available commands:
//
CORBA::Long ClientThread::print_help(const std::string & arg, std::ostream & os)
    throw()
{
     os << " +======================+============================================+" << "\n"
        << " | COMMAND              | DESCRIPTION                                |" << "\n"
        << " +======================+============================================+" << "\n"
        << " | all                  | test all the 4 strategies                  |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | entity               | test the Entity strategy                   |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | entity_restart opt   | test restart of the Entity strategy        |" << "\n"
        << " |                      | opt = create|start|stop|remove|all         |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | x | exit             | exit ClientThread mode                     |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | h | help             | print available commands                   |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | service              | test the Service strategy                  |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | session              | test the Session strategy                  |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | sleep n              | Sleeps for n milliseconds                  |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | stop  procAgentFac   | Stops a process hosting the ProcessAgent   |" << "\n"
        << " |                      | factory.                                   |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | unmanaged            | test the UnManagedEntity strategy          |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | unmanaged_restart opt| test restart of UnManagedEntity strategy   |" << "\n"
        << " |                      | opt = create|start|stop|remove|all         |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | verbose              | toggle verbose mode on/off                 |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << std::endl;

     return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::stop_proc(const std::string & arg, std::ostream & os)
    throw()
{ 
     
    CORBA::Long result = OP_FAILURE;

    try {
        
        CosNaming::NamingContext_var nc 
            = m_repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);

        if (arg == "") {
            os << "Factory name needed." << std::endl;
        } else {        
            Cdmw::NamingAndRepository::NamingInterface ni(nc.in());

            typedef Cdmw::NamingAndRepository::NamingUtil<Validation::ProcessAgentFactory> Util;
            
            Validation::ProcessAgentFactory_var factory
                = Util::resolve_name(ni,arg);

            // Create a ProcessAgent object
            Validation::ProcessAgent_var proc_agent
                = factory->create_process_agent();
            if (CORBA::is_nil(proc_agent.in())) {                
                os << "Nil ProcessAgent reference!" << std::endl;
            } else {
                proc_agent->stop_process();
            }
            result = OP_SUCCESS;
        }
    } catch (const CdmwNamingAndRepository::Repository::NoRootContext & ) {
        os << "Cannot reach DefaultRootContext!" << std::endl;
    } catch (const CosNaming::NamingContext::NotFound & ) {
        os << "CosNaming::NamingContext::NotFound exception!" << std::endl;
    } catch (const CosNaming::NamingContext::CannotProceed & ) {
        os << "Unexpected Error (CannotProceed exception)!" << std::endl;
    } catch (const CosNaming::NamingContext::InvalidName & ) {
        os << "Invalid Name <" << arg << ">!" << std::endl;
    } catch (const Cdmw::Common::TypeMismatchException & e) {
        os << "TypeMismatchException (Not a Process Agent Factory object) <" 
           << e.what() << ">" << std::endl;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::sleep(const std::string & arg, std::ostream & os)
    throw()
{ 
     
    CORBA::Long result = OP_FAILURE;

    try {
        if (arg == "") {
            os << "Number of milliseconds needed." << std::endl;
        } else {
            std::istringstream is(arg);
            unsigned int n;
            is >> n;
            if (is.fail()) {             
                os << "Invalid number of milliseconds!" << std::endl;
            } else {
                if (m_verbose) os << "Sleeping for " << n << " milliseconds..." << std::endl;
                Cdmw::OsSupport::OS::sleep(n);
                result = OP_SUCCESS;
            }
        }
    } catch (const Cdmw::Exception & e ) {
        os << "CDMW Exception : " << e.what() << std::endl;
    }

    return result;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::test_Entity(const std::string & arg, std::ostream & os)
    throw()
{ 
  
    using namespace Cdmw::LifeCycle;
    TestEntityStrategy entity(arg,os);
    entity.start(); 
    return OP_SUCCESS;
}
//----------------------------------------------------------------------------
CORBA::Long ClientThread::test_EntityRestart(const std::string & arg, std::ostream & os)
    throw()
{ 
  
    using namespace Cdmw::LifeCycle;
    TestEntityStrategyRestart entity_restart(arg,os);
    entity_restart.start(); 
    return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::test_UnManagedEntity(const std::string & arg, std::ostream & os)
    throw()
{ 
    
    using namespace Cdmw::LifeCycle;
    TestUnManagedEntityStrategy unmng_entity(arg,os);
    unmng_entity.start(); 
    return OP_SUCCESS;
}
//----------------------------------------------------------------------------
CORBA::Long ClientThread::test_UnManagedEntityRestart(const std::string & arg, 
                                                      std::ostream & os)
    throw()
{ 
    
    using namespace Cdmw::LifeCycle;
    TestUnManagedEntityStrategyRestart unmng_entity_restart(arg,os);
    unmng_entity_restart.start(); 
    return OP_SUCCESS;
}


//----------------------------------------------------------------------------
CORBA::Long ClientThread::test_Session(const std::string & arg, std::ostream & os)
    throw()
{ 
    using namespace Cdmw::LifeCycle;
    TestSessionStrategy session(arg,os);
    session.start(); 
    return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::test_Service(const std::string & arg, std::ostream & os)
    throw()
{ 
    using namespace Cdmw::LifeCycle;
    TestServiceStrategy service(arg,os);
    service.start(); 
    return OP_SUCCESS;
}
//----------------------------------------------------------------------------
CORBA::Long ClientThread::test_all(const std::string & arg, std::ostream & os)
    throw()
{
    CORBA::Long entity    = this->test_Entity(arg, os);
    CORBA::Long unmanaged = test_UnManagedEntity(arg, os);
    CORBA::Long session   = test_Session(arg, os);
    CORBA::Long service   = test_Service(arg, os);
    return entity && unmanaged && session && service;
}
//----------------------------------------------------------------------------

}; // namespace LifeCycle
}; // namespace Cdmw

