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
#include "Foundation/common/Locations.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include "Repository/naminginterface/NamingInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "TraceAndPerf/performancelibrary/PerformanceMngr.hpp"

#include "test3/ClientThread.hpp"
#include "test3/TargetThread.hpp"
#include "test3/TestPerformanceFile.hpp"

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;  

    const int POA_THREAD_POOL_SIZE = 5;    
	
    static void
    usage(std::ostream & os, const std::string &program_name)
    {
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help         Display this information.\n"
           << "--url  IOR     URL of the CDMW Naming and Repository service.\n"
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


// PerformanceAdmin domain
CdmwNamingAndRepository::NameDomain_var dom = CdmwNamingAndRepository::NameDomain::_nil();

// PerformanceAdmin object name
std::string perfAdminName = "PerformanceAdmin";

// file to store stat results
static std::string stat_file = "apiStat.txt";
// Max number of target thread
static const int max_target = 2;
	
int auto_check (bool logCreationSuccess)
{ 
  int status = SUCCESS;
  
  Cdmw::Performance::TestPerformanceFile 
		              test_log_file (stat_file,logCreationSuccess,max_target);

  test_log_file.start();

  return status;
}
				
int run(CORBA::ORB_ptr orb, int & argc, char** argv, bool interactive_mode)
{
	int status = SUCCESS;
	 
    if (!interactive_mode) 
	{
        // Do all the tests
        std::string command("verbose\n"
                            "start\n"
                            "activate T\n"
                            "start\n"
							"x\n");

		// convert string command to stream
        std::istringstream is(command);

		// create client thread object
        Cdmw::Performance::ClientThread client(orb,is,std::cout);

		// start client and run orb
        client.start();
        orb->run();

		// wait end of client thread after corba loop stopping
        client.join();
    }
   	else { 
        // Enter interactive mode
        Cdmw::Performance::ClientThread client(orb,std::cin,std::cout);

		// start client and run orb
        client.start();
        orb->run();

	    // wait end of client thread after corba loop stopping
        client.join();
    }

    return status;
}

int process_init(CORBA::ORB_ptr orb, const std::string &url)
{ 
	int status = FAILURE;
	
	using Cdmw::Common::Locations;
	using namespace Cdmw::OrbSupport;
		
	CdmwPerformanceAdmin::PerformanceAdmin_var perfoAdminRef = 
			                CdmwPerformanceAdmin::PerformanceAdmin::_nil();
	try
    {
      // get a reference to the root POA
	  PortableServer::POA_var rootPOA = 
	   	   	  	 PortableServer::POA::_narrow(orb->resolve_initial_references ("RootPOA"));
	  		
	  // create and initialize the performance manager
	  perfoAdminRef = 
              Cdmw::Performance::PerformanceMngr::init(rootPOA.in(),stat_file);

	  // export the object reference to a file
	  CORBA::String_var ref_string = orb->object_to_string(perfoAdminRef.in());
	  std::ofstream os("perfoLibrary.ior");
	  os << ref_string.in();
	  os.close();

	  // activate the POA Manager
	  PortableServer::POAManager_var POAMngr = rootPOA->the_POAManager();
	  POAMngr->activate();

	  status = SUCCESS;
	}
	
  	catch (const Cdmw::Exception &ex) {
      std::cerr << ex.what() << std::endl;
	}
   	catch(const CORBA::Exception &ex)  {     
	  std::cerr << ex << std::endl;   
	}
	
	// Register PerformanceAdmin object

	CdmwNamingAndRepository::Repository_var rep = CdmwNamingAndRepository::Repository::_nil();

	CosNaming::NamingContext_var admin_root = CosNaming::NamingContext::_nil();

	try
    {
	  if (status == SUCCESS)
	  {
		status = FAILURE;

	    // Retrieve Repository object reference
	    rep = get_cdmw_repository(orb,url);

		// Get "AdminRootContext" root context
		admin_root = rep->resolve_root_context(Locations::ADMIN_ROOT_CONTEXT_ID);

	    status = SUCCESS;
	  }
	}
    catch (const CdmwNamingAndRepository::Repository::NoRootContext & ) 
	{
      std::cerr << "Admin Root Context does not exist" << std::endl;
	  throw CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO);
	}
	catch (const CORBA::SystemException & )
	{
	  throw;
	}

	try
    {
	  if (status == SUCCESS)
	  {
		status = FAILURE;
        
		// get NamingInterface on admin root context
		Cdmw::NamingAndRepository::NamingInterface ni (admin_root.in());
		
		/******************************
		// If already registered, force its registration
        // set performance location
        // Pattern is :
        //    "PERFORMANCE"/"PerformanceAdmin"
        std::string perf_admin_name("PERFORMANCE");
        perf_admin_name += "/";
        perf_admin_name += "PerformanceAdmin";

	    ni.bind(perf_admin_name,perfoAdminRef.in(),true);
		********************************/

		// get Domain of object
        // set performance location
        // Pattern is :
        //    CDMW/ADMIN/"PERFORMANCE"/"PerformanceAdmin"
        std::string perf_admin_domain(Locations::CDMW_ADMIN_NAME_DOMAIN);
        perf_admin_domain += "/";
        perf_admin_domain += "PERFORMANCE";

		dom = rep->resolve_name_domain(perf_admin_domain.c_str());

		// reserve the PerformanceAdmin name in the domain
		CdmwNamingAndRepository::NameDomain::RegistrationId_var regID =
				              dom->new_name(perfAdminName.c_str());

		// register the object in the domain
		dom->register_object(regID.in(), perfoAdminRef.in());

        std::cout << "PerformanceAdmin is registered in CDMW/ADMIN/PERFORMANCE/PerformanceAdmin" 
				  << std::endl;

	    status = SUCCESS;
	  }
	}
    catch (const CosNaming::NamingContext::NotFound & )
	{
	  // Should not happen!
	  throw CORBA::BAD_PARAM(BAD_PARAMInvalidPerformanceAdminLocation,CORBA::COMPLETED_NO);
	}
	catch (const CosNaming::NamingContext::CannotProceed & )
	{
      // Should not happen!
      throw CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO);
    } 
	catch (const CosNaming::NamingContext::InvalidName & )
	{
	  throw CORBA::BAD_PARAM(BAD_PARAMInvalidPerformanceAdminName,CORBA::COMPLETED_NO);
    } 
	catch (const CosNaming::NamingContext::AlreadyBound & )
	{
      // Should not happen because force is set to true!
      throw CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO);
    }
    catch (const CdmwNamingAndRepository::NoNameDomain & ) 
	{
      std::cerr << "Performance Name Domain does not exist" << std::endl;
	  throw CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO);
	}
    catch (const CdmwNamingAndRepository::InvalidName & ) 
	{
      std::cerr << "Name has illegal form" << std::endl;
	  throw CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO);
	}
    catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists & ) 
	{
	  //throw CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO);
      std::cerr << "PerformanceAdmin object already registered in repository"
	            << std::endl;
	  // process must continue in this case
	  status = SUCCESS;
	}
    catch (const CdmwNamingAndRepository::NameDomain::InvalidRegistration & ) 
	{
      std::cerr << "object registration identifier is invalid" << std::endl;
	  throw CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO);
	}
	catch (const CORBA::BAD_PARAM & )
	{
      std::cerr << "attempt to register a nil object" << std::endl;
      throw;
    }
	catch (const CORBA::SystemException & )
	{
      throw;
    }

	return status;
}

void process_end()
{
	using namespace Cdmw::OrbSupport;

    Cdmw::Performance::PerformanceMngr::cleanup();

	// release the PerformanceAdmin name in the domain
	try
	{
      if (dom.in() != CdmwNamingAndRepository::NameDomain::_nil())
	  {
	    dom->release_name(perfAdminName.c_str());
	  }
	}
    catch (const CdmwNamingAndRepository::NameDomain::NotRegistered & ) 
	{
      std::cerr << "Performance Admin does not registered" << std::endl;
	  throw CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO);
	}
    catch (const CdmwNamingAndRepository::InvalidName & ) 
	{
      std::cerr << "Name has illegal form" << std::endl;
	  throw CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO);
	}
	catch (const CORBA::NO_PERMISSION & )
	{
      std::cerr << "attempt to release an object without permission" << std::endl;
      throw;
    }
}


int main(int argc, char* argv[])
{
    using namespace Cdmw::OsSupport;

    if(argc == 1)
	{
      usage(std::cout, argv[0]);
      return FAILURE;
    }

    std::string nar_url;
	
    //
    // Execute command
    //
    if(strcmp(argv[1], "--help") == 0)
	{
      usage(std::cout, argv[0]);
      return SUCCESS;
    }
    else if(strcmp(argv[1], "--url") == 0)
    {
      if(argc != 3)
	  {
        usage(std::cout,argv[0]);
        return FAILURE;
      }
	  else
	  {    
	    nar_url = argv[2];
	  }
    }
    else
    {
      usage(std::cout,argv[0]);
      return FAILURE;
    }

    // ---- set interactive mode ----
    bool interactive_mode = true;
    
    int status = SUCCESS;

    CORBA::ORB_var orb;

    try {
        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

        status = process_init(orb.in(),nar_url);

		if (status == SUCCESS)
	    {
		  // run processing
          status = run(orb.in(), argc, argv, interactive_mode);
		}
    } 
	catch (const Cdmw::Exception & ex) {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    }
   	catch(const CORBA::Exception& ex)  {     
        std::cerr << ex << std::endl;   
        status = FAILURE;
    }

	// end of process
	
	process_end();

    // in auto mode start auto checking
    if (!interactive_mode) 
	{ 
	  // check statistics result
      status = auto_check(status==SUCCESS);
    }
	
	// end of corba processing
	
    if(!CORBA::is_nil(orb.in()))
    {
        try {
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    orb->destroy();
        }
        catch(const CORBA::Exception& ex) {     
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    return status;    
}


