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
 * @brief A Supervision Observer for the LBGroupManager
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it> 
 */
#include <Foundation/common/String.hpp>
#include <iostream>
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include <LoadBalancing/lbgroupmanager/TypeRepository_impl.hpp>
#include <LoadBalancing/lbgroupmanager/ConfigurationMngr.hpp>

#include "lbgroupmanager/LBGroupManager_impl.hpp"

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
#include <Foundation/commonsvcs/datastore/StdMapDataStoreFactory.hpp>
#include <LoadBalancing/lbcommon/StateTransferConfig.hpp>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;

std::string SYSTEM_CORBALOC_OPTION = "--system-corbaloc";

typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory
< PortableGroup::ObjectGroupId,
  CdmwLB::ObjectGroupData> 
DefaultDataStoreFactory;


int main( int argc, char* argv[] )
{
    int res = 0;
    std::string system_corbaloc;
    std::string observer_name;
    
    
    std::string xml_conf;
    bool validate = false;
    
    

        // Check command line arguments

        if (argc < 2)
        {
            std::cerr << "Usage: " << "--CdmwLBXMLFile=<LBGroupManager Configuration File>" << std::endl;
	    std::cerr << "       " << SYSTEM_CORBALOC_OPTION << "=system_corbaloc" << std::endl;
            std::cerr << std::endl;
            std::cerr << std::endl;
	    return -1;
        }
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
                    return -1;                }
                
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
	System_var system;
	PortableServer::POA_var poa;
	CORBA::ORB_var orb = CORBA::ORB::_nil();
        // If an XML configuration file has been transmit, we use it
        if (!xml_conf.empty())
        {
            conf.parse(xml_conf, validate);
	}
	
	try{
	char** ORB_init_argv = 0;
        int ORB_init_argc = argc;
        std::string str_opt( "" );

        for (int i=0; i<argc; i++)
	    str_opt += std::string( argv[i] ) + " ";
	str_opt += std::string( "--LBInterceptorSide=LBGroupManager") + " ";
	str_opt += std::string(Common::Options::LOCALISATION_SERVICE_OPTION );
        str_opt += "="; 
        str_opt += conf.getSystemPortString();
	ORB_init_argc += 2;
        ORB_init_argv = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings( str_opt ) );

	
        // Initialise ORB
        OrbSupport::StrategyList orbStrategies;
        orbStrategies.add_OrbThreaded();
        orbStrategies.add_PoaThreadPool( 30 );
	orbStrategies.add_multicast();

	Cdmw::LB::LBServiceInit::Init(ORB_init_argc, ORB_init_argv);

	orb = OrbSupport::OrbSupport::ORB_init( ORB_init_argc, ORB_init_argv, orbStrategies );
	::Cdmw::OrbSupport::CodecBase::init(orb.in()); 

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
	
	// Resolve the LBGroupManager from corbaloc (ok at compile time)
	
	DefaultDataStoreFactory::Create_data_store(OBJECT_GROUP_DATA_IDENTIFIER);
	
	// Create the TypeRepository object and register it
        CdmwLB::TypeRepository_var typeRepository =
            new Cdmw::LB::TypeRepository_impl(conf.getSupportedInterfaceMap());
        orb->register_initial_reference("LBTypeRepository", typeRepository.in());
        
       	
	Cdmw::LB::LBGroupManager_impl* groupManager = new Cdmw::LB::LBGroupManager_impl(orb.in(), poa.in(), conf.getDomainID());
	CdmwLB::LBGroupManager_var observer  = groupManager->_this();
	orb->register_initial_reference("LBGroupManager", observer.in());
	OrbSupport::OrbSupport::bind_object_to_corbaloc(
							orb.in(), "group_mgr", observer.in() );

	std::string register_option =  conf.getRegisterOption();
	std::string unregister_option =  conf.getUnregisterOption();

	// Register the observer to the designated (with corbaloc) system
	if ( register_option.compare("yes") == 0 )
	{
	    // Connect to the designated system
	    system_corbaloc = OsSupport::OS::get_option_value( argc, argv, SYSTEM_CORBALOC_OPTION );
	    observer_name = conf.getObserverName();
	    
	    
	    CORBA::Object_var system_obj
	      = orb->string_to_object( system_corbaloc.c_str() );
	    
	    if ( CORBA::is_nil( system_obj.in() ) )
	      CDMW_THROW2(
			  Exception,
			  Exception::FATAL,
			  "Invalid corbaloc" );
	    
	    system = System::_narrow( system_obj.in() );
	    
	    if ( CORBA::is_nil( system.in() ) )
	      CDMW_THROW2(
			  Exception,
			  Exception::FATAL,
			  "Invalid system reference" );
	    // Register the observer to the system
	    system->register_observer( observer_name.c_str(),observer.in() );
	  }
	
	// Accept requests
	orb->run();
	if (unregister_option.compare("yes") == 0 )

	{
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

