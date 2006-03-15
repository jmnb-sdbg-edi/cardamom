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


#include <cstdlib>
#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"

#include "performancelibrary/PerformanceAdmin_impl.hpp"
#include "performancelibrary/PerformanceMngr.hpp"

#include "test2/ClientThread.hpp"
#include "test2/TargetThread.hpp"
#include "test2/TestPerformanceFile.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;    

}; // End anonymous namespace


// file to store stat results
static std::string default_stat_file = "apiStat.txt";
// Max number of target thread
static const int max_target = 2;
	

void usage(std::ostream & os, const std::string &program_name)
{
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help                            Display this information.\n"
           << "--interactive                     Start in interactive mode.\n"
           << "\n"
           << "--activate                        Start with performance activated.\n"
           << "--deactivate                      Start with performance deactivated.\n"
           << "--logFile=<name>                  Name of Performance Log File.\n"
           << std:: endl;
}

int auto_check (bool logCreationSuccess, std::string& perf_stat_file)
{ 
  int status = SUCCESS;
  
  Cdmw::Performance::TestPerformanceFile 
		              test_log_file (perf_stat_file,logCreationSuccess,max_target);

  test_log_file.start();

  return status;
}
				
int run (CORBA::ORB_ptr orb, int & argc, char** argv, 
	 	 bool interactive_mode, int activation_type)
{
	int status = SUCCESS;
	 
    if (!interactive_mode) 
	{
        // Auto tests
        std::string auto_command("verbose\n"
                                 "start\n"
                                 "activate T\n"
                                 "start\n"
							     "x\n");

        // Auto performance activate test
        std::string activate_command  ("activate T\n"
                                       "start\n"
							           "x\n");

        // Auto performance deactivate test
        std::string deactivate_command("activate F\n"
                                       "start\n"
							           "x\n");

		// set command to process
		std::string command;

		if (activation_type == 1)
        {
          command = activate_command;
		}
		else if (activation_type == 2)
        {
          command = deactivate_command;
		}
		else
        {
          command = auto_command;
		}

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

int process_init(CORBA::ORB_ptr orb, std::string& perf_stat_file)
{ 
	int status = SUCCESS;
	
	try
    {
      // get a reference to the root POA
	  PortableServer::POA_var rootPOA = 
	   	   	  	 PortableServer::POA::_narrow(orb->resolve_initial_references ("RootPOA"));
	  		
	  // create and initialize the performance manager
	  CdmwPerformanceAdmin::PerformanceAdmin_var perfoMngrRef = 
                    Cdmw::Performance::PerformanceMngr::init(rootPOA.in(),perf_stat_file);

      // export the object reference to a file
	  CORBA::String_var ref_string = orb->object_to_string(perfoMngrRef.in());
	  std::ofstream os("perfoLibrary.ior");
	  os << ref_string.in();
	  os.close();
  	   	  	 
	  // activate the POA Manager
	  rootPOA->the_POAManager()->activate();
	}
	
  	catch (const Cdmw::Exception &ex) {
      std::cerr << ex.what() << std::endl;
      status = FAILURE;
	}
   	catch(const CORBA::Exception &ex)  {     
	  std::cerr << ex << std::endl;   
      status = FAILURE;
	}
	
	return status;
}

void process_end()
{
    Cdmw::Performance::PerformanceMngr::cleanup();
}


int main(int argc, char* argv[])
{
    using namespace Cdmw::OsSupport;

	//
	// help argument
	//
	if(argc == 2)
	{
	  if(strcmp(argv[1], "--help") == 0)
	  {
	    usage(std::cout, argv[0]);
	    return SUCCESS;
	  }
	}
	
    bool interactive_mode = false;
    int activation_type = 0;

    // ---- Does it use interactive mode? ----
    std::string allTests = OS::get_option_value(argc, argv, "--interactive") ;
 
    if (allTests != "no")
	{
      interactive_mode = true;
    } 
	else
    {
      // ---- Get parameter of activation? ----
	  // = 0 no parameter : use internal scenario (default)
	  // = 1 activation   : use performance activation scenario
	  // = 2 deactivation : use performance deactivation scenario
	  //

      std::string activation = OS::get_option_value(argc, argv, "--activate") ;
      if (activation != "no")
	  {
        activation_type = 1;
      } 
	  else
      {
        activation = OS::get_option_value(argc, argv, "--deactivate") ;
        if (activation != "no")
	    {
          activation_type = 2;
        } 
	  }
	}
    
	//
	// get Performance Log File Name
	//
	std::string perf_stat_file = OS::get_option_value (argc, argv, "--logFile");
	if (perf_stat_file == "no")
	{
	  perf_stat_file = default_stat_file;
	}
	
	// 
	// start
	//
    int status = SUCCESS;

    CORBA::ORB_var orb;

    try {
        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

        status = process_init(orb.in(), perf_stat_file);

		if (status == SUCCESS)
	    {
		  // run processing
          status = run(orb.in(), argc, argv, interactive_mode, activation_type);
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
    if (!interactive_mode && activation_type == 0) 
	{ 
	  // check statistics result
      status = auto_check(status==SUCCESS,perf_stat_file);
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


