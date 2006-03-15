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


#include <fstream>
#include <sstream>
#include <iostream>
#include "Foundation/ossupport/OS.hpp"
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"
#include "testorblogging/TestHello_impl.hpp"
#include "testorblogging/TestOrbLoggingDriver.hpp"


namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;
 
}; // End anonymous namespace


using Cdmw::OsSupport::OS;

int
main(int argc, char** argv)
{
    int status = SUCCESS;
    try 
    {
        // ===================================================
        // Initialize the ORB
        // ===================================================
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
        
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);
        
        // ===================================================
        // Get the root POA 
        // ===================================================
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
        // ===================================================
        // Activate the root POA manager
        // ===================================================
        PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
        poaManager->activate();
        
        // Are we started as the CORBA server ?
        
        if ( OS::get_option_value( argc, argv, "--server") == "yes" )
        {
        
            
            Cdmw::Logging::TestHello_impl* hello_impl = new Cdmw::Logging::TestHello_impl(orb.in());
            Cdmw::TestHello_var hello = hello_impl->_this();
            
            // We write its ior in a file
            std::ofstream iorFile("TestHello.ior");
            CORBA::String_var strIor = orb->object_to_string( hello.in() );            
            iorFile << strIor.in() << std::endl;
            iorFile.close();
            
            orb->run();
            
            // Else, we are the main test driver
        }
        else
        {
            Cdmw::Logging::TestOrbLoggingDriver testOrbLoggingDriver(orb.in(),
                                                                     argv[0]);
            testOrbLoggingDriver.start();
        }
        
    } 
    catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex)
    {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
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
    catch(...)  
    {        
        std::cerr << "unexpected exception" << std::endl;
        status = FAILURE;
    }
    
}

