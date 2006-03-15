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

#include <sstream>
#include <iostream>

#include <Foundation/common/System.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"

#include "testcodec_lib/TestEncoder.hpp"


namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;
 
}; // End anonymous namespace



int main(int argc, char* argv[])
{    
    
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::TestCodec;

    int status = SUCCESS;
    
    std::cout << "Start of encoder" << std::endl;
    

    CORBA::ORB_var orb;                        // orb reference

    // We allow the user to specify a different port number for the decoder
    std::string option("--CdmwDecoderPort");
    std::string portNumber = OS::get_option_value(argc, argv, option) ;

    if ( portNumber == "no" ) {
        std::stringstream defaultDecoderPort;
        defaultDecoderPort << TestEncoder::CDMW_LOCALISATION_PORT;
        portNumber= defaultDecoderPort.str();
    }
    else if ( portNumber != "yes" ) {
        TestEncoder::CDMW_LOCALISATION_PORT = atoi(portNumber.c_str());
    }

    std::cout << "process test_decoder creation with CdmwLocalisationService=" << portNumber << std::endl;
    // Launch decoder

    OS::ProcessId idDecoder = 0;
    try 
    {
        idDecoder = OS::create_process( "test_decoder", "--CdmwLocalisationService=" + portNumber );
    } 
    catch (const Cdmw::BadParameterException & ex)
    {
        std::cerr << "BadParameterException " << ex.what() << std::endl;
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

	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OS::sleep(timescale*3000);
   
    try 
    {
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
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        // ===================================================
        // Activate the root POA manager
        // ===================================================
        PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
        poaManager->activate();
        
        std::cout << "Encoder Started" << std::endl;

        TestEncoder testEncoder(orb.in());
        testEncoder.init();
        testEncoder.start();
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


    // ===================================================
    // destroy orb
    // ===================================================
    
    if (!CORBA::is_nil(orb.in()))
    {
        try 
        {
            orb -> shutdown(false);
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb -> destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    if (idDecoder != 0) {
        OS::kill_process( idDecoder );
    }

    return status;    
}

