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

#include <Foundation/orbsupport/CORBA.hpp>

#include <faulttoleranceclient2/TestFTInit.hpp>
#include <faulttoleranceclient2/TestUtils.hpp>

#include <faulttoleranceclient2/TestHello.stub.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

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
    set_nbOfRequestedTestOK (28);

    const char * rep_id_HelloInterface1 = "IDL:thalesgroup.com/Cdmw/Test/HelloInterface1:1.0";
    const char * rep_id_HelloInterface2 = "IDL:thalesgroup.com/Cdmw/Test/HelloInterface2:1.0";

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
    factoryInfos1.add_ftLocation(m_host_name2, "APPL2", "P21");
    factoryInfos1.add_ftLocation(m_host_name3, "APPL3", "P31");
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
    TEST_INFO("Create group");
    ::FT::ObjectGroup_var obj_group_HelloInterface1;

    try
    {
        obj_group_HelloInterface1 =  m_replication_manager->create_object(rep_id_HelloInterface1,
                                                                          ft_criteria,
                                                                          factory_creation_id1.out());
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e << std::endl;;
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
        std::cout << e << std::endl;;
        TEST_SUCCEED();  
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout << e << std::endl;;
        TEST_FAILED();  
    }


    Cdmw::OsSupport::OS::sleep( timescale * 1000 );

    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
  
    Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                           repository.in());

    Cdmw::CommonSvcs::Naming::NamingInterface ni =
    Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");


    ::FT::Location loc;
    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P11";
    loc[2].kind = "processname";

    std::string s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    std::string full_name = "dom3/P11HelloInterface1";
    CORBA::Object_var obj;
    
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


    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    try
    {
        obj_group_HelloInterface1 = m_replication_manager->add_member(hello_group_HelloInterface1.in(),
                                                                      loc,
                                                                      hello_APPL1_PROC11_INTER1.in());
        hello_group_HelloInterface1 = Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
        TEST_SUCCEED();
        Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }
    

    // add new member

    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P21";
    loc[2].kind = "processname";

    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P21HelloInterface1";


    // recover the reference of the APPL2/PROC21/HelloInterface1 object
    try
    {
        obj = ni.resolve (full_name);
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

    Cdmw::Test::HelloInterface1_var hello_APPL2_PROC21_INTER1 =  Cdmw::Test::HelloInterface1::_narrow(obj.in());

    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    try
    {
        obj_group_HelloInterface1 = m_replication_manager->add_member(hello_group_HelloInterface1.in(),
                                                                      loc,
                                                                      hello_APPL2_PROC21_INTER1.in());
        hello_group_HelloInterface1 = Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
        TEST_SUCCEED();
        Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }
    

    Cdmw::Test::HelloInterface1_var hello_group_HelloInterface1_bis = Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
    TEST_SUCCEED();

    TEST_INFO("calling all object without the object group ref");
    Cdmw::Test::process_info_var hello_APPL1_PROC11_INTER1_info = hello_APPL1_PROC11_INTER1->who_is_called();
    Cdmw::Test::process_info_var hello_APPL2_PROC21_INTER1_info = hello_APPL2_PROC21_INTER1->who_is_called();


    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface1------primary"<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface1------backup "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL2_PROC21_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    
    // call the method on an object group with primary
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

    

    TEST_INFO("kill the process P11 and P21");

    TEST_INFO("Press any key and Enter to continue");
    std::string dummy;
    std::cin >> dummy;

    TEST_INFO("process P11 and P21 killed");
    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    // add new member

    loc.length(3);
    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL3";
    loc[1].kind = "applicationname";
    loc[2].id = "P31";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P31HelloInterface1";


    // recover the reference of the APPL3/PROC31/HelloInterface1 object
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

    Cdmw::Test::HelloInterface1_var hello_APPL3_PROC31_INTER1 =  Cdmw::Test::HelloInterface1::_narrow(obj.in());
    Cdmw::Test::process_info_var hello_APPL3_PROC31_INTER1_info = hello_APPL3_PROC31_INTER1->who_is_called();

    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    try
    {
        obj_group_HelloInterface1 = m_replication_manager->add_member(hello_group_HelloInterface1.in(),
                                                                      loc,
                                                                      hello_APPL3_PROC31_INTER1.in());
        //hello_group_HelloInterface1 = Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
        TEST_SUCCEED();
        Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    if (Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1_bis.in()) <
        Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), obj_group_HelloInterface1.in()))
        TEST_SUCCEED();
    else
        TEST_FAILED();
    
    // call the method on an object group with primary
    try
    {
        TEST_INFO("Calling hello on HelloInterface1... (on the old object)");
//        hello_group_HelloInterface1_bis->hello();
        Cdmw::Test::process_info_var temp = hello_group_HelloInterface1_bis->who_is_called();
        TEST_SUCCEED();

        if (temp->processID == hello_APPL3_PROC31_INTER1_info->processID)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        // Test REQ-SRS-FLT-120
        if (Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface1_bis.in()) ==
            Cdmw::FT::TestUtils::get_object_group_version(m_orb.in(), obj_group_HelloInterface1.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

//     TEST_INFO("Press any key and Enter to continue");
//     std::cin >> dummy;


    TEST_INFO("start again the process P11 and P21");

    TEST_INFO("Press any key and Enter to continue");
    std::cin >> dummy;

    TEST_INFO("process P11 and P21 started");
    Cdmw::OsSupport::OS::sleep( timescale * 4000 );
    // add new member


    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P11";
    loc[2].kind = "processname";

    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
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

    hello_APPL1_PROC11_INTER1 =  Cdmw::Test::HelloInterface1::_narrow(obj.in());


    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    try
    {
        obj_group_HelloInterface1 = m_replication_manager->add_member(hello_group_HelloInterface1.in(),
                                                                      loc,
                                                                      hello_APPL1_PROC11_INTER1.in());
        //hello_group_HelloInterface1 = Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
        TEST_SUCCEED();
        Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    }
    catch (const ::FT::ObjectGroupNotFound& e)
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }
    catch (const ::FT::MemberAlreadyPresent& e)
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }
    catch (const ::FT::ObjectNotAdded& e)
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }

    loc.length(3);
    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P21";
    loc[2].kind = "processname";

    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P21HelloInterface1";


    // recover the reference of the APPL1/PROC21/HelloInterface1 object
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

    hello_APPL2_PROC21_INTER1 =  Cdmw::Test::HelloInterface1::_narrow(obj.in());


    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    try
    {
        obj_group_HelloInterface1 = 
           m_replication_manager->add_member(hello_group_HelloInterface1.in(),
                                             loc,
                                             hello_APPL2_PROC21_INTER1.in());
        hello_group_HelloInterface1 = 
           Cdmw::Test::HelloInterface1::_narrow(obj_group_HelloInterface1.in());
        TEST_SUCCEED();
        Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    }
    catch (const ::FT::ObjectGroupNotFound& e)
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }
    catch (const ::FT::MemberAlreadyPresent& e)
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }
    catch (const ::FT::ObjectNotAdded& e)
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

//     TEST_INFO("Press any key and Enter to continue");
//     std::cin >> dummy;

    // call the method on an object group with primary
    TEST_INFO("Check the COMM_FAILURE exception thrown by the ORB(the process server is aborted");
    try
    {
        TEST_INFO("Calling send_exception");
        hello_group_HelloInterface1->send_exception(10);
        TEST_SUCCEED();

    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<<e<<std::endl;
        TEST_FAILED();
    }
  

    TEST_INFO("Press any key and Enter to continue");
    std::cin >> dummy;

    
    // remove a new menber on the group previously deleted
    TEST_INFO("Remove a member P21 from the helloInterface1");
    loc.length(3);
    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P21";
    loc[2].kind = "processname";
    try
    {
        obj_group_HelloInterface1 = 
           m_replication_manager->remove_member(hello_group_HelloInterface1.in(),
                                                loc);
        TEST_SUCCEED();
    }
    catch( const CORBA::SystemException& e )
    {
        TEST_INFO(e._name());
        TEST_FAILED();
    }
    catch( const CORBA::Exception& e )
    {
        TEST_INFO(e._name());
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



    TEST_INFO("TEST 2");
    Cdmw::OsSupport::OS::sleep( timescale * 1000 );

    TEST_INFO("Press any key and Enter to continue");
    std::cin >> dummy;

    //
    // Create group HelloInterface2
    //
    TEST_INFO("Create group");
    ::FT::ObjectGroup_var obj_group_HelloInterface2;
    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id2;

    Cdmw::FT::FactoryInfos factoryInfos2;
    factoryInfos2.add_ftLocation(m_host_name1, "APPL1", "P12");
    factoryInfos2.add_ftLocation(m_host_name2, "APPL2", "P22");
    factoryInfos2.add_ftLocation(m_host_name3, "APPL3", "P32");
    prop[3].val <<= factoryInfos2.get_factoryInfos();

    ft_criteria[0].val <<= prop;



    try
    {
        obj_group_HelloInterface2 =  m_replication_manager->create_object(rep_id_HelloInterface2,
                                                                          ft_criteria,
                                                                          factory_creation_id2.out());
    }    
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        throw;
    }
    

    Cdmw::Test::HelloInterface2_var hello_group_HelloInterface2 = Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());

    loc.length(3);
    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P22";
    loc[2].kind = "processname";

    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P22HelloInterface2";

    // recover the reference of the APPL2/PROC22/HelloInterface2 object
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

    Cdmw::Test::HelloInterface2_var hello_APPL2_PROC22_INTER2 =  Cdmw::Test::HelloInterface2::_narrow(obj.in());
    


    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    try
    {
    obj_group_HelloInterface2 = m_replication_manager->add_member(hello_group_HelloInterface2.in(),
                                                                loc,
                                                                hello_APPL2_PROC22_INTER2.in());
    hello_group_HelloInterface2 = Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());
    TEST_SUCCEED();
    }
    
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );
  

    // add new member

    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL3";
    loc[1].kind = "applicationname";
    loc[2].id = "P32";
    loc[2].kind = "processname";

    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P32HelloInterface2";


    // recover the reference of the APPL2/PROC22/HelloInterface2 object
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

    Cdmw::Test::HelloInterface2_var hello_APPL3_PROC32_INTER2 =  Cdmw::Test::HelloInterface2::_narrow(obj.in());

    // add a new member in the object group
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    try
    {
    obj_group_HelloInterface2 = m_replication_manager->add_member(hello_group_HelloInterface2.in(),
                                                                loc,
                                                                hello_APPL3_PROC32_INTER2.in());
    hello_group_HelloInterface2 = Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());
    }
    
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );


    // add new member

    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P12";
    loc[2].kind = "processname";

    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P12HelloInterface2";


    // recover the reference of the APPL1/PROC12/HelloInterface2 object

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

    Cdmw::Test::HelloInterface2_var hello_APPL1_PROC12_INTER2 =  
       Cdmw::Test::HelloInterface2::_narrow(obj.in());

    // add a new member in the object group
    // (REQ 159)
    TEST_INFO("Add new member on the group");
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    try
    {
        obj_group_HelloInterface2 = 
           m_replication_manager->add_member(hello_group_HelloInterface2.in(),
                                             loc,
                                             hello_APPL1_PROC12_INTER2.in());
        hello_group_HelloInterface2 = 
           Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());
    }
    
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    Cdmw::Test::process_info_var hello_APPL1_PROC12_INTER2_info = hello_APPL1_PROC12_INTER2->who_is_called();
    Cdmw::Test::process_info_var hello_APPL2_PROC22_INTER2_info = hello_APPL2_PROC22_INTER2->who_is_called();
    Cdmw::Test::process_info_var hello_APPL3_PROC32_INTER2_info = hello_APPL3_PROC32_INTER2->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P12------HelloInterface2------backup "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC12_INTER2_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P22------HelloInterface2------primary "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER2_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P32------HelloInterface2------backup"<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL3_PROC32_INTER2_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    
    // (REQ 158)
    TEST_INFO("remove_member P12 (this request must be intercepted by an interceptor on the server side");
    TEST_INFO("=> the version on the location manager is lower than the Replication Manager");
    
    // remove the first member
    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P12";
    loc[2].kind = "processname";

    try
    {
        obj_group_HelloInterface2 = m_replication_manager->remove_member(hello_group_HelloInterface2.in(), loc);
        //hello_group_HelloInterface2 = Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());
        TEST_SUCCEED();
    }
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }

    TEST_INFO("calling all object without the object group ref");

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS                                                         "<<std::endl;
    std::cout<<"     |                                                          "<<std::endl;
    std::cout<<"     |-------APPL2------P22------HelloInterface2------primary "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER2_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P32------HelloInterface2------backup"<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL3_PROC32_INTER2_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    
    // call the method on an object group with primary
    // the object group version in the location manager of the process P22 is lower than the 
    // version of the replication manager
    try
    {
        TEST_INFO("Calling hello on HelloInterface2...");
        hello_group_HelloInterface2->hello();
        TEST_FAILED();
    }
    catch (const CORBA::TRANSIENT& e)
    {
        std::cout << e << std::endl;;
        TEST_SUCCEED();  
    }

    catch (const CORBA::SystemException& e)
    {
        std::cerr << e << std::endl;
        TEST_FAILED();
    }

    // remove the third member
    loc.length(3);
    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL3";
    loc[1].kind = "applicationname";
    loc[2].id = "P32";
    loc[2].kind = "processname";

    try
    {
        obj_group_HelloInterface2 = 
           m_replication_manager->remove_member(hello_group_HelloInterface2.in(), loc);
        hello_group_HelloInterface2 = 
           Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());
        TEST_SUCCEED();
    }
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;;
        TEST_FAILED();
    }

    // remove the third member
    loc.length(3);
    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P22";
    loc[2].kind = "processname";

    try
    {
        obj_group_HelloInterface2 = 
           m_replication_manager->remove_member(hello_group_HelloInterface2.in(), loc);
        hello_group_HelloInterface2 = 
           Cdmw::Test::HelloInterface2::_narrow(obj_group_HelloInterface2.in());
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
        std::cerr << e << std::endl;
        TEST_FAILED();
    }

}


} // end namespace FT

} // end namespace Cdmw

