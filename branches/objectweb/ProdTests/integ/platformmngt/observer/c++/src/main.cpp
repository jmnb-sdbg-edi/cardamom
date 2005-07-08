/* =========================================================================== *
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
 * =========================================================================== */

#include <iostream>
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationAgentDeath_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/UserEvent_impl.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp"
#include "observer/MyProcessBehaviour.hpp"
#include "eventdata/DataA_impl.hpp"
#include "eventdata/DataB_impl.hpp"

using namespace Cdmw;
using namespace Common;
using namespace PlatformMngt;
using namespace CdmwPlatformMngt;

std::string HELP_OPTION = "--help";
std::string MANAGED_OPTION = "--managed";
std::string SYSTEM_CORBALOC_OPTION = "--system";
std::string OBSERVER_NAME_OPTION = "--name";

static void usage( std::ostream & os, const std::string &program_name )
{
    os << "Usage:"<< std::endl
       << program_name << " options" << std::endl
       << std::endl
       << "Options:" << std::endl
       << HELP_OPTION << " Display this information." << std::endl
       << MANAGED_OPTION << " The observer is a managed process" << std::endl
       << SYSTEM_CORBALOC_OPTION << "=system_corbaloc" << std::endl
       << OBSERVER_NAME_OPTION << "=observer_name" << std::endl
       << std:: endl;
}
    
void register_factories( CORBA::ORB_ptr orb )
throw( CORBA::SystemException )
{
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

    factory = new DataAFactory;
    old_factory = orb->register_value_factory(
        "IDL:DataA:1.0", factory.in() );

    factory = new DataBFactory;
    old_factory = orb->register_value_factory(
        "IDL:DataB:1.0", factory.in() );
}


int main( int argc, char* argv[] )
{
    int ret_code = 0;
    CORBA::ORB_var orb;
    PortableServer::POA_var poa;

    try
    {
        // Check command line arguments
        std::string help_option = OsSupport::OS::get_option_value(
            argc, argv, HELP_OPTION );
        std::string managed_option = OsSupport::OS::get_option_value(
            argc, argv, MANAGED_OPTION );
        std::string system_corbaloc = OsSupport::OS::get_option_value(
            argc, argv, SYSTEM_CORBALOC_OPTION );
        std::string observer_name = OsSupport::OS::get_option_value(
            argc, argv, OBSERVER_NAME_OPTION );

        if( help_option == "yes" )
	    {
            usage( std::cout, argv[0] );
            return ret_code;
        }
        if( system_corbaloc == "yes" )
        {
            usage( std::cout, argv[0] );
            return ( ret_code = 1 );
        }

        if( system_corbaloc == "no" )
            system_corbaloc
                = "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";
        if( observer_name == "no" )
            observer_name = "user_observer";

        // Initialise ORB
        OrbSupport::StrategyList orbStrategies;
            orbStrategies.add_OrbThreaded();
            orbStrategies.add_PoaThreadPool( 10 );
        orb = OrbSupport::OrbSupport::ORB_init( argc, argv, orbStrategies );

        // Get a reference to the root POA
        CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
        poa = PortableServer::POA::_narrow( obj.in() );

        // Activate the POA manager
        PortableServer::POAManager_var mgr = poa->the_POAManager();
        mgr->activate();

        // Initialise the platform interface
        if( managed_option == "yes" )
            PlatformInterface::setup( orb.in(), argc, argv );

        // Register event factories
        register_factories( orb.in() );

        // Connect to the designated system
        CORBA::Object_var system_obj
            = orb->string_to_object( system_corbaloc.c_str() );
        if( CORBA::is_nil( system_obj.in() ) )
            CDMW_THROW2(
                Exception,
                Exception::FATAL,
                "Invalid corbaloc" );

        System_var system = System::_narrow( system_obj.in() );
        if( CORBA::is_nil( system.in() ) )
            CDMW_THROW2(
                Exception,
                Exception::FATAL,
                "Invalid system reference" );

        // Create an observer object
        std::auto_ptr< MyProcessBehaviour > pMyProcess( new MyProcessBehaviour(
            orb.in(), system.in(), observer_name.c_str() ) );

        // Acknowledge the creation of the process
        if( managed_option == "yes" )
        {
            PlatformInterface::acknowledgeCreation( pMyProcess.get() );
            pMyProcess.release();
        }

        // Accept requests
        orb->run();
    }
    catch ( const CORBA::Exception &e )
    {
        std::cerr << e._name() << std::endl;
        ret_code = 1;
    }
    catch( ... )
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl;
        ret_code = 1;
    }

    // Stop the POA
    if( !CORBA::is_nil( poa.in() ) )
        poa->destroy( 1, 1 );

    // Cleanup the ORB to exit completely
    if( !CORBA::is_nil( orb.in() ) )
    {
        orb->shutdown( false );
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup( orb.in() );
        orb->destroy();
    }

    return ret_code;
}

