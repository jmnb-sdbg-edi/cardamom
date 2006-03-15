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

#ifndef FEDERATED_CONTROLLED_CLOCK_TEST_H
#define FEDERATED_CONTROLLED_CLOCK_TEST_H
/**
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 * @author Raffaele Mele <rmele@progesi.it>
 */

#include "testcontrolledexecutor/ControlledClockTest.hpp"

/**
 * @brief Class to test ControlledClock method.
 */
class FederatedControlledClockTest 
:  public ControlledClockTest 
{
    CPPUNIT_TEST_SUB_SUITE( FederatedControlledClockTest , ControlledClockTest);
    CPPUNIT_TEST_SUITE_END( );

public:
    void  setUp();
    void  tearDown();

    void  multiple_virtual_clock();
};

#endif // CONTROLLED_CLOCK_TEST_H

