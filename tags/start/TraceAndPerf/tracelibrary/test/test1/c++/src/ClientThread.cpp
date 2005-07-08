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


#include "test1/ClientThread.hpp"
#include <string>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>


#include "Foundation/common/System.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "Repository/naminginterface/NamingInterface.hpp"
#include "Repository/naminginterface/NamingUtil.hpp"

#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.stub.hpp"
#include "TraceAndPerf/tracelibrary/Trace.hpp"
#include "TraceAndPerf/tracelibrary/FlushAreaMngr.hpp"

#include "test1/TargetThread.hpp"
#include "test1/TargetThread1.hpp"
#include "test1/TargetThread2.hpp"

#define ECHO_TRACELIB_TEST if (m_verbose) os 

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

namespace
{
    const char* INFO_MESSAGE = "Type h or help for a list of available commands.";
    const char* PROMPT_MESSAGE = "\n%>";
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
namespace Trace
{

//----------------------------------------------------------------------------
ClientThread::ClientThread(CORBA::ORB_ptr orb, 
                           std::istream & is,
                           std::ostream & os,
						   CdmwNamingAndRepository::Repository_ptr rep)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_verbose(false),
      m_istream(is),
      m_ostream(os),
      m_rootContext(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT),
	  m_repository(CdmwNamingAndRepository::Repository::_duplicate(rep)),
	  m_pTarget(NULL)
{
    try
    {
      CORBA::Object_var objref;
	  char *p_file_name = "traceLibrary.ior";

	  // import the object reference from the file
	  std::ifstream is (p_file_name);
	  if (is.good())
	  {
	    char objref_text[5000];
	    is >> objref_text;
	    objref = m_orb->string_to_object(objref_text);

	    is.close();
	  }
	  else
	  {
        os << "File : " << __FILE__ << " Line : " << __LINE__
	       << "error opening reference filename :" << p_file_name << std::endl;
	  }
	
	  // get the TraceProducer proxy
	  m_traceProducer = CdmwTrace::TraceProducer::_narrow(objref.in());

	  if (CORBA::is_nil(m_traceProducer.in()))
	  {
	    os << "File : " << __FILE__ << " Line : " << __LINE__
	       << "could not _narrow object to type TraceProducer" << std::endl;
	  }


	  // initialize trace collector context
	  // Pattern is :
	  //    CDMW/SERVICES/TRACE/COLLECTORS/
	  m_collector_context = Common::Locations::CDMW_SERVICES_NAME_DOMAIN;
	  m_collector_context	+= "/";
	  m_collector_context	+= Common::Locations::TRACE_COLLECTORS_NAME_DOMAIN;

    } 
  
    catch (const Cdmw::Exception &e ) 
    {
	  PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
      CDMW_ASSERT(0);
    }
    catch (const CORBA::SystemException &e)
    {
	  PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
      CDMW_ASSERT(0);
    }
    catch (const CORBA::Exception &e)
    {
      PRINT_EXCEPTION(__FILE__,__LINE__,e,os);
      CDMW_ASSERT(0);
    }
}

//----------------------------------------------------------------------------
ClientThread::~ClientThread() throw()
{
    if (m_pTarget != NULL)
    {
      // delete target thread 2
      delete m_pTarget;
    }
}

//----------------------------------------------------------------------------
void ClientThread::run()
  throw()
{
    run (m_istream,m_ostream);
}

//----------------------------------------------------------------------------
const CORBA::ULong ClientThread::nb_commands = 20;
const ClientThread::command_def ClientThread::commands[] =  // should be sorted alphabetically (by operation name)
{ 
    { "activate_level"        , &ClientThread::activate_level              },  
    { "deactivate_level"      , &ClientThread::deactivate_level            },  
    { "exit"                  , &ClientThread::exit                        },
	{ "get_collectors"        , &ClientThread::get_registered_collectors   },
    { "get_levels"            , &ClientThread::get_levels                  },
    { "h"                     , &ClientThread::print_help                  },
    { "help"                  , &ClientThread::print_help                  },
    { "register_collector"    , &ClientThread::register_collector          },
	{ "set_collector_context" , &ClientThread::set_collector_context       },
	{ "set_root_context"      , &ClientThread::set_root_context            },
	{ "sleep_process"         , &ClientThread::sleep_process               },
	{ "start_domain_target"   , &ClientThread::start_domain_target         },
	{ "start_flushing"        , &ClientThread::start_flushing              },
	{ "start_stop_targets"    , &ClientThread::start_stop_targets          },
	{ "start_target"          , &ClientThread::start_target                },
	{ "stop_flushing"         , &ClientThread::stop_flushing               },
	{ "stop_target"           , &ClientThread::stop_target                 },
    { "unregister_collector"  , &ClientThread::unregister_collector        },
    { "verbose"               , &ClientThread::toggle_verbose              },
    { "x"                     , &ClientThread::exit                        }
};

    
//----------------------------------------------------------------------------
CORBA::Long ClientThread::runOperation(const std::string &op,
                                       const std::string &arg, 
                                       std::ostream &os)
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
      {
	    // run command
        return (this->*commands[m].op)(arg,os);
	  }
      
	  if(res > 0)
	  {
        r = m;
	  }
      else
	  {
        l = m + 1;
	  }
    }
	
    if(op != "") unknown_command(os);
    
    return OP_FAILURE;
}

//----------------------------------------------------------------------------
void ClientThread::print_info(std::ostream& os)
{
    os << "\n\n"
       << "\t\t---------------------------------------\n"
       << "\t\t  Cdmw Trace Library Test             \n"
       << "\t\t---------------------------------------\n"
       << "\n\n"
       << INFO_MESSAGE
       << "\nInfo: (verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
}

//----------------------------------------------------------------------------
void ClientThread::print_prompt(std::ostream& os)
{
    os << PROMPT_MESSAGE << std::flush;
}

//----------------------------------------------------------------------------
void ClientThread::unknown_command( std::ostream & os)
{
    os << UNKNOWN_COMMAND_MESSAGE << std::endl;
}

//----------------------------------------------------------------------------
void ClientThread::run(std::istream& is,std::ostream& os)
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
        {
          arg = std::string(i,end);
        }
        
		ECHO_TRACELIB_TEST << "@cmd = <" << command << "> @args = <" << arg << ">" << std::endl;


        if (runOperation(command,arg,os) == EXIT_ORDERED) break; 
        print_prompt(os); 
    } 
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::exit(const std::string & arg, std::ostream & os) 
{    
    try 
   	{
        if (m_verbose) os << "Initiating ORB shutdown." << std::endl;
        m_orb->shutdown(false);
    } 
	catch (const CORBA::SystemException & ex) 
	{
        PRINT_EXCEPTION(__FILE__,__LINE__,ex,os);
    }
    return EXIT_ORDERED;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::toggle_verbose(const std::string & arg, std::ostream & os) 
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
		<< " | set_root_context <name>           | set root context for trace collector       |" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | set_collector_context <name>      | set context for trace collector            |" << "\n"
		<< " |                                   | default is CDMW/SERVICES/TRACE/COLLECTORS/|" << "\n"
        << " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | activate_level <domain> <level>   | activate the level    all-> [* -1]         |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | deactivate_level <domain> <level> | deactivate the level                       |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | get_levels                        | get levels                                 |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | register_collector                |                                            |" << "\n"
		<< " |               <name> <mnemonic>   | register the collector                     |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | unregister_collector <ident>      | unregister the collector                   |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | get_collectors                    | get collectors                             |" << "\n"
		<< " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | start_flushing                    | start flushing trace message               |" << "\n"
	    << " +-----------------------------------+--------------------------------------------+" << "\n"
		<< " | stop_flushing                     | stop flushing trace message                |" << "\n"
	    << " +-----------------------------------+--------------------------------------------+" << "\n"
	    << " | start_target                      | start target with trace                    |" << "\n"
	    << " +-----------------------------------+--------------------------------------------+" << "\n"
	    << " | stop_target                       | stop target with trace                     |" << "\n"
	    << " +-----------------------------------+--------------------------------------------+" << "\n"
	    << " | start_stop_targets                | start (and stop) targets with trace        |" << "\n"
	    << " +-----------------------------------+--------------------------------------------+" << "\n"
	    << " | start_domain_target               | start (and stop) target with domain trace  |" << "\n"
	    << " +-----------------------------------+--------------------------------------------+" << "\n"
	    << " | sleep_process <time>              | set process to sleep, time in msec         |" << "\n"
	    << " +-----------------------------------+--------------------------------------------+" << "\n"
        << std::endl;

     return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::set_root_context(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
  const char *p_arg = arg.c_str();
  
  // if undefined context name set defaut root context
  if (*p_arg == '\0')                
  {
    m_rootContext = CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT;
  }
  else
  {
    m_rootContext = p_arg;
  }

  os << "Context set to : " << m_rootContext.c_str() << std::endl;

  try 
  {
    CosNaming::NamingContext_var nc =
            m_repository->resolve_root_context(m_rootContext.c_str());

	result = OP_SUCCESS;
  } 
  catch (const CdmwNamingAndRepository::Repository::NoRootContext & )
  {
    os << "Cannot reach DefaultRootContext!" << std::endl;
  } 

  return result;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::set_collector_context(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
  const char *p_arg = arg.c_str();
	      
  // if undefined context name set defaut context
  if (*p_arg == '\0')                
  {
    // CDMW/SERVICES/TRACE/COLLECTORS/
    m_collector_context = Common::Locations::CDMW_SERVICES_NAME_DOMAIN;
    m_collector_context	+= "/";
    m_collector_context	+= Common::Locations::TRACE_COLLECTORS_NAME_DOMAIN;
  }
  else
  {
    m_collector_context = p_arg;
  }

  os << "collector context set to : " << m_collector_context.c_str() << std::endl;

  result = OP_SUCCESS;

  return result;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::activate_level(const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = process_level(true, arg, os);
  return result;
}

CORBA::Long ClientThread::deactivate_level(const std::string &arg, std::ostream &os)
{
  CORBA::Long result;

  result = process_level(false, arg, os);
  return result;
}


CORBA::Long ClientThread::process_level(const bool activation, 
				                        const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;

  // extract domain
  std::string::const_iterator i = arg.begin();
  std::string::const_iterator end = arg.end();

  // find end of domain
  std::string::const_iterator j = std::find_if(i,end,isspace);

  // copy the characters in [i,j)
  std::string domain;
  if (i != end)
    domain = std::string(i,j);
		
  // extract level
  // skip leading whitespaces
  i = std::find_if(j,end,not_space);
  std::string level_strg;

  // copy level
  CdmwTrace::Value level_value;
  if (i != end)
  {
    level_strg = std::string(i,end);
    std::istringstream level_strm(level_strg);
	level_strm >> level_value;
	if (level_strm.fail())
    {
      os << "bad value for domain level" << std::endl;
      return OP_FAILURE;
    }
  }
  else
  {
    level_value = CdmwTrace::ALL_VALUES;
  }

  try 
  {
    if (activation)
    {
	  // activate the level
	  m_traceProducer->activate_level(domain.c_str(), level_value);
    }
	else
    {
	  // deactivate the level
	  m_traceProducer->deactivate_level(domain.c_str(), level_value);
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
CORBA::Long ClientThread::get_levels(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;

  try
  {
    std::string title_name_str = "domain name             ";
	std::string name_str = title_name_str;
	std::string title_level_str = "level    ";
	std::string level_str = title_level_str;
	std::string title_activ_str = "activation";
	std::string activ_str;
						    
	int field1_maxsize = name_str.size() - 1;
	int field2_maxsize = level_str.size() - 1;

    // get sequence of trace filter
 
    CdmwTrace::TraceFilterSeq_var filterSeq;

    filterSeq = m_traceProducer->get_trace_filters ();
  
    
    os << "list of trace filters" << std::endl; 
    os << "---------------------" << std::endl; 
    
    os << title_name_str.c_str() << title_level_str.c_str() 
       << title_activ_str.c_str() << std::endl; 

    for (unsigned int i=0 ; i < filterSeq->length() ; i++)
    {
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
	    name_str.replace(field2_size,nspace,nspace,' ');
      }
      
      
      if ((*filterSeq)[i].activation == true)
      {
        activ_str = "yes"; 
      }
      else
      {
        activ_str = "no";
      }
    
	  // print info : domain , level, activation
	  os << name_str.c_str() << level_str.c_str() << activ_str.c_str() << std::endl;
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
CORBA::Long ClientThread::register_collector(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;

  // extract collector name
  std::string::const_iterator i = arg.begin();
  std::string::const_iterator end = arg.end();

  // find end of collector name
  std::string::const_iterator j = std::find_if(i,end,isspace);

  // copy the characters in [i,j)
  std::string collector_name;
  if (i != end)
    collector_name = std::string(i,j);
		
  // extract mnemonic
  // skip leading whitespaces
  i = std::find_if(j,end,not_space);
  // find end of mnemonic
  j = std::find_if(i,end,isspace);

  // copy collector mnemonic
  std::string collector_mnemonic;
  if (i != j)
    collector_mnemonic = std::string(i,j);
		
  // find collector servant object reference from repository
  // and call its service
  try 
  {
    // get root naming context
    CosNaming::NamingContext_var nc_root =
            m_repository->resolve_root_context(m_rootContext.c_str());

    if (collector_name == "")
   	{
      os << "Trace Collector name needed." << std::endl;
    } 
	else 
	{        
	  // NamingInterface on Root context
      Cdmw::NamingAndRepository::NamingInterface ni_root(nc_root.in());

      typedef Cdmw::NamingAndRepository::NamingUtil<CdmwTrace::Collector> Util;
            
	  // set complete collector name
	  std::string collector_path = m_collector_context;
	  collector_path += "/";
	  collector_path += collector_name;
	  
	  // get trace collector servant using Root context NamingInterface
	  CdmwTrace::Collector_var traceCollectorRef = Util::resolve_name(ni_root,collector_path);

	  // register the collector servant in trace producer
	  m_traceProducer->register_collector(traceCollectorRef.in(), 
					                      collector_name.c_str(), collector_mnemonic.c_str());
    }

	result = OP_SUCCESS;
  } 
  catch (const std::bad_alloc&)
  {
    PRINT_EXCEPTION_MSG(__FILE__,__LINE__,
					    "Bad Allocation Exception",
					    os);
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



CORBA::Long ClientThread::unregister_collector(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;

  try 
  {
    // extract collector ident
    CdmwTrace::TraceProducer::Ident ident_value;

    if (arg != "")
    {
      std::istringstream ident_strm(arg);
	  ident_strm >> ident_value;
	  if (!ident_strm.fail())
      {
        // unregister the collector
	    m_traceProducer->unregister_collector(ident_value);

	    result = OP_SUCCESS;
      }
    }

    if (result == OP_FAILURE)
    {
      os << "collector ident is not valid" << std::endl;
	}
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
CORBA::Long ClientThread::get_registered_collectors(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
  
  try
  {
    // get sequence of collectors from trace producer servant
    CdmwTrace::TraceProducer::RegistrationSeq_var collectorSeq = 
		                    m_traceProducer->get_registered_collectors ();
  
    std::string mnemo_name_str = "mnemonic name          ";
    std::string obj_name_str   = "object name            ";
    int field1_size = mnemo_name_str.size();
    int field2_size = obj_name_str.size();

    os << "list of registered collectors" << std::endl; 
    os << "-----------------------------" << std::endl; 
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
CORBA::Long ClientThread::start_flushing(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_SUCCESS;

  CDMW_TRACE_ACTIVE_FLUSHING();

  return result;
}

CORBA::Long ClientThread::stop_flushing(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_SUCCESS;

  Cdmw::Trace::FlushAreaMngr::instance().deactivate_flushing();

  return result;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::start_stop_targets(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
   
  try
  {
    // create 2 target threads
    Trace::TargetThread target1;
    Trace::TargetThread target2;
  
    // start clients which stop automatically
    target1.start();
    target2.start();

    // wait end of targets
    target1.join();
    target2.join();

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
CORBA::Long ClientThread::start_domain_target(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
   
  try
  {
    // create target thread 1
    Trace::TargetThread1 target;
  
    // start client which stop automatically
    target.start();

    // wait end of targets
    target.join();

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
CORBA::Long ClientThread::start_target(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
   
  try
  {
    // create target thread 2
    m_pTarget = new TargetThread2;
  
    // start target 
    m_pTarget->start();

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
  int timescale = Cdmw::TestUtils::Testable::get_timescale();
  OsSupport::OS::sleep(timescale*500);

  return result;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::stop_target(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
   
  try
  {
    // stop target 
    if (m_pTarget != NULL)
    {
      m_pTarget->stop();

	  // wait end of target
      m_pTarget->join();

      // delete target thread 2
      delete m_pTarget;
      m_pTarget = NULL;
    }

    result = OP_SUCCESS;
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
CORBA::Long ClientThread::sleep_process(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
   
  int time_value;

  if (arg != "")
  {
    std::istringstream time_strm(arg);
	time_strm >> time_value;
	if (!time_strm.fail())
    {
	  result = OP_SUCCESS;
    }
  }

  if (result == OP_FAILURE)
  {
    os << "bad value for time" << std::endl;
  }
  else
  {
	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OsSupport::OS::sleep(timescale*time_value);
  }

	
  return result;
}
//----------------------------------------------------------------------------

}; // namespace Trace
}; // namespace Cdmw

