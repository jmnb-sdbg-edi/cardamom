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


#include "test1/ClientThread.hpp"
#include <string>
#include <cctype>
#include <algorithm>

#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"

#include "TraceAndPerf/idllib/CdmwPerformanceAdmin.stub.hpp"
#include "performancelibrary/ProbeCollector.hpp"
#include "performancelibrary/FunctionStat.hpp"

#include "test1/TargetThread.hpp"

#include <sstream>
#include <iostream>
#include <fstream>

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
namespace Performance
{

//----------------------------------------------------------------------------
ClientThread::ClientThread(CORBA::ORB_ptr orb, 
                           std::istream & is,
                           std::ostream & os)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_verbose(false),
      m_istream(is),
      m_ostream(os)
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
const CORBA::ULong ClientThread::nb_commands = 10;
const ClientThread::command_def ClientThread::commands[] =  // should be sorted alphabetically (by operation name)
{ 
    { "activate"          , &ClientThread::activate_perfo              },  
    { "exit"              , &ClientThread::exit                        },
	{ "get_api"           , &ClientThread::get_api_info                },
	{ "get_stat"          , &ClientThread::get_function_stat           },
    { "h"                 , &ClientThread::print_help                  },
    { "help"              , &ClientThread::print_help                  },
	{ "start"             , &ClientThread::start_targets               },
    { "verbose"           , &ClientThread::toggle_verbose              },
	{ "write_stat"        , &ClientThread::write_stat                  },
    { "x"                 , &ClientThread::exit                        }
};

    
//----------------------------------------------------------------------------
CORBA::Long ClientThread::runOperation(const std::string &op,
                                       const std::string &arg, 
                                       std::ostream &os) throw()
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
void ClientThread::print_info(std::ostream& os) const throw()
{
    os << "\n\n"
       << "\t\t---------------------------------------\n"
       << "\t\t  Cdmw Performance Test               \n"
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
        
        if (runOperation(command,arg,os) == EXIT_ORDERED) break; 
        print_prompt(os); 
    } 
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::exit(const std::string & arg, std::ostream & os) 
    throw()
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
        << " | x | exit             | exit ClientThread mode                     |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
        << " | h | help             | print available commands                   |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n" 
        << " | verbose              | toggle verbose mode on/off                 |" << "\n"
        << " +----------------------+--------------------------------------------+" << "\n"
		<< " | activate <T/F>       | activate/deactivate the measure            |" << "\n"
		<< " +----------------------+--------------------------------------------+" << "\n"
		<< " | get_api              | get info of API's                          |" << "\n"
	    << " +----------------------+--------------------------------------------+" << "\n"
		<< " | get_stat <API name>  | get statistics for the api name            |" << "\n"
		<< " +----------------------+--------------------------------------------+" << "\n"
	    << " | start                | start API to measure                       |" << "\n"
	    << " +----------------------+--------------------------------------------+" << "\n"
		<< " | write_stat <file>    | write statistics in specified file         |" << "\n"
	    << " +----------------------+--------------------------------------------+" << "\n"
				
        << std::endl;

     return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::activate_perfo(const std::string &arg, std::ostream &os) throw()
{
  CORBA::Long result = OP_FAILURE;
  const char *p_arg = arg.c_str();
  
  try 
  {
    CORBA::Object_var objref;
	char *p_file_name = "perfoLibrary.ior";

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
	  return result;
	}
	
	// get the PerformanceAdmin proxy
	CdmwPerformanceAdmin::PerformanceAdmin_var perfoAdmin = 
			     CdmwPerformanceAdmin::PerformanceAdmin::_narrow(objref.in());

	if (CORBA::is_nil(perfoAdmin.in()))
	{
	  os << "File : " << __FILE__ << " Line : " << __LINE__
	     << "could not _narrow object to type PerformanceAdmin" << std::endl;
	  return result;
	}

	// activate or deactivate the performance admin according parameter
	if (*p_arg == 'T' || *p_arg == 't')
	{
	  perfoAdmin->activate();
	}
	else
	{
	  perfoAdmin->deactivate();
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

  return result;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::start_targets(const std::string &arg, std::ostream &os) throw()
{
  CORBA::Long result = OP_FAILURE;
   
  try
  {
    // create 2 target threads
    Performance::TargetThread target1;
    Performance::TargetThread target2;
  
    // start clients
    target1.start();
    target2.start();

    // wait end of targets
    target1.join();
    target2.join();

    result = OP_SUCCESS;
  } 

  catch (const Cdmw::Exception & e )
  {
	PRINT_EXCEPTION(__FILE__,__LINE__,e,os);	
  }
	
  return result;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::get_api_info(const std::string &arg, std::ostream &os) throw()
{
  TargetThread target;

  int nbr_of_api = target.getApiNbr();
  std::string api_name; 
  int i;

  for (i=0 ; i < nbr_of_api ; i++)
  {
    target.getApiName (i, api_name);
    os << "api name          : " << api_name.c_str() << std::endl; 
	os << "  sleep time msec : " << target.getApiSleepTime(i) << std::endl;	
	os << "  call counter    : " << target.getApiCount(i) << std::endl;	
	os << std::endl;
  }

  return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::get_function_stat(const std::string &arg, std::ostream &os) throw()
{
  CORBA::Long result = OP_FAILURE;
  const char *p_arg = arg.c_str();
     
  try
  {					  
    Performance::ProbeCollector *p_collector = NULL;

    // get the Probe Collector
    p_collector = Performance::ProbeCollector::GetInstance();
  
    // get the Function stat	
    if (p_arg != NULL)
	{
	  const Performance::FunctionStat *p_function_stat = 
			                                  p_collector->getFunctionStat(arg);

	  if (p_function_stat != NULL)
	  {
        os << "api name : " << p_function_stat->getFunctionName().c_str()
		   << std::endl
		   << "min elapsed time (microsec) : " << p_function_stat->getMinElapsedTime()
		   << std::endl
		   << "max elapsed time (microsec) : " << p_function_stat->getMaxElapsedTime()
		   << std::endl
		   << "sum elapsed time (microsec) : " << p_function_stat->getSumElapsedTime()
		   << std::endl
		   << "hit count : " << p_function_stat->getHitCount()
	       << std::endl;
      }
      else
	  {
	     os << "api name is not recorded in collector" << std::endl;
	  }	  
	}
	else
	{
      os << "api name is missing" << std::endl;
	}
	
   	result = OP_SUCCESS;
  } 

  catch (const Cdmw::Exception & e )
  {
	PRINT_EXCEPTION(__FILE__,__LINE__,e,os);	
  }
	
  return result;
}

//----------------------------------------------------------------------------
CORBA::Long ClientThread::write_stat(const std::string & arg, std::ostream & os) throw()
{
  CORBA::Long result = OP_FAILURE;
  const char *p_arg = arg.c_str();
     
  try
  {					  
    Performance::ProbeCollector *p_collector = NULL;

    // get the Probe Collector
    p_collector = Performance::ProbeCollector::GetInstance();
  
    // write statistic in file passed as argument
    if (p_arg != NULL)
	{
	  p_collector->writeStatistic(arg);
	}
	else
	{
      os << "file name is missing" << std::endl;
	}
	
   	result = OP_SUCCESS;
  } 

  catch (const Cdmw::Exception & e )
  {
	PRINT_EXCEPTION(__FILE__,__LINE__,e,os);	
  }
	
  return result;
}	
				
//----------------------------------------------------------------------------

}; // namespace Performance
}; // namespace Cdmw

