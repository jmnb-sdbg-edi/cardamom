/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include <iostream>
#include <cstdlib>
#include <string>

#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"

#include "testentitycleanupnotifier/TestEntityCleanupNotifier.hpp"

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;  
};


int
run(CORBA::ORB_ptr orb, int argc, char* argv[])
{
    //
    // Resolve the Root POA
    //
    CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
    PortableServer::POA_var root = PortableServer::POA::_narrow(obj.in());
    PortableServer::POAManager_var poaManager = root -> the_POAManager();
    
    //
    // Initialize the server so that incoming requests are
    // handled from this point on
    //
    poaManager -> activate();
    
    //
    // Get property service
    //
    
    try {
        obj = orb -> resolve_initial_references("PropertyService");
    } catch(const CORBA::ORB::InvalidName&) {
        std::cerr << argv[0]
                  << ": can't resolve `PropertyService'"
                  << std::endl;
        return FAILURE;
    }

    if (CORBA::is_nil(obj.in())) {
        std::cerr << argv[0] 
                  << ": `PropertyService' is a nil object reference"
                  << std::endl;
        return FAILURE;
    }

    CosPropertyService::PropertySetDefFactory_var factory
        = CosPropertyService::PropertySetDefFactory::_narrow(obj.in());

    if(CORBA::is_nil(factory.in())) {
        std::cerr << argv[0] 
                  << ": `PropertyService' is not a PropertyDefSetFactory object "
                  << "reference"
                  << std::endl;
        return FAILURE;
    }
    using namespace Cdmw::LifeCycle;
    //Testing TestNamingInterfaceResolve
    TestEntityCleanupNotifier cleanupNotifierDriver(factory.in());
    cleanupNotifierDriver.start();    
    
    return SUCCESS;
}

int main(int argc, char* argv[])
{
	using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;

    // We allow the user to specify a different port number for the Property Service
    std::string option("--CdmwPropertyPort");
    std::string portNumber = OS::get_option_value(argc, argv, option) ;

    if ( portNumber == "no" ) {
        portNumber="5200";
    } 

    OS::ProcessId idProperty ;
    // Starting the Property Service (Standalone version compiled only in minimal package)
    try
    {
        idProperty 
            = OS::create_process( "cdmw_property_server", "--PropPort=" + portNumber );
    }
    catch ( const Cdmw::BadParameterException& )
    {
        idProperty 
            = OS::create_process( "cdmw_property_server_standalone", "--PropPort=" + portNumber );        
    }
    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OS::sleep(timescale*3000);
    
    std::string propertyCorbaloc
        = " -ORBInitRef PropertyService=corbaloc::localhost:" + 
        portNumber + "/PropertyService";
    

    std::string strArgv = "main " + propertyCorbaloc;
    char** myArgv = String::to_char_array( String::to_strings(strArgv) );
    int myArgc(3);
    
    int status = SUCCESS;
    CORBA::ORB_var orb;

    try
    {
        orb = CORBA::ORB_init(myArgc, myArgv);
	    status = run(orb.in(), myArgc, myArgv);
    }
    catch(const CORBA::Exception& ex)
    {
	    std::cerr << ex << std::endl;
	    status = FAILURE;
    }

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

    // We free memory allocated by String::to_char_array
    for (int iArgc=0; iArgc < myArgc; ++iArgc) {
        delete[] myArgv[iArgc];
    }
    delete[] myArgv;

    // And we kill the Property Server
    OS::kill_process( idProperty );
    return status;    
}


