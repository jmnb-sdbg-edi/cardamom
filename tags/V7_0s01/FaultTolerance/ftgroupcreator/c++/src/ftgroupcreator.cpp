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


// Standard Files 
#include <iostream>
#include <string>

// Cdmw Files
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>

#include <Foundation/orbsupport/StrategyList.hpp>
#include "FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp"
#include "ftgroupcreator/ConfigurationMngr.hpp"
#include "ftgroupcreator/GroupConfigurationMngr.hpp"
#include <FaultTolerance/ftcommon/FTConfiguration.hpp>

#include <sax/SAXParseException.hpp>
#include <util/PlatformUtils.hpp>



namespace
{
    std::string CONFIG_OPTION = "--conf";
};


using namespace Cdmw;

int main( int argc, char* argv[] )
{
    std::string xml_conf;
    try
    {
        XMLPlatformUtils::Initialize();

        std::string xml_init;
        bool validate = false;

        // checks the xml configuration file option
        std::string xml_init_option =
        Cdmw::OsSupport::OS::get_option_value( argc, argv, Cdmw::Common::Options::PROCESS_XML_FILE_OPTION);

        if ( xml_init_option != "no" )
        {
            if (xml_init_option != "yes")
            {
                if (Cdmw::OsSupport::OS::file_exists(xml_init_option))
                {
                    xml_init = xml_init_option;  	  
                }
                else
                {
                   // LogMngr::logMessage(FTL, MSG_ID_DAEMON_CONF_FILE_NOT_FOUND, xml_conf_option);
                    std::cerr<<"No configuration file present"<<std::endl;
                    ::exit(EXIT_FAILURE);
                }
                
            }
            else
            {
                //LogMngr::logMessage(FTL, MSG_ID_DAEMON_CONF_FILE_NOT_SPECIFIED);
                std::cerr<<"No configuration file present"<<std::endl;
                ::exit(EXIT_FAILURE);
            }
        }

        // checks the xml configuration file option
        std::string xml_conf_option =
        Cdmw::OsSupport::OS::get_option_value( argc, argv, CONFIG_OPTION);

        if ( xml_conf_option != "no" )
        {
            if (xml_conf_option != "yes")
            {
                if (Cdmw::OsSupport::OS::file_exists(xml_conf_option))
                {
                    xml_conf = xml_conf_option;  		

                    // checks the optional validate option
                    std::string validateXML_option =
                    Cdmw::OsSupport::OS::get_option_value( argc, argv, CONFIG_OPTION);

                    if (validateXML_option != "no")
                    {
                        validate = true;
                    }

                }
                else
                {
                   // LogMngr::logMessage(FTL, MSG_ID_DAEMON_CONF_FILE_NOT_FOUND, xml_conf_option);
                    std::cerr<<"No configuration file present"<<std::endl;
                    ::exit(EXIT_FAILURE);
                }
                
            }
            else
            {
                //LogMngr::logMessage(FTL, MSG_ID_DAEMON_CONF_FILE_NOT_SPECIFIED);
                std::cerr<<"No configuration file present"<<std::endl;
                ::exit(EXIT_FAILURE);
            }
        }

        // Initialise FT service
        Cdmw::FT::FTConfiguration::Activate_FT_service(argc, argv);
    
        // Initialises the ORB
        OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPerConnection();

        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(
            argc, argv, strategyList );

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
        PortableServer::POA_var rootPOA
            = PortableServer::POA::_narrow( obj.in() );

        Cdmw::FT::ConfigurationMngr conf("Init");

        if (!xml_init.empty())
        {
            conf.parse(xml_init, validate);
        }

        TimeBase::TimeT duration_time = 0;

        obj = orb->resolve_initial_references("ORBPolicyManager");

        CORBA::PolicyManager_var policy_manager 
            = CORBA::PolicyManager::_narrow(obj.in());

        // Create and apply an ORB-wide Routed policy        
        duration_time = conf.getRequestDurationTime();
        if (duration_time == 0)
        {
            CDMW_THROW2(Cdmw::BadParameterException, " Request duration time ", " not defined ");
        }     

        CORBA::Any any;
        any <<= duration_time;
        CORBA::PolicyList policies(1);
        policies.length(1);
        policies[0] =
            orb->create_policy(::FT::REQUEST_DURATION_POLICY, any);
        
        policy_manager->set_policy_overrides(policies, CORBA::SET_OVERRIDE);



        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        std::string corbamane_rm("corbaname::localhost:"); 
        corbamane_rm += conf.getSystemDaemonPort();
        corbamane_rm += "#ReplicationManager" ;

        std::cout<< corbamane_rm <<std::endl;

        obj = orb->string_to_object(corbamane_rm.c_str());

        CdmwFT::ReplicationManager_var replication_mgr = CdmwFT::ReplicationManager::_narrow(obj.in());
        if (CORBA::is_nil(replication_mgr.in()))
        {
            std::cout<<"The Replication Manager reference can't be resolved"<<std::endl;
            throw CORBA::OBJECT_NOT_EXIST();
        }
        
        // The default Group configuration
        Cdmw::FT::GroupConfigurationMngr group_conf(replication_mgr.in());

        // If an XML Group configuration file has been transmit, we use it to create 
        // default group needed by the system
        if (!xml_conf.empty())
        {
            group_conf.parse(xml_conf, validate);
        }

        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
        
        XMLPlatformUtils::Terminate();
    }
    catch ( const Cdmw::FT::ConfigurationException &ex )
    {
        if (ex.getLineNumber() != 0)
        {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<<"At Line :" << ex.getLineNumber()<<" of "<< xml_conf 
                      << "\n" << ex.getMessage()<< std::endl;
        } else {
            std::cerr <<__FILE__<<" "<<__LINE__<<" "<<ex.getMessage() << std::endl;
        }
        return EXIT_FAILURE;
    }
    catch( const CORBA::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << std::endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch( ... )
    {
        std::cerr << "Unexpected exception" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

