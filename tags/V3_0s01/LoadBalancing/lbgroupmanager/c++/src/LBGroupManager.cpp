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
 * @file
 * @brief Start an LBGroupManager
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */
#include "Foundation/orbsupport/CORBA.hpp"
#include <Foundation/common/String.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/Codec.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/commonsvcs/datastore/StdMapDataStoreFactory.hpp>
#include <LoadBalancing/lbcommon/StateTransferConfig.hpp>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
#include <LoadBalancing/lbgroupmanager/TypeRepository_impl.hpp>
#include <LoadBalancing/lbgroupmanager/ConfigurationMngr.hpp>
#include <iostream>
#include <string>

#include "orbsvcs/CosNamingC.h"
#define SUCCESS 0;


#include "LoadBalancing/lbgroupmanager/LBGroupManager_impl.hpp"

#include <iostream>
using namespace Cdmw;
typedef Cdmw::CommonSvcs::DataStore::StdMapDataStoreFactory
    < PortableGroup::ObjectGroupId,
      CdmwLB::ObjectGroupData> 
DefaultDataStoreFactory;

//using namespace CdmwPlatformMngt;
using namespace Cdmw;
using namespace Common;
using namespace Cdmw::LB;



int main(int argc, char* argv[])
{
    int status = SUCCESS;
    std::string xml_conf;
    bool validate = false;

    
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
                }
                else
                {
		  std::cerr << " ERROR: the xml configuration file doesn't exist" << std::endl ;                    
                    return -1;
                }
                
            }
            else
            {
		std::cerr << " ERROR: the xml configuration file must be specified" << std::endl;                    
                return -1;
            }
        }
        else
        {
            std::cerr <<  " ERROR: the xml configuration file must be specified" << std::endl;                    
            return -1;
        }
	  CORBA::ORB_var orb = CORBA::ORB::_nil();
        // The default configuration
        Cdmw::LB::ConfigurationMngr conf;

        // If an XML configuration file has been transmit, we use it
        if (!xml_conf.empty())
        {
            conf.parse(xml_conf, validate);
	}
	try{

        // set localisation_port
        char** ORB_init_argv = 0;
        int ORB_init_argc=argc;
        std::string str_opt( "" );

        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";
        str_opt += std::string(Common::Options::LOCALISATION_SERVICE_OPTION );
        str_opt += "=" ;
	str_opt += conf.getSystemPortString();
        str_opt += " ";
        str_opt += std::string( "--LBInterceptorSide=LBGroupManager");
	str_opt += " ";
        str_opt += std::string( "--CdmwLBStateTransferPort" );
	str_opt += "=" ;
	str_opt += conf.getStateTransferPort();
        ORB_init_argc += 3;
        ORB_init_argv = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings( str_opt ) );
     
        Cdmw::LB::LBServiceInit::Init(ORB_init_argc, ORB_init_argv);
      

        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
	orb_strategies.add_PoaThreadPool( 30 );
        orb_strategies.add_multicast();

       

        orb =
          Cdmw::OrbSupport::OrbSupport::ORB_init(ORB_init_argc, 
                                                 ORB_init_argv,
                                                 orb_strategies); 

        ::Cdmw::OrbSupport::CodecBase::init(orb.in()); 

        
        CORBA::Object_var obj=
            orb->resolve_initial_references("RootPOA");
        
        PortableServer::POA_var rootPOA =
            PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poa_manager =
            rootPOA->the_POAManager();

        poa_manager->activate();
       
        DefaultDataStoreFactory::Create_data_store(OBJECT_GROUP_DATA_IDENTIFIER);

       // Create the TypeRepository object and register it
        CdmwLB::TypeRepository_var typeRepository =
            new Cdmw::LB::TypeRepository_impl(conf.getSupportedInterfaceMap());
        orb->register_initial_reference("LBTypeRepository", typeRepository.in());
        
         Cdmw::LB::LBGroupManager_impl*  groupManager = new Cdmw::LB::LBGroupManager_impl(orb.in(), rootPOA.in (), conf.getDomainID());
        CdmwLB::LBGroupManager_var gm = groupManager->_this();
        std::cout << "LBGroupManager>> LB Group Manager Started." << std::endl; 

        OrbSupport::OrbSupport::bind_object_to_corbaloc(orb.in(), "group_mgr", gm.in() );
       
        orb->register_initial_reference("LBGroupManager", gm.in());
	
        orb->run();
	delete groupManager;

    }
    catch (CORBA::SystemException& e)
    {
        std::cout << "LBGroupManager>> Unhandled CORBA::SystemException..."
                  << e << std::endl;
        status = -1;
    }

    if(!CORBA::is_nil(orb.in()))
    {
	try{
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    orb->destroy();
	}catch(const CORBA::SystemException& e)
	{
	    std::cout << "LBGroupManager>> Unhandled CORBA::SystemException..."
		      << e << std::endl;
	    status = -1;
	}
    }

    return status;

}
