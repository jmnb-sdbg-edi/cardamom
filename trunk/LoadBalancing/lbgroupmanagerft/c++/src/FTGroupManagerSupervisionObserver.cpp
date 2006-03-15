/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/**
 * @brief A FT Group Manager Supervision Observer process
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include "Foundation/common/Options.hpp"
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>

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

#include <LoadBalancing/lbgroupmanager/TypeRepository_impl.hpp>

#include "LoadBalancing/lbgroupmanagerft/FTGroupManagerProcessBehaviour.hpp"
#include "LoadBalancing/lbcommon/StateTransferConfig.hpp"
#include <LoadBalancing/lbinit/LBServiceInit.hpp>

using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;

static const int POA_THREAD_POOL_SIZE = 30;
std::string SYSTEM_CORBALOC_OPTION = "--system-corbaloc";

int main( int argc, char* argv[] )
{
    int ret_code = 0;
    std::string xml_conf;
    bool validate = false;


    // Check if primary option is specified
    std::string primary_option = Cdmw::OsSupport::OS::get_option_value(
        argc, argv, SET_PRIMARY_OPTION );
    
    bool set_as_primary=false;
    if (primary_option == "yes") {
        set_as_primary=true;
        std::cout << "  This FTGroupManager will be primary." << std::endl;
    }    
    
    CORBA::ORB_var orb;
    PortableServer::POA_var poa;
    try
    {
        
	// Check command line arguments

        if (argc < 2)
        {
            std::cerr << "Usage: " << "--CdmwLBXMLFile=<LBGroupManager Configuration File>" << std::endl;
            std::cerr << "       " << "--CdmwXMLFile=<Configuration File for CARDAMOM Services initialisation>" << std::endl;
            std::cerr << "       " << SYSTEM_CORBALOC_OPTION << "=system_corbaloc" << std::endl;
            std::cerr << std::endl;
            return -1;
        }


	// Initialise FT service (to be done before ORB_init)
        std::cout << "FTGroupManager: initialise FT service" << std::endl;
        Cdmw::FT::FTServiceInit::Init( argc, argv, false );

	        // checks the xml configuration file option
        std::string xml_conf_option =
            Cdmw::OsSupport::OS::get_option_value( argc, argv, Cdmw::Common::Options::LB_GROUP_MANAGER_CONF);

        if ( xml_conf_option != "no" )
        {
            if (xml_conf_option != "yes")
            {
                if (Cdmw::OsSupport::OS::file_exists(xml_conf_option))
                {
                    xml_conf = xml_conf_option;  		
		    
		    // checks the optional validate option
                    std::string validateXML_option =
		      Cdmw::OsSupport::OS::get_option_value( argc, argv, Cdmw::Common::Options::VALIDATE_XML_OPTION);

                    if (validateXML_option != "no")
                    {
                        validate = true;
                    }

                }
                else
                {
		    std::cerr << "ERROR : the xml configuration file doesn't exist" << std::endl;                    
                    return -1;
                }
                
            }
            else
            {
                std::cerr << "ERROR : the xml configuration file must be specified" << std::endl;                    
                return -1;
            }
        }
        else
        {
            std::cerr << "ERROR : the xml configuration file must be specified" << std::endl;                    
            return -1;
        }

        // The default configuration
        Cdmw::LB::ConfigurationMngr conf;

        // If an XML configuration file has been transmit, we use it
        if (!xml_conf.empty())
        {
            conf.parse(xml_conf, validate);
	}

	char** ORB_init_argv = 0;
        int ORB_init_argc = argc;
        std::string str_opt( "" );

        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";
        str_opt += std::string( "--LBInterceptorSide=LBGroupManager");
        ORB_init_argc += 1;
        ORB_init_argv = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings( str_opt ) );

        Cdmw::LB::LBServiceInit::Init(ORB_init_argc, ORB_init_argv);

        // Set ORB strategies
        std::cout << "FTGroupManager: initialise the orb" << std::endl;
        Cdmw::OrbSupport::StrategyList strategyList;
	strategyList.add_OrbThreaded();
	strategyList.add_PoaThreadPool( POA_THREAD_POOL_SIZE );
	strategyList.add_multicast();

        // Initialise the ORB
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init( argc, argv, strategyList );

	::Cdmw::OrbSupport::CodecBase::init(orb.in());

	// Create a supervision observer (eventualy register it to the system)
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

	std::string system_corbaloc = OsSupport::OS::get_option_value(
                                                  argc, argv, SYSTEM_CORBALOC_OPTION );

	std::string observer_name = conf.getObserverName();

	CORBA::Object_var system_obj
	    = orb->string_to_object( system_corbaloc.c_str() );
	
	if ( CORBA::is_nil( system_obj.in() ) )
	    CDMW_THROW2(
		Exception,
		Exception::FATAL,
		"Invalid corbaloc" );

        System_var system = System::_narrow( system_obj.in() );
        // creates the process behaviour
        std::auto_ptr<Cdmw::LB::FTGroupManagerProcessBehaviour> pProcessBehaviour(
            new Cdmw::LB::FTGroupManagerProcessBehaviour(orb.in(), poa.in(), set_as_primary, system.in(), conf ) );

        // init Cdmw
        std::string host_name = Cdmw::OsSupport::OS::get_hostname();
        std::cout << host_name << " Executor: init Cdmw" << std::endl;
	Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, pProcessBehaviour.get() );

        // Enter in the main loop
        orb->run();
	
	if (set_as_primary)
	// Unregister the status change event observer from the system
	system->unregister_observer( observer_name.c_str() );

    }

    catch(const CORBA::Exception &e)
    {
        std::cerr
            << "FAILURE : Unexpected exception" << std::endl
            << e._name() << std::endl;
        ret_code = EXIT_FAILURE;
    }

    
    if ( !CORBA::is_nil( poa.in() ) )
    {
        try
        {
            poa->destroy( 1, 1 );
        }
        catch ( const CORBA::Exception &e )
        {
            std::cerr << e << std::endl;
        }
    }

    if ( !CORBA::is_nil( orb.in() ) )
    {
        try
        {
	    Cdmw::CdmwInit::CDMW_cleanup(orb.in());  
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
        }
        catch ( const CORBA::Exception &e )
        {
            std::cerr << e << std::endl;
        }
    }

    return ret_code;
}


