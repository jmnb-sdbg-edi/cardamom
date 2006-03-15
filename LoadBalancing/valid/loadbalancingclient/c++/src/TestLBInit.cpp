/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#include <loadbalancingclient/TestLBInit.hpp>
#include <loadbalancingclient/TestUtils.hpp>

#include <loadbalancingclient/TestHello.stub.hpp>
#include "Foundation/ossupport/OS.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

namespace Cdmw 
{

namespace LB 
{

// constructor
TestLBInit::TestLBInit(CORBA::ORB_ptr                  orb,
                       PortableServer::POA_ptr         rootPOA,
                       CdmwLB::LBGroupManager_ptr group_manager,
                       const std::string&              name ,
                       std::string host_name1,
                       std::string host_name2,
		       std::string host_name3)
    : Testable(name),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_rootPOA(PortableServer::POA::_duplicate(rootPOA)),
      m_group_manager(CdmwLB::LBGroupManager::_duplicate(group_manager)),
      m_host_name1(host_name1),
      m_host_name2(host_name2),
      m_host_name3(host_name3)
{    
    
}

// destructor
TestLBInit::~TestLBInit()
{
}

// do_tests
void TestLBInit::do_tests()
{

    set_nbOfRequestedTestOK (116);
    const char * rep_id_HelloInterface = "IDL:CdmwLBInit/HelloInterface:1.0";


    TEST_INFO("define load balancing properties");
    
    std::string lb_policy = "default";
    //
    // Create a group with default load balancing policy (RoundRobin)
    //
    ::PortableGroup::Properties prop;

    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    prop[0].val <<=  lb_policy.c_str();

    ::PortableGroup::Criteria crit;
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;

    
    

    //
    // Create group HelloInterface
    //
    TEST_INFO("Create group for HelloInterface");
    ::PortableGroup::ObjectGroup_var obj_group_HelloInterface;

    try
    {
        obj_group_HelloInterface =  m_group_manager->create_object(rep_id_HelloInterface,
                                                                   crit,
                                                                   factory_creation_id1.out());
        TEST_SUCCEED();
    }   
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        TEST_FAILED();
        throw;
    }
    

    CdmwLBInit::HelloInterface_var hello_group_HelloInterface = CdmwLBInit::HelloInterface::_narrow(obj_group_HelloInterface.in());
    

    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    Cdmw::OsSupport::OS::sleep( timescale * 1000 );
    
    // call the method on an empty object group
    try
    {
        TEST_INFO("Calling hello on HelloInterface on an empty object group...");
        hello_group_HelloInterface->hello();
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

    Cdmw::CommonSvcs::Naming::NamingInterface ni =
    Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");


    ::PortableGroup::Location loc;
    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P11";
    loc[2].kind = "processname";

    std::string s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);

    std::string full_name = "dom3/P11HelloInterface";

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


    CdmwLBInit::HelloInterface_var hello_APPL1_PROC11_INTER1 =  CdmwLBInit::HelloInterface::_narrow(obj.in());
    TEST_CHECK( ! CORBA::is_nil(hello_APPL1_PROC11_INTER1.in()) );


    // add a new member in the object group
    TEST_INFO("Add new member on the group");


    try
    {
        
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL1_PROC11_INTER1.in());
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();  
        std::cerr << e._name() << std::endl;;
    }

    hello_group_HelloInterface = CdmwLBInit::HelloInterface::_narrow(obj_group_HelloInterface.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

    // add new member

    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P21";
    loc[2].kind = "processname";

    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P21HelloInterface";

    // recover the reference of the APPL1/PROC11/HelloInterface object
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

    CdmwLBInit::HelloInterface_var hello_APPL2_PROC21_INTER1 =  CdmwLBInit::HelloInterface::_narrow(obj.in());
    TEST_CHECK( ! CORBA::is_nil(hello_APPL2_PROC21_INTER1.in()) );

    // add a new member in the object group
    // (REQ 261)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL2_PROC21_INTER1.in());
    hello_group_HelloInterface = CdmwLBInit::HelloInterface::_narrow(obj_group_HelloInterface.in());
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

    TEST_INFO("get client information");
    Cdmw::Test::process_info_var hello_APPL1_PROC11_INTER1_info = hello_APPL1_PROC11_INTER1->who_is_called();
    Cdmw::Test::process_info_var hello_APPL2_PROC21_INTER1_info = hello_APPL2_PROC21_INTER1->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL2_PROC21_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    std::cout<<"Press any key and Enter"<<std::endl;
    std::string dummy;
    std::cin >> dummy;

    // call the method 8 times on an object group with default=LoadBalancing Policy
    // and get the group version
    //(REQ 257 - 258)
    int version_HelloInterface = 0;
    
    try 
    {
        CORBA::Long id = 0;
        TEST_INFO("Calling hello on HelloInterface...");
        for(int i = 0; i < 8; i++)
        {
            TEST_INFO ("Request..." << i);
            Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
        
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            TEST_INFO ("Checking Round Robin policy");
            if ((temp_info->processID == id) && (!(i==5)))
                TEST_FAILED();
            else
            {
                id = temp_info->processID;
                TEST_SUCCEED();
            }

            
        }
        
    }
    catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }

    version_HelloInterface = TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in());

    // add new member

    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P12";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P12HelloInterface";


    // recover the reference of the APPL1/PROC12/HelloInterface object
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

    CdmwLBInit::HelloInterface_var hello_APPL1_PROC12_INTER1 =  CdmwLBInit::HelloInterface::_narrow(obj.in());
    Cdmw::Test::process_info_var hello_APPL1_PROC12_INTER1_info = hello_APPL1_PROC12_INTER1->who_is_called();

    // add a new member in the object group
    // (REQ 261)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL1_PROC12_INTER1.in());
    hello_group_HelloInterface = CdmwLBInit::HelloInterface::_narrow(obj_group_HelloInterface.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    // add new member

    loc.length(3);
    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL3";
    loc[1].kind = "applicationname";
    loc[2].id = "P31";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P31HelloInterface";


    // recover the reference of the APPL3/PROC31/HelloInterface object
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

    CdmwLBInit::HelloInterface_var hello_APPL3_PROC31_INTER1 =  CdmwLBInit::HelloInterface::_narrow(obj.in());

    TEST_INFO("get client information");
    Cdmw::Test::process_info_var hello_APPL3_PROC31_INTER1_info = hello_APPL3_PROC31_INTER1->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |         ---------P12------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC12_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface------Replica 3 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC21_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface------Replica 4 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;



    // add a new member in the object group
    // (REQ 261)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL3_PROC31_INTER1.in());
    hello_group_HelloInterface = CdmwLBInit::HelloInterface::_narrow(obj_group_HelloInterface.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

    // call the method on an object group with the new member
    // and get the group version
    // the group version must be higher than the previous group version stored
    //(REQ 259)
    try
    {
        CORBA::Long id = 0;
        TEST_INFO("Calling hello on HelloInterface...");
        for(int i = 0; i < 5; i++)
        {
            TEST_INFO ("Request..." << i);

            Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            TEST_INFO ("Checking Round Robin policy");
            if ((temp_info->processID == id) && (!(i==5)))
		TEST_FAILED();
            else
            {
		id = temp_info->processID;
		TEST_SUCCEED();
            }
	    TEST_INFO("check the version");
        }
        if ( version_HelloInterface < TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in()))
        {
            version_HelloInterface = TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in());
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
   
    ::PortableGroup::ObjectGroupId object_group_id = 0;
    CdmwLBInit::HelloInterface_var temp;
    std::cout<<" Calling the get_object_group_id on the group manager"<<std::endl;
    try
    {
        object_group_id = m_group_manager->get_object_group_id(hello_group_HelloInterface.in());
        TEST_SUCCEED();
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        TEST_FAILED();
    }
    
    // use group manager method to recover the most recent object group reference
    TEST_INFO("Calling the hello interface from the reference coming from the group manager (get_object_group_ref_from_gid)");
    
    try
    {
        ::PortableGroup::ObjectGroup_var object_group = m_group_manager->get_object_group_ref_from_id(object_group_id);
        temp = CdmwLBInit::HelloInterface::_narrow(object_group.in());
        CORBA::Long id = 0;
        TEST_INFO("Calling hello on HelloInterface...");
        for(int i = 0; i < 5; i++)
        {
            TEST_INFO ("Request..." << i);
          
            Cdmw::Test::process_info_var temp_info = temp->who_is_called();
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            TEST_INFO ("Checking Round Robin policy");

	    if ((temp_info->processID == id) && (!(i==5)))
		TEST_FAILED();
            else
            {
                id = temp_info->processID;
                TEST_SUCCEED();
            }
        }
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;
        TEST_FAILED();
    }

    TEST_INFO("Calling the hello interface from the reference coming from the group manager (get_object_group_ref)");

    try
    {
        ::PortableGroup::ObjectGroup_var object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());
        temp = CdmwLBInit::HelloInterface::_narrow(object_group.in());
    CORBA::Long id = 0;
        TEST_INFO("Calling hello on HelloInterface...");
        for(int i = 0; i < 5; i++)
        {
            TEST_INFO ("Request..." << i);
          
            Cdmw::Test::process_info_var temp_info = temp->who_is_called();
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            TEST_INFO ("Checking Round Robin policy");

            if ((temp_info->processID == id) && (!(i==5)))
		TEST_FAILED();
            else
            {
                id = temp_info->processID;
                TEST_SUCCEED();
            }
        }    
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        TEST_FAILED();
    }

     // add new member

    loc.length(3);
    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P22";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P22HelloInterface";


    // recover the reference of the APPL2/PROC22/HelloInterface object
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

    CdmwLBInit::HelloInterface_var hello_APPL2_PROC22_INTER1 =  CdmwLBInit::HelloInterface::_narrow(obj.in());

    Cdmw::Test::process_info_var hello_APPL2_PROC22_INTER1_info = hello_APPL2_PROC22_INTER1->who_is_called();
    // add a new member in the object group
    // (REQ 261)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL2_PROC22_INTER1.in());
    hello_group_HelloInterface = CdmwLBInit::HelloInterface::_narrow(obj_group_HelloInterface.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );

  // add new member

    loc.length(3);
    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL3";
    loc[1].kind = "applicationname";
    loc[2].id = "P32";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P32HelloInterface";


    // recover the reference of the APPL2/PROC22/HelloInterface object
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

    CdmwLBInit::HelloInterface_var hello_APPL3_PROC32_INTER1 =  CdmwLBInit::HelloInterface::_narrow(obj.in());

    TEST_INFO("get client information");
    Cdmw::Test::process_info_var hello_APPL3_PROC32_INTER1_info = hello_APPL3_PROC32_INTER1->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |        |                                          PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |        ----------P12------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC12_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface------Replica 3 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER1_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface------Replica 4 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface------Replica 5 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"     |         ---------P32------HelloInterface------Replica 6 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL3_PROC32_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    


    // add a new member in the object group
    // (REQ 261)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL3_PROC32_INTER1.in());
    hello_group_HelloInterface = CdmwLBInit::HelloInterface::_narrow(obj_group_HelloInterface.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );


    // use replication manager method to recover the next object group reference
    // (REQ 259 - 261)
    TEST_INFO("use group manager method to recover the next object group reference");
    try
    {
        if ( Cdmw::LB::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in()) >
             Cdmw::LB::TestUtils::get_object_group_version(m_orb.in(), temp.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        ::PortableGroup::ObjectGroup_var object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());
        temp = CdmwLBInit::HelloInterface::_narrow(object_group.in());

        if ( Cdmw::LB::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in()) ==
             Cdmw::LB::TestUtils::get_object_group_version(m_orb.in(), temp.in()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        CORBA::Long id = 0;
        TEST_INFO("Calling hello on HelloInterface...");
        for(int i = 0; i < 8; i++)
        {
            TEST_INFO ("Request..." << i);
            Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface->who_is_called();

            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            TEST_INFO ("Checking Round Robin policy");

            if ((temp_info->processID == id) && (!(i==5)))
		TEST_FAILED();
            else
            {
                id = temp_info->processID;
                TEST_SUCCEED();
            }
        }
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        TEST_FAILED();
    }

    int helloInterface_version = Cdmw::LB::TestUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in());

    
    //In case of server crash after a request dispatching,
    //CARDAMOM shall be able to send again the same request
    //to one of the remaining available servers (REQ 262)
    
    TEST_INFO("Test the crash of a replica...");
    TEST_INFO("kill process P11");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    
    TEST_INFO("Calling hello on HelloInterface...");
    try
    {   
        
        for (int i = 0; i<10; i++)
        {
            TEST_INFO ("Request..." << i);
            Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
            
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            TEST_INFO ("Checking Round Robin policy");
            if (temp_info->processID == hello_APPL1_PROC11_INTER1_info->processID)
                    TEST_FAILED();
            else
                    TEST_SUCCEED();
        }
        
    }catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }
    
    
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    TEST_INFO("Check the good version of the object group");
    int object_group_version = m_group_manager->get_object_group_version_from_ref(hello_group_HelloInterface.in());
    if ( helloInterface_version <  object_group_version)
    {
        helloInterface_version = object_group_version;
        TEST_SUCCEED();
    }
    else 
        TEST_FAILED();

    TEST_INFO("calling all object without the object group ref");
    Cdmw::Test::process_info_var hello_APPL1_PROC12_INTER2_info = hello_APPL1_PROC12_INTER1->who_is_called();
    Cdmw::Test::process_info_var hello_APPL2_PROC21_INTER2_info = hello_APPL2_PROC21_INTER1->who_is_called();
    Cdmw::Test::process_info_var hello_APPL2_PROC22_INTER2_info = hello_APPL2_PROC22_INTER1->who_is_called();
    Cdmw::Test::process_info_var hello_APPL3_PROC31_INTER2_info = hello_APPL3_PROC31_INTER1->who_is_called();
    Cdmw::Test::process_info_var hello_APPL3_PROC32_INTER2_info = hello_APPL3_PROC32_INTER1->who_is_called();
    

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P12------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC12_INTER2_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface------Replica 3 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER2_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface------Replica 4 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL3_PROC31_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P32------HelloInterface------Replica 5 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL3_PROC32_INTER2_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    TEST_INFO("Check the good number of member in each object group");
    ::PortableGroup::ObjectGroup_var object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());    
    std::cout<<"get_number_of_member "<< Cdmw::LB::TestUtils::get_number_of_member(m_orb.in(), object_group.in())<<std::endl;
    if ( Cdmw::LB::TestUtils::get_number_of_member(m_orb.in(), object_group.in()) == 5)
        TEST_SUCCEED();
    else
        TEST_FAILED();


    TEST_INFO("Test the recovering of a replica...");
    TEST_INFO("re-start process P11");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

  // add new member

    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P11";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P11HelloInterface";


    // recover the reference of the APPL1/PROC11/HelloInterface object
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

    hello_APPL1_PROC11_INTER1 =  CdmwLBInit::HelloInterface::_narrow(obj.in());

    TEST_INFO("get client information");
    hello_APPL1_PROC11_INTER1_info = hello_APPL1_PROC11_INTER1->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |        |                                          PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |        ----------P12------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC12_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface------Replica 3 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER1_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface------Replica 4 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface------Replica 5 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"     |         ---------P32------HelloInterface------Replica 6 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL3_PROC32_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    
    

    // add a new member in the object group
    // (REQ 261)
    TEST_INFO("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL1_PROC11_INTER1.in());
    TEST_SUCCEED();
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );


    
    TEST_INFO("Calling hello on HelloInterface...the LBGroupManager should update the IOGR with the re-started replica");
    try
    {   
      CORBA::Long id = 0;
        for (int i = 0; i<10; i++)
        {
            TEST_INFO ("Request..." << i);
            Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
            if (temp_info->processID == hello_APPL1_PROC11_INTER1_info->processID)
	      std::cout<< "[RE-STARTED REPLICA]==>" << "the process called is the "<<temp_info->name<<std::endl;
	    else
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            TEST_INFO ("Checking Round Robin policy");
            if ((temp_info->processID == id) && (!(i==5)))
		TEST_FAILED();
            else
            {
                id = temp_info->processID;
                TEST_SUCCEED();
            }
        }
        
    }catch (const CORBA::SystemException& e)
    {
        TEST_FAILED();
    }
    
    Cdmw::OsSupport::OS::sleep( timescale * 1500 );
    TEST_INFO("Check the good version of the object group");
    object_group_version = m_group_manager->get_object_group_version_from_ref(hello_group_HelloInterface.in());
    if ( helloInterface_version <  object_group_version)
    {
        helloInterface_version = object_group_version;
        TEST_SUCCEED();
    }
    else 
        TEST_FAILED();

    TEST_INFO("Check the good number of member in each object group");
    object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());    
    std::cout<<"get_number_of_member "<< Cdmw::LB::TestUtils::get_number_of_member(m_orb.in(), object_group.in())<<std::endl;
    if ( Cdmw::LB::TestUtils::get_number_of_member(m_orb.in(), object_group.in()) == 6)
        TEST_SUCCEED();
    else
        TEST_FAILED();
    
    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    TEST_INFO("kill the application APPL1");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    Cdmw::OsSupport::OS::sleep( timescale * 2000 );

    std::cout<<"NEW SYSTEM"<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL2------P21------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER2_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface------Replica 3 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL3_PROC31_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P32------HelloInterface------Replica 4 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL3_PROC32_INTER2_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    
    try
    {
        CORBA::Long id = 0;
        TEST_INFO("Calling hello on HelloInterface...");
        for(int i = 0; i < 8; i++)
        {
            TEST_INFO ("Request..." << i);
            
            Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            TEST_INFO ("Checking Round Robin policy");
            if (temp_info->processID == hello_APPL1_PROC12_INTER2_info->processID)
                TEST_FAILED();
            else
            {
                id = temp_info->processID;
                TEST_SUCCEED();
            }
        }
    }
    catch (const CORBA::SystemException& e)
        {
            TEST_FAILED();
        }
        
    object_group_version = m_group_manager->get_object_group_version_from_ref(hello_group_HelloInterface.in());
    
    TEST_INFO("Check the good version of each object group");
        
    std::cout<<"helloInterface_version :"<<helloInterface_version<<std::endl;
    std::cout<<" NEW object_group_version :"
             << object_group_version << std::endl;
        
    if ( helloInterface_version < object_group_version )
        {
            helloInterface_version = object_group_version;
            TEST_SUCCEED();
        }
    else 
        TEST_FAILED();
    
    TEST_INFO("Check the good number of member in each object group");
    object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());
    std::cout<<"get_number_of_member "<< Cdmw::LB::TestUtils::get_number_of_member(m_orb.in(), object_group.in())<<std::endl;
    if ( Cdmw::LB::TestUtils::get_number_of_member(m_orb.in(), object_group.in()) == 4)
        TEST_SUCCEED();
    else
        TEST_FAILED();

    Cdmw::OsSupport::OS::sleep( timescale * 2000 );

    
    TEST_INFO("kill the daemon on the host3");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin>>dummy;
    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    
    std::cout<<"NEW SYSTEM"<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL2------P21------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL2_PROC22_INTER2_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    TEST_INFO("Check the good version of each object group");
    std::cout<<"helloInterface_version :"<<helloInterface_version<<std::endl;
    object_group_version = m_group_manager->get_object_group_version_from_ref(hello_group_HelloInterface.in());
    std::cout<<" NEW object_group_version :"<< object_group_version  <<std::endl;
    
    if ( helloInterface_version < object_group_version )
        TEST_SUCCEED();
    
    else 
        TEST_FAILED();
    TEST_INFO("Check the good number of member in each object group");

    object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());
    std::cout<<"get_number_of_member "<< Cdmw::LB::TestUtils::get_number_of_member(m_orb.in(), object_group.in())<<std::endl;
    if ( Cdmw::LB::TestUtils::get_number_of_member(m_orb.in(), object_group.in()) == 2)
        TEST_SUCCEED();
    else
        TEST_FAILED();
    
    TEST_INFO("Calling hello on HelloInterface...");
     try{

       for (int i = 0; i<10; i++)
	
       {
	    Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
	    TEST_INFO ("Request..." << i);
	    
	    std::cout<<"the process called is the "<<temp_info->name<<std::endl;
	    TEST_INFO ("Checking Round Robin policy");
	    if ((temp_info->processID == hello_APPL3_PROC31_INTER2_info->processID) ||
		(temp_info->processID == hello_APPL3_PROC32_INTER2_info->processID))
		TEST_FAILED();
	    else
	    {
		TEST_SUCCEED();
	    }
       }
	}
	catch (const CORBA::SystemException& e)
	{
	    TEST_FAILED();
	}
   
    
    // remove the menber previously deleted

    TEST_INFO("Remove a member P32 already killed from the helloInterface (Member Not Found Exception should be raised)");
    // set_primary on the first member
    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL3";
    loc[1].kind = "applicationname";
    loc[2].id = "P32";
    loc[2].kind = "processname";
    try
    {
        obj_group_HelloInterface = m_group_manager->remove_member(hello_group_HelloInterface.in(),
                                                                  loc);
        TEST_FAILED();
    }
    catch( const PortableGroup::MemberNotFound& e )
    {
        TEST_INFO("Exception MemberNotFound Raised");
        TEST_SUCCEED();
    }

    
    TEST_INFO("kill the Process P21");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin>>dummy;
    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    
    try
    {
      TEST_INFO("Calling hello on HelloInterface...");
      for(int i = 0; i < 8; i++)
        {
            TEST_INFO ("Request..." << i);
	    Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
	    std::cout<<"the process called is the "<<temp_info->name<<std::endl;
	    TEST_INFO ("Checking Round Robin policy...there is only P22 alive in the object group");
	    if (temp_info->processID ==  hello_APPL2_PROC22_INTER2_info->processID) 
	      TEST_SUCCEED();
	    else
	      TEST_FAILED();
	}
    }
    catch (const CORBA::SystemException& e)
    {
      std::cout << e._name() << std::endl;
      TEST_FAILED();
    }


    // remove the object group
    TEST_INFO("Delete the object group helloInterface");
    try
    {
        m_group_manager->delete_object(factory_creation_id1.in());    
        TEST_SUCCEED();
    }
    
    catch( const CORBA::Exception& e )
    {
        TEST_FAILED();
    } 


    TEST_INFO("kill the Group Manager Supervision Observer");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin>>dummy;
    Cdmw::OsSupport::OS::sleep( timescale * 2000 );
    try
    {
        
        TEST_INFO("Calling hello on HelloInterface...");
        for(int i = 0; i < 8; i++)
        {
            TEST_INFO ("Request..." << i);
            
            Cdmw::Test::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
	    TEST_INFO ("Checking Round Robin policy...there is only P22 alive in the object group");
	    if (temp_info->processID ==  hello_APPL2_PROC22_INTER2_info->processID) 
	      TEST_SUCCEED();
	    else
	      TEST_FAILED();   
	}
    }
    catch (const CORBA::SystemException& e)
        {
            TEST_FAILED();
        }


}


} // end namespace LB

} // end namespace Cdmw

