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


#include <string>
#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "testsupervisionobserver/SupervisionObserverTester2.hpp"
#include "idllib/CdmwPlatformMngtSupervisionObserver.stub.hpp"

using namespace std;

using namespace Cdmw;

using namespace Cdmw::Common;

using namespace Cdmw::OsSupport;

int main( int argc, char* argv[] )
{

    int res = 0;

    string observer_port_number = "2479";
    string observer_name = "theObserver";

    OS::ProcessId idObserver;

    try
    {
        // Initialises the ORB
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // Starts the observer
        string arguments("--CdmwLocalisationService=");
        arguments += observer_port_number;
        arguments += " --observer-name=";
        arguments += observer_name;

        idObserver = OS::create_process("cdmw_platform_supervision_observer", arguments);

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*5000);

        // Creates the observer reference
        string observer_corbaloc = "corbaloc::" + OsSupport::OS::get_hostname() +
                                   ":" + observer_port_number + "/" + observer_name;

        CORBA::Object_var observer_obj
        = orb->string_to_object(observer_corbaloc.c_str() );

        if ( CORBA::is_nil( observer_obj.in() ) )
            CDMW_THROW2(
                Cdmw::Exception,
                Cdmw::Exception::FATAL,
                "Invalid corbaloc" );

        CdmwPlatformMngt::SupervisionObserver_var
        observer = CdmwPlatformMngt::SupervisionObserver::_narrow( observer_obj.in() );

        if ( CORBA::is_nil( observer.in() ) )
            CDMW_THROW2(
                Cdmw::Exception,
                Cdmw::Exception::FATAL,
                "Invalid observer reference" );

        // Do test
        SupervisionObserverTester test(orb.in(), "SupervisionObserver", observer.in());

        test.start();

        // Kills the observer
        if ( idObserver != 0 )
            OS::kill_process( idObserver );

        orb->shutdown(false);

        orb->destroy();

    }
    catch ( const CORBA::Exception &e )
    {
        cerr << e._name() << endl;
        res = -1;
    }
    catch ( const Cdmw::Exception &e )
    {
        cerr << e.what() << endl;
        res = -1;
    }
    catch (...)
    {
        cerr << "Uncaught exception" << endl;
        res = -1;
    }

    return res;
}

