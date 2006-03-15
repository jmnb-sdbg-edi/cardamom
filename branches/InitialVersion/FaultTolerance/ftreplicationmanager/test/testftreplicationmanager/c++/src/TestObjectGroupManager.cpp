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
#include "testftreplicationmanager/TestObjectGroupManager.hpp"
#include "idllib/CdmwFTReplicationManager.stub.hpp"
#include "testftreplicationmanager/TestHello_impl.hpp"
#include "testftreplicationmanager/TestPrimaryBackupAdmin.hpp"
#include <Foundation/ossupport/OS.hpp>
#include <exception>
#include <iostream>

#if CDMW_ORB_VDR == tao
#include <FaultTolerance/ftcommon/TAO_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif

namespace Cdmw {
namespace FT {

// constructor
TestObjectGroupManager::TestObjectGroupManager(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name )
: Testable( name )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

// destructor
TestObjectGroupManager::~TestObjectGroupManager()
{
}

// do_tests
void TestObjectGroupManager::do_tests()
{
    set_nbOfRequestedTestOK (44);

    try
    {
        // create the Replication Manager
        TEST_INFO( "Create the Replication Manager" );

        CORBA::Object_var objref = m_orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());

	    if (CORBA::is_nil(rm.in()))
	    {
	        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                  << "could not _narrow object to type ReplicationManager" << std::endl;
	        abort();
	    }

        TEST_SUCCEED();


        Cdmw::HelloInterface1_impl * helloInterface1_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface1 =  helloInterface1_impl->_this();

        Cdmw::HelloInterface1_impl * helloInterface2_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface2 =  helloInterface2_impl->_this();
        TEST_SUCCEED();

        ::FT::Location loc;
        loc.length(3);
        loc[0].id="HOST5";
        loc[0].kind="hostname";        
        loc[1].id="APPL1";
        loc[1].kind="applicationname";        
        loc[2].id="PROC1";
        loc[2].kind="processname";

        // set default properties
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
        factoryInfos.length(2);
        factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
        factoryInfos[0].the_location = loc;
        ::FT::Criteria factoryCrit;        
        factoryCrit.length(0);
        factoryInfos[0].the_criteria = factoryCrit;

        factoryInfos[1].the_factory = ::FT::GenericFactory::_nil();
        ::FT::Location loc2;
        loc2.length(3);
        loc2[0].id="HOST6";
        loc2[0].kind="hostname";        
        loc2[1].id="APPL1";
        loc2[1].kind="applicationname";        
        loc2[2].id="PROC1";
        loc2[2].kind="processname";

        factoryInfos[1].the_location = loc2;
        factoryInfos[1].the_criteria = factoryCrit;

        prop[3].val <<= factoryInfos;

        TEST_SUCCEED();

        const char * rid = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface1:1.0";
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id11;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id2;
        ::FT::Criteria crit;        
        crit.length(2);
        crit[0].nam.length(1);
        crit[0].nam[0].id="org.omg.ft.FTProperties";
        crit[0].val <<= prop;

        ::FT::Name OGDName;
        OGDName.length(1);
        OGDName[0].id="OGDName";

        crit[1].nam.length(1);
        crit[1].nam[0].id="com.thalesgroup.cdmw.ft.Name";
        crit[1].val <<= OGDName;
       

        CORBA::Object_var obj1;
        CORBA::Object_var obj2;
        
        //create the first obect group
        TEST_INFO("Create a first object group");
              
        try
        {
            obj1 =  rm->create_object(rid,
                                      crit,
                                      factory_creation_id1.out());
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        //create the first obect group
        TEST_INFO("Create a object group with the same name");
              
        try
        {
            obj1 =  rm->create_object(rid,
                                      crit,
                                      factory_creation_id11.out());
            TEST_FAILED();
        }
        catch( const ::FT::ObjectNotCreated& e)
        {
            std::cerr << e << std::endl;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Test narrow on empty group");
        // try to narrow empty group into HelloInterface1
        CdmwReplicationManager::HelloInterface1_var hello1 =
            CdmwReplicationManager::HelloInterface1::_nil();
        try {
            hello1 = CdmwReplicationManager::HelloInterface1::_narrow(obj1.in());

            if (CORBA::is_nil(hello1.in())) {
                std::cout << "Narrow of empty object group into HelloInterface1 failed." << std::endl;
                TEST_FAILED();
            } else {
                TEST_SUCCEED();
            }
        } catch (const CORBA::Exception& e) {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        TEST_INFO("\n---------------------------------------------------------------------");
        // try to narrow empty group into CORBA::Object
        try {
            CORBA::Object_var obj3 =
                CORBA::Object::_narrow(hello1.in());

            if (CORBA::is_nil(obj3.in())) {
                std::cout << "Narrow of empty object group into CORBA::Object failed." << std::endl;
                TEST_FAILED();
            } else {
                TEST_SUCCEED();
            }
        } catch (const CORBA::Exception& e) {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        // try to narrow empty group into BaseInterface
        try {
            CdmwReplicationManager::BaseInterface_var base =
                CdmwReplicationManager::BaseInterface::_narrow(obj1.in());

            if (CORBA::is_nil(base.in())) {
                std::cout << "Narrow of empty object group into BaseInterface failed." << std::endl;
                TEST_FAILED();
            } else {
                TEST_SUCCEED();
            }
        } catch (const CORBA::Exception& e) {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        // try to narrow empty group into HelloInterface2
        try {
            CdmwReplicationManager::HelloInterface2_var hello2 =
                CdmwReplicationManager::HelloInterface2::_narrow(obj1.in());

            if (CORBA::is_nil(hello2.in())) {
                TEST_SUCCEED();
            } else {
                std::cout << "Narrow of empty object group into HelloInterface2 succeed but shouldn't !!" << std::endl;
                TEST_FAILED();
            }
        } catch (const CORBA::Exception& e) {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        
        // create an IOGRFactor
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Create the IOGRFactory");
         Cdmw::FT::IOGRFactory* m_iogrFactory;
        
#if CDMW_ORB_VDR == tao
        m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(m_orb.in(),
                                                      std::cout);
#endif

        // check the number of profile into the IOGR 
        // the must have 1 profile (the replication manager profile)
        CORBA::ULong nb_profile;
        
        nb_profile = m_iogrFactory->profile_count(obj1.in());
        TEST_CHECK( nb_profile == (1));
        

        // get_object_group_ref_from_name on the group previously deleted
        // the ::FT::ObjectGroupNotFound exception must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Get_object_group_ref_from_name on the group");
        
        try
        {
            ::FT::Name TestOGDName;
            TestOGDName.length(1);
            TestOGDName[0].id="OGDName";

            objref = rm->get_object_group_ref_from_name(TestOGDName);
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }






        // delete the first object group
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Delete the first object group");
        try
        {
            rm->delete_object(factory_creation_id1.in());    
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }


        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin1 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin1 = testPrimaryBackupAdmin1->_this();

        rm->register_location(loc, primary_backup_admin1.in());

        
        // add a new menber on the group previously deleted
        // the ::FT::ObjectGroupNotFound exception must be raised        

        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Add new member on the group previously deleted (ObjectGroupNotFound exception must be raised)");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface1.in());
            TEST_FAILED();                           
        }
        catch( const ::FT::ObjectGroupNotFound& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const ::FT::ObjectNotAdded& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        // remove a new menber on the group previously deleted
        // the ::FT::ObjectGroupNotFound exception must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Remove a new member on the group previously deleted (ObjectGroupNotFound exception must be raised)");
        try
        {
            obj1 = rm->remove_member(obj1.in(),
                                     loc);
            TEST_FAILED();                           
        }
        catch( const ::FT::ObjectGroupNotFound& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
//         // set_primary_member  a new menber on the group previously deleted
//         // the ::FT::ObjectGroupNotFound exception must be raised
//         TEST_INFO("\n---------------------------------------------------------------------");
//         TEST_INFO("Set_primary_member on the group previously deleted (ObjectGroupNotFound exception must be raised)");
//         try
//         {
//             obj1 = rm->set_primary_member(obj1.in(),
//                                           loc);
//             TEST_FAILED();                           
//         }
//         catch( const ::FT::ObjectGroupNotFound& e )
//         {
//             TEST_SUCCEED();
//         }
      
//         catch( const CORBA::Exception& e )
//         {
//             std::cerr << e << std::endl;
//             TEST_FAILED();
//         }

        // call of Locations_of_members  on the group previously deleted
        // the ::FT::ObjectGroupNotFound exception must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Locations_of_members on the group previously deleted (ObjectGroupNotFound exception must be raised)");
         ::FT::Locations_var locations;
        
        try
        {
            locations = rm->locations_of_members(obj1.in());
            TEST_FAILED();                           
        }
        catch( const ::FT::ObjectGroupNotFound& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        } 

        // get_object_group_id on the group previously deleted
        // the ::FT::ObjectGroupNotFound exception must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Get_object_group_id on the group previously deleted (ObjectGroupNotFound exception must be raised)");
        ::FT::ObjectGroupId objID;
        
        try
        {
            objID = rm->get_object_group_id(obj1.in());
            TEST_FAILED();                           
        }
        catch( const ::FT::ObjectGroupNotFound& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        // get_object_group_ref on the group previously deleted
        // the ::FT::ObjectGroupNotFound exception must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Get_object_group_ref on the group previously deleted (ObjectGroupNotFound exception must be raised)");
        
        try
        {
            objref = rm->get_object_group_ref(obj1.in());
            TEST_FAILED();                           
        }
        catch( const ::FT::ObjectGroupNotFound& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        // get_object_group_ref_from_name on the group previously deleted
        // the ::FT::ObjectGroupNotFound exception must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Get_object_group_ref_from_name on the group previously deleted (ObjectGroupNotFound exception must be raised)");
        
        try
        {
            ::FT::Name TestOGDName;
            TestOGDName.length(1);
            TestOGDName[0].id="TestOGDName";

            objref = rm->get_object_group_ref_from_name(TestOGDName);
            TEST_FAILED();                           
        }
        catch( const ::FT::ObjectGroupNotFound& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        // get_member_ref on the group previously deleted
        // the ::FT::ObjectGroupNotFound exception must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Get_member_ref on the group previously deleted (ObjectGroupNotFound exception must be raised)");
        
        try
        {
            objref = rm->get_member_ref(obj1.in(), loc);
            TEST_FAILED();                           
        }
        catch( const ::FT::ObjectGroupNotFound& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        


/*******************************************************************************/
        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin = testPrimaryBackupAdmin->_this();


        // create an new object group
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Create an object group");
              
        try
        {
            obj1 =  rm->create_object(rid,
                                      crit,
                                      factory_creation_id1.out());
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        nb_profile = m_iogrFactory->profile_count(obj1.in());
        TEST_INFO("\n---------------------------------------------------------------------");
        std::cout<< nb_profile<<std::endl;
        TEST_CHECK( nb_profile == (1));

        loc[0].id="HOST3";


        // add a new member on the group with a bad location
        // the exception ObjectNotAdded must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Add new member on the group with a bad location (ObjectNotAdded exception must be raised)");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface1.in());
            TEST_FAILED();                           
        }
        catch( const ::FT::ObjectNotAdded& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        // remove a new member on the group with a bad location
        // the exception ObjectNotAdded must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Remove a new member on the group with a bad location (MemberNotFound exception must be raised)");
        try
        {
            obj1 = rm->remove_member(obj1.in(),
                                     loc);
            TEST_FAILED();                           
        }
        catch( const ::FT::MemberNotFound& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

//         // set_primary_member on the group with a bad location
//         // the exception ObjectNotAdded must be raised
//         TEST_INFO("\n---------------------------------------------------------------------");
//         TEST_INFO("Set_primary_member on the group with a bad location (MemberNotFound exception must be raised)");
//         try
//         {
//             obj1 = rm->set_primary_member(obj1.in(),
//                                           loc);
//             TEST_FAILED();                           
//         }
//         catch( const ::FT::MemberNotFound& e )
//         {
//             TEST_SUCCEED();
//         }
      
//         catch( const CORBA::Exception& e )
//         {
//             std::cerr << e << std::endl;
//             TEST_FAILED();
//         }


        // locations_of_members on the group with a bad location
        // the exception ObjectNotAdded must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Locations_of_members on the group with a bad location (No exception must be raised)");
        try
        {
            locations = rm->locations_of_members(obj1.in());
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        // get_member_ref on the group with a bad location
        // the exception ObjectNotAdded must be raised       
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Get_member_ref on the group with a bad location (MemberNotFound exception must be raised)");
        try
        {
            objref = rm->get_member_ref(obj1.in(), loc);
            TEST_FAILED();                           
        }
        catch( const ::FT::MemberNotFound& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
/*******************************************************************************/

        loc[0].id="HOST5";

         
        // add a new member ont the group
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Add new member on the group");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface1.in());
            TEST_SUCCEED();                           
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        nb_profile = m_iogrFactory->profile_count(obj1.in());
        TEST_INFO("\n---------------------------------------------------------------------");
        std::cout<< nb_profile<<std::endl;
        TEST_CHECK( nb_profile == (2));

//         CdmwReplicationManager::HelloInterface1_var hello1 =
//             CdmwReplicationManager::HelloInterface1::_nil();
//         try {
//             hello1 = CdmwReplicationManager::HelloInterface1::_narrow(obj1.in());

//             if (CORBA::is_nil(hello1.in())) {
//                 std::cout << "Narrow of empty object group into HelloInterface1 failed." << std::endl;
//                 TEST_FAILED();
//             } else {
//                 TEST_SUCCEED();
//                 hello1->hello();
//                 TEST_SUCCEED();
//             }
//         } catch (const CORBA::Exception& e) {
//             std::cerr << e << std::endl;
//             TEST_FAILED();
//         }

        // add again the same member on the group
        // the MemberAlreadyPresent exception must be raised
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Add new member on the group (MemberAlreadyPresent exception must be raised)");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface1.in());
            TEST_FAILED();                           
        }
        catch( const ::FT::MemberAlreadyPresent& e )
        {
            TEST_SUCCEED();
        }
      
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        // call the locations_of_members on the object group
        try
        {
            locations = rm->locations_of_members(obj1.in());
            TEST_SUCCEED();                           
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        loc[0].id="HOST6";



        // add new member on the group
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Add new second member on the group");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface2.in());
            TEST_FAILED();
        }
        catch( const ::FT::ObjectNotAdded& e )
        {
            std::cerr << e << std::endl;
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }


        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin2 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin2 = testPrimaryBackupAdmin2->_this();
        
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Register location HOST6");
        rm->register_location(loc, primary_backup_admin2.in());

        // add new member on the group
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Add new second member on the group");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface2.in());
            TEST_SUCCEED();                           
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        nb_profile = m_iogrFactory->profile_count(obj1.in());
        TEST_INFO("\n---------------------------------------------------------------------");
        std::cout<< nb_profile<<std::endl;
        TEST_CHECK( nb_profile == (3));

//         // set_primary_member on the second member
//         TEST_INFO("\n---------------------------------------------------------------------");
//         TEST_INFO("Set_primary_member on the group and on the second object");
//         try
//         {
//             obj1 = rm->set_primary_member(obj1.in(),
//                                           loc);
//             TEST_SUCCEED();                           
//         }
//         catch( const CORBA::Exception& e )
//         {
//             std::cerr << e << std::endl;
//             TEST_FAILED();
//         }

//         loc[0].id="HOST5";

//         // set_primary_member on the first object
//         // the PrimaryNotSet exception must be raised
//         TEST_INFO("\n---------------------------------------------------------------------");
//         TEST_INFO("Set_primary_member on the group and on the first object");
//         try
//         {
//             obj1 = rm->set_primary_member(obj1.in(),
//                                           loc);
//             TEST_SUCCEED();                           
//         }
//         catch( const CORBA::Exception& e )
//         {
//             std::cerr << e << std::endl;
//             TEST_FAILED();                           
//         }

        loc[0].id="HOST6";

        if (CORBA::is_nil(obj1.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();

        nb_profile = m_iogrFactory->profile_count(obj1.in());
        TEST_INFO("\n---------------------------------------------------------------------");
        std::cout<< nb_profile<<std::endl;
        TEST_CHECK( nb_profile == (3));
        
        ::FT::TagFTGroupTaggedComponent * tag_group1;
        ::FT::TagFTGroupTaggedComponent * tag_group2;
        try 
        {
            tag_group1 = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),0);
            TEST_SUCCEED();
            tag_group2 = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),1);
            TEST_SUCCEED();
            
            // compare the ft_domain_id on the two first profiles
            std::string ft_domain_id1 = tag_group1->group_domain_id.in();
            std::string ft_domain_id2 = tag_group2->group_domain_id.in();
            
            if (ft_domain_id1 == ft_domain_id2)
                TEST_SUCCEED();
            else
                TEST_FAILED();
            
            // compare the object_group_id on the two first profiles
            if (tag_group1->object_group_id == tag_group2->object_group_id)
                TEST_SUCCEED();
            else
                TEST_FAILED();
            
            // compare the object_group_ref_version on the two first profiles        
            if (tag_group1->object_group_ref_version == tag_group2->object_group_ref_version)
                TEST_SUCCEED();
            else
                TEST_FAILED();
            
            delete tag_group1;
            delete tag_group2;
        }
        catch(...)
        {
            TEST_FAILED();
        }

        // remove the second object 
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Remove the second object");
        try
        {
            obj1 = rm->remove_member(obj1.in(),
                                     loc);
            TEST_SUCCEED();                           
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        // remove the object group
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Delete the object group (throw BAD_INV_ORDER because the objectGroup is not empty");
        try
        {
            rm->delete_object(factory_creation_id1.in());    
            TEST_FAILED();
        }
        catch (const CORBA::BAD_INV_ORDER&e)
        {
            std::cerr << e << std::endl;
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        // remove the second object 
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Remove the second object");
        loc[0].id="HOST5";

        try
        {
            obj1 = rm->remove_member(obj1.in(),
                                     loc);
            TEST_SUCCEED();                           
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        // remove the object group
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Delete the object group");
        try
        {
            rm->delete_object(factory_creation_id1.in());    
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        // remove the object group again
        TEST_INFO("\n---------------------------------------------------------------------");
        TEST_INFO("Delete the object group (ObjectNotFound exception must be raised)");
        try
        {
            rm->delete_object(factory_creation_id1.in());    
            TEST_FAILED();
        }
        catch( const ::FT::ObjectNotFound& e )
        {
            TEST_SUCCEED();
        }

        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
   

    }
    
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        std::cerr << e << std::endl;
        TEST_FAILED();
    }


}

}; // namespace FT
}; // namespace Cdmw

