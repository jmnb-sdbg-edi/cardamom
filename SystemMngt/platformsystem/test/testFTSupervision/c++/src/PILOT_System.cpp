/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include <iostream>

#include "testFTSupervision/PILOT_System.hpp"
#include <sstream>

#include <list>
#include <fstream>

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "testFTSupervision/SIM_OrbThread.hpp"
#include "testFTSupervision/SIM_FaultNotifier.hpp"
#include "testFTSupervision/SIM_FaultDetector.hpp"
#include "testFTSupervision/SIM_FaultManager.hpp"
#include "FaultTolerance/ftreplicationmanager/InitUtils.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"

#include "idllib/CdmwPlatformMngtSupervisionObserver.skel.hpp"
#include "idllib/CdmwPlatformMngtSystem.stub.hpp"

using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;


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
    : Testable("Cdmw::System")
{

}


PILOT_System::~PILOT_System()
    throw()
{

}


void
PILOT_System::do_tests()                
    throw()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (11);
    
    
    TEST_INFO("Creating the ORB");
    OrbThread orbThread;
    orbThread.start();
    
	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OsSupport::OS::sleep(timescale*4000);
    CORBA::ORB_var  orb = orbThread.orb();
    PortableServer::POA_var poa = orbThread.poa();


    TEST_INFO("Create the ftdetector");
    PortableServer::ServantBase_var fd_servantBase = 
                           new Cdmw::FT::SIM_FaultDetector();
    Cdmw::FT::SIM_FaultDetector *fd_servant = 
                           dynamic_cast<Cdmw::FT::SIM_FaultDetector *> (fd_servantBase.in());
    CdmwFT::FaultManagement::FaultDetector_var fault_detector = fd_servant->_this();
    

    TEST_INFO("Create the ftmanager");  
    PortableServer::ServantBase_var fm_servantBase =   
                           new Cdmw::FT::SIM_FaultManager(orb.in(), poa.in(), fault_detector.in());
    
    Cdmw::FT::SIM_FaultManager* fm_servant = 
                           dynamic_cast<Cdmw::FT::SIM_FaultManager *> (fm_servantBase.in());
    CdmwFT::FTManager_var fault_manager = fm_servant->_this();

    
    std::string supervisionArgs("--creation-timeout=20000 --event-timeout=6000 --RequestDurationTime=20000000 --FaultManagerRegistration=");
    supervisionArgs += orb->object_to_string(fault_manager.in());
    supervisionArgs += " ";
    supervisionArgs += Cdmw::Common::Options::LOG_FILE_OPTION;    
    supervisionArgs += "=log.txt";
    
    std::string daemonArgs(" --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml "); 
    
    OsSupport::OS::ProcessId supervisionId = 0;
    bool supervisionStarted = false;
    
    try
    {  
        TEST_INFO("Starting the PlatformDaemon");
        OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);   
    
        TEST_INFO("Starting the System");
        supervisionId = OsSupport::OS::create_process("cdmw_platform_supervision", supervisionArgs);  
        supervisionStarted = true;  
    
        TEST_INFO("Sleeping a while to wait for the supervision creation");
        OsSupport::OS::sleep(timescale*15000);

        TEST_INFO("create the observer and register it");  
        PortableServer::ServantBase_var obs_servantBase = new SupervisionObserver();
    
        SupervisionObserver* obs_servant = 
                           dynamic_cast<SupervisionObserver *> (obs_servantBase.in());
        CdmwPlatformMngt::SupervisionObserver_var observer = obs_servant->_this();
    
        CORBA::Object_var system_obj
            = orb->string_to_object("corbaloc::localhost:21871/CdmwPlatformMngtSupervision");

        if (CORBA::is_nil (system_obj.in()))
            CDMW_THROW2(
                    Exception,
                    Exception::FATAL,
                    "Invalid corbaloc" );
    
        System_var system = System::_narrow (system_obj.in());

        if (CORBA::is_nil (system.in()))
            CDMW_THROW2(
                    Exception,
                    Exception::FATAL,
                    "Invalid system reference" );

        system->register_observer ("my_observer", observer.in());

        ::FT::FaultNotifier_var fault_notifier = fault_manager->get_fault_notifier();


        std::string adminArgs("--system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-define system.xml");
    
        TEST_INFO("Define the system");
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);    
        OsSupport::OS::sleep(timescale*15000);

        TEST_INFO("check if the start monitoring is called by the supervision");    
        TEST_CHECK(fd_servant->start_monitoring_is_called());    
        OsSupport::OS::unlink("start_monitoring.stt");

        TEST_INFO("Send a fault report to notify a host not responding");    
        ::FT::Location location;
        location.length(1);
        location[0].kind = CORBA::string_dup("hostname");
        location[0].id = CORBA::string_dup("localhost");
    
        // Prepare the event for the fault notification
        CosNotification::StructuredEvent fault_event;
        fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        fault_event.filterable_data.length(2);
        fault_event.filterable_data[0].name = CORBA::string_dup("FTDomainId");
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = CORBA::string_dup("Location");
        fault_event.filterable_data[1].value <<= location;
        fault_notifier->push_structured_fault(fault_event);
        OsSupport::OS::sleep(timescale*4000);
    
        TEST_INFO("get the message of the observer");   
        std::string last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        TEST_CHECK(last_message.find("H:'localhost' NOT_RESPONDING") != last_message.npos);


        fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        fault_event.filterable_data.length(2);
        fault_event.filterable_data[0].name = CORBA::string_dup("FTDomainId");
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = CORBA::string_dup("Location");
        fault_event.filterable_data[1].value <<= location;
        fault_notifier->push_structured_fault(fault_event);
        OsSupport::OS::sleep(timescale*4000);
    
        TEST_INFO("get the message of the observer");   
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        TEST_CHECK(last_message.size() == 0);


        TEST_INFO("Send a fault report to notify a host responding"); 
        fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectResponding");
        fault_event.filterable_data.length(2);
        fault_event.filterable_data[0].name = CORBA::string_dup("FTDomainId");
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = CORBA::string_dup("Location");
        fault_event.filterable_data[1].value <<= location;
        fault_notifier->push_structured_fault(fault_event);
        OsSupport::OS::sleep(timescale*4000);

        TEST_INFO("get the message of the observer");
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        TEST_CHECK(last_message.find("H:'localhost' RESPONDING") != last_message.npos);

        TEST_INFO("Send a fault report to notify a host responding"); 
        fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectResponding");
        fault_event.filterable_data.length(2);
        fault_event.filterable_data[0].name = CORBA::string_dup("FTDomainId");
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = CORBA::string_dup("Location");
        fault_event.filterable_data[1].value <<= location;
        fault_notifier->push_structured_fault(fault_event);
        OsSupport::OS::sleep(timescale*4000);

        TEST_INFO("get the message of the observer");
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        TEST_CHECK(last_message.size() == 0);

        TEST_INFO("Start of the system");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-start";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-snapshot";
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
        

        OsSupport::OS::sleep(timescale*4000);

        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        TEST_CHECK(last_message.find("S:'TEST' A:'APPL1' P:'PROC11' FAILED_DEATH") != last_message.npos);

        TEST_INFO("Restart the process");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --proc-initialise APPL1 PROC11";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --proc-run APPL1 PROC11";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);
        
        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_message();
        std::cout<<last_message<<std::endl;    
        TEST_CHECK(last_message.find("S:'TEST' A:'APPL1' P:'PROC11' RUNNING") != last_message.npos);


        TEST_INFO("Send a fault report to notify a end of the monitoring process");
        location.length(3);
        location[0].kind = CORBA::string_dup("hostname");
        location[0].id = CORBA::string_dup("localhost");
        location[1].kind = CORBA::string_dup("applicationname");
        location[1].id = CORBA::string_dup("APPL1");
        location[2].kind = CORBA::string_dup("processname");
        location[2].id = CORBA::string_dup("PROC11");
        fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        fault_event.filterable_data.length(2);
        fault_event.filterable_data[0].name = CORBA::string_dup("FTDomainId");
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = CORBA::string_dup("Location");
        fault_event.filterable_data[1].value <<= location;
        fault_notifier->push_structured_fault(fault_event);
        OsSupport::OS::sleep(timescale*4000);


        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        TEST_CHECK(last_message.find("S:'TEST' A:'APPL1' P:'PROC11' FAILED_DEATH") != last_message.npos);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --proc-stop APPL1 PROC11";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        TEST_INFO("Send a fault report to notify a end of the agent");
        location.length(2);
        location[0].kind = CORBA::string_dup("hostname");
        location[0].id = CORBA::string_dup("localhost");
        location[1].kind = CORBA::string_dup("applicationname");
        location[1].id = CORBA::string_dup("APPL1");
        fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup("FT_CORBA");
        fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup("ObjectCrashFault");
        fault_event.filterable_data.length(2);
        fault_event.filterable_data[0].name = CORBA::string_dup("FTDomainId");
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = CORBA::string_dup("Location");
        fault_event.filterable_data[1].value <<= location;
        fault_notifier->push_structured_fault(fault_event);
        OsSupport::OS::sleep(timescale*4000);

        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        TEST_CHECK(last_message.find("S:'TEST' A:'APPL1'") != last_message.npos && 
                   last_message.find("P:'PROC10'") == last_message.npos);





        TEST_INFO("Remove the process PROC21");

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --proc-stop APPL2 PROC21";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --app-remove-process APPL2 PROC21";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*20000);

        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        TEST_CHECK(last_message == "");

        TEST_INFO("Remove the application APPL2");

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --app-stop APPL2";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-remove-application APPL2";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*20000);

        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        TEST_CHECK(last_message == "");


        TEST_INFO("Stop the system");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-stop";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);


    } 
    catch (...) 
    {
        std::cerr << "unexpected exception raised" << std::endl;
        TEST_FAILED();
    }
    
             
    TEST_INFO("Stopping the daemon");
    daemonArgs += " stop";
    OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);

    TEST_INFO("Sleeping a while to wait for the daemon to stop");
    OsSupport::OS::sleep(timescale*15000);

    TEST_INFO("Stopping the supervision");
    
    if (supervisionStarted)
    {
        try
        { 
             OsSupport::OS::kill_process (supervisionId); 
             OsSupport::OS::sleep(timescale*10000);
        }
        catch (...) 
        {
        }
    }

    TEST_INFO("Stopping the orb");
    orbThread.shutdown();
    orbThread.join();
    
}    



} // End namespace PlatformMngt
} // End namespace Cdmw


