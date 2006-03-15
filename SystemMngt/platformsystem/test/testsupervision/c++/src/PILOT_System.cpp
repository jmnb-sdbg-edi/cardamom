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
#include <sstream>
#include <fstream>
#include <list>


#include "testsupervision/PILOT_System.hpp"
#include "testsupervision/SIM_OrbThread.hpp"

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/ossupport/OS.hpp"


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

CPPUNIT_TEST_SUITE_REGISTRATION(PILOT_System);

/**
*Implementation notes: none 
*<p>
*Portability issues: none 
*<p>
*/

PILOT_System::PILOT_System()
{

}


PILOT_System::~PILOT_System()
{

}


void
PILOT_System::do_tests()                
{
    // set number of requested successfull tests
//     set_nbOfRequestedTestOK (8);
    
    TEST_INFO("Creating the ORB");
    OrbThread orbThread;
    orbThread.start();
    
	 int timescale = Cdmw::TestUtils::get_timescale();
    OsSupport::OS::sleep(timescale*4000);
    CORBA::ORB_var  orb = orbThread.orb();
    PortableServer::POA_var poa = orbThread.poa();

    OsSupport::OS::ProcessId supervisionId = 0;
    bool supervisionStarted = false;
    std::string daemonArgs(" --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml "); 
    
    try
    {  
        std::string supervisionArgs("--creation-timeout=20000 --event-timeout=6000 --RequestDurationTime=20000000 ");
        supervisionArgs += Cdmw::Common::Options::LOG_FILE_OPTION;    
        supervisionArgs += "=log.txt";
        
        OsSupport::OS::sleep(timescale*1000);

        TEST_INFO("Starting the PlatformDaemon");
        TEST_INFO(daemonArgs);
        OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);   
        OsSupport::OS::sleep(timescale*5000);
    
        TEST_INFO("Starting the System");
        TEST_INFO(supervisionArgs);
        supervisionId = OsSupport::OS::create_process("cdmw_platform_supervision", supervisionArgs);  
        //supervisionId = OsSupport::OS::create_process("ddd", "cdmw_platform_supervision");  
        supervisionStarted = true;  
    
        TEST_INFO("Sleeping a while to wait for the supervision creation");
        OsSupport::OS::sleep(timescale*15000);
        //OsSupport::OS::sleep(10*15000);

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

        std::string adminArgs("--system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-define system.xml");
    
        TEST_INFO("Define the system");
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);    
        OsSupport::OS::sleep(timescale*5000);
        
        TEST_INFO("get the message of the observer");    
        std::string last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        CPPUNIT_ASSERT(last_message == "");

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-snapshot";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*3000);

        TEST_INFO("Start of the system");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-start";
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
        last_message = obs_servant->get_last_ew_message();
        std::string hostname = OsSupport::OS::get_hostname();
        std::string message ("S:'TEST' A:'APPL1' H:'");
        message += hostname;
        message += "' P:'PROC11' FAILED_DEATH";
        
        std::cout<<last_message<<std::endl;    
        CPPUNIT_ASSERT(last_message.find(message.c_str()) != last_message.npos);

        TEST_INFO("Restart the process");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --proc-initialise APPL1 PROC11 " + hostname;
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_message();
        std::cout<<last_message<<std::endl;
        message = "S:'TEST' A:'APPL1' H:'";
        message += hostname;
        message += "' P:'PROC11' INITIALISED";
        CPPUNIT_ASSERT(last_message.find(message.c_str()) != last_message.npos);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --proc-run APPL1 PROC11 " + hostname;
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);
        
        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_message();
        std::cout<<last_message<<std::endl;    
        message = "S:'TEST' A:'APPL1' H:'";
        message += hostname;
        message += "' P:'PROC11' RUNNING";
        CPPUNIT_ASSERT(last_message.find(message.c_str()) != last_message.npos);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --proc-stop APPL1 PROC11 " + hostname;
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        TEST_INFO("Remove the process PROC21");

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --proc-stop APPL2 PROC21 " + hostname;
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);
        
        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_message();
        std::cout<<last_message<<std::endl;    
        message = "S:'TEST' A:'APPL2' H:'";
        message += hostname;
        message += "' P:'PROC21' STOPPED";
        CPPUNIT_ASSERT(last_message.find(message) != last_message.npos);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --app-remove-process APPL2 PROC21 " + hostname;
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*20000);

        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        CPPUNIT_ASSERT(last_message == "");

        TEST_INFO("Remove the application APPL2");

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --app-stop APPL2";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);

        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_message();
        std::cout<<last_message<<std::endl;    
        CPPUNIT_ASSERT(last_message.find("S:'TEST' A:'APPL2' STOPPED") != last_message.npos);

        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-remove-application APPL2";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*20000);

        TEST_INFO("get the message of the observer");    
        last_message = obs_servant->get_last_ew_message();
        std::cout<<last_message<<std::endl;    
        CPPUNIT_ASSERT(last_message == "");


        TEST_INFO("Stop the system");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21871/CdmwPlatformMngtSupervision --sys-stop";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);
    } 
    catch (...) 
    {
        std::cerr << "unexpected exception raised" << std::endl;
        CPPUNIT_ASSERT(false);
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


