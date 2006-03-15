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


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"
#include "simrepository/SimRepository_impl.hpp"
#include "simrepository/SimServiceBroker_impl.hpp"
#include "simrepository/SimProcessMessageBroker_impl.hpp"
#include "simrepository/SimProcessCallback_impl.hpp"
#include "simrepository/MessageLogger_impl.hpp"

#include <fstream>
#include <signal.h>

namespace
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5; 
    
    // Object IDs
    const char* REPOSITORY_USER_OBJECT_ID              = "repository.simulated";
    const char* MESSAGE_LOGGER_USER_OBJECT_ID          = "message-logger.simulated";
    const char* PROCESS_CALLBACK_USER_OBJECT_ID        = "process-callback.simulated";
    const char* SERVICE_BROKER_USER_OBJECT_ID          = "service-broker.simulated";
    const char* ENTITY_OBSERVER_USER_OBJECT_ID         = "entity-observer.simulated";
    const char* PROCESS_MESSAGE_BROKER_USER_OBJECT_ID  = "process-message-broker.simulated";

    // Binding names
    const char* REPOSITORY_BINDING_NAME        = "CDMW.I/repository.simulated";
    const char* PROCESS_CALLBACK_BINDING_NAME  = "CDMW.I/process-callback.simulated";


    // IOR filenames
    const char* REPOSITORY_IOR_FILE_NAME        = "SimRepository.IOR";
    const char* PROCESS_CALLBACK_IOR_FILE_NAME  = "SimProcessCallback.IOR";

    // Log filenames
    const char* MESSAGE_LOGGER_LOG_FILE_NAME    = "SimPlatformManagement.log";

    // CORBALOC IDs
    const char* REPOSITORY_CORBALOC_ID         = "SimRepository";
    const char* PROCESS_CALLBACK_CORBALOC_ID   = "SimProcessCallback";
    const char* NAME_SERVICE_ID                = "NameService";
 
    
    // NamingContext prefix
    const char* REPOSITORY_NC_PREFIX           = "CDMW.I/NamingAndRepository.simulated";
    const char* SERVICE_BROKER_NC_PREFIX       = "CDMW.I/ServiceBroker.simulated";
    const char* PROCESS_CALLBACK_NC_PREFIX     = "CDMW.I/ProcessCallback.simulated";


    // Default application name
    const char* DEFAULT_APPLICATION_NAME       = "CDMW Test Application";
}


int
run(CORBA::ORB_ptr orb, int& argc, char** argv)
{   
    //
    // Get the root POA
    //
    CORBA::Object_var rootObj =
        orb -> resolve_initial_references("RootPOA");
    PortableServer::POA_var root = PortableServer::POA::_narrow(rootObj.in());
    
    //
    // Create persistent POA
    //
    PortableServer::POAManager_var poaManager = root -> the_POAManager();
    CORBA::PolicyList policies;
    policies.length(2);
    policies[0] =
        root -> create_id_assignment_policy(PortableServer::USER_ID);
    policies[1] =
        root -> create_lifespan_policy(PortableServer::PERSISTENT);
    PortableServer::POA_var multithread_poa =
        root -> create_POA("SimRepository", poaManager.in(), policies);
    
    
    try {
        //
        // Register Repository within the Naming Service
        //
        CORBA::Object_var obj =
            orb -> resolve_initial_references("NameService");
        if (CORBA::is_nil(obj.in())) {
            std::cerr << "Could not resolve initial reference \"NameService\""
                      << std::endl;
            throw 1;
        }

        CosNaming::NamingContext_var nc = CosNaming::NamingContext::_nil();
        try {
            nc = CosNaming::NamingContext::_narrow(obj.in());
        } catch (const CORBA::Exception & ex) {
            std::cerr << "Cannot reach the naming service \n"
                      << "Reason: " << ex << std::endl;
            throw 1;
        }
        if (CORBA::is_nil(nc.in())) {
            std::cerr << "Invalid reference to a Naming Service"
                      << std::endl;
            throw 1;
        }
        Cdmw::CommonSvcs::Naming::NamingInterface ni(nc.in());
        //
        // Create Simulated Repository object
        //
        CdmwNamingAndRepository::Repository_var rep_reference
            = CdmwNamingAndRepository::Repository::_nil();
        {
            Cdmw::CommonSvcs::Naming::NamingInterface repository_ni_base
                = ni.bind_new_context(REPOSITORY_NC_PREFIX, true);
            CosNaming::NamingContext_var rep_nc = repository_ni_base.name_context();
            
            Cdmw::Tools::SimRepository_impl * rep
                = new Cdmw::Tools::SimRepository_impl(orb, multithread_poa.in(), 
                                                       rep_nc.in(),
                                                       argc, argv);
        
            PortableServer::ServantBase_var repServant = rep;
            PortableServer::ObjectId_var oid 
                = PortableServer::string_to_ObjectId(REPOSITORY_BINDING_NAME);
            multithread_poa -> activate_object_with_id(oid.in(), repServant.in());
            rep_reference = rep -> _this();

            ni.rebind(REPOSITORY_BINDING_NAME,rep_reference.in(),false);

            // Bind the Repository to a Corbaloc name
            Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc( orb,
                                                                    REPOSITORY_CORBALOC_ID,
                                                                    rep_reference.in() );
	    // Write IOR to file SimRepository.IOR
	    std::ofstream ior_file(REPOSITORY_IOR_FILE_NAME);

	    CORBA::String_var rep_ior = orb->object_to_string(rep_reference.in());

	    ior_file << rep_ior.in();
	    ior_file.close();
            
            // Bind the Default RootContext to corbaloc Id NameService
            // to be accessible by corbaname
            CosNaming::NamingContext_var default_root_ctxt = 
                rep -> resolve_root_context (CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
            Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc( orb,
                                                                    NAME_SERVICE_ID,
                                                                    default_root_ctxt.in() );
        }
        
        // Create Simulated Platform Management Service objects
        {
            //
            // Create a single thread POA for the message logger and the
            // ProcessCallback objects
            //
            CORBA::PolicyList policies;
            policies.length(3);
            policies[0] =
                root -> create_id_assignment_policy(PortableServer::USER_ID);
            policies[1] =
                root -> create_lifespan_policy(PortableServer::PERSISTENT);
            policies[2] =
                root -> create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);
            PortableServer::POA_var singlethread_poa =
                root -> create_POA("SimPlatformManagement", poaManager.in(), policies);

            // Create a MessageLogger object
            Cdmw::Tools::MessageLogger_impl * logger
                = new Cdmw::Tools::MessageLogger_impl(singlethread_poa.in(),
						       MESSAGE_LOGGER_LOG_FILE_NAME);            
        
            PortableServer::ServantBase_var loggerServant = logger;
            CdmwSimulatedRepository::MessageLogger_var logger_ref
                = CdmwSimulatedRepository::MessageLogger::_nil();
            {
                PortableServer::ObjectId_var oid 
                    = PortableServer::string_to_ObjectId(MESSAGE_LOGGER_USER_OBJECT_ID);
                singlethread_poa -> activate_object_with_id(oid.in(), loggerServant.in());
                logger_ref = logger -> _this();
            }
            
            // Create a ServiceBroker object
            Cdmw::CommonSvcs::Naming::NamingInterface service_broker_ni_base
                = ni.bind_new_context(SERVICE_BROKER_NC_PREFIX, true);
            CosNaming::NamingContext_var service_broker_nc
                = service_broker_ni_base.name_context();

            Cdmw::Tools::SimServiceBroker_impl * service_broker
                = new Cdmw::Tools::SimServiceBroker_impl(multithread_poa.in(),
                                                          service_broker_nc.in(),
                                                          logger_ref.in());
                                                          
            PortableServer::ServantBase_var service_brokerServant = service_broker;
            CdmwPlatformMngtService::ServiceBroker_var  service_broker_ref
                =  CdmwPlatformMngtService::ServiceBroker::_nil();
            {
                PortableServer::ObjectId_var oid 
                    = PortableServer::string_to_ObjectId(SERVICE_BROKER_USER_OBJECT_ID);
                multithread_poa -> activate_object_with_id(oid.in(), service_brokerServant.in());
                service_broker_ref = service_broker -> _this();
            }
            
            //
            // Register the Repository as a service within the ServiceBroker
            //
            const char* sn=Cdmw::PlatformMngt::ServiceNames::NAMING_AND_REPOSITORY_SERVICE;
            service_broker->set_service(sn,rep_reference.in());

            // Create a ProcessMessageBroker object
            Cdmw::Tools::SimProcessMessageBroker_impl * message_broker
                = new Cdmw::Tools::SimProcessMessageBroker_impl(multithread_poa.in(),
                                          service_broker_ref.in(),logger_ref.in());                   
            PortableServer::ServantBase_var message_brokerServant = message_broker;
            CdmwPlatformMngt::ProcessMessageBroker_var  message_broker_ref
                = CdmwPlatformMngt::ProcessMessageBroker::_nil();
            {
                PortableServer::ObjectId_var oid 
                    = PortableServer::string_to_ObjectId(PROCESS_MESSAGE_BROKER_USER_OBJECT_ID);
                multithread_poa -> activate_object_with_id(oid.in(), message_brokerServant.in());
                message_broker_ref = message_broker -> _this();
            }
            
                      
			//
            // Create a single thread POA for the message logger and the
            // ProcessCallback objects
            //
            PortableServer::POA_var pcb_singlethread_poa =
                root -> create_POA("SimProcessCallback", poaManager.in(), policies);

            // Create a ProcessCallback object
            Cdmw::CommonSvcs::Naming::NamingInterface process_callback_ni_base
                = ni.bind_new_context(PROCESS_CALLBACK_NC_PREFIX, true);
            CosNaming::NamingContext_var process_callback_nc
                = process_callback_ni_base.name_context();
            Cdmw::Tools::SimProcessCallback_impl * process_callback
                = new Cdmw::Tools::SimProcessCallback_impl(pcb_singlethread_poa.in(),
                                                            process_callback_nc.in(),
                                                            message_broker_ref.in(),
                                                            service_broker_ref.in(),
                                                            logger_ref.in(),
                                                            DEFAULT_APPLICATION_NAME);
        
            PortableServer::ServantBase_var process_callbackServant = process_callback;
            CdmwPlatformMngt::ProcessCallback_var process_callback_ref
                = CdmwPlatformMngt::ProcessCallback::_nil();
            {
                PortableServer::ObjectId_var oid 
                    = PortableServer::string_to_ObjectId(PROCESS_CALLBACK_USER_OBJECT_ID);
                pcb_singlethread_poa -> activate_object_with_id(oid.in(), process_callbackServant.in());
                process_callback_ref = process_callback -> _this();
            }

            ni.rebind(PROCESS_CALLBACK_BINDING_NAME,process_callback_ref.in(),false);

            // Bind the Repository to a Corbaloc name
            Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc
                ( orb,
                  PROCESS_CALLBACK_CORBALOC_ID,
                  process_callback_ref.in() );

	    // Write IOR to file SimProcessCallback.IOR
	    std::ofstream ior_file(PROCESS_CALLBACK_IOR_FILE_NAME);

	    CORBA::String_var process_callback_ior
                = orb->object_to_string(process_callback_ref.in());

	    ior_file << process_callback_ior.in();
	    ior_file.close();
        }
    } catch (const CORBA::Exception & ex) {
        std::cerr << ex << std::endl;
        return FAILURE;
    }catch (...) {
        return FAILURE;
    }

    //
    // Run implementation
    //
    poaManager -> activate();
    orb -> run();

    return SUCCESS;
}


CORBA::ORB_var orb = CORBA::ORB::_nil();

void exit_handler(int sig) {
    if (!CORBA::is_nil(orb.in())) {
        try {
            orb->shutdown(false);
        } catch (const CORBA::SystemException& e) {
            std::cerr << "Error while shuting ORB down in exit_handler:\n"
                      << e << " - minor code: " << e.minor() << std::endl;
        }
    }
}



int main(int argc, char* argv[])
{
    int status = SUCCESS;

    try	{
        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPerConnection();

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, 
                                                      orb_strategies);

        struct sigaction action;
        action.sa_handler=exit_handler;
        sigemptyset(&action.sa_mask);
        sigaction(SIGTERM, &action, NULL);
        sigaction(SIGINT, &action, NULL);
        sigaction(SIGQUIT, &action, NULL);
        sigaction(SIGABRT, &action, NULL);
                                                      
        //orb = CORBA::ORB_init(argc, argv);
        status = run(orb.in(), argc, argv);
    } catch(const CORBA::Exception& ex) {
        std::cerr << ex << std::endl;
	status = FAILURE;
    }

    if(!CORBA::is_nil(orb.in())) {
	try {
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    orb->destroy();
	} catch(const CORBA::Exception& ex) {
            std::cerr << ex << std::endl;
	    status = FAILURE;
	}
    }
    
    return status;
}


