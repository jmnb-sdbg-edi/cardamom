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

#ifndef INCL_TEST_LB_STRATEGY_HPP
#define INCL_TEST_LB_STRATEGY_HPP

/**
 * @file
 * @brief Class to test Load Balancing Strategy methods.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */
#include <Foundation/orbsupport/CORBA.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include "testlbstrategy/TestPing.skel.hpp"


class TestLBStrategy : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestLBStrategy );
    CPPUNIT_TEST( lb_strategy );
    CPPUNIT_TEST_SUITE_END( );

public:
    void  setUp();
    void  tearDown() ;
    /**
     * @brief test lb round robin and random strategy.
     *
     */
    virtual void lb_strategy ();
};
#endif //INCL_TEST_LB_STRATEGY_HPP
    
