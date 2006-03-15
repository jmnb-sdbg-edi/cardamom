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
#include <string>
#include <fstream>

#include <Foundation/ossupport/OS.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/Codec.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>
#include <FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp>
#include <FaultTolerance/ftlocationmanager/PrimaryBackupAdmin_impl.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>

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


#include "testftccmcif/FTSessionComponentWithFacetHome_impl.hpp"
#include "testftccmcif/SessionComponentWithConsumerHome_impl.hpp"
#include "testftccmcif/FTSessionComponentWithConsumerHome_impl.hpp"
#include "testftccmcif/ComponentInstallationStub.hpp"


const static int SUCCESS = 0;
const static int FAILURE = 1;  
const static int POA_THREAD_POOL_SIZE = 5;   
const static int TIMESCALE = Cdmw::TestUtils::Testable::get_timescale();

static std::string ProcessName     = "";
static std::string ApplicationName = "TestFtCcmCif";
static std::string HostName        = "localhost";



using namespace Cdmw::OsSupport;



void register_ft_location(CORBA::ORB_ptr orb)
    throw (CORBA::Exception)
{
    //
    // Create Location
    //
    ::FT::Location loc;
    loc.length(3);
    loc[0].id = HostName.c_str();
    loc[0].kind = "hostname";
    loc[1].id = ApplicationName.c_str();
    loc[1].kind = "application";
    loc[2].id = ProcessName.c_str();
    loc[2].kind = "process";    
    
        std::cout << "   -> Get Replication Manager" << std::endl;
    //
    // Get Replication Manager
    //
    CORBA::Object_var obj;
    try {
        obj = orb -> resolve_initial_references("ReplicationManager");
    } catch(const CORBA::ORB::InvalidName&) {
        std::cerr<< "Can't resolve `ReplicationManager'" << std::endl;
        throw;
    }
    if(CORBA::is_nil(obj.in())) {
        std::cerr << "`ReplicationManager' is a nil object reference" << std::endl;
        throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENT,
                               CORBA::COMPLETED_NO);
    }
    CdmwFT::ReplicationManager_var repMgr;
    try {
        repMgr = CdmwFT::ReplicationManager::_narrow(obj.in());
    } catch (...) {
        std::cerr << "Obj is not a ReplicationManager" << std::endl;
        throw;
    }

    //
    // Get the root POA 
    //
    CORBA::Object_var rootPOAobj = orb -> resolve_initial_references("RootPOA");
    PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(rootPOAobj.in());

        std::cout << "   -> Get PrimaryBackupGroupRepository object" << std::endl;
    //
    // Get PrimaryBackupGroupRepository object
    //
    Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primary_backup_repository =
        Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();
        
        std::cout << "   -> Create PrimaryBackupAdmin" << std::endl;
    //
    // Create PrimaryBackupAdmin object
    //
    ::Cdmw::FT::Location::PrimaryBackupAdmin_impl * primary_backup_admin
        = new ::Cdmw::FT::Location::PrimaryBackupAdmin_impl(orb, 
                                                            rootPOA.in(), 
                                                            loc, 
                                                            primary_backup_repository);
    ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin_ref 
        = primary_backup_admin->_this();
    
        std::cout << "   -> register_location" << std::endl;
    //
    // Register PrimaryBackupAdmin
    //
    repMgr->register_location(loc, primary_backup_admin_ref.in());
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

   

PortableServer::POA_ptr
create_container_POA(PortableServer::POA_ptr parent)
{
    //
    // Create a POA with Retain and Single-thread policicies
    // to be used to activate the ComponentServer and Container objects.
    // Lifespan policy            = TRANSIENT
    // Id Assignment policy       = SYSTEM_ID
    // Id Uniqueness policy       = UNIQUE_ID
    // Implicit Activation policy = IMPLICIT_ACTIVATION
    // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY
    // Servant Retention policy   = RETAIN;
    // Thread Policy              = SINGLE_THREAD_MODEL
    //        
    using namespace PortableServer;
    PortableServer::POAManager_var poaManager = parent -> the_POAManager();
    CORBA::PolicyList policies;
    policies.length(7);
    policies[0] = parent -> create_id_assignment_policy       (SYSTEM_ID);
    policies[1] = parent -> create_lifespan_policy            (TRANSIENT);
    policies[2] = parent -> create_servant_retention_policy   (RETAIN);
    policies[3] = parent -> create_id_uniqueness_policy       (UNIQUE_ID);
    policies[4] = parent -> create_request_processing_policy  (USE_ACTIVE_OBJECT_MAP_ONLY);
    policies[5] = parent -> create_implicit_activation_policy (IMPLICIT_ACTIVATION);
    policies[6] = parent -> create_thread_policy              (SINGLE_THREAD_MODEL);

    Cdmw::OrbSupport::StrategyList poaStrategies;

    const char* CONTAINER_POA_NAME = "ContainerPOA";
    PortableServer::POA_var container_poa =
        Cdmw::OrbSupport::OrbSupport::create_POA(parent,
                                                  CONTAINER_POA_NAME,
                                                  poaManager.in(), 
                                                  policies,
                                                  poaStrategies);
    return container_poa._retn();
}
    
    
   
Components::Deployment::Container_ptr
prepare_ccm_environment(CORBA::ORB_ptr orb)
    throw (CORBA::Exception)
{
        std::cout << "   -> register_value_factories" << std::endl;
    //
    //  register requiered valutype factories
    //    
    register_value_factories(orb);
    
        std::cout << "   -> Register HomeAllocators" << std::endl;
    //
    // Register HomeAllocators
    //
    typedef Cdmw::CCM::Container::HomeAllocator
            <Cdmw::CCM::CIF::CdmwTestFtCcmCif::ComponentsModule::FTSessionComponentWithFacetHome_impl> 
            FTComponentWithFacetHome_Allocator;
    Cdmw::CCM::Container::HomeAllocatorRegistry::Register(
        "Cdmw.CCM.CIF.CdmwTestFtCcmCif.ComponentsModule.FTSessionComponentWithFacetHome_impl",
        &FTComponentWithFacetHome_Allocator::TheAllocator);

    typedef Cdmw::CCM::Container::HomeAllocator
            <Cdmw::CCM::CIF::CdmwTestFtCcmCif::ComponentsModule::SessionComponentWithConsumerHome_impl> 
            ComponentWithConsumerHome_Allocator;
    Cdmw::CCM::Container::HomeAllocatorRegistry::Register(
        "Cdmw.CCM.CIF.CdmwTestFtCcmCif.ComponentsModule.SessionComponentWithConsumerHome_impl",
        &ComponentWithConsumerHome_Allocator::TheAllocator);

    typedef Cdmw::CCM::Container::HomeAllocator
            <Cdmw::CCM::CIF::CdmwTestFtCcmCif::ComponentsModule::FTSessionComponentWithConsumerHome_impl> 
            FTComponentWithConsumerHome_Allocator;
    Cdmw::CCM::Container::HomeAllocatorRegistry::Register(
        "Cdmw.CCM.CIF.CdmwTestFtCcmCif.ComponentsModule.FTSessionComponentWithConsumerHome_impl",
        &FTComponentWithConsumerHome_Allocator::TheAllocator);


    //
    // Get the root POA 
    //
    CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
    PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

    //
    // Create ComponentInstallation stub
    //
    TestFtCcmCif::ComponentInstallationStub* comp_install_stub
        = new TestFtCcmCif::ComponentInstallationStub(rootPOA.in());
    CdmwDeployment::ComponentInstallation_var comp_install
        = comp_install_stub->_this();

        std::cout << "   -> ConfigValues for Container" << std::endl;
    //
    // ConfigValues for Container
    //
    Components::ConfigValues config_values(2);
    config_values.length(1);
    CORBA::Any value;
    value <<= CdmwDeployment::SESSION;
    config_values[0]
        = new Cdmw::CCM::Container::ConfigValue_impl(CdmwDeployment::COMPONENT_KIND, value);

    //
    // POAs for Container
    //
    PortableServer::POA_var homes_serialize_poa;
    PortableServer::POA_var homes_multithread_poa;
    PortableServer::POA_var container_poa = create_container_POA(rootPOA.in());
    // Create a SingleThread POA for single threaded homes and a Multithread POA
    // for multithread homes.
    try {
        using namespace Cdmw::OrbSupport;             
        PortableServer::POAManager_var poaManager = container_poa -> the_POAManager();
        CORBA::PolicyList policies;
        policies.length(7);
        policies[0] =
            container_poa -> create_id_assignment_policy(PortableServer::USER_ID);
        policies[1] =
            container_poa -> create_lifespan_policy(PortableServer::PERSISTENT);
        policies[2] =
            container_poa -> create_servant_retention_policy(PortableServer::RETAIN);
        policies[3] =
            container_poa -> create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
        policies[4] = container_poa ->
            create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER);
        policies[5] = container_poa ->
            create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);
        // Create a single thread poa
        policies[6] = container_poa ->
            create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);
        
        StrategyList poaStrategies;
        
        const char* SINGLE_THREAD_HOMES_POA_NAME = "HomeSerializePOA";
        const char* MULTI_THREAD_HOMES_POA_NAME  = "HomeMultithreadPOA";
        
        homes_serialize_poa = Cdmw::OrbSupport::OrbSupport::create_POA(container_poa.in(),
                                                                        SINGLE_THREAD_HOMES_POA_NAME,
                                                                        poaManager.in(), 
                                                                        policies,
                                                                        poaStrategies);
        std::cout << "   -> set_servant_manager" << std::endl;
        //
        // Create and install servant activator
        //
        {
            PortableServer::ServantActivator_var the_activator =
                new Cdmw::CCM::Container::HomesServantActivator();
            homes_serialize_poa -> set_servant_manager(the_activator.in());
        }
    
        
        // Create a multi thread poa
        policies[6] = rootPOA ->
            create_thread_policy(PortableServer::ORB_CTRL_MODEL);
        
        homes_multithread_poa = Cdmw::OrbSupport::OrbSupport::create_POA(container_poa.in(),
                                                                          MULTI_THREAD_HOMES_POA_NAME,
                                                                          poaManager.in(), 
                                                                          policies,
                                                                          poaStrategies);
        std::cout << "   -> set_servant_manager 2" << std::endl;
        //
        // Create and install servant activator
        //
        {
            PortableServer::ServantActivator_var the_activator =
                new Cdmw::CCM::Container::HomesServantActivator();
            homes_multithread_poa -> set_servant_manager(the_activator.in());
        }
        
    } catch (const PortableServer::POA::AdapterAlreadyExists &) {
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                              CORBA::COMPLETED_YES);
    } catch (const PortableServer::POA::InvalidPolicy &) {
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                              CORBA::COMPLETED_YES);
    } catch (const CORBA::SystemException &) {
        throw; // rethrow
    } 
    
        std::cout << "   -> Create a container instance" << std::endl;
    // Create a container instance
    Cdmw::CCM::Container::Container_impl* container_impl 
        = new Cdmw::CCM::Container::Container_impl 
        (orb, 
         container_poa.in(), 
         homes_serialize_poa.in(),
         homes_multithread_poa.in(),
         comp_install.in(), 
         Components::Deployment::ComponentServer::_nil(),
         Components::HomeRegistration::_nil(),
         CdmwEvent::EventChannelFactory::_nil(),
         ProcessName.c_str(),
         ApplicationName.c_str(),
         config_values);
    
    Components::Deployment::Container_var container = container_impl->_this();

    return container._retn();
}
 


   
int main(int argc, char* argv[])
{
    using namespace Cdmw::OsSupport;

    int status = SUCCESS;
    CORBA::ORB_var orb;

    try
    {
        //
        // Get --ProcessName argument
        //
        ProcessName = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--ProcessName");
        if (ProcessName == "no") {
            std::cerr << "ERROR: executable " << argv[0] 
                      << " requires --ProcessName option" << std::endl;
            return FAILURE;
        }

        std::cout << "ComponentServer " << ProcessName << " starting..." << std::endl;
        
        // Initialise FT service
        Cdmw::FT::FTServiceInit::Init(argc, argv, false);

        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
        orb_strategies.add_multicast();

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

        // PCR-0049
        Cdmw::OrbSupport::CodecBase::init(orb.in());

        //
        // Register FT Location
        //
        register_ft_location(orb.in());
        
        //
        // Get the root POA 
        //
        CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
        //
        // Activate the POA manager
        //
        PortableServer::POAManager_var poaManager = rootPOA -> the_POAManager();
        poaManager -> activate();

        //
        // Prepare CCM environment
        //
        Components::Deployment::Container_var container = 
            prepare_ccm_environment(orb.in());

        //
        // Write Container's IOR into a file
        //
        std::string file_name = ProcessName + "_container.IOR";
        CORBA::String_var ref_string = orb->object_to_string(container.in());
        std::ofstream os(file_name.c_str());
        os << ref_string.in();
        os.close();
        
        std::cout << "ComponentServer "<< ProcessName <<" initialised" << std::endl;
        
        // Main loop
        orb->run();

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
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << "Error during cleanup: " << ex << std::endl;
            status = FAILURE;
        } 
    }            
    
    return status;
}







