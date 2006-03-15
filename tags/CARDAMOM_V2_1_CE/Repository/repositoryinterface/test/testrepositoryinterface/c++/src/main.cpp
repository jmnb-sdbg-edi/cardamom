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


#include <iostream>
#include <cstdlib>
#include <string>

#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"

#include "testrepositoryinterface/TestRepository.hpp"
#include "testrepositoryinterface/TestMacroDefs.hpp"

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;  
};

using namespace Cdmw::NamingAndRepository;

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
    // Get naming service
    //
    try
    {
        obj = orb -> resolve_initial_references("NameService");
    }
    catch(const CORBA::ORB::InvalidName&)
    {
        NI_TEST_CERR(argv[0] << ": can't resolve `NameService'");
        return FAILURE;
    }

    if(CORBA::is_nil(obj.in()))
    {
        NI_TEST_CERR(argv[0] << ": `NameService' is a nil object reference");
        return FAILURE;
    }
    
    CosNaming::NamingContextExt_var nc =
        CosNaming::NamingContextExt::_narrow(obj.in());

    if(CORBA::is_nil(nc.in()))
    {
        NI_TEST_CERR(argv[0] << ": `NameService' is not a NamingContextExt object "
	     << "reference");
        return FAILURE;
    }

    
    //Testing TestNamingInterfaceResolve
    TestRepository repositoryDriver(nc.in());
    repositoryDriver.start();    
    
    return SUCCESS;
}

int main(int argc, char* argv[])
{
	using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;
    int status = SUCCESS;
	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    // We allow the user to specify a different port number for the Name Service
    std::string option("--CdmwNamePort");
    std::string portNumber = OS::get_option_value(argc, argv, option) ;

    if ( portNumber == "no" ) {
        portNumber="5000";
    } 

    // Starting the Name Service
    OS::ProcessId idNaming = 0;
    try{
    #if (CDMW_ORB_VDR == orbacus )
        std::string namingservice="nameserv";
        idNaming = OS::create_process( namingservice, "-OAport " + portNumber );
    #elif (CDMW_ORB_VDR == tao )
        std::string namingservice="Naming_Service";
        idNaming = OS::create_process( namingservice, "-ORBEndpoint iiop://localhost:" + portNumber );
    #endif
    }
    catch (Cdmw::BadParameterException& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        status = FAILURE;
    }

	
    OS::sleep(timescale*10000);
    
#if (CDMW_ORB_VDR == orbacus )
    std::string namingCorbaloc = "-ORBnaming corbaloc::localhost:" + portNumber + "/NameService";
#elif (CDMW_ORB_VDR == tao )
    std::string namingCorbaloc = "-ORBInitRef NameService=corbaloc:iiop:localhost:" + portNumber + "/NameService";
#endif

    // Starting the Repository Simulator
    std::string simRepositoryArgs(namingCorbaloc);
    simRepositoryArgs += " -DOM MyDefault.system/MyDefault.domain";
    simRepositoryArgs += " -DOM MyDefault.system/MyDefault.domain/dom1.1/dom2.2";
    simRepositoryArgs += " -DOM MyDefault.system/MyDefault.domain/dom2.1";
    simRepositoryArgs += " -DOM Another.system/Another.domain";
    simRepositoryArgs += " -DOM Another.system/Another.domain/domX.Y ";

    OS::ProcessId idRepository = 0;
    try {
    idRepository = OS::create_process( "cdmw_simulated_repository", simRepositoryArgs );
    }
    catch (Cdmw::BadParameterException& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        status = FAILURE;
    }



    OS::sleep(timescale*10000);
    

//    cdmw_simulated_repository -ORBnaming corbaloc::localhost:5000/NameService -DOM MyDefault.system/MyDefault.domain -DOM MyDefault.system/MyDefault.domain/dom1.1/dom2.2 -DOM MyDefault.system/MyDefault.domain/dom2.1 -DOM Another.system/Another.domain -DOM Another.system/Another.domain/domX.Y 

    std::string strArgv = "testRI " + namingCorbaloc;
    char** myArgv = String::to_char_array( String::to_strings(strArgv) );
    int myArgc(3);
    

    CORBA::ORB_var orb;

    try
    {
        orb = CORBA::ORB_init(myArgc, myArgv);
	    status = run(orb.in(), myArgc, myArgv);
    }
    catch(const CORBA::Exception& ex)
    {
	    NI_TEST_CERR(ex._name());
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
    	    NI_TEST_CERR(ex._name());
	        status = FAILURE;
    	}
    }        

    // We free memory allocated by String::to_char_array
    for (int iArgc=0; iArgc < myArgc; ++iArgc) {
        delete[] myArgv[iArgc];
    }
    delete[] myArgv;

    // And we kill the Repository simulator and Name Server
    OS::kill_process( idRepository );
    OS::kill_process( idNaming );
    return status;    
}


