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


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "FaultTolerance/idllib/CdmwFTFaultDetector.stub.hpp"
#include "testftmanager/CdmwLocalFTManager.stub.hpp"
#include "testftmanager/TestHello.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "testftmanager/FaultManagementTester.hpp"
#include "testftmanager/ServiceRegistration_impl.hpp"
#include "Foundation/idllib/CdmwServiceLocator.skel.hpp"
#include <fstream>
#if CDMW_ORB_VDR == tao
#include <FaultTolerance/ftcommon/TAO_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif
#include <sstream>

namespace Cdmw {
namespace FT {
        

FaultManagementTester::FaultManagementTester(CORBA::ORB_ptr                  orb,
                                             PortableServer::POA_ptr         rootPOA,
                                             const std::string& name)
    : Testable( name ),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(rootPOA))
{
}

FaultManagementTester::~FaultManagementTester()
{
}

void FaultManagementTester::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK( 43 );
    int timescale = Cdmw::TestUtils::Testable::get_timescale();

    try{

        TEST_INFO("Start the fault manager");
        std::string managerArgs(" --CdmwXMLFile=CdmwFaultToleranceManager_test_conf.xml");
        OsSupport::OS::ProcessId ft_manager_id = OsSupport::OS::create_process("cdmw_ft_manager", managerArgs);
        OsSupport::OS::sleep(timescale*15000);


        
        TEST_INFO("Get the fault manager reference");
        CORBA::Object_var fm_obj
        = m_orb->string_to_object("corbaloc::localhost:4555/fault_manager");

        if ( CORBA::is_nil( fm_obj.in() ) )
            CDMW_THROW2(Exception,
                        Exception::FATAL,
                        "Invalid corbaloc" );
        
        ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );
        
        ::CdmwFT::LocalFTManager_var local_fault_manager = ::CdmwFT::LocalFTManager::_narrow( fm_obj.in() );
        
        if ( CORBA::is_nil( local_fault_manager.in() ) )
            std::cout<<"local_fault_manager is nil"<<std::endl;
        


        TEST_INFO("get_FTManager_objects");
        CdmwFT::StateTransfer::DataStoreGroup_ptr ds
            = CdmwFT::StateTransfer::DataStoreGroup::_nil();
        CdmwFT::LocationList list;
        CdmwFT::FTManagerObjects* ftmanagerobject = fault_manager->get_FTManager_objects(Cdmw::OsSupport::OS::get_hostname().c_str(), ds, list);


        ::FT::FaultNotifier_var fault_notifier 
            = ::FT::FaultNotifier::_duplicate(ftmanagerobject->fault_notifier.in());

        if ( CORBA::is_nil( fault_notifier.in() ) )
            CDMW_THROW2(Exception,
                        Exception::FATAL,
                        "Invalid fault_notifier reference" );
        

        CdmwFT::FaultManagement::FaultDetector_var fault_detector 
            = CdmwFT::FaultManagement::FaultDetector::_duplicate(ftmanagerobject->fault_detector.in());
        
        if ( CORBA::is_nil( fault_detector.in() ) )
            CDMW_THROW2(Exception,
                        Exception::FATAL,
                        "Invalid fault_notifier reference" );

        ::CdmwFT::ReplicationManager_var replication_manager
            = ::CdmwFT::ReplicationManager::_duplicate(ftmanagerobject->replication_manager.in());
        
        if ( CORBA::is_nil( replication_manager.in() ) )
            CDMW_THROW2(Exception,
                        Exception::FATAL,
                        "Invalid replication_manager reference" );


        
        // create an IOGRFactory
        TEST_INFO("Create the IOGRFactory");
        Cdmw::FT::IOGRFactory* m_iogrFactory;

#if CDMW_ORB_VDR == tao
        m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(m_orb.in(),
                                                      std::cout);
#endif

        TEST_INFO("Check the fault_notifier is a IOR");
        // Check the IOGR profiles 
        {
            CORBA::ULong nb_profile = m_iogrFactory->profile_count(fault_notifier.in());
            TEST_CHECK( nb_profile == 1);

            try
            {
                ::FT::TagFTGroupTaggedComponent * tag_group 
                    = m_iogrFactory->decode_profile_with_group_tag (fault_notifier.in(),0);
                delete tag_group;
                TEST_FAILED();
            }
            catch(...)
            {
                TEST_SUCCEED();
            }
        }


        // Check the IOGR profiles 
        {
            CORBA::ULong nb_profile = m_iogrFactory->profile_count(fault_detector.in());
            TEST_CHECK( nb_profile == 1);

            try
            {
                ::FT::TagFTGroupTaggedComponent * tag_group 
                    = m_iogrFactory->decode_profile_with_group_tag (fault_detector.in(),0);
                delete tag_group;
                TEST_FAILED();
            }
            catch(...)
            {
                TEST_SUCCEED();
            }
        }

        // Check the IOGR profiles 
        {
            CORBA::ULong nb_profile = m_iogrFactory->profile_count(replication_manager.in());
            TEST_CHECK( nb_profile == 1);

            try
            {
                ::FT::TagFTGroupTaggedComponent * tag_group 
                    = m_iogrFactory->decode_profile_with_group_tag (replication_manager.in(),0);
                delete tag_group;
                TEST_FAILED();
            }
            catch(...)
            {
                TEST_SUCCEED();
            }
        }

        
        TEST_INFO("get the fault_notifier reference from the replication manager (the FTManager in not already master)");
        try
        {
            replication_manager->get_fault_notifier();
            TEST_FAILED();
        }
        catch(const ::FT::InterfaceNotFound&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();    
        }
        
        TEST_INFO("get the fault_detector reference from the replication manager (the FTManager in not already master)");
        try
        {
            replication_manager->get_fault_detector();
            TEST_FAILED();
        }
        catch(const ::FT::InterfaceNotFound&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();    
        }
        
        
        TEST_INFO("get the fault_notifier reference from the fault manager (the FTManager in not already master)");
        try
        {
            fault_manager->get_fault_notifier();
            TEST_FAILED();
        }
        catch(const ::FT::ObjectGroupNotFound& ex)
        {
            TEST_SUCCEED();
        }
        catch(const CORBA::SystemException& ex)
        {
            std::cout<<"ex:"<<ex<<std::endl;
            TEST_FAILED();    
        }
        

        TEST_INFO("get the fault_detector reference from the fault manager (the FTManager in not already master)");
        try
        {
            fault_manager->get_fault_detector();
            TEST_FAILED();
        }
        catch(const ::FT::ObjectGroupNotFound& ex)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();    
        }
        
        TEST_INFO("get the replication_manager reference from the fault manager (the FTManager in not already master)");
        try
        {
            fault_manager->get_replication_manager();
            TEST_FAILED();
        }
        catch(const ::FT::ObjectGroupNotFound& ex)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();    
        }
        

        TEST_INFO("add_location with a unvalide location");
        try
        {
            local_fault_manager->add_location("invalid_loc");
            TEST_FAILED();            
        }
        catch(const ::FT::ObjectNotAdded&)
        {
            TEST_SUCCEED();
        }
        catch(const CORBA::SystemException& ex)
        {
            std::cout<<ex<<std::endl;
            
            TEST_FAILED();
        }
        
        TEST_INFO("activate the ftmanager");
        try
        {
            local_fault_manager->activate();
            TEST_SUCCEED();            
        }
        catch (...)
        {
            TEST_FAILED();            
        }
        
        TEST_INFO("reactivate the ftmanager");
        try
        {
            local_fault_manager->activate();
            TEST_FAILED();
        }
        catch(const CdmwFT::FTManager::AlreadyActivated&)
        {            
            TEST_SUCCEED();            
        }
        catch (...)
        {
            TEST_FAILED();            
        }
            
        

        TEST_INFO("get the fault_notifier reference from the fault manager");
        try
        {
            CORBA::Object_var obj_notifier = fault_manager->get_fault_notifier();

            // Check the IOGR profiles 
            {
                CORBA::ULong nb_profile = m_iogrFactory->profile_count(obj_notifier.in());
                TEST_CHECK( nb_profile == 1);
                
                try
                {
                    ::FT::TagFTGroupTaggedComponent * tag_group 
                      = m_iogrFactory->decode_profile_with_group_tag (obj_notifier.in(),0);
                    delete tag_group;
                    TEST_SUCCEED();
                }
                catch(...)
                {
                    TEST_FAILED();
                }
            }
        }
        catch(...)
        {
            TEST_FAILED();    
        }
        

        TEST_INFO("get the fault_detector reference from the fault manager");
        try
        {
            CORBA::Object_var obj_detector = fault_manager->get_fault_detector();
            // Check the IOGR profiles 
            {
                CORBA::ULong nb_profile = m_iogrFactory->profile_count(obj_detector.in());
                TEST_CHECK( nb_profile == 1);
                
                try
                {
                    ::FT::TagFTGroupTaggedComponent * tag_group 
                      = m_iogrFactory->decode_profile_with_group_tag (obj_detector.in(),0);
                    delete tag_group;
                    TEST_SUCCEED();
                }
                catch(...)
                {
                    TEST_FAILED();
                }
            }
        }
        catch(...)
        {
            TEST_FAILED();    
        }
        
        TEST_INFO("get the replication_manager reference from the fault manager");
        CdmwFT::ReplicationManager_var rm;
        try
        {
            CORBA::Object_var obj_rm = fault_manager->get_replication_manager();

            rm = CdmwFT::ReplicationManager::_narrow(obj_rm.in());
            if (CORBA::is_nil(rm.in()))
            {
                std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                      << "could not _narrow object to type ReplicationManager" << std::endl;
                TEST_FAILED();
            }
                
            // Check the IOGR profiles 
            {
                CORBA::ULong nb_profile = m_iogrFactory->profile_count(obj_rm.in());
                TEST_CHECK( nb_profile == 1);
                
                try
                {
                    ::FT::TagFTGroupTaggedComponent * tag_group 
                      = m_iogrFactory->decode_profile_with_group_tag (obj_rm.in(),0);
                    delete tag_group;
                    TEST_SUCCEED();
                }
                catch(...)
                {
                    TEST_FAILED();
                }
            }
        }
        catch(...)
        {
            TEST_FAILED();    
        }

        
        TEST_INFO("Create object group");
        CORBA::Object_var obj;
        try {
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

    
            ::FT::Location loc1;
            loc1.length(3);
            loc1[0].id = "localhost";
            loc1[0].kind = "hostname";
            loc1[1].id = "APPL";
            loc1[1].kind = "application";
            loc1[2].id = "PROC";
            loc1[2].kind = "process";   
    
            prop[3].nam.length(1);
            prop[3].nam[0].id="org.omg.ft.Factories";
    
            ::FT::FactoryInfos factoryInfos;
            factoryInfos.length(1);
            factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
            factoryInfos[0].the_location = loc1;
            ::FT::Criteria factoryCrit;        
            factoryCrit.length(0);
            factoryInfos[0].the_criteria = factoryCrit;
        
            prop[3].val <<= factoryInfos; 

            const char * rid = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface1:1.0";
            ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
            ::FT::Criteria crit;        
            crit.length(1);
            crit[0].nam.length(1);
            crit[0].nam[0].id="org.omg.ft.FTProperties";
            crit[0].val <<= prop;
            
            obj =  rm->create_object(rid,
                                     crit,
                                     factory_creation_id1.out());            
            TEST_SUCCEED();
        } catch( const CORBA::Exception& e ) {
            std::cerr << "Unexpected exception: " << e << std::endl;;
            TEST_FAILED();
        }
        
        TEST_INFO("Try to narrow object group");
        // try to narrow empty group into HelloInterface1
        CdmwReplicationManager::HelloInterface1_var hello1 =
            CdmwReplicationManager::HelloInterface1::_nil();
        try {
            hello1 = CdmwReplicationManager::HelloInterface1::_narrow(obj.in());

            if (CORBA::is_nil(hello1.in())) {
                std::cout << "Narrow of empty object group into HelloInterface1 failed." << std::endl;
                TEST_FAILED();
            } else {
                TEST_SUCCEED();
            }
        } catch (const CORBA::Exception& e) {
            std::cerr << "Unexpected exception: " << e << std::endl;
            TEST_FAILED();
        }        
        
        // try to narrow empty group into CORBA::Object
        try {
            CORBA::Object_var obj2 =
                CORBA::Object::_narrow(hello1.in());

            if (CORBA::is_nil(obj2.in())) {
                std::cout << "Narrow of empty object group into CORBA::Object failed." << std::endl;
                TEST_FAILED();
            } else {
                TEST_SUCCEED();
            }
        } catch (const CORBA::Exception& e) {
            std::cerr << "Unexpected exception: " << e << std::endl;
            TEST_FAILED();
        }

        // try to narrow empty group into BaseInterface
        try {
            CdmwReplicationManager::BaseInterface_var base =
                CdmwReplicationManager::BaseInterface::_narrow(obj.in());

            if (CORBA::is_nil(base.in())) {
                std::cout << "Narrow of empty object group into BaseInterface failed." << std::endl;
                TEST_FAILED();
            } else {
                TEST_SUCCEED();
            }
        } catch (const CORBA::Exception& e) {
            std::cerr << "Unexpected exception: " << e << std::endl;
            TEST_FAILED();
        }
        
        // try to narrow empty group into HelloInterface2
        try {
            CdmwReplicationManager::HelloInterface2_var hello2 =
                CdmwReplicationManager::HelloInterface2::_narrow(obj.in());

            if (CORBA::is_nil(hello2.in())) {
                TEST_SUCCEED();
            } else {
                std::cout << "Narrow of empty object group into HelloInterface2 succeed but shouldn't !!" << std::endl;
                TEST_FAILED();
            }
        } catch (const CORBA::Exception& e) {
            std::cerr << "Unexpected exception: " << e << std::endl;
            TEST_FAILED();
        }
        

        TEST_INFO("Try to call operation on an empty group");
        try {
            hello1->hello();
            std::cout << "Succeed to call hello() on empty group !!" << std::endl; 
            TEST_FAILED();
        } catch (const CORBA::TRANSIENT& e) {
            TEST_SUCCEED();
        } catch (const CORBA::SystemException& e) {
            std::cout << "Unexpected exception (CORBA::TRANSIENT was expected):\n" << e << std::endl; 
            TEST_FAILED();
        }
        
        
        TEST_INFO("Remove an unknown location");
        try
        {
            local_fault_manager->remove_location("unknown_loc");
            TEST_FAILED();
        }
        catch(const ::FT::MemberNotFound&)
        {
            TEST_SUCCEED();            
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        
        TEST_INFO("kill the replication manager");            
        OsSupport::OS::kill_process(ft_manager_id);
        OsSupport::OS::sleep(timescale*5000);



        TEST_INFO("Start the fault manager");
        std::string managerArgs2(" --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml  --validateXML");
        OsSupport::OS::ProcessId ft_manager_id2 = OsSupport::OS::create_process("cdmw_ft_manager", managerArgs2);
        OsSupport::OS::sleep(timescale*15000);




        TEST_INFO("create a service registration");
        Cdmw::FT::ServiceRegistration_impl* registration_obj = new Cdmw::FT::ServiceRegistration_impl();

        std::ostringstream registration_service_address;
        registration_service_address << "corbaloc:miop:1.0@1.0-cdmw" << Cdmw::OsSupport::OS::get_logname() << "ftdomain-6/225.1.1.8:4561" << std::ends;
        
        CORBA::Object_var registration_service_group = m_orb->string_to_object(registration_service_address.str().c_str());
        
        PortableServer::ObjectId_var registration_id = m_poa->create_id_for_reference(registration_service_group.in());
        m_poa->activate_object_with_id (registration_id.in (),registration_obj);

        CdmwServiceLocator::ServiceRegistration_var registration = registration_obj->_this();

        std::ostringstream multicast_addr;
        multicast_addr << "corbaloc:miop:1.0@1.0-cdmw" << Cdmw::OsSupport::OS::get_logname() << "ftdomain-5/225.1.1.8:4560" << std::ends;
        std::cout<<multicast_addr.str().c_str()<<std::endl;
        CORBA::Object_var group = m_orb->string_to_object(multicast_addr.str().c_str());

        CdmwServiceLocator::ServiceLocator_var  locator = CdmwServiceLocator::ServiceLocator::_unchecked_narrow (group.in());

        
        registration_obj->reset();
        locator->locate_service ("FaultNotifier",registration_service_address.str().c_str());
        OsSupport::OS::sleep(timescale*2000);
        
        CORBA::Object_var res = registration_obj->get_service();
        TEST_CHECK(!CORBA::is_nil( res.in() ) );
        
        ::FT::FaultNotifier_var fault_notifier2 
          = ::FT::FaultNotifier::_narrow(res.in());
        
        TEST_CHECK(!CORBA::is_nil( fault_notifier2.in() ) );


        try
        {
            fault_notifier2->disconnect_consumer( 5555 );
            TEST_FAILED();
        }
        catch(const CosEventComm::Disconnected& )
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }


        registration_obj->reset();
        locator->locate_service ("FaultDetector",registration_service_address.str().c_str());
        OsSupport::OS::sleep(timescale*2000);
        
        res = registration_obj->get_service();
        TEST_CHECK(!CORBA::is_nil( res.in() ) );
        
        CdmwFT::FaultManagement::FaultDetector_var fault_detector1 
        = CdmwFT::FaultManagement::FaultDetector::_narrow(res.in());
        TEST_CHECK(!CORBA::is_nil( fault_detector1.in() ) );
        
        try
        {
            fault_detector1->set_fault_notifier(fault_notifier2.in());
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

 
        registration_obj->reset();
        locator->locate_service ("ReplicationManager",registration_service_address.str().c_str());
        OsSupport::OS::sleep(timescale*2000);
        
        res = registration_obj->get_service();
        TEST_CHECK(!CORBA::is_nil( res.in() ) );

        ::CdmwFT::ReplicationManager_var replication_manager2
          = ::CdmwFT::ReplicationManager::_narrow(res.in());

        TEST_CHECK(!CORBA::is_nil( replication_manager2.in() ) );

        try
        {
            replication_manager2->get_ft_domain_id();
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }
        

        TEST_INFO("kill the replication manager");            
        OsSupport::OS::kill_process(ft_manager_id2);
        OsSupport::OS::sleep(timescale*5000);

        
        {
            
        TEST_INFO("Start the fault manager without xml file");
        std::string managerArgs1_("  ");
        OsSupport::OS::create_process("start_ft_manager.sh", managerArgs1_);
        OsSupport::OS::sleep(timescale*5000);


        std::ifstream ifs("test.txt");
        int  size = 1000;
        char buf[size] ;
        if (ifs.good())
        {
            std::ostream result(ifs.rdbuf());
            bool res = false;
            while (ifs.getline(buf, size))
            {
                std::cout<<buf<<std::endl;
                if (!strcmp(buf,"==>The xml configuration file must be specified" ));
                    res = true;
            }
            TEST_CHECK(res);
            ifs.close();
        }
        OsSupport::OS::unlink("test.txt");
        }
        {
            
        TEST_INFO("Start the fault manager without xml file");
        std::string managerArgs2_(" --CdmwXMLFile ");
        OsSupport::OS::create_process("start_ft_manager.sh", managerArgs2_);
        OsSupport::OS::sleep(timescale*5000);
        std::ifstream ifs("test.txt");
        int  size = 100;
        char buf[size] ;
        if (ifs.good())
        {
            std::ostream result(ifs.rdbuf());
            bool res = false;
            while (ifs.getline(buf, size))
            {
                std::cout<<buf<<std::endl;
                if (!strcmp(buf,"==>The xml configuration file must be specified" ));
                    res = true;
            }
            TEST_CHECK(res);
            ifs.close();
        }
        OsSupport::OS::unlink("test.txt");
        }
        {
            

        TEST_INFO("Start the fault manager with a bad xml file (the xml contains unknown element");
        std::string managerArgs3_(" --CdmwXMLFile=CdmwFaultToleranceManager_test2_conf.xml --validateXML ");
        OsSupport::OS::create_process("start_ft_manager.sh", managerArgs3_);
        OsSupport::OS::sleep(timescale*5000);
        std::ifstream ifs("test.txt");
        int  size = 100;
        char buf[size] ;
        if (ifs.good())
        {
            std::ostream result(ifs.rdbuf());
            bool res = false;
            while (ifs.getline(buf, size))
            {
                std::cout<<buf<<std::endl;
                if (!strcmp(buf,"Unknown element 'SystemPorts'" ))
                    res = true;
            }
            TEST_CHECK(res);
            ifs.close();
        }
        OsSupport::OS::unlink("test.txt");
        }
        {
            
        TEST_INFO("Start the fault manager with a bad xml file (the dtd contains missing element");
        std::string managerArgs4_(" --CdmwXMLFile=CdmwFaultToleranceManager_test3_conf.xml --validateXML ");
        OsSupport::OS::create_process("start_ft_manager.sh", managerArgs4_);
        OsSupport::OS::sleep(timescale*5000);
        std::ifstream ifs("test.txt");
        int  size = 100;
        char buf[size] ;
        if (ifs.good())
        {
            std::ostream result(ifs.rdbuf());
            bool res = false;
            while (ifs.getline(buf, size))
            {
                std::cout<<buf<<std::endl;
                if (!strcmp(buf,"Unknown element 'SystemPort'" ));
                    res = true;
            }
            TEST_CHECK(res);
            ifs.close();
        }
        OsSupport::OS::unlink("test.txt");
        }
        {
            
        TEST_INFO("Start the fault manager with a bad xml file (the xml root element is not the good");
        std::string managerArgs5_(" --CdmwXMLFile=CdmwFaultToleranceManager_test4_conf.xml --validateXML ");
        OsSupport::OS::create_process("start_ft_manager.sh", managerArgs5_);
        OsSupport::OS::sleep(timescale*5000);
        std::ifstream ifs("test.txt");
        int  size = 100;
        char buf[size] ;
        if (ifs.good())
        {
            std::ostream result(ifs.rdbuf());
            bool res = false;
            while (ifs.getline(buf, size))
            {
                std::cout<<buf<<std::endl;
                if (!strcmp(buf,"==>The XML file used is not the good type of file for the FT Manager" ));
                    res = true;
            }
            TEST_CHECK(res);
            ifs.close();
        }
        OsSupport::OS::unlink("test.txt");
        }
        {
            
        TEST_INFO("Start the fault manager with a bad xml file (the doctype is not the good");
        std::string managerArgs6_(" --CdmwXMLFile=CdmwFaultToleranceManager_test5_conf.xml --validateXML ");
        OsSupport::OS::create_process("start_ft_manager.sh", managerArgs6_);
        OsSupport::OS::sleep(timescale*5000);
        std::ifstream ifs("test.txt");
        int  size = 100;
        char buf[size] ;
        if (ifs.good())
        {
            std::ostream result(ifs.rdbuf());
            bool res = false;
            while (ifs.getline(buf, size))
            {
                std::cout<<buf<<std::endl;
                if (!strcmp(buf,"Root element different from DOCTYPE" ));
                    res = true;
            }
            TEST_CHECK(res);
            ifs.close();
        }
        OsSupport::OS::unlink("test.txt");
        
        }
        
        TEST_INFO("Start the fault manager to print the help");
        std::string managerArgs6_(" --help ");
        OsSupport::OS::create_process("start_ft_manager.sh", managerArgs6_);
        OsSupport::OS::sleep(timescale*5000);
        TEST_SUCCEED();
        

    }
    catch(const CORBA::SystemException& ex)
    {
        std::cout<<"<<<TEST_FAILED>>> : "<<ex<<std::endl;            
    }    
    catch(...)
    {
        std::cout<<"<<<TEST_FAILED>>>"<<std::endl;            
    }
}

}; // namespace FT
}; // namespace Cdmw

