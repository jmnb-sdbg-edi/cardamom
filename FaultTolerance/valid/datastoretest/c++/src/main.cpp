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

#include <cstdlib>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>

#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>

#include "datastoretest/DatastoreDefinition.hpp"
#include "datastoretest/OrbThread.hpp"
#include "datastoretest/TestThread.hpp"


namespace {
    
const int SUCCESS = 0;
const int FAILURE = 1;  

std::string HELP_OPTION = "--help";


static void usage( std::ostream & os, const std::string &program_name )
{
    os << "Usage:"<< std::endl
       << program_name << " options" << std::endl
       << std:: endl;
}

};  // end anonymous namespace


int main( int argc, char* argv[] )
{
    // the return code
    int ret_code = SUCCESS;
	int timescale = Cdmw::TestUtils::Testable::get_timescale();

    OrbThread orbThread(argc, argv);
    CORBA::ORB_var orb = orbThread.orb();
    PortableServer::POA_var rootPOA = orbThread.poa();

    // Check command line arguments
    std::string help_option = Cdmw::OsSupport::OS::get_option_value(
        argc, argv, HELP_OPTION);

    if (help_option == "yes")
    {
        usage (std::cout, argv[0]);
        return ret_code;
    }

    try
    {
        // Start the orb
        orbThread.start();

        // Timeout for cohort
        Cdmw::OsSupport::OS::Timeval cohort_timeout;
        cohort_timeout.seconds = Cdmw::FT::COHORT_TIMEOUT;
        cohort_timeout.microseconds = 0;
    
        // Timeout for coordinator
        Cdmw::OsSupport::OS::Timeval coordinator_timeout;
        coordinator_timeout.seconds = Cdmw::FT::COORDINATOR_TIMEOUT;
        coordinator_timeout.microseconds = 0;

        Cdmw::FT::DataStoreContext* context;

        std::cout << "create DataStore for DataPart1" << std::endl;
        // StorageHome for DataPart1
        Cdmw::FT::DataPart1StorageHome* dataPart1StorageHome = NULL;

        // DataStorage for DataPart1
        Cdmw::FT::DataPart1Store* dataPart1Store = NULL;

        // StorageHome for DataPart2
        Cdmw::FT::DataPart2StorageHome* dataPart2StorageHome = NULL;

        // DataStorage for DataPart2
        Cdmw::FT::DataPart2Store* dataPart2Store = NULL;

/*
        if ( Cdmw::OsSupport::OS::get_option_value( argc, argv, "--client") == "no" ) 
        {
            // Start the orb for server
            //orbThread.start();
            Cdmw::FT::DataStoreBase::Set_Location_Name("server");
        }
        else
        {
            Cdmw::FT::DataStoreBase::Set_Location_Name("client");	
        }	
*/

        context = new Cdmw::FT::DataStoreContext
            (orb.in(),
             rootPOA.in(),
             Cdmw::FT::MULTICAST_PORT,
             Cdmw::FT::MULTICAST_DOMAIN,
             Cdmw::FT::MULTICAST_IP,
             Cdmw::FT::MAX_TRANSACTION_IN_PROGRESS,
             Cdmw::FT::MAX_TRANSACTION_DONE,
             cohort_timeout,
             coordinator_timeout);

        std::cout << "create DataStore for DataPart1" << std::endl;
        dataPart1Store = 
           new Cdmw::FT::DataPart1Store(Cdmw::FT::DATA_STORE_1_IDENTIFIER,
                                        orb.in(),
                                        rootPOA.in(),
                                        *context);

        std::cout << "create DataStorageHome for DataPart1" << std::endl;
        dataPart1StorageHome =
           new Cdmw::FT::DataPart1StorageHome(Cdmw::FT::DATA_STORE_1_IDENTIFIER);

        std::cout << "create DataStore for DataPart2" << std::endl;
        dataPart2Store = 
           new Cdmw::FT::DataPart2Store(Cdmw::FT::DATA_STORE_2_IDENTIFIER,
                                        orb.in(),
                                        rootPOA.in(),
                                        *context);

        std::cout << "create DataStorageHome for DataPart2" << std::endl;
        dataPart2StorageHome = 
           new Cdmw::FT::DataPart2StorageHome(Cdmw::FT::DATA_STORE_2_IDENTIFIER);

/*
	// Are we started as the CORBA server ?
	if ( Cdmw::OsSupport::OS::get_option_value( argc, argv, "--client") == "no" ) 
	{	
		Cdmw::FT::DataStoreBase::Add_Location(Cdmw::FT::DATA_STORE_1_IDENTIFIER,"client");
		Cdmw::FT::DataStoreBase::Add_Location(Cdmw::FT::DATA_STORE_2_IDENTIFIER,"client");

		dataPart1Store->activate();
		dataPart2Store->activate();
		std::string managerArgs(" --client");
 		//Cdmw::OsSupport::OS::ProcessId process_id = Cdmw::OsSupport::OS::create_process("cdmw_data_store_test", managerArgs);
 		Cdmw::OsSupport::OS::create_process("cdmw_data_store_test", managerArgs);        
		Cdmw::OsSupport::OS::sleep(timescale*10000);
*/
        bool realOverlap = false;
        if ( Cdmw::OsSupport::OS::get_option_value( argc, argv, "--realOverlap") == "yes" )
        {
            realOverlap = true;
        }
        // Create Threads embedding test
        Cdmw::FT::TestThread testThread1(orb.in(), dataPart1StorageHome, dataPart2StorageHome, 1, realOverlap);
        Cdmw::FT::TestThread testThread2(orb.in(), dataPart1StorageHome, dataPart2StorageHome, 2, realOverlap);

        // Start threads 
        testThread1.start();
        testThread2.start();

        //orb->run();

        testThread1.join();
        std::cout << "Thread1 is joined" << std::endl;
        testThread2.join();
        std::cout << "Thread2 is joined" << std::endl;
        
/*	
		dataPart1Store->deactivate();
		dataPart2Store->deactivate();
        std::cout << "Datastores deactivated" << std::endl;
*/
		Cdmw::OsSupport::OS::sleep(timescale*2000);

        delete dataPart1Store;
        delete dataPart1StorageHome;
        delete dataPart2Store;
        delete dataPart2StorageHome;

        //Cdmw::OsSupport::OS::kill_process(process_id);

        // Stop the orb for server
        //orbThread.shutdown();
        //orbThread.join();
/*
	}
	else
	{
		Cdmw::FT::DataStoreBase::Add_Location(Cdmw::FT::DATA_STORE_1_IDENTIFIER,"server");
		Cdmw::FT::DataStoreBase::Add_Location(Cdmw::FT::DATA_STORE_2_IDENTIFIER,"server");
		//orb->run();
        while ((!dataPart1StorageHome->contains(900*Cdmw::FT::SIZESCALE))||(!dataPart2StorageHome->contains(900*Cdmw::FT::SIZESCALE)))
        {
            std::cout << std::endl << "client continue: datasizes are " <<
             dataPart1StorageHome->size() << " and " << dataPart2StorageHome->size() << std::endl;
		    Cdmw::OsSupport::OS::sleep(timescale*5000);
        }
	}
*/
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << e << std::endl;
        ret_code = FAILURE;
    }
    catch (const CORBA::Exception& e)
    {  
        std::cerr << e._name() << std::endl;
        ret_code = FAILURE;
    }
    catch (...)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl;
        ret_code = FAILURE;
    }

    //std::cout << Cdmw::FT::DataStoreBase::Get_Location_Name() << " ended" << std::endl;

    // Stop the orb for both client and server
    orbThread.shutdown();
    orbThread.join();

    return ret_code;
}

