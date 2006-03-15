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


#include <exception>
#include <iostream>
#include <sstream>


#include "idllib/CdmwFTReplicationManager.stub.hpp"
#include "testftlocationmanager/TestSync.stub.hpp"

#include "testftlocationmanager/TestHello_impl.hpp"
#include "testftlocationmanager/TestFTActivationHandler_impl.hpp"

#include "testftlocationmanager/TestLocationManager.hpp"
#include "testftlocationmanager/TestClientThread.hpp"


namespace Cdmw {
namespace FT {

// constructor
TestLocationManager::TestLocationManager(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name,
    const std::string& process_name )
    : Testable( name ),
      m_process_name(process_name)
      
    
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

//destructor
TestLocationManager::~TestLocationManager()
{
}

// do_tests
void TestLocationManager::do_tests()
{
    set_nbOfRequestedTestOK (160);

    try
    {
    
        // STEP 1 : TestLocationManager : creation of the environment : properties, create_object, create 2 processes.
        //          TestClientThread : creation of the location manager, add_member (two differents type of object)
        //          TestLocationManager : wait the creation of the two previous process
        //          TestClientThead : get information about members, primary, group version, etc (with objectgroupref and objectgroupID)
        //          TestLocationManager : recover the information and check if it's rigth

        // STEP 2 : TestLocationManager : add a new member (on the main process) in the first group
        //          TestClientThread : get information about members, primary, group version, etc (with objectgroupref and objectgroupID)
        //          TestLocationManager : recover the information and check if it's rigth

        // STEP 3 : TestLocationManager : set the member of the process 1 as primary in the first group
        //          TestClientThread : get information about members, primary, group version, activation,
        //                             and update_primary etc (with objectgroupref and objectgroupID)
        //          TestLocationManager : recover the information and check if it's rigth

        // STEP 4 : TestLocationManager : set the member of the process 2 as primary in the first group
        //          TestClientThread : get information about members, primary, group version, activation,
        //                             and update_primary etc (with objectgroupref and objectgroupID)
        //          TestLocationManager : recover the information and check if it's rigth

        // STEP 5 : TestLocationManager : check if the second group is not affected by changing first group primary
        //          TestClientThread : get information about members, primary, group version, activation,
        //                             and update_primary, etc (with objectgroupref and objectgroupID)
        //          TestLocationManager : recover the information and check if it's rigth

        // STEP 6 : TestLocationManager : remove the primary member from the first object group 
        //                                (check if the second group is not affected)
        //          TestClientThread : get information about the second group, primary, group version, activation,
        //                             and update_primary, etc (with objectgroupref and objectgroupID)
        //          TestLocationManager : recover the information and check if it's rigth

        // STEP 7 : TestClientThread : get information about the remove members of first group, primary, group version, activation,
        //                             and update_primary, etc (with objectgroupref and objectgroupID)
        //          TestLocationManager : recover the information and check if it's rigth

        // STEP 8 : TestLocationManager : get the groupinfo from the replication manager (after the remove object)
        //                                and call directly the update_group_view method
                
        // STEP 9 : Check API errors
        //






        //
        // STEP 1 : Creation of the environment and check if all informations are the same for all the threads
        //
        
        std::string iorfile1 = "SynchAPPL1.ior";
        std::string iorfile2 = "SynchAPPL2.ior";
        
        
        // first delete the ior files
        try
        {
            OsSupport::OS::unlink (iorfile1);	
        }
        catch (...)
        {
        }

        try
        {
            OsSupport::OS::unlink (iorfile2);	
        }
        catch (...)
        {
        }
        
        
        
        TEST_INFO( "Create the Replication Manager" );

        CORBA::Object_var objref = m_orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());

	if (CORBA::is_nil(rm.in()))
	{
	    std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	              << "could not _narrow object to type HelloInterface" << std::endl;
	    abort();
        }

        TEST_SUCCEED();

        // set default properties
        ::FT::Location loc1;
        loc1.length(3);
        loc1[0].id = "HOST1";
        loc1[0].kind = "hostname";
        
        loc1[1].id = "APPL1";
        loc1[1].kind = "application";
        loc1[2].id = "PROC1";
        loc1[2].kind = "process";    
        
        ::FT::Location loc2;
        loc2.length(3);
        loc2[0].id = "HOST2";
        loc2[0].kind = "hostname";
        
        loc2[1].id = "APPL2";
        loc2[1].kind = "application";
        loc2[2].id = "PROC2";
        loc2[2].kind = "process";    
        
        ::FT::Location loc3;
        loc3.length(3);
        loc3[0].id = "HOST3";
        loc3[0].kind = "hostname";
        
        loc3[1].id = "APPL3";
        loc3[1].kind = "application";
        loc3[2].id = "PROC3";
        loc3[2].kind = "process";  
        
        ::FT::Location loc4;
        loc4.length(3);
        loc4[0].id = "HOST4";
        loc4[0].kind = "hostname";
        loc4[1].id = "APPL4";
        loc4[1].kind = "application";
        loc4[2].id = "PROC4";
        loc4[2].kind = "process";  
        
        TEST_INFO( "set_default_properties" );
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
        prop[3].nam[0].id="org.omg.ft.Factories";
        
        ::FT::FactoryInfos factoryInfos;
        factoryInfos.length(3);
        factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
        factoryInfos[0].the_location = loc1;
        ::FT::Criteria factoryCrit;        
        factoryCrit.length(0);
        factoryInfos[0].the_criteria = factoryCrit;
        
        factoryInfos[1].the_factory = ::FT::GenericFactory::_nil();
        factoryInfos[1].the_location = loc2;
        factoryInfos[1].the_criteria = factoryCrit;
        
        factoryInfos[2].the_factory = ::FT::GenericFactory::_nil();
        factoryInfos[2].the_location = loc3;
        factoryInfos[2].the_criteria = factoryCrit;
        
        prop[3].val <<= factoryInfos;
        
        
        ::FT::Criteria ft_criteria1;       
        ft_criteria1.length(1);
        ft_criteria1[0].nam.length(1);
        ft_criteria1[0].nam[0].id="org.omg.ft.FTProperties";
        ft_criteria1[0].val <<= prop;

        TEST_SUCCEED();
        
        // create a first object with an invalid criteria
        // the invalid criteria is the FaultMonitoringStyle 
        TEST_INFO("Create a first object group");

        const char * rid1 = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface1:1.0";
        const char * rid2 = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface2:1.0";

        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id2;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id3;
        CORBA::Object_var obj1;
        CORBA::Object_var obj2;
        CORBA::Object_var obj3;
        CORBA::Object_var obj4 = CORBA::Object::_nil();
        
        try
        {
            obj1 =  rm->create_object(rid1,
                                      ft_criteria1,
                                      factory_creation_id1.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        
        try
        {
            obj2 =  rm->create_object(rid2,
                                      ft_criteria1,
                                      factory_creation_id2.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }



        ::FT::ObjectGroupId ogi1 = 0;
        try
        {
            ogi1 = rm->get_object_group_id(obj1.in());
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        
        ::FT::ObjectGroupId ogi2 = 0;
        try
        {
            ogi2 = rm->get_object_group_id(obj2.in());
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }


        TEST_INFO("start clients");
        std::ostringstream appl1;
        std::ostringstream appl2;
        appl1<<"--client APPL1 --object_group_id1="<<ogi1<<" --object_group_id2="<<ogi2;
        appl2<<"--client APPL2 --object_group_id1="<<ogi1<<" --object_group_id2="<<ogi2;

        OsSupport::OS::ProcessId pid1  = 
        OsSupport::OS::create_process( m_process_name, appl1.str());
        sleep(2);
        
        OsSupport::OS::ProcessId pid2  = 
        OsSupport::OS::create_process( m_process_name, appl2.str());   
        sleep(4);
        TEST_SUCCEED();

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*1000);
        

        
        // wait creation of ior files
        while (!(OsSupport::OS::file_exists(iorfile1) && OsSupport::OS::file_exists(iorfile2)))
        {
            OsSupport::OS::sleep(timescale*1000);
        }

        std::string file_url_prefix = "file://";
        
        std::string file_url = file_url_prefix;
        file_url += iorfile1;
        
        CORBA::Object_var object_synch1 = m_orb->string_to_object(file_url.c_str());
        CdmwLocationManager::Synch_var synchAPPL1 = CdmwLocationManager::Synch::_narrow(object_synch1.in());

        file_url = file_url_prefix;
        file_url += iorfile2;
        
        CORBA::Object_var object_synch2 = m_orb->string_to_object(file_url.c_str());
        CdmwLocationManager::Synch_var synchAPPL2 = CdmwLocationManager::Synch::_narrow(object_synch2.in());
      

        TEST_SUCCEED();

        TEST_INFO("client started")
        
        TEST_INFO("waiting end of initialisation of clients");
		
        OsSupport::OS::sleep(timescale*1000);

        //WAIT_THREADS ( flag )
        // wait the end of the initialisation
        bool flag = false;
        {
            flag = false;
            for(int i=0; i <10; i++) 
            { 
                Cdmw::OsSupport::OS::sleep(1000); 
                if (synchAPPL1->get_OK() && synchAPPL2->get_OK() ) { flag = true; break; } 
            };
            TEST_CHECK ( flag );
        }
        
        TEST_INFO("end of initialisation received from clients");
        
        TEST_SUCCEED();
        
        TEST_INFO("");
        TEST_INFO("<< STEP 1 >>");
        TEST_INFO("continue 1 send to clients");
        
        synchAPPL1->set_OK(false);
        synchAPPL2->set_OK(false);
        
        OsSupport::OS::sleep(timescale*1000);

        //WAIT_THREADS ( flag )
        
        {
            flag = false;
            for(int i=0; i <10; i++) 
            { 
                Cdmw::OsSupport::OS::sleep(1000); 
                if (synchAPPL1->get_OK() && synchAPPL2->get_OK() ) { flag = true; break; } 
            };
            TEST_CHECK ( flag );
        }
        
        TEST_INFO("synchro 1 received from clients");
        
        TEST_SUCCEED();
        
        // for group 1
        // check the equivalence between the result of the two threads and the replication manager
        TEST_INFO("start get_object_group on group 1");
        objref = rm->get_object_group_ref(obj1.in());
        
        // check the equivalence for the get_object_group method
        if (objref->_is_equivalent(synchAPPL1->get_result_get_object_group()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (objref->_is_equivalent(synchAPPL2->get_result_get_object_group()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        // check the equivalence for the get_object_group_from_gid method
        if (objref->_is_equivalent(synchAPPL1->get_result_get_object_group_from_gid()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (objref->_is_equivalent(synchAPPL2->get_result_get_object_group_from_gid()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        TEST_INFO("end get_object_group");

        
        // check the equivalence for the get_object_group_version method

        ::FT::ObjectGroupRefVersion version = rm->get_object_group_version_from_ref(objref.in());

        
        if (version == synchAPPL1->get_result_get_object_group_version())
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (version == synchAPPL2->get_result_get_object_group_version())
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        
        // check the equivalence for the get_other_group_members method
        ::CdmwFT::GroupInfo_var group_info = rm->get_object_group_info(objref.in());
        ::CdmwFT::MemberInfos_var members1 = synchAPPL1->get_result_get_other_group_members();
        ::CdmwFT::MemberInfos_var members2 = synchAPPL2->get_result_get_other_group_members();
        
        CORBA::Long length = members1->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        length = members2->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        

        length = (group_info->members).length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        int res = 0;
        // check the equivalent between the memberInfos from the thread1 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](0).the_reference.in())))
                res++;    
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();
      
        res = 0;
        // check the equivalent between the memberInfos from the thread2 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](0).the_reference.in())))
                res++;
            
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();


        // check the equivalence for the get_other_group_members method
        members1 = synchAPPL1->get_result_get_other_group_members_from_gid();
        members2 = synchAPPL2->get_result_get_other_group_members_from_gid();
        
        length = members1->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        length = members2->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        

        length = (group_info->members).length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        res = 0;
        // check the equivalent between the memberInfos from the thread1 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](0).the_reference.in())))
                res++;    
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();
      
        res = 0;
        // check the equivalent between the memberInfos from the thread2 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](0).the_reference.in())))
                res++;
            
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();





        // check the equivalence for the is_primary method
        TEST_INFO("check the is_primary method");
        TEST_CHECK(synchAPPL1->get_result_is_primary())
        TEST_CHECK(!synchAPPL2->get_result_is_primary())
        TEST_CHECK(synchAPPL1->get_result_is_primary_from_gid())
        TEST_CHECK(!synchAPPL2->get_result_is_primary_from_gid())
       
        
        //
        // STEP 2 : add a new member for the group 1
        //

        TEST_INFO("");
        TEST_INFO("<< STEP 2 >>");
        
        // create a reference for the PrimaryBackupGroupRepository
        ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primaryBackupGroupRepository_impl
          = ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();

		  // export GroupRepository in orb
		  CdmwFT::Location::PrimaryBackupGroupRepository_var primary_backup_rep_ref
			  = primaryBackupGroupRepository_impl;
		  
		  m_orb->register_initial_reference("FTGroupRepository", 
				                              primary_backup_rep_ref.in());

        // create a reference for the PrimaryBackupAdmin
        ::Cdmw::FT::Location::PrimaryBackupAdmin_impl * primaryBackupAdmin_impl
          = new ::Cdmw::FT::Location::PrimaryBackupAdmin_impl(m_orb.in(), m_rootPOA.in(), loc3, primaryBackupGroupRepository_impl);
        
        CdmwFT::Location::PrimaryBackupAdmin_var primaryBackupAdmin = primaryBackupAdmin_impl->_this();
        
        // register the Member Admin (the PrimaryBackupAdmin) on the Replication Manager
        rm->register_location(loc3, primaryBackupAdmin.in());

        Cdmw::HelloInterface1_impl * helloInterface1_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface1 =  helloInterface1_impl->_this();


        // Add new member on the group
        TEST_INFO("get the reference of the object group 1");
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // Add new member on the group
        TEST_INFO("add a new member in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc3,
                                  helloInterface1.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // Add new member on the group
        Cdmw::HelloInterface2_impl * helloInterface2_impl = new Cdmw::HelloInterface2_impl();
        CdmwReplicationManager::HelloInterface2_var helloInterface2 =  helloInterface2_impl->_this();

        TEST_INFO("get the reference of the object group 2");
        try
        {
            obj2 = rm->get_object_group_ref(obj2.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // Add new member on the group
        TEST_INFO("add a new member in group 2");
        try
        {
            obj2 = rm->add_member(obj2.in(),
                                  loc3,
                                  helloInterface2.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }


        
        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("continue 2 send to clients");
        
        synchAPPL1->set_OK(false);
        synchAPPL2->set_OK(false);


        //WAIT_THREADS ( flag )
        {
            flag = false;
            for(int i=0; i <10; i++) 
            { 
                Cdmw::OsSupport::OS::sleep(1000); 
                if (synchAPPL1->get_OK() && synchAPPL2->get_OK() ) { flag = true; break; } 
            };
            TEST_CHECK ( flag );
        }
        
        TEST_INFO("synchro 2 received from clients");
        
        TEST_SUCCEED();
        
        // for group 1
        // check the equivalence between the result of the two threads and the replication manager
        TEST_INFO("start get_object_group on group 1");
        objref = rm->get_object_group_ref(obj1.in());
        
        // check the equivalence for the get_object_group method
        if (objref->_is_equivalent(synchAPPL1->get_result_get_object_group()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (objref->_is_equivalent(synchAPPL2->get_result_get_object_group()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        // check the equivalence for the get_object_group_from_gid method
        if (objref->_is_equivalent(synchAPPL1->get_result_get_object_group_from_gid()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (objref->_is_equivalent(synchAPPL2->get_result_get_object_group_from_gid()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        TEST_INFO("end get_object_group");

        
        // check the equivalence for the get_object_group_version method

        version++;
        
        if (version == synchAPPL1->get_result_get_object_group_version())
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (version == synchAPPL2->get_result_get_object_group_version())
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        
        // check the equivalence for the get_other_group_members method
        group_info = rm->get_object_group_info(objref.in());
        members1 = synchAPPL1->get_result_get_other_group_members();
        members2 = synchAPPL2->get_result_get_other_group_members();
        
        length = members1->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        length = members2->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        

        length = (group_info->members).length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 3)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        res = 0;
        // check the equivalent between the memberInfos from the thread1 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](0).the_reference.in())))
                res++;    
            if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](1).the_reference.in())))
                res++;    
        }
        if (res == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();
      
        res = 0;
        // check the equivalent between the memberInfos from the thread2 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](0).the_reference.in())))
                res++;
            if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](1).the_reference.in())))
                res++;
        }
        if (res == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();


        // check the equivalence for the get_other_group_members method
        members1 = synchAPPL1->get_result_get_other_group_members_from_gid();
        members2 = synchAPPL2->get_result_get_other_group_members_from_gid();
        
        length = members1->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        length = members2->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        

        length = (group_info->members).length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 3)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        res = 0;
        // check the equivalent between the memberInfos from the thread1 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](0).the_reference.in())))
                res++;    
            if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](1).the_reference.in())))
                res++;    
        }
        if (res == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();
      
        res = 0;
        // check the equivalent between the memberInfos from the thread2 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](0).the_reference.in())))
                res++;
            if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](1).the_reference.in())))
                res++;            
        }
        if (res == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();





        // check the equivalence for the is_primary method
        TEST_INFO("check the is_primary method");
        TEST_CHECK(synchAPPL1->get_result_is_primary())
        TEST_CHECK(!synchAPPL2->get_result_is_primary())
        TEST_CHECK(synchAPPL1->get_result_is_primary_from_gid())
        TEST_CHECK(!synchAPPL2->get_result_is_primary_from_gid())
       
        
        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("continue 2-1 send to clients");
        
        synchAPPL1->set_OK(false);
        synchAPPL2->set_OK(false);
        

        //WAIT_THREADS ( flag )
        {
            flag = false;
            for(int i=0; i <10; i++) 
            { 
                Cdmw::OsSupport::OS::sleep(1000);
                if (synchAPPL1->get_OK() && synchAPPL2->get_OK() ) { flag = true; break; } 
            };
            TEST_CHECK ( flag );
        }
        
        TEST_INFO("synchro 2-1 received from clients");
        
        TEST_SUCCEED();
        

        //
        // STEP 3 : set the first process as primary for group 1
        //

        TEST_INFO("");
        TEST_INFO("<< STEP 3 >>");
/*
        // set the first process as primary
        TEST_INFO("Set primary the first member of group 1");
        
        try
        {
            obj1 =  rm->set_primary_member(obj1.in(),
                                           loc1);
            
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
*/
 
        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("continue 3 send to clients")
        
        synchAPPL1->set_OK(false);
        synchAPPL2->set_OK(false);
        
        //WAIT_THREADS ( flag )
        {
            flag = false;
            for(int i=0; i <10; i++) 
            { 
                Cdmw::OsSupport::OS::sleep(1000); 
                std::cout<<i;
                if (synchAPPL1->get_OK() && synchAPPL2->get_OK() ) { flag = true; break; } 
            };
            TEST_CHECK ( flag );
        }
        
        TEST_INFO("synchro 3 received from clients");
        
        TEST_SUCCEED();
        

        TEST_INFO("check the first process is primary for group 1");
        TEST_CHECK(synchAPPL1->get_result_is_primary())
        TEST_CHECK(!synchAPPL2->get_result_is_primary())
        TEST_CHECK(synchAPPL1->get_result_is_primary_from_gid())
        TEST_CHECK(!synchAPPL2->get_result_is_primary_from_gid())
        
        TEST_INFO("check the activation of the TestFTActivationHandler_impl object");
        TEST_CHECK(synchAPPL1->get_activation())
        TEST_CHECK(!synchAPPL2->get_activation())


        //
        // STEP 4 : set the second process as primary for group 1
        //

        TEST_INFO("");
        TEST_INFO("<< STEP 4 >>");

/*
        // set the second process as primary
        TEST_INFO("Set primary the second member of group 1");
        
        try
        {
            obj1 =  rm->set_primary_member(obj1.in(),
                                           loc2);
            
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
*/      
        TEST_INFO("remove the fisrt member of group 1");
        // This test is equivalent to the old test set_primary not available currently
        // First, remove the primary member => a new one will be elected (the second in the list)
        // Second, add member of the first location in both group because the first location has been 
        // removed automatically (FT at process level) inthe second group.
        try
        {
            obj1 = rm->remove_member(obj1.in(),
                                     loc1);
            TEST_SUCCEED(); 
        }
        catch (const CORBA::Exception& e )
        {
            TEST_INFO( e._name());
            TEST_FAILED();
        }


        // Add new member on the group
        file_url = file_url_prefix;
        file_url += "HelloInterface1_1.ior";
        
        CORBA::Object_var object1_1 = m_orb->string_to_object(file_url.c_str());
        CdmwReplicationManager::HelloInterface1_var helloInterface1_1 =  CdmwReplicationManager::HelloInterface1::_narrow(object1_1.in());
        

        TEST_INFO("get the reference of the object group 1");
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // Add new member on the group
        TEST_INFO("add a new member in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc1,
                                  helloInterface1_1.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }


        // Add new member on the group
        file_url = file_url_prefix;
        file_url += "HelloInterface2_1.ior";
        
        CORBA::Object_var object2_1 = m_orb->string_to_object(file_url.c_str());
        CdmwReplicationManager::HelloInterface2_var helloInterface2_1 =  CdmwReplicationManager::HelloInterface2::_narrow(object2_1.in());

        TEST_INFO("get the reference of the object group 2");
        try
        {
            obj2 = rm->get_object_group_ref(obj2.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // Add new member on the group
        TEST_INFO("add a new member in group 2");
        try
        {
            obj2 = rm->add_member(obj2.in(),
                                  loc1,
                                  helloInterface2_1.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("continue 4 send to clients");
        
        synchAPPL1->set_OK(false);
        synchAPPL2->set_OK(false);
        
        //WAIT_THREADS ( flag )
        {
            flag = false;
            for(int i=0; i <10; i++) 
            { 
                Cdmw::OsSupport::OS::sleep(1000); 
                std::cout<<i;
                if (synchAPPL1->get_OK() && synchAPPL2->get_OK() ) { flag = true; break; } 
            };
            TEST_CHECK ( flag );
        }
        
        TEST_INFO("synchro 4 received from clients");
        
        TEST_SUCCEED();
        
        
        //TEST_INFO("check the is_primary method : second process is primary for group 1");
        // impossible to set a primary in an other location from now!!!!
        TEST_INFO("check the first process is primary for group 1");
        TEST_CHECK(!synchAPPL1->get_result_is_primary())
        TEST_CHECK(synchAPPL2->get_result_is_primary())
        TEST_CHECK(!synchAPPL1->get_result_is_primary_from_gid())
        TEST_CHECK(synchAPPL2->get_result_is_primary_from_gid())
        
        TEST_INFO("check the activation of the TestFTActivationHandler_impl object");
        TEST_CHECK(!synchAPPL1->get_activation())
        TEST_CHECK(synchAPPL2->get_activation())


        //
        // STEP 5 : check if the second group is not changed
        //
        
        TEST_INFO("");
        TEST_INFO("<< STEP 5 >>");

        TEST_INFO("check if the second object is not changed");
        
        
        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("continue 5 send to clients");
        
        synchAPPL1->set_OK(false);
        synchAPPL2->set_OK(false);
    
        //WAIT_THREADS ( flag )
        
        {
            flag = false;
            for(int i=0; i <10; i++) 
            { 
                Cdmw::OsSupport::OS::sleep(1000); 
                if (synchAPPL1->get_OK() && synchAPPL2->get_OK() ) { flag = true; break; } 
            };
            TEST_CHECK ( flag );
        }
        
        TEST_INFO("synchro 5 received from clients");
        
        TEST_SUCCEED();
        
        // for group 2
        // check the equivalence between the result of the two threads and the replication manager
        TEST_INFO("start get_object_group on group 2");
        objref = rm->get_object_group_ref(obj2.in());
        
        // check the equivalence for the get_object_group method
        if (objref->_is_equivalent(synchAPPL1->get_result_get_object_group()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (objref->_is_equivalent(synchAPPL2->get_result_get_object_group()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        // check the equivalence for the get_object_group_from_gid method
        if (objref->_is_equivalent(synchAPPL1->get_result_get_object_group_from_gid()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (objref->_is_equivalent(synchAPPL2->get_result_get_object_group_from_gid()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        TEST_INFO("end get_object_group");

        
        // check the equivalence for the get_object_group_version method

        ::FT::ObjectGroupRefVersion version2 = rm->get_object_group_version_from_ref(objref.in());

        
        if (version2 == synchAPPL1->get_result_get_object_group_version())
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (version2 == synchAPPL2->get_result_get_object_group_version())
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        
        // check the equivalence for the get_other_group_members method
        group_info = rm->get_object_group_info(objref.in());
        members1 = synchAPPL1->get_result_get_other_group_members();
        members2 = synchAPPL2->get_result_get_other_group_members();
        
        length = members1->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        length = members2->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        

        length = (group_info->members).length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 3)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        res = 0;
        // check the equivalent between the memberInfos from the thread1 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](0).the_reference.in())))
                res++;    
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();
      
        res = 0;
        // check the equivalent between the memberInfos from the thread2 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](0).the_reference.in())))
                res++;
            
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();


        // check the equivalence for the get_other_group_members method
        members1 = synchAPPL1->get_result_get_other_group_members_from_gid();
        members2 = synchAPPL2->get_result_get_other_group_members_from_gid();
        
        length = members1->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        length = members2->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        

        length = (group_info->members).length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 3)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        res = 0;
        // check the equivalent between the memberInfos from the thread1 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](0).the_reference.in())))
                res++;    
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();
      
        res = 0;
        // check the equivalent between the memberInfos from the thread2 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](0).the_reference.in())))
                res++;
            
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();





        // check the equivalence for the is_primary method
        TEST_INFO("check the is_primary method");
        TEST_CHECK(!synchAPPL1->get_result_is_primary())
        TEST_CHECK(synchAPPL2->get_result_is_primary())
        TEST_CHECK(!synchAPPL1->get_result_is_primary_from_gid())
        TEST_CHECK(synchAPPL2->get_result_is_primary_from_gid())
       
        

        //
        // STEP 6 : remove the second member of group 1 
        //          check if the group 2 is not changed after the remove in group 1
        //

        TEST_INFO("");
        TEST_INFO("<< STEP 6 >>");

        // Remove a member on the group
        TEST_INFO("get the reference of the object group 1");
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // remove the second member (set as primary) on the group
        TEST_INFO("remove the second member of group 1");
        try
        {
            obj1 = rm->remove_member(obj1.in(),
                                     loc2);
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("continue 6 send to clients");
        
        synchAPPL1->set_OK(false);
        synchAPPL2->set_OK(false);

        //WAIT_THREADS ( flag )
        {
            flag = false;
            for(int i=0; i <10; i++) 
            { 
                Cdmw::OsSupport::OS::sleep(1000); 
                if (synchAPPL1->get_OK() && synchAPPL2->get_OK() ) { flag = true; break; } 
            };
            TEST_CHECK ( flag );
        }
        
        TEST_INFO("synchro 6 received from clients");
        
        TEST_SUCCEED();
        

        TEST_INFO("remove a member of the first group");
        
        // for group 2
        // check the equivalence between the result of the two threads and the replication manager
        TEST_INFO("start get_object_group on group 2");
        objref = rm->get_object_group_ref(obj2.in());

        if (CORBA::is_nil(objref.in()))
        {
           TEST_FAILED();
        }
        else
        {
            ::FT::ObjectGroupId object_group_id2 = rm->get_object_group_id(objref.in());
            std::ostringstream os;
            os << "group 2 id = " << object_group_id2 << " (" << ogi2 << ")" << std::ends;
            TEST_INFO(os.str());

            if (CORBA::is_nil(synchAPPL1->get_result_get_object_group()))
            {
                TEST_FAILED();
            }
            else
            {
                // check the equivalence for the get_object_group method
                ::FT::ObjectGroup_var APPL_objref = synchAPPL1->get_result_get_object_group();
                if (objref->_is_equivalent(APPL_objref.in()))
                    TEST_SUCCEED();
                else
                {
                    try 
                    {
                        object_group_id2 
                            = rm->get_object_group_id(APPL_objref.in());
                         os << "synchAPPL1 group 2 id = " << object_group_id2 << std::ends;
                         TEST_INFO(os.str());
                    }
                    catch (const CORBA::Exception& e)
                    {
                         TEST_INFO(e._name());
                         TEST_FAILED();
                    }
                    TEST_FAILED();
                }
            }
            
            if (CORBA::is_nil(synchAPPL2->get_result_get_object_group()))
            {
                TEST_SUCCEED();
            }
            else
            {
                if (objref->_is_equivalent(synchAPPL2->get_result_get_object_group()))
                    TEST_FAILED();
                else
                    TEST_FAILED();
            }
            
            if (CORBA::is_nil(synchAPPL1->get_result_get_object_group_from_gid()))
            {
                TEST_FAILED();
            }
            else
            {
                // check the equivalence for the get_object_group_from_gid method
                if (objref->_is_equivalent(synchAPPL1->get_result_get_object_group_from_gid()))
                    TEST_SUCCEED();
                else
                    TEST_FAILED();
            }
            
            if (CORBA::is_nil(synchAPPL2->get_result_get_object_group_from_gid()))
            {
                TEST_SUCCEED();
            }
            else
            {
                if (objref->_is_equivalent(synchAPPL2->get_result_get_object_group_from_gid()))
                    TEST_FAILED();
                else
                    TEST_FAILED();
            }
        }
        TEST_INFO("end get_object_group");

        
        // check the equivalence for the get_object_group_version method
        // it must be incremented in APPL1 and set to 0 in APPL2

        if (version2 < synchAPPL1->get_result_get_object_group_version())
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (0 == synchAPPL2->get_result_get_object_group_version())
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        
        TEST_INFO("get_object_group_info");
        // check the equivalence for the get_other_group_members method
        group_info = rm->get_object_group_info(objref.in());
        TEST_INFO("get_result_get_other_group_members");
        members1 = synchAPPL1->get_result_get_other_group_members();
        members2 = synchAPPL2->get_result_get_other_group_members();
        
        TEST_INFO("test nb of members from APPL1");
        length = members1->length();
        std::cout<<"length"<<length<<std::endl;
            
        if (length == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        TEST_INFO("test nb of members from APPL2");
        length = members2->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        TEST_INFO("test nb of members from group_info");
        length = (group_info->members).length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        res = 0;
        if (members1->length() == 0)
        {
            TEST_FAILED();
        }
        else
        {
            // check the equivalent between the memberInfos from the thread1 and the memberInfos from the replication manager
            for (int i =0; i< length; i++)
            {
                if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](0).the_reference.in())))
                    res++;    
            }
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();
      
        // check the equivalence for the get_other_group_members method
        members1 = synchAPPL1->get_result_get_other_group_members_from_gid();
        members2 = synchAPPL2->get_result_get_other_group_members_from_gid();
        
        length = members1->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        length = members2->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        

        length = (group_info->members).length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        res = 0;
        if (members1->length() == 0)
        {
            TEST_FAILED();
        }
        else
        {
            // check the equivalent between the memberInfos from the thread1 and the memberInfos from the replication manager
            for (int i =0; i< length; i++)
            {
                if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](0).the_reference.in())))
                    res++;    
            }
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();
      
        // check the equivalence for the is_primary method
        TEST_INFO("check the is_primary method");
        TEST_CHECK(synchAPPL1->get_result_is_primary())
        TEST_CHECK(!synchAPPL2->get_result_is_primary())
        TEST_CHECK(synchAPPL1->get_result_is_primary_from_gid())
        TEST_CHECK(!synchAPPL2->get_result_is_primary_from_gid())
       



        //
        // STEP 7 : Check Info from group 1 after the object removing
        //

        TEST_INFO("");
        TEST_INFO("<< STEP 7 >>");


        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("continue 7 send to clients");
        
        synchAPPL1->set_OK(false);
        synchAPPL2->set_OK(false);

        //WAIT_THREADS ( flag )
        {
            flag = false;
            for(int i=0; i <10; i++) 
            { 
                Cdmw::OsSupport::OS::sleep(1000); 
                if (synchAPPL1->get_OK() && synchAPPL2->get_OK() ) { flag = true; break; } 
            };
            TEST_CHECK ( flag );
        }
        
        TEST_INFO("synchro 7 received from clients");
        
        TEST_SUCCEED();
        
        // for group 1
        // check the equivalence between the result of the two threads and the replication manager
        TEST_INFO("start get_object_group on group 1");
        objref = rm->get_object_group_ref(obj1.in());
        
        // check the equivalence for the get_object_group method
        if (objref->_is_equivalent(synchAPPL1->get_result_get_object_group()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (CORBA::is_nil(synchAPPL2->get_result_get_object_group()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        // check the equivalence for the get_object_group_from_gid method
        if (objref->_is_equivalent(synchAPPL1->get_result_get_object_group_from_gid()))
            TEST_SUCCEED();
        else
            TEST_FAILED();        

        if (CORBA::is_nil(synchAPPL2->get_result_get_object_group_from_gid()))
            TEST_SUCCEED();
        else
            TEST_FAILED();
        TEST_INFO("end get_object_group");

        
        // check the equivalence for the get_object_group_version method

        version = rm->get_object_group_version_from_ref(objref.in());

        if (version == synchAPPL1->get_result_get_object_group_version())
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        if (0 == synchAPPL2->get_result_get_object_group_version())
            TEST_SUCCEED();
        else
            TEST_FAILED();
        
        std::cout<<version<<"   "<<synchAPPL1->get_result_get_object_group_version()<<"   "<<synchAPPL2->get_result_get_object_group_version()<<std::endl;
        

        // check the equivalence for the get_other_group_members method
        group_info = rm->get_object_group_info(objref.in());
        members1 = synchAPPL1->get_result_get_other_group_members();
        members2 = synchAPPL2->get_result_get_other_group_members();
        
        length = members1->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        length = members2->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        

        length = (group_info->members).length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        res = 0;
        // check the equivalent between the memberInfos from the thread1 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](0).the_reference.in())))
                res++;    
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();

// comment because member->length = 0
//         res = 0;
//         // check the equivalent between the memberInfos from the thread2 and the memberInfos from the replication manager
//         for (int i =0; i< length; i++)
//         {
//             if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](0).the_reference.in())))
//                 res++;
//             if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](1).the_reference.in())))
//                 res++;
//         }
//         if (res == 2)
//             TEST_SUCCEED();
//         else
//             TEST_FAILED();


        // check the equivalence for the get_other_group_members method
        members1 = synchAPPL1->get_result_get_other_group_members_from_gid();
        members2 = synchAPPL2->get_result_get_other_group_members_from_gid();
        
        length = members1->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        length = members2->length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 0)
            TEST_SUCCEED();
        else
            TEST_FAILED();
        

        length = (group_info->members).length();
        std::cout<<"length"<<length<<std::endl;
        
        if (length == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        res = 0;
        // check the equivalent between the memberInfos from the thread1 and the memberInfos from the replication manager
        for (int i =0; i< length; i++)
        {
            if ((group_info->members[i].the_reference)->_is_equivalent((members1->operator[](0).the_reference.in())))
                res++;    
        }
        if (res == 1)
            TEST_SUCCEED();
        else
            TEST_FAILED();
      

        // comment because member->length = 0 
//         res = 0;
//         // check the equivalent between the memberInfos from the thread2 and the memberInfos from the replication manager
//         for (int i =0; i< length; i++)
//         {
//             if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](0).the_reference.in())))
//                 res++;
//             if ((group_info->members[i].the_reference)->_is_equivalent((members2->operator[](1).the_reference.in())))
//                 res++;            
//         }
//         if (res == 2)
//             TEST_SUCCEED();
//         else
//             TEST_FAILED();





        // check the equivalence for the is_primary method
        TEST_INFO("check the is_primary method : first process is primary for group 1");
        sleep(4);
        
        TEST_CHECK(synchAPPL1->get_result_is_primary())
        TEST_CHECK(!synchAPPL2->get_result_is_primary())
        TEST_CHECK(synchAPPL1->get_result_is_primary_from_gid())
        TEST_CHECK(!synchAPPL2->get_result_is_primary_from_gid())


        //
        // STEP 8 : Call Primary Backup Admin
        //
        
        TEST_INFO("");
        TEST_INFO("<< STEP 8 >>");

        TEST_INFO("start get_object_group on group 1");
        
        try
        {
            objref = rm->get_object_group_ref(obj1.in());
            group_info = rm->get_object_group_info(objref.in());
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        CORBA::Object_var object_primary_backup_admin_APPL1 = m_orb->string_to_object("file://pbaAPPL1.ior");
        CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin_APPL1 = CdmwFT::Location::PrimaryBackupAdmin::_narrow(object_primary_backup_admin_APPL1.in());
        TEST_SUCCEED(); 
        CORBA::Object_var object_primary_backup_admin_APPL2 = m_orb->string_to_object("file://pbaAPPL2.ior");
        CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin_APPL2 = CdmwFT::Location::PrimaryBackupAdmin::_narrow(object_primary_backup_admin_APPL2.in());
        TEST_SUCCEED(); 


        // notify an update group view directly by the Primary Backup Admin

        // Process 1 is primary for group 1
        
        try
        {
            primary_backup_admin_APPL1->update_group_view(group_info.in());
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // Process 2 has not any members in group 1
        try
        {
            primary_backup_admin_APPL2->update_group_view(group_info.in());
            TEST_FAILED();
        }
        catch( ::FT::ObjectGroupNotFound& e)
        {
             TEST_SUCCEED();           
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }


/*
        // set the current thread (added as third member) as primary
        TEST_INFO("Set primary the current thread (third member)");
        objref = rm->get_object_group_ref(obj1.in());
        try
        {
            obj1 =  rm->set_primary_member(objref.in(),
                                           loc3);
            
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
*/


        // delete the group 1
        TEST_INFO("Delete group 1");
        try
        {
            rm->delete_object(factory_creation_id1.in());

            TEST_FAILED();
        }
        catch (const CORBA::BAD_INV_ORDER& )
        {
            TEST_INFO("CORBA::BAD_INV_ORDERi exception raised!");
            TEST_SUCCEED(); 
        }
        catch (const CORBA::Exception& e )
        {
            TEST_INFO( e._name());
            TEST_FAILED();
        }

        // Remove a member on the group
        TEST_INFO("get the reference of the object group 1");
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
            TEST_SUCCEED(); 
        }
        catch (const CORBA::Exception& e )
        {
            TEST_INFO( e._name());
            TEST_FAILED();
        }

        TEST_INFO("remove the third member of group 1");
        try
        {
            obj1 = rm->remove_member(obj1.in(),
                                     loc3);
        }
        catch (const CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("remove the first member of group 1");
        try
        {
            obj1 = rm->remove_member(obj1.in(),
                                     loc1);
        }
        catch (const CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // delete the group 2
        TEST_INFO("Delete group 2");
        try
        {
            rm->delete_object(factory_creation_id2.in());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO( "Killing the client1" );
        OsSupport::OS::kill_process( pid1);

        TEST_INFO( "Killing the client2" );
        OsSupport::OS::kill_process( pid2);


        /////////////////////////////////////////////////////////
        //
        // STEP 9 : Check get information errors
        //
        
        TEST_INFO("");
        TEST_INFO("<< STEP 9 >>");


        TEST_INFO( "set_default_properties" );
        ::FT::Properties prop2;
        
        prop2.length(4);
        prop2[0].nam.length(1);
        prop2[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop2[0].val <<= (CORBA::UShort)2;
        
        prop2[1].nam.length(1);
        prop2[1].nam[0].id="org.omg.ft.ReplicationStyle";
        prop2[1].val <<= (CORBA::UShort)2;
        
        prop2[2].nam.length(1);
        prop2[2].nam[0].id="org.omg.ft.MembershipStyle";
        prop2[2].val <<= (CORBA::UShort)0;
        
        prop2[3].nam.length(1);
        prop2[3].nam[0].id="org.omg.ft.Factories";
        
        ::FT::FactoryInfos factoryInfos2;
        factoryInfos2.length(1);
        factoryInfos2[0].the_factory = ::FT::GenericFactory::_nil();
        factoryInfos2[0].the_location = loc3;
        factoryInfos2[0].the_criteria = factoryCrit;
        
        prop2[3].val <<= factoryInfos2;
        
        
        ::FT::Criteria ft_criteria2;       
        ft_criteria2.length(1);
        ft_criteria2[0].nam.length(1);
        ft_criteria2[0].nam[0].id="org.omg.ft.FTProperties";
        ft_criteria2[0].val <<= prop2;

        TEST_SUCCEED();


        TEST_INFO("Create the object group 3");

        const char * rid = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface1:1.0";
        
        try
        {
            obj3 =  rm->create_object(rid,
                                      ft_criteria2,
                                      factory_creation_id3.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

 

        
        TEST_INFO("Create a new object");
        Cdmw::HelloInterface1_impl * helloInterface3_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface3 =  helloInterface3_impl->_this();
 
        
        // resolve the Activation Manager
        TEST_INFO( "Create the Activation Manager" );

        objref = m_orb->resolve_initial_references("FTActivationManager");
        CdmwFT::Location::ActivationManager_var activationManager3 = 
                             CdmwFT::Location::ActivationManager::_narrow(objref.in());
        
        // create an Activation Handle
        ::Cdmw::FT::TestFTActivationHandler_impl * testFTActivationHandler_impl3 =
                             new ::Cdmw::FT::TestFTActivationHandler_impl();

        // get the group_info of the group 1
        group_info = rm->get_object_group_info(obj3.in());

        // try to get information with an object which not belong to the object group
        // (object has not be added in group as member)

        try
        {
            ::FT::ObjectGroup_var result_get_object_group = 
                     primaryBackupGroupRepository_impl->get_object_group (helloInterface3.in());
            
            TEST_FAILED();
        }
        catch (::FT::MemberNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
 
        try
        {
            ::CdmwFT::MemberInfos_var result_get_other_group_members = 
                     primaryBackupGroupRepository_impl->get_other_group_members (helloInterface3.in());
            
            TEST_FAILED();
        }
        catch (::FT::MemberNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        
        
        ::FT::ObjectGroupRefVersion result_get_object_group_version;
        
        try
        {
            result_get_object_group_version = 
                    primaryBackupGroupRepository_impl->get_object_group_version (helloInterface3.in());
            
            TEST_FAILED();
        }
        catch (::FT::MemberNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        CORBA::Boolean result_is_primary;
        
        try
        {
            result_is_primary = primaryBackupGroupRepository_impl->is_primary (helloInterface3.in());
            TEST_FAILED();
        }
        catch (::FT::MemberNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // try to find information with a bad object_group_id
        ::FT::ObjectGroupId object_group_id = 100;

        try
        {
            ::FT::ObjectGroup_var result_get_object_group_from_gid = 
                  primaryBackupGroupRepository_impl->get_object_group_from_gid (object_group_id);
            
            TEST_FAILED();
        }
        catch (::FT::ObjectGroupNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        try
        {
            ::CdmwFT::MemberInfos_var result_get_other_group_members_from_gid = 
                  primaryBackupGroupRepository_impl->get_other_group_members_from_gid (object_group_id);
            
            TEST_FAILED();
        }
        catch (::FT::ObjectGroupNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        ::FT::ObjectGroupRefVersion result_get_object_group_version_from_gid;
        
        try
        {
            result_get_object_group_version_from_gid = 
                 primaryBackupGroupRepository_impl->get_object_group_version_from_gid (object_group_id);
            
            TEST_FAILED();
        }
        catch (::FT::ObjectGroupNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        
        CORBA::Boolean result_is_primary_from_gid;
        
        try
        {
            result_is_primary_from_gid = 
                 primaryBackupGroupRepository_impl->is_primary_from_gid (object_group_id);
            
            TEST_FAILED();
        }
        catch (::FT::ObjectGroupNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }


 //        // Add new member on the group
//         TEST_INFO("add object as a new member of group 1");
//         try
//         {
//             obj3 = rm->add_member(obj3.in(),
//                                   loc3,
//                                   helloInterface3.in());
//         }
        
//         catch( CORBA::Exception& e )
//         {
//             std::cerr << e._name() << std::endl;
//             TEST_FAILED();
//         }



        // register the Activation Handle into the Activation Manager 
        ::CdmwFT::Location::HandlerId activationManagerHandler;
        
        try
        {
            activationManagerHandler = 
              activationManager3->register_activation_handler(obj3.in(), 
                                                              testFTActivationHandler_impl3);
            
            TEST_SUCCEED();
        }
		  catch( CdmwFT::Location::HandlerAlreadyExist& e )
        {
            std::cerr << "HandlerAlreadyExist" << std::endl;
            TEST_FAILED();
        }       
		  catch(const  ::FT::ObjectGroupNotFound& e )
        {
            std::cerr << "FT::ObjectGroupNotFound" << std::endl;
            TEST_FAILED();
		  }
		  catch (const CORBA::SystemException& e)
		  {
            std::cerr << e << std::endl;
            TEST_FAILED();
		  }
        catch(const CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // register the Activation Handle into the Activation Manager twice
        try
        {
            activationManagerHandler = 
              activationManager3->register_activation_handler(obj3.in(), 
                                                              testFTActivationHandler_impl3);
            TEST_FAILED();
        }
        catch( CdmwFT::Location::HandlerAlreadyExist& e )
        {
            std::cout<<"hello"<<std::endl;
            
            TEST_SUCCEED();
        }        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        try
        {
            rm->delete_object(factory_creation_id1.in());
            
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // register the Activation Handle into the Activation Manager 
        // with an bad object group
        try
        {
            activationManagerHandler = 
              activationManager3->register_activation_handler(obj4.in(), 
                                                              testFTActivationHandler_impl3);
            TEST_FAILED();
        }
        catch (::FT::ObjectGroupNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        // unregister the Activation Handle into the Activation Manager 
        // with an bad handlerID
        try
        {
            activationManagerHandler = 100;
            activationManager3->unregister_activation_handler(activationManagerHandler);
            TEST_FAILED();
        }
        catch (::CdmwFT::Location::InvalidHandlerId& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        
        // call the update_group_view with the previous group info and after the delete object
        try
        {
            primaryBackupAdmin->update_group_view(group_info.in());
            TEST_FAILED();
        }
        catch (::FT::ObjectGroupNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        
        // call the update_primary with the previous group info and after the delete object
        try
        {
            primaryBackupAdmin->update_primary(group_info.in(), loc4);
            TEST_FAILED();
        }
        catch (::FT::ObjectGroupNotFound& e)
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
    }
    catch(const CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        std::cerr << e.what() << std::endl;
        TEST_FAILED();
    }


}

}; // namespace FT
}; // namespace Cdmw

