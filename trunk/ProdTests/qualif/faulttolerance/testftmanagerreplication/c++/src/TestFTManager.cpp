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


#include <exception>
#include <iostream>
#include <sstream>


#include "idllib/CdmwFTReplicationManager.stub.hpp"

#include "testftmanagerreplication/TestHello_impl.hpp"
#include <testftmanagerreplication/SIM_consumer.hpp>
#include "testftmanagerreplication/TestFTManager.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"


namespace Cdmw {
namespace FT {

// constructor
TestFTManager::TestFTManager(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name,
    const std::string& process_name,
    const std::string& host1,
    const std::string& host2)
    : Testable( name ),
      m_process_name(process_name),
      m_host1(host1),
      m_host2(host2)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

//destructor
TestFTManager::~TestFTManager()
{
}


// do_tests
void TestFTManager::do_tests()
{
    set_nbOfRequestedTestOK (8);


    try
    {


        TEST_INFO( "Resolve the Replication Manager reference" );

        CORBA::Object_var objref = m_orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());
        
        if (CORBA::is_nil(rm.in()))
        {
            std::cout << "File : " << __FILE__ << " Line : " << __LINE__
                      << "could not _narrow object to type HelloInterface" << std::endl;
            abort();
        }
        
        TEST_SUCCEED();


        
        TEST_INFO( "Resolve the Fault Notifier reference" );
        ::FT::FaultNotifier_var fault_notifier
            = rm->get_fault_notifier();        
        TEST_SUCCEED();

        TEST_INFO( "Resolve the Fault Detector reference" );
        CdmwFT::FaultManagement::FaultDetector_var fault_detector
            = rm->get_fault_detector();
        TEST_SUCCEED();



        TEST_INFO( "Get the ftmanger references");
        std::string corbaloc_ft_manager_primary = "corbaloc::";
        corbaloc_ft_manager_primary += m_host1;
        corbaloc_ft_manager_primary += ":4555/fault_manager";
        
        std::cout<<corbaloc_ft_manager_primary<<std::endl;
        
        CORBA::Object_var pfm_obj
        = m_orb->string_to_object(corbaloc_ft_manager_primary.c_str());
        
        if ( CORBA::is_nil( pfm_obj.in() ) )
            CDMW_THROW2(Exception,
                        Exception::FATAL,
                        "Invalid corbaloc" );
        
        ::CdmwFT::FTManager_var primary_fault_manager = ::CdmwFT::FTManager::_narrow( pfm_obj.in() );
        std::cout<<m_orb->object_to_string(primary_fault_manager.in())<<std::endl;
        
        std::string corbaloc_ft_manager_backup = "corbaloc::";
        corbaloc_ft_manager_backup += m_host2;
        corbaloc_ft_manager_backup += ":4555/fault_manager";

        std::cout<<corbaloc_ft_manager_backup<<std::endl;

        CORBA::Object_var bfm_obj
        = m_orb->string_to_object(corbaloc_ft_manager_backup.c_str());
        
        if ( CORBA::is_nil( bfm_obj.in() ) )
            CDMW_THROW2(Exception,
                        Exception::FATAL,
                        "Invalid corbaloc" );
        
        std::cout<<m_orb->object_to_string(bfm_obj.in())<<std::endl;
        ::CdmwFT::FTManager_var backup_fault_manager = ::CdmwFT::FTManager::_narrow( bfm_obj.in() );
        std::cout<<m_orb->object_to_string(backup_fault_manager.in())<<std::endl;

        
        TEST_INFO("----------------------------");
        TEST_INFO(" The FaultNotifier Part");
        TEST_INFO("----------------------------");


        TEST_INFO("Realise a dump in the primary ftmanager and on the backup");

        primary_fault_manager->dump_datastore(CdmwFT::FAULT_NOTIFIER);
        backup_fault_manager->dump_datastore(CdmwFT::FAULT_NOTIFIER);

        TEST_INFO("Check if the two faultNotifiers contain the same datas");
        TEST_INFO("Press any key and Enter");
        std::string dummy;
        std::cin >> dummy;

        TEST_INFO("Add a new consumer on the primary FaultManager");
        Cdmw::FT::SIM_consumer* consumer_obj = new Cdmw::FT::SIM_consumer(1);
        CosNotifyComm::StructuredPushConsumer_var consumer = consumer_obj->_this();

        fault_notifier->connect_structured_fault_consumer(consumer.in());

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*1000);

        TEST_INFO("Realise a dump in the primary ftmanager and on the backup");

        primary_fault_manager->dump_datastore(CdmwFT::FAULT_NOTIFIER);
        backup_fault_manager->dump_datastore(CdmwFT::FAULT_NOTIFIER);

        TEST_INFO("Check if the two faultNotifiers contain the same datas");
        TEST_INFO("and if the new consumer is registered");
        TEST_INFO("Press any key and Enter");
        std::cin >> dummy;
       
        TEST_INFO("Realise a bad operation on the FaultNotifier");
        try 
        {
            fault_notifier->disconnect_consumer( 100 );
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

        TEST_INFO("Realise a dump in the primary ftmanager and on the backup");

        primary_fault_manager->dump_datastore(CdmwFT::FAULT_NOTIFIER);
        backup_fault_manager->dump_datastore(CdmwFT::FAULT_NOTIFIER);

        TEST_INFO("and check data are the same as the previous");
        TEST_INFO("Press any key and Enter");
        std::cin >> dummy;


        TEST_INFO("----------------------------");
        TEST_INFO(" The FaultDetector Part");
        TEST_INFO("----------------------------");

        TEST_INFO("Realise a dump in the primary ftmanager and on the backup");

        primary_fault_manager->dump_datastore(CdmwFT::FAULT_DETECTOR);
        backup_fault_manager->dump_datastore(CdmwFT::FAULT_DETECTOR);

        TEST_INFO("Check if the two faultDetectors contain the same datas");
        TEST_INFO("Press any key and Enter");
        std::cin >> dummy;

        TEST_INFO("Add a new host2 to monitor");

        ::FT::Location location;
        location.length(1);
        location[0].kind="hostname";
        location[0].id=m_host2.c_str();
    
        ::FT::FaultMonitoringIntervalAndTimeoutValue time;
        time.monitoring_interval=0;
        time.timeout=0;    
        CdmwFT::FaultManagement::MonitoringId monitoringID;
        monitoringID = fault_detector->start_monitoring(::FT::PullMonitorable::_nil(),
                                                        location,
                                                        time);
        
        
        
        TEST_INFO("Realise a dump in the primary ftmanager and on the backup");

        primary_fault_manager->dump_datastore(CdmwFT::FAULT_DETECTOR);
        backup_fault_manager->dump_datastore(CdmwFT::FAULT_DETECTOR);

        TEST_INFO("Check if the two faultDetectors contain the same datas");
        TEST_INFO("and if the new host is registered");
        TEST_INFO("Press any key and Enter");
        std::cin >> dummy;


        TEST_INFO("Realise a bad operation on the FaultDetector");
        try 
        {
            fault_detector->stop_monitoring (monitoringID + 1);
            TEST_FAILED();
        }
        catch(const CdmwFT::FaultManagement::InvalidMonitoringId&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_INFO("Realise a dump in the primary ftmanager and on the backup");

        primary_fault_manager->dump_datastore(CdmwFT::FAULT_DETECTOR);
        backup_fault_manager->dump_datastore(CdmwFT::FAULT_DETECTOR);

        TEST_INFO("and check data are the same as the previous");
        TEST_INFO("Press any key and Enter");
        std::cin >> dummy;





        TEST_INFO("----------------------------");
        TEST_INFO(" The ReplicationManager Part");
        TEST_INFO("----------------------------");


        // create the object groups
        TEST_INFO("Create objects groups");

        const char * rid1 = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface:1.0";

        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id2;

        CORBA::Object_var obj1;
        CORBA::Object_var obj2;
        
        // set default properties
        TEST_INFO( "set_default_properties" );
    
        ::FT::Properties prop1;
    
        prop1.length(2);
        prop1[0].nam.length(1);
        prop1[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop1[0].val <<= (CORBA::UShort)2;

        ::FT::Locations locs(2);
        locs.length(2);
        locs[0].length(3);
        locs[0][0].id = m_host1.c_str();
        locs[0][0].kind = "hostname";
        locs[0][1].id = "APPL1";
        locs[0][1].kind = "applicationname";
        locs[0][2].id = "P11";
        locs[0][2].kind = "processname";

        locs[1].length(3);
        locs[1][0].id = m_host2.c_str();
        locs[1][0].kind = "hostname";
        locs[1][1].id = "APPL2";
        locs[1][1].kind = "applicationname";
        locs[1][2].id = "P21";
        locs[1][2].kind = "processname";

        const CORBA::ULong factory_infos_len = locs.length();
        ::FT::FactoryInfos factoryInfos;
        factoryInfos.length(factory_infos_len);
        for (CORBA::ULong i = 0; i < factory_infos_len; ++i) 
        {
            factoryInfos[i].the_factory = ::FT::GenericFactory::_nil();
            std::cout << '[' << i << "] " << " --- " 
                      << Cdmw::CommonSvcs::Naming::NamingInterface::to_string
                           (locs[i]) << std::endl;

            factoryInfos[i].the_location = locs[i];
            ::FT::Criteria factoryCrit;        
            factoryCrit.length(0);
            factoryInfos[i].the_criteria = factoryCrit;
        }

        prop1[1].nam.length(1);
        prop1[1].nam[0].id="org.omg.ft.Factories";
        prop1[1].val <<= factoryInfos;


        ::FT::Criteria crit1;        
        crit1.length(1);
        crit1[0].nam.length(1);
        crit1[0].nam[0].id="org.omg.ft.FTProperties";
        crit1[0].val <<= prop1;

        try {
            obj1 =  rm->create_object(rid1,
                                      crit1,
                                      factory_creation_id1.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*4000);

        TEST_INFO("Realise a dump in the primary ftmanager and on the backup");

        primary_fault_manager->dump_datastore(CdmwFT::REPLICATION_MANAGER);
        backup_fault_manager->dump_datastore(CdmwFT::REPLICATION_MANAGER);

        
        TEST_INFO("Check if the two replication managers contain the same datas");
        TEST_INFO("Press any key and Enter");
        std::cin >> dummy;

        TEST_INFO("Add a new objectGroup");

        // set default properties
        TEST_INFO( "set_default_properties" );
    
        ::FT::Properties prop2;
    
        prop2.length(2);
        prop2[0].nam.length(1);
        prop2[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop2[0].val <<= (CORBA::UShort)2;

        ::FT::Locations locs2(2);
        locs2.length(2);
        locs2[0].length(3);
        locs2[0][0].id = m_host1.c_str();
        locs2[0][0].kind = "hostname";
        locs2[0][1].id = "APPL1";
        locs2[0][1].kind = "applicationname";
        locs2[0][2].id = "P12";
        locs2[0][2].kind = "processname";

        locs2[1].length(3);
        locs2[1][0].id = m_host2.c_str();
        locs2[1][0].kind = "hostname";
        locs2[1][1].id = "APPL2";
        locs2[1][1].kind = "applicationname";
        locs2[1][2].id = "P22";
        locs2[1][2].kind = "processname";

        factoryInfos.length(factory_infos_len);
        for (CORBA::ULong i = 0; i < factory_infos_len; ++i) 
        {
            factoryInfos[i].the_factory = ::FT::GenericFactory::_nil();
            std::cout << '[' << i << "] " << " --- " 
                      << Cdmw::CommonSvcs::Naming::NamingInterface::to_string
                           (locs2[i]) << std::endl;

            factoryInfos[i].the_location = locs2[i];
            ::FT::Criteria factoryCrit;        
            factoryCrit.length(0);
            factoryInfos[i].the_criteria = factoryCrit;
        }

        prop2[1].nam.length(1);
        prop2[1].nam[0].id="org.omg.ft.Factories";
        prop2[1].val <<= factoryInfos;


        ::FT::Criteria crit2;        
        crit2.length(1);
        crit2[0].nam.length(1);
        crit2[0].nam[0].id="org.omg.ft.FTProperties";
        crit2[0].val <<= prop2;

        try {
            obj2 =  rm->create_object(rid1,
                                      crit2,
                                      factory_creation_id2.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*4000);

        TEST_INFO("Realise a dump in the primary ftmanager and on the backup");

        primary_fault_manager->dump_datastore(CdmwFT::REPLICATION_MANAGER);
        backup_fault_manager->dump_datastore(CdmwFT::REPLICATION_MANAGER);

        
        TEST_INFO("Check if the two replication managers contain the same datas");
        TEST_INFO("and if the new objectgroup is registered");
        TEST_INFO("Press any key and Enter");
        std::cin >> dummy;


        TEST_INFO("Realise a add_member on the second objectgroup");
        TEST_INFO("but the location was not already register");

        Cdmw::HelloInterface_impl* hello_obj = new Cdmw::HelloInterface_impl();
        CdmwReplicationManager::HelloInterface_var hello = hello_obj->_this();
        
        try
        {
            rm->add_member(obj2.in(), locs2[0], hello.in());
            TEST_FAILED();
        }
        catch(const ::FT::ObjectNotAdded&)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        OsSupport::OS::sleep(timescale*2000);

        TEST_INFO("Realise a dump in the primary ftmanager and on the backup");

        primary_fault_manager->dump_datastore(CdmwFT::REPLICATION_MANAGER);
        backup_fault_manager->dump_datastore(CdmwFT::REPLICATION_MANAGER);

        
        TEST_INFO("Check if the two replication managers contain the same datas");
        TEST_INFO("Press any key and Enter");
        std::cin >> dummy;


        OsSupport::OS::sleep(timescale*2000);
    }
    catch(const CORBA::Exception& e ) {
        std::cerr << e._name() << std::endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e ) {
        std::cerr << e.what() << std::endl;
        TEST_FAILED();
    }

}

}; // namespace FT
}; // namespace Cdmw

