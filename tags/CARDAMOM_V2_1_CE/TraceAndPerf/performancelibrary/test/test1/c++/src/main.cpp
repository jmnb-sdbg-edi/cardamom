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
#include "performancelibrary/ProbeCollector.hpp"

#include "test1/ClientThread.hpp"
#include "test1/TargetThread.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;    

}; // End anonymous namespace


Cdmw::Performance::ProbeCollector *p_collector = NULL;
 
int auto_check (char *p_stat_file)
{ 
  int status = SUCCESS;
  const int MAX_TARGET = 2;

  Cdmw::Performance::TargetThread target;
   
  const int MAX_LINE = 200;
  char line[MAX_LINE+1];
 
  std::ifstream result(p_stat_file, std::ios::in);

  if (!result)
  {
    status = FAILURE;
	std::cerr << "cannot open file : " << p_stat_file << std::endl;
  }
  else
  {
    while (result.getline (line,MAX_LINE) && status==SUCCESS)
	{
	  char *p_api_name = &line[0];
	  
	  char *p_min_time = NULL;
	  char *p_max_time = NULL;
	  char *p_sum_time = NULL;
	  char *p_hit_count = NULL;
	  
	  int seq = 1;
	  int end_loop = 0;
	  int i;
	  
	  for (i=0 ; i < MAX_LINE && end_loop == 0 ; i++)
	  {
	    if (line[i] == ';')
	    {
		  line[i] = '\0';

		  if (seq == 1)
		  {
		    p_min_time = &line[i+1];
          }	
		  else if (seq == 2)
		  {
		    p_max_time = &line[i+1];
		  }
	      else if (seq == 3)
		  {
		    p_sum_time = &line[i+1];
		  }
	      else if (seq == 4)
		  {
			p_hit_count = &line[i+1];
		  }	

	      seq++;
        }

        else if (line[i] == '\0' && seq == 5)
	    {
	      end_loop = 1;
		}
	  }
	  
	  std::string api_name_str = p_api_name;
	  int api_index = target.getApiIndex(api_name_str);

	  if (api_index == -1)
	  {
	    status = FAILURE;
		std::cerr << "API name not found : " << p_api_name << std::endl;		
	  }
	  else
	  {
	    int min_time = atoi(p_min_time);
	    int max_time = atoi(p_max_time);
	    int sum_time = atoi(p_sum_time);
		int hit_count = atoi(p_hit_count);

		// sleep time is returned in milli sec and is converted in micro sec
	    int targ_time = target.getApiSleepTime(api_index) * 1000;	    

		int targ_hit_count = MAX_TARGET * target.getApiCount(api_index);
		int targ_sum_time = targ_time * targ_hit_count;

        if (min_time < targ_time || min_time > (targ_time + 200000000))
		{
		  status = FAILURE;
		  std::cerr << "error for api " << p_api_name << "in min time : " << min_time << std::endl;
		}
		else if (max_time < targ_time || max_time > (targ_time + 200000000)
				 || max_time < min_time)
		{
		  status = FAILURE;
		  std::cerr << "error for api " << p_api_name << "in max time : " << max_time << std::endl;	  
		}			  
		else if (sum_time < targ_sum_time || sum_time > (targ_sum_time + 1000000000))
		{
		  status = FAILURE;
		  std::cerr << "error for api " << p_api_name << "in sum time : " << sum_time << std::endl;		  
		}			  
		else if (hit_count != targ_hit_count)
		{
		  status = FAILURE;
		  std::cerr << "error for api " << p_api_name << "in hit count : " << hit_count << std::endl;	
		}			  
	  }
	}
	
	result.close();
  }

  return status;
}
				
int run(CORBA::ORB_ptr orb, int & argc, char** argv, bool interactive_mode)
{
	int status = SUCCESS;
	
    if (!interactive_mode) 
	{
	    // file to store stat results
	    char *p_stat_file = "apiStat.txt";
		
        // Do all the tests
        std::string command("verbose\n"
                            "start\n"
                            "activate T\n"
                            "start\n"
                            "write_stat ");

		// add write stat file name
		command += p_stat_file;
		command += "\n"
				   "x\n";
		
		// convert string command to stream
        std::istringstream is(command);

		// create client thread object
        Cdmw::Performance::ClientThread client(orb,is,std::cout);

		// start client and run orb
        client.start();
        orb->run();

		// wait end of client thread after corba loop stopping
        client.join();

		// check statistics result
		status = auto_check(p_stat_file);
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

int process_init(CORBA::ORB_ptr orb)
{ 
	int status = SUCCESS;
	
	try
    {	
	  // create PerformanceAdmin servant (delete will be processed by POA when
	  //                                  ref count set to 0)
	  Cdmw::Performance::PerformanceAdmin_impl *serv =
			                 new Cdmw::Performance::PerformanceAdmin_impl();   

	  // get a reference to the root POA
	  PortableServer::POA_var rootPOA = 
	   	  	 PortableServer::POA::_narrow(orb->resolve_initial_references ("RootPOA"));
	
	  // activate the servant on the root POA and get its id (ref count is incremented)
	  PortableServer::ObjectId_var id = rootPOA->activate_object(serv);

	  // create an object var for decrementing ref count when its destructor
	  // will be called at the end of method (ref count is not incremented by creation)
	  PortableServer::ServantBase_var serv_var = serv;
	  
      // export the object reference to a file
	  CORBA::Object_var obj_ref = rootPOA->id_to_reference(id);
	  CORBA::String_var ref_string = orb->object_to_string(obj_ref.in());
	  std::ofstream os("perfoLibrary.ior");
	  os << ref_string.in();
	  os.close();

	  // activate the POA Manager
	  rootPOA->the_POAManager()->activate();
	  
	  // create the Probe Collector
	  p_collector = Cdmw::Performance::ProbeCollector::GetInstance();
	}
	
  	catch (const Cdmw::Exception & ex) {
      std::cerr << ex.what() << std::endl;
      status = FAILURE;
	}
   	catch(const CORBA::Exception& ex)  {     
	  std::cerr << ex << std::endl;   
      status = FAILURE;
	}
	
	return status;
}

void process_end()
{
  if (p_collector != NULL)
  {
    delete p_collector;
  }
}


int main(int argc, char* argv[])
{
    using namespace Cdmw::OsSupport;

    std::string allTests = OS::get_option_value(argc, argv, "--interactive") ;
 
    // ---- Does it use interactive mode? ----
    bool interactive_mode = false;
    if ( allTests != "no" ) {
        interactive_mode = true;
    } 
    
    int status = SUCCESS;

    CORBA::ORB_var orb;

    try {
        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

        status = process_init(orb.in());

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


