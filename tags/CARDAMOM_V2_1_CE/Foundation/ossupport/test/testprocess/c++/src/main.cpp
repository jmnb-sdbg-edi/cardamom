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
#include <fstream>
#include <cstdlib>
#include <string>

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/Testable.hpp"

using namespace Cdmw::OsSupport;


int main(int argc, char** argv)
{
    // Creation of a file to test
    std::string tmpFile = OS::get_option_value(argc, argv, "--create-file");

    if ( tmpFile != "no" )
    {
        std::ofstream file(tmpFile.c_str());
        file << OS::get_processId() << std::endl;        
    }

    // Process to sleep
    std::string sleepTime = OS::get_option_value(argc, argv, "--sleep");

    if ( sleepTime != "no" )
    {
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OS::sleep(timescale*atoi(sleepTime.c_str())*1000);
    }
    
    return EXIT_SUCCESS;
}


