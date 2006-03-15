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

#include "datastoretest/DatastoreDefinition.hpp"
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
        std::cout << "create DataStore for DataPart1" << std::endl;
        // StorageHome for DataPart1
        DataPart1StorageHome* dataPart1StorageHome = NULL;

        // StorageHome for DataPart2
        DataPart2StorageHome* dataPart2StorageHome = NULL;

        std::cout << "create DataStore for DataPart1" << std::endl;
        DataPart1StoreFactory::Create_data_store(DATA_STORE_1_IDENTIFIER);

        std::cout << "create DataStorageHome for DataPart1" << std::endl;
        dataPart1StorageHome =
           new DataPart1StorageHome(DATA_STORE_1_IDENTIFIER);

        std::cout << "create DataStore for DataPart2" << std::endl;
        DataPart2StoreFactory::Create_data_store(DATA_STORE_2_IDENTIFIER);

        std::cout << "create DataStorageHome for DataPart2" << std::endl;
        dataPart2StorageHome = 
           new DataPart2StorageHome(DATA_STORE_2_IDENTIFIER);

        bool realOverlap = false;
        if ( Cdmw::OsSupport::OS::get_option_value( argc, argv, "--realOverlap") == "yes" )
        {
            realOverlap = true;
        }
        // Create Threads embedding test
        TestThread testThread1(dataPart1StorageHome, dataPart2StorageHome, 1, realOverlap);
        TestThread testThread2(dataPart1StorageHome, dataPart2StorageHome, 2, realOverlap);

        // Start threads 
        testThread1.start();
        testThread2.start();

        testThread1.join();
        std::cout << "Thread1 is joined" << std::endl;
        testThread2.join();
        std::cout << "Thread2 is joined" << std::endl;
        
	Cdmw::OsSupport::OS::sleep(timescale*2000);

        delete dataPart1StorageHome;
        delete dataPart2StorageHome;

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

    return ret_code;
}

