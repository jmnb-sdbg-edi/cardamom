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

#include "Foundation/common/System.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"

#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "ServerActivationHandler.hpp"


namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;    
}; // End anonymous namespace



int main(int argc, char* argv[])
{    
    int status = SUCCESS;

    CORBA::ORB_var orb;    
   
    try 
    {
        // Initialise FT service
        std::cout << argv[0] << " : initialise FT service" << std::endl;
        Cdmw::FT::FTServiceInit::Init( argc, argv );
        // ===================================================
        // Initialize the ORB
        // ===================================================
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

        // ===================================================
        // Get the root POA 
        // ===================================================
        CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        // ===================================================
        // Activate the root POA manager
        // ===================================================
        PortableServer::POAManager_var poaManager = rootPOA -> the_POAManager();
        poaManager -> activate();

        // ===================================================
        // Call generated CdmwInit
        // ===================================================
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv);
        
        // ===================================================
        // Get the process name and application name
        // ===================================================
        std::string processName = Cdmw::CdmwInit::InitUtils::get_cdmw_process_name();
        
        std::string applicationName = Cdmw::CdmwInit::InitUtils::get_cdmw_application_name();

        std::cout << "===> main component server of " 
                  << applicationName.c_str() << "/" << processName.c_str() 
                  << std::endl;


        // Create an Activation Handler and register it
        CdmwFT::Location::ActivationHandler_var act = new ServerActivationHandler();
		
        // Register activation handler
        try
        {
            using namespace Cdmw::NamingAndRepository;
            using namespace Cdmw::CommonSvcs::Naming;
            NamingInterface ni = 
                RepositoryInterface::get_domain_naming_interface("demo_ccm_ft/hello_servers");
            ::FT::ObjectGroup_var group = ni.resolve("hello_server.group");
            ::FT::ObjectGroup_var facet = ni.resolve("hello_server_facet.group");
            // get ActivationManager
            CORBA::Object_var obj
                = orb->resolve_initial_references( "FTActivationManager" );
            CdmwFT::Location::ActivationManager_var activation_manager
                = CdmwFT::Location::ActivationManager::_narrow( obj.in() );
            
            // Register activation handler into ActivationManager
            std::cout << "     Server " << processName << ": register activation handler" << std::endl;
            activation_manager->register_activation_handler(group.in(), 
                                                            act.in() );
            activation_manager->register_activation_handler(facet.in(), 
                                                            act.in() );
        } catch( const CORBA::Exception & ex )
        {
            std::cerr
                << "ERROR: Impossible to register FTActivationHandler! \n"
                << ex
                << std::endl;
        }
        // ===================================================
        // wait for CORBA message
        // ===================================================
        orb->run();
    } 
    catch (const Cdmw::Exception & ex) 
    {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    } 
    catch(const CORBA::Exception& ex)  
    {        
        std::cerr << ex << std::endl;
        status = FAILURE;
    }


    // ========================================================
    // program stopping
    // ========================================================
    
    // ===================================================
    // Call generated Cdmw cleanup
    // ===================================================
    Cdmw::CdmwInit::CDMW_cleanup(orb.in());
            
    // ===================================================
    // Call ORB cleanup
    // ===================================================
    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
       
    // =====================================================
    // Destroy orb
    // =====================================================
    if(!CORBA::is_nil(orb.in()))
    {
        try 
        {
            orb -> destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    return status;    
}


