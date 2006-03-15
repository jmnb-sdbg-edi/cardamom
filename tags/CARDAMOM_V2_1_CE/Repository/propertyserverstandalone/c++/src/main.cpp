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
#include <Foundation/common/System.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/testutils/Testable.hpp>

#if CDMW_ORB_VDR == orbacus	    
#include <Foundation/idllib/CosProperty.stub.hpp>
#elif CDMW_ORB_VDR == tao
#include <orbsvcs/Property/CosPropertyService_i.h>
#endif

namespace
{
    const int SUCCESS = 0;
    const int FAILURE = 1;  
    
    const char* PROP_SERVICE_ID               = "PropertyService";
    const std::string PROPERTY_PORT_OPTION    = "--PropPort";
    const int POA_THREAD_POOL_SIZE = 5;
    
    static void
        usage(std::ostream & os, const std::string &program_name)
    {
        os << "Usage:\n"
            << program_name << " options\n"
            << "\n"
            << "Options:\n"
            << "--help                     Display this information.\n"
            << "\n"
            << "[--PropPort=<nbr>]         Port of Property Service.\n"
            << std:: endl;
    }


};  // end anonymous namespace


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
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;

    int status = SUCCESS;

#if CDMW_ORB_VDR == orbacus	    
    OS::ProcessId idPropServer;
#endif

    std::string help = OS::get_option_value (argc, argv, "--help");
    if(help == "yes") 
    {
        usage(std::cout, argv[0]);
        return SUCCESS;
    }

    // get Property service port
    std::string propServPort = OS::get_option_value (argc, argv, PROPERTY_PORT_OPTION);
    
    if (propServPort  == "no") 
    {
        propServPort = "5015";
    } 

    std::string root_poa_port_option;
    root_poa_port_option 
        = OS::get_option_value(argc, argv, Options::LOCALISATION_SERVICE_OPTION);

    
    // We must ensure to be listening on a host and a port known to the client
    std::string str_opt( "" );
    char** argv_tmp = NULL;
    int argc_tmp = argc;
    
#if CDMW_ORB_VDR == orbacus	 
    if (root_poa_port_option == "no")
    {
        // no localisation port defined
        // Nothing special to do! Just copy argc argv
        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";
        
    }
    else
    {
        // a localisation port is defined ! It is becomes the propserv port
        propServPort = root_poa_port_option;
        
        // and we should remove the option from the args!
        argc_tmp = 0;
        for (int i=0; i<argc; i++)
        {
            std::string arg(argv[i]);
            
            // We check if in the current argument we have, the option
            if (arg.find(Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION) 
                == std::string::npos ) 
            {
                // no localisation port, copy argument
                str_opt += std::string( argv[i] ) + " ";
                argc_tmp ++;
            }
        }
    }
    argv_tmp = Cdmw::Common::String::to_char_array(
        Cdmw::Common::String::to_strings( str_opt ) );


#elif CDMW_ORB_VDR == tao	
    if (root_poa_port_option == "no")
    {
        // no localisation port defined
        // add propServPort as LOCALISATION_SERVICE_OPTION
        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";

        str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
        str_opt += "=";
        str_opt += propServPort + " ";
        argc_tmp++;
    }
    else
    {
        // a localisation port is defined ! It is becomes the propserv port
        // Nothing special to do! Just copy argc argv
        propServPort = root_poa_port_option;

        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";
    }
    argv_tmp = Cdmw::Common::String::to_char_array(
        Cdmw::Common::String::to_strings( str_opt ) );

#endif

    // Initialize the ORB
    Cdmw::OrbSupport::StrategyList orb_strategies;
    orb_strategies.add_OrbThreaded();
    orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
    
    
    orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc_tmp, argv_tmp, orb_strategies);
    
    CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(poa_obj.in());
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    
#if CDMW_ORB_VDR == orbacus	    
    // Starting Orbacus Propserver
    std::cout << "Starting Orbacus propserv on port " << propServPort << std::endl;
    idPropServer = OS::create_process("propserv", " -OAport " + propServPort);

    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OS::sleep(timescale*2000);

    std::cout << "Propserv started" << std::endl;
#else  // TAO
    // Create PropertySet factory and then register.
    std::cout << "Create Property Set Def Factory" << std::endl;
    TAO_PropertySetDefFactory *propsetdef_factory_impl = new TAO_PropertySetDefFactory();
    
    CosPropertyService::PropertySetDefFactory_var propsetdef_factory 
        = propsetdef_factory_impl->_this();

    std::cout << "bind_object_to_corbaloc Property Set Def Factory" << std::endl;
    Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc (orb.in(),
                                                            PROP_SERVICE_ID,
                                                            propsetdef_factory.in());
#endif

    std::cout << "Property Set Factory's corbaloc name: " << PROP_SERVICE_ID << std::endl;
    std::cout << "Property Set Factory's corbaloc port: " << propServPort << std::endl;
    
    try
    {
            struct sigaction action;
            action.sa_handler=exit_handler;
            sigemptyset(&action.sa_mask);
            sigaction(SIGTERM, &action, NULL);
            sigaction(SIGINT, &action, NULL);
            sigaction(SIGQUIT, &action, NULL);
            sigaction(SIGABRT, &action, NULL);

            try
            {
                orb->run();
            }
            catch(const CORBA::Exception& ex)
            {
                std::cerr << ex << std::endl;
                status = FAILURE;
            }
    }
    catch(const CORBA::ORB::InvalidName&)
    {
        std::cerr << "Can't resolve `NameService'" << std::endl;
        status = FAILURE;
    }
    catch(const CORBA::Exception& ex)
    {
        std::cerr << ex << std::endl;
        status = FAILURE;
    }
    catch (...) 
    {
        std::cerr << "unknown exception in main" << std::endl;
        status = FAILURE;
    }           

    if(!CORBA::is_nil(orb.in()))
    {
        try {
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb -> destroy();
#if CDMW_ORB_VDR == orbacus	    
            OS::kill_process(idPropServer);
#endif
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        
    
    return status;    
}

