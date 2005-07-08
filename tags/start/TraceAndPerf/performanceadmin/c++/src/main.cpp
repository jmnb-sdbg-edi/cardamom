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


#include <iostream>
#include <cstdlib>
#include <string>

#include "Foundation/common/System.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "performanceadmin/PerformanceAdminTool.hpp"


using namespace Cdmw;
using namespace Cdmw::OsSupport;

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;  

    const std::string NAMING_REPOS_URL_OPTION = "--namingRepos-url";
	
    static void
    usage(std::ostream & os, const std::string &program_name)
    {
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help                   Display this information.\n"
		   << "--namingRepos-url=<URL>  URL of the CDMW Naming and Repository service.\n"
           << std:: endl;
    }



	
    CdmwNamingAndRepository::Repository_ptr
    get_cdmw_repository(CORBA::ORB_ptr orb, const std::string &url)
        throw(CORBA::TRANSIENT)
    {
        CDMW_ASSERT(!CORBA::is_nil(orb));
    
        CORBA::Object_var obj = CORBA::Object::_nil();

        //
        // Get naming service
        //
        try
		{
          obj = orb->string_to_object(url.c_str());
        } 
		catch(...)
		{
          std::cerr << "Invalid URL" << std::endl;
          throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                 CORBA::COMPLETED_NO);
        }
        
        if(CORBA::is_nil(obj.in()))
		{
          std::cerr << "URL denotes a nil object reference" << std::endl;
          throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                 CORBA::COMPLETED_NO);
        }
        
        // Get reference to the repository interface
        CdmwNamingAndRepository::Repository_var rep
                                  = CdmwNamingAndRepository::Repository::_nil();
        try 
		{
          rep = CdmwNamingAndRepository::Repository::_narrow(obj.in());
		  
          if (CORBA::is_nil(rep.in()))
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
		
        return rep._retn();
    }
	
}; // End anonymous namespace


int run(CORBA::ORB_ptr orb, const std::string &url)
{
    // Retrive Repository object reference
    CdmwNamingAndRepository::Repository_var rep = get_cdmw_repository(orb,url);

    Cdmw::PerformanceAdminTool::PerformanceAdminTool adm(orb,rep.in());
    
    adm.run(std::cin,std::cout);

    return SUCCESS;
}

int main(int argc, char* argv[])
{
    int status = SUCCESS;
    CORBA::ORB_var orb;

    if(argc == 1)
	{
      usage(std::cout, argv[0]);
      return FAILURE;
    }

    //
    // Execute command
    //
	
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

    try
    {
      orb = CORBA::ORB_init(argc, argv);
      status = run(orb.in(), namingReposURL);
    }
    catch(const CORBA::Exception& ex)
    {
      std::cerr << ex << std::endl;
      status = FAILURE;
    }

    if(!CORBA::is_nil(orb.in()))
    {
      try
	  {
        orb -> destroy();
      }
      catch(const CORBA::Exception& ex)
      {
        std::cerr << ex << std::endl;
        status = FAILURE;
      }
    }        

    return status;    
}



