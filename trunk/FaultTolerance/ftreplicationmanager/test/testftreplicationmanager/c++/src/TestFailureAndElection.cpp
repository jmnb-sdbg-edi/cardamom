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

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>

#include "idllib/CdmwFTReplicationManager.stub.hpp"
#include "testftreplicationmanager/TestFailureAndElection.hpp"
#include "testftreplicationmanager/TestFaultNotifier.hpp"
#include "testftreplicationmanager/TestFaultDetector.hpp"
#include "testftreplicationmanager/TestHello_impl.hpp"
#include "testftreplicationmanager/TestPrimaryBackupAdmin.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>


#if CDMW_ORB_VDR == tao
#include <FaultTolerance/ftcommon/TAO_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif

namespace Cdmw {
namespace FT {

    // constructor
    TestFailureAndElection::TestFailureAndElection(
                                                   CORBA::ORB_ptr orb,
                                                   PortableServer::POA_ptr rootPOA,
                                                   const std::string& name )
        : Testable( name )
    {
        m_orb = CORBA::ORB::_duplicate( orb );
        m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
    }

    // destructor
    TestFailureAndElection::~TestFailureAndElection()
    {
    }

    // do_tests
    void TestFailureAndElection::do_tests()
    {
        set_nbOfRequestedTestOK (158);

        unsigned int number_of_member = 0;
        unsigned int required_object_group_version=0;

        // create an IOGRFactory
        TEST_INFO("Create the IOGRFactory");
        Cdmw::FT::IOGRFactory* m_iogrFactory;
        
#if CDMW_ORB_VDR == tao
        m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(m_orb.in(),
                                                      std::cout);
#endif

        // create the replication manager
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

        // set default properties for the object group
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


        std::vector<std::string> slocvect(9);
        slocvect[0] = "HOST1.hostname/APPL1.applicationname/PROC1.processname";
        slocvect[1] = "HOST1.hostname/APPL1.applicationname/PROC2.processname";
        slocvect[2] = "HOST1.hostname/APPL1.applicationname/PROC3.processname";
        slocvect[3] = "HOST2.hostname/APPL1.applicationname/PROC1.processname";
        slocvect[4] = "HOST2.hostname/APPL1.applicationname/PROC2.processname";
        slocvect[5] = "HOST2.hostname/APPL2.applicationname/PROC1.processname";
        slocvect[6] = "HOST3.hostname/APPL1.applicationname/PROC1.processname";
        slocvect[7] = "HOST3.hostname/APPL1.applicationname/PROC2.processname";
        slocvect[8] = "HOST4.hostname/APPL1.applicationname/PROC1.processname";



        const CORBA::ULong MAX_LOCS=slocvect.size();
        ::FT::FactoryInfos factoryInfos;
        factoryInfos.length(MAX_LOCS);
        for (CORBA::ULong i = 0; i < MAX_LOCS; ++i) {
            factoryInfos[i].the_factory = ::FT::GenericFactory::_nil();
            ::FT::Location_var loc = 
                  Cdmw::CommonSvcs::Naming::NamingInterface::to_name(slocvect[i]);
            std::cerr << '[' << i << "] " << slocvect[i] << " --- " 
                      << Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc.in()) << std::endl;

            factoryInfos[i].the_location = loc.in();
            ::FT::Criteria factoryCrit;        
            factoryCrit.length(0);
            factoryInfos[i].the_criteria = factoryCrit;
        }
    
        prop[3].nam.length(1);
        prop[3].nam[0].id="org.omg.ft.Factories";
        prop[3].val <<= factoryInfos;



        TEST_SUCCEED();

        std::cout << "                                              " << std::endl;        
        std::cout << "    --> HOST1 |--> APPL1 |--> PROC1           " << std::endl;
        std::cout << "                         |--> PROC2           " << std::endl;
        std::cout << "                         |--> PROC3           " << std::endl;
        std::cout << "                                              " << std::endl;
        std::cout << "    --> HOST2 |--> APPL1 |--> PROC1           " << std::endl;
        std::cout << "              |          |--> PROC2           " << std::endl;
        std::cout << "              |                               " << std::endl;
        std::cout << "              |--> APPL2 |--> PROC1           " << std::endl;
        std::cout << "                                              " << std::endl;
        std::cout << "    --> HOST3 |--> APPL1 |--> PROC1           " << std::endl;
        std::cout << "                         |--> PROC2           " << std::endl;
        std::cout << "                                              " << std::endl;
        std::cout << "    --> HOST4 |--> APPL1 |--> PROC1           " << std::endl;
        std::cout << "                                              " << std::endl;

        const char * rid = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface1:1.0";

        // create all the object 
        Cdmw::HelloInterface1_impl * helloInterface1_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface1 =  helloInterface1_impl->_this();

        Cdmw::HelloInterface1_impl * helloInterface2_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface2 =  helloInterface2_impl->_this();
        
        Cdmw::HelloInterface1_impl * helloInterface3_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface3 =  helloInterface3_impl->_this();

        Cdmw::HelloInterface1_impl * helloInterface4_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface4 =  helloInterface4_impl->_this();

        Cdmw::HelloInterface1_impl * helloInterface5_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface5 =  helloInterface5_impl->_this();
        
        Cdmw::HelloInterface1_impl * helloInterface6_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface6 =  helloInterface6_impl->_this();

        Cdmw::HelloInterface1_impl * helloInterface7_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface7 =  helloInterface7_impl->_this();

        Cdmw::HelloInterface1_impl * helloInterface8_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface8 =  helloInterface8_impl->_this();

        Cdmw::HelloInterface1_impl * helloInterface9_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface9 =  helloInterface9_impl->_this();

        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;

        ::FT::Criteria crit;        
        crit.length(1);
        crit[0].nam.length(1);
        crit[0].nam[0].id="org.omg.ft.FTProperties";
        crit[0].val <<= prop;



        CORBA::Object_var obj1;
        CORBA::Object_var obj2;
     
        // create the object group
        TEST_INFO("Create a first object");

        try
        {
            obj1 =  rm->create_object(rid,
                                      crit,
                                      factory_creation_id1.out());
            required_object_group_version++;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
     
        if (CORBA::is_nil(obj1.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();
  
        // add six new members in the previous object group
        // for each member, a PrimaryBackupAdmin is created

        ::FT::Location loc;
        loc.length(3);
        loc[0].kind="hostname";
        loc[0].id="HOST1";
        loc[1].kind="applicationname";
        loc[1].id="APPL1";
        loc[2].kind="processname";
        loc[2].id="PROC1";

        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin1 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin1 = testPrimaryBackupAdmin1->_this();

        rm->register_location(loc, primary_backup_admin1.in());


        TEST_INFO("Add new member on the group HOST1/APPL1/PROC1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface1.in());
            number_of_member++;
            required_object_group_version++;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }



        TEST_INFO("Realise a set_primary for a location already primary");
        try
        {
            obj1 = rm->set_primary_member(obj1.in(),
                                          loc);
            TEST_SUCCEED();
        }
        catch(const ::FT::PrimaryNotSet& )
        {
            TEST_FAILED();
        }
        catch(...)
        {
            TEST_FAILED();
        }




        // check if the update method of the PrimaryBackupAdmin is called
        TEST_INFO("check the update function called");
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        Cdmw::OsSupport::OS::sleep(timescale*1000);
        if (testPrimaryBackupAdmin1->m_update_group_view_called == false)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin1->m_update_group_view_called = false;
        }
        else
            TEST_FAILED();

        if (testPrimaryBackupAdmin1->m_update_primary_called == true)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin1->m_update_primary_called = false;
        }
        else
            TEST_FAILED();

        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC2";

        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin2 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin2 = testPrimaryBackupAdmin2->_this();
        
        rm->register_location(loc, primary_backup_admin2.in());

        TEST_INFO("Add new member on the group HOST1/APPL1/PROC2");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface2.in());
            number_of_member++;
            required_object_group_version++;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        TEST_INFO("Realise a set_primary for a location not already primary");
        try
        {
            obj1 = rm->set_primary_member(obj1.in(),
                                          loc);
            TEST_FAILED();
        }
        catch(const ::FT::PrimaryNotSet& )
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }


        // check if the update method of the PrimaryBackupAdmin is called
        TEST_INFO("check the update function called");
        
        Cdmw::OsSupport::OS::sleep(timescale*1000);
        if (testPrimaryBackupAdmin1->m_update_group_view_called == true)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin1->m_update_group_view_called = false;
        }
        else
            TEST_FAILED();

        if (testPrimaryBackupAdmin1->m_update_primary_called == false)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin1->m_update_primary_called = false;
        }
        else
            TEST_FAILED();

        if (testPrimaryBackupAdmin2->m_update_group_view_called == true)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin2->m_update_group_view_called = false;
        }
        else
            TEST_FAILED();

        if (testPrimaryBackupAdmin2->m_update_primary_called == false)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin2->m_update_primary_called = false;
        }
        else
            TEST_FAILED();
        

        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC3";

        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin3 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin3 = testPrimaryBackupAdmin3->_this();
        
        rm->register_location(loc, primary_backup_admin3.in());


        TEST_INFO("Add new member on the group HOST1/APPL1/PROC3");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface3.in());
            number_of_member++;
            required_object_group_version++;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        TEST_INFO("check the update function called");        
        Cdmw::OsSupport::OS::sleep(timescale*1000);
        // check if the update method of the PrimaryBackupAdmin is called
        if (testPrimaryBackupAdmin1->m_update_group_view_called == true)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin1->m_update_group_view_called = false;
        }
        else
            TEST_FAILED();

        if (testPrimaryBackupAdmin1->m_update_primary_called == false)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin1->m_update_primary_called = false;
        }
        else
            TEST_FAILED();

        if (testPrimaryBackupAdmin2->m_update_group_view_called == true)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin2->m_update_group_view_called = false;
        }
        else
            TEST_FAILED();

        if (testPrimaryBackupAdmin2->m_update_primary_called == false)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin2->m_update_primary_called = false;
        }
        else
            TEST_FAILED();

        if (testPrimaryBackupAdmin3->m_update_group_view_called == true)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin3->m_update_group_view_called = false;
        }
        else
            TEST_FAILED();

        if (testPrimaryBackupAdmin3->m_update_primary_called == false)
        {
            TEST_SUCCEED();
            testPrimaryBackupAdmin3->m_update_primary_called = false;
        }
        else
            TEST_FAILED();

        loc[0].id="HOST2";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin4 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin4 = testPrimaryBackupAdmin4->_this();
        
        rm->register_location(loc, primary_backup_admin4.in());


        TEST_INFO("Add new member on the group HOST2/APPL1/PROC1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface4.in());
            number_of_member++;
            required_object_group_version++;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST2";
        loc[1].id="APPL1";
        loc[2].id="PROC2";

        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin5 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin5 = testPrimaryBackupAdmin5->_this();
        
        rm->register_location(loc, primary_backup_admin5.in());


        TEST_INFO("Add new member on the group HOST2/APPL1/PROC2");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface5.in());
            number_of_member++;
            required_object_group_version++;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST2";
        loc[1].id="APPL2";
        loc[2].id="PROC1";

        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin6 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin6 = testPrimaryBackupAdmin6->_this();
        
        rm->register_location(loc, primary_backup_admin6.in());


        TEST_INFO("Add new member on the group HOST2/APPL2/PROC1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface6.in());
            number_of_member++;
            required_object_group_version++;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST3";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin7 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin7 = testPrimaryBackupAdmin7->_this();
        
        rm->register_location(loc, primary_backup_admin7.in());


        TEST_INFO("Add new member on the group HOST3/APPL1/PROC1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface7.in());
            number_of_member++;
            required_object_group_version++;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST3";
        loc[1].id="APPL1";
        loc[2].id="PROC2";

        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin8 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin8 = testPrimaryBackupAdmin8->_this();
        
        rm->register_location(loc, primary_backup_admin8.in());


        TEST_INFO("Add new member on the group HOST3/APPL1/PROC2");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface8.in());
            number_of_member++;
            required_object_group_version++;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST4";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        ::Cdmw::FT::TestPrimaryBackupAdmin * testPrimaryBackupAdmin9 
          = new  Cdmw::FT::TestPrimaryBackupAdmin(m_rootPOA.in(), loc );
        ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin9 = testPrimaryBackupAdmin9->_this();
        
        rm->register_location(loc, primary_backup_admin9.in());


        TEST_INFO("Add new member on the group HOST4/APPL1/PROC1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloInterface9.in());
            number_of_member++;
            required_object_group_version++;
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        testPrimaryBackupAdmin1->m_update_group_view_called= false;
        testPrimaryBackupAdmin2->m_update_group_view_called= false;
        testPrimaryBackupAdmin3->m_update_group_view_called= false;
        testPrimaryBackupAdmin4->m_update_group_view_called= false;
        testPrimaryBackupAdmin5->m_update_group_view_called= false;
        testPrimaryBackupAdmin6->m_update_group_view_called= false;
        testPrimaryBackupAdmin7->m_update_group_view_called= false;
        testPrimaryBackupAdmin8->m_update_group_view_called= false;
        testPrimaryBackupAdmin9->m_update_group_view_called= false;




        // Check the IOGR profiles 
        {
            CORBA::ULong nb_profile = m_iogrFactory->profile_count(obj1.in());
            TEST_CHECK( nb_profile == (number_of_member+1));
            
            // we don't check the last profile (it's the replication Manager)
            for (CORBA::ULong i = 0; i < number_of_member ; i++)
            {
                try
                {
                    TEST_INFO("Check the profile n°"<<i);
                    ::FT::TagFTGroupTaggedComponent * tag_group 
                      = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),i);
                    if (tag_group->object_group_ref_version >= required_object_group_version)
                    {
                        TEST_SUCCEED();
                        required_object_group_version = tag_group->object_group_ref_version;
                    }
                    else
                        TEST_FAILED();
                    
                    delete tag_group;
                    TEST_SUCCEED();
                }
                catch(...)
                {
                    TEST_FAILED();
                }
            }
        }

        
        // create the Fault Notifier and store it on the Replication Manager
        TEST_INFO( "Create the FaultNotifier" );
        
        Cdmw::FT::TestFaultNotifier* fn_impl = new  Cdmw::FT::TestFaultNotifier();
        ::FT::FaultNotifier_var fn = fn_impl->_this();
        TEST_SUCCEED();
        TEST_INFO( "Register the FaultNotifier to the ReplicationManager" );
        
        rm->register_fault_notifier(fn.in());
        
        TEST_SUCCEED();
        
        // create the Fault Detector and registe it on the Replication Manager
        TEST_INFO( "Create the FaultDetector" );
        
        Cdmw::FT::TestFaultDetector* fd_impl = new  Cdmw::FT::TestFaultDetector();
        ::CdmwFT::FaultManagement::FaultDetector_var fd = fd_impl->_this();
        TEST_SUCCEED();
        TEST_INFO( "Register the FaultDetector to the ReplicationManager" );

        rm->register_fault_detector(fd.in());

        TEST_SUCCEED();


        // Recover the FaultNotifier
        TEST_INFO("Recover the FaultNotifier");
        ::FT::FaultNotifier_var fn2 ;
        
        try
        {
            fn2 = rm->get_fault_notifier();
            TEST_SUCCEED();
        }        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        if (CORBA::is_nil(fn2.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();

        // Register the Replication Manager as a consumer of the Fault Notifier
        TEST_INFO("Create a reference of the StructuredPushConsumer (it is the replication manager)");

        ::CosNotifyComm::StructuredPushConsumer_var  rmStructuredPushConsumer =
          ::CosNotifyComm::StructuredPushConsumer::_narrow(rm.in());

        
        ::FT::FaultNotifier::ConsumerId consumerID;
        
        TEST_INFO("Register a reference of the StructuredPushConsumer (it is the replication manager)");
      
        try
        {
            consumerID = fn2->connect_structured_fault_consumer (rmStructuredPushConsumer.in());
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

  


        
        // create a fault report to send a bug report to the Fault Notifier
        // (the fault Notifier forward the bug report to the Replication Manager)
        TEST_INFO("Creation of a fault report for the primary member");

        CosNotification::StructuredEvent notification;

        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        std::ostringstream ft_domain_id;
        ft_domain_id << "My_" << Cdmw::OsSupport::OS::get_logname();
        ft_domain_id << "FTdomain" << std::ends;

        notification.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        notification.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        notification.filterable_data.length(2);
        notification.filterable_data[0].name = "FTDomainID";
        notification.filterable_data[0].value <<= ft_domain_id.str().c_str();
        notification.filterable_data[1].name = "Location";
        notification.filterable_data[1].value <<= loc;
        
        // decrease the number of members
        number_of_member--;
        required_object_group_version++;

        TEST_INFO("Send the fault report to the Fault Notifier");
        try
        {
            fn2->push_structured_fault( notification );
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        // sleep to wait the new version of the object group from the Replication Manager
        Cdmw::OsSupport::OS::sleep(timescale*2000);

        // recover the new version of the object group
        TEST_INFO("Get the new object group reference");
        
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
        
        // check if some process are deleted and other are present
        CORBA::Object_var obj_ref;

        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC1";


        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_FAILED();    
        }
        catch( const ::FT::MemberNotFound& e )
        {
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC2";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }



        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC3";

        
        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
        
        loc[0].id="HOST2";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        
        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        // Check profile to see if a primary is set
        TEST_INFO("Check if the primary is elected");
        // Check the IOGR profiles 
        {
            CORBA::Boolean retval = false;
            CORBA::ULong nb_profile = m_iogrFactory->profile_count(obj1.in());
            TEST_CHECK( nb_profile == (number_of_member+1));
            
            // we don't check the last profile (it's the replication Manager)
            for (CORBA::ULong i = 0; i < number_of_member ; i++)
            {
                try
                {
                    TEST_INFO("Check the profile n°"<<i);                    
                    ::FT::TagFTGroupTaggedComponent * tag_group 
                      = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),i);
                    if (tag_group->object_group_ref_version >= required_object_group_version)
                    {
                        TEST_SUCCEED();
                        required_object_group_version = tag_group->object_group_ref_version;
                    }
                    else
                        TEST_FAILED();                    

                    try 
                    {
                        ::FT::TagFTPrimaryTaggedComponent tag_primary
                          = m_iogrFactory->decode_profile_with_primary_tag (obj1.in(),i);
                        retval = true;
                        TEST_INFO("Profile n°"<<i<<" is primary");
                    }
                    catch(const CdmwFT::TagNotFound&)
                    {
                        TEST_INFO("Profile n°"<<i<<" is not primary");
                    }
                        
                    delete tag_group;
                    TEST_SUCCEED();
                }
                catch(const CORBA::Exception & e)
                {
                    std::cerr << e << std::endl;
                    TEST_FAILED();
                }
                catch(...)
                {
                    TEST_FAILED();
                }
            }
            
            if (retval == true)
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }



        // create a fault report to send a bug report to the Fault Notifier
        // (the fault Notifier forward the bug report to the Replication Manager)
        TEST_INFO("Creation of a fault report for a non primary member");

        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC3";

        notification.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        notification.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        notification.filterable_data.length(2);
        notification.filterable_data[0].name = "FTDomainID";
        notification.filterable_data[0].value <<= ft_domain_id.str().c_str();
        notification.filterable_data[1].name = "Location";
        notification.filterable_data[1].value <<= loc;
        
        // decrease the number of members
        number_of_member--;
        required_object_group_version++;

        TEST_INFO("Send the fault report to the Fault Notifier");
        try
        {
            fn2->push_structured_fault( notification );
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        // sleep to wait the new version of the object group from the Replication Manager
        Cdmw::OsSupport::OS::sleep(timescale*2000);

        // recover the new version of the object group
        TEST_INFO("Get the new object group reference");
        
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
        

        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC3";


        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_FAILED();    
        }
        catch( const ::FT::MemberNotFound& e )
        {
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC2";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
        
        loc[0].id="HOST2";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        
        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        // Check profile to see if a primary is set
        TEST_INFO("Check if the primary is elected");
        // Check the IOGR profiles 
        {
            CORBA::Boolean retval = false;
            CORBA::ULong nb_profile = m_iogrFactory->profile_count(obj1.in());
            TEST_CHECK( nb_profile == (number_of_member+1));
            
            // we don't check the last profile (it's the replication Manager)
            for (CORBA::ULong i = 0; i < number_of_member ; i++)
            {
                try
                {
                    TEST_INFO("Check the profile n°"<<i);                    
                    ::FT::TagFTGroupTaggedComponent * tag_group 
                      = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),i);
                    if (tag_group->object_group_ref_version >= required_object_group_version)
                    {
                        TEST_SUCCEED();
                        required_object_group_version = tag_group->object_group_ref_version;
                    }
                    else
                        TEST_FAILED();                    

                    try 
                    {
                        ::FT::TagFTPrimaryTaggedComponent tag_primary
                          = m_iogrFactory->decode_profile_with_primary_tag (obj1.in(),i);
                        retval = true;
                        TEST_INFO("Profile n°"<<i<<" is primary");
                    }
                    catch(const CdmwFT::TagNotFound&)
                    {
                        TEST_INFO("Profile n°"<<i<<" is not primary");
                    }
                        
                    delete tag_group;
                    TEST_SUCCEED();
                }
                catch(const CORBA::Exception & e)
                {
                    std::cerr << e << std::endl;
                    TEST_FAILED();
                }
                catch(...)
                {
                    TEST_FAILED();
                }
            }
            
            if (retval == true)
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }




        
        // create a fault report for a bug on the helloInterface1
        TEST_INFO("Creation of a fault report for the primary member");
        loc.length(3);
        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC2";

        notification.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        notification.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        notification.filterable_data.length(2);
        notification.filterable_data[0].name = "FTDomainID";
        notification.filterable_data[0].value <<= ft_domain_id.str().c_str();
        notification.filterable_data[1].name = "Location";
        notification.filterable_data[1].value <<= loc;

        // decrease the number of members
        number_of_member = number_of_member - 1;
        // increase the version of the object group
        required_object_group_version++;
        

        TEST_INFO("Send the fault report to the Fault Notifier");

        try
        {
            fn2->push_structured_fault( notification );
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        Cdmw::OsSupport::OS::sleep(timescale*2000);
    
        // recover the new reference of the object group
        TEST_INFO("Get the new object group reference");
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        // check if some process are deleted and other are present

        loc.length(3);

        loc[0].id="HOST1";
        loc[1].id="APPL1";
        loc[2].id="PROC2";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_FAILED();    
        }
        catch( const ::FT::MemberNotFound& e )
        {
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST2";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
        
        loc[0].id="HOST2";
        loc[1].id="APPL2";
        loc[2].id="PROC1";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        // Check profile to see if a primary is set
        TEST_INFO("Check if the primary is elected");
        // Check the IOGR profiles 
        {
            CORBA::Boolean retval = false;
            CORBA::ULong nb_profile = m_iogrFactory->profile_count(obj1.in());
            TEST_CHECK( nb_profile == (number_of_member+1));
            
            // we don't check the last profile (it's the replication Manager)
            for (CORBA::ULong i = 0; i < number_of_member ; i++)
            {
                try
                {
                    TEST_INFO("Check the profile n°"<<i);                    
                    ::FT::TagFTGroupTaggedComponent * tag_group 
                      = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),i);
                    if (tag_group->object_group_ref_version >= required_object_group_version)
                    {
                        TEST_SUCCEED();
                        required_object_group_version = tag_group->object_group_ref_version;
                    }
                    else
                        TEST_FAILED();                    

                    try 
                    {
                        ::FT::TagFTPrimaryTaggedComponent tag_primary
                          = m_iogrFactory->decode_profile_with_primary_tag (obj1.in(),i);
                        retval = true;
                        TEST_INFO("Profile n°"<<i<<" is primary");                        
                    }
                    catch(const CdmwFT::TagNotFound&)
                    {
                        TEST_INFO("Profile n°"<<i<<" is not primary");
                    }
                        
                    delete tag_group;
                    TEST_SUCCEED();
                }
                catch(...)
                {
                    TEST_FAILED();
                }
            }
            
            if (retval == true)
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }


       
        // create a fault report for a bug on the helloInterface1
        TEST_INFO("Creation of a fault report for a non primary member");
        loc.length(3);
        loc[0].id="HOST2";
        loc[1].id="APPL2";
        loc[2].id="PROC1";

        notification.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        notification.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        notification.filterable_data.length(2);
        notification.filterable_data[0].name = "FTDomainID";
        notification.filterable_data[0].value <<= ft_domain_id.str().c_str();
        notification.filterable_data[1].name = "Location";
        notification.filterable_data[1].value <<= loc;

        // decrease the number of members
        number_of_member = number_of_member - 1;
        // increase the version of the object group
        required_object_group_version++;
        

        TEST_INFO("Send the fault report to the Fault Notifier");

        try
        {
            fn2->push_structured_fault( notification );
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        Cdmw::OsSupport::OS::sleep(timescale*2000);
    
        // recover the new reference of the object group
        TEST_INFO("Get the new object group reference");
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        // check if some process are deleted and other are present

        loc.length(3);

        loc[0].id="HOST2";
        loc[1].id="APPL2";
        loc[2].id="PROC1";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_FAILED();    
        }
        catch( const ::FT::MemberNotFound& e )
        {
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST2";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        // Check profile to see if a primary is set
        TEST_INFO("Check if the primary is elected");
        // Check the IOGR profiles 
        {
            CORBA::Boolean retval = false;
            CORBA::ULong nb_profile = m_iogrFactory->profile_count(obj1.in());
            TEST_CHECK( nb_profile == (number_of_member+1));
            
            // we don't check the last profile (it's the replication Manager)
            for (CORBA::ULong i = 0; i < number_of_member ; i++)
            {
                try
                {
                    TEST_INFO("Check the profile n°"<<i);                    
                    ::FT::TagFTGroupTaggedComponent * tag_group 
                      = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),i);
                    if (tag_group->object_group_ref_version >= required_object_group_version)
                    {
                        TEST_SUCCEED();
                        required_object_group_version = tag_group->object_group_ref_version;
                    }
                    else
                        TEST_FAILED();                    

                    try 
                    {
                        ::FT::TagFTPrimaryTaggedComponent tag_primary
                          = m_iogrFactory->decode_profile_with_primary_tag (obj1.in(),i);
                        retval = true;
                        TEST_INFO("Profile n°"<<i<<" is primary");                        
                    }
                    catch(const CdmwFT::TagNotFound&)
                    {
                        TEST_INFO("Profile n°"<<i<<" is not primary");
                    }
                        
                    delete tag_group;
                    TEST_SUCCEED();
                }
                catch(...)
                {
                    TEST_FAILED();
                }
            }
            
            if (retval == true)
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }




        // create a fault report for a bug on the helloInterface1
        TEST_INFO("Creation of a fault report for the primary member");
        loc.length(1);
        loc[0].id="HOST2";


        notification.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        notification.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        notification.filterable_data.length(2);
        notification.filterable_data[0].name = "FTDomainID";
        notification.filterable_data[0].value <<= ft_domain_id.str().c_str();
        notification.filterable_data[1].name = "Location";
        notification.filterable_data[1].value <<= loc;

        // decrease the number of members
        number_of_member = number_of_member - 2;
        // increase the version of the object group
        required_object_group_version++;
      
        TEST_INFO("Send the fault report to the Fault Notifier");

        try
        {
            fn2->push_structured_fault( notification );
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        Cdmw::OsSupport::OS::sleep(timescale*2000);
    
        // recover the new reference of the object group
        TEST_INFO("Get the new object group reference");
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
        
        loc.length(3);
        loc[0].id="HOST2";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_FAILED();    
        }
        catch( ::FT::MemberNotFound& e )
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST2";
        loc[1].id="APPL1";
        loc[2].id="PROC2";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_FAILED();    
        }
        catch( ::FT::MemberNotFound& e )
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        
        loc[0].id="HOST3";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST4";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        // Check profile to see if a primary is set
        TEST_INFO("Check if the primary is elected");
        // Check the IOGR profiles 
        {
            CORBA::Boolean retval = false;
            CORBA::ULong nb_profile = m_iogrFactory->profile_count(obj1.in());
            TEST_CHECK( nb_profile == (number_of_member+1));
            
            // we don't check the last profile (it's the replication Manager)
            for (CORBA::ULong i = 0; i < number_of_member ; i++)
            {
                try
                {
                    TEST_INFO("Check the profile n°"<<i);
                    ::FT::TagFTGroupTaggedComponent * tag_group 
                      = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),i);
                    if (tag_group->object_group_ref_version >= required_object_group_version)
                    {
                        TEST_SUCCEED();
                        required_object_group_version = tag_group->object_group_ref_version;
                    }
                    else
                        TEST_FAILED();                    

                    try 
                    {
                        ::FT::TagFTPrimaryTaggedComponent tag_primary
                          = m_iogrFactory->decode_profile_with_primary_tag (obj1.in(),i);
                        TEST_INFO("Profile n°"<<i<<" is primary");
                        retval = true;
                    }
                    catch(const CdmwFT::TagNotFound&)
                    {
                        TEST_INFO("Profile n°"<<i<<" is not primary");
                    }
                        
                    delete tag_group;
                    TEST_SUCCEED();
                }
                catch(...)
                {
                    TEST_FAILED();
                }
            }
            
            if (retval == true)
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }

        // create a fault report for a bug on the helloInterface1
        TEST_INFO("Creation of a fault report for the primary member");
        loc.length(1);
        loc[0].id="HOST4";


        notification.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        notification.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        notification.filterable_data.length(2);
        notification.filterable_data[0].name = "FTDomainID";
        notification.filterable_data[0].value <<= ft_domain_id.str().c_str();
        notification.filterable_data[1].name = "Location";
        notification.filterable_data[1].value <<= loc;

        // decrease the number of members
        number_of_member = number_of_member - 1;
        // increase the version of the object group
        required_object_group_version++;
      
        TEST_INFO("Send the fault report to the Fault Notifier");

        try
        {
            fn2->push_structured_fault( notification );
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        Cdmw::OsSupport::OS::sleep(timescale*2000);
    
        // recover the new reference of the object group
        TEST_INFO("Get the new object group reference");
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
            TEST_SUCCEED();
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
        
        loc.length(3);
        loc[0].id="HOST4";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_FAILED();    
        }
        catch( ::FT::MemberNotFound& e )
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        loc[0].id="HOST3";
        loc[1].id="APPL1";
        loc[2].id="PROC1";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        
        loc[0].id="HOST3";
        loc[1].id="APPL1";
        loc[2].id="PROC2";

        try
        {
            obj_ref = rm->get_member_ref(obj1.in() , loc);
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        // Check profile to see if a primary is set
        TEST_INFO("Check if the primary is elected");
        // Check the IOGR profiles 
        {
            CORBA::Boolean retval = false;
            CORBA::ULong nb_profile = m_iogrFactory->profile_count(obj1.in());
            TEST_CHECK( nb_profile == (number_of_member+1));
            
            // we don't check the last profile (it's the replication Manager)
            for (CORBA::ULong i = 0; i < number_of_member ; i++)
            {
                try
                {
                    TEST_INFO("Check the profile n°"<<i);
                    ::FT::TagFTGroupTaggedComponent * tag_group 
                      = m_iogrFactory->decode_profile_with_group_tag (obj1.in(),i);
                    if (tag_group->object_group_ref_version >= required_object_group_version)
                    {
                        TEST_SUCCEED();
                        required_object_group_version = tag_group->object_group_ref_version;
                    }
                    else
                        TEST_FAILED();                    

                    try 
                    {
                        ::FT::TagFTPrimaryTaggedComponent tag_primary
                          = m_iogrFactory->decode_profile_with_primary_tag (obj1.in(),i);
                        TEST_INFO("Profile n°"<<i<<" is primary");
                        retval = true;
                    }
                    catch(const CdmwFT::TagNotFound&)
                    {
                        TEST_INFO("Profile n°"<<i<<" is not primary");
                    }
                        
                    delete tag_group;
                    TEST_SUCCEED();
                }
                catch(...)
                {
                    TEST_FAILED();
                }
            }
            
            if (retval == true)
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }



    }

}; // namespace FT
}; // namespace Cdmw


