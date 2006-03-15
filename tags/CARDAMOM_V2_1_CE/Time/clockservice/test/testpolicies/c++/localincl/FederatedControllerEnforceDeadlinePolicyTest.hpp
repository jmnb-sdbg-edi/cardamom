/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#ifndef FEDERATEDCONTROLLERENFORCEDEADLINEPOLICYTEST_HPP
#define FEDERATEDCONTROLLERENFORCEDEADLINEPOLICYTEST_HPP

/**
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CdmwTime.stub.hpp"
#include "testpolicies/ControllerEnforceDeadlinePolicyTest.hpp"
#include <functional>

class FederatedControllerEnforceDeadlinePolicyTest 
    : public ControllerEnforceDeadlinePolicyTest
{
    CPPUNIT_TEST_SUB_SUITE(FederatedControllerEnforceDeadlinePolicyTest, ControllerEnforceDeadlinePolicyTest);
    CPPUNIT_TEST_SUITE_END();

public:

    FederatedControllerEnforceDeadlinePolicyTest();

    void setUp();
    void tearDown() ;
};

#endif // FEDERATEDCONTROLLERENFORCEDEADLINEPOLICYTEST_HPP

