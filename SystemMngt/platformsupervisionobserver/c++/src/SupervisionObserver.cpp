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


#include <iostream>
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include "platformsupervisionobserver/SupervisionObserver_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationAgentDeath_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/UserEvent_impl.hpp"
#include "idllib/CdmwPlatformMngtSystem.stub.hpp"

using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;

std::string REGISTER_OPTION = "--register";
std::string UNREGISTER_OPTION = "--unregister";
std::string SYSTEM_CORBALOC_OPTION = "--system-corbaloc";
std::string OBSERVER_PORT_OPTION = Options::LOCALISATION_SERVICE_OPTION;
std::string OBSERVER_NAME_OPTION = "--observer-name";


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


int main( int argc, char* argv[] )
{
    int res = 0;
    PortableServer::POA_var poa;

    try
    {
        // Check command line arguments

        if ( (argc != 3) && (argc != 4) )
        {
            std::cerr << "Usage: " << REGISTER_OPTION << " or " << UNREGISTER_OPTION << std::endl;
            std::cerr << "       " << SYSTEM_CORBALOC_OPTION << "=system_corbaloc" << std::endl;
            std::cerr << "       " << OBSERVER_NAME_OPTION << "=observer_name" << std::endl;
            std::cerr << std::endl;
            std::cerr << "   Or: " << OBSERVER_PORT_OPTION << "=observer_port" << std::endl;
            std::cerr << "       " << OBSERVER_NAME_OPTION << "=observer_name" << std::endl;
            std::cerr << std::endl;
            throw 0;
        }

        // Initialise ORB
        OrbSupport::StrategyList orbStrategies;

        orbStrategies.add_OrbThreaded();

        orbStrategies.add_PoaThreadPool( 10 );

        orb = OrbSupport::OrbSupport::ORB_init( argc, argv, orbStrategies );

        // Create a supervision observer (eventualy register it to the system)
        if ( OsSupport::OS::get_option_value( argc, argv, UNREGISTER_OPTION ) == "no" )
        {
            // Get a reference to the root POA
            CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
            poa = PortableServer::POA::_narrow( obj.in() );

            // Activate the POA manager
            PortableServer::POAManager_var mgr = poa->the_POAManager();
            mgr->activate();

            // Register event factories
            CORBA::ValueFactoryBase_var factory, old_factory;

            factory = new SystemModeChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/SystemModeChange:1.0",
                              factory.in() );

            factory = new SystemStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/SystemStatusChange:1.0",
                              factory.in() );

            factory = new HostStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/HostStatusChange:1.0",
                              factory.in() );

            factory = new ApplicationAgentDeathFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentDeath:1.0",
                              factory.in() );

            factory = new ApplicationModeChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationModeChange:1.0",
                              factory.in() );

            factory = new ApplicationStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationStatusChange:1.0",
                              factory.in() );

            factory = new ProcessMessageFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessage:1.0",
                              factory.in() );

            factory = new ProcessStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessStatusChange:1.0",
                              factory.in() );

            factory = new SystemEntityStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/SystemEntityStatusChange:1.0",
                              factory.in() );
                              
            factory = new HostEntityStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/HostEntityStatusChange:1.0",
                              factory.in() );

            factory = new ApplicationEntityStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationEntityStatusChange:1.0",
                              factory.in() );

            factory = new ProcessEntityStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessEntityStatusChange:1.0",
                              factory.in() );

            factory = new UserEventFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/UserEvent:1.0",
                              factory.in() );

            // Create an observer
            SupervisionObserver_impl observer_i;
            CdmwPlatformMngt::SupervisionObserver_var observer = observer_i._this();

            if ( OsSupport::OS::get_option_value( argc, argv, REGISTER_OPTION ) == "no" )
            {
                std::string observer_name = OsSupport::OS::get_option_value(
                                                argc, argv, OBSERVER_NAME_OPTION );
                std::string observer_port = OsSupport::OS::get_option_value(
                                                argc, argv, OBSERVER_PORT_OPTION );
                OrbSupport::OrbSupport::bind_object_to_corbaloc(
                    orb.in(), observer_name, observer.in() );
                std::cout << "corbaloc::" << OsSupport::OS::get_hostname()
                << ":" << observer_port
                << "/" << observer_name
                << std::endl;
            }

            // Register the observer to the designated (with corbaloc) system
            if ( OsSupport::OS::get_option_value( argc, argv, REGISTER_OPTION ) == "yes" )
            {
                // Connect to the designated system
                std::string system_corbaloc = OsSupport::OS::get_option_value(
                                                  argc, argv, SYSTEM_CORBALOC_OPTION );
                std::string observer_name = OsSupport::OS::get_option_value(
                                                argc, argv, OBSERVER_NAME_OPTION );

                CORBA::Object_var system_obj
                = orb->string_to_object( system_corbaloc.c_str() );

                if ( CORBA::is_nil( system_obj.in() ) )
                    CDMW_THROW2(
                        Exception,
                        Exception::FATAL,
                        "Invalid corbaloc" );

                System_var system = System::_narrow( system_obj.in() );

                if ( CORBA::is_nil( system.in() ) )
                    CDMW_THROW2(
                        Exception,
                        Exception::FATAL,
                        "Invalid system reference" );

                // Register the observer to the system
                system->register_observer( observer_name.c_str(), observer.in() );
            }

            struct sigaction action;
            action.sa_handler=exit_handler;
            sigemptyset(&action.sa_mask);
            sigaction(SIGTERM, &action, NULL);
            sigaction(SIGINT, &action, NULL);
            sigaction(SIGQUIT, &action, NULL);
            sigaction(SIGABRT, &action, NULL);

            // Accept requests
            orb->run();
        }

        // Unregister the supervision observer from the system
        if ( OsSupport::OS::get_option_value( argc, argv, UNREGISTER_OPTION ) == "yes" )
        {
            // Connect to the designated system
            std::string system_corbaloc = OsSupport::OS::get_option_value(
                                              argc,
                                              argv,
                                              SYSTEM_CORBALOC_OPTION );
            std::string observer_name = OsSupport::OS::get_option_value(
                                            argc,
                                            argv,
                                            OBSERVER_NAME_OPTION );

            CORBA::Object_var system_obj
            = orb->string_to_object( system_corbaloc.c_str() );

            if ( CORBA::is_nil( system_obj.in() ) )
                CDMW_THROW2(
                    Exception,
                    Exception::FATAL,
                    "Invalid corbaloc" );

            System_var system = System::_narrow( system_obj.in() );

            if ( CORBA::is_nil( system.in() ) )
                CDMW_THROW2(
                    Exception,
                    Exception::FATAL,
                    "Invalid system reference" );

            // Unregister the status change event observer from the system
            system->unregister_observer( observer_name.c_str() );
        }


    }
    catch ( const CORBA::Exception &e )
    {
        std::cerr << e._name() << std::endl;
        res = 1;
    }
    catch ( ... )
    {
        res = 1;
    }


    if ( !CORBA::is_nil( orb.in() ) )
    {
        try
        {
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
        }
        catch ( const CORBA::Exception &e )
        {
            std::cerr << e << std::endl;
        }
    }

    return res;
}

