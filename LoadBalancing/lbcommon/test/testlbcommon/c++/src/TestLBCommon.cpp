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
 * @brief _implementation for TestLBCommon with cppUnit library.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 *
 */

#include <iostream>
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/common/String.hpp"
#include "testlbcommon/TestLBCommon.hpp"
#include "Foundation/ossupport/OS.hpp"
#include <LoadBalancing/idllib/CdmwLBCommon.stub.hpp>
#include <LoadBalancing/idllib/CdmwLBIOGRFactory.stub.hpp>
#include "testlbcommon/TestHello_impl.hpp"
#include "testlbcommon/CORBAManager.hpp"

#if CDMW_ORB_VDR == tao
#include <LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif

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

const char* REP_ID = "IDL:thalesgroup.com/HelloInterface1:1.0";
const ::PortableGroup::ObjectGroupId ID = 0xffffff01L;
CdmwLBCommon::HelloInterface1_var TestLBCommon::M_obj1;
CdmwLBCommon::HelloInterface1_var TestLBCommon::M_obj2;
CdmwLBCommon::HelloInterface2_var TestLBCommon::M_obj3;
Cdmw::LB::IOGRFactory* TestLBCommon::M_iogrFactory;
CORBA::ORB_var TestLBCommon::M_orb;
::PortableGroup::Location TestLBCommon::M_loc1;
::PortableGroup::Location TestLBCommon::M_loc2;
/*****************************************************************************/

              CPPUNIT_TEST_SUITE_REGISTRATION( TestLBCommon );

/*****************************************************************************/
    
 
void
TestLBCommon::setUp()
{
    
    M_orb = CORBAManager::instance()->get_orb();
    // create two object hello1
    Cdmw::HelloInterface1_impl* obj1_impl = new Cdmw::HelloInterface1_impl();
    Cdmw::HelloInterface1_impl* obj2_impl = new Cdmw::HelloInterface1_impl();

    // create one object hello2
    Cdmw::HelloInterface2_impl* obj3_impl = new Cdmw::HelloInterface2_impl();

    M_obj1 = obj1_impl->_this();
    M_obj2 = obj2_impl->_this();
    M_obj3 = obj3_impl->_this();

    M_loc1.length(1);
    M_loc1[0].id="HOST1";
    M_loc1[0].kind="hostname";

    M_loc2.length(1);
    M_loc2[0].id="HOST2";
    M_loc2[0].kind="hostname";

    
#if CDMW_ORB_VDR == tao
    M_iogrFactory = new Cdmw::LB::TAO_LB_IOGRFactory(std::cerr,
                                                     M_orb.in());
#endif
    
}

/****************************************************************************/


void
TestLBCommon::tearDown()
{
}

/*****************************************************************************/

/**
 * @brief test build iogr operation.
 *
 */
void
TestLBCommon::build_iogr()
{
    ::CdmwLB::MemberInfo member_info1;
    member_info1.the_reference = CORBA::Object::_duplicate(M_obj1.in());
    member_info1.the_location = M_loc1;

    ::CdmwLB::MemberInfo member_info2;
    member_info2.the_reference = CORBA::Object::_duplicate(M_obj2.in());
    member_info2.the_location = M_loc2;

    DEBUG_ECHO("\n");
    DEBUG_DUMP( "   Create MemberInfos   " );
    ::CdmwLB::MemberInfos new_members;
    new_members.length(2);
    new_members[0] = member_info1;
    new_members[1] = member_info2;

    ::CdmwLB::Objects_var no_fallBack = new ::CdmwLB::Objects;
    no_fallBack->length(0);

    ::PortableGroup::ObjectGroupRefVersion  object_group_ref_version = 1;

    ::CdmwLB::MemberInfos no_members;
    no_members.length(0);

    DEBUG_DUMP( "   Try to create an iogr without members (ObjectNotCreated exception must be raised)   " );
    CPPUNIT_ASSERT_THROW( M_iogrFactory->build_iogr(no_members,
                                                    ID,
                                                    REP_ID,
                                                    "my_domain",
                                                    "Random",
                                                    object_group_ref_version,
                                                    no_fallBack),
                          ::PortableGroup::ObjectNotCreated);

    DEBUG_DUMP( "   Create an iogr with the two previous objects (No exception must be raised)   " );
    CPPUNIT_ASSERT_NO_THROW( M_iogrFactory->build_iogr(new_members,
                                                       ID,
                                                       REP_ID,
                                                       "my_domain",
                                                       "Random",
                                                       object_group_ref_version,
                                                       no_fallBack));

    member_info2.the_reference = CORBA::Object::_duplicate(M_obj3.in());
    member_info2.the_location = M_loc2;

    DEBUG_DUMP( "   Create MemberInfos   " );
    new_members.length(2);
    new_members[0] = member_info1;
    new_members[1] = member_info2;

    DEBUG_DUMP( "   Try to create an iogr with diffrent ior type (ObjectNotCreated exception must be raised)   " );
    CPPUNIT_ASSERT_THROW( M_iogrFactory->build_iogr(new_members,
                                                    ID,
                                                    REP_ID,
                                                    "my_domain",
                                                    "Random",
                                                    object_group_ref_version,
                                                    no_fallBack),
                          ::PortableGroup::ObjectNotCreated);

    DEBUG_DUMP( "   Create MemberInfos   " );
    new_members.length(2);
    new_members[0] = member_info1;
    new_members[1] = member_info1;

    DEBUG_DUMP( "   Try to create an iogr with duplicated ior (ObjectNotCreated exception must be raised)   " );
    CPPUNIT_ASSERT_THROW( M_iogrFactory->build_iogr(new_members,
                                                    ID,
                                                    REP_ID,
                                                    "my_domain",
                                                    "Random",
                                                    object_group_ref_version,
                                                    no_fallBack),
                          ::PortableGroup::ObjectNotCreated);
}

/**
 * @brief test profile count operation.
 *
 */

void
TestLBCommon::profile_count()
{
    ::CdmwLB::MemberInfo member_info1;
    member_info1.the_reference = CORBA::Object::_duplicate(M_obj1.in());
    member_info1.the_location = M_loc1;

    ::CdmwLB::MemberInfo member_info2;
    member_info2.the_reference = CORBA::Object::_duplicate(M_obj2.in());
    member_info2.the_location = M_loc2;

    DEBUG_ECHO("\n");
    DEBUG_DUMP( "   Create MemberInfos   " );
    ::CdmwLB::MemberInfos new_members;
    new_members.length(2);
    new_members[0] = member_info1;
    new_members[1] = member_info2;

    ::CdmwLB::Objects_var no_fallBack = new ::CdmwLB::Objects;
    no_fallBack->length(0);

    ::PortableGroup::ObjectGroupRefVersion  object_group_ref_version = 1;

    ::PortableGroup::ObjectGroup_var iogr;
    DEBUG_DUMP( "   Create an iogr with the two previous objects (No exception must be raised)   " );
    CPPUNIT_ASSERT_NO_THROW( iogr = M_iogrFactory->build_iogr(new_members,
                                                       ID,
                                                       REP_ID,
                                                       "my_domain",
                                                       "Random",
                                                       object_group_ref_version,
                                                       no_fallBack));
    DEBUG_DUMP( "   Count the number of profile into the iogr just created (with two members)   ");
    CORBA::ULong nb_profile = M_iogrFactory->profile_count(iogr.in());
    CPPUNIT_ASSERT(nb_profile == 2);
}

/**
 * @brief test decode profile with group tag operation.
 *
 */

void
TestLBCommon::decode_profile_with_group_tag()
{
    ::CdmwLB::MemberInfo member_info1;
    member_info1.the_reference = CORBA::Object::_duplicate(M_obj1.in());
    member_info1.the_location = M_loc1;

    ::CdmwLB::MemberInfo member_info2;
    member_info2.the_reference = CORBA::Object::_duplicate(M_obj2.in());
    member_info2.the_location = M_loc2;

    DEBUG_ECHO("\n");
    DEBUG_DUMP( "   Create MemberInfos   " );
    ::CdmwLB::MemberInfos new_members;
    new_members.length(2);
    new_members[0] = member_info1;
    new_members[1] = member_info2;

    ::CdmwLB::Objects_var no_fallBack = new ::CdmwLB::Objects;
    no_fallBack->length(0);

    ::PortableGroup::ObjectGroupRefVersion  object_group_ref_version = 1;

    ::PortableGroup::ObjectGroup_var iogr;
    DEBUG_DUMP( "   Create an iogr with the two previous objects (No exception must be raised)   " );
    CPPUNIT_ASSERT_NO_THROW( iogr = M_iogrFactory->build_iogr(new_members,
                                                              ID,
                                                              REP_ID,
                                                              "my_domain",
                                                              "Random",
                                                              object_group_ref_version,
                                                              no_fallBack));

    DEBUG_DUMP( "   Check if the profile 1 contains the TAG_LB_GROUP (No exception must be raised)  " );
    CPPUNIT_ASSERT_NO_THROW(M_iogrFactory->decode_profile_with_group_tag (iogr.in(),0));

    DEBUG_DUMP( "   Check if the profile 2 contains the TAG_LB_GROUP (No exception must be raised)  " );
    CPPUNIT_ASSERT_NO_THROW(M_iogrFactory->decode_profile_with_group_tag (iogr.in(),1));

    DEBUG_DUMP( "   Check if the profile 3 doesn't exist (ProfileNotFound exception must be raised)  " );
    CPPUNIT_ASSERT_THROW(M_iogrFactory->decode_profile_with_group_tag (iogr.in(),2),
                            CdmwLB::ProfileNotFound);

    DEBUG_DUMP( "   Check if no TAG_LB_GROUP is present into a simple ior (TagNotFound exception must be raised)  " );
    CPPUNIT_ASSERT_THROW(M_iogrFactory->decode_profile_with_group_tag (M_obj1.in(),0),
                         CdmwLB::TagNotFound);
}

/**
 * @brief test decode profile with fallback tag operation.
 *
 */
void
TestLBCommon::decode_profile_with_fallback_tag()
{
    ::CdmwLB::MemberInfo member_info;
    member_info.the_reference = CORBA::Object::_duplicate(M_obj2.in());
    member_info.the_location = M_loc1;

    DEBUG_ECHO("\n");
    DEBUG_DUMP( "   Create MemberInfos   " );
    ::CdmwLB::MemberInfos new_members;
    new_members.length(1);
    new_members[0] = member_info;

    ::CdmwLB::Objects_var fallBack = new ::CdmwLB::Objects;

    fallBack->length(0);

    ::PortableGroup::ObjectGroupRefVersion  object_group_ref_version = 1;

    ::PortableGroup::ObjectGroup_var iogr;

    DEBUG_DUMP( "   Try to create an iogr without fallback replicas (No exception must be raised)   " );
    CPPUNIT_ASSERT_NO_THROW( iogr = M_iogrFactory->build_iogr(new_members,
                                                              ID,
                                                              REP_ID,
                                                              "my_domain",
                                                              "Random",
                                                              object_group_ref_version,
                                                              fallBack));

    DEBUG_DUMP( "   Check if the profile 1 contains the TAG_FALLBACK (TagNotFound exception must be raised)  " );
    CPPUNIT_ASSERT_THROW(M_iogrFactory->decode_profile_with_fallback_tag (iogr.in(),0),
                         CdmwLB::TagNotFound);
    
    DEBUG_DUMP( "   Check if the profile 2 doesn't exist (ProfileNotFound exception must be raised)  " );
    CPPUNIT_ASSERT_THROW(M_iogrFactory->decode_profile_with_fallback_tag (iogr.in(),1),
                         CdmwLB::ProfileNotFound);



    fallBack->length(1);
    fallBack[0L] = CORBA::Object::_duplicate(M_obj1.in());

    DEBUG_DUMP( "   Try to create an iogr with a fallback replica (No exception must be raised)   " );
    CPPUNIT_ASSERT_NO_THROW( iogr = M_iogrFactory->build_iogr(new_members,
                                                              ID,
                                                              REP_ID,
                                                              "my_domain",
                                                              "Random",
                                                              object_group_ref_version,
                                                              fallBack));

    DEBUG_DUMP( "   Check if the profile 1 contains the TAG_FALLBACK (TagNotFound exception must be raised)  " );
    CPPUNIT_ASSERT_THROW(M_iogrFactory->decode_profile_with_fallback_tag (iogr.in(),0),
                         CdmwLB::TagNotFound);

    DEBUG_DUMP( "   Check if the profile 1 contains the TAG_LB_GROUP (No exception must be raised)  " );
    CPPUNIT_ASSERT_NO_THROW(M_iogrFactory->decode_profile_with_group_tag (iogr.in(),0));

    DEBUG_DUMP( "   Check if the profile 2 (fallback replica) contains the TAG_FALLBACK (No exception must be raised)  " );
    CPPUNIT_ASSERT_NO_THROW(M_iogrFactory->decode_profile_with_fallback_tag (iogr.in(),1));

    DEBUG_DUMP( "   Check if the profile 2 (fallback replica) contains the TAG_LB_GROUP (No exception must be raised)  " );
    CPPUNIT_ASSERT_NO_THROW(M_iogrFactory->decode_profile_with_group_tag (iogr.in(),1));

    DEBUG_DUMP( "   Check if the profile 3 doesn't exist (ProfileNotFound exception must be raised)  " );
    CPPUNIT_ASSERT_THROW(M_iogrFactory->decode_profile_with_fallback_tag (iogr.in(),2),
                         CdmwLB::ProfileNotFound);
}

/**
 * @brief test get member ref operation.
 *
 */
void
TestLBCommon::get_member_ref()
{
    ::CdmwLB::MemberInfo member_info1;
    member_info1.the_reference = CORBA::Object::_duplicate(M_obj1.in());
    member_info1.the_location = M_loc1;

    DEBUG_ECHO("\n");
    DEBUG_DUMP( "   Create MemberInfos   " );
    ::CdmwLB::MemberInfos new_members;
    new_members.length(1);
    new_members[0] = member_info1;

    ::CdmwLB::Objects_var no_fallBack = new ::CdmwLB::Objects;
    no_fallBack->length(0);

    ::PortableGroup::ObjectGroupRefVersion  object_group_ref_version = 1;

    ::PortableGroup::ObjectGroup_var iogr;
    DEBUG_DUMP( "   Create an iogr with the previous object (No exception must be raised)   " );
    CPPUNIT_ASSERT_NO_THROW( iogr = M_iogrFactory->build_iogr(new_members,
                                                       ID,
                                                       REP_ID,
                                                       "my_domain",
                                                       "Random",
                                                       object_group_ref_version,
                                                       no_fallBack));
    CORBA::Object_var member_ref;
    DEBUG_DUMP( "   Try to invoke get_member_ref on the first profile (No exception must be raised)   ");
    CPPUNIT_ASSERT_NO_THROW( member_ref = M_iogrFactory->get_member_ref(iogr.in(), 0));
    DEBUG_DUMP("   ...MemberReference --> " << M_orb->object_to_string(member_ref.in()));

    DEBUG_DUMP( "   Try to invoke get_member_ref on the second profile (MemberNotFound exception must be raised)   ");
    CPPUNIT_ASSERT_THROW( member_ref = M_iogrFactory->get_member_ref(iogr.in(), 1),
                         PortableGroup::MemberNotFound);
}
