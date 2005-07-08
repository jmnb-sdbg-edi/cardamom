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


#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "Foundation/common/System.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "traceadmin/TraceAdminTool.hpp"


using namespace Cdmw;
using namespace Cdmw::OsSupport;

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;  

    const std::string NAMING_REPOS_URL_OPTION = "--namingRepos-url";
    const std::string COMMAND_FILE_NAME_OPTION = "--commandFile-name";
	
	const int POA_THREAD_POOL_SIZE = 5;

    static void
    usage(std::ostream & os, const std::string &program_name)
    {
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help                       Display this information.\n"
		   << "--namingRepos-url=<URL>      URL of the CDMW Naming and Repository service.\n"
		   << "[--commandFile-name=<name>]  Name of command file.\n"
           << std:: endl;
    }



	
    CdmwNamingAndRepository::Repository_ptr
    get_cdmw_repository(CORBA::ORB_ptr orb, const std::string &namingReposURL)
        throw(CORBA::TRANSIENT)
    {
        CDMW_ASSERT(!CORBA::is_nil(orb));
    
        CORBA::Object_var repositoryObj = CORBA::Object::_nil();

        //
        // Get naming service
        //
        try
		{
          repositoryObj = orb->string_to_object(namingReposURL.c_str());
        } 
		catch(...)
		{
          std::cerr << "Invalid URL" << std::endl;
          throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                 CORBA::COMPLETED_NO);
        }
        
        if(CORBA::is_nil(repositoryObj.in()))
		{
          std::cerr << "URL denotes a nil object reference" << std::endl;
          throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                 CORBA::COMPLETED_NO);
        }
        
        // Get reference to the repository interface
        CdmwNamingAndRepository::Repository_var repositoryRef =
                                  CdmwNamingAndRepository::Repository::_nil();
        try 
		{
          repositoryRef = CdmwNamingAndRepository::Repository::_narrow(repositoryObj.in());
		  
          if (CORBA::is_nil(repositoryRef.in()))
		  {
            std::cerr << "Couldn't retrieve reference of the Repository"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
          }
        } 
		catch (...) 
		{
          std::cerr << "Couldn't retrieve reference of the Repository"
                    << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
          throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                 CORBA::COMPLETED_NO);
        }
		
        return repositoryRef._retn();
    }
	
}; // End anonymous namespace


int run(CORBA::ORB_ptr orb, const std::string &url, const std::string &command_file_name)
{
    // Retrive Repository object reference
    CdmwNamingAndRepository::Repository_var repositoryRef = get_cdmw_repository(orb,url);

    TraceAdmin::TraceAdminTool adm (orb, repositoryRef.in(), command_file_name);
    
    // if no command file, get input command from standard input
    if (command_file_name == "")
    {
      adm.run(std::cin,std::cout,false);
    }
    // else command got from command file
    else
    {
      std::ifstream command_file (command_file_name.c_str(), std::ios::in);

	  if (!command_file)
	  {
		std::cout << "cannot open file : " << command_file_name << std::endl;
	  }
	  else
	  {
	    adm.run(command_file,std::cout,true);
	  }
    }  

    return SUCCESS;
}

void exceptionHandler(void)
{
    std::cout << "Trace Admin : UNEXPECTED EXCEPTION HANDLER" << std::endl;
}

int main(int argc, char* argv[])
{
#   ifndef _MSC_VER    

    std::set_unexpected(&exceptionHandler);

    // VC++ contrary to the clause 18.6 of the C++ standard, 
	//  set_unexpected, is not inside the std:: namespace
#   else

    set_unexpected(&exceptionHandler);

#   endif  


    int status = SUCCESS;
    CORBA::ORB_var orb;

    if(argc == 1)
	{
      usage(std::cout, argv[0]);
      return FAILURE;
    }

    //
    // help argument
    //
    if(strcmp(argv[1], "--help") == 0)
	{
      usage(std::cout, argv[0]);
      return SUCCESS;
    }

	//
	// get Naming & Repository URL argument
	//
	std::string namingReposURL = OS::get_option_value (argc, argv, NAMING_REPOS_URL_OPTION);
	if (namingReposURL == "no")
	{
	  std::cout << NAMING_REPOS_URL_OPTION.c_str() << "=<URL> option must be defined"
	            << std::endl;
	  usage(std::cout,argv[0]);
	  return FAILURE;
	}

	//
	// get Input Command File name argument
	//
	std::string commandFile = OS::get_option_value (argc, argv, COMMAND_FILE_NAME_OPTION);
	if (commandFile == "no")
	{
	  commandFile = "";
	}

    try
    {
      // Initialize the ORB
	  Cdmw::OrbSupport::StrategyList orb_strategies;
	  orb_strategies.add_OrbThreaded();
	  orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

	  orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);
      status = run (orb.in(), namingReposURL, commandFile);
    }
    catch(const CORBA::Exception& ex)
    {
      std::cerr << ex << std::endl;
      status = FAILURE;
    }
	catch (...) 
	{
      std::cerr << "unknown exception in main" << std::endl;
      status = FAILURE;
    }

    if(!CORBA::is_nil(orb.in()))
    {
      try
	  {
	Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	orb->destroy();
      }
      catch(const CORBA::Exception& ex)
      {
        std::cerr << ex << std::endl;
        status = FAILURE;
      }
    }        

    return status;    
}



