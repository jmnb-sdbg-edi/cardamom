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

/* @author Fabrizio Lorenna <card03@selex-si.com> */

#include <fstream>
#include "testvalidnavigability/TestValidNavigability.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"

#include "cppunit/TestResult.h"
#include "cppunit/TestResultCollector.h"
#include "cppunit/TestRunner.h"
#include "cppunit/XmlOutputter.h"

int main( int argc, char* argv[])
{
    using namespace Cdmw;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::PlatformMngt;
    
    //Cdmw::PlatformMngt::TestValidNavigability testnav;

	 /*
         ===================================================
         Initialize the ORB
         ===================================================
	 */
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();



        CORBA::ORB_var orb;

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv,orb_strategies);
        Cdmw::TestUtils::CORBATestManager::instance()->init(orb.in(), NULL,argc,&argv);

     Cdmw::TestUtils::CORBATestManager::instance()->run_tests();
     Cdmw::TestUtils::CORBATestManager::instance()->summary();
     return Cdmw::TestUtils::CORBATestManager::instance()->was_successful(); 
}

