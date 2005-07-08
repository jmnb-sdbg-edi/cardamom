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


#include <fstream>
#include "testplatformdaemon/PILOT_PlatformDaemon.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/Testable.hpp"

int main( int argc, char* argv[])
{
    using namespace Cdmw;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::PlatformMngt;
    


    
    if ( OS::get_option_value(argc,argv, "--server") == "yes" ) {
        std::ofstream file("platformdaemon_test_marker");
        file << "Have been run";
		int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OS::sleep(timescale*6000);
        return 0;
    }

    
    Cdmw::PlatformMngt::PILOT_PlatformDaemon pilot_PlatformDaemon;
    pilot_PlatformDaemon.start();
    
    return 0;
}

