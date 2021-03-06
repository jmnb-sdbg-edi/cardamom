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


#include "traceadmin/TraceAdminTool.hpp"

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
#include "TraceAndPerf/idllib/CdmwPerformanceAdmin.stub.hpp"
#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.stub.hpp"
#include "TraceAndPerf/idllib/CdmwTraceCollector.stub.hpp"

#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingUtil.hpp"


#define ECHO_TRACEADM if (m_verbose) os 

#define PRINT_EXCEPTION(f,l,e,os) \
do { \
  os << "File : " << f << " Line : " << l << "\n";\
  print_exception(e,os);\
} while(0)

#define PRINT_EXCEPTION_MSG(f,l,msg,os) \
do { \
  os << "File : " << f << " Line : " << l << "\n";\
  os << msg << "\n";\
  CDMW_ASSERT(0); \
} while(0)

using namespace Cdmw;
using namespace Cdmw::OsSupport;
using namespace Cdmw::OrbSupport;

namespace
{
    const char* INFO_MESSAGE = "Type h or help for a list of available commands.";
    const char* PROMPT_MESSAGE = "\nTRACE ADMIN>";
    const char* UNKNOWN_COMMAND_MESSAGE = "Bad command!>";

    const CORBA::Long EXIT_ORDERED(-10);
    const CORBA::Long OP_SUCCESS = 0;
    const CORBA::Long OP_FAILURE = -1;

	const int PRODUCER = 0;
	const int GLOBAL_COLLECTOR = 1;
	const int LOCAL_COLLECTOR = 2;
    
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
    { return !isspace(c); }

}; // End anonymous namespace

namespace Cdmw
{
namespace TraceAdmin
{

//----------------------------------------------------------------------------
TraceAdminTool::TraceAdminTool(CORBA::ORB_ptr orb, 
                               CdmwNamingAndRepository::Repository_ptr rep,
							   const std::string& commandFile)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_repository(CdmwNamingAndRepository::Repository::_duplicate(rep)),
	  m_commandFile(commandFile),
      m_verbose(false)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    CDMW_ASSERT(!CORBA::is_nil(rep));

	// initialize default root naming context
    try 
    {
      m_defaultRootContext = m_repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
    } 
    catch (const CdmwNamingAndRepository::Repository::NoRootContext & )
    {
	  std::cout << "Cannot reach default RootContext!" << std::endl;
    } 

	// initialize admin root naming context
    try 
    {
      m_adminRootContext = m_repository->resolve_root_context(Common::Locations::ADMIN_ROOT_CONTEXT_ID);
    } 
    catch (const CdmwNamingAndRepository::Repository::NoRootContext & )
    {
	  std::cout << "Cannot reach admin RootContext!" << std::endl;
    } 

	// initialize the global trace collector context
	// Pattern is :
	//    CDMW/SERVICES/TRACE/COLLECTORS/
    m_global_collector_context = Common::Locations::CDMW_SERVICES_NAME_DOMAIN;
    m_global_collector_context	+= "/";
    m_global_collector_context	+= Common::Locations::TRACE_COLLECTORS_NAME_DOMAIN;
    
    // initialize the local trace collector context to localhost
    m_local_collector_context = Cdmw::OsSupport::OS::get_hostname();

	m_traceProducerRef = CdmwTrace::TraceProducer::_nil();
    m_global_collectorRef = CdmwTrace::Collector::_nil();
    m_local_collectorRef = CdmwTrace::Collector::_nil();
}

//----------------------------------------------------------------------------
TraceAdminTool::~TraceAdminTool() throw()
{
}

//----------------------------------------------------------------------------
const CORBA::ULong TraceAdminTool::nb_commands = 21;

// WARNING:
// TraceAdminTool::commands should be sorted alphabetically (by operation name)
const TraceAdminTool::command_def TraceAdminTool::commands[] = 
{ 
	{ "activ_global_collector_level"   , &TraceAdminTool::activ_global_collector_level   },
	{ "activ_local_collector_level"    , &TraceAdminTool::activ_local_collector_level    },
	{ "activ_producer_level"           , &TraceAdminTool::activ_producer_level           },
	{ "deactiv_global_collector_level" , &TraceAdminTool::deactiv_global_collector_level },
	{ "deactiv_local_collector_level"  , &TraceAdminTool::deactiv_local_collector_level  },
	{ "deactiv_producer_level"         , &TraceAdminTool::deactiv_producer_level         },
	{ "exit"                           , &TraceAdminTool::exit                           },
	{ "get_global_collector_levels"    , &TraceAdminTool::get_global_collector_levels    },	
	{ "get_global_collectors"          , &TraceAdminTool::get_global_collectors          },
	{ "get_local_collector_levels"     , &TraceAdminTool::get_local_collector_levels     },
	{ "get_local_collectors"           , &TraceAdminTool::get_local_collectors           },
	{ "get_producer_levels"            , &TraceAdminTool::get_producer_levels            },
	{ "get_producers"                  , &TraceAdminTool::get_producers                  },
	{ "get_registered_local_collectors", &TraceAdminTool::get_registered_local_collectors},
	{ "h"                              , &TraceAdminTool::print_help                     },
	{ "help"                           , &TraceAdminTool::print_help                     },
	{ "register_local_collector"       , &TraceAdminTool::register_local_collector       },      
	{ "set_producer_context"           , &TraceAdminTool::set_producer_context           },    
	{ "unregister_local_collector"     , &TraceAdminTool::unregister_local_collector     },
	{ "verbose"                        , &TraceAdminTool::toggle_verbose                 },
	{ "x"                              , &TraceAdminTool::exit                           }
};

    
//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::runOperation(const std::string &op,
                                         const std::string &arg, 
                                         std::ostream &os) 
{
    CORBA::Long l = 0;
    CORBA::Long r = (CORBA::Long)nb_commands;
    CORBA::Long m;
    CORBA::Long res;
    
    // Do nothing if command begins with character '#' (comment line in cmd file)
    if (op[0] == '#')
    {
      return OP_SUCCESS;
    }
    
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
void TraceAdminTool::print_info(std::ostream& os)
{
    os << "\n\n"
       << "\t\t--------------------------------------------------\n"
       << "\t\t Cdmw Trace administration console               \n"
       << "\t\t--------------------------------------------------\n"
       << "\n\n"
       << INFO_MESSAGE
       << "\nInfo: (verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
}

//----------------------------------------------------------------------------
void TraceAdminTool::print_prompt(std::ostream& os)
{
	os << PROMPT_MESSAGE << std::flush;
}

//----------------------------------------------------------------------------
void TraceAdminTool::unknown_command( std::ostream & os)
{
    os << UNKNOWN_COMMAND_MESSAGE << std::endl;
}

//----------------------------------------------------------------------------
void TraceAdminTool::run(std::istream& is,std::ostream& os,bool commandFile)
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
        {
          arg = std::string(i,end);
		  // suppress if exist \r at end of of arguments string
		  int chix = arg.size() - 1;
		  if (arg[chix] == '\r')
		  {
		    arg.erase(chix);
		  }
		}
        
        ECHO_TRACEADM << "@cmd = <" << command << "> @args = <" << arg << ">" << std::endl;
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
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::exit(const std::string & arg, std::ostream & os)
{	
	return EXIT_ORDERED;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::toggle_verbose(const std::string & arg, std::ostream & os)
{	
    m_verbose = !m_verbose;
    os << "(verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
	return OP_SUCCESS;
}

//----------------------------------------------------------------------------
// Available commands:
//
CORBA::Long TraceAdminTool::print_help(const std::string & arg, std::ostream & os)
{
     os << " +===================================+============================================+" << "\n"
        << " | COMMAND                           | DESCRIPTION                                |" << "\n"
        << " +===================================+============================================+" << "\n"
        << " | x | exit                          | exit ClientThread mode                     |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n"
        << " | h | help                          | print available commands                   |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n" 
        << " | verbose                           | toggle verbose mode on/off                 |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | set_producer_context <path>       | set path of producer context               |" << "\n"
		<< " |                                   | path has the form <host>/<application>     |" << "\n"
		<< " |                                   | <host> is used to set the local trace      |" << "\n"
		<< " |                                   | collector context                          |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | get_global_collectors             | get available global collectors            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | get_local_collectors              | get available local collectors             |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | get_producers                     | get available processes with producer      |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | activ_global_collector_level      | activate the level in global collector     |" << "\n"
		<< " |  <collector_name>                 |                                            |" << "\n"
		<< " |  [componentname] <domain> <level> |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | deactiv_global_collector_level    | deactivate the level in global collector   |" << "\n"
		<< " |  <collector_name>                 |                                            |" << "\n"
		<< " |  [componentname] <domain> <level> |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | get_global_collector_levels       | get levels of global collector             |" << "\n"
		<< " |  <collector_name>                 |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | activ_local_collector_level       | activate the level in local collector      |" << "\n"
		<< " |  <collector_name>                 |                                            |" << "\n"
		<< " |  [componentname] <domain> <level> |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | deactiv_local_collector_level     | deactivate the level in local collector    |" << "\n"
		<< " |  <collector_name>                 |                                            |" << "\n"
		<< " |  [componentname] <domain> <level> |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | get_local_collector_levels        | get levels of local collector              |" << "\n"
		<< " |  <collector_name>                 |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | activ_producer_level              | activate the level in producer             |" << "\n"
		<< " |  <process_name>                   |                                            |" << "\n"
		<< " |  [componentname] <domain> <level> |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | deactiv_producer_level            | deactivate the level in producer           |" << "\n"
		<< " |  <process_name>                   |                                            |" << "\n"
		<< " |  [componentname] <domain> <level> |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | get_producer_levels               | get levels of producer                     |" << "\n"
		<< " |  <process_name>                   |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | register_local_collector          | register local collector in the producer   |" << "\n"
		<< " |  <process_name>                   |                                            |" << "\n"
		<< " |  <collector_name> <mnemonic>      |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | unregister_local_collector        | unregister the local collector in the      |" << "\n"
		<< " |  <process_name> <ident>           | producer                                   |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | get_registered_local_collectors   | get registered local collectors of producer|" << "\n"
		<< " |  <process_name>                   |                                            |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
        << std::endl;

     return OP_SUCCESS;
}


//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::set_producer_context(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
  const char *p_arg = arg.c_str();
  
  // if undefined context name set error
  if (*p_arg == '\0')                
  {
    os << "producer context is  not defined" << std::endl;
  }
  else
  {
    m_producer_context = p_arg;

    os << "producer context set to : " << m_producer_context.c_str() << std::endl;
    
    // set the associated local trace collector (extract the host name)
    size_t ix = arg.find ('/');
    
    // if not found
    if (ix == std::string::npos)
    {
        // initialize the local trace collector context to localhost
        m_local_collector_context = Cdmw::OsSupport::OS::get_hostname();
    }
    else
    {
        // initialize the local trace collector context with extracted host
        m_local_collector_context = arg.substr (0,ix);
    }
    
	result = OP_SUCCESS;
  }


  return result;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::get_global_collector_serv (const std::string &collectorName, 
				                                       bool check_serv, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
  
  std::string collector_path;

  try 
  {
    // global trace collector location is:
    // CDMW/SERVICES/TRACE/COLLECTORS/collectorName
      
	// NamingInterface on default Root context
    Cdmw::CommonSvcs::Naming::NamingInterface ni_root (m_defaultRootContext.in());
    
	// set complete collector name
	collector_path = m_global_collector_context;
	collector_path += "/";
	collector_path += collectorName;

	// get TraceCollector object using Root context NamingInterface
    typedef Cdmw::CommonSvcs::Naming::NamingUtil<CdmwTrace::Collector> Util;
	m_global_collectorRef = Util::resolve_name(ni_root,collector_path);

	if (CORBA::is_nil(m_global_collectorRef.in()))
    {
      // print error message only if call is to get and not to check object
      if (check_serv == false)
      {
        os << "Collector Name Resolve Error \n" 
           << collectorName.c_str()
		   << std::endl;
      }
	}
	else
	{
	  result = OP_SUCCESS;
    }
  } 
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const CosNaming::NamingContext::NotFound &)
  {
    // print error message only if call is to get and not to check object
    if (check_serv == false)
    {
      os << "Trace Collector Name not found \n"
	     << collector_path.c_str() 
	     << std::endl;
    }
  }
  catch (const CosNaming::NamingContext::CannotProceed & ) 
  {
    os << "Unexpected Error (CannotProceed exception)!" << std::endl;
  }
  catch (const CdmwNamingAndRepository::NoNameDomain & ) 
  {
    os << "Trace Collector Name Domain does not exist \n"
	   << collector_path.c_str() 
	   << std::endl;
  }
  catch (const CdmwNamingAndRepository::InvalidName &) 
  {
    os << "Name has illegal form \n<" << collector_path.c_str() << ">!" << std::endl;
  }
  catch (const CosNaming::NamingContext::InvalidName & )
  {
    os << "Invalid Name \n<" << collector_path.c_str() << ">!" << std::endl;
  } 
  catch (const Cdmw::Common::TypeMismatchException & e) 
  {
    os << "TypeMismatchException <" 
	   << e.what() << ">" << std::endl;
  }
  catch (const Cdmw::Exception &e ) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }


  return result;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::get_local_collector_serv (const std::string &collectorName, 
				                                      bool check_serv, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
  
  Cdmw::CommonSvcs::Naming::NamingInterface ni_root(m_adminRootContext.in());
  
  std::string full_path;
  
  try 
  {
      // local trace collector location is:
      // <hostname>/SERVICES/TRACE/COLLECTORS/collectorName
        
      CosNaming::Name collectorName_;
      collectorName_.length(5);
      collectorName_[0].id = CORBA::string_dup (m_local_collector_context.c_str());
      collectorName_[1].id = CORBA::string_dup("SERVICES");
      collectorName_[2].id = CORBA::string_dup("TRACE");
      collectorName_[3].id = CORBA::string_dup("COLLECTORS");
      collectorName_[4].id = CORBA::string_dup(collectorName.c_str());

      full_path = ni_root.to_string(collectorName_);
      
      CORBA::Object_var obj = ni_root.resolve(full_path);
      m_local_collectorRef = CdmwTrace::Collector::_narrow(obj.in());
      
      result = OP_SUCCESS;
  }
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const CosNaming::NamingContext::NotFound&) 
  {
      std::cerr << "The local trace collector "
                << full_path
                << " could not be found in the repository."
                << std::endl;
  }
  catch (const Cdmw::Common::TypeMismatchException & e) 
  {
    os << "TypeMismatchException <" 
	   << e.what() << ">" << std::endl;
  }
  catch (const Cdmw::Exception &e ) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  
  return result;
}


//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::get_producer_serv (const std::string &processName, 
				                               bool check_serv, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
  
  std::string producer_context_name;        

  try 
  {
    // set TraceProducer location under admin root context
    // Pattern is :
    //   "[<host_name>/<application_name>/<process_name>]/TRACE/TraceProducer"
         				
    // Build the complete path of the Trace Producer object
	producer_context_name = m_producer_context;
	producer_context_name += "/";
	producer_context_name += processName;
	producer_context_name += "/TRACE/TraceProducer";
  
	// NamingInterface on Admin Root context
    Cdmw::CommonSvcs::Naming::NamingInterface ni_root(m_adminRootContext.in());
    
	// get TraceProducer object using Root context NamingInterface
    typedef Cdmw::CommonSvcs::Naming::NamingUtil<CdmwTrace::TraceProducer> Util;
	m_traceProducerRef = Util::resolve_name(ni_root,producer_context_name);

	if (CORBA::is_nil(m_traceProducerRef.in()))
    {
      // print error message only if call is to get and not to check object
      if (check_serv == false)
      {
        os << "Producer Name Resolve Error \n" << std::endl
           << producer_context_name.c_str()
		   << std::endl;
	  }
	}
	else
	{	
	  result = OP_SUCCESS;
    }
  } 
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const CosNaming::NamingContext::NotFound &)
  {
    // print error message only if call is to get and not to check object
    if (check_serv == false)
    {
      os << "Trace Producer Name not found \n"
	     << producer_context_name.c_str() 
	     << std::endl;
    }
  }
  catch (const CosNaming::NamingContext::CannotProceed & ) 
  {
    os << "Unexpected Error (CannotProceed exception)!" << std::endl;
  }
  catch (const CdmwNamingAndRepository::NoNameDomain & ) 
  {
    os << "Trace Producer Name Domain does not exist \n"
	   << producer_context_name.c_str() 
	   << std::endl;
  }
  catch (const CdmwNamingAndRepository::InvalidName &) 
  {
    os << "Name has illegal form \n<" << producer_context_name.c_str() << ">!" << std::endl;
  }
  catch (const CosNaming::NamingContext::InvalidName & )
  {
    os << "Invalid Name \n<" << producer_context_name.c_str() << ">!" << std::endl;
  } 
  catch (const Cdmw::Common::TypeMismatchException & e) 
  {
    os << "TypeMismatchException <" 
	   << e.what() << ">" << std::endl;
  }
  catch (const Cdmw::Exception &e ) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }


  return result;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::activ_global_collector_level (const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = process_level(true, GLOBAL_COLLECTOR, arg, os);
  return result;
}

CORBA::Long TraceAdminTool::deactiv_global_collector_level (const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = process_level(false, GLOBAL_COLLECTOR, arg, os);
  return result;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::activ_local_collector_level (const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = process_level(true, LOCAL_COLLECTOR, arg, os);
  return result;
}

CORBA::Long TraceAdminTool::deactiv_local_collector_level (const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = process_level(false, LOCAL_COLLECTOR, arg, os);
  return result;
}


CORBA::Long TraceAdminTool::activ_producer_level (const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = process_level(true, PRODUCER, arg, os);
  return result;
}

CORBA::Long TraceAdminTool::deactiv_producer_level (const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = process_level(false, PRODUCER, arg, os);
  return result;
}

CORBA::Long TraceAdminTool::process_level(bool activation, int serv_type,
				                          const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;

  std::string servant_name;
  std::string domain;
  CdmwTrace::Value level_value;
  // ECR-0123
  std::string componentName;

  try 
  {
    // extract servant name
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();

    // find end of servant name
    std::string::const_iterator j = std::find_if(i,end,isspace);

    // copy the characters in [i,j)
    if (i != j)
      servant_name = std::string(i,j);

    if (servant_name == "")
    {
      os << "no value for servant name" << std::endl;
      return OP_FAILURE;
    }

    // ECR-0123
    componentName = CdmwTrace::ALL_COMPONENT_NAMES;
    domain = CdmwTrace::ALL_DOMAINS;
    level_value = CdmwTrace::ALL_VALUES;

    // ECR-0123
    // this command expects up to 3 arguments
    int max_nb_args = 3;
    int nb_args = 0;
    std::string args[max_nb_args];
    for (int k = 0; k < max_nb_args; ++k) {
        if (j != end) {
            // skip leading whitespaces
            i = std::find_if(j, end, not_space);
            // position of the next whitespace from the current position
            j = std::find_if(i, end, isspace);

            // copy the word
            if (i != j) {
                args[k] = std::string(i, j);
                nb_args++;
            }
        }
    }

    int componentName_arg_idx = -1;
    int domain_arg_idx = -1;
    int level_arg_idx = -1;

    if (nb_args == 3) {
        // arguments: component name/domain/level
        componentName_arg_idx = 0;
        domain_arg_idx = 1;
        level_arg_idx = 2;
    } else if (nb_args == 2) {
        // arguments: domain/level
        domain_arg_idx = 0;
        level_arg_idx = 1;
    } else if (nb_args == 1) {
        // argument: domain
        domain_arg_idx = 0;
    }

    if (componentName_arg_idx != -1) {
        componentName = args[componentName_arg_idx];
    }

    if (domain_arg_idx != -1) {
        domain = args[domain_arg_idx];
    }

    if (level_arg_idx != -1) {
        if (args[level_arg_idx] == "*") {
            level_value = -1;
        }
        else {
            std::istringstream level_strm(args[level_arg_idx]);
            level_strm >> level_value;
            if (level_strm.fail()) {
                os << "bad value for domain level" << std::endl;
                return OP_FAILURE;
            }
        }
    }
    

    
    if (serv_type == GLOBAL_COLLECTOR)
    {
	  CORBA::Long err_retn = get_global_collector_serv (servant_name, false, os);
	  if (err_retn == OP_FAILURE)
      {
        return OP_FAILURE;
	  }

        // ECR-0123
        if (activation) {
	        // activate the level in collector
	        m_global_collectorRef->activate_level(componentName.c_str(),
                                           domain.c_str(),
                                           level_value);
        }
	    else {
	        // deactivate the level in collector
	        m_global_collectorRef->deactivate_level(componentName.c_str(),
                                             domain.c_str(),
                                             level_value);
        }
    }
    else if (serv_type == LOCAL_COLLECTOR)
    {
	  CORBA::Long err_retn = get_local_collector_serv (servant_name, false, os);
	  if (err_retn == OP_FAILURE)
      {
        return OP_FAILURE;
	  }

        // ECR-0123
        if (activation) {
	        // activate the level in collector
	        m_local_collectorRef->activate_level(componentName.c_str(),
                                                 domain.c_str(),
                                                 level_value);
        }
	    else {
	        // deactivate the level in collector
	        m_local_collectorRef->deactivate_level(componentName.c_str(),
                                                   domain.c_str(),
                                                   level_value);
        }
    }
	else
    {
	  CORBA::Long err_retn = get_producer_serv (servant_name, false, os);
	  if (err_retn == OP_FAILURE)
      {
        return OP_FAILURE;
	  }

        // ECR-0123
        if (activation) {
	        // activate the level in producer
	        m_traceProducerRef->activate_level(componentName.c_str(),
                                               domain.c_str(),
                                               level_value);
        }
	    else {
	        // deactivate the level in producer
	        m_traceProducerRef->deactivate_level(componentName.c_str(),
                                                 domain.c_str(),
                                                 level_value);
        }
    }

	result = OP_SUCCESS;
  } 
  
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const Cdmw::Exception &e ) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }

  return result;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::get_global_collector_levels (const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = get_levels(GLOBAL_COLLECTOR, arg, os);
  return result;
}

CORBA::Long TraceAdminTool::get_local_collector_levels (const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = get_levels(LOCAL_COLLECTOR, arg, os);
  return result;
}

CORBA::Long TraceAdminTool::get_producer_levels (const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = get_levels(PRODUCER, arg, os);
  return result;
}




CORBA::Long TraceAdminTool::get_levels(int serv_type, const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;

  std::string servant_name;

  try
  {
    // extract servant name
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();

    // find end of servant name
    std::string::const_iterator j = std::find_if(i,end,isspace);

    // copy the characters in [i,j)
    if (i != j)
      servant_name = std::string(i,j);

    if (servant_name == "")
    {
      os << "no value for servant name" << std::endl;
      return OP_FAILURE;
    }
	
	
    // get servant reference 
    if (serv_type == GLOBAL_COLLECTOR)
    {
	  CORBA::Long err_retn = get_global_collector_serv (servant_name, false, os);
	  if (err_retn == OP_FAILURE)
      {
        return OP_FAILURE;
	  }
    }
    else if (serv_type == LOCAL_COLLECTOR)
    {
	  CORBA::Long err_retn = get_local_collector_serv (servant_name, false, os);
	  if (err_retn == OP_FAILURE)
      {
        return OP_FAILURE;
	  }
    }
	else
    {
	  CORBA::Long err_retn = get_producer_serv (servant_name, false, os);
	  if (err_retn == OP_FAILURE)
      {
        return OP_FAILURE;
	  }
    }
  
 


    // ECR-0123
    std::string title_component_str = "component name             ";
    std::string component_str = title_component_str;
    std::string title_name_str = "domain name             ";
    std::string name_str = title_name_str;
    std::string title_level_str = "level    ";
    std::string level_str = title_level_str;
    std::string title_activ_str = "activation";
    std::string activ_str;

    // ECR-0123
    int field0_maxsize = component_str.size() - 1;
    int field1_maxsize = name_str.size() - 1;
    int field2_maxsize = level_str.size() - 1;

    CdmwTrace::TraceFilterSeq_var filterSeq;

    // get sequence of trace filter
 
    if (serv_type == GLOBAL_COLLECTOR)
    {
      filterSeq = m_global_collectorRef->get_trace_filters ();
    }
    else if (serv_type == LOCAL_COLLECTOR)
    {
      filterSeq = m_local_collectorRef->get_trace_filters ();
    }
	else
    {
      filterSeq = m_traceProducerRef->get_trace_filters ();
    }
  
    
    os << "list of trace filters" << std::endl; 
    os << "---------------------" << std::endl; 

    // ECR-0123
    os << title_component_str.c_str()
       << title_name_str.c_str()
       << title_level_str.c_str()
       << title_activ_str.c_str() << std::endl; 

    for (unsigned int i=0 ; i < filterSeq->length() ; i++)
    {
        // ECR-0123
        std::string componentName = (*filterSeq)[i].the_component_name.in();
        int componentName_size = componentName.size();
        if (componentName_size > field0_maxsize) 
        {
            // drop the end characters that do not fit into the field
            componentName_size = field0_maxsize;
        }
        
        component_str.replace(0,
                              componentName_size,
                              componentName,
                              0,
                              componentName_size);
                              
        if (componentName_size < field0_maxsize) {
            int nspace = field0_maxsize - componentName_size;
            // fill the field with whitespaces
            component_str.replace(componentName_size, nspace, nspace, ' ');
        }


      std::string domain_name = (*filterSeq)[i].the_domain.in();
      int name_size = domain_name.size();

	  if (name_size > field1_maxsize)
	  {
        name_size = field1_maxsize;
	  }
	
	  name_str.replace(0,name_size,domain_name,0,name_size);

	  if (name_size < field1_maxsize)
	  {
	    int nspace = field1_maxsize - name_size;
	    name_str.replace(name_size,nspace,nspace,' ');
      }

	  
      int level_value = (*filterSeq)[i].the_value;
      int field2_size;
      
	  if (level_value == -1)
      {
        level_str.replace(0,1,1,'*');
        field2_size = 1;
      }
	  else
      {
        std::ostringstream level_strm;
        level_strm << level_value;
        
        int level_size = level_strm.str().size();
        field2_size = level_size;
        if (field2_size > field2_maxsize)
        {
          field2_size = field2_maxsize;
        }
    
        level_str.replace(0,field2_size,level_strm.str(),0,level_size);
	  }
	  
	  if (field2_size < field2_maxsize)
	  {
	    int nspace = field2_maxsize - field2_size;
	    level_str.replace(field2_size,nspace,nspace,' ');
      }
      
      
      if ((*filterSeq)[i].activation == true)
      {
        activ_str = "yes"; 
      }
      else
      {
        activ_str = "no";
      }
    
	  // print info : component name, domain , level, activation
      // ECR-0123
	  os << component_str.c_str()
         << name_str.c_str()
         << level_str.c_str()
         << activ_str.c_str() << std::endl;
    }

    os << std::endl; 

    result = OP_SUCCESS;
  } 
  
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const Cdmw::Exception &e ) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }

  return result;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::register_local_collector(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
  CORBA::Long err_retn;

  std::string producer_process_name;
  std::string collector_name;
  std::string collector_mnemonic;

  try 
  {
    // extract producer process name
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();

    // find end of process name
    std::string::const_iterator j = std::find_if(i,end,isspace);

    // copy the characters in [i,j)
    if (i != j)
      producer_process_name = std::string(i,j);

    if (producer_process_name == "")
    {
      os << "no value for producer process name" << std::endl;
      return OP_FAILURE;
    }
		
    if (j != end)
    {
      // extract collector name to register
      // skip leading whitespaces
      i = std::find_if(j,end,not_space);

      // find end of name
      j = std::find_if(i,end,isspace);

      // copy the characters in [i,j)
      if (i != j)
        collector_name = std::string(i,j);
    }
		
    if (j != end)
    {
      // extract collector mnemonic
      // skip leading whitespaces
      i = std::find_if(j,end,not_space);

      // find end of mnemonic
      j = std::find_if(i,end,isspace);

      // copy the characters in [i,j)
      if (i != j)
        collector_mnemonic = std::string(i,j);
    }

    if (collector_name == "")
    {
      os << "no value for collector name" << std::endl;
      return OP_FAILURE;
    }
		
    if (collector_mnemonic == "")
    {
      os << "no value for collector mnemonic" << std::endl;
      return OP_FAILURE;
    }

    // find trace producer servant
    err_retn = get_producer_serv (producer_process_name, false, os);
	if (err_retn == OP_FAILURE)
    {
      return OP_FAILURE;
	}
	
	// find local trace collector servant
    err_retn = get_local_collector_serv (collector_name, false, os);
	if (err_retn == OP_FAILURE)
    {
      return OP_FAILURE;
	}
    
	// register the collector servant in trace producer
	m_traceProducerRef->register_collector(m_local_collectorRef.in(), 
					                             collector_name.c_str(), 
										         collector_mnemonic.c_str());

	result = OP_SUCCESS;
  }  
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const Cdmw::Exception &e ) 
  {
	PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
  	PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }

  return result;
}



CORBA::Long TraceAdminTool::unregister_local_collector(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;

  std::string producer_process_name;
  std::string collector_ident_str;

  try 
  {
    // extract producer servant name
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();

    // find end of servant name
    std::string::const_iterator j = std::find_if(i,end,isspace);

    // copy the characters in [i,j)
    if (i != j)
      producer_process_name = std::string(i,j);

    if (producer_process_name == "")
    {
      os << "no value for producer process name" << std::endl;
      return OP_FAILURE;
    }
		
	
    if (j != end)
    {
      // extract collector ident
      // skip leading whitespaces
      i = std::find_if(j,end,not_space);

      // find end of ident
      j = std::find_if(i,end,isspace);

      // copy the characters in [i,j)
      if (i != j)
        collector_ident_str = std::string(i,j);
    }
		
    if (collector_ident_str == "")
    {
      os << "no value for collector ident" << std::endl;
      return OP_FAILURE;
    }
		
    // extract collector ident
    CdmwTrace::TraceProducer::Ident collector_ident;
  
    std::istringstream ident_strm(collector_ident_str);
    ident_strm >> collector_ident;
    if (ident_strm.fail())
    {
      os << "collector ident is not valid" << std::endl;
      return OP_FAILURE;
	}

    // find trace producer servant
    CORBA::Long err_retn = get_producer_serv (producer_process_name, false, os);
	if (err_retn == OP_FAILURE)
    {
      return OP_FAILURE;
	}
	
	// unregister the collector servant in trace producer
	m_traceProducerRef->unregister_collector(collector_ident);

	result = OP_SUCCESS;
  }  
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const Cdmw::Exception &e ) 
  {
	PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
  	PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }

  return result;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::get_registered_local_collectors(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;

  std::string producer_process_name;

  try
  {
    // extract producer servant name
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();

    // find end of servant name
    std::string::const_iterator j = std::find_if(i,end,isspace);

    // copy the characters in [i,j)
    if (i != j)
      producer_process_name = std::string(i,j);

    if (producer_process_name == "")
    {
      os << "no value for producer process name" << std::endl;
      return OP_FAILURE;
    }
  
    // find trace producer servant
    CORBA::Long err_retn = get_producer_serv (producer_process_name, false, os);
	if (err_retn == OP_FAILURE)
    {
      return OP_FAILURE;
	}
	
    // get sequence of collectors from trace producer servant
    CdmwTrace::TraceProducer::RegistrationSeq_var collectorSeq = 
		                    m_traceProducerRef->get_registered_collectors();
  
    std::string mnemo_name_str = "mnemonic name          ";
    std::string obj_name_str   = "object name            ";
    int field1_size = mnemo_name_str.size();
    int field2_size = obj_name_str.size();

    os << "list of registered local collectors" << std::endl; 
    os << "-----------------------------------" << std::endl; 
    os << mnemo_name_str.c_str() << obj_name_str.c_str() << "identifier" << std::endl; 

    for (unsigned int i=0 ; i < collectorSeq->length() ; i++)
    {
      int name_size;

      std::string collector_mnemo_name = (*collectorSeq)[i].collectorMnemoName.in();
      name_size = collector_mnemo_name.size();

	  if (name_size > field1_size)
	  {
        name_size = field1_size;
	  }
	
	  mnemo_name_str.replace(0,name_size,collector_mnemo_name,0,name_size);

	  if (name_size < field1_size)
	  {
	    int nspace = field1_size - name_size;
	    mnemo_name_str.replace(name_size,nspace,nspace,' ');
      }

      std::string collector_obj_name = (*collectorSeq)[i].collectorObjName.in();
      name_size = collector_obj_name.size();

	  if (name_size > field2_size)
	  {
        name_size = field2_size;
	  }
	
	  obj_name_str.replace(0,name_size,collector_obj_name,0,name_size);

	  if (name_size < field2_size)
	  {
	    int nspace = field2_size - name_size;
	    obj_name_str.replace(name_size,nspace,nspace,' ');
      }

      os << mnemo_name_str.c_str() << obj_name_str.c_str()
		 << (*collectorSeq)[i].collectorId << std::endl; 
    }

    os << std::endl; 

	result = OP_SUCCESS;
  } 
  
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const Cdmw::Exception &e ) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }


  return result;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::get_global_collectors(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
  
  try
  {
    // set TraceCollector location under default root context
    // Pattern is :
    //   "CDMW/SERVICES/TRACE/COLLECTORS/<collectorName>"
    
	// NamingInterface on Root context
    Cdmw::CommonSvcs::Naming::NamingInterface ni_root(m_defaultRootContext.in());
    
	// create Collector domain naming context
	CosNaming::NamingContext_var collector_nc = CosNaming::NamingContext::_nil();
	
    typedef CommonSvcs::Naming::NamingUtil<CosNaming::NamingContext> Util;
	collector_nc = Util::resolve_name (ni_root,m_global_collector_context);

	if (CORBA::is_nil(collector_nc.in()))
    {
      os << "Collector Name Context Resolve Error \n"
	     << m_global_collector_context.c_str() 
		 << std::endl;
	}
	else
	{	
	  // NamingInterface on Producer context
	  CommonSvcs::Naming::NamingInterface ni(collector_nc.in());
            
	  CosNaming::BindingIterator_var bindingItr;
	  CosNaming::BindingList_var bindingList;
            
      os << "list of global trace collectors" << std::endl; 
      os << "-------------------------------" << std::endl;
               
      // All binding in this list are Collectors, so we try to get all
      ni.list (CommonSvcs::Naming::NamingInterface::LIST_CHUNK_SIZE, bindingList, bindingItr);
            
      // We stop when there is no more binding
      bool isMoreBinding = false;
            
      do 
      {            
        for (size_t iBinding=0 ; iBinding < bindingList->length() ; iBinding++) 
        {              
		  CosNaming::Binding binding = bindingList[iBinding];
                    
          // get collector name
		  std::string collector_name = ni.to_string (binding.binding_name);  
        
          os << collector_name.c_str() << std::endl;                  
        }

        // If the iterator is not NULL, there is more binding to get back
        if (!CORBA::is_nil(bindingItr.in())) 
        {
          isMoreBinding = 
               bindingItr->next_n(CommonSvcs::Naming::NamingInterface::LIST_CHUNK_SIZE, 
                                  bindingList);
        }          
      } while (isMoreBinding);
  
      os << std::endl;
    
  	  result = OP_SUCCESS;
  	}
  } 
  
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const CosNaming::NamingContext::NotFound &)
  {
    std::cerr << "Trace Collector Name Domain not found \n"
	          << m_global_collector_context.c_str() 
			  << std::endl;
  }
  catch (const CosNaming::NamingContext::CannotProceed & ) 
  {
    os << "Unexpected Error (CannotProceed exception)!" << std::endl;
  }
  catch (const CdmwNamingAndRepository::NoNameDomain & ) 
  {
    std::cerr << "Trace Collector Name Domain does not exist \n"
	          << m_global_collector_context.c_str() 
			  << std::endl;
  }
  catch (const CdmwNamingAndRepository::InvalidName &) 
  {
    std::cerr << "Trace Collector Name Domain has illegal form \n"
	          << m_global_collector_context.c_str() 
			  << std::endl;
  }
  catch (const CosNaming::NamingContext::InvalidName & )
  {
    os << "Trace Collector Domain invalid  \n<" << m_global_collector_context.c_str() 
	   << ">!" << std::endl;
  } 
  catch (const Cdmw::Common::TypeMismatchException & e) 
  {
    os << "TypeMismatchException (Not a NamingContext) <" 
	   << e.what() << ">" << std::endl;
  }
  catch (const Cdmw::Exception &e ) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }


  return result;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::get_local_collectors(const std::string &arg, std::ostream &os)
{
  using namespace CosNaming;

  CORBA::Long result = OP_FAILURE;
  
  // return if local trace collector context undefined
  if (m_local_collector_context == "")
  {
    std::cerr << "Local Trace Collector Name Context is undefined \n -> use set_producer_context command"
			  << std::endl;
			  
    return result;
  }

  try
  {
    // set TraceCollector location under admin root context
    // Pattern is :
    //   "<hostname>/SERVICES/TRACE/COLLECTORS/<collectorName>"
         				
	// NamingInterface on Admin Root context
    Cdmw::CommonSvcs::Naming::NamingInterface ni_root(m_adminRootContext.in());
    
	// create TraceCollector domain naming context
	CosNaming::NamingContext_var collector_nc = CosNaming::NamingContext::_nil();
	
	CosNaming::Name collectorName_;
    collectorName_.length(4);
    collectorName_[0].id = CORBA::string_dup (m_local_collector_context.c_str());
    collectorName_[1].id = CORBA::string_dup("SERVICES");
    collectorName_[2].id = CORBA::string_dup("TRACE");
    collectorName_[3].id = CORBA::string_dup("COLLECTORS");
    
    typedef CommonSvcs::Naming::NamingUtil<CosNaming::NamingContext> Util;
	collector_nc = Util::resolve_name (ni_root,ni_root.to_string(collectorName_));
	  
	if (CORBA::is_nil(collector_nc.in()))
    {
      os << "Local Trace Collector Name Context Resolve Error" << std::endl;
	}
	else
	{	
	  // NamingInterface on TraceCollector context
	  CommonSvcs::Naming::NamingInterface ni(collector_nc.in());
            
	  CosNaming::BindingIterator_var bindingItr;
	  CosNaming::BindingList_var bindingList;
            
      os << "list of local trace collectors" << std::endl; 
      os << "------------------------------" << std::endl;
               
      // All binding in this list are TraceCollector, so we try to get all
      ni.list (CommonSvcs::Naming::NamingInterface::LIST_CHUNK_SIZE, bindingList, bindingItr);
            
      // We stop when there is no more binding
      bool isMoreBinding = false;
            
      do 
      {            
        for (size_t iBinding=0 ; iBinding < bindingList->length() ; iBinding++) 
        {              
		  CosNaming::Binding binding = bindingList[iBinding];
		  
		  // get collector name
		  std::string collector_name = ni.to_string (binding.binding_name);  
        
          os << collector_name.c_str() << std::endl;
        }

        // If the iterator is not NULL, there is more binding to get back
        if (!CORBA::is_nil(bindingItr.in())) 
        {
          isMoreBinding = 
               bindingItr->next_n(CommonSvcs::Naming::NamingInterface::LIST_CHUNK_SIZE, 
                                  bindingList);
        }          
      } while (isMoreBinding);
  
      os << std::endl;
    
  	  result = OP_SUCCESS;
  	}
  } 
  
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const CosNaming::NamingContext::NotFound &)
  {
    std::cerr << "Trace Producer Name Context not found \n"
	          << m_producer_context.c_str() 
			  << std::endl;
  }
  catch (const CosNaming::NamingContext::CannotProceed & ) 
  {
    os << "Unexpected Error (CannotProceed exception)!" << std::endl;
  }
  catch (const CdmwNamingAndRepository::NoNameDomain & ) 
  {
    std::cerr << "Trace Producer Name Domain does not exist \n"
	          << m_producer_context.c_str() 
			  << std::endl;
  }
  catch (const CdmwNamingAndRepository::InvalidName &) 
  {
    std::cerr << "Trace Producer Name Context has illegal form \n"
	          << m_producer_context.c_str() 
			  << std::endl;
  }
  catch (const CosNaming::NamingContext::InvalidName & )
  {
    os << "Trace Producer Name invalid  \n<" << m_producer_context.c_str()
	   << ">!" << std::endl;
  } 
  catch (const Cdmw::Common::TypeMismatchException & e) 
  {
    os << "TypeMismatchException (Not a NamingContext) <" 
	   << e.what() << ">" << std::endl;
  }
  catch (const Cdmw::Exception &e ) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }

  return result;
}

//----------------------------------------------------------------------------
CORBA::Long TraceAdminTool::get_producers(const std::string &arg, std::ostream &os)
{
  using namespace CosNaming;

  CORBA::Long result = OP_FAILURE;
  
  // return if producer context undefined
  if (m_producer_context == "")
  {
    std::cerr << "Trace Producer Name Context is undefined \n -> use set_producer_context command"
			  << std::endl;
    return result;
  }

  try
  {
    // set TraceProducer location under admin root context
    // Pattern is :
    //   "<host>/<appli_name>/<process_name>/TRACE/TraceProducer"
         				
	// NamingInterface on Admin Root context
    Cdmw::CommonSvcs::Naming::NamingInterface ni_root(m_adminRootContext.in());
    
	// create Producer domain naming context
	CosNaming::NamingContext_var producer_nc = CosNaming::NamingContext::_nil();
	
    typedef CommonSvcs::Naming::NamingUtil<CosNaming::NamingContext> Util;
	producer_nc = Util::resolve_name (ni_root,m_producer_context);
	  
	if (CORBA::is_nil(producer_nc.in()))
    {
      os << "Producer Name Context Resolve Error" << std::endl;
	}
	else
	{	
	  // NamingInterface on Producer context
	  CommonSvcs::Naming::NamingInterface ni(producer_nc.in());
            
	  CosNaming::BindingIterator_var bindingItr;
	  CosNaming::BindingList_var bindingList;
            
      os << "list of trace producers process" << std::endl; 
      os << "-------------------------------" << std::endl;
               
      // All binding in this list are Producers, so we try to get all
      ni.list (CommonSvcs::Naming::NamingInterface::LIST_CHUNK_SIZE, bindingList, bindingItr);
            
      // We stop when there is no more binding
      bool isMoreBinding = false;
            
      do 
      {            
        for (size_t iBinding=0 ; iBinding < bindingList->length() ; iBinding++) 
        {              
		  CosNaming::Binding binding = bindingList[iBinding];
                    
          // get process name of producer
          std::string process_name = ni.to_string (binding.binding_name);  
        
		  // check the producer servant object
	      CORBA::Long err_retn = get_producer_serv (process_name, true, os);
		  // if object found print its process name
	      if (err_retn == OP_SUCCESS)
          {
            os << process_name.c_str() << std::endl;                  
          }
        }

        // If the iterator is not NULL, there is more binding to get back
        if (!CORBA::is_nil(bindingItr.in())) 
        {
          isMoreBinding = 
               bindingItr->next_n(CommonSvcs::Naming::NamingInterface::LIST_CHUNK_SIZE, 
                                  bindingList);
        }          
      } while (isMoreBinding);
  
      os << std::endl;
    
  	  result = OP_SUCCESS;
  	}
  } 
  
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
						os);
  }
  catch (const CosNaming::NamingContext::NotFound &)
  {
    std::cerr << "Trace Producer Name Context not found \n"
	          << m_producer_context.c_str() 
			  << std::endl;
  }
  catch (const CosNaming::NamingContext::CannotProceed & ) 
  {
    os << "Unexpected Error (CannotProceed exception)!" << std::endl;
  }
  catch (const CdmwNamingAndRepository::NoNameDomain & ) 
  {
    std::cerr << "Trace Producer Name Domain does not exist \n"
	          << m_producer_context.c_str() 
			  << std::endl;
  }
  catch (const CdmwNamingAndRepository::InvalidName &) 
  {
    std::cerr << "Trace Producer Name Context has illegal form \n"
	          << m_producer_context.c_str() 
			  << std::endl;
  }
  catch (const CosNaming::NamingContext::InvalidName & )
  {
    os << "Trace Producer Name invalid  \n<" << m_producer_context.c_str()
	   << ">!" << std::endl;
  } 
  catch (const Cdmw::Common::TypeMismatchException & e) 
  {
    os << "TypeMismatchException (Not a NamingContext) <" 
	   << e.what() << ">" << std::endl;
  }
  catch (const Cdmw::Exception &e ) 
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::SystemException &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }
  catch (const CORBA::Exception &e)
  {
    PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
  }

  return result;
}
//----------------------------------------------------------------------------

}; // namespace TraceAdmin
}; // namespace Cdmw

