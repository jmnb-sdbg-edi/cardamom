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
#include <Foundation/orbsupport/OrbSupport.hpp>

#include <testftinit/TestFTInit.hpp>
#include <testftinit/TestUtils.hpp>

#include <testftinit/TestHello.stub.hpp>


#include <fstream>

// BEGIN TAO TESTS


#include <vector>
#include <algorithm>
#include <iomanip>
#include <numeric>

#include <testftinit/TestHello_impl.hpp>
// The existing approach uses the standard TAO FT mechanism.
#include <tao/ORB_Core.h>
#define private public
#include <orbsvcs/orbsvcs/FaultTolerance/FT_ClientRequest_Interceptor.h>
#undef private
namespace 
{

    // TODO: Move this to Foundation CSCI
    TimeBase::TimeT 
    convert_TimeVal_to_TimeT (const ::Cdmw::OsSupport::OS::Timeval & tv)
    {
        // FIXME: TAO Does not seem use DCE time timebase for the expiration_time
        // but uses rather Posix time!
        // TimeVal is in sec/microsec from 1st Jan 1970 (This is different in Windows!!!)
        //
        // Time difference in 100 nanoseconds between DCE and POSIX
        // is 141427 days.
        //        static const TimeBase::TimeT DELTA_POSIX_DCE = 
        //            static_cast<TimeBase::TimeT>(141427) * 
        //            static_cast<TimeBase::TimeT>(86400) *
        //            static_cast<TimeBase::TimeT>(10000000);
        //        TimeBase::TimeT result = 
        //            DELTA_POSIX_DCE +
        //            static_cast<TimeBase::TimeT>(tv.seconds ) * 10000000 +
        //            static_cast<TimeBase::TimeT>(tv.microseconds) * 10 ;

        TimeBase::TimeT result = 
            static_cast<TimeBase::TimeT>(tv.seconds ) * 10000000 +
            static_cast<TimeBase::TimeT>(tv.microseconds) * 10 ;
        return result;
    }

    //
    // Return the current time in DCE std: 100 ns from 15/10/1582
    //
    TimeBase::TimeT
    get_DCE_time()
        throw( ::Cdmw::InternalErrorException )
    {
        // First get current time in sec/microsec from 1st Jan 1970
        return convert_TimeVal_to_TimeT(::Cdmw::OsSupport::OS::get_time());
    }

} // end of anonymous namespace

// END TAO TESTS

namespace Cdmw 
{

namespace FT 
{

// constructor
TestFTInit::TestFTInit(CORBA::ORB_ptr                  orb,
                       PortableServer::POA_ptr         rootPOA,
                       ::FT::ObjectGroup_ptr           object_group,
                       CdmwFT::ReplicationManager_ptr replication_manager,
                       const std::string&              name ,
                       long                            loop_for_exporation_time)
: Testable(name),
  m_orb(CORBA::ORB::_duplicate(orb)),
  m_rootPOA(PortableServer::POA::_duplicate(rootPOA)),
  m_hello_group(CdmwFT::Test::HelloInterface::_narrow(object_group)),
  m_replication_manager(CdmwFT::ReplicationManager::_duplicate(replication_manager)),
  m_loop_for_exporation_time(loop_for_exporation_time)
{    
    
}

// destructor
TestFTInit::~TestFTInit()
{
}

// do_tests
void TestFTInit::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK(45);
    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    

    try 
    {
        Cdmw::OsSupport::OS::unlink("hello.stt");
        Cdmw::OsSupport::OS::unlink("hello.act");
        Cdmw::OsSupport::OS::unlink("hello.dct");
    }
    catch(...)
    {
    }
    




    try
    {
        TEST_INFO("Calling hello on HelloInterface...");
        std::string hello_group_str = m_orb->object_to_string(m_hello_group.in());
        TEST_INFO("hello_group_str = " << hello_group_str);
        m_hello_group->hello();
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("Unexpected CORBA System Exception raised " << e);
        TEST_FAILED();  
    }

    TEST_INFO("try to bind an object");
    try
    {
        Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc(m_orb.in(), "Hello", m_hello_group.in());
        TEST_SUCCEED();
    }
    catch (const CORBA::Exception& e )
    {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
    }


    TEST_INFO("try to get an object group with a corbaloc");
    try
    {
        
        CORBA::Object_var obj = m_orb->string_to_object("corbaloc::localhost:45678/Hello");
        std::cout<<m_orb->object_to_string(obj.in())<<std::endl;

        CdmwFT::Test::HelloInterface_var hello =  CdmwFT::Test::HelloInterface::_narrow(obj.in());
        
        if ( CORBA::is_nil( hello.in() ) )
            std::cout<<"Hello is nil"<<std::endl;
        hello->hello();
        std::cout<<m_orb->object_to_string(hello.in())<<std::endl;
        TEST_SUCCEED();
    }
    catch (const CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;
        TEST_FAILED();
    }

    int how_many = 3;
    for (int i = 1; i<9; i++)
    {
        try
        {
            
            switch (i) 
            {
                case 1:
                {
                    TEST_INFO("throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_NO)");
                    break;
                }
                case 2:
                {
                    TEST_INFO("throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_MAYBE)");
                    break;
                }
                case 3:
                {
                    TEST_INFO("throw CORBA::TRANSIENT(minor_code, CORBA::COMPLETED_NO)");
                    break;
                }
                case 4:
                {
                    TEST_INFO("throw CORBA::TRANSIENT(minor_code, CORBA::COMPLETED_MAYBE)");
                    break;
                }
                case 5:
                {
                    TEST_INFO("throw CORBA::NO_RESPONSE(minor_code, CORBA::COMPLETED_NO)");
                    break;
                }
                case 6:
                {
                    TEST_INFO("throw CORBA::NO_RESPONSE(minor_code, CORBA::COMPLETED_MAYBE)");
                    break;
                }
                case 7:
                {
                    TEST_INFO("throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_NO)");
                    break;
                }
                case 8:
                {
                    TEST_INFO("throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_MAYBE)");
                    break;
                }
            }
            
            
            if (m_hello_group->send_exception(i, how_many) == how_many)
            {
                TEST_SUCCEED();  
            } else {
                TEST_FAILED();
            }   
        }
        catch (const CORBA::SystemException& e)
        {
            TEST_FAILED();
        }   
    }
    
    TEST_INFO("Check the request duration time");


    TEST_INFO("Obtain the ORBs PolicyManager ");
    CORBA::Object_var objPM = m_orb->resolve_initial_references("ORBPolicyManager");
    
    CORBA::PolicyManager_var policy_manager = CORBA::PolicyManager::_narrow(objPM.in());

    CORBA::PolicyTypeSeq policy_types(1);
    policy_types.length(1);
    policy_types[0] = ::FT::REQUEST_DURATION_POLICY;

    CORBA::PolicyList_var set_policies 
    = policy_manager->get_policy_overrides(policy_types);
    
    ::FT::RequestDurationPolicy_var durationPolicy = ::FT::RequestDurationPolicy::_narrow((set_policies->operator[](0)).in());
#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
    TimeBase::TimeT duration_time = durationPolicy->request_duration_policy_value();
#else
    TimeBase::TimeT duration_time = durationPolicy->request_duration_value();
#endif

    
// BEGIN TAO TESTS
    {
        char buff[255];
        const std::string SEP
            ("+-------+-------------------+-------------------+------------+------------+------------+-------------------+");

        TAO_ORB_Core* orb_core = m_orb->orb_core();
        TEST_INFO( "number of interceptors = " << orb_core->client_request_interceptors().size());
        PortableInterceptor::ClientRequestInterceptor* client_interceptor
            = orb_core->client_request_interceptors()[0].in();
        TAO::FT_ClientRequest_Interceptor* ft_client_interceptor
            = dynamic_cast<TAO::FT_ClientRequest_Interceptor*>(client_interceptor);
        TEST_INFO( "interceptor = " << ft_client_interceptor);

        std::vector<TimeBase::TimeT> DeltaVect(::Cdmw::Test::HelloInterface_impl::MaxCycles);
        std::vector<float> PurcentageVect(::Cdmw::Test::HelloInterface_impl::MaxCycles);
        
        
        std::ofstream result_file;
        result_file.open("./results_file",std::ios::out);
        result_file << "===================================\n"
                    << "Results of " << ::Cdmw::Test::HelloInterface_impl::MaxCycles << " tests \n"
                    << "===================================\n"
                    << "Request duration time : "<<duration_time<< std::endl;
        result_file << "\n" << SEP << "\n"
                    << "| CYCLE | START             | END               | DELTA      | PURCENTAGE | RETENTION  | EXPIRATION        | \n"
                    << "|       |                   |                   |            |            | ID         | TIME              | \n" 
                    << SEP << std::endl;


        TimeBase::TimeT time_start__, time_end__;
        for (int i=0; i < m_loop_for_exporation_time; ++i) {
            ::TAO::FT_TSS * tss__ = ft_client_interceptor->tss_resources();;
            time_start__ = get_DCE_time();

            add_nbOfRequestedTestOK(2);
            
            try {
                m_hello_group->send_TRANSIENT_exception(i);
                TEST_FAILED();
            } catch(const CORBA::TRANSIENT& ex) {
                TEST_SUCCEED();
            }
            time_end__ = get_DCE_time();

            tss__ = ft_client_interceptor->tss_resources();;
            tss__->clean_flag_=true;
        
            TimeBase::TimeT delta__=(time_end__ - time_start__);
            DeltaVect[i]=delta__;
            PurcentageVect[i]=(static_cast<float>(delta__-duration_time)/static_cast<float>(duration_time))*100.0;

            sprintf(buff,"| #%.4d | %17lld | %17lld | %.10lld | %6.4f%%    | %6d     | %17lld | %s",
                    i, time_start__, time_end__, DeltaVect[i], PurcentageVect[i],
                    tss__->retention_id_, tss__->expiration_time_,
                    (PurcentageVect[i] > 1.0)?"(*)":"");
            result_file << buff << "\n" << SEP << std::endl;  
            TEST_CHECK(((PurcentageVect[i] > 10.0)?false:true));
        }
    

        TimeBase::TimeT MinTime = *(std::min_element(DeltaVect.begin(),DeltaVect.end()));
        TimeBase::TimeT MaxTime = *(std::max_element(DeltaVect.begin(),DeltaVect.end()));
        
        
        result_file << "===================================\n"
                    << "Summary of " << ::Cdmw::Test::HelloInterface_impl::MaxCycles  << " tests \n"
                    << "===================================\n"
                    << "Request duration time : "<<duration_time<< std::endl;
        result_file << "Min= " << std::setw(8) << MinTime << " Max= " << MaxTime << std::endl;

        
    }
// END TAO TESTS


 //    ::Cdmw::OsSupport::OS::Timeval time_start = ::Cdmw::OsSupport::OS::get_time();

//     try
//     {
//         m_hello_group->send_exception(3,0);
//         TEST_FAILED();
//     }
//     catch(const CORBA::TRANSIENT& ex)
//     {
//         TEST_SUCCEED();
//     }
    
//     ::Cdmw::OsSupport::OS::Timeval time_end = ::Cdmw::OsSupport::OS::get_time();


//     TimeBase::TimeT start = time_start.seconds * 10000000+ time_start.microseconds * 10;
//     TimeBase::TimeT end = time_end.seconds * 10000000+ time_end.microseconds * 10;
       
            
//     // the test is OK if the end-start is contained between the duration_time and the duration_time + 10% 
//     TEST_INFO("the test is OK if the end-start is contained between the duration_time and the duration_time + 10% ");
//     std::cout<<"Request duration time : "<<duration_time<<std::endl;
//     std::cout<<"time measured is "<<end - start<<std::endl;
//     if (( duration_time < (end - start) ) && ((duration_time + 0.1 * duration_time) > (end - start)))
//         TEST_SUCCEED();
//     else
//         TEST_FAILED();



    try
    {
        TEST_INFO("Updating the location managers with an old version...");
        TestUtils::location_manager_update_group_view(m_orb.in(),
                                                      m_hello_group.in(), 
                                                      m_replication_manager.in());

        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("Unexpected CORBA System Exception raised " << e);
        TEST_FAILED();  
    }
    // NOTA: From now on first joiner is primary. So the tests bellow relying on a group with no primary are
    //       not relevant any more.

//     try
//     {
//         TEST_INFO("Calling hello on HelloInterface while no primary elected...");
//         m_hello_group->hello();
//         TEST_INFO("No CORBA System Exception raised as expected!");
//         TEST_FAILED();
//     }
//     catch (const CORBA::TRANSIENT& e)
//     {
//         TEST_INFO("Expected CORBA::TRANSIENT Exception raised " << e);
//         TEST_SUCCEED();  
//     }
//     catch (const CORBA::SystemException& e)
//     {
//         TEST_INFO("Unexpected CORBA System Exception raised " << e);
//         TEST_FAILED();  
//     }

//     TEST_INFO("Check no primary has been elected...");
//     std::ifstream is("hello.act");
//     if (is.good())
//     {
//         TEST_FAILED();
//     }
//     else
//     {
//         TEST_SUCCEED();  
//     }

    try
     {
         TEST_INFO("Calling hello on primary HelloInterface...");
         m_hello_group->hello();
         TEST_FAILED();
     }
     catch (const CORBA::TRANSIENT& e)
     {
         TEST_INFO("Expected CORBA::TRANSIENT Exception raised " << e);
         TEST_SUCCEED();  
     }
     catch (const CORBA::SystemException& e)
     {
         TEST_INFO("Unexpected CORBA System Exception raised " << e);
         TEST_FAILED();  
     }

//      TEST_INFO("Check primary has been elected...");
//      std::ifstream is("hello.act");
//      if (is.good())
//      {
//          TEST_SUCCEED();  
//      }
//      else
//      {
//          TEST_FAILED();
//      }

     CORBA::ULong location_index = 1;
    try
    {
        TEST_INFO("Setting the primary...");
        ::FT::ObjectGroup_var object_group 
          = TestUtils::change_primary(m_orb.in(), m_hello_group.in(), 
                                        m_replication_manager.in(), 
                                        location_index);

        OsSupport::OS::sleep(timescale*1000);

        //std::string object_group_str = m_orb->object_to_string(object_group.in());
        //TEST_INFO("New object_group_str = " << object_group_str);

        if (!CORBA::is_nil(object_group.in()))
        {
            m_hello_group = CdmwFT::Test::HelloInterface::_narrow(object_group.in());

            if (!CORBA::is_nil(m_hello_group.in()))
            {
                std::ifstream is("hello.act");
                if (is.good())
                {
                    TEST_FAILED();  
                }
                else
                {
                    // no activation handler registered
                    TEST_SUCCEED();  
                }
            }
            else
            {
                TEST_FAILED();
            }
        }
        else 
        {
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }


//     try
//     {
//         TEST_INFO("Updating the location managers with an old version...");
//         TestUtils::location_manager_update_group_view(m_orb.in(),
//                                                       m_hello_group.in(), 
//                                                       m_replication_manager.in(),
//                                                       true);

//         TEST_SUCCEED();  
//     }
//     catch (const CORBA::SystemException& e)
//     {
//         TEST_INFO("Unexpected CORBA System Exception raised " << e);
//         TEST_FAILED();  
//     }

    try
    {
        TEST_INFO("Calling init on HelloInterface...");

        for (int i=0; i<3; i++)
        {
            CdmwFT::Test::HelloInterface_var hello =
                TestUtils::get_hello_ref_from_file(m_orb.in(), 
                                                   TestUtils::M_files[i]);
            
            hello->init(m_hello_group.in());
            TEST_SUCCEED();  
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

        // Test FT Current operation
    TEST_INFO("Test FTCurrent operations outside FT context: shall raise NoContext");
    try
    {
        TEST_INFO("Test FTCurrent::get_ft_domain_id");

        CdmwFT::Test::HelloInterface_var hello =
            TestUtils::get_hello_ref_from_file(m_orb.in(), 
                                                   TestUtils::M_files[0]);
            
        ::FT::FTDomainId ft_domain_id = hello->get_ft_domain_id();
        TEST_INFO(ft_domain_id);
        TEST_FAILED();
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Test FTCurrent::get_object_group_id");

        CdmwFT::Test::HelloInterface_var hello =
            TestUtils::get_hello_ref_from_file(m_orb.in(), 
                                                   TestUtils::M_files[0]);
            
        ::FT::ObjectGroupId group_id = hello->get_object_group_id();
        TEST_INFO(group_id);
        TEST_FAILED();
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Test FTCurrent::get_object_group_ref_version");

        CdmwFT::Test::HelloInterface_var hello =
            TestUtils::get_hello_ref_from_file(m_orb.in(), 
                                                   TestUtils::M_files[0]);
            
        ::FT::ObjectGroupRefVersion group_ref_version = hello->get_object_group_ref_version();
        TEST_INFO(group_ref_version);
        TEST_FAILED();
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Test FTCurrent::get_client_id");

        CdmwFT::Test::HelloInterface_var hello =
            TestUtils::get_hello_ref_from_file(m_orb.in(), 
                                                   TestUtils::M_files[0]);
            
        std::string client_id = hello->get_client_id();
        TEST_INFO(client_id);
        TEST_FAILED();
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Test FTCurrent::get_retention_id");

        CdmwFT::Test::HelloInterface_var hello =
            TestUtils::get_hello_ref_from_file(m_orb.in(), 
                                                   TestUtils::M_files[0]);
            
        CORBA::Long retention_id = hello->get_retention_id();
        TEST_INFO(retention_id);
        TEST_FAILED();
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Test FTCurrent::get_expiration_time");

        CdmwFT::Test::HelloInterface_var hello =
            TestUtils::get_hello_ref_from_file(m_orb.in(), 
                                                   TestUtils::M_files[0]);
            
        TimeBase::TimeT expiration_time = hello->get_expiration_time();
        TEST_INFO(expiration_time);
        TEST_FAILED();
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    TEST_INFO("Test FTCurrent operations in FT context");
    try
    {
        TEST_INFO("Test FTCurrent::get_ft_domain_id");

        std::string ft_domain_id = m_hello_group->get_ft_domain_id();

        std::string rm_ft_domain_id = m_replication_manager->get_ft_domain_id();

        if (ft_domain_id == rm_ft_domain_id)
        {
           TEST_SUCCEED();  
        }
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Test FTCurrent::get_object_group_id");

        ::FT::ObjectGroupId group_id = m_hello_group->get_object_group_id();
        TEST_INFO(group_id);

        ::FT::ObjectGroupId rm_group_id = m_replication_manager->get_object_group_id(m_hello_group.in());

        if (group_id == rm_group_id)
        {
           TEST_SUCCEED();  
        }
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Test FTCurrent::get_object_group_ref_version");

        ::FT::ObjectGroupRefVersion group_ref_version = m_hello_group->get_object_group_ref_version();
        TEST_INFO(group_ref_version);

        ::FT::ObjectGroupRefVersion rm_group_ref_version = 
           m_replication_manager->get_object_group_version_from_ref(m_hello_group.in());

        if (group_ref_version == rm_group_ref_version)
        {
           TEST_SUCCEED();  
        }
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Test FTCurrent::get_client_id");
        std::string client_id = m_hello_group->get_client_id();
        TEST_INFO(client_id);
        TEST_SUCCEED();
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Test FTCurrent::get_retention_id");

        CORBA::Long retention_id = m_hello_group->get_retention_id();
        TEST_INFO(retention_id);
        TEST_SUCCEED();  
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Test FTCurrent::get_expiration_time");

        TimeBase::TimeT expiration_time = m_hello_group->get_expiration_time();
        TEST_INFO(expiration_time);
        TEST_SUCCEED();  
    }
    catch (const CdmwFT::Test::HelloInterface::NoContext& )
    {
        TEST_INFO("CdmwFT::Current::NoContext raised");
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface...");
        m_hello_group->hello();

        OsSupport::OS::sleep(timescale*1000);

        std::ifstream is("hello.stt");
        if (is.good())
        {
            std::string hello_location;
            is >> hello_location;
            // check host activated
            if (hello_location == TestUtils::M_locations[location_index])
            {
                TEST_SUCCEED();  
            }
            else
            {
                std::cerr << "Invalid primary elected" << std::endl;
                TEST_FAILED();
            }
            
            is.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: hello.stt" << std::endl;
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    CORBA::ULong old_location_index = location_index;
    try
    {
        TEST_INFO("Changing the primary...");
        location_index = 2;
        ::FT::ObjectGroup_var object_group 
            = TestUtils::change_primary(m_orb.in(), m_hello_group.in(), 
                                        m_replication_manager.in(), 
                                        location_index);

        OsSupport::OS::sleep(timescale*1000);

        //std::string object_group_str = m_orb->object_to_string(object_group.in());
        //TEST_INFO("New object_group_str = " << object_group_str);

        if (!CORBA::is_nil(object_group.in()))
        {
            m_hello_group = CdmwFT::Test::HelloInterface::_narrow(object_group.in());

            if (!CORBA::is_nil(m_hello_group.in()))
            {
                std::ifstream is_dct("hello.dct");
                if (is_dct.good())
                {
                    std::string host_deactivated;
                    is_dct >> host_deactivated;
                    
                    // check host activated
                    if (host_deactivated == TestUtils::M_locations[old_location_index])
                    {
                        TEST_SUCCEED();  
                    }
                    else
                    {
                        std::cerr << "Invalid primary deactivated" << std::endl;
                        TEST_FAILED();
                    }
                    
                    is_dct.close();
                }
                else
                {
                    std::cerr << "Error opening reference filename: hello.dct" << std::endl;
                    TEST_FAILED();
                }
                
                std::ifstream is_act("hello.act");
                if (is_act.good())
                {
                    std::string host_activated;
                    is_act >> host_activated;
                    
                    // check host activated
                    if (host_activated == TestUtils::M_locations[location_index])
                    {
                        TEST_SUCCEED();  
                    }
                    else
                    {
                        std::cerr << "Invalid primary elected" << std::endl;
                        TEST_FAILED();
                    }
                    
                    is_act.close();
                }
                else
                {
                    std::cerr << "Error opening reference filename: hello.act" << std::endl;
                    TEST_FAILED();
                }
            }
            else
            {
                TEST_FAILED();
            }
        }
        else 
        {
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface...");
        m_hello_group->hello();
        OsSupport::OS::sleep(timescale*1000);

        std::ifstream is("hello.stt");
        if (is.good())
        {
            std::string hello_location;
            is >> hello_location;

            // check host activated
            if (hello_location == TestUtils::M_locations[location_index])
            {
                TEST_SUCCEED();  
            }
            else
            {
                std::cerr << "Invalid primary elected" << std::endl;
                TEST_FAILED();
            }
            
            is.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: hello.act" << std::endl;
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Changing the primary again...");
        old_location_index = location_index;
        location_index = 1;
        TestUtils::change_primary(m_orb.in(), m_hello_group.in(), 
                                  m_replication_manager.in(), 
                                  location_index);

        OsSupport::OS::sleep(timescale*1000);

        std::ifstream is_dct("hello.dct");
        if (is_dct.good())
        {
            std::string host_deactivated;
            is_dct >> host_deactivated;
            
            // check host activated
            if (host_deactivated == TestUtils::M_locations[old_location_index])
            {
                TEST_SUCCEED();  
            }
            else
            {
                std::cerr << "Invalid primary deactivated" << std::endl;
                TEST_FAILED();
            }
            
            is_dct.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: hello.dct" << std::endl;
            TEST_FAILED();
        }
        
        std::ifstream is_act("hello.act");
        if (is_act.good())
        {
            std::string host_activated;
            is_act >> host_activated;
            
            // check host activated
            if (host_activated == TestUtils::M_locations[location_index])
            {
                TEST_SUCCEED();  
            }
            else
            {
                std::cerr << "Invalid primary elected" << std::endl;
                TEST_FAILED();
            }
            
            is_act.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: hello.act" << std::endl;
            TEST_FAILED();
        }
        
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface with the old group ref...");
        m_hello_group->hello();

        OsSupport::OS::sleep(timescale*1000);

        std::ifstream is("hello.stt");
        if (is.good())
        {
            std::string hello_location;
            is >> hello_location;

            // check host activated
            if (hello_location == TestUtils::M_locations[location_index])
            {
                TEST_SUCCEED();  
            }
            else
            {
                std::cerr << "Invalid primary elected" << std::endl;
                TEST_FAILED();
            }
            
            is.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: hello.stt" << std::endl;
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface with the old group ref...");
        m_hello_group->hello();
        OsSupport::OS::sleep(timescale*1000);

        std::ifstream is("hello.stt");
        if (is.good())
        {
            std::string hello_location;
            is >> hello_location;

            // check host activated
            if (hello_location == TestUtils::M_locations[location_index])
            {
                TEST_SUCCEED();  
            }
            else
            {
                std::cerr << "Invalid primary elected" << std::endl;
                TEST_FAILED();
            }
            
            is.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: hello.stt" << std::endl;
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    CdmwFT::Test::HelloInterface_var ext_hello_group;
    try
    {
        TEST_INFO("Create a new version outside CDMW context...");
        TEST_INFO("Neither the Replication Manager nor the Location Manager is aware!");

        ::FT::ObjectGroup_var object_group 
            = TestUtils::update_group_with_primary_outside_cdmw_ft
                          (m_orb.in(), 
                           m_hello_group.in(), 
                           m_replication_manager.in(), 
                           2);

        if (!CORBA::is_nil(object_group.in()))
        {
            ext_hello_group = CdmwFT::Test::HelloInterface::_narrow(object_group.in());

            if (!CORBA::is_nil(ext_hello_group.in()))
            {
                TEST_SUCCEED();  
            }
            else
            {
                TEST_FAILED();
            }
        }
        else 
        {
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface with the new external group ref...");
        ext_hello_group->hello();
        TEST_INFO("No CORBA System Exception raised as expected!");
        TEST_FAILED();
    }
    catch (const CORBA::INV_OBJREF& e)
    {
        TEST_INFO("Expected CORBA::INV_OBJREF Exception raised");
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("Unexpected CORBA System Exception raised " << e);
        TEST_FAILED();  
    }

    try
    {
        TEST_INFO("Changing the primary...");
        old_location_index = location_index;
        location_index = 2;
        ::FT::ObjectGroup_var object_group 
            = TestUtils::change_primary(m_orb.in(), m_hello_group.in(), 
                                        m_replication_manager.in(), 
                                        location_index);

        OsSupport::OS::sleep(timescale*1000);

        if (!CORBA::is_nil(object_group.in()))
        {
            m_hello_group = CdmwFT::Test::HelloInterface::_narrow(object_group.in());

            if (!CORBA::is_nil(m_hello_group.in()))
            {
                std::ifstream is_dct("hello.dct");
                if (is_dct.good())
                {
                    std::string host_deactivated;
                    is_dct >> host_deactivated;
                    
                    // check host activated
                    if (host_deactivated == TestUtils::M_locations[old_location_index])
                    {
                        TEST_SUCCEED();  
                    }
                    else
                    {
                        std::cerr << "Invalid primary deactivated" << std::endl;
                        TEST_FAILED();
                    }
                    
                    is_dct.close();
                }
                else
                {
                    std::cerr << "Error opening reference filename: hello.dct" << std::endl;
                    TEST_FAILED();
                }
                
                std::ifstream is_act("hello.act");
                if (is_act.good())
                {
                    std::string host_activated;
                    is_act >> host_activated;
                    
                    // check host activated
                    if (host_activated == TestUtils::M_locations[location_index])
                    {
                        TEST_SUCCEED();  
                    }
                    else
                    {
                        std::cerr << "Invalid primary elected" << std::endl;
                        TEST_FAILED();
                    }
                    
                    is_act.close();
                }
                else
                {
                    std::cerr << "Error opening reference filename: hello.act" << std::endl;
                    TEST_FAILED();
                }
            }
            else
            {
                TEST_FAILED();
            }
        }
        else 
        {
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Changing the primary again...");
        old_location_index = location_index;
        location_index = 1;
        TestUtils::change_primary(m_orb.in(), m_hello_group.in(), 
                                  m_replication_manager.in(), 
                                  location_index);

        OsSupport::OS::sleep(timescale*1000);

        std::ifstream is_dct("hello.dct");
        if (is_dct.good())
        {
            std::string host_deactivated;
            is_dct >> host_deactivated;
            
            // check host activated
            if (host_deactivated == TestUtils::M_locations[old_location_index])
            {
                TEST_SUCCEED();  
            }
            else
            {
                std::cerr << "Invalid primary deactivated" << std::endl;
                TEST_FAILED();
            }
            
            is_dct.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: hello.dct" << std::endl;
            TEST_FAILED();
        }
        
        std::ifstream is_act("hello.act");
        if (is_act.good())
        {
            std::string host_activated;
            is_act >> host_activated;
            
            // check host activated
            if (host_activated == TestUtils::M_locations[location_index])
            {
                TEST_SUCCEED();  
            }
            else
            {
                std::cerr << "Invalid primary elected" << std::endl;
                TEST_FAILED();
            }
            
            is_act.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: hello.act" << std::endl;
            TEST_FAILED();
        }
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Updating the location managers with an old version...");
        TestUtils::location_manager_update_group_view(m_orb.in(),
                                                      m_hello_group.in(), 
                                                      m_replication_manager.in());

        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("CORBA System Exception raised " << e);
        TEST_FAILED();
    }

    try
    {
        TEST_INFO("Calling hello on HelloInterface...");
        TEST_INFO("A check with the Replication Manager shall be done.");
        TEST_INFO("First the version from the Replication Manager is greater, ");
        TEST_INFO("a location forward is done!");
        TEST_INFO("Then, the version from the Replication Manager is equal ");
        TEST_INFO("to the request group version, a TRANSIENT exception is raised ");
        TEST_INFO("because the location manager has not been updated and then, ");
        TEST_INFO("the new primary or the old one may be not activated/desactivated");
        ext_hello_group->hello();
        TEST_INFO("No CORBA System Exception raised as expected!");
        TEST_FAILED();
    }
    catch (const CORBA::TRANSIENT& e)
    {
        TEST_INFO("Expected CORBA::TRANSIENT Exception raised");
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("Unexpected CORBA System Exception raised " << e);
        TEST_FAILED();  
    }

    Cdmw::OsSupport::OS::unlink("hello.stt");
    Cdmw::OsSupport::OS::unlink("hello.act");
    Cdmw::OsSupport::OS::unlink("hello.dct");

}


} // end namespace FT

} // end namespace Cdmw

