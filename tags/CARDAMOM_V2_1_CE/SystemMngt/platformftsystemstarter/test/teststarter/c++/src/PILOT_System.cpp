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


#include <iostream>

#include "teststarter/PILOT_System.hpp"
#include <sstream>
#include <fstream>

#include <list>

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/TestManager.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"

#include "teststarter/SIM_OrbThread.hpp"

#include "idllib/CdmwPlatformMngtSupervisionObserver.skel.hpp"
#include "idllib/CdmwPlatformMngtSystem.stub.hpp"

using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;

namespace {
   const int MAX_RETRY_ON_FTMGR = 4;
   const int SLEEP_RETRY_ON_FTMGR_MS = 1000;
}

CPPUNIT_TEST_SUITE_REGISTRATION( PILOT_System );

namespace Cdmw
{
namespace PlatformMngt
{


    class SupervisionObserver: public virtual POA_CdmwPlatformMngt::SupervisionObserver,
                               public virtual PortableServer::RefCountServantBase
    {
        
    public:
        void notify( CdmwPlatformMngt::Event* an_event )
            throw( CORBA::SystemException )
        {
            CORBA::String_var message = an_event->to_string();
            std::string mes=CORBA::string_dup(message);
            std::cout<<mes<<std::endl;
            
            if (mes.find ("ERR  CDMW_PFMMNG") != mes.npos || mes.find ("WRN  CDMW_PFMMNG")!= mes.npos  )
            {
                m_ew_message=mes;
            }
            m_message = mes;
            
        }       

        std::string get_last_ew_message()
        {
            std::string temp = m_ew_message;
            m_ew_message="";            
            
            return temp;
        }

        std::string get_last_message()
        {
            std::string temp = m_message;
            m_message="";            
            
            return temp;
        }
        
    private :
        std::string m_ew_message;    
        std::string m_message;
        
    };


/**
*Implementation notes: none 
*<p>
*Portability issues: none 
*<p>
*/

PILOT_System::PILOT_System()
    throw()
{

}


PILOT_System::~PILOT_System()
    throw()
{

}


void
PILOT_System::do_tests()                
{
    // set number of requested successfull tests
// //     set_nbOfRequestedTestOK (8);
    
    TEST_INFO("Creating the ORB");
    OrbThread orbThread;
    orbThread.start();
    
	 int timescale = Cdmw::TestUtils::get_timescale();
    OsSupport::OS::sleep(timescale*4000);
    CORBA::ORB_var  orb = orbThread.orb();
    PortableServer::POA_var poa = orbThread.poa();

    OsSupport::OS::ProcessId groupCreatorId = 0;
    std::string daemonArgs(" --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml "); 
    
    try
    {  
        std::string groupCreatorArgs("--CdmwXMLFile=CdmwFaultToleranceManager_conf.xml --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml");
        
        TEST_INFO("Start FTManager and create Group for Supervision process");
        groupCreatorId = OsSupport::OS::create_process("cdmw_ft_manager", groupCreatorArgs);
        OsSupport::OS::sleep(timescale*5000);

        std::string fault_manager_corbaloc = "corbaloc::localhost:4555/fault_manager";

        CORBA::Object_var fault_manager_obj = 
           orb->string_to_object(fault_manager_corbaloc.c_str());
        if (CORBA::is_nil(fault_manager_obj.in()))
        {
            CPPUNIT_ASSERT(false);
            CDMW_THROW2(Cdmw::Exception,
                        Cdmw::Exception::FATAL,
                        "Invalid fault manager corbaloc");    
        }
        CdmwFT::FTManager_var fault_manager;
        try {
            fault_manager = ::CdmwFT::FTManager::_narrow(fault_manager_obj.in());
        } catch (const CORBA::SystemException & ex) {
            std::cerr << "Exception while contacting FTManager: "
                      << ex << "\n"
                      << fault_manager_corbaloc << " is probably invalid"
                      << std::endl;
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(true);

        OsSupport::OS::sleep(timescale*3000);

        TEST_INFO("Starting the PlatformDaemon");
//         daemonArgs += " --FaultManagerRegistration=";
//         daemonArgs += fault_manager_corbaloc;
        TEST_INFO(daemonArgs);
        OsSupport::OS::sleep(timescale*5000);
        OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);   
        OsSupport::OS::sleep(timescale*10000);

        TEST_INFO("Check the corbaloc of CdmwPlatformMngtSupervision Proxy is bound by the daemon");
        OsSupport::OS::sleep(timescale*5000);
        CORBA::Object_var system_proxy_obj
            = orb->string_to_object(
                  "corbaloc::localhost:21869/CdmwPlatformMngtSupervision");
        System_var system_proxy = System::_nil();

        if (CORBA::is_nil (system_proxy_obj.in()))
        {
           CPPUNIT_ASSERT(false);
            CPPUNIT_ASSERT(false);
                CDMW_THROW2(
                    Exception,
                    Exception::FATAL,
                    "Invalid corbaloc" );
        }
        else
        {
           system_proxy = System::_narrow (system_proxy_obj.in());

            if (CORBA::is_nil (system_proxy.in()))
            {
                CPPUNIT_ASSERT(false);
                CDMW_THROW2(
                    Exception,
                    Exception::FATAL,
                    "Invalid system reference" );
            }
            else
                CPPUNIT_ASSERT(true);
        }
   
        TEST_INFO("Starting the System with the Starter");
        std::string supervisionStarterArgs = "--CdmwXMLFile=CdmwPlatformMngtSystemStart.xml --validate";
        TEST_INFO(supervisionStarterArgs);
        OsSupport::OS::sleep(timescale*5000);
        OsSupport::OS::create_process("cdmw_platform_supervision_starter", supervisionStarterArgs);  
    
        TEST_INFO("Sleeping a while to wait for the supervision creation");
        OsSupport::OS::sleep(timescale*15000);

        TEST_INFO("Check the corbaloc of CdmwPlatformMngtSupervision is bound");
        CORBA::Object_var system_obj
            = orb->string_to_object(
                  "corbaloc::localhost:21871/CdmwPlatformMngtSupervision");

        if (CORBA::is_nil (system_obj.in()))
        {
            CPPUNIT_ASSERT(false);
            CDMW_THROW2(
                    Exception,
                    Exception::FATAL,
                    "Invalid corbaloc" );
        }
    
        System_var system = System::_narrow (system_obj.in());

        if (CORBA::is_nil (system.in()))
        {
            CPPUNIT_ASSERT(false);
            CDMW_THROW2(
                    Exception,
                    Exception::FATAL,
                    "Invalid system reference" );
        }
        CPPUNIT_ASSERT(true);
 
        TEST_INFO("Create the observer and register it");  
        OsSupport::OS::sleep(timescale*3000);
        PortableServer::ServantBase_var obs_servantBase = 
           new SupervisionObserver();
        SupervisionObserver* obs_servant = 
           dynamic_cast<SupervisionObserver *> (obs_servantBase.in());
        CdmwPlatformMngt::SupervisionObserver_var observer = 
           obs_servant->_this();
        system_proxy->register_observer ("my_observer", observer.in());

        std::string adminArgs("--system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-define system.xml");
    
        TEST_INFO("Define the system using the System Proxy");
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);    
        OsSupport::OS::sleep(timescale*5000);
        
        TEST_INFO("Snapshot of the system using the System Proxy");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-snapshot";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        TEST_INFO("Start of the system");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-start";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        char *file_name1 = "/tmp/CdmwDaemon_21869/APPL1/PROC11.pid";
        std::ifstream is1(file_name1);
        if (is1.good())
        {
            std::string pid;
            is1 >> pid;
            OsSupport::OS::kill_process (::atoi(pid.c_str())); 
        }
        
        OsSupport::OS::sleep(timescale*8000);

        TEST_INFO("get the message of the observer");    
        std::string last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        std::string hostname = OsSupport::OS::get_hostname();
        std::string message("S:'TEST' A:'APPL1' H:'");
        message += hostname;
        message += "' P:'PROC11' FAILED_DEATH";
        CPPUNIT_ASSERT(last_message.find(message.c_str()) != last_message.npos);

        TEST_INFO("Restart the process");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --proc-initialise APPL1 PROC11 " + hostname;
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --proc-run APPL1 PROC11 " + hostname;
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);
        
        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_message();
        std::cout<<last_message<<std::endl;    
        message = "S:'TEST' A:'APPL1' H:'";
        message += hostname;
        message += "' P:'PROC11' RUNNING";
        CPPUNIT_ASSERT(last_message.find(message.c_str()) != last_message.npos);

        TEST_INFO("Send a fault report to notify a end of the monitoring process");
        ::FT::Location location;
        location.length(3);
        location[0].kind = CORBA::string_dup("hostname");
        location[0].id = CORBA::string_dup(hostname.c_str());
        location[1].kind = CORBA::string_dup("applicationname");
        location[1].id = CORBA::string_dup("APPL1");
        location[2].kind = CORBA::string_dup("processname");
        location[2].id = CORBA::string_dup("PROC11");
        CosNotification::StructuredEvent fault_event;
        fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        fault_event.filterable_data.length(2);
        fault_event.filterable_data[0].name = CORBA::string_dup("FTDomainId");
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = CORBA::string_dup("Location");
        fault_event.filterable_data[1].value <<= location;
        ::FT::FaultNotifier_var fault_notifier = fault_manager->get_fault_notifier();
        fault_notifier->push_structured_fault(fault_event);
        OsSupport::OS::sleep(timescale*4000);


        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        message = "S:'TEST' A:'APPL1' H:'";
        message += hostname;
        message += "' P:'PROC11' FAILED_DEATH";
        CPPUNIT_ASSERT(last_message.find(message.c_str()) != last_message.npos);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --proc-stop APPL1 PROC11 " + hostname;
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        TEST_INFO("Remove the process PROC21");

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --proc-stop APPL2 PROC21 " + hostname;
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --app-remove-process APPL2 PROC21 " + hostname;
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*20000);

        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        CPPUNIT_ASSERT(last_message == "");

        TEST_INFO("Remove the application APPL2");

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --app-stop APPL2";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-remove-application APPL2";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*20000);

        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        CPPUNIT_ASSERT(last_message == "");


        TEST_INFO("Stop the system");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-stop";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

    } 
    catch (...) 
    {
        std::cerr << "unexpected exception raised" << std::endl;
        CPPUNIT_ASSERT(false);
    }
    
    TEST_INFO("Stopping the FTManager");
    try {
        OsSupport::OS::kill_process (groupCreatorId); 
        OsSupport::OS::sleep(timescale*10000);
    }
    catch (...)
    {
    }
    TEST_INFO("FTManager stopped");
            
    TEST_INFO("Stopping the daemon");
    daemonArgs += " stop";
    OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);

    TEST_INFO("Sleeping a while to wait for the daemon to stop");
    OsSupport::OS::sleep(timescale*15000);

    TEST_INFO("Stopping the orb");
    orbThread.shutdown();
    orbThread.join();
    
}    



} // End namespace PlatformMngt
} // End namespace Cdmw


