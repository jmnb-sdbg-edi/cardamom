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

#ifndef INCL_TEST_OBJECT_GROUP_MANAGER_HPP
#define INCL_TEST_OBJECT_GROUP_MANAGER_HPP

/**
 * @file
 * @brief Class to test ObjectGroupManager methods.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include <Foundation/orbsupport/CORBA.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include "idllib/CdmwLBGroupManager.stub.hpp"
#include "testlbgroupmanager/TestHello.skel.hpp"

class TestObjectGroupManager : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestObjectGroupManager );
    CPPUNIT_TEST( add_member );
    CPPUNIT_TEST( remove_member );
    CPPUNIT_TEST( get_object_group_id );
    CPPUNIT_TEST( get_object_group_ref );
    CPPUNIT_TEST( get_member_ref );
    CPPUNIT_TEST( locations_of_members );
    CPPUNIT_TEST_SUITE_END( );

    static CdmwLB::LBGroupManager_var M_gm;
    static CORBA::ORB_var M_orb;
    static CdmwGroupManager::HelloInterface1_var M_obj1;
    static CdmwGroupManager::HelloInterface2_var M_obj2;
    static CdmwGroupManager::HelloInterface1_var M_obj3;

public:
    void  setUp();
    void  tearDown() ;

    /**
     * @brief test add operation.
     *
     */
    virtual void add_member();

    /**
     * @brief test remove operation.
     *
     */
    virtual void remove_member();

    /**
     * @brief test get object group id operation.
     *
     */
    virtual void get_object_group_id();


    /**
     * @brief test get object group reference operation.
     *
     */
    virtual void get_object_group_ref();

    /**
     * @brief test get member reference operation.
     *
     */
    virtual void get_member_ref();

    /**
     * @brief test locations of members operation.
     *
     */
    virtual void locations_of_members();
};
#endif //INCL_TEST_OBJECT_GROUP_MANAGER_HPP

