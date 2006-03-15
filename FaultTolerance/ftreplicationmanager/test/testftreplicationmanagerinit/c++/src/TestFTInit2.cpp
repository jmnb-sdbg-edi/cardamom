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


#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>

#include <testftreplicationmanagerinit/TestFTInit2.hpp>
#include <testftreplicationmanagerinit/TestUtils.hpp>
#include <testftreplicationmanagerinit/CdmwLocalFTManager.stub.hpp>
#include <Foundation/common/Assert.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>

#if CDMW_ORB_VDR == tao
#include <FaultTolerance/ftcommon/TAO_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif

namespace 
{
 
    const char* REP_ID_FAULT_DETECTOR = "IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector:1.0";

} // end anonymous namespace
namespace Cdmw 
{

namespace FT 
{

// constructor
TestFTInit2::TestFTInit2(CORBA::ORB_ptr                  orb,
                         PortableServer::POA_ptr         rootPOA,
                         ::FT::ObjectGroup_ptr           object_group_fd,
                         ::FT::ObjectGroup_ptr           object_group_h,
                         CdmwFT::ReplicationManager_ptr replication_manager,
                         const std::string&              name )
    : Testable(name),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_rootPOA(PortableServer::POA::_duplicate(rootPOA)),
      m_fd_group(CdmwFT::FaultManagement::FaultDetector::_narrow(object_group_fd)),
      m_h_group(CdmwFT::Test::HelloInterface::_narrow(object_group_h)),
      m_replication_manager(CdmwFT::ReplicationManager::_duplicate(replication_manager))
{ 
    CORBA::ULong object_group_ref_version = 0L;

    m_object_group_data.repository_id = REP_ID_FAULT_DETECTOR;
    m_object_group_data.object_group_ref_version = object_group_ref_version;
    m_object_group_data.object_group_ref = CORBA::Object::_nil();

    m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(m_orb.in(),
                                                  std::cout);
}

// destructor
TestFTInit2::~TestFTInit2()
{
}

// do_tests
void TestFTInit2::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK(21);
    int timescale = Cdmw::TestUtils::Testable::get_timescale();

    



  CORBA::ULong location_index1 = 1;
    try
    {
        TEST_INFO("Setting the primary...");
        if (CORBA::is_nil(m_h_group.in()))
        {
            TEST_FAILED();
        }
        ::FT::ObjectGroup_var object_group 
          = TestUtils::change_primary(m_orb.in(),
                                      m_h_group.in(), 
                                        m_replication_manager.in(), 
                                        location_index1);

        OsSupport::OS::sleep(timescale*1000);
        TEST_SUCCEED();

        m_h_group = CdmwFT::Test::HelloInterface::_narrow(object_group.in());
        if (CORBA::is_nil(object_group.in()))
        {
            TEST_FAILED();
        }
        
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }


    try
    {
        TEST_INFO("Calling hello on HelloInterface...");
        m_h_group->hello();
        TEST_INFO("No CORBA System Exception raised as expected!");
        TEST_FAILED();
    }
    catch (const CORBA::TRANSIENT& e)
    {
        TEST_INFO("Expected CORBA::TRANSIENT Exception raised" << e);
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("Unexpected CORBA System Exception raised" << e);
        TEST_FAILED();  
    }

    location_index1 = 2;
    
    unsigned int object_group_version=0;
    unsigned int object_group_version2=0;

     
    TEST_INFO("Check all profile in the IOGR");
    // Check the IOGR profiles 
    {
        CORBA::ULong nb_profile = m_iogrFactory->profile_count(m_h_group.in());
        // we don't check the last profile (it's the replication Manager)
        for (CORBA::ULong i = 0; i <  nb_profile - 1 ; i++)
        {
            try
            {
                TEST_INFO("Check the profile n°"<<i);
                ::FT::TagFTGroupTaggedComponent * tag_group 
                  = m_iogrFactory->decode_profile_with_group_tag (m_h_group.in(),i);
                object_group_version = tag_group->object_group_ref_version;
                delete tag_group;
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
        }
    }

    try
    {
        TEST_INFO("Setting a new primary...");
        if (CORBA::is_nil(m_h_group.in()))
        {
            TEST_FAILED();
        }
        ::FT::ObjectGroup_var object_group 
            = TestUtils::change_primary(m_orb.in(),
                                        m_h_group.in(), 
                                        m_replication_manager.in(), 
                                        location_index1);

        OsSupport::OS::sleep(timescale*1000);
        TEST_SUCCEED();

        m_h_group = CdmwFT::Test::HelloInterface::_narrow(object_group.in());
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised" << e);
        TEST_FAILED();
    }



    try
    {
        TEST_INFO("Calling hello on a odl HelloInterface reference...");
        m_h_group->hello();
        TEST_INFO("No CORBA System Exception raised as expected!");
        
        TEST_FAILED();  
    }
    catch (const CORBA::TRANSIENT& e)
    {
        TEST_INFO("Expected CORBA::TRANSIENT Exception raised" << e);
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("Unexpected CORBA System Exception raised" << e);
        TEST_FAILED();  
    }

    TEST_INFO("Check all profile in the IOGR");
    // Check the IOGR profiles 
    {
        CORBA::ULong nb_profile = m_iogrFactory->profile_count(m_h_group.in());
        // we don't check the last profile (it's the replication Manager)
        for (CORBA::ULong i = 0; i <  nb_profile - 1 ; i++)
        {
            try
            {
                TEST_INFO("Check the profile n°"<<i);
                ::FT::TagFTGroupTaggedComponent * tag_group 
                  = m_iogrFactory->decode_profile_with_group_tag (m_h_group.in(),i);
                object_group_version2 = tag_group->object_group_ref_version;
                delete tag_group;
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
        }
    }

    TEST_CHECK(object_group_version2 > object_group_version);
    
    TEST_INFO("get the fault_manager reference");
    CORBA::Object_var obj = m_orb->string_to_object("corbaloc::127.0.0.1:4555/fault_manager");
    ::CdmwFT::LocalFTManager_var local_fault_manager = ::CdmwFT::LocalFTManager::_narrow( obj.in() );
    
    if ( CORBA::is_nil( local_fault_manager.in() ) )
        std::cout<<"local_fault_manager is nil"<<std::endl;

    CdmwFT::FTManager_var ftManager = CdmwFT::FTManager::_narrow(obj.in());

    //CdmwFT::FaultManagement::FaultDetector_var fd = ftManager->get_fault_detector();
    CdmwFT::StateTransfer::DataStoreGroup_ptr dsgroup =
        CdmwFT::StateTransfer::DataStoreGroup::_nil();
    CdmwFT::LocationList list;
    CdmwFT::FTManagerObjects* ftmanagerobject = ftManager->get_FTManager_objects(Cdmw::OsSupport::OS::get_hostname().c_str(), dsgroup, list);
    CdmwFT::FaultManagement::FaultDetector_var fd = CdmwFT::FaultManagement::FaultDetector::_duplicate(ftmanagerobject->fault_detector.in());

    try
    {
        fd->is_alive();
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }
    

    
    
    TEST_INFO("call is alive on the fault detector with a object with a old version");
    ::CdmwFT::MemberInfo member_info_fault_detector;
    
    ::FT::Location loc;
    loc.length(1);
    loc[0].id =  Cdmw::OsSupport::OS::get_hostname().c_str();

    member_info_fault_detector.the_reference = CORBA::Object::_duplicate(fd.in());
    member_info_fault_detector.the_location = loc;

    ::CdmwFT::MemberInfos members;
    members.length(1);
    members[0L] = member_info_fault_detector;

    ::CdmwFT::Objects_var no_fallBack = new ::CdmwFT::Objects;     
    no_fallBack->length(0);   
    ::FT::ObjectGroupId id = 0xffffff03L;
    ::FT::ObjectGroupRefVersion version = 0;

    CORBA::Object_var object = m_iogrFactory->build_iogr(members,
                                                         loc,
                                                         id,
                                                         REP_ID_FAULT_DETECTOR,
                                                         "CDMW_FaultToleranceDomain",
                                                         version,
                                                         no_fallBack.in());
    
    CdmwFT::FaultManagement::FaultDetector_var fdog = CdmwFT::FaultManagement::FaultDetector::_narrow(object.in());
    CdmwFT::FaultManagement::FaultDetector_var fdog_good = CdmwFT::FaultManagement::FaultDetector::_duplicate(fdog.in());
    


    TEST_INFO("Check all profile in the IOGR");
    // Check the IOGR profiles 
    {
        CORBA::ULong nb_profile = m_iogrFactory->profile_count(fdog.in());
        for (CORBA::ULong i = 0; i <  nb_profile ; i++)
        {
            try
            {
                TEST_INFO("Check the profile n°"<<i);
                ::FT::TagFTGroupTaggedComponent * tag_group 
                  = m_iogrFactory->decode_profile_with_group_tag (fdog.in(),i);
                object_group_version = tag_group->object_group_ref_version;
                delete tag_group;
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
        }
    }

    TEST_INFO("Check the good version of the object : "<<object_group_version);
    TEST_CHECK(object_group_version == 1);

    try
    {
        fdog->is_alive();
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }
    
    TEST_INFO("Check all profile in the IOGR");
    // Check the IOGR profiles 
    {
        CORBA::ULong nb_profile = m_iogrFactory->profile_count(fdog.in());
        for (CORBA::ULong i = 0; i <  nb_profile  ; i++)
        {
            try
            {
                TEST_INFO("Check the profile n°"<<i);
                ::FT::TagFTGroupTaggedComponent * tag_group 
                  = m_iogrFactory->decode_profile_with_group_tag (fdog.in(),i);
                object_group_version = tag_group->object_group_ref_version;
                delete tag_group;
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
        }
    }

    TEST_INFO("Check the version of the object forwared: "<<object_group_version);
    TEST_CHECK(object_group_version == 2);

    version = 10;
    
    object = m_iogrFactory->build_iogr(members,
                                       loc,
                                       id,
                                       REP_ID_FAULT_DETECTOR,
                                       "CDMW_FaultToleranceDomain",
                                       version,
                                       no_fallBack.in());

    fdog = CdmwFT::FaultManagement::FaultDetector::_narrow(object.in());


    TEST_INFO("Check all profile in the IOGR");
    // Check the IOGR profiles 
    {
        CORBA::ULong nb_profile = m_iogrFactory->profile_count(fdog.in());
        for (CORBA::ULong i = 0; i <  nb_profile ; i++)
        {
            try
            {
                TEST_INFO("Check the profile n°"<<i);
                ::FT::TagFTGroupTaggedComponent * tag_group 
                  = m_iogrFactory->decode_profile_with_group_tag (fdog.in(),i);
                object_group_version = tag_group->object_group_ref_version;
                delete tag_group;
                TEST_SUCCEED();
            }
            catch(...)
            {
                TEST_FAILED();
            }
        }
    }
    TEST_INFO("Check the version of the object: "<<object_group_version);

    TEST_CHECK(object_group_version == 11);
    TEST_INFO("call is_alive on the object with a version higher of the normal version");
    try
    {
        fdog->is_alive();
        TEST_FAILED();
    }
    catch(const CORBA::INV_OBJREF&)
    {
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }


    TEST_INFO("deactivate the ftManager");
    local_fault_manager->deactivate();


    try
    {
        fdog_good->is_alive();
        TEST_FAILED();
    }
    catch (const CORBA::NO_RESPONSE& e)
    {
        TEST_INFO("Expected CORBA::NO_RESPONCE Exception raised" << e);
        TEST_SUCCEED();  
    }
   catch (const CORBA::TRANSIENT& e)
   {
       TEST_INFO("Expected CORBA::TRANSIENT Exception raised" << e);
       TEST_SUCCEED();  
   }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("Unexpected CORBA System Exception raised" << e);
        TEST_FAILED();  
    }
    
}


} // end namespace FT

} // end namespace Cdmw

