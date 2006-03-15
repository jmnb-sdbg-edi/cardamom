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


#include "performanceadmin/PerformanceAdminTool.hpp"
#include "TraceAndPerf/idllib/CdmwPerformanceAdmin.stub.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingUtil.hpp"

#include "Foundation/common/Locations.hpp"
#include "Foundation/common/Assert.hpp"

#include <string>
#include <cctype>
#include <algorithm>


#define ECHO_PERFADM if (m_verbose) os 

#define PRINT_EXCEPTION(f,l,e,os) \
do { \
  os << "File : " << f << " Line : " << l << "\n";\
  print_exception(e,os);\
} while(0)


using namespace Cdmw;

namespace
{
    const char* INFO_MESSAGE = "Type h or help for a list of available commands.";
    const char* PROMPT_MESSAGE = "\nADMIN>";
    const char* UNKNOWN_COMMAND_MESSAGE = "Bad command!>";

    const CORBA::Long EXIT_ORDERED(-10);
    const CORBA::Long OP_SUCCESS = 0;
    const CORBA::Long OP_FAILURE = -1;

	void print_exception(const CORBA::SystemException & ex, std::ostream & os)
    {
      os << "CORBA system Exception : \n" << ex << std::endl;
	}

	void print_exception(const CORBA::Exception & ex, std::ostream & os)
	{
	  os << "CORBA Exception : \n" << ex << std::endl;
	}

	void print_exception(const Cdmw::Exception & ex, std::ostream & os)
	{
	  os << "CDMW Exception : \n" << ex.what() << std::endl;
	}
    
    bool not_space(char c)
    { return !isspace(c);}

}; // End anonymous namespace

namespace Cdmw
{
namespace PerformanceAdminTool
{

//----------------------------------------------------------------------------
PerformanceAdminTool::PerformanceAdminTool(CORBA::ORB_ptr orb, 
                                           CdmwNamingAndRepository::Repository_ptr rep)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_repository(CdmwNamingAndRepository::Repository::_duplicate(rep)),
      m_verbose(false),
	  m_context(Common::Locations::ADMIN_ROOT_CONTEXT_ID)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    CDMW_ASSERT(!CORBA::is_nil(rep));
}

//----------------------------------------------------------------------------
PerformanceAdminTool::~PerformanceAdminTool() throw()
{
}

//----------------------------------------------------------------------------
const CORBA::ULong PerformanceAdminTool::nb_commands = 8;

// WARNING:
// PerformanceAdminTool::commands should be sorted alphabetically (by operation name)
const PerformanceAdminTool::command_def PerformanceAdminTool::commands[] = 
{ 
    { "activate"   , &PerformanceAdminTool::activate       },
    { "context"    , &PerformanceAdminTool::set_context    },
    { "deactivate" , &PerformanceAdminTool::deactivate     },
    { "exit"       , &PerformanceAdminTool::exit           },
    { "h"          , &PerformanceAdminTool::print_help     },
    { "help"       , &PerformanceAdminTool::print_help     },
    { "verbose"    , &PerformanceAdminTool::toggle_verbose },
    { "x"          , &PerformanceAdminTool::exit           }
};

    
//----------------------------------------------------------------------------
CORBA::Long PerformanceAdminTool::runOperation(const std::string &op,
                                               const std::string &arg, 
                                               std::ostream &os) 
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
void PerformanceAdminTool::print_info(std::ostream& os) const throw()
{
    os << "\n\n"
       << "\t\t--------------------------------------------------\n"
       << "\t\t Cdmw Performance administration console         \n"
       << "\t\t--------------------------------------------------\n"
       << "\n\n"
       << INFO_MESSAGE
       << "\nInfo: (verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
}

//----------------------------------------------------------------------------
void PerformanceAdminTool::print_prompt(std::ostream& os) const throw()
{
	os << PROMPT_MESSAGE << std::flush;
}

//----------------------------------------------------------------------------
void PerformanceAdminTool::unknown_command( std::ostream & os) const throw()
{
    os << UNKNOWN_COMMAND_MESSAGE << std::endl;
}

//----------------------------------------------------------------------------
void PerformanceAdminTool::run(std::istream& is,std::ostream& os) throw()
{
    print_info(os);
	print_prompt(os);

    std::string s;
    while(std::getline(is,s)) 
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
        
        ECHO_PERFADM << "@cmd = <" << command << "> @args = <" << arg << ">" << std::endl;
        if (runOperation(command,arg,os) == EXIT_ORDERED) break; 
        print_prompt(os); 
    } 
}

//----------------------------------------------------------------------------
CORBA::Long PerformanceAdminTool::exit(const std::string & arg, std::ostream & os) throw()
{	
	return EXIT_ORDERED;
}

//----------------------------------------------------------------------------
CORBA::Long PerformanceAdminTool::toggle_verbose(const std::string & arg, std::ostream & os)
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
CORBA::Long PerformanceAdminTool::print_help(const std::string & arg, std::ostream & os) throw()
{
    os << " +==================+============================================+" << "\n"
       << " | COMMAND          | DESCRIPTION                                |" << "\n"
       << " +==================+============================================+" << "\n"
       << " | x | exit         | exit ADMIN mode                            |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | h | help         | print available commands                   |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | verbose          | toggle verbose mode on/off                 |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | context [name]   | set root context where PerformanceAdmin    |" << "\n"
       << " |                  | is registered                              |" << "\n"
       << " |                  | default is AdminRootContext                |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | activate <host> <application> <process>                       |" << "\n"
       << " |                  | activate performance service giving name of|" << "\n"
       << " |                  | host, application, process                 |" << "\n"
       << " | or               |                                            |" << "\n"
       << " | activate <name>  | activate performance service giving        |" << "\n"
       << " |                  | complete context name where                |" << "\n"
       << " |                  | PerformanceAdmin object is registered      |" << "\n"
       << " |                  | under root context                         |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | deactivate <host> <application> <process>                     |" << "\n"
       << " |                  | deactivate performance service giving name |" << "\n"
       << " |                  | of host, application, process              |" << "\n"
       << " | or               |                                            |" << "\n"
       << " | deactivate <name>| deactivate performance service giving      |" << "\n"
       << " |                  | complete context name where                |" << "\n"
       << " |                  | PerformanceAdmin object is registered      |" << "\n"
       << " |                  | under root context                         |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << std::endl;

    return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long PerformanceAdminTool::set_context(const std::string &arg, std::ostream &os)
		throw()
{
  CORBA::Long result = OP_FAILURE;
  const char *p_arg = arg.c_str();
  
  // if undefined context name set defaut root context
  if (*p_arg == '\0')                
  {
    m_context = Common::Locations::ADMIN_ROOT_CONTEXT_ID;
  }
  else
  {
    m_context = p_arg;
  }

  os << "Context set to : " << m_context.c_str() << std::endl;

  try 
  {
    CosNaming::NamingContext_var nc =
            m_repository->resolve_root_context(m_context.c_str());

	result = OP_SUCCESS;
  } 
  catch (const CdmwNamingAndRepository::Repository::NoRootContext & )
  {
    os << "Cannot reach DefaultRootContext!" << std::endl;
  } 

  return result;
}

//----------------------------------------------------------------------------
CORBA::Long PerformanceAdminTool::activate(const std::string &arg, std::ostream &os)
		throw()
{
  CORBA::Long result = set_performance(arg,os,true);
  
  return result;
}

//----------------------------------------------------------------------------
CORBA::Long PerformanceAdminTool::deactivate(const std::string &arg, std::ostream &os)
	    throw()
{
  CORBA::Long result = set_performance(arg,os,false);
  
  return result;
}

//----------------------------------------------------------------------------
CORBA::Long PerformanceAdminTool::set_performance(const std::string &arg, std::ostream &os,
				                                  bool activation) throw()
{
  CORBA::Long result = OP_FAILURE;
  
  try 
  {
    CosNaming::NamingContext_var nc =
            m_repository->resolve_root_context(m_context.c_str());

    if (arg == "")
   	{
      os << "Performance admin name needed." << std::endl;
    } 
	else 
	{   
	  // extract argument
	  std::string host_name;
	  std::string appli_name;
	  std::string process_name;
	  
	  std::string full_context;
	  
	  std::string::const_iterator i = arg.begin();
      std::string::const_iterator end = arg.end();
      
      // find end of first argument delimited by space if exist
      std::string::const_iterator j = std::find_if(i,end,isspace);
	  
      // if several arguments -> "host_name appli_name process_name"
      if (j != end)
      {
        host_name = std::string(i,j);
            
        // skip leading whitespaces
        i = std::find_if(j,end,not_space);
        
        // find end of second argument delimited by space if exist
        j = std::find_if(i,end,isspace);
        
        // if not exist set error
        if (j == end)
        {
          os << "Process name needed." << std::endl;
          return result;
        }
        
        appli_name = std::string(i,j);
        
        // skip leading whitespaces to find third argument
        i = std::find_if(j,end,not_space);
        
        // if not exist set error
        if (i == end)
        {
          os << "Process name needed." << std::endl;
          return result;
        }
        
        process_name = std::string(i,end);
        
        full_context = host_name;
        full_context += std::string("/") + appli_name;
        full_context +=  std::string("/") + process_name +
	                     std::string("/PERFORMANCE/PerformanceAdmin");
	                     
	  }
	  
	  // else full context is given by command argument
	  else
	  {
	    full_context = arg;
      }
                
      os << "get PerformanceAdmin in context " 
         << full_context.c_str() << std::endl
         << "    under root context " << m_context.c_str() << std::endl;
                 
	  // NamingInterface on Root context
      Cdmw::CommonSvcs::Naming::NamingInterface ni(nc.in());

      typedef Cdmw::CommonSvcs::Naming::NamingUtil<CdmwPerformanceAdmin::PerformanceAdmin> Util;
            
	  // get PerformanceAdmin object using Root context NamingInterface
	  CdmwPerformanceAdmin::PerformanceAdmin_var perfoAdmin = Util::resolve_name(ni,full_context);

	  // activate or deactivate the performance admin according parameter
	  if (activation == true)
  	  {
	    perfoAdmin->activate();
	  }
	  else
	  {
	    perfoAdmin->deactivate();
	  }
    }

	result = OP_SUCCESS;
  } 
  catch (const CdmwNamingAndRepository::Repository::NoRootContext & )
  {
    os << "Cannot reach DefaultRootContext!" << std::endl;
  } 
  catch (const CosNaming::NamingContext::NotFound & )
  {
    os << "CosNaming::NamingContext::NotFound exception!" << std::endl;
  } 
  catch (const CosNaming::NamingContext::CannotProceed & ) 
  {
    os << "Unexpected Error (CannotProceed exception)!" << std::endl;
  } 
  catch (const CosNaming::NamingContext::InvalidName & )
  {
    os << "Invalid Name <" << arg << ">!" << std::endl;
  } 
  catch (const Cdmw::Common::TypeMismatchException & e) 
  {
    os << "TypeMismatchException (Not a Factory object) <" 
        << e.what() << ">" << std::endl;
  } 
  catch (const CORBA::SystemException & e) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception & e) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }

  return result;
}

//----------------------------------------------------------------------------

}; // namespace PerformanceAdminTool
}; // namespace Cdmw

