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


#include "test2/ClientThread.hpp"
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

#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingUtil.hpp"

#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.stub.hpp"
#include "TraceAndPerf/tracelibrary/Trace.hpp"
#include "TraceAndPerf/tracelibrary/FlushAreaMngr.hpp"

#include "test2/TargetThread.hpp"

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
	  m_repository(CdmwNamingAndRepository::Repository::_duplicate(rep))
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
}

//----------------------------------------------------------------------------
void ClientThread::run()
  throw()
{
    run (m_istream,m_ostream);
}

//----------------------------------------------------------------------------
const CORBA::ULong ClientThread::nb_commands = 7;
const ClientThread::command_def ClientThread::commands[] =  // should be sorted alphabetically (by operation name)
{  
    { "exit"                  , &ClientThread::exit                        },
    { "h"                     , &ClientThread::print_help                  },
    { "help"                  , &ClientThread::print_help                  },
	{ "sleep_process"         , &ClientThread::sleep_process               },
	{ "start_stop_targets"    , &ClientThread::start_stop_targets          },
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
	    << " | start_stop_targets                | start (and stop) targets with trace        |" << "\n"
	    << " +-----------------------------------+--------------------------------------------+" << "\n"
	    << " | sleep_process <time>              | set process to sleep, time in msec         |" << "\n"
	    << " +-----------------------------------+--------------------------------------------+" << "\n"
        << std::endl;

     return OP_SUCCESS;
}





//----------------------------------------------------------------------------
CORBA::Long ClientThread::start_stop_targets(const std::string &arg, std::ostream &os)
{
  CORBA::Long result = OP_FAILURE;
   
  try
  {
    // create the target thread
    Trace::TargetThread target (m_traceProducer.in());
  
    // start client which stop automatically
    target.start();

    // wait end of target
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

