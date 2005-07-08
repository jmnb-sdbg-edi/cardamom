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

/**
 * @brief implementation for TestLBInit with cppUnit library.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 *
 */

// Standard Files
#include <string>

// Cdmw Files
#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <testlbinit/TestLBInit.hpp>

#include <iostream>
using namespace std;

using namespace Cdmw;


#define ECHO_HEADER() \
    "[Debug] "

#define ECHO_ERROR(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)


#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment ;\
} while(0)

#endif


/*****************************************************************************/

CPPUNIT_TEST_SUITE_REGISTRATION( TestLBInit );

/*****************************************************************************/

void
TestLBInit::setUp()
{
}
void
TestLBInit::tearDown()
{
}

/**
 * @brief test lb init operation.
 *
 */
void TestLBInit::lb_init ()
{
    DEBUG_ECHO("\n");

    OsSupport::OS::create_process ("test_lb_init"," ");

    OsSupport::OS::sleep(1000);
}

/**
 * @brief test register lb policy operation and override lb policy default operation.
 *
 */
void TestLBInit::override_lb_policy_default()
{
    OsSupport::OS::create_process ("test_override_lb_policy_default","--CdmwLBPolicyDefault=UserDefinedStrategy");
    
    OsSupport::OS::sleep(1000);
}

/**
 * @brief test override lb policy force operation.
 *
 */
void TestLBInit::override_lb_policy_force()
{
    DEBUG_ECHO("\n");

    OsSupport::OS::create_process ("test_override_lb_policy_force","--CdmwLBPolicyForce=UserDefinedStrategy");

    OsSupport::OS::sleep(1000);

}



 
