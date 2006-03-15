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
#include <cstdlib>
#include <string>
#include <sstream>


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

#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.stub.hpp"

#include "test_interface/ClientThread.hpp"

#include "SystemMngt/tools/ProcessAdmin.hpp"

#include "Event/idllib/CosEventComm.skel.hpp"
#include "Event/idllib/CosEventComm.stub.hpp"
#include "Event/idllib/CosTypedEventComm.skel.hpp"
#include "Event/idllib/CosTypedEventComm.stub.hpp"

#include "Event/idllib/CdmwEvent.stub.hpp"

#include "test_interface/Echo.skel.hpp"

#include "Event/eventinterface/EventChannel.hpp"
#include "Event/eventinterface/TypedEventChannel.hpp"


namespace {

    const char* CHANNEL_1_NAME = "TestEventChannel1";
    const char* CHANNEL_2_NAME = "TestEventChannel2";
    const char* CHANNEL_3_NAME = "TestEventChannel3";
    const char* CHANNEL_4_NAME = "TestEventChannel4";
    const char* COMPLETE_CHANNEL_1_NAME = "EVENT/EVENT_CHANNELS/TestEventChannel1";
    const char* COMPLETE_CHANNEL_2_NAME = "EVENT/EVENT_CHANNELS/TestEventChannel2";
    const char* COMPLETE_CHANNEL_3_NAME = "EVENT/EVENT_CHANNELS/TestEventChannel3";
    const char* COMPLETE_CHANNEL_4_NAME = "EVENT/EVENT_CHANNELS/TestEventChannel4";

    const char* TYPED_CHANNEL_1_NAME = "TestTypedEventChannel1";
    const char* TYPED_CHANNEL_2_NAME = "TestTypedEventChannel2";
    const char* TYPED_CHANNEL_3_NAME = "TestTypedEventChannel3";
    const char* TYPED_CHANNEL_4_NAME = "TestTypedEventChannel4";
    const char* COMPLETE_TYPED_CHANNEL_1_NAME = "EVENT/EVENT_CHANNELS/TestTypedEventChannel1";
    const char* COMPLETE_TYPED_CHANNEL_2_NAME = "EVENT/EVENT_CHANNELS/TestTypedEventChannel2";
    const char* COMPLETE_TYPED_CHANNEL_3_NAME = "EVENT/EVENT_CHANNELS/TestTypedEventChannel3";
    const char* COMPLETE_TYPED_CHANNEL_4_NAME = "EVENT/EVENT_CHANNELS/TestTypedEventChannel4";

    const CORBA::String_var INTERFACE_REF=CORBA::string_dup("IDL:test/TypedEcho:1.0");

    const std::string CDMW_IDL_PATH_OPTION       = "--CdmwIdlPath";
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
        << "--CdmwIdlPath=<path>      Path of Cdmw Idl.\n"
        << "[--NamePort=<nbr>]         Port of Name Service.\n"
        << "[--RepPort=<nbr>]          Port of Name and Repository.\n"
        << std:: endl;
}


class PushConsumer_impl : virtual public POA_CosEventComm::PushConsumer,
                          virtual public PortableServer::RefCountServantBase
{
    CORBA::ORB_var orb_; // The ORB
    PortableServer::POA_var poa_; // My POA.
    CosEventComm::PushConsumer_var consumer_;  // consumer on eventchannel2

public:
    
    PushConsumer_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, CosNaming::NamingContext_ptr nc)
        : orb_(CORBA::ORB::_duplicate(orb)),
           poa_(PortableServer::POA::_duplicate(poa))
    {
        Cdmw::Event::EventChannel* evtchannel;

        try
        {
            evtchannel = new Cdmw::Event::EventChannel(COMPLETE_CHANNEL_2_NAME, nc);
            consumer_ = evtchannel->get_pushConsumer(CosEventComm::PushSupplier::_nil());
        }
        catch (...)
        {
            std::cerr << "Exception raised during PushConsumer_impl init" << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }
    }

     virtual
    ~PushConsumer_impl()
    {
    }
    
    void
    push(const CORBA::Any& data)
    throw(CORBA::SystemException)
    {
          const char* str;
        CORBA::Any sendData;
         
          if (data >>= str)
        {
           std::cout << "Consumer1 has received: " << str << std::endl;
        }
          else
        {
              std::cout << "Consumer1 has received an unknown event." << std::endl;
        }
        
        consumer_->push(data);
    }

    void
    disconnect_push_consumer()
        throw(CORBA::SystemException)
    {
        std::cout << "Disconnect" << std::endl;
        PortableServer::ObjectId_var oid = poa_ -> servant_to_id(this);
        poa_ -> deactivate_object(oid.in());

         orb_ -> shutdown(false);
    }

    PortableServer::POA_ptr
    _default_POA()
    {
        return PortableServer::POA::_duplicate(poa_.in());
    }    
}; // End PushConsumer_impl class

// ----------------------------------------------------------------------
// Echo_impl implementation  (typed event push consumer)
// ----------------------------------------------------------------------
class Echo_impl : virtual public POA_test::TypedEcho,
                  virtual public PortableServer::RefCountServantBase
{
    CORBA::ORB_var orb_; // The ORB
    PortableServer::POA_var poa_; // My POA.
    test::TypedEcho_var consumer_;  // consumer on eventchannel2


public:
    
    Echo_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, CosNaming::NamingContext_ptr nc)
        : orb_(CORBA::ORB::_duplicate(orb)),
           poa_(PortableServer::POA::_duplicate(poa))
    {
        Cdmw::Event::TypedEventChannel* evtchannel;

        try
        {
            evtchannel = new Cdmw::Event::TypedEventChannel(COMPLETE_TYPED_CHANNEL_2_NAME, nc);
            consumer_ = 
                evtchannel->get_typedPushConsumer<test::TypedEcho>(INTERFACE_REF,
                                                  CosEventComm::PushSupplier::_nil());

        }
        catch (...)
        {
            std::cerr << "Exception raised during Echo_impl init" << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }

    }
    
    //
    // IDL to C++ mapping
    //
    void
    echo_string(const char* str)
    throw(CORBA::SystemException)
    {
        std::cout << "Consumer1 has received: " << str << std::endl;
        consumer_->echo_string(str);
    }

    void
    disconnect_push_consumer()
    throw(CORBA::SystemException)
    {
        std::cout << "Disconnect";
        PortableServer::ObjectId_var oid = poa_ -> servant_to_id(this);
        poa_ -> deactivate_object(oid.in());

         orb_ -> shutdown(false);
    }

    void
    push(const CORBA::Any& data)
    throw(CORBA::SystemException)
    {
         std::cout << "TypedPushEventClient.cpp : push any not implemented" << std::endl;
    }
    
    PortableServer::POA_ptr
    _default_POA()
    {
        return PortableServer::POA::_duplicate(poa_.in());
    }    
}; // End class Echo_impl

    
const int SUCCESS = 0;
const int FAILURE = 1;  

const int POA_THREAD_POOL_SIZE = 5;   

const char* FACTORY_SERVER_IOR = "FactoryServerIor";

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
    
    CosNaming::NamingContextExt_var nc;
    try
    {
        nc = CosNaming::NamingContextExt::_narrow(obj.in());
    }
    catch (...) 
    {
        std::cerr << "Obj is not a NamingContextExt" << std::endl;
        throw;
    }

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
run(CORBA::ORB_ptr orb, const char* actualAgentIOR)
{
     Cdmw::Event::ClientThread client(orb, actualAgentIOR);

     client.start();
    orb->run();
    client.join();

    return SUCCESS;
}


int main(int argc, char* argv[])
{
     using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;
    int status = SUCCESS;
    CORBA::ORB_var orb;
    CdmwPlatformMngt::ProcessCallback_var callback;
       CosEventChannelAdmin::EventChannel_var channel;

    OS::ProcessId idNaming = 0, idRepository = 0; 
    OS::ProcessId idProfileManager;
    OS::ProcessId idEventAdmin;
#if CDMW_ORB_VDR == orbacus        
    OS::ProcessId idEventServer;
    OS::ProcessId idIrServ;
    OS::ProcessId idTypedEventServer;
#endif
    int timescale = Cdmw::TestUtils::Testable::get_timescale();

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

    // get Cdmw Idl path
    std::string cdmwIdlPath = OS::get_option_value (argc, argv, CDMW_IDL_PATH_OPTION);
    if (cdmwIdlPath == "no"|| cdmwIdlPath == "yes") 
    {
        std::cout << CDMW_IDL_PATH_OPTION.c_str() << "=<path> option must be defined\n";
        std::cout << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
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
            
            OS::sleep(timescale*10000);            
            std::cout << "Simrepository started!" << std::endl;
            
            // init repository 
            initRepository(orb.in());
            OS::sleep(timescale*3000);
            std::cout << "Simrepository initialised!" << std::endl;
            
            // Getting callback IOR
            callback = get_process_callback(orb.in());
            callbackIOR = orb->object_to_string(callback.in());
        }
        
        // Starting the EventChannelManager
        OS::sleep(timescale*3000);
        std::string profileManagerService="cdmw_event_channel_manager";
#if (CDMW_ORB_VDR == orbacus )
        std::string managerArgs=" -ORBconfig obtest.conf ";
#else 
        std::string managerArgs="";
#endif
        managerArgs += Options::CALLBACK_IOR_OPTION;
        managerArgs += "=" + callbackIOR + " " 
                    + Options::PROCESS_PORT_OPTION + "=15327 " 
                    + Options::LOCALISATION_SERVICE_OPTION+ "=15328 " +
                    " --ProfileXMLFile=EventChannelConf1.xml " 
                    + Options::PROCESS_XML_FILE_OPTION + "=Server.xml "
                          + Options::VALIDATE_XML_OPTION;
        //std::string dummy;
        
        idProfileManager
            = OS::create_process(profileManagerService, managerArgs);
        OS::sleep(timescale*10000);
        std::cout << "EventChannelManager started" << std::endl;

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

#if CDMW_ORB_VDR == orbacus        
        // Starting Orbacus Eventserver
        idEventServer = OS::create_process("eventserv", " -OAport 21410 -ORBconfig obtest.conf");
        OS::sleep(timescale*10000);
        std::cout << "Eventserv started" << std::endl;

        // Starting irserv (needed by Orbacus typed event service)
        std::string irservArgs = "-ORBconfig obtest.conf -I";        
        irservArgs += cdmwIdlPath;
        irservArgs += " Echo.idl";
     
        idIrServ = OS::create_process("irserv", irservArgs);
        OS::sleep(timescale*10000);
        std::cout << "irserv started" << std::endl;

        // Starting Typed Orbacus Eventservice
        idTypedEventServer = OS::create_process("eventserv", " -t -OAport 21420 -ORBconfig obtest.conf");
        OS::sleep(timescale*10000);
        std::cout << "TypedEventserv started" << std::endl;
        OS::sleep(timescale*5000);
#endif      
      
        // Starting the cdmw_event_admin which creates event channels
        // This process stops itself

        
        OS::sleep(timescale*5000);

        std::string eventAdmin = "cdmw_event_admin";
        std::string adminArgs = " -ORBInitRef NameService=corbaloc:iiop:localhost:" 
                              + repositoryPort
                              +"/NameService --commandFile-name=ec_creation.cmd";
        std::cout<<eventAdmin<<" "<<adminArgs<<std::endl;

        try
        {            
            idEventAdmin = OS::create_process(eventAdmin, adminArgs);
        }
        catch(Cdmw::BadParameterException& e)
        {
            std::cerr << "Unexpected exception raised: " << e.what() << std::endl;
            throw;
        }
        catch(Cdmw::OutOfMemoryException& e)
        {
            std::cerr << "Unexpected exception raised: " << e.what() << std::endl;
            throw;
        }
         catch(Cdmw::InternalErrorException& e)
        {
            std::cerr << "Unexpected exception raised: " << e.what() << std::endl;
            throw;
        }
       
        std::cout << "start cdmw_event_admin " << adminArgs.c_str()  << std::endl;
        OS::sleep(timescale*10000);

        // Get the root POA 
        //
        obj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
        //
        // Run implementation (Activate the POA manager)
        //
        PortableServer::POAManager_var poaManager = rootPOA -> the_POAManager();
        poaManager -> activate();
 
        //
        // Get NamingInterface object from RepositoryInterface
        //
        Cdmw::CommonSvcs::Naming::NamingInterface ni
            = Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(Locations::CDMW_SERVICES_NAME_DOMAIN);   

        // Create consumer 
        PushConsumer_impl* pcImpl = new PushConsumer_impl(orb.in(), rootPOA.in(), ni.name_context());
        Cdmw::Event::EventChannel* eventchannel1;

        std::cout << "Initialise eventchannel1" << std::endl;
        try
        {
            eventchannel1 = new Cdmw::Event::EventChannel(COMPLETE_CHANNEL_1_NAME, ni.name_context() );
        }
        catch (Cdmw::BadParameterException& e)
        {
            std::cerr << "Unexpected exception raised: " << e.what() << std::endl;
            throw;
        }
        
        std::cout << "Connect to eventchannel1" << std::endl;
        try
        {
            eventchannel1->connect_pushConsumer(pcImpl->_this());
        }
        catch (...)
        {
            std::cerr << "Unexpected exception raised!" << std::endl;
            throw;
        }

#if CDMW_ORB_VDR == orbacus        
        // Create consumer 
        Echo_impl* typedpcImpl = new Echo_impl(orb, rootPOA, ni.name_context());
        Cdmw::Event::TypedEventChannel* typedeventchannel1;

        std::cout << "Initialise typedeventchannel1" << std::endl;
        try
        {
            typedeventchannel1 = new Cdmw::Event::TypedEventChannel(COMPLETE_TYPED_CHANNEL_1_NAME, ni.name_context() );
        }
        catch (Cdmw::BadParameterException& e)
        {
            std::cerr << "Unexpected exception raised: " << e.what() << std::endl;
            throw;
        }
        
        std::cout << "Connect to typedeventchannel1" << std::endl;
        try
        {
            typedeventchannel1->connect_typedPushConsumer(INTERFACE_REF,
                                                         typedpcImpl->_this());
        }
        catch(...)
        {
            std::cerr << "Unexpected exception raised!" << std::endl;
            throw;
        }
#endif
        OS::sleep(timescale*1000);

          std::cout << "server initialised" << std::endl;

        status = run(orb.in(), callbackIOR.c_str());
        OS::sleep (timescale*2000);

        // send stop order
        commands = "stop\n exit \n";
        commands_is.str(commands);
        std::cout << "Stop cdmw_event_channel_manager" << std::endl;
        admin.run(commands_is,std::cout);

        OS::sleep (timescale*3000);

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
            Cdmw::NamingAndRepository::RepositoryInterface::finish();
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
#if CDMW_ORB_VDR == orbacus        
        OS::kill_process(idEventServer);
        OS::kill_process(idTypedEventServer);
        OS::kill_process(idIrServ);
#endif   
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

    return status;    
}


