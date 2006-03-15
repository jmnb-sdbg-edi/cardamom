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
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Event/idllib/CdmwEvent.stub.hpp"

#include "testccmcif/ClientThread.hpp"


// Cdmw CCM Files
#include <CCMContainer/ccmcommon/Cookie_impl.hpp>
#include <CCMContainer/ccmcif/ComponentPortDescription_impl.hpp>
#include <CCMContainer/ccmcif/PortDescription_impl.hpp>
#include <CCMContainer/ccmcif/FacetDescription_impl.hpp>
#include <CCMContainer/ccmcif/ReceptacleDescription_impl.hpp>
#include <CCMContainer/ccmcif/ConnectionDescription_impl.hpp>
#include <CCMContainer/ccmcif/SubscriberDescription_impl.hpp>
#include <CCMContainer/ccmcif/EmitterDescription_impl.hpp>
#include <CCMContainer/ccmcif/PublisherDescription_impl.hpp>
#include <CCMContainer/ccmcif/ConsumerDescription_impl.hpp>
#include <CCMContainer/ccmcontainer/Container_impl.hpp>
#include <CCMContainer/ccmcontainer/ConfigValue_impl.hpp>
#include <CCMContainer/ccmcontainer/HomeAllocatorRegistry.hpp>
#include <CCMContainer/ccmcontainer/HomesServantActivator.hpp>

#include "testccmcif/SessionServerHome_impl.hpp"
#include "testccmcif/SessionClientHome_impl.hpp"

#include "CCMContainer/idllib/CdmwDeployment.skel.hpp"





namespace {
    
    const std::string PROCESS_NAME = "DummyProcessName";
    const std::string APPLICATION_NAME = "DummyApplicationName";

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


class ComponentInstallation_plug_impl: virtual public POA_CdmwDeployment::ComponentInstallation,
                                       virtual public PortableServer::RefCountServantBase
{
    ComponentInstallation_plug_impl(const ComponentInstallation_plug_impl&);
    void operator=(const ComponentInstallation_plug_impl&);

    PortableServer::POA_var poa_;

public:

    ComponentInstallation_plug_impl(PortableServer::POA_ptr  poa)
        : poa_(PortableServer::POA::_duplicate(poa))
    {
    };

    ~ComponentInstallation_plug_impl(){};

    virtual PortableServer::POA_ptr _default_POA()
    {
        return PortableServer::POA::_duplicate(poa_.in());
    };

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/install:1.0
    //
    virtual void install(const char* implUUID,
                         const char* component_loc)
        throw(Components::Deployment::InvalidLocation,
              Components::Deployment::InstallationFailure,
              CORBA::SystemException)
    {
    };

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/replace:1.0
    //
    virtual void replace(const char* implUUID,
                         const char* component_loc)
        throw(Components::Deployment::InvalidLocation,
              Components::Deployment::InstallationFailure,
              CORBA::SystemException)
    {
    };

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/remove:1.0
    //
    virtual void remove(const char* implUUID)
        throw(Components::Deployment::UnknownImplId,
              Components::RemoveFailure,
              CORBA::SystemException)
    {
    };

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/get_implementation:1.0
    //
    virtual Components::Deployment::Location get_implementation(const char* implUUID)
        throw(Components::Deployment::UnknownImplId,
              Components::Deployment::InstallationFailure,
              CORBA::SystemException)
    {
        // CORBA::string_dup("/home/Cdmw/Users/Gomez/CDMW/runtime/ccmcif_in_progress/c++/test/test_lib_so/prod/orbacus_v4.10/sparc-sun-solaris2.7-gcc-2-95-2/libBasicDemo.so");
        Components::Deployment::Location_var location = CORBA::string_dup("libcdmwBasicDemo.so");
        return location._retn();
    };

    //
    // IDL:thalesgroup.com/CdmwDeployment/ComponentInstallation/get_valuetypefactory_location:1.0
    //
    virtual Components::Deployment::Location get_valuetypefactory_location(const char* implUUID,
                                                                           const char* repid)
        throw(Components::Deployment::UnknownImplId,
              CdmwDeployment::UnknownValuetype,
              Components::Deployment::InstallationFailure,
              CORBA::SystemException)
    {
        Components::Deployment::Location_var location = CORBA::string_dup("");
        return location._retn();
    };

};

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
const int SUCCESS = 0;
const int FAILURE = 1;  

const int POA_THREAD_POOL_SIZE = 5;   


};  // end namespace


int
run(CORBA::ORB_ptr orb, const char* containerIOR)
{
    Cdmw::CCM::CIF::ClientThread client(orb, containerIOR);

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
    //CdmwPlatformMngt::ProcessCallback_var callback;
    //OS::ProcessId idNaming, idRepository; 

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

    try
    {
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        std::string serverArgs(argv[0]);
        //serverArgs += " -ORBconfig obtest.conf";
        //int nbArgs = 3;
        int nbArgs = 1;
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(nbArgs, 
            String::to_char_array(String::to_strings(serverArgs)), orb_strategies);



        // Get the root POA 
        //
        CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
        //
        // Run implementation (Activate the POA manager)
        //
        PortableServer::POAManager_var poaManager = rootPOA -> the_POAManager();
        poaManager -> activate();
 

        // declare Value factory to the orb
    #if CDMW_ORB_VDR == tao && CDMW_ORB_VER < 14
        CORBA_ValueFactoryBase_var vf;
    #else
        CORBA::ValueFactoryBase_var vf;
    #endif
        vf = new Cdmw::CCM::CIF::FacetDescriptionFactory();
        orb->register_value_factory ("IDL:omg.org/Components/FacetDescription:1.0", vf.in());
       
        vf = new Cdmw::CCM::CIF::PortDescriptionFactory();
        orb->register_value_factory ("IDL:omg.org/Components/PortDescription:1.0", vf.in());
        
        vf = new Cdmw::CCM::Common::CookieFactory();
        orb->register_value_factory ("IDL:omg.org/Components/Cookie:1.0", vf.in());
        
        vf = new Cdmw::CCM::CIF::ConnectionDescriptionFactory();
        orb->register_value_factory ("IDL:omg.org/Components/ConnectionDescription:1.0", vf.in());
        
        vf = new Cdmw::CCM::CIF::ReceptacleDescriptionFactory();
        orb->register_value_factory ("IDL:omg.org/Components/ReceptacleDescription:1.0", vf.in());
        
        vf = new Cdmw::CCM::CIF::ConsumerDescriptionFactory();
        orb->register_value_factory ("IDL:omg.org/Components/ConsumerDescription:1.0", vf.in());
        
        vf = new Cdmw::CCM::CIF::EmitterDescriptionFactory();
        orb->register_value_factory ("IDL:omg.org/Components/EmitterDescription:1.0", vf.in());
        
        vf = new Cdmw::CCM::CIF::SubscriberDescriptionFactory();
        orb->register_value_factory ("IDL:omg.org/Components/SubscriberDescription:1.0", vf.in());
        
        vf = new Cdmw::CCM::CIF::PublisherDescriptionFactory();
        orb->register_value_factory ("IDL:omg.org/Components/PublisherDescription:1.0", vf.in());
        
        vf = new Cdmw::CCM::CIF::ComponentPortDescriptionFactory();
        orb->register_value_factory ("IDL:omg.org/Components/ComponentPortDescription:1.0",
            vf.in());

        vf = new Cdmw::CCM::Container::ConfigValueFactory();
        orb->register_value_factory ("IDL:omg.org/Components/ConfigValue:1.0",
            vf.in());

        // Register HomeAllocator
        typedef Cdmw::CCM::Container::HomeAllocator<Cdmw::CCM::CIF::CdmwBasicDemo::SessionServerHome_impl> SessionServerHome_Allocator;
        Cdmw::CCM::Container::HomeAllocatorRegistry::Register("Cdmw.CCM.CIF.CdmwBasicDemo.SessionServerHome_impl",
            &SessionServerHome_Allocator::TheAllocator);

        typedef Cdmw::CCM::Container::HomeAllocator<Cdmw::CCM::CIF::CdmwBasicDemo::SessionClientHome_impl> SessionClientHome_Allocator;
        Cdmw::CCM::Container::HomeAllocatorRegistry::Register("Cdmw.CCM.CIF.CdmwBasicDemo.SessionClientHome_impl",
            &SessionClientHome_Allocator::TheAllocator);

        /*CORBA::PolicyList pl;
        pl.length(1);
        pl[0] = rootPOA->create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);

        // Get parent POA's POA manager
        PortableServer::POAManager_var pmanager
            = rootPOA->the_POAManager();

        // Create new POA
        PortableServer::POA_var poa =
            rootPOA->create_POA("test_poa", pmanager, pl);

        // Clean up
        for (CORBA::ULong i = 0; i < pl.length(); ++i)
            pl[i]->destroy();
        */

        ComponentInstallation_plug_impl* comp_install 
            = new ComponentInstallation_plug_impl(rootPOA.in());
        //    = new ComponentInstallation_plug_impl(poa);
        CdmwDeployment::ComponentInstallation_var  comp_install_var
            = comp_install->_this();

        Components::ConfigValues config_values(2);
        config_values.length(1);
        CORBA::Any value;
        value <<= CdmwDeployment::SESSION;
        config_values[0]
            = new Cdmw::CCM::Container::ConfigValue_impl(CdmwDeployment::COMPONENT_KIND, value);

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
        
        // Create a container instance

        CdmwEvent::EventChannelFactory_var no_event_channel_factory
            = CdmwEvent::EventChannelFactory::_nil();
        Cdmw::CCM::Container::Container_impl* container 
            = new Cdmw::CCM::Container::Container_impl 
            (orb.in(), 
             container_poa.in(), 
             homes_serialize_poa.in(),
             homes_multithread_poa.in(),
             comp_install_var.in(), 
             Components::Deployment::ComponentServer::_nil(),
             Components::HomeRegistration::_nil(),
             no_event_channel_factory.in(),
             PROCESS_NAME,
             APPLICATION_NAME,
             config_values);
        
        PortableServer::ServantBase_var container_servant =  container;
        Components::Deployment::Container_var container_var = container->_this();
        CORBA::String_var containerIOR = orb->object_to_string(container_var.in());
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OS::sleep(timescale*1000);
        
        std::cout << "server initialised" << std::endl;
        
        status = run(orb.in(), containerIOR.in());
        OS::sleep (timescale*2000);
        
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
            //Cdmw::NamingAndRepository::RepositoryInterface::finish();
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
        //OS::kill_process(idNaming);
        //OS::kill_process(idRepository);
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

