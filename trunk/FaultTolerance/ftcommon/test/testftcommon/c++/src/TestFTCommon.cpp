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

#include <iostream>
#include "testftcommon/TestFTCommon.hpp"
#include "Foundation/ossupport/OS.hpp"
#include <FaultTolerance/idllib/CdmwFTCommon.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTIOGRFactory.stub.hpp>
#include "testftcommon/TestHello_impl.hpp"
#include <FaultTolerance/ftcommon/FTConfiguration.hpp>

#if CDMW_ORB_VDR == tao
#include <FaultTolerance/ftcommon/TAO_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif


const char* REP_ID = "IDL:thalesgroup.com/HelloInterface1:1.0";
const ::FT::ObjectGroupId ID = 0xffffff01L;

TestFTCommon::TestFTCommon(CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA, int& argc, char** argv)
    : Testable( "TestFTCommon" ),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_rootPOA(PortableServer::POA::_duplicate(rootPOA)),
      m_argc(argc),
      m_argv(argv)
{
}

TestFTCommon::~TestFTCommon()
{
}

void 
TestFTCommon::do_tests()
{
    set_nbOfRequestedTestOK(21);
    
    
    try 
    {      
        // create two object hello1
        TEST_INFO("Create three objects");
        Cdmw::HelloInterface1_impl* obj1_impl = new Cdmw::HelloInterface1_impl();
        Cdmw::HelloInterface1_impl* obj2_impl = new Cdmw::HelloInterface1_impl();
        Cdmw::HelloInterface2_impl* obj3_impl = new Cdmw::HelloInterface2_impl();
        
        HelloInterface1_var obj1 = obj1_impl->_this();
        HelloInterface1_var obj2 = obj2_impl->_this();
        HelloInterface2_var obj3 = obj3_impl->_this();
        

        ::FT::Location loc1;
        loc1.length(1);
        loc1[0].id="HOST1";
        loc1[0].kind="hostname";

        ::FT::Location loc2;
        loc2.length(1);
        loc2[0].id="HOST2";
        loc2[0].kind="hostname";

        ::CdmwFT::MemberInfo member_info1;
        member_info1.the_reference = CORBA::Object::_duplicate(obj1.in());
        member_info1.the_location = loc1;

        ::CdmwFT::MemberInfo member_info2;
        member_info2.the_reference = CORBA::Object::_duplicate(obj2.in());
        member_info2.the_location = loc2;
        
        TEST_INFO("Create MemberInfos");
        ::CdmwFT::MemberInfos new_members;
        new_members.length(2);
        new_members[0] = member_info1;
        new_members[1] = member_info2;
        
        
        ::CdmwFT::Objects_var no_fallBack = new ::CdmwFT::Objects;     
        no_fallBack->length(0);
        
        ::FT::Location primary_location;
        primary_location.length(0 );

        ::FT::ObjectGroupRefVersion  object_group_ref_version = 1;

        // create an IOGRFactory
        TEST_INFO("Create the IOGRFactory");
        Cdmw::FT::IOGRFactory* m_iogrFactory;

#if CDMW_ORB_VDR == tao
        m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(m_orb.in(),
                                                      std::cout);
#endif
        ::CdmwFT::MemberInfos no_members;
        no_members.length(0);
        
        TEST_INFO("Try to create an iogr without members");
        try
        {
            CORBA::Object_var iogr = 
            m_iogrFactory->build_iogr(no_members,
                                      primary_location,
                                      ID,
                                      REP_ID,
                                      "my_domain",
                                      object_group_ref_version,
                                      no_fallBack);
            TEST_FAILED();
        }        
        catch(::FT::ObjectNotCreated)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        
        TEST_INFO("create an iogr with the two previous objects");
        
        CORBA::Object_var iogr = 
        m_iogrFactory->build_iogr(new_members,
                                  primary_location,
                                  ID,
                                  REP_ID,
                                  "my_domain",
                                  object_group_ref_version,
                                  no_fallBack);
        
        
        TEST_INFO("Count the number of profile into the iogr");
        CORBA::ULong nb_profile = m_iogrFactory->profile_count(iogr.in());
        TEST_CHECK(nb_profile == 2);

        TEST_INFO("Check if the profile n°1 contains the TAG_FT_GROUP");
        try
        {
            ::FT::TagFTGroupTaggedComponent * tag_group 
              = m_iogrFactory->decode_profile_with_group_tag (iogr.in(),0);
            delete tag_group;
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        TEST_INFO("Check if the profile n°2 contains the TAG_FT_GROUP");
        try
        {
            ::FT::TagFTGroupTaggedComponent * tag_group 
              = m_iogrFactory->decode_profile_with_group_tag (iogr.in(),1);
            delete tag_group;
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_INFO("Check if the profile n°3 doesn't exist");
        try
        {
            ::FT::TagFTGroupTaggedComponent * tag_group 
              = m_iogrFactory->decode_profile_with_group_tag (iogr.in(),2);
            delete tag_group;
            TEST_FAILED();
        }
        catch(const CdmwFT::ProfileNotFound&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }


        TEST_INFO("Check if no TAG_FT_GROUP is present into a simple ior");
        try
        {
            ::FT::TagFTGroupTaggedComponent * tag_group 
              = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),0);            
            delete tag_group;
            TEST_FAILED();
        }
        catch(const CdmwFT::TagNotFound&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }


        TEST_INFO("Check if no TAG_FT_PRIMARY is not present into the profile n°1");
        try
        {
            ::FT::TagFTPrimaryTaggedComponent tag_primary 
              = m_iogrFactory->decode_profile_with_primary_tag (iogr.in(),0);            
            TEST_FAILED();
        }
        catch(const CdmwFT::TagNotFound&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_INFO("Check if no TAG_FT_PRIMARY is not present into the profile n°2");
        try
        {
            ::FT::TagFTPrimaryTaggedComponent tag_primary 
              = m_iogrFactory->decode_profile_with_primary_tag (iogr.in(),1); 
            TEST_FAILED();
        }
        catch(const CdmwFT::TagNotFound&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }


        TEST_INFO("Build a new iogr with a primary member");
        primary_location.length(1);
        primary_location[0].id="HOST1";
        primary_location[0].kind="hostname";

        iogr = 
        m_iogrFactory->build_iogr(new_members,
                                  primary_location,
                                  ID,
                                  REP_ID,
                                  "my_domain",
                                  object_group_ref_version,
                                  no_fallBack);
        
        
        TEST_INFO("Count the number of profile into the iogr");
        nb_profile = m_iogrFactory->profile_count(iogr.in());
        TEST_CHECK(nb_profile == 2);

        TEST_INFO("Check if the profile n°1 contains the TAG_FT_GROUP");
        try
        {
            ::FT::TagFTGroupTaggedComponent * tag_group 
              = m_iogrFactory->decode_profile_with_group_tag (iogr.in(),0);
            delete tag_group;
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        TEST_INFO("Check if the profile n°2 contains the TAG_FT_GROUP");
        try
        {
            ::FT::TagFTGroupTaggedComponent * tag_group 
              = m_iogrFactory->decode_profile_with_group_tag (iogr.in(),1);
            delete tag_group;
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_INFO("Check if the profile n°3 doesn't exist");
        try
        {
            ::FT::TagFTGroupTaggedComponent * tag_group 
              = m_iogrFactory->decode_profile_with_group_tag (iogr.in(),2);
            delete tag_group;
            TEST_FAILED();
        }
        catch(const CdmwFT::ProfileNotFound&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }


        TEST_INFO("Check if no TAG_FT_GROUP is present into a simple ior");
        try
        {
            ::FT::TagFTGroupTaggedComponent * tag_group 
              = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),0);            
            delete tag_group;
            TEST_FAILED();
        }
        catch(const CdmwFT::TagNotFound&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }


        TEST_INFO("Check if no TAG_FT_PRIMARY is present into the profile n°1");
        try
        {
            ::FT::TagFTPrimaryTaggedComponent tag_primary 
              = m_iogrFactory->decode_profile_with_primary_tag (iogr.in(),0);            
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_INFO("Check if no TAG_FT_PRIMARY is not present into the profile n°2");
        try
        {
            ::FT::TagFTPrimaryTaggedComponent tag_primary 
              = m_iogrFactory->decode_profile_with_primary_tag (iogr.in(),1); 
            TEST_FAILED();
        }
        catch(const CdmwFT::TagNotFound&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }




        member_info2.the_reference = CORBA::Object::_duplicate(obj3.in());
        member_info2.the_location = loc2;
        
        TEST_INFO("Create MemberInfos");
        new_members.length(2);
        new_members[0] = member_info1;
        new_members[1] = member_info2;

        TEST_INFO("Try to create an iogr with different ior type");
        try
        {
            CORBA::Object_var iogr = 
            m_iogrFactory->build_iogr(new_members,
                                      primary_location,
                                      ID,
                                      REP_ID,
                                      "my_domain",
                                      object_group_ref_version,
                                      no_fallBack);
            TEST_FAILED();
        }        
        catch(const ::FT::ObjectNotCreated&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_INFO("Create MemberInfos");
        new_members.length(2);
        new_members[0] = member_info1;
        new_members[1] = member_info1;

        TEST_INFO("Try to create an iogr with duplicated ior");
        try
        {
            CORBA::Object_var iogr = 
            m_iogrFactory->build_iogr(new_members,
                                      primary_location,
                                      ID,
                                      REP_ID,
                                      "my_domain",
                                      object_group_ref_version,
                                      no_fallBack);
            TEST_FAILED();
        }        
        catch(const ::FT::ObjectNotCreated&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_INFO("set the client_id on the orb");
        try
        {
            Cdmw::FT::FTConfiguration::Set_client_id(m_orb.in(),"my_client_id");
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        TEST_INFO("get the client_id on the orb");
        try
        {
            std::string client_id;
            client_id = Cdmw::FT::FTConfiguration::Get_client_id(m_orb.in());
            std::cout<<"The client_id is :"<<client_id<<std::endl;
            TEST_CHECK(client_id == "my_client_id");
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_INFO("Get the FT_endpoint_selector_factory");
        try
        {
            Cdmw::FT::FTConfiguration::Get_FT_endpoint_selector_factory();
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_INFO("Try to activate the FT service");
        try
        {
            Cdmw::FT::FTConfiguration::Activate_FT_service(m_argc, m_argv);
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }


    } 
    catch (...)
    {
    }
    
}
