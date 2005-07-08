/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
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
 * =========================================================================== */

#include <Foundation/orbsupport/CORBA.hpp>

#include <faulttoleranceclient1/TestFTInit.hpp>
#include <faulttoleranceclient1/TestUtils.hpp>
#include <faulttoleranceclient1/SIM_consumer.hpp>

#include <faulttoleranceclient1/TestHello.stub.hpp>
#include "Foundation/ossupport/OS.hpp"
#include <Repository/naminginterface/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include <FaultTolerance/idllib/FT.stub.hpp>
#include <FaultTolerance/ftcommon/FactoryInfos.hpp>
#include <FaultTolerance/ftcommon/FTConfiguration.hpp>

namespace Cdmw 
{

namespace FT 
{

// constructor
TestFTInit::TestFTInit(CORBA::ORB_ptr                  orb,
                       PortableServer::POA_ptr         rootPOA,
                       CdmwFT::ReplicationManager_ptr replication_manager,
                       const std::string&              name ,
                       std::string host_name1,
                       std::string host_name2,
                       std::string host_name3)
    : Testable(name),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_rootPOA(PortableServer::POA::_duplicate(rootPOA)),
      m_replication_manager(CdmwFT::ReplicationManager::_duplicate(replication_manager)),
      m_host_name1(host_name1),
      m_host_name2(host_name2),
      m_host_name3(host_name3)
{    
    
}

// destructor
TestFTInit::~TestFTInit()
{
}

// do_tests
void TestFTInit::do_tests()
{

    set_nbOfRequestedTestOK (132);
    const char * rep_id_HelloInterface1 = "IDL:thalesgroup.com/Cdmw/Test/HelloInterface1:1.0";
    const char * rep_id_HelloInterface2 = "IDL:thalesgroup.com/Cdmw/Test/HelloInterface2:1.0";
    const char * rep_id_HelloInterface3 = "IDL:thalesgroup.com/Cdmw/Test/HelloInterface3:1.0";


    TEST_INFO("define fault tolerance properties");
    
    //
    // define fault tolerance properties
    //
    ::FT::Properties prop;
    
    prop.length(4);
    prop[0].nam.length(1);
    prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
    prop[0].val <<= (CORBA::UShort)2;
    
    prop[1].nam.length(1);
    prop[1].nam[0].id="org.omg.ft.ReplicationStyle";
    prop[1].val <<= (CORBA::UShort)2;
    
    prop[2].nam.length(1);
    prop[2].nam[0].id="org.omg.ft.MembershipStyle";
    prop[2].val <<= (CORBA::UShort)0;
    
    prop[3].nam.length(1);
    prop[3].nam[0].id = "org.omg.ft.Factories";
    Cdmw::FT::FactoryInfos factoryInfos1;
    factoryInfos1.add_ftLocation(m_host_name1, "APPL1", "P11");
    factoryInfos1.add_ftLocation(m_host_name2, "APPL3", "P31");
    factoryInfos1.add_ftLocation(m_host_name3, "APPL5", "P51");
    prop[3].val <<= factoryInfos1.get_factoryInfos();

    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::FT::Criteria ft_criteria;
    ft_criteria.length(1);
    ft_criteria[0].nam.length(1);
    ft_criteria[0].nam[0].id="org.omg.ft.FTProperties";
    ft_criteria[0].val <<= prop;


    //
    // Create group HelloInterface1
    //
    TEST_INFO("Create group for HelloInterface1");
    ::FT::ObjectGroup_var obj_group_HelloInterface1;

    try
    {
        obj_group_HelloInterface1 
           =  m_replication_manager->create_object(rep_id_HelloInterface1,
                                                   ft_criteria,
                                                   factory_creation_id1.out());
        TEST_SUCCEED();
    }   
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;
        TEST_FAILED();
        throw;
    }
    

    Cdmw::Test::HelloInterface1_var hello_group_HelloInterface1 = Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
    

    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    Cdmw::OsSupport::OS::sleep( timescale * 1000 );
    
    // call the method on an empty object group
    //(REQ 160)
    try
    {
        TEST_INFO("Calling hello on HelloInterface on an empty object group...");
        hello_group_HelloInterface1->hello();
        TEST_FAILED();
    }
    catch (const CORBA::TRANSIENT& e)
    {
        TEST_INFO("Expected CORBA::TRANSIENT Exception raised" << e);
        if (e.completed () == CORBA::COMPLETED_NO)
            TEST_SUCCEED();  
        else
            TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << e._name() << std::endl;;
        TEST_FAILED();  
    }

    Cdmw::OsSupport::OS::sleep( timescale * 1000 );
    
    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
 
    Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                           repository.in());

    Cdmw::NamingAndRepository::NamingInterface ni =
    Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");


    ::FT::Location loc;
    loc.length(3);
    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL5";
    loc[1].kind = "applicationname";
    loc[2].id = "P51";
    loc[2].kind = "processname";

    std::string s_name = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);

    std::string full_name = "dom3/P51HelloInterface1";

    CORBA::Object_var obj;
    
    TEST_INFO("resolving dom3/P51HelloInterface1...");
    try
    {
        obj = ni.resolve (full_name);
        TEST_SUCCEED();
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::Exception& e)
    {
        std::cout<< e._name() << std::endl;
        TEST_FAILED();
    }


    Cdmw::Test::HelloInterface1_var hello_APPL5_PROC51_INTER1 =  Cdmw::Test::HelloInterface1::_narrow(obj.in());
    TEST_CHECK( ! CORBA::is_nil(hello_APPL5_PROC51_INTER1.in()) );


    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");

    TEST_INFO("resolve_initial_references( ReplicationManager )");
    CORBA::Object_var replication_mgr_obj
    = m_orb->resolve_initial_references( "ReplicationManager" );
    
    m_replication_manager = CdmwFT::ReplicationManager::_narrow(replication_mgr_obj.in() );
    TEST_CHECK( ! CORBA::is_nil(m_replication_manager.in()) );

    try
    {
        
    obj_group_HelloInterface1 = 
       m_replication_manager->add_member(hello_group_HelloInterface1.in(),
                                         loc,
                                         hello_APPL5_PROC51_INTER1.in());
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();  
        std::cerr << e._name() << std::endl;;
    }

    hello_group_HelloInterface1 = Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );


//    This part is in comment because now, the first add member realise a set_primary on the object group
//    Consequently, we can't have a object group without primary
//     // call the method on an object group without primary
//     //(REQ 160, 173)
//     try
//     {
//         TEST_INFO("Calling hello on HelloInterface on an object group without primary...");
//         hello_group_HelloInterface1->hello();
//         TEST_FAILED();
//     }
//     catch (const CORBA::TRANSIENT& e)
//     {
//         TEST_INFO("Expected CORBA::TRANSIENT Exception raised" << e);
//         if (e.completed () == CORBA::COMPLETED_NO)
//             TEST_SUCCEED();  
//         else
//             TEST_FAILED();
//     }
//     catch (const CORBA::SystemException& e)
//     {
//         TEST_FAILED();  
//         std::cerr << e._name() << std::endl;;
//     }

    // add new member

    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL3";
    loc[1].kind = "applicationname";
    loc[2].id = "P31";
    loc[2].kind = "processname";

    s_name = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);
    
    full_name = "dom3/P31HelloInterface1";

    // recover the reference of the APPL3/PROC31/HelloInterface1 object
    TEST_INFO("resolving dom3/P31HelloInterface1...");
    try
    {
        obj = ni.resolve (full_name);
        TEST_SUCCEED();
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::Exception& e)
    {
        std::cout<< e._name() << std::endl;
        TEST_FAILED();
    }

    TEST_INFO("narrowing hello_APPL3_PROC31_INTER1... ");
    Cdmw::Test::HelloInterface1_var hello_APPL3_PROC31_INTER1 =  Cdmw::Test::HelloInterface1::_narrow(obj.in());
    TEST_CHECK( ! CORBA::is_nil(hello_APPL3_PROC31_INTER1.in()) );

    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface1 = 
       m_replication_manager->add_member(hello_group_HelloInterface1.in(),
                                         loc,
                                         hello_APPL3_PROC31_INTER1.in());
    hello_group_HelloInterface1 = 
       Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

    TEST_INFO("get client information");
    Cdmw::Test::process_info_var hello_APPL3_PROC31_INTER1_info 
       = hello_APPL3_PROC31_INTER1->who_is_called();
    Cdmw::Test::process_info_var hello_APPL5_PROC51_INTER1_info 
       = hello_APPL5_PROC51_INTER1->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS                                                       "<<std::endl;
    std::cout<<"     |                                                        "<<std::endl;
    std::cout<<"     |                                                        "<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface1------backup "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL5------P51------HelloInterface1------primary"<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL5_PROC51_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    std::cout<<"Press any key and Enter"<<std::endl;
    std::string dummy;
    std::cin >> dummy;

    // call the method on an object group with primary
    // and get the group version
    //(REQ 160)
    int version_HelloInterface1 = 0;
    
    try
    {
        TEST_INFO("Calling hello on HelloInterface1...");
        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface1->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL5_PROC51_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();
 
        version_HelloInterface1 = hello_group_HelloInterface1->group_version_identification();
        
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    // add new member

    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P11";
    loc[2].kind = "processname";


    s_name = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);
    
    full_name = "dom3/P11HelloInterface1";


    // recover the reference of the APPL1/PROC11/HelloInterface1 object
    try
    {
        obj = ni.resolve (full_name);
        TEST_SUCCEED();
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }

    Cdmw::Test::HelloInterface1_var hello_APPL1_PROC11_INTER1 =  Cdmw::Test::HelloInterface1::_narrow(obj.in());

    TEST_INFO("get client information");
    Cdmw::Test::process_info_var hello_APPL1_PROC11_INTER1_info = hello_APPL1_PROC11_INTER1->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface1------backup "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface1------backup"<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL5------P51------HelloInterface1------primary "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL5_PROC51_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;



    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface1 =
       m_replication_manager->add_member(hello_group_HelloInterface1.in(),
                                         loc,
                                         hello_APPL1_PROC11_INTER1.in());
    hello_group_HelloInterface1 = 
       Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

    // call the method on an object group with primary
    // and get the group version
    // the group version must be higher than the previous group version stored
    //(REQ 160)
    try
    {
        TEST_INFO("Calling hello on HelloInterface1...");
        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface1->who_is_called();
        TEST_SUCCEED();
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL5_PROC51_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();


        TEST_INFO("check the version");
        if ( version_HelloInterface1 < hello_group_HelloInterface1->group_version_identification())
        {
            version_HelloInterface1 = hello_group_HelloInterface1->group_version_identification();
            TEST_SUCCEED();
        }
        else
            TEST_FAILED();
        

    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }
    

    // get_object_group_id
    // (REQ 178)
    ::FT::ObjectGroupId object_group_id = 0;
    Cdmw::Test::HelloInterface1_var temp;
    std::cout<<"(REQ 178) Calling the get_object_group_id on the replication manager"<<std::endl;
    try
    {
        object_group_id = m_replication_manager->get_object_group_id(hello_group_HelloInterface1.in());
        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        TEST_FAILED();
    }
    
    // use replication manager method to recover the most recent object group reference
    TEST_INFO("(REQ 178) Calling the hello interface from the reference coming from the replication manager (get_object_group_ref_from_gid)");
    
    try
    {
        ::FT::ObjectGroup_var object_group = m_replication_manager->get_object_group_ref_from_gid(object_group_id);
        temp = Cdmw::Test::HelloInterface1::_narrow(object_group.in());

        Cdmw::Test::process_info_var temp_info = temp->who_is_called();
        TEST_SUCCEED();
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL5_PROC51_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();




    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;
        TEST_FAILED();
    }

    TEST_INFO("Calling the hello interface from the reference coming from the replication manager (get_object_group_ref)");

    try
    {
        ::FT::ObjectGroup_var object_group = m_replication_manager->get_object_group_ref(hello_group_HelloInterface1.in());
        temp = Cdmw::Test::HelloInterface1::_narrow(object_group.in());

        Cdmw::Test::process_info_var temp_info = temp->who_is_called();
        TEST_SUCCEED();
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL5_PROC51_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        TEST_FAILED();
    }

    // set_primary on the P11 member
    // (REQ 010)
    TEST_INFO("REQ 010: select the P11 member as the next primary, remove the primary and add it again");
    try
    {
        m_replication_manager->set_first_backup(loc);
        TEST_SUCCEED();
    }
    catch (const ::CdmwFT::AlreadyPrimary& )
    {
        TEST_INFO("Unexpected AlreadyPrimary exception raised!");
        TEST_FAILED();
    }
    catch (const ::FT::MemberNotFound& )
    {
        TEST_INFO("Unexpected MemberNotFound exception raised!");
        TEST_FAILED();
    }
    catch (const ::FT::BadReplicationStyle& )
    {
        TEST_INFO("Unexpected BadReplicationStyle exception raised!");
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO(e._name());
        TEST_FAILED();
    }

    loc.length(3);
    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL5";
    loc[1].kind = "applicationname";
    loc[2].id = "P51";
    loc[2].kind = "processname";


    int helloInterface1_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in());

    try
    {
        obj_group_HelloInterface1 = 
           m_replication_manager->remove_member(hello_group_HelloInterface1.in(),
                                                loc);
        TEST_SUCCEED();
    }
    catch( const CORBA::SystemException& e )
    {
        TEST_FAILED();
    }

    try
    {
        
        obj_group_HelloInterface1 = 
            m_replication_manager->add_member(hello_group_HelloInterface1.in(),
                                              loc,
                                              hello_APPL5_PROC51_INTER1.in());
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();  
        std::cerr << e << std::endl;;
    }
    catch (const CORBA::Exception& e)
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();  
    }


    hello_group_HelloInterface1 = Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );


    
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface1------primary"<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface1------backup "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL5------P51------HelloInterface1------backup "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL5_PROC51_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    // call the method on an object group with primary
    //(REQ 160)
    try
    {
        TEST_INFO("Calling hello on HelloInterface1...");

        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface1->who_is_called();
        TEST_SUCCEED();
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL1_PROC11_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();



        if ( version_HelloInterface1 < hello_group_HelloInterface1->group_version_identification())
        {
            version_HelloInterface1 = hello_group_HelloInterface1->group_version_identification();
            TEST_SUCCEED();
        }
        else
            TEST_FAILED();

    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }


    try
    {
        TEST_INFO("Calling hello on HelloInterface1...");
        hello_group_HelloInterface1->hello();
        TEST_SUCCEED();
        
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }
    
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;


    // calling the server to recover the reference of the server object group 
    // the object group (on client side) must be equivalent of the object group on replication manager side
    // (REQ 181)
    std::cout<<"the object group (on client side) must be equivalent of the object group on replication manager side "<<std::endl;
    
    try
    {
        ::FT::ObjectGroupId id = m_replication_manager->get_object_group_id(hello_group_HelloInterface1.in());
        ::FT::ObjectGroup_var obj = hello_group_HelloInterface1->get_objectgroup(id);
        TEST_SUCCEED();
        
        if (obj->_is_equivalent(obj_group_HelloInterface1.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        TEST_FAILED();
    }

    




    // use replication manager method to recover the next object group reference
    // (REQ 178)
    TEST_INFO("use replication manager method to recover the next object group reference");
    try
    {
        if ( Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in()) >
             Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), temp.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        ::FT::ObjectGroup_var object_group = m_replication_manager->get_object_group_ref(hello_group_HelloInterface1.in());
        temp = Cdmw::Test::HelloInterface1::_narrow(object_group.in());

        if ( Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in()) ==
             Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), temp.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();

        Cdmw::Test::process_info_var temp_info = temp->who_is_called();
        TEST_SUCCEED();
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL1_PROC11_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        object_group = m_replication_manager->get_object_group_ref_from_gid(object_group_id);
        temp = Cdmw::Test::HelloInterface1::_narrow(object_group.in());

        if ( Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in()) ==
             Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), temp.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();

        temp_info = temp->who_is_called();
        TEST_SUCCEED();
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL1_PROC11_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        TEST_FAILED();
    }


    // call the method on an object group with primary
    // the method called is the send_exseption which generated exception
    // (REQ 162 & 164)
    TEST_INFO("(REQ 162 & 164) Calling the send_exception interface...");
    for (int i = 1; i<9; i++)
    {
        try
        {

        switch (i) 
        {
            case 1:
                {
                    std::cout<<"throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_NO)"<<std::endl;
                    break;
                 }
            case 2:
                {
                    std::cout<<"throw CORBA::COMM_FAILURE(minor_code, CORBA::COMPLETED_MAYBE)"<<std::endl;
                    break;
                }
            case 3:
                {
                    std::cout<<"throw CORBA::TRANSIENT(minor_code, CORBA::COMPLETED_NO)"<<std::endl;
                    break;
                }
            case 4:
                {
                    std::cout<<"throw CORBA::TRANSIENT(minor_code, CORBA::COMPLETED_MAYBE)"<<std::endl;
                    break;
                }
            case 5:
                {
                    std::cout<<"throw CORBA::NO_RESPONSE(minor_code, CORBA::COMPLETED_NO)"<<std::endl;
                    break;
                }
            case 6:
                {
                    std::cout<<"throw CORBA::NO_RESPONSE(minor_code, CORBA::COMPLETED_MAYBE)"<<std::endl;
                     break;
               }
            case 7:
                {
                    std::cout<<"throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_NO)"<<std::endl;
                    break;
                }
            case 8:
                {
                    std::cout<<"throw CORBA::OBJ_ADAPTER(minor_code, CORBA::COMPLETED_MAYBE)"<<std::endl;
                    break;
                }
        }

            
            if (hello_group_HelloInterface1->send_exception(i) == 2)
                TEST_SUCCEED();  
        }
        catch (const CORBA::SystemException& e)
        {
            TEST_FAILED();
        }
    }


    // Call the send_exception with 9 in parameter
    // the method return always TRANSIENT
    // this allow to check the Request Duration Time
    // (REQ-163)
    // Obtain the ORB’s PolicyManager
    TEST_INFO("Check the ::FT::REQUEST_DURATION_POLICY");
    
    std::cout << "Obtain the ORBs PolicyManager " << std::endl;
    CORBA::Object_var objPM = m_orb->resolve_initial_references("ORBPolicyManager");
    
    CORBA::PolicyManager_var policy_manager = CORBA::PolicyManager::_narrow(objPM.in());

    CORBA::PolicyTypeSeq policy_types(1);
    policy_types.length(1);
    policy_types[0] = ::FT::REQUEST_DURATION_POLICY;

    CORBA::PolicyList_var set_policies 
    = policy_manager->get_policy_overrides(policy_types);

    std::cout << "Number of set policies is: " << set_policies->length() << std::endl;

    
    ::FT::RequestDurationPolicy_var durationPolicy = ::FT::RequestDurationPolicy::_narrow((set_policies->operator[](0)).in());
#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
    TimeBase::TimeT duration_time = durationPolicy->request_duration_policy_value();
#else
    TimeBase::TimeT duration_time = durationPolicy->request_duration_value();
#endif
    std::cout<<"Request duration time : "<<duration_time<<std::endl;


    ::Cdmw::OsSupport::OS::Timeval time_start = ::Cdmw::OsSupport::OS::get_time();
    try 
    {
        hello_group_HelloInterface1->send_exception(9);
        TEST_FAILED();
    }
    catch (const CORBA::OBJ_ADAPTER& e)
    {
        std::cout<<e<<std::endl;
        TEST_SUCCEED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<<e<<std::endl;
        TEST_FAILED();
    }
    
    ::Cdmw::OsSupport::OS::Timeval time_end = ::Cdmw::OsSupport::OS::get_time();


    TimeBase::TimeT start = time_start.seconds * 10000000+ time_start.microseconds * 10;
    TimeBase::TimeT end = time_end.seconds * 10000000+ time_end.microseconds * 10;

    // the test is OK if the end-start is contained between the duration_time and the duration_time + 10% 
    TEST_INFO("the test is OK if the end-start is contained between the duration_time and the duration_time + 10% ");
    if (( duration_time < (end - start) ) && ((duration_time + 0.1 * duration_time) > (end - start)))
        TEST_SUCCEED();
    else
        TEST_FAILED();


    // call the method request_identification
    // check if the client_id, retention_id are in the request and 
    // they are OK with the configuration
    TEST_INFO("check if the client_id, retention_id are in the request ");
    ::FT::FTRequestServiceContext_var serviceContext;
    try 
    {
        serviceContext = hello_group_HelloInterface1->request_identification();
        TEST_SUCCEED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

//     Remove because tao generate autmotically a unique client id
//     std::string s1 = (const char*) serviceContext->client_id;
//     std::string s2 = Cdmw::FT::FTConfiguration::get_client_id(m_orb.in());
//     std::cout<<"serviceContext->client_id :"<<s1 <<std::endl;
//     std::cout<<"get_client_id :"<<s2<<std::endl;
//     if (s1 ==  s2)
//         TEST_SUCCEED();
//     else
//         TEST_FAILED();

    // check if the retention id is increase for each request
    TEST_INFO("check if the retention id is increase for each request");
    long retention_id = serviceContext->retention_id;
    try 
    {
        for (int i = 0; i<10; i++)
        {
            ::FT::FTRequestServiceContext_var serviceContext2 = hello_group_HelloInterface1->request_identification();
            retention_id ++;
            if (serviceContext2->retention_id == retention_id)
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }    
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }
     
    
    
    // the Replication Manager Version is lower than the object group reference 
    // create a new object group reference with a higher group version
    TEST_INFO("the Replication Manager Object Version is lower than the object group reference ");
    
    ::FT::ObjectGroup_var object_group_with_bad_version = 
      TestUtils::update_group_with_primary_outside_cdmw_ft(m_orb.in(), 
                                                            hello_group_HelloInterface1.in(),
                                                            m_replication_manager.in(),
                                                            loc);

    temp = Cdmw::Test::HelloInterface1::_narrow(object_group_with_bad_version.in());
    try
    {
        temp->hello();
        TEST_FAILED();
    }        
    catch (const CORBA::INV_OBJREF& e)
    {
        TEST_INFO("Expected CORBA::INV_OBJREF Exception raised");
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_INFO("Unexpected CORBA System Exception raised" << e);
        TEST_FAILED();  
    }


    //
    // Create group HelloInterface2
    //
    TEST_INFO("Create group HelloInterface2");
    
    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id2;
    ::FT::ObjectGroup_var obj_group_HelloInterface2;                       

    Cdmw::FT::FactoryInfos factoryInfos2;
    factoryInfos2.add_ftLocation(m_host_name1, "APPL2", "P21");
    factoryInfos2.add_ftLocation(m_host_name2, "APPL4", "P41");
    factoryInfos2.add_ftLocation(m_host_name3, "APPL6", "P61");
    prop[3].val <<= factoryInfos2.get_factoryInfos();

    ft_criteria[0].val <<= prop;


    try
    {
        obj_group_HelloInterface2 
           =  m_replication_manager->create_object(rep_id_HelloInterface2,
                                                   ft_criteria,
                                                   factory_creation_id2.out());
    }    
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        throw;
    }
    Cdmw::Test::HelloInterface2_var hello_group_HelloInterface2 = Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());
    
    Cdmw::OsSupport::OS::sleep( timescale * 1000 );
    

    // Create and initialize the name
    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P21";
    loc[2].kind = "processname";

    s_name = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);
    
    full_name ="dom3/P21HelloInterface2";
    // recover the reference of the APPL2/PROC21/HelloInterface2 object
    try
    {
        obj = ni.resolve (full_name);
        TEST_SUCCEED();
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }

    Cdmw::Test::HelloInterface2_var hello_APPL2_PROC21_INTER2 =  
       Cdmw::Test::HelloInterface2::_narrow(obj.in());

    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface2 = 
       m_replication_manager->add_member(hello_group_HelloInterface2.in(),
                                         loc,
                                         hello_APPL2_PROC21_INTER2.in());

    hello_group_HelloInterface2 = 
       Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL4";
    loc[1].kind = "applicationname";
    loc[2].id = "P41";
    loc[2].kind = "processname";

    s_name = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);

    full_name =  "dom3/P41HelloInterface2";
    // recover the reference of the APPL4/PROC41/HelloInterface2 object
    try
    {
        obj = ni.resolve (full_name);
        TEST_SUCCEED();
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }

    Cdmw::Test::HelloInterface2_var hello_APPL4_PROC41_INTER2 =  Cdmw::Test::HelloInterface2::_narrow(obj.in());

    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface2 = 
       m_replication_manager->add_member(hello_group_HelloInterface2.in(),
                                         loc,
                                         hello_APPL4_PROC41_INTER2.in());
    hello_group_HelloInterface2 =
       Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL6";
    loc[1].kind = "applicationname";
    loc[2].id = "P61";
    loc[2].kind = "processname";

    s_name = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);
    
    full_name =  "dom3/P61HelloInterface2";

    // recover the reference of the APPL6/PROC61/HelloInterface2 object
    try
    {
        obj = ni.resolve (full_name);
        TEST_SUCCEED();
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }

    Cdmw::Test::HelloInterface2_var hello_APPL6_PROC61_INTER2 = 
       Cdmw::Test::HelloInterface2::_narrow(obj.in());

    int helloInterface2_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in());

    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface2 = 
       m_replication_manager->add_member(hello_group_HelloInterface2.in(),
                                         loc,
                                         hello_APPL6_PROC61_INTER2.in());
    hello_group_HelloInterface2 = 
       Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

    //
    // Create group HelloInterface3
    //
    TEST_INFO("Create group");
    
    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id3;
    ::FT::ObjectGroup_var obj_group_HelloInterface3;

    Cdmw::FT::FactoryInfos factoryInfos3;
    factoryInfos3.add_ftLocation(m_host_name1, "APPL2", "P22");
    factoryInfos3.add_ftLocation(m_host_name2, "APPL4", "P42");
    factoryInfos3.add_ftLocation(m_host_name3, "APPL6", "P62");
    prop[3].val <<= factoryInfos3.get_factoryInfos();

    ft_criteria[0].val <<= prop;

    try
    {
        obj_group_HelloInterface3 =  
           m_replication_manager->create_object(rep_id_HelloInterface3,
                                                ft_criteria,
                                                factory_creation_id3.out());
    }    
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        throw;
    }
    Cdmw::Test::HelloInterface3_var hello_group_HelloInterface3 = Cdmw::Test::HelloInterface3::_narrow(obj_group_HelloInterface3.in());
    
    Cdmw::OsSupport::OS::sleep( timescale * 1000 );

    // Create and initialize the name
    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P22";
    loc[2].kind = "processname";

    s_name = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);
    
    full_name = "dom3/P22HelloInterface3";
    // recover the reference of the APPL2/PROC22/HelloInterface3 object
    try
    {
        obj = ni.resolve (full_name);
        TEST_SUCCEED();
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
    }
    
    Cdmw::Test::HelloInterface3_var hello_APPL2_PROC22_INTER3 =  
       Cdmw::Test::HelloInterface3::_narrow(obj.in());

    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface3 = 
       m_replication_manager->add_member(hello_group_HelloInterface3.in(),
                                         loc,
                                         hello_APPL2_PROC22_INTER3.in());
    hello_group_HelloInterface3 = 
       Cdmw::Test::HelloInterface3::_narrow(obj_group_HelloInterface3.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL4";
    loc[1].kind = "applicationname";
    loc[2].id = "P42";
    loc[2].kind = "processname";

    s_name = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);
    
    full_name = "dom3/P42HelloInterface3";

    // recover the reference of the APPL4/PROC42/HelloInterface3 object
    try
    {
        obj = ni.resolve (full_name);
        TEST_SUCCEED();
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }

    Cdmw::Test::HelloInterface3_var hello_APPL4_PROC42_INTER3 =  
       Cdmw::Test::HelloInterface3::_narrow(obj.in());

    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface3 = 
       m_replication_manager->add_member(hello_group_HelloInterface3.in(),
                                         loc,
                                         hello_APPL4_PROC42_INTER3.in());
    hello_group_HelloInterface3 = 
       Cdmw::Test::HelloInterface3::_narrow(obj_group_HelloInterface3.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL6";
    loc[1].kind = "applicationname";
    loc[2].id = "P62";
    loc[2].kind = "processname";

    s_name = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);
    
    full_name ="dom3/P62HelloInterface3";

    // recover the reference of the APPL6/PROC62/HelloInterface3 object
    try
    {
        obj = ni.resolve (full_name);
        TEST_SUCCEED();
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
        TEST_FAILED();
    }

    Cdmw::Test::HelloInterface3_var hello_APPL6_PROC62_INTER3 =  
       Cdmw::Test::HelloInterface3::_narrow(obj.in());

    int helloInterface3_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in());

    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface3 = 
       m_replication_manager->add_member(hello_group_HelloInterface3.in(),
                                         loc,
                                         hello_APPL6_PROC62_INTER3.in());
    hello_group_HelloInterface3 = 
       Cdmw::Test::HelloInterface3::_narrow(obj_group_HelloInterface3.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );


    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface1...");
        hello_group_HelloInterface1->who_is_called();
        TEST_SUCCEED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface2...");
        hello_group_HelloInterface2->who_is_called();
        TEST_SUCCEED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface3...");
        hello_group_HelloInterface3->who_is_called();
        TEST_SUCCEED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

        TEST_INFO("Create 2 fault notifier consumer");
    
    SIM_consumer* consumer_obj1 = new SIM_consumer();
    SIM_consumer* consumer_obj2 = new SIM_consumer();

    CosNotifyComm::StructuredPushConsumer_var consumer1 = consumer_obj1->_this();
    CosNotifyComm::StructuredPushConsumer_var consumer2 = consumer_obj2->_this();
    ::FT::FaultNotifier_var fault_notifier 
      = ::FT::FaultNotifier::_duplicate(m_replication_manager->get_fault_notifier());

    ::FT::FaultNotifier::ConsumerId id1 = 0;            
    ::FT::FaultNotifier::ConsumerId id2 = 0; 

    TEST_INFO("register the first consumer");
    id1 =  fault_notifier->connect_structured_fault_consumer(consumer1.in());
    std::cout<<"id1:"<<id1<<std::endl;
    
    TEST_INFO("register the second consumer");
    id2 =  fault_notifier->connect_structured_fault_consumer(consumer2.in());
    std::cout<<"id2:"<<id2<<std::endl;

    ::CdmwFT::FaultManagement::FaultDetector_var fault_detector 
      = CdmwFT::FaultManagement::FaultDetector::_duplicate(m_replication_manager->get_fault_detector());

    TEST_INFO("disconnect one of the previous consumer");
    try
    {
        fault_notifier->disconnect_consumer(id2);
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }
    
    TEST_INFO("disconnect an unknown consumer");
    try
    {
        fault_notifier->disconnect_consumer(1000);
        TEST_FAILED();
    }
    catch(const CosEventComm::Disconnected&)
    {
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }


    if ( helloInterface1_version < Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();

    if ( helloInterface2_version < Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();

    if ( helloInterface3_version < Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();


    TEST_INFO("calling all object without the object group ref");
    Cdmw::Test::process_info_var hello_APPL2_PROC21_INTER2_info = hello_APPL2_PROC21_INTER2->who_is_called();
    Cdmw::Test::process_info_var hello_APPL4_PROC41_INTER2_info = hello_APPL4_PROC41_INTER2->who_is_called();
    Cdmw::Test::process_info_var hello_APPL6_PROC61_INTER2_info = hello_APPL6_PROC61_INTER2->who_is_called();

    Cdmw::Test::process_info_var hello_APPL2_PROC22_INTER3_info = hello_APPL2_PROC22_INTER3->who_is_called();
    Cdmw::Test::process_info_var hello_APPL4_PROC42_INTER3_info = hello_APPL4_PROC42_INTER3->who_is_called();
    Cdmw::Test::process_info_var hello_APPL6_PROC62_INTER3_info = hello_APPL6_PROC62_INTER3->who_is_called();



    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface1------primary"<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface2------primary "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface3------primary "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER3_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface1------backup "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL4------P41------HelloInterface2------backup "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL4_PROC41_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P42------HelloInterface3------backup"<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL4_PROC42_INTER3_info->processID<<std::endl;
    std::cout<<"     |-------APPL5------P51------HelloInterface1------backup "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL5_PROC51_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL6------P61------HelloInterface2------backup"<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL6_PROC61_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P62------HelloInterface3------backup "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL6_PROC62_INTER3_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    
    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface1...");
        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface1->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL1_PROC11_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface2...");
        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface2->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL2_PROC21_INTER2_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface3...");
        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface3->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL2_PROC22_INTER3_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    helloInterface1_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in());
    helloInterface2_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in());
    helloInterface3_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in());

    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    TEST_INFO("kill the process P11");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    std::cout<<"NEW SYSTEM"<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1                                              "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface2------primary "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface3------primary "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER3_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface1------primary "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL4------P41------HelloInterface2------backup "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL4_PROC41_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P42------HelloInterface3------backup"<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL4_PROC42_INTER3_info->processID<<std::endl;
    std::cout<<"     |-------APPL5------P51------HelloInterface1------backup "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL5_PROC51_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL6------P61------HelloInterface2------backup"<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL6_PROC61_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P62------HelloInterface3------backup "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL6_PROC62_INTER3_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface1...");

        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface1->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL3_PROC31_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface2...");

        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface2->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL2_PROC21_INTER2_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();


    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<<e<<std::endl;
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface3...");

        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface3->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL2_PROC22_INTER3_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    TEST_INFO("Check the good version of each object group");
    
    if ( helloInterface1_version < Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();

    if ( helloInterface2_version == Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();

    if ( helloInterface3_version == Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();
  
    helloInterface1_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in());
    helloInterface2_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in());
    helloInterface3_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in());
    
    TEST_INFO("Check the good number of member in each object group");
    if ( Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface1.in()) == 2)
        TEST_SUCCEED();
    else
        TEST_FAILED();

    if ( Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface2.in()) == 3)
        TEST_SUCCEED();
    else
        TEST_FAILED();

    if ( Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface3.in()) == 3)
        TEST_SUCCEED();
    else
        TEST_FAILED();
    

    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    TEST_INFO("kill the agent of the APPL2");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    Cdmw::OsSupport::OS::sleep( timescale * 2000 );

    std::cout<<"NEW SYSTEM"<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1                                              "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface1------primary "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL4------P41------HelloInterface2------primary "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL4_PROC41_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P42------HelloInterface3------primary"<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL4_PROC42_INTER3_info->processID<<std::endl;
    std::cout<<"     |-------APPL5------P51------HelloInterface1------backup "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL5_PROC51_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL6------P61------HelloInterface2------backup"<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL6_PROC61_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P62------HelloInterface3------backup "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL6_PROC62_INTER3_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;


    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface1...");

        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface1->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL3_PROC31_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();


    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface2...");

        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface2->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL4_PROC41_INTER2_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();


    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface3...");

        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface3->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL4_PROC42_INTER3_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();


    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    TEST_INFO("Check the good version of each object group");
    std::cout<<"helloInterface1_version :"<<helloInterface1_version<<std::endl;
    std::cout<<" Cdmw::FT::TestUtils::get_object_group_version :"<<Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in())<<std::endl;

    if ( helloInterface1_version == Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();
    std::cout<<"helloInterface2_version :"<<helloInterface2_version<<std::endl;
    std::cout<<" Cdmw::FT::TestUtils::get_object_group_version :"<<Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in())<<std::endl;

    if ( helloInterface2_version < Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();
    std::cout<<"helloInterface3_version :"<<helloInterface3_version<<std::endl;
    std::cout<<" Cdmw::FT::TestUtils::get_object_group_version :"<<Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in())<<std::endl;

    if ( helloInterface3_version < Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();
  
    helloInterface1_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in());
    helloInterface2_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in());
    helloInterface3_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in());
    TEST_INFO("Check the good number of member in each object group");
    if ( Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface1.in()) == 2)
        TEST_SUCCEED();
    else
        TEST_FAILED();

    if ( Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface2.in()) == 2)
        TEST_SUCCEED();
    else
        TEST_FAILED();

    if ( Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface3.in()) == 2)
        TEST_SUCCEED();
    else
        TEST_FAILED();
    

    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    TEST_INFO("kill the daemon on the host2");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin>>dummy;
    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    
    std::cout<<"NEW SYSTEM"<<std::endl;
        std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1                                              "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |-------APPL5------P51------HelloInterface1------primary "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL5_PROC51_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL6------P61------HelloInterface2------primary"<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL6_PROC61_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P62------HelloInterface3------primary "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL6_PROC62_INTER3_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface1...");

        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface1->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL5_PROC51_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();


    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface2...");
        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface2->who_is_called();
        TEST_SUCCEED();

        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL6_PROC61_INTER2_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch (const CORBA::TRANSIENT& e)
    {
        TEST_INFO("Expected CORBA::TRANSIENT Exception raised" << e);
        if (e.completed () == CORBA::COMPLETED_NO)
            TEST_SUCCEED();  
        else
            TEST_FAILED();
    }

    catch (const CORBA::SystemException& e)
    {
        std::cout<<e<<std::endl;
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface3...");
        hello_group_HelloInterface3->hello();
        TEST_SUCCEED();

        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface3->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL6_PROC62_INTER3_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }
    TEST_INFO("Check the good version of each object group");
    std::cout<<"helloInterface1_version :"<<helloInterface1_version<<std::endl;
    std::cout<<" Cdmw::FT::TestUtils::get_object_group_version :"<<Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in())<<std::endl;
    
    if ( helloInterface1_version < Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in()) )
        TEST_SUCCEED();
    
    else 
        TEST_FAILED();
    std::cout<<"helloInterface2_version :"<<helloInterface2_version<<std::endl;
    std::cout<<" Cdmw::FT::TestUtils::get_object_group_version :"<<Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in())<<std::endl;

    if ( helloInterface2_version < Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();
    std::cout<<"helloInterface3_version :"<<helloInterface3_version<<std::endl;
    std::cout<<" Cdmw::FT::TestUtils::get_object_group_version :"<<Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in())<<std::endl;

    if ( helloInterface3_version < Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in()) )
        TEST_SUCCEED();
    else 
        TEST_FAILED();
  
    helloInterface1_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1.in());
    helloInterface2_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface2.in());
    helloInterface3_version = Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface3.in());

    TEST_INFO("Check the good number of member in each object group");

    std::cout<<"get_number_of_member"<<Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface1.in())<<std::endl;
    if ( Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface1.in()) == 1)
        TEST_SUCCEED();
    else
        TEST_FAILED();

    std::cout<<"get_number_of_member"<<Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface2.in())<<std::endl;

    if ( Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface2.in()) == 1)
        TEST_SUCCEED();
    else
        TEST_FAILED();

    std::cout<<"get_number_of_member"<<Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface3.in())<<std::endl;

    if ( Cdmw::FT::TestUtils::get_number_of_member(m_orb.in(), hello_group_HelloInterface3.in()) == 1)
        TEST_SUCCEED();
    else
        TEST_FAILED();




    TEST_INFO("calling send_exception for the emission of a notifyFatalError from the server to the supervision");
    try
    {
        hello_group_HelloInterface1->send_exception(11);
        TEST_SUCCEED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    
    std::cout<<"NEW SYSTEM"<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1                                              "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |-------APPL6------P61------HelloInterface2------primary"<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL6_PROC61_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P62------HelloInterface3------primary "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL6_PROC62_INTER3_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface1...");
        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface1->who_is_called();
        TEST_FAILED();
    }
    catch (const CORBA::TRANSIENT& e)
    {
        TEST_INFO("Expected CORBA::TRANSIENT Exception raised" << e);
        if (e.completed () == CORBA::COMPLETED_NO)
            TEST_SUCCEED();  
        else
            TEST_FAILED();
    }

    catch (const CORBA::SystemException& e)
    {
        std::cout<<e<<std::endl;
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface2...");
        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface2->who_is_called();
        TEST_SUCCEED();  

    }
    catch (const CORBA::TRANSIENT& e)
    {
        TEST_INFO("Unexpected CORBA::TRANSIENT Exception raised" << e);
        TEST_FAILED();
    }

    catch (const CORBA::SystemException& e)
    {
        std::cout<<e<<std::endl;
        TEST_FAILED();
    }

    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface3...");
        hello_group_HelloInterface3->hello();
        TEST_SUCCEED();

        Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface3->who_is_called();
        TEST_SUCCEED();
        
        std::cout<<"the process called is the "<<temp_info->name<<std::endl;
        if (temp_info->processID == hello_APPL6_PROC62_INTER3_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }






    // remove a new menber on the group previously deleted
    TEST_INFO("Remove a member P61 from the helloInterface1");
    loc.length(3);
    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL6";
    loc[1].kind = "applicationname";
    loc[2].id = "P61";
    loc[2].kind = "processname";
    try
    {
        obj_group_HelloInterface2 = m_replication_manager->remove_member(hello_group_HelloInterface2.in(),
                                                                         loc);
        TEST_SUCCEED();
    }
    catch( const CORBA::SystemException& e )
    {
        TEST_FAILED();
    }


    TEST_INFO("Remove a member P62 from the helloInterface3");
    loc.length(3);
    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL6";
    loc[1].kind = "applicationname";
    loc[2].id = "P62";
    loc[2].kind = "processname";
    try
    {
        obj_group_HelloInterface3 = m_replication_manager->remove_member(hello_group_HelloInterface3.in(),
                                                                         loc);
        TEST_SUCCEED();
    }
    catch( const CORBA::SystemException& e )
    {
        TEST_FAILED();
    }

    // remove the object group
    TEST_INFO("Delete the object group helloInterface1");
    try
    {
        m_replication_manager->delete_object(factory_creation_id1.in());    
        TEST_SUCCEED();
    }
    
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    } 


    // remove the object group
    TEST_INFO("Delete the object group helloInterface2");
    try
    {
        m_replication_manager->delete_object(factory_creation_id2.in());    
        TEST_SUCCEED();
    }
    
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    } 

    // remove the object group
    TEST_INFO("Delete the object group helloInterface3");
    try
    {
        m_replication_manager->delete_object(factory_creation_id3.in());    
        TEST_SUCCEED();
    }
    
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;
        TEST_FAILED();
    }

}


} // end namespace FT

} // end namespace Cdmw

