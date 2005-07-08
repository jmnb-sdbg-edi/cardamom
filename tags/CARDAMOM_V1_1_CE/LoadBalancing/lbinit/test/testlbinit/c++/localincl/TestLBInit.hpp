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

#ifndef INCL_TEST_LB_INIT_HPP
#define INCL_TEST_LB_INIT_HPP

/**
 * @file
 * @brief Class to test Load Balancing Init methods.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */
#include <Foundation/orbsupport/CORBA.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include "testlbinit/TestHello.skel.hpp"


class TestLBInit : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestLBInit );
    CPPUNIT_TEST( lb_init );
    CPPUNIT_TEST( override_lb_policy_default );
    CPPUNIT_TEST( override_lb_policy_force );
    CPPUNIT_TEST_SUITE_END( );

public:
    void  setUp();
    void  tearDown() ;
    /**
     * @brief test lb init operation.
     *
     */
    virtual void lb_init ();

    /**
     * @brief test override lb policy default operation.
     *
     */
    virtual void override_lb_policy_default();

    /**
     * @brief test override lb policy force operation.
     *
     */
    virtual void override_lb_policy_force();

};
#endif //INCL_TEST_LB_INIT_HPP
    
