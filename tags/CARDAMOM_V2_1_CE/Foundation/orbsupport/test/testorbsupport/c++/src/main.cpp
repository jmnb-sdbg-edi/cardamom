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


#include "Foundation/ossupport/OS.hpp"

#include "testorbsupport/TestOrbSupportDriver.hpp"
#include "testorbsupport/TestOrbSupportServerProcess.hpp"



using Cdmw::OrbSupport::TestOrbSupportDriver;
using Cdmw::OrbSupport::TestOrbSupportServerProcess;
using Cdmw::OsSupport::OS;

int
main(int argc, char** argv)
{

    // Are we started as the CORBA server ?

    if ( OS::get_option_value( argc, argv, "--server") == "yes" )
    {

        TestOrbSupportServerProcess testOrbSupportServerProcess;
        testOrbSupportServerProcess.start( argc, argv );

        // Else, we are the main test driver
    }
    else
    {

        if (OS::get_option_value(argc, argv, "--valid-only") == "yes" )
        {
            TestOrbSupportDriver testOrbSupportDriver( argv[0], true );
            testOrbSupportDriver.start();
        }
        else
        {
            TestOrbSupportDriver testOrbSupportDriver( argv[0] );
            testOrbSupportDriver.start();
        }
    }

}

