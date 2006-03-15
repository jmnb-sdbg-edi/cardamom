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


// Standard include files
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>

// Cdmw include files
#include "Foundation/common/Locations.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"

#include "SystemMngt/tools/ProcessAdmin.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.stub.hpp"

// Application include files
#include "test_profilemanager/TestEventChannelProfileManager.hpp"


namespace {
    
const int SUCCESS = 0;
const int FAILURE = 1;  
const int POA_THREAD_POOL_SIZE=5;

const std::string NAMING_SERVICE_PORT_OPTION  = "--NamePort";
const std::string REPOSITORY_PORT_OPTION      = "--RepPort";
    
static void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
        << program_name << " options\n"
        << "\n"
        << "Options:\n"
        << "--help                     Display this information.\n"
        << "\n"
        << "[--NamePort=<nbr>          Port of Name Service.\n"
        << "[--RepPort=<nbr>           Port of Name and Repository.\n"
        << std:: endl;
}

CdmwPlatformMngt::ProcessCallback_ptr
get_process_callback(CORBA::ORB_ptr orb)
    throw(CORBA::TRANSIENT)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    
    CORBA::Object_var obj = CORBA::Object::_nil();
    //
    // Get naming service
    //
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
    {
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
    }
    return callback._retn();
}


void initRepository(CORBA::ORB_ptr orb)
{
     using namespace Cdmw::NamingAndRepository;
     using namespace Cdmw::Common;

     //
    // Get naming service
    //
     CORBA::Object_var obj;

    try
    {
        obj = orb -> resolve_initial_references("NameService");
    }
    catch(const CORBA::ORB::InvalidName&)
    {
         std::cerr<< "Can't resolve `NameService'" << std::endl;
       throw;
    }

    if(CORBA::is_nil(obj.in()))
    {
         std::cerr << "`NameService' is a nil object reference" << std::endl;
       throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                              CORBA::COMPLETED_NO);
    }
    
    CosNaming::NamingContextExt_var nc =
        CosNaming::NamingContextExt::_narrow(obj.in());

    if(CORBA::is_nil(nc.in()))
    {
         std::cerr <<"`NameService' is not a NamingContextExt object reference"<< std::endl;
        throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                               CORBA::COMPLETED_NO);
    }

    const char* REPOSITORY_NAME = "CDMW.I/repository.simulated";

    // Get reference to the repository interface
     CdmwNamingAndRepository::Repository_var  rep
        = CdmwNamingAndRepository::Repository::_nil();
    {
        try {
            Cdmw::CommonSvcs::Naming::NamingInterface ni(nc.in());
            CORBA::Object_var obj__ = ni.resolve(REPOSITORY_NAME);
                
            rep = CdmwNamingAndRepository::Repository::_narrow(obj__.in());
            if (CORBA::is_nil(rep.in())) 
                {
                    std::cerr << "Couldn't retrieve reference of the Repository"
                             << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
                throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                       CORBA::COMPLETED_NO);
            }
        } catch (...) {
              std::cerr << "Couldn't retrieve reference of the Repository"
                         << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
        }

          try {
            RepositoryInterface::init(Locations::CDMW_SERVICES_NAME_DOMAIN, rep.in());
          }
          catch (...) {
              std::cerr << "Couldn't init Repository"
                         << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
              throw;
        }
    }
}

};  // end namespace


int
run(CORBA::ORB_ptr orb, Cdmw::Tools::ProcessAdmin* proc_admin)
{
     using namespace Cdmw::Common;
     using namespace Cdmw::NamingAndRepository;
     using namespace Cdmw::CommonSvcs::Naming;
     using namespace CdmwEventAdmin;
     using namespace Cdmw::EventAdmin;
    
    //
     // Get NamingInterface object from RepositoryInterface
     //
     NamingInterface ni
    = RepositoryInterface::get_domain_naming_interface(Locations::CDMW_SERVICES_NAME_DOMAIN);

     //
     //Get a reference on the EventChannelProfileManager
    //
     CORBA::Object_var obj = ni.resolve("EVENT/EventChannelProfileManager");
    EventChannelProfileManager_var manager = EventChannelProfileManager::_narrow(obj.in());

    if (CORBA::is_nil(manager.in())) 
     {
        std::cerr << "EventChannelProfileManager is a nil object reference"
                  << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        return FAILURE;
    }

    //Testing TestEventChannelProfileManager
     std::cout << "EventChannelProfileManager reference found!" << std::endl;
    TestEventChannelProfileManager testProfileManager(manager.in());
    testProfileManager.start();    
    
    // send stop order
    std::string commands("stop\n exit \n");
    std::istringstream commands_is(commands);
    std::cout << "Stop cdmw_event_channel_manager" << std::endl;
    proc_admin->run(commands_is,std::cout);

    return SUCCESS;
}


int main(int argc, char* argv[])
{
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;
    int status = SUCCESS;
    CORBA::ORB_var orb;
    CdmwPlatformMngt::ProcessCallback_var callback;

    // help argument
    if (argc >= 2)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            usage (std::cout, argv[0]);
            return SUCCESS;
        }
    }
    

    std::string callbackIOR = OS::get_option_value(argc, argv,
                                                   Options::CALLBACK_IOR_OPTION);

    bool unit_tests = true;
    // ---- Is it launched by platform management for integration tests? ----
    if (callbackIOR != "no" && callbackIOR != "yes")
    {
        unit_tests = false;
    }

    // get Name Service port
    std::string nameServicePort = OS::get_option_value (argc, argv, NAMING_SERVICE_PORT_OPTION);
    
    if (nameServicePort  == "no") 
    {
        nameServicePort = "5010";
    } 
    
    // get Repository port
    std::string repositoryPort = OS::get_option_value (argc, argv, REPOSITORY_PORT_OPTION);
    
    if (repositoryPort  == "no") 
    {
        repositoryPort = "5020";
    } 

    // launch Nameservice and SimRepository for tests
    OS::ProcessId idNaming = 0, idRepository = 0; 
    OS::ProcessId idProfileManager = 0;
    int timescale = Cdmw::TestUtils::Testable::get_timescale();

    try
    {
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        std::string serverArgs(argv[0]);
#if (CDMW_ORB_VDR == orbacus )
        serverArgs += " -ORBconfig obtest.conf";
        int nbArgs = 3;
#elif (CDMW_ORB_VDR == tao )
        serverArgs += " -ORBInitRef NameService=corbaloc:iiop:localhost:" 
                      + nameServicePort + "/NameService";
        int nbArgs = 3;
#else
        int nbArgs = 1;
#endif
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(nbArgs, 
              String::to_char_array(String::to_strings(serverArgs)), orb_strategies);
 
        if (unit_tests) 
        {
            // Starting the Name Service : "nameserv -OAport <port>"           
            std::cout << "start name service" << std::endl;
#if (CDMW_ORB_VDR == orbacus )
            std::string namingservice="nameserv";
            std::cout << namingservice.c_str() << " -OAport " << nameServicePort.c_str() << std::endl;
            idNaming = OS::create_process (namingservice, " -OAport " + nameServicePort);
#elif (CDMW_ORB_VDR == tao )
            std::string namingservice="Naming_Service";
            std::cout << namingservice.c_str() << " -ORBEndpoint iiop://localhost:" << nameServicePort.c_str() << std::endl;
            idNaming = OS::create_process( namingservice, "-ORBEndpoint iiop://localhost:" + nameServicePort );
#endif
            OS::sleep(timescale*3000);

            // Starting the Repository Simulator :
            // "simulated_repository -OAport <port> 
            //  -ORBnaming corbaloc::localhost:<port>/NameService
            //  -DOM CDMW/SERVICES -DOM CDMW/ADMIN -DOM CDMW/SERVICES/EVENT ...
            //  -ROOT AdminRootContext"
            std::string repository = "cdmw_simulated_repository";
            std::string namingCorbaloc =            
            " -ORBInitRef NameService=corbaloc:iiop:localhost:" + nameServicePort + "/NameService";
    
            std::string repositoryArgs ("");
#if (CDMW_ORB_VDR == orbacus )
            repositoryArgs += " -OAport ";
#elif (CDMW_ORB_VDR == tao )
            repositoryArgs += " -ORBEndpoint iiop://localhost:";
#endif 
            repositoryArgs += repositoryPort;
            repositoryArgs += namingCorbaloc;
            repositoryArgs += " -DOM ";
            repositoryArgs += Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN;
            repositoryArgs += " -DOM ";
            repositoryArgs += Cdmw::Common::Locations::CDMW_EVENT_SERVICE_NAME_DOMAIN;
            repositoryArgs += " -DOM CDMW/SERVICES/EVENT/FACTORIES";
            repositoryArgs += " -DOM CDMW/SERVICES/EVENT/EVENT_CHANNELS";
            repositoryArgs += " -DOM CDMW/SERVICES/EVENT/TYPED_EVENT_CHANNELS";
            repositoryArgs += " -ROOT ";
            repositoryArgs += Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID;
            
            std::cout << "start repository simulator" << std::endl;
            std::cout << repository.c_str() << repositoryArgs.c_str() << std::endl;
            
            idRepository = OS::create_process (repository, repositoryArgs);
            
            OS::sleep(timescale*3000);            
            std::cout << "Simrepository started!" << std::endl;
            
            // init repository 
            initRepository(orb.in());
            std::cout << "Simrepository initialised!" << std::endl;
            
            // Getting callback IOR
            callback = get_process_callback(orb.in());
            callbackIOR = orb->object_to_string(callback.in());
        }

        // Starting the EventChannelProfileManager
        std::string profileManagerService="cdmw_event_channel_manager";
        std::string managerArgs=Options::CALLBACK_IOR_OPTION;

        managerArgs += "=" + callbackIOR + " "
            + Options::PROCESS_PORT_OPTION + "=15300 "
            + Options::PROCESS_XML_FILE_OPTION + "=Server.xml "
            + "--ProfileXMLFile=EventChannelConf1.xml " 
            + Options::VALIDATE_XML_OPTION;
     
        //std::string dummy;
        std::cout << " launch EventChannelManager " << managerArgs;
        //std::cin >> dummy;
        idProfileManager
            = OS::create_process(profileManagerService, managerArgs);
        OS::sleep(timescale*5000);
        std::cout << "EventChannelProfileManager started" << std::endl;

        CdmwPlatformMngt::ProcessDelegate_var proc = CdmwPlatformMngt::ProcessDelegate::_nil();
        std::string proc_url("corbaname::localhost:");
        proc_url += nameServicePort;
        proc_url += "#CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_000";
        CORBA::Object_var obj = orb->string_to_object(proc_url.c_str());

        if (!CORBA::is_nil(obj.in())) 
        {
            proc = CdmwPlatformMngt::ProcessDelegate::_narrow(obj.in());
        }
        else
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                << " IN FILE: " << __FILE__ << std::endl;
            return FAILURE;
        }

        Cdmw::Tools::ProcessAdmin admin(orb.in(),proc.in());
        
        // send initialise order
        std::string commands("init LAST 2\n exit \n");
        std::istringstream commands_is(commands);
        admin.run(commands_is,std::cout);
        OS::sleep(timescale*5000);
        
        // send run order
        commands = "run\n exit \n";
        commands_is.str(commands);
        admin.run(commands_is,std::cout);

        status = run(orb.in(), &admin);
        
        Cdmw::NamingAndRepository::RepositoryInterface::finish();
    }
    catch(const CORBA::Exception& ex)
    {
        std::cerr << ex << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        status = FAILURE;
    }

    if (!CORBA::is_nil(orb.in()))
    {
         try
         {
         Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
         orb->destroy();
         }
         catch(const CORBA::Exception& ex)
         {
             std::cerr << ex << std::endl;
             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                       << " IN FILE: " << __FILE__ << std::endl;
             status = FAILURE;
         }
    }        

    // And we kill launched process
    try 
    {
        OS::kill_process(idNaming);
        OS::kill_process(idRepository);
    }
    catch (Cdmw::BadParameterException& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        status = FAILURE;
    }
    catch (Cdmw::PermissionDeniedException& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        status = FAILURE;
    }
    catch (Cdmw::InternalErrorException& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        status = FAILURE;
    }      

    // Force the killing of eventchannelmanager even if we've already sent a stop order
    try 
    {
        OS::kill_process(idProfileManager);
    }
    catch (...)
    {
        // ignore
    }
    return status;    
}


