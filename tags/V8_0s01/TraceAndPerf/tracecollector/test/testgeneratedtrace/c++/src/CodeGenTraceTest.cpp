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
#include <fstream>
#include <sstream>
#include <string>

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/idllib/FTPullMonitorable.skel.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"

#include "testgeneratedtrace/CodeGenTraceTest.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
#include "Foundation/ossupport/OS.hpp"

#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

#include <TraceAndPerf/tracelibrary/InitUtils.hpp>
#include <TraceAndPerf/tracelibrary/Trace.hpp>

using namespace Cdmw::OsSupport;


PlatformInterfaceTest::PlatformInterfaceTest()
  throw()
  : Testable("Cdmw::PlatformInterface")
{
}

PlatformInterfaceTest::~PlatformInterfaceTest()
  throw()
{
}


CdmwPlatformMngt::ProcessCallback_ptr get_process_callback(CORBA::ORB_ptr orb)
  throw(CORBA::TRANSIENT)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
      
    CORBA::Object_var obj = CORBA::Object::_nil();

    // Get naming service
    try
	{
	  obj = orb -> resolve_initial_references("NameService");
	}
    catch(const CORBA::ORB::InvalidName&)
	{
	  std::cerr<< "Can't resolve `NameService'" << std::endl;
	  throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
				 CORBA::COMPLETED_NO);
	}
      
    if(CORBA::is_nil(obj.in()))
    {
	  std::cerr<< "`NameService' is a nil object reference" << std::endl;
	  throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
				 CORBA::COMPLETED_NO);

    }
      
    CosNaming::NamingContextExt_var nc =
	CosNaming::NamingContextExt::_narrow(obj.in());

    if(CORBA::is_nil(nc.in()))
    {
	  std::cerr<< "`NameService' is not a NamingContextExt object reference" << std::endl;
	  throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
				 CORBA::COMPLETED_NO);
    }
      
    const char* PROCESS_CALLBACK_BINDING_NAME = "CDMW.I/process-callback.simulated";
    // Get reference to the ProcessCallback object
    CdmwPlatformMngt::ProcessCallback_var callback
	        = CdmwPlatformMngt::ProcessCallback::_nil();

	try {
	  Cdmw::CommonSvcs::Naming::NamingInterface ni(nc.in());
	  CORBA::Object_var obj__ = ni.resolve(PROCESS_CALLBACK_BINDING_NAME);
	  callback = CdmwPlatformMngt::ProcessCallback::_narrow(obj__.in());
	  if (CORBA::is_nil(callback.in())) {
	    std::cerr<< "Couldn't retrieve reference of the ProcessCallback"
		     << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
	    // TODO: Update minor code
	    throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachApplicationAgent,
				   CORBA::COMPLETED_NO);
	  }
	} catch (...) {
	  std::cerr<< "Couldn't retrieve reference of the ProcessCallback"
		   << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
	  // TODO: Update minor code
	  throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachApplicationAgent,
				 CORBA::COMPLETED_NO);
	}

    return callback._retn();
}


//////



void PlatformInterfaceTest::do_tests()
  throw()
{

    std::string  file = "LoggingTrace";  
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (7);

    std::string nameServicePort = "4997";
  
    try 
    {
 
      TEST_INFO("Initialize the ORB");
      
      Cdmw::OrbSupport::StrategyList orb_strategies;
      orb_strategies.add_OrbThreaded();
      orb_strategies.add_PoaThreadPool(5);
      
      std::string str_opt( "" );
      char** argv_tmp = NULL;
      int argc_tmp(2);

      CORBA::ORB_var orb;
      try
	  {
	    str_opt += " -ORBInitRef NameService=corbaloc:iiop:localhost:" 
	            + nameServicePort + "/NameService";
	  
	    argv_tmp = Cdmw::Common::String::to_char_array(Cdmw::Common::String::to_strings( str_opt ) );
	    orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc_tmp, argv_tmp, orb_strategies);
	    TEST_SUCCEED();
	  }
      catch(const CORBA::SystemException& ex)
	  {
	    std::cerr << "System Exception " << ex << std::endl;
	    TEST_FAILED();
	  }
      catch(const CORBA::Exception& ex)
	  {
	    TEST_FAILED();
	  }
	  
      // ===================================================
      // Get the root POA 
      // ===================================================
      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
      
      // ===================================================
      // Activate the root POA manager
      // ===================================================
      PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
      poaManager->activate();

      std::string m_nsPort = "4997";
      std::string repositoryPort = "5000";
      
      OS::ProcessId m_idRepository = 0;
      OS::ProcessId m_idNS = 0;
      
      try
	  {
	    TEST_INFO("Launch the naming service");

	    std::string namingservice="Naming_Service";
	    m_idNS = OS::create_process( namingservice, "-ORBEndpoint iiop://localhost:" + m_nsPort );
	  
	    int timescale = Cdmw::TestUtils::Testable::get_timescale();
	    OS::sleep(timescale*3000);
	  
	    TEST_INFO("Launch the simulator repository");
	    //OS::sleep(timescale*3000);
	  
	    // Starting the Repository Simulator :
	    std::string repository = "cdmw_simulated_repository";
	    std::string namingCorbaloc = " -ORBInitRef NameService=corbaloc:iiop:localhost:"+ m_nsPort +"/NameService";
	  
	    std::string repositoryArgs ("");
	  
	    repositoryArgs += " -ORBEndpoint iiop://localhost:";
	    repositoryArgs += repositoryPort;
	    repositoryArgs += namingCorbaloc;
	    repositoryArgs += " -DOM CDMW";
	    repositoryArgs += " -DOM ";
	    repositoryArgs += Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN;
	    repositoryArgs += " -DOM CDMW/SERVICES/TRACE";
	    repositoryArgs += " -DOM CDMW/SERVICES/TRACE/COLLECTORS";
	    repositoryArgs += " -ROOT ";
	    repositoryArgs += Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID;
	  
	    std::cout << repository.c_str() << repositoryArgs.c_str() << std::endl;
	  
	    m_idRepository = OS::create_process (repository, repositoryArgs);
	  
	    OS::sleep(timescale*10000*2);
	    TEST_SUCCEED();
	  }
      catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
      
      // create callback
      TEST_INFO("create callback");

      CdmwPlatformMngt::ProcessCallback_var callback;
      // Getting callback IOR
      callback = get_process_callback(orb.in());
      std::string callbackIOR = orb->object_to_string(callback.in());
      
      
      str_opt +=  Cdmw::Common::Options::CALLBACK_IOR_OPTION;
      str_opt +=   "=";
      str_opt += callbackIOR;  	
      ++argc_tmp;
      argv_tmp = Cdmw::Common::String::to_char_array( Cdmw::Common::String::to_strings (str_opt));
      
      OS::ProcessId m_idTrace = 0;

      int timescale = Cdmw::TestUtils::Testable::get_timescale();
      
      try
	  {
	    TEST_INFO("Launch the trace and collector service");
	  
	    //int timescale = Cdmw::TestUtils::Testable::get_timescale();
	    OS::sleep(timescale*3000);
	  
	    m_idTrace = OS::create_process("cdmw_trace_collector"," --collector-name=the_collector --namingRepos-url=corbaloc:iiop:localhost:5000/SimRepository --traceFile-name=LoggingTrace --traceFile-number=1");
	  
	    OS::sleep(timescale*10000);
	    TEST_SUCCEED();
	  }
      catch(const Cdmw::BadParameterException& ex)
	  {
	    TEST_FAILED();
	  }
      catch(const CORBA::Exception& ex)
	  {
	    TEST_FAILED();
	  }
	
	  //////////////////////////////////////////////
	
	  TEST_INFO("Call the generated CDMW_init ");
	
	  std::string str_options( "" );
	  str_options +=  "./cdmw_test_generated_trace ";
	  str_options +=  Cdmw::Common::Options::CALLBACK_IOR_OPTION;
	  str_options +=   "=";
	  str_options += callbackIOR;  
	  str_options +=  " ";
	  str_options +=  Cdmw::Common::Options::PROCESS_XML_FILE_OPTION;
	  str_options +=   "=";
	  str_options +=  "Server.xml";
	  str_options +=  " ";
	  str_options +=  Cdmw::Common::Options::VALIDATE_XML_OPTION;
	  str_options +=  " ";
	  str_options +=  "--collector-name=the_collector";
	  int argc(5);
	  argv_tmp = Cdmw::Common::String::to_char_array(Cdmw::Common::String::to_strings( str_options ) );

	
	  try {
	    Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv_tmp, NULL);
	    TEST_SUCCEED();
	  }
	  catch(const CORBA::SystemException &ex) 
	  {
	    TEST_FAILED();
	  }
	
        
	  Cdmw::Trace::Stream stream;
	  
        
        
      // ===================================================
      // trace a message (don't use CDMW_TRACE in main)
      // ===================================================
      std::string msg = "Server started";  

	  TEST_INFO("Send a message to collector");
	  try {
	    CDMW_TRACE_ST(stream,"TUTOR", 1, msg.c_str());
	    TEST_SUCCEED();
	  }
	  catch(...) {
	    TEST_FAILED();
	  }

	  OS::sleep(timescale*2000);

	  std::string trace_file = file;
	
	  // build name of backup info
	  std::string backUpInfoFileName = trace_file;
	  backUpInfoFileName += "_bkpinfo";
	
	  short current_bkp = 0;
	
	  // open the file for read backup info
	  std::ifstream bkp_file_in_str(backUpInfoFileName.c_str(), std::ios::in);
	
	  // if file exist and can be opened
	  if (bkp_file_in_str) {
	    // get current backup from info file
	    bkp_file_in_str >> current_bkp;
	  
	    // if current backup not retrieve
	    // set no backup
	    if (!bkp_file_in_str.good()) {
            current_bkp = 0;
	    }
	  
	    // close the file
	    bkp_file_in_str.close();
	  }

	  if (current_bkp > 0) {
	    // update the file name base with the backup nbr
	    std::ostringstream name_str;
	    name_str << file << "_" << current_bkp;
	  
	    trace_file = name_str.str();
	  }
	
	  // add extension for 1st circular file
	  trace_file += ".0";
	
	
	  TEST_INFO("Test Trace library Log File");
	
	
	  std::ifstream result(trace_file.c_str(), std::ios::in);
	  if (!result)
	  {
	    TEST_FAILED();
	    get_resultStream() << "cannot open file : " << trace_file << std::endl;
	  }
	  else
	  {
	    // file is opened
	    TEST_SUCCEED();
	    
	    TEST_INFO("Check if the message has been correctly logged");
	    
	    int string_found = 0;
	    std::string s;
	    
	    while(getline(result, s)) {
	      if (s == "Server started") {
		    string_found = 1;
		    break;
	      }	    
	    }
	    
	    if (string_found)
	      TEST_SUCCEED();
	    else 
	      TEST_FAILED();
	    
	    result.close();
	  }
	
	  TEST_INFO("Kill The Collector");
	  OS::kill_process(m_idTrace);
	
	  TEST_INFO("Kill The repository and Naming service");	  
	  OS::kill_process(m_idRepository);
	  OS::kill_process(m_idNS);
	  	
	  TEST_INFO("Destroy the ORB");
	
      Cdmw::CdmwInit::CDMW_cleanup(orb.in());


	  orb->shutdown(false);
	  Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	  orb->destroy();
	
	  TEST_INFO("End of test");
    }  
    catch (...)
    {
    }
}
