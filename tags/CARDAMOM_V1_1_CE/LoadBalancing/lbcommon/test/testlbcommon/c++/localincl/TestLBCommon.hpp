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

#ifndef INCL_CDMW_TEST_LB_COMMON_HPP
#define INCL_CDMW_TEST_LB_COMMON_HPP

/**
 * @file
 * @brief
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include <Foundation/orbsupport/CORBA.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <LoadBalancing/lbcommon/LB_IOGRFactory.hpp>
#include "testlbcommon/TestHello.skel.hpp"
/**
 * @brief Class to test IOGR Factory methods.
 */
class TestLBCommon : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestLBCommon );
    CPPUNIT_TEST( build_iogr );
    CPPUNIT_TEST( profile_count );
    CPPUNIT_TEST( decode_profile_with_group_tag );
    CPPUNIT_TEST( decode_profile_with_fallback_tag );
    CPPUNIT_TEST( get_member_ref );
    CPPUNIT_TEST_SUITE_END( );

    static CdmwLBCommon::HelloInterface1_var M_obj1;
    static CdmwLBCommon::HelloInterface1_var M_obj2;
    static CdmwLBCommon::HelloInterface2_var M_obj3;
    static CORBA::ORB_var M_orb;
    static Cdmw::LB::IOGRFactory* M_iogrFactory;
    static ::PortableGroup::Location M_loc1;
    static ::PortableGroup::Location M_loc2;
    

public:

    void  setUp();
    void  tearDown() ;
 
    /**
     * @brief test build iogr operation.
     *
     */
    virtual void build_iogr();

    /**
     * @brief test profile count operation.
     *
     */
    virtual void profile_count();

    /**
     * @brief test decode profile with group tag operation.
     *
     */
    virtual void decode_profile_with_group_tag();

    /**
     * @brief test decode profile with fallback tag operation.
     *
     */
    virtual void decode_profile_with_fallback_tag();


    /**
     * @brief test get member ref operation.
     *
     */
    virtual void get_member_ref();
    
};


#endif //INCL_CDMW_TEST_LB_COMMON_HPP
