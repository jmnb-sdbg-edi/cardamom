/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Time/clockservice/CdmwCosClock.hpp"


#include "ClientProcessControl.hpp"

namespace
{
const int SUCCESS = 0;
const int FAILURE = 1;

const int POA_THREAD_POOL_SIZE = 5;

}; // End anonymous namespace

int main(int argc, char* argv[])
{
     
	int status = SUCCESS;

	std::cerr	<< "-------------------------" << std::endl
			<< "      Start Client" << std::endl
			<< "-------------------------" << std::endl;
	for (int n=0; n < argc; n++)
		std::cerr << "argv[" << n <<"]: " << argv[n] << std::endl;
	std::cerr 	<< "-------------------------" << std::endl;

	//number of test
	int niter = 1000;

	//delay between calls
    unsigned long deltaT = 10;

	std::cout << "Reading Configuration File..." << std::endl;
	//read configuration file

	FILE * fd;
	if ((fd = fopen("client.cfg", "r"))!=NULL) {
		fscanf(fd,"%d",&niter);  // number of waking ups requested to the Timer
		fscanf(fd,"%lu",&deltaT);// deltaT (msec) of the waking ups
		if (ferror(fd)) {
			std::cerr << "Error in reading file client.cfg\n";
			exit(-1);
		}
		fclose(fd);
	}
	else {
		std::cerr << "Error: client.cfg don't exist!\n";
		//exit(-1);
	}

	std::cout << "Sample invocation= " << niter << std::endl;
    std::cout << "Delay (ms)= " << deltaT<< std::endl;

    // transform millisec in TimeBase::TimeT formats
    // Note: 1s == 10000000 TimeBase::TimeT units.
    deltaT = deltaT * 10000;
    std::cout << "Delay (usec/10)= " << deltaT<< std::endl;

                           
	CORBA::ORB_var orb;                        // orb reference

	try
	{
		// ===================================================
		// Initialize the ORB
		// ===================================================
		printf("*** client: Initialise The ORB\n");
		Cdmw::OrbSupport::StrategyList orb_strategies;
		orb_strategies.add_OrbThreaded();
		orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
		orb_strategies.add_multicast();

		printf("*** client: ORB_init\n");
		orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

		// ===================================================
		// Get the root POA
		// ===================================================
		printf("*** client: Get Root POA\n");
		CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
		PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

		// ===================================================
		// Activate the root POA manager
		// ===================================================
		printf("*** client: Activate the ORB\n");
		PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
		poaManager->activate();

		// ===================================================
		// create the process control for platform management
		// ===================================================
		Cdmw::CdmwInit::ProcessControl_var process_ctrl =
              new TimerThroughput::ClientProcessControl(orb.in(),
		            niter,
                                                 deltaT);
		// ===================================================
		// Call generated CdmwInit
		// ===================================================
		printf("*** client: Call generated CDMW_Init\n");
		Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, process_ctrl.in());


		// ===================================================
		// wait for CORBA message
		// ===================================================
		printf("*** client: ORB run\n");
		orb->run();
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

	// ===================================================
	// destroy orb
	// ===================================================

	if (!CORBA::is_nil(orb.in()))
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
