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

#include "Foundation/testutils/Testable.hpp"

#include "test/TestSingleShotTimer.hpp"
#include "test/TestLocationPriorityStrategy.hpp"
#include "test/TestMutualLivelinessRepository.hpp"

#include <exception>

#include <Foundation/logging/LogManager.hpp>

void exceptionHandler(void)
{
	std::cout << "Unexpected Exception : Test failed" << std::endl;
}

int main(int argc, char* argv[])
{
    using namespace Cdmw::FT::Supervision;  
    
    // the return code 
    int ret_code = 0;

    std::set_unexpected(&exceptionHandler);

    // Init the Logging library
    Cdmw::Logging::LogManager::Init(argc, argv);
    
    try
    {
      TestSingleShotTimer test1("Single Shot Timer");
      test1.start();
      TestLocationPriorityStrategy test2("Location Priority Strategy");
      test2.start();
      TestMutualLivelinessRepository test3("Mutual Liveliness Repository");
      test3.start();
    }
    catch(const std::exception &e)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl
            << e.what() << std::endl;
        ret_code = EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl;
        ret_code = EXIT_FAILURE;
    }
    
    return ret_code;

}

