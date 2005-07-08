/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


// Standard include files
#include <iostream>
#include <string>

#include <Foundation/ossupport/OS.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Locations.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>

// Cdmw CCM Files
#include <CCMContainer/ccmcommon/Cookie_impl.hpp>
#include <CCMContainer/ccmcontainer/Container_impl.hpp>
#include <CCMContainer/ccmcontainer/ConfigValue_impl.hpp>
#include <CCMContainer/ccmcontainer/HomeAllocatorRegistry.hpp>
#include <CCMContainer/ccmcontainer/HomesServantActivator.hpp>
#include <CCMContainer/ccmcif/ComponentPortDescription_impl.hpp>
#include <CCMContainer/ccmcif/PortDescription_impl.hpp>
#include <CCMContainer/ccmcif/FacetDescription_impl.hpp>
#include <CCMContainer/ccmcif/ReceptacleDescription_impl.hpp>
#include <CCMContainer/ccmcif/ConnectionDescription_impl.hpp>
#include <CCMContainer/ccmcif/SubscriberDescription_impl.hpp>
#include <CCMContainer/ccmcif/EmitterDescription_impl.hpp>
#include <CCMContainer/ccmcif/PublisherDescription_impl.hpp>
#include <CCMContainer/ccmcif/ConsumerDescription_impl.hpp>


#include "testftccmcif/SessionComponentWithFacetHome_impl.hpp"
#include "testftccmcif/SessionComponentWithConsumerHome_impl.hpp"
#include "testftccmcif/ComponentInstallationStub.hpp"
#include "testftccmcif/TestThread.hpp"


const static int SUCCESS = 0;
const static int FAILURE = 1;  
const static int POA_THREAD_POOL_SIZE = 5;   
const static int TIMESCALE = Cdmw::TestUtils::Testable::get_timescale();
const static std::string NAME_SERVICE_PORT = "5010";
const static std::string REPOSITORY_PORT   = "5020";
const static std::string FT_MANAGER_PORT   = "4555";
const static std::string ApplicationName   = "TestFtCcmCif";
const static std::string HostName          = "localhost";


using namespace Cdmw::OsSupport;


void kill_process(OS::ProcessId pid)
{
    try 
    {
        if (pid != 0)
            OS::kill_process(pid);
    }
    catch (Cdmw::Exception& e)
    {
        std::cerr << "Cannot kill process with pid " << pid 
                  << ":\n" << e.what() << std::endl;
    }
}




OS::ProcessId start_name_service(CORBA::ORB_ptr orb) 
    throw (CORBA::Exception)
{
    OS::ProcessId idNaming = 0;
    try {
        // Starting the Name Service
        std::cout << "start name service" << std::endl;

        #if (CDMW_ORB_VDR == orbacus )
            std::string namingservice="nameserv";
            std::string namingservice_opt=" -OAport " + NAME_SERVICE_PORT;
        #elif (CDMW_ORB_VDR == tao )
            std::string namingservice="Naming_Service";
            std::string namingservice_opt=" -ORBEndpoint iiop://localhost:" + NAME_SERVICE_PORT;
        #endif
        std::cout << namingservice.c_str() << namingservice_opt << std::endl;
        idNaming = OS::create_process( namingservice, namingservice_opt );

        OS::sleep(TIMESCALE*3000);
    
        // Register the Name Service into ORB
        std::string nsCorbaloc = "corbaloc::localhost:" + NAME_SERVICE_PORT + "/NameService";
        CORBA::Object_var nsObj = 
            orb->string_to_object(nsCorbaloc.c_str());
        orb->register_initial_reference("NameService", nsObj.in());
    } catch (...) {
        kill_process(idNaming);
        throw;
    }
    
    return idNaming;
}



OS::ProcessId start_sim_repository(CORBA::ORB_ptr orb,
                                   CosNaming::NamingContextExt_ptr naming) 
    throw (CORBA::Exception)
{
    OS::ProcessId idRepository = 0;
    try {
        CORBA::String_var namingIOR = orb->object_to_string(naming);
    
        // Starting the Repository Simulator :
        // "simulated_repository -OAport <port> 
        //  -ORBnaming corbaloc::localhost:<port>/NameService
        //  -DOM CDMW/SERVICES -DOM CDMW/ADMIN -DOM CDMW/SERVICES/EVENT ...
        //  -ROOT AdminRootContext"
        std::string repository = "cdmw_simulated_repository";
        std::string namingOption = " -ORBInitRef NameService=";
        namingOption += namingIOR.in();
        
        std::string repositoryArgs ("");
    #if (CDMW_ORB_VDR == orbacus )
        repositoryArgs += " -OAport ";
    #elif (CDMW_ORB_VDR == tao )
        repositoryArgs += " -ORBEndpoint iiop://localhost:";
    #endif
        repositoryArgs += REPOSITORY_PORT;
        repositoryArgs += namingOption;
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
        
        OS::sleep(TIMESCALE*10000);            
        std::cout << "Simrepository started!" << std::endl;
    
        // init repository 
        const char* REPOSITORY_NAME = "CDMW.I/repository.simulated";
    
        // Get reference to the repository interface
        CdmwNamingAndRepository::Repository_var rep =
            CdmwNamingAndRepository::Repository::_nil();
        try {
            Cdmw::NamingAndRepository::NamingInterface ni(naming);
            CORBA::Object_var obj__ = ni.resolve(REPOSITORY_NAME);
            rep = CdmwNamingAndRepository::Repository::_narrow(obj__.in());
            
            if (CORBA::is_nil(rep.in())) {
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
            Cdmw::NamingAndRepository::RepositoryInterface::init(
                Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN, 
                rep.in());
        } catch (...) {
            std::cerr << "Couldn't init Repository"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
            throw;
        }
    
        OS::sleep(TIMESCALE*3000);
        std::cout << "Simrepository initialised!" << std::endl;
    
    } catch (...) {
        kill_process(idRepository);
        throw;
    }
    
    return idRepository;
}
   


OS::ProcessId start_ft_manager(CORBA::ORB_ptr orb)
    throw (CORBA::Exception)
{
    OS::ProcessId idFTManager = 0;
    try {
        std::cout<<"Start the fault manager"<<std::endl;
        std::string managerArgs(" --CdmwXMLFile=CdmwFaultToleranceManager_loose_conf.xml");
        idFTManager = OS::create_process("cdmw_ft_manager", managerArgs);

        OS::sleep(TIMESCALE*15000);
        
        std::cout<<"Get the fault manager reference"<<std::endl;
        std::string fm_corbaloc = "corbaloc::localhost:" + FT_MANAGER_PORT + "/fault_manager";
        CORBA::Object_var fm_obj = 
            orb->string_to_object(fm_corbaloc.c_str());
            
        if (CORBA::is_nil(fm_obj.in())) {
            std::cerr << "Couldn't retrieve reference of the FTManager"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENT,
                                   CORBA::COMPLETED_NO);
        }
            
        CdmwFT::FTManager_var fault_manager = 
            CdmwFT::FTManager::_narrow( fm_obj.in() );

        std::cout<<"Get the replication manager reference"<<std::endl;
        CdmwFT::ReplicationManager_var rm =  
            fault_manager->get_replication_manager();
        if ( CORBA::is_nil(rm.in()) ) {
            std::cerr << "Couldn't retrieve reference of the ReplicationManager"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENT,
                                   CORBA::COMPLETED_NO);
        }
            
        orb->register_initial_reference("ReplicationManager", rm.in());
    
    } catch (...) {
        OS::kill_process(idFTManager);
        throw;
   }
   
   return idFTManager;
}  
   

   
void register_value_factories(CORBA::ORB_ptr orb) 
    throw (CORBA::Exception)
{
    // declare Value factory to the orb
#if CDMW_ORB_VDR == tao && CDMW_ORB_VER < 14
    CORBA_ValueFactoryBase_var vf, old_vf;
#else
    CORBA::ValueFactoryBase_var vf, old_vf;
#endif

    vf = new Cdmw::CCM::CIF::FacetDescriptionFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/FacetDescription:1.0", vf.in());
   
    vf = new Cdmw::CCM::CIF::PortDescriptionFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/PortDescription:1.0", vf.in());
    
    vf = new Cdmw::CCM::Common::CookieFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/Cookie:1.0", vf.in());
    
    vf = new Cdmw::CCM::CIF::ConnectionDescriptionFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/ConnectionDescription:1.0", vf.in());
    
    vf = new Cdmw::CCM::CIF::ReceptacleDescriptionFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/ReceptacleDescription:1.0", vf.in());
    
    vf = new Cdmw::CCM::CIF::ConsumerDescriptionFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/ConsumerDescription:1.0", vf.in());
    
    vf = new Cdmw::CCM::CIF::EmitterDescriptionFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/EmitterDescription:1.0", vf.in());
    
    vf = new Cdmw::CCM::CIF::SubscriberDescriptionFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/SubscriberDescription:1.0", vf.in());
    
    vf = new Cdmw::CCM::CIF::PublisherDescriptionFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/PublisherDescription:1.0", vf.in());
    
    vf = new Cdmw::CCM::CIF::ComponentPortDescriptionFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/ComponentPortDescription:1.0", vf.in());

    vf = new Cdmw::CCM::Container::ConfigValueFactory();
    old_vf = orb->register_value_factory ("IDL:omg.org/Components/ConfigValue:1.0", vf.in());
}



OS::ProcessId start_component_server(std::string processName,
                                     CORBA::ORB_ptr orb)
    throw (CORBA::Exception)
{
    //
    // get ReplicationManager
    //
    CORBA::Object_var obj;
    try {
        obj = orb->resolve_initial_references("ReplicationManager");
    } catch (const CORBA::ORB::InvalidName&) {
        std::cerr<< "Can't resolve `ReplicationManager'" << std::endl;
        throw;
    }
    
    CORBA::String_var repMgrIOR = orb->object_to_string(obj.in());
    std::string args = " --ProcessName=" + processName + 
                       " -ORBInitRef ReplicationManager=" + repMgrIOR.in();
    
    std::cout << "Start ComponentServer " << processName 
              << ":\n" << "mainComponentServer " << args
              << std::endl;
    return OS::create_process("mainComponentServer", args);
}

   

int
run(CORBA::ORB_ptr orb,
    FT::Location primLocation,
    FT::Location backLocation,
    Components::Deployment::Container_ptr primContainer,
    Components::Deployment::Container_ptr backContainer)
{
    TestFtCcmCif::TestThread test(orb, 
                                  primLocation,
                                  backLocation,
                                  primContainer, 
                                  backContainer);

    test.start();
    orb->run();
    test.join();

    return SUCCESS;
}
  
   
int main(int argc, char* argv[])
{
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;

    int status = SUCCESS;
    CORBA::ORB_var orb;
    OS::ProcessId idNaming = 0; 
    OS::ProcessId idRepository = 0; 
    OS::ProcessId idFTManager = 0;
    OS::ProcessId idCompSrv1 = 0;
    OS::ProcessId idCompSrv2 = 0;

    try
    {
        // Initialise FT service
        Cdmw::FT::FTServiceInit::init(argc, argv, true);

        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
        orb_strategies.add_multicast();

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

        //
        // Start naming service
        //
        idNaming = start_name_service(orb.in());
    
        //
        // Get naming service
        //
        CORBA::Object_var obj;
        try {
            obj = orb -> resolve_initial_references("NameService");
        } catch(const CORBA::ORB::InvalidName&) {
            std::cerr<< "Can't resolve `NameService'" << std::endl;
            throw;
        }
        if(CORBA::is_nil(obj.in())) {
            std::cerr << "`NameService' is a nil object reference" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
        }

        CosNaming::NamingContextExt_var nc;
        try {
            nc = CosNaming::NamingContextExt::_narrow(obj.in());
        } catch (...) {
            std::cerr << "Obj is not a NamingContextExt" << std::endl;
            throw;
        }
        if(CORBA::is_nil(nc.in())) {
            std::cerr <<"`NameService' is not a NamingContextExt object reference"<< std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
        }
    
        //
        // Start SimRepository
        //
        idRepository = start_sim_repository(orb.in(), nc.in());
    
        //
        // Start FTManager
        //
        idFTManager = start_ft_manager(orb.in());
    
        //
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
        // Register valuetype factories
        //
        register_value_factories(orb.in());
        
        //
        // Start Primary ComponentServer
        //
        std::string primProcessName = "PrimaryCompSrv";
        idCompSrv1 = start_component_server(primProcessName.c_str(), orb.in());
        //
        // Start Backup ComponentServer
        //
        std::string backProcessName = "BackupCompSrv";
        idCompSrv2 = start_component_server(backProcessName.c_str(), orb.in());
        
        OS::sleep (TIMESCALE*10000);
        
        //
        // get primary container
        //
        std::cout << "Get container from PrimaryCompSrv" << std::endl;
        std::string primContainerIOR = "file://" + primProcessName + "_container.IOR";
        CORBA::Object_var primContainerObj = 
            orb->string_to_object(primContainerIOR.c_str());
        Components::Deployment::Container_var primContainer =
            Components::Deployment::Container::_narrow(primContainerObj.in());
        
        //
        // get backup container
        //
        std::cout << "Get container from BackupCompSrv" << std::endl;
        std::string backContainerIOR = "file://" + backProcessName + "_container.IOR";
        CORBA::Object_var backContainerObj = 
            orb->string_to_object(backContainerIOR.c_str());
        Components::Deployment::Container_var backContainer =
            Components::Deployment::Container::_narrow(backContainerObj.in());
       
        
        std::cout << "main server initialised" << std::endl;
        
        //
        // Create Locations
        //
        ::FT::Location primLoc;
        primLoc.length(3);
        primLoc[0].id = HostName.c_str();
        primLoc[0].kind = "hostname";
        primLoc[1].id = ApplicationName.c_str();
        primLoc[1].kind = "application";
        primLoc[2].id = primProcessName.c_str();
        primLoc[2].kind = "process";    
        ::FT::Location backLoc;
        backLoc.length(3);
        backLoc[0].id = HostName.c_str();
        backLoc[0].kind = "hostname";
        backLoc[1].id = ApplicationName.c_str();
        backLoc[1].kind = "application";
        backLoc[2].id = backProcessName.c_str();
        backLoc[2].kind = "process";    
        
        // Main loop
        status = run(orb.in(),
                     primLoc,
                     backLoc,
                     primContainer.in(), 
                     backContainer.in());

        OS::sleep (TIMESCALE*3000);

    } catch (const CORBA::Exception& e) {
        std::cerr << "Exception: " 
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e)
                  << std::endl;
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
            std::cerr << "Error during cleanup: " << ex << std::endl;
            status = FAILURE;
        } 
    }            
    
    kill_process(idNaming);
    kill_process(idRepository);
    kill_process(idFTManager);
    kill_process(idCompSrv1);
    kill_process(idCompSrv2);
        
    return status;
}







