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

#include "testplatformftaccessorlibrary/PILOT_System.hpp"
#include <sstream>
#include <fstream>

#include <list>

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtApplication.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtProcess.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtEntity.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtHost.stub.hpp> 
#include "testplatformftaccessorlibrary/SIM_OrbThread.hpp"
#include <SystemMngt/platformvaluetypes/ProcessDef_impl.hpp> 

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

CPPUNIT_TEST_SUITE_REGISTRATION(  PILOT_System );

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
// //     set_nbOfRequestedTestOK (153);
    
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

        TEST_INFO("get the reference of the replication manager");
        CdmwFT::ReplicationManager_var replication_manager = fault_manager->get_replication_manager();
        

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
 

        std::cout<<orb->object_to_string(system.in())<<std::endl;


        TEST_INFO("Create the observer and register it");  
        OsSupport::OS::sleep(timescale*3000);
        PortableServer::ServantBase_var obs_servantBase = 
           new SupervisionObserver();
        SupervisionObserver* obs_servant = 
           dynamic_cast<SupervisionObserver *> (obs_servantBase.in());
        CdmwPlatformMngt::SupervisionObserver_var observer = 
           obs_servant->_this();
        system_proxy->register_observer ("my_observer", observer.in());
        system_proxy->unregister_observer ("my_observer");

        std::string adminArgs("--system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-define system.xml");
    
        TEST_INFO("Define the system using the System Proxy");
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);    
        OsSupport::OS::sleep(timescale*5000);
        
        TEST_INFO("Snapshot of the system using the System Proxy");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-snapshot";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*10000);


        
        // get from the replication manager the system accessor group reference
        // from its group name
        ::FT::Name group_name;
        group_name.length(1);
        group_name[0].id =
        Cdmw::PlatformMngt::Configuration::Get_SystemAccessor_group_name().c_str();
        
        CORBA::Object_var system_accessor_group_obj = 
            replication_manager->get_object_group_ref_from_name(group_name);
        
        CdmwFTSystMngt::SystemAccessor_var system_accessor = 
            CdmwFTSystMngt::SystemAccessor::_narrow(system_accessor_group_obj.in());
        
        // get the other accessor from system accessor
        CdmwFTSystMngt::ApplicationAccessor_var application_accessor = 
            CdmwFTSystMngt::ApplicationAccessor::_duplicate(system_accessor->get_application_accessor());
        CdmwFTSystMngt::ProcessAccessor_var process_accessor = 
            CdmwFTSystMngt::ProcessAccessor::_duplicate(system_accessor->get_process_accessor());
        CdmwFTSystMngt::HostAccessor_var host_accessor = 
            CdmwFTSystMngt::HostAccessor::_duplicate(system_accessor->get_host_accessor());
        CdmwFTSystMngt::EntityAccessor_var entity_accessor = 
            CdmwFTSystMngt::EntityAccessor::_duplicate(system_accessor->get_entity_accessor());



        TEST_INFO("test service")
        {
            





            CdmwPlatformMngtService::ServiceDef_var services1
            = new CdmwPlatformMngtService::ServiceDef();
            services1->service_name = CORBA::string_dup("service1");
            CdmwPlatformMngtService::ServiceProviderID_var provider1 
            = new CdmwPlatformMngtService::ServiceProviderID();
            provider1->application_name = CORBA::string_dup("APPL1");
            provider1->process_name = CORBA::string_dup("PROC11");
            provider1->host_name = Cdmw::OsSupport::OS::get_hostname().c_str();
            services1->service_provider = provider1.in();

            CdmwPlatformMngtService::ServiceDef_var services2
            = new CdmwPlatformMngtService::ServiceDef();
            services2->service_name = CORBA::string_dup("service2");
            CdmwPlatformMngtService::ServiceProviderID_var provider2
            = new CdmwPlatformMngtService::ServiceProviderID();
            provider2->application_name = CORBA::string_dup("APPL1");
            provider2->process_name = CORBA::string_dup("PROC11");
            provider2->host_name = Cdmw::OsSupport::OS::get_hostname().c_str();
            services2->service_provider = provider2.in();

            CdmwPlatformMngtService::ServiceDef_var services3
            = new CdmwPlatformMngtService::ServiceDef();
            services3->service_name = CORBA::string_dup("service3");
            CdmwPlatformMngtService::ServiceProviderID_var provider3
            = new CdmwPlatformMngtService::ServiceProviderID();
            provider3->application_name = CORBA::string_dup("APPL1");
            provider3->process_name = CORBA::string_dup("PROC11");;
            provider3->host_name = Cdmw::OsSupport::OS::get_hostname().c_str();
            services3->service_provider = provider3.in();


            TEST_INFO("Service system");
            try
            {
                system_accessor->add_service_def("s.syswxxxx", services1.in());
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            
            
            
            try
            {
                system_accessor->add_service_def("s.sys", services1.in());
                system_accessor->add_service_def("s.sys", services2.in());
                system_accessor->add_service_def("s.sys", services3.in());
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                system_accessor->add_service_def("s.sys", services1.in());
                CPPUNIT_ASSERT(false);   
            }
            catch(const CdmwPlatformMngtService::ServiceAlreadyExists&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                system_accessor->remove_service_def ("ssdfvgdf.sys","service1");
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                system_accessor->remove_service_def ("s.sys","service1");
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                system_accessor->remove_service_def ("s.sys","service1");
                CPPUNIT_ASSERT(false);
            }
            catch( const CdmwPlatformMngtService::ServiceNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            


            try
            {
                system_accessor->remove_all_service_defs ("s.sys");
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                system_accessor->remove_service_def ("s.sys","service3");
                CPPUNIT_ASSERT(false);
            }
            catch( const CdmwPlatformMngtService::ServiceNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            



            TEST_INFO("Service application");
            try
            {
                application_accessor->add_service_def("s.sys/APPL1xcvsdvdf.appl", services1.in());
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            
            
            
            try
            {
                application_accessor->add_service_def("s.sys/APPL1.appl", services1.in());
                application_accessor->add_service_def("s.sys/APPL1.appl", services2.in());
                application_accessor->add_service_def("s.sys/APPL1.appl", services3.in());
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                application_accessor->add_service_def("s.sys/APPL1.appl", services1.in());
                CPPUNIT_ASSERT(false);   
            }
            catch(const CdmwPlatformMngtService::ServiceAlreadyExists&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                application_accessor->remove_service_def ("s.sys/APPLbgfbgfb1.appl","service1");
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                application_accessor->remove_service_def ("s.sys/APPL1.appl","service1");
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                application_accessor->remove_service_def ("s.sys/APPL1.appl","service1");
                CPPUNIT_ASSERT(false);
            }
            catch( const CdmwPlatformMngtService::ServiceNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            


            try
            {
                application_accessor->remove_all_service_defs ("s.sys/APPL1.appl");
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                application_accessor->remove_service_def ("s.sys/APPL1.appl","service3");
                CPPUNIT_ASSERT(false);
            }
            catch( const CdmwPlatformMngtService::ServiceNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }


            TEST_INFO("Service process");
            try
            {
                process_accessor->add_service_def("s.sys/APPL1xcvsdvdf.appl", services1.in());
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            
            
            std::stringstream streampath;
            streampath << "s.sys/APPL1.appl/" 
                       << Cdmw::OsSupport::OS::get_hostname().c_str()
                       << ".host/PROC11.proc";
            try
            {
                process_accessor->add_service_def(streampath.str().c_str(), services1.in());
                process_accessor->add_service_def(streampath.str().c_str(), services2.in());
                process_accessor->add_service_def(streampath.str().c_str(), services3.in());
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                process_accessor->add_service_def(streampath.str().c_str(), services1.in());
                CPPUNIT_ASSERT(false);   
            }
            catch(const CdmwPlatformMngtService::ServiceAlreadyExists&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                process_accessor->remove_service_def ("s.sys/APPLbgfbgfb1.appl","service1");
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                process_accessor->remove_service_def (streampath.str().c_str(),"service1");
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                process_accessor->remove_service_def (streampath.str().c_str(),"service1");
                CPPUNIT_ASSERT(false);
            }
            catch( const CdmwPlatformMngtService::ServiceNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            


            try
            {
                process_accessor->remove_all_service_defs (streampath.str().c_str());
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }            

            try
            {
                process_accessor->remove_service_def (streampath.str().c_str(),"service3");
                CPPUNIT_ASSERT(false);
            }
            catch( const CdmwPlatformMngtService::ServiceNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            } 
        }


        TEST_INFO("Test Entities");
        {
            TEST_INFO("Test entity for system");
            {
                
                CdmwPlatformMngt::EntityDef_var entity_system1 = new CdmwPlatformMngt::EntityDef();
                entity_system1->entity_name = "entity_system1";

                CdmwPlatformMngt::EntityDef_var entity_system2 = new CdmwPlatformMngt::EntityDef();
                entity_system2->entity_name = "entity_system2";

                CdmwPlatformMngt::EntityDef_var entity_system3 = new CdmwPlatformMngt::EntityDef();
                entity_system3->entity_name = "entity_system3";

                try
                {
                    system_accessor->add_entity("sdvdfvs.sys", entity_system1.in());
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                

                try
                {
                    system_accessor->add_entity("s.sys", entity_system1.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
            

                try
                {
                    system_accessor->add_entity("s.sys", entity_system2.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    system_accessor->add_entity("s.sys", entity_system3.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    system_accessor->remove_entity("s.sys","entity_system3");
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    system_accessor->remove_entity("s.sys","entity_system3");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::EntityNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    system_accessor->remove_entity("ssdvsdv.sys","entity_system2");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    system_accessor->get_number_of_entities("ssdvsdv.sys");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
 
                try
                {
                    CORBA::ULong nb = system_accessor->get_number_of_entities("s.sys");
                    CPPUNIT_ASSERT(nb == 2);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
               
                try
                {
                    std::string entity = system_accessor->get_entity("ssdvsdv.sys","entity_system2");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    std::string entity = system_accessor->get_entity("s.sys","entity_system3");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::EntityNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
 
                try
                {
                    std::string entity = system_accessor->get_entity("s.sys", "entity_system2");
                    std::cout<<entity<<std::endl;
                    CPPUNIT_ASSERT(!strcmp(entity.c_str(),"s.sys/entity_system2.ent"));
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
               

            }

            TEST_INFO("Test entity for host");
            {
                
                std::stringstream streampath;
                streampath << "s.sys/" 
                           << Cdmw::OsSupport::OS::get_hostname().c_str()
                           << ".host";
                std::stringstream streampath2;
                streampath2 << "s.sys/" 
                            << Cdmw::OsSupport::OS::get_hostname().c_str()
                            << ".host/entity_host2.ent";
 
                CdmwPlatformMngt::EntityDef_var entity_host1 = new CdmwPlatformMngt::EntityDef();
                entity_host1->entity_name = "entity_host1";

                CdmwPlatformMngt::EntityDef_var entity_host2 = new CdmwPlatformMngt::EntityDef();
                entity_host2->entity_name = "entity_host2";

                CdmwPlatformMngt::EntityDef_var entity_host3 = new CdmwPlatformMngt::EntityDef();
                entity_host3->entity_name = "entity_host3";

                try
                {
                    host_accessor->add_entity("sdvdfvs.sys", entity_host1.in());
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                

                try
                {
                    host_accessor->add_entity(streampath.str().c_str(), entity_host1.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
            

                try
                {
                    host_accessor->add_entity(streampath.str().c_str(), entity_host2.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    host_accessor->add_entity(streampath.str().c_str(), entity_host3.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    host_accessor->remove_entity(streampath.str().c_str(),"entity_host3");
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    host_accessor->remove_entity(streampath.str().c_str(),"entity_host3");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::EntityNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    host_accessor->remove_entity("ssdvsdv.sys","entity_host2");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }


                try
                {
                    host_accessor->get_number_of_entities("ssdvsdv.sys");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
 
                try
                {
                    CORBA::ULong nb = host_accessor->get_number_of_entities(streampath.str().c_str());
                    CPPUNIT_ASSERT(nb == 2);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
               
                try
                {
                    std::string entity = host_accessor->get_entity("ssdvsdv.sys","entity_host2");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    std::string entity = host_accessor->get_entity(streampath.str().c_str(),"entity_host3");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::EntityNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
 
                try
                {
                    std::string entity = host_accessor->get_entity(streampath.str().c_str(), "entity_host2");
                    std::cout<<entity<<" "<<streampath2.str().c_str()<<std::endl;
                    CPPUNIT_ASSERT(!strcmp(entity.c_str(),streampath2.str().c_str()));
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
               


            }

            TEST_INFO("Test entity for application");
            {
                
                CdmwPlatformMngt::EntityDef_var entity_application1 = new CdmwPlatformMngt::EntityDef();
                entity_application1->entity_name = "entity_application1";

                CdmwPlatformMngt::EntityDef_var entity_application2 = new CdmwPlatformMngt::EntityDef();
                entity_application2->entity_name = "entity_application2";

                CdmwPlatformMngt::EntityDef_var entity_application3 = new CdmwPlatformMngt::EntityDef();
                entity_application3->entity_name = "entity_application3";

                try
                {
                    application_accessor->add_entity("s.sys/APPLhn1.appl", entity_application1.in());
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                

                try
                {
                    application_accessor->add_entity("s.sys/APPL1.appl", entity_application1.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
            

                try
                {
                    application_accessor->add_entity("s.sys/APPL1.appl", entity_application2.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    application_accessor->add_entity("s.sys/APPL1.appl", entity_application3.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    application_accessor->remove_entity("s.sys/APPL1.appl","entity_application3");
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    application_accessor->remove_entity("s.sys/APPL1.appl","entity_application3");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::EntityNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    application_accessor->remove_entity("s.sys/APPL1jgfngfng.appl","entity_application2");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }



                try
                {
                    application_accessor->get_number_of_entities("ssdvsdv.sys");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
 
                try
                {
                    CORBA::ULong nb = application_accessor->get_number_of_entities("s.sys/APPL1.appl");
                    CPPUNIT_ASSERT(nb == 2);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
               
                try
                {
                    std::string entity = application_accessor->get_entity("ssdvsdv.sys","entity_application2");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    std::string entity = application_accessor->get_entity("s.sys/APPL1.appl","entity_application3");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::EntityNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
 
                try
                {
                    std::string entity = application_accessor->get_entity("s.sys/APPL1.appl", "entity_application2");
                    std::cout<<entity<<std::endl;
                    CPPUNIT_ASSERT(!strcmp(entity.c_str(),"s.sys/APPL1.appl/entity_application2.ent"));
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

            }

            TEST_INFO("Test entity for process");
            {
                
                std::stringstream streampath;
                streampath << "s.sys/APPL1.appl/" 
                           << Cdmw::OsSupport::OS::get_hostname().c_str()
                           << ".host/PROC11.proc";
                std::stringstream streampath2;
                streampath2 << "s.sys/APPL1.appl/" 
                           << Cdmw::OsSupport::OS::get_hostname().c_str()
                           << ".host/PROC11.proc/entity_process2.ent";


                CdmwPlatformMngt::EntityDef_var entity_process1 = new CdmwPlatformMngt::EntityDef();
                entity_process1->entity_name = "entity_process1";

                CdmwPlatformMngt::EntityDef_var entity_process2 = new CdmwPlatformMngt::EntityDef();
                entity_process2->entity_name = "entity_process2";

                CdmwPlatformMngt::EntityDef_var entity_process3 = new CdmwPlatformMngt::EntityDef();
                entity_process3->entity_name = "entity_process3";

                try
                {
                    process_accessor->add_entity("dhndgng", entity_process1.in());
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                

                try
                {
                    process_accessor->add_entity(streampath.str().c_str(), entity_process1.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
            

                try
                {
                    process_accessor->add_entity(streampath.str().c_str(), entity_process2.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    process_accessor->add_entity(streampath.str().c_str(), entity_process3.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    process_accessor->remove_entity(streampath.str().c_str(),"entity_process3");
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    process_accessor->remove_entity(streampath.str().c_str(),"entity_process3");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::EntityNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    process_accessor->remove_entity("ssdvsdv.sys","entity_process2");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }


                try
                {
                    process_accessor->get_number_of_entities("ssdvsdv.sys");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
 
                try
                {
                    CORBA::ULong nb = process_accessor->get_number_of_entities(streampath.str().c_str());
                    CPPUNIT_ASSERT(nb == 2);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
               
                try
                {
                    std::string entity = process_accessor->get_entity("ssdvsdv.sys","entity_process2");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                    
                try
                {
                    std::string entity = process_accessor->get_entity(streampath.str().c_str(),"entity_process3");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::EntityNotFound&)
                {
                    CPPUNIT_ASSERT(true);                    
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
 
                try
                {
                    std::string entity = process_accessor->get_entity(streampath.str().c_str(), "entity_process2");
                    std::cout<<entity<<" "<<streampath2.str().c_str()<<std::endl;
                    CPPUNIT_ASSERT(!strcmp(entity.c_str(),streampath2.str().c_str()));
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

            }
        }
        



        TEST_INFO("Test the system");

        TEST_INFO("get_system_element_path");
        std::string path = system_accessor->get_system_element_path();
        std::cout<<path<<std::endl;
        CPPUNIT_ASSERT(!strcmp(path.c_str(), "s.sys"));

        TEST_INFO("get_status");
        char* status;
        CORBA::String_out status_out(status);
        TEST_INFO(system_accessor->get_status(status_out));
        TEST_INFO(status);
        
        TEST_INFO("get_mode");
        TEST_INFO(system_accessor->get_mode(status_out));
        TEST_INFO(status);
       
        TEST_INFO("get_host_container");
        CdmwPlatformMngt::HostContainer_var hostcontainer;
        
 

        
        

        TEST_INFO("get_applications");
        {
            CdmwPlatformMngt::ApplicationsIterator_var _iter;
            CdmwPlatformMngt::Applications_var appl = system->get_applications(1,
                                                                               _iter.out());
            std::cout<<(*appl)[0]->name()<<std::endl;
            CPPUNIT_ASSERT(!strcmp((*appl)[0]->name(), "APPL1"));
            
            CdmwPlatformMngt::Application_var out;
            _iter->next_one(out);
            std::cout<<out->name()<<std::endl;
            CPPUNIT_ASSERT(!strcmp(out->name(),"APPL2"));
        }
        {
            
            ElementPathsIterator_var iter;
            ElementPaths_var elementPaths = system_accessor->get_applications(1,
                                                                              iter.out());
            
            path = (*elementPaths)[0];
            CPPUNIT_ASSERT(!strcmp(path.c_str(), "s.sys/APPL1.appl"));
            
            CdmwPlatformMngt::ElementPath_var path_out;
            iter->reset();
            iter->next_one(path_out.out());
            CPPUNIT_ASSERT(!strcmp(path_out.in(), "s.sys/APPL2.appl"));
        }
        
        TEST_INFO("run set method");
        {
            
            CdmwPlatformMngt::RunSetDef runsetdef;
            runsetdef.set_name = "set1";
            runsetdef.run_set.length(1);
            runsetdef.run_set[0] = "APPL1";

            try
            {
                system_accessor->set_run_set(runsetdef);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }

            runsetdef.set_name = "set2";
            try
            {
                system_accessor->set_run_set(runsetdef);
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::DuplicateRunSetElement&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            runsetdef.set_name = "set2";
            runsetdef.run_set[0] = "APPL2";
            try
            {
                system_accessor->set_run_set(runsetdef);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            try
            {
                system_accessor->remove_run_set("set1");
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }

            try
            {
                system_accessor->remove_run_set("set1");
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::RunSetNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }


            try
            {
                system_accessor->remove_all_run_sets();
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
        }
        TEST_INFO("host method");
        {
            
            CdmwPlatformMngt::HostDef hostdef;
            
            hostdef.host_name = "zzzhost2";
            hostdef.daemon_monitoring_info.monitoring_interval=2000;
            hostdef.daemon_monitoring_info.monitoring_timeout=1000;
            try
            {
                system_accessor->add_host(hostdef);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            try
            {
                system_accessor->add_host(hostdef);
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::HostAlreadyExists&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }



            {
                //CPPUNIT_ASSERT(system_accessor->get_number_of_hosts(path.c_str()) == 2);
                
                
            
                CdmwPlatformMngt::ElementNames_var elementnames;
                CdmwPlatformMngt::ElementNamesIterator_var elementnamesiterator;
                try
                {
                    system_accessor->get_all_host_names("UNKNOWN",
                                                        1,
                                                        elementnames.out(),
                                                        elementnamesiterator.out());
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }            
                
                try
                {
                    system_accessor->get_all_host_names(system_accessor->get_system_element_path(),
                                                        1,
                                                        elementnames.out(),
                                                        elementnamesiterator.out());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
            
                std::cout<<(*elementnames)[0].in()<<std::endl;
                CdmwPlatformMngt::ElementName_var element;
                elementnamesiterator->next_one(element.out());
                std::cout<<element.in()<<std::endl;


                std::string hostpath;
            
                try
                {
                    hostpath = system_accessor->get_host("UNKNOWN",
                                                         "UNKNOMW");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                try
                {
                    hostpath = system_accessor->get_host(system_accessor->get_system_element_path(),
                                                         "UNKNOMW");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::HostNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
            
                try
                {
                    hostpath = system_accessor->get_host(system_accessor->get_system_element_path(),
                                                         Cdmw::OsSupport::OS::get_hostname().c_str());
                    std::cout<<hostpath<<std::endl;
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

            
               try
                {
                    CdmwPlatformMngt::ElementPaths_var paths;
                    CdmwPlatformMngt::ElementNames names;
                    names.length(1);
                    names[0] = Cdmw::OsSupport::OS::get_hostname().c_str();
                
                    system_accessor->get_hosts("UNKNOWN",
                                               names,
                                               paths.out());
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }


                try
                {
                    CdmwPlatformMngt::ElementPaths_var paths;
                    CdmwPlatformMngt::ElementNames names;
                    names.length(1);
                    names[0] = "host3";
 
               
                    bool res = system_accessor->get_hosts(system_accessor->get_system_element_path(),
                                                          names,
                                                          paths.out());
                    CPPUNIT_ASSERT(!res);

                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }


                try
                {
                    CdmwPlatformMngt::ElementPaths_var paths;
                    CdmwPlatformMngt::ElementNames names;
                    names.length(1);
                    names[0] = Cdmw::OsSupport::OS::get_hostname().c_str();
 
               
                    bool res = system_accessor->get_hosts(system_accessor->get_system_element_path(),
                                                          names,
                                                          paths.out());
 
                    CPPUNIT_ASSERT(res);
                    std::cout<<(*paths)[0].in()<<std::endl;
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }


                try
                {
                
                    CdmwPlatformMngt::ElementPaths_var hosts;
                    CdmwPlatformMngt::ElementPathsIterator_var rest;
                    system_accessor->get_all_hosts("UNKNOWN",
                                                   1,
                                                   hosts.out(),
                                                   rest.out());
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(const CORBA::SystemException& e)
                {
                    std::cout<<e<<std::endl;
                    CPPUNIT_ASSERT(false);
                }
                catch(const std::exception& e)
                {
                    std::cout<<e.what()<<std::endl;
                    CPPUNIT_ASSERT(false);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                
                std::string pathhost1, pathhost2;
                

                try
                {
                
                    CdmwPlatformMngt::ElementPaths_var hosts;
                    CdmwPlatformMngt::ElementPathsIterator_var rest;
                    system_accessor->get_all_hosts(system_accessor->get_system_element_path(),
                                                   1,
                                                   hosts.out(),
                                                   rest.out());
                    std::cout<<(*hosts)[0].in()<<std::endl;
                    CdmwPlatformMngt::ElementPath_var element;
                    rest->next_one(element.out());
                    std::cout<<element.in()<<std::endl;
                    pathhost1 = (*hosts)[0].in();
                    pathhost2 = element.in();
                    

                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                CdmwPlatformMngt::HostStatus hoststatus;
                




                try
                {
                    hoststatus = host_accessor->get_status("UNKNOWN");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    hoststatus = host_accessor->get_status(pathhost1.c_str());

                    CPPUNIT_ASSERT(hoststatus == CdmwPlatformMngt::HOST_RESPONDING);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    hoststatus = host_accessor->get_status("UNKNOWN");
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                

                try
                {
                    hoststatus = host_accessor->get_status(pathhost1.c_str());
                    CPPUNIT_ASSERT(hoststatus == CdmwPlatformMngt::HOST_RESPONDING);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    std::string platfromaaplicationpath = host_accessor->get_platform_application_exe(pathhost1.c_str());
                    TEST_INFO(platfromaaplicationpath);
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
            }
            

            try
            {
                system_accessor->remove_host("zzzhost2");
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            try
            {
                system_accessor->remove_host("zzzhost2");
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::HostNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
        }

            
        TEST_INFO("Application method");
        {
            TEST_INFO("get_status");
            try
            {
                char* status;
                CORBA::String_out status_out(status);
                TEST_INFO(application_accessor->get_status("UNKNOWN",status_out));
                TEST_INFO(status);
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            try
            {
                char* status;
                CORBA::String_out status_out(status);
                TEST_INFO(application_accessor->get_status("s.sys/APPL1.appl",status_out));
                TEST_INFO(status);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            TEST_INFO("get_mode");
            try
            {
                char* status;
                CORBA::String_out status_out(status);
                TEST_INFO(application_accessor->get_mode("s.sys/APPL1.appl",status_out));
                TEST_INFO(status);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            TEST_INFO("get_snapshot");
            try
            {
                application_accessor->get_snapshot("s.sys/APPL1.appl");
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            
            
            
            
            
        }
        
            


        TEST_INFO("start system and put it into a SYSTEM_STARTUP_SUSPENDED status");
        {
            CdmwPlatformMngt::RunSetDef runsetdef;
            runsetdef.set_name = "set1";
            runsetdef.run_set.length(1);
            runsetdef.run_set[0] = "APPL1";

            try
            {
                system_accessor->set_run_set(runsetdef);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }



            CdmwPlatformMngtBase::StartupKind startupkind;
            startupkind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
            startupkind.startup_mode = 0;
            startupkind.specified_state_id = "";

            system_accessor->start(startupkind);
            
            OsSupport::OS::sleep(timescale*5000);
            
            TEST_INFO("Snapshot of the system using the System Proxy");
            adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-snapshot";
            OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
            OsSupport::OS::sleep(timescale*5000);


            CPPUNIT_ASSERT(system_accessor->get_status(status_out) == CdmwPlatformMngt::SYSTEM_STARTUP_SUSPENDED);

            runsetdef.set_name = "set2";
            runsetdef.run_set[0] = "APPL2";
            try
            {
                system_accessor->set_run_set(runsetdef);
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::IncompatibleStatus&)
            {
                
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            
            try
            {
                system_accessor->remove_run_set("set1");
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::IncompatibleStatus&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            
            try
            {
                system_accessor->remove_all_run_sets();
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::IncompatibleStatus&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            CdmwPlatformMngt::HostDef hostdef;
            
            hostdef.host_name = "zzzhost2";
            hostdef.daemon_monitoring_info.monitoring_interval=2000;
            hostdef.daemon_monitoring_info.monitoring_timeout=1000;
            
            try
            {
                system_accessor->add_host(hostdef);
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::IncompatibleStatus&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }


            try
            {
                system_accessor->remove_host(Cdmw::OsSupport::OS::get_hostname().c_str());
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::IncompatibleStatus&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }

            TEST_INFO("stop the application failed");
            try
            {
                application_accessor->stop("s.sys/APPL2.appl", false);
                OsSupport::OS::sleep(timescale*5000);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            TEST_INFO("remove the bad process");
            try
            {
                application_accessor->remove_process("s.sys/APPL2.appl",
                                                     "PROC21",
                                                     Cdmw::OsSupport::OS::get_hostname().c_str());
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
           
            TEST_INFO("add a new process");
            try
            {


                PlatformMngt::ManagedProcessDefFactory managedProcessDefFactory;
                
                CdmwPlatformMngt::ProcessDef_var processdef
                = managedProcessDefFactory.create("PROC32", CdmwPlatformMngt::MANAGED_PROCESS);                             
                processdef->name("PROC32");
                processdef->host_name(Cdmw::OsSupport::OS::get_hostname().c_str());
                processdef->type(CdmwPlatformMngt::MANAGED_PROCESS);
                processdef->manual_starting(false);
                processdef->auto_ending(false);
                


                CdmwPlatformMngt::ProcessInfo_var processInfo
                = new CdmwPlatformMngt::ProcessInfo();
                
                processInfo->process_exe = CORBA::string_dup("bad_process");
                processInfo->process_port = 21880;
                processInfo->working_directory = CORBA::string_dup("");
                processInfo->process_args = CORBA::string_dup("--nb-steps=1");

                processdef->process_info(processInfo.in());
                CdmwPlatformMngt::ManagedProcessDef* managedProcessDef
                = CdmwPlatformMngt::ManagedProcessDef::_downcast(processdef.in());
                
                CdmwPlatformMngt::LifeCycleDef_var lifeCycleDef
                = new CdmwPlatformMngt::LifeCycleDef();
                
                // Get an init step
                lifeCycleDef->init_steps.length( 1 );
                CdmwPlatformMngt::Step_var initStep1
                = new CdmwPlatformMngt::Step();
                initStep1->step_label = "step1";
                initStep1->step_timeout = 2000;
                
                lifeCycleDef->init_steps[ 0 ] = initStep1.in();

                lifeCycleDef->run_timeout = 2000;
                lifeCycleDef->stop_timeout = 2000;
                            
                managedProcessDef->life_cycle( lifeCycleDef.in() );



                // Set the monitoring info
                CdmwPlatformMngt::CompleteMonitoringInfo_var monitoringInfo
                = new CdmwPlatformMngt::CompleteMonitoringInfo();
                
                monitoringInfo->monitoring_model = CdmwPlatformMngt::PULL_MONITORING_MODEL;
                monitoringInfo->pull_monitoring_interval = 3000;
                monitoringInfo->pull_monitoring_timeout = 1000;
                monitoringInfo->push_monitoring_interval = 0;
                monitoringInfo->push_monitoring_timeout = 0;


                managedProcessDef->monitoring_info( monitoringInfo.in() ); 

                application_accessor->add_process("s.sys/APPL3.appl",
                                                  processdef.in());
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }


            OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
            OsSupport::OS::sleep(timescale*5000);

            TEST_INFO("Initialise the application APPL3");
            try
            {
                CdmwPlatformMngtBase::StartupKind startupkind;
                startupkind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
                startupkind.startup_mode = 0;
                startupkind.specified_state_id = "";
                application_accessor->initialise_and_wait("s.sys/APPL3xxxxx.appl",
                                                          startupkind);
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }

            try
            {
                CdmwPlatformMngtBase::StartupKind startupkind;
                startupkind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
                startupkind.startup_mode = 0;
                startupkind.specified_state_id = "";
                application_accessor->initialise_and_wait("s.sys/APPL3.appl",
                                                          startupkind);
                OsSupport::OS::sleep(timescale*5000);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            TEST_INFO("Run the application APPL3");
            try
            {
                application_accessor->run("s.sys/APPL3xxxx.appl");
                CPPUNIT_ASSERT(false);
            }
            catch (const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }

            try
            {
                application_accessor->run("s.sys/APPL3.appl");
                OsSupport::OS::sleep(timescale*5000);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            try
            {
                application_accessor->run("s.sys/APPL3.appl");
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::IncompatibleStatus&)
            {                
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            

            TEST_INFO("get_processes on the application accessor");
            try
            {
                
            

                ElementPathsIterator_var iter;
                ElementPaths_var elementPaths = application_accessor->get_processes("s.sys/APPL3xxx.appl",
                                                                                    1,
                                                                                    iter.out());
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            

            try
            {
                
            

                ElementPathsIterator_var iter;
                ElementPaths_var elementPaths = application_accessor->get_processes("s.sys/APPL3.appl",
                                                                                    1,
                                                                                    iter.out());
                std::string path;
                path = (*elementPaths)[0];
                std::cout<<path<<std::endl;

                std::stringstream streampath1, streampath2;
                streampath1 << "s.sys/APPL3.appl/" 
                            << Cdmw::OsSupport::OS::get_hostname().c_str()
                            << ".host/PROC31.proc";
                streampath2 << "s.sys/APPL3.appl/" 
                            << Cdmw::OsSupport::OS::get_hostname().c_str()
                            << ".host/PROC32.proc";



                CPPUNIT_ASSERT(!strcmp(path.c_str(), streampath1.str().c_str()));
                
                CdmwPlatformMngt::ElementPath_var path_out;
                iter->reset();
                iter->next_one(path_out.out());
                std::cout<<path_out.in()<<std::endl;
                CPPUNIT_ASSERT(!strcmp(path_out.in(), streampath2.str().c_str()));
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            
            
            TEST_INFO("Try to resume the application");
            
            try
            {
                application_accessor->resume_and_wait("s.sys/APPL3xxxx.appl");
                CPPUNIT_ASSERT(false);
            }
            catch(const CdmwPlatformMngt::ManagedElementNotFound&)
            {
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }


            try
            {
                application_accessor->resume_and_wait("s.sys/APPL3.appl");
                OsSupport::OS::sleep(timescale*2000);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }


            try
            {
                application_accessor->acknowledge("s.sys/APPL3.appl");
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }




            try
            {
                system_accessor->resume();
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            OsSupport::OS::sleep(timescale*1000);

            try
            {
                system_accessor->acknowledge();
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }
            OsSupport::OS::sleep(timescale*1000);

            OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
            OsSupport::OS::sleep(timescale*5000);

            TEST_INFO("process method");
            {
                
                std::stringstream streampath;
                streampath << "s.sys/APPL3.appl/" 
                           << Cdmw::OsSupport::OS::get_hostname().c_str()
                           << ".host/PROC32.proc";
                try
                {
                    process_accessor->get_type("UNKNOWN");
                    CPPUNIT_ASSERT(false);
                    
                }
                catch(const CdmwPlatformMngt::ManagedElementNotFound&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
            
                try
                {
                    CdmwPlatformMngt::ProcessType type = process_accessor->get_type(streampath.str().c_str());
                    CPPUNIT_ASSERT(CdmwPlatformMngt::MANAGED_PROCESS == type);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }


                try
                {

                    TEST_INFO("get_status");
                    char* status;
                    CORBA::String_out status_out(status);
                    CdmwPlatformMngt::ProcessStatus status_ = process_accessor->get_status(streampath.str().c_str(), status_out);
                    CPPUNIT_ASSERT(CdmwPlatformMngt::PROCESS_INITIAL == status_);
                    TEST_INFO(status);
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    process_accessor->set_auto_ending(streampath.str().c_str(), true);
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    bool autoending = process_accessor->is_auto_ending(streampath.str().c_str());
                    CPPUNIT_ASSERT(autoending == true);
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }



//                 TEST_INFO("change the monitoring");
//                 {
//                     try
//                     {
//                         CdmwPlatformMngt::CompleteMonitoringInfo_var monitoring;
//                         monitoring = process_accessor->get_monitoring_info(streampath.str().c_str());
//                         CPPUNIT_ASSERT(false);
//                     }
//                     catch(const CdmwPlatformMngt::IncompatibleType&)
//                     {
//                         CPPUNIT_ASSERT(true);
//                     }
//                     catch(...)
//                     {
//                         CPPUNIT_ASSERT(false);
//                     }
                    
                    
                    
//                     try
//                     {
//                         CdmwPlatformMngt::CompleteMonitoringInfo_var monitoring;
//                         monitoring->monitoring_model = CdmwPlatformMngt::PULL_MONITORING_MODEL;
//                         monitoring->pull_monitoring_interval = 3000;
//                         monitoring->pull_monitoring_timeout = 1000;
//                         monitoring->push_monitoring_interval = 0;
//                         monitoring->push_monitoring_timeout = 0;
                        
//                         process_accessor->set_monitoring_info(streampath.str().c_str(), monitoring.in());
//                         CPPUNIT_ASSERT(false);
//                     }
//                     catch(const CdmwPlatformMngt::IncompatibleType&)
//                     {
//                         CPPUNIT_ASSERT(true);
//                     }
//                     catch(...)
//                     {
//                         CPPUNIT_ASSERT(false);
//                     }        
//                 }
            



                try
                {
                    process_accessor->set_auto_ending(streampath.str().c_str(), false);
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    bool autoending = process_accessor->is_auto_ending(streampath.str().c_str());
                    CPPUNIT_ASSERT(autoending == false);
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }


                try
                {
                    std::string host = process_accessor->get_host(streampath.str().c_str());
                    std::cout<<host<<std::endl;
                    

                    std::stringstream streampath_;
                    streampath_ << "s.sys/" 
                                << Cdmw::OsSupport::OS::get_hostname().c_str()
                                << ".host";

                    CPPUNIT_ASSERT(!strcmp(host.c_str(), streampath_.str().c_str()));
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                



                try
                {
                    CdmwPlatformMngt::ProcessInfo_var processInfo;
                    processInfo =  process_accessor->get_process_info(streampath.str().c_str());
                    CPPUNIT_ASSERT(!strcmp(processInfo->process_exe, "bad_process"));
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                
                try
                {
                    CdmwPlatformMngt::ProcessInfo_var processInfo
                    = new CdmwPlatformMngt::ProcessInfo();
                    
                    processInfo->process_exe = CORBA::string_dup("cdmw_test_managed_process");
                    processInfo->process_port = 21880;
                    processInfo->working_directory = CORBA::string_dup(".");
                    processInfo->process_args = CORBA::string_dup("--nb-steps=2");
                    process_accessor->set_process_info(streampath.str().c_str(), processInfo.in());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                

                try
                {
                    CdmwPlatformMngt::LifeCycleDef_var lifeCycleDef;
                    lifeCycleDef = process_accessor->get_life_cycle(streampath.str().c_str());
                    CPPUNIT_ASSERT(lifeCycleDef->init_steps.length() == 1);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
            
                try
                {
                    CdmwPlatformMngt::LifeCycleDef_var lifeCycleDef
                    = new CdmwPlatformMngt::LifeCycleDef();
                    
                    // Get an init step
                    lifeCycleDef->init_steps.length( 2 );
                    CdmwPlatformMngt::Step_var initStep1
                    = new CdmwPlatformMngt::Step();
                    initStep1->step_label = "step1";
                    initStep1->step_timeout = 2000;
                    
                    lifeCycleDef->init_steps[ 0 ] = initStep1.in();
                    
                    CdmwPlatformMngt::Step_var initStep2
                    = new CdmwPlatformMngt::Step();
                    initStep2->step_label = "step1";
                    initStep2->step_timeout = 2000;
                    
                    lifeCycleDef->init_steps[ 1 ] = initStep2.in();
                    
                    lifeCycleDef->run_timeout = 2000;
                    lifeCycleDef->stop_timeout = 2000;
                    process_accessor->set_life_cycle(streampath.str().c_str(), lifeCycleDef.in());
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::DuplicateStep&)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
 

                try
                {
                    CdmwPlatformMngt::LifeCycleDef_var lifeCycleDef
                    = new CdmwPlatformMngt::LifeCycleDef();
                    
                    // Get an init step
                    lifeCycleDef->init_steps.length( 2 );
                    CdmwPlatformMngt::Step_var initStep1
                    = new CdmwPlatformMngt::Step();
                    initStep1->step_label = "step1";
                    initStep1->step_timeout = 2000;
                    
                    lifeCycleDef->init_steps[ 0 ] = initStep1.in();
                    
                    CdmwPlatformMngt::Step_var initStep2
                    = new CdmwPlatformMngt::Step();
                    initStep2->step_label = "step2";
                    initStep2->step_timeout = 2000;
                    
                    lifeCycleDef->init_steps[ 1 ] = initStep2.in();
                    
                    lifeCycleDef->run_timeout = 2000;
                    lifeCycleDef->stop_timeout = 2000;
                    process_accessor->set_life_cycle(streampath.str().c_str(), lifeCycleDef.in());
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                

 //                TEST_INFO("monitoring");
                
                    
//                 try
//                 {
//                     CdmwPlatformMngt::CompleteMonitoringInfo_var monitoring;
//                     monitoring->monitoring_model = CdmwPlatformMngt::PULL_MONITORING_MODEL;
//                     monitoring->pull_monitoring_interval = 2000;
//                     monitoring->pull_monitoring_timeout = 1000;
//                     monitoring->push_monitoring_interval = 0;
//                     monitoring->push_monitoring_timeout = 0;
                        
//                     process_accessor->set_monitoring_info(streampath.str().c_str(), monitoring.in());
//                     CPPUNIT_ASSERT(true);
//                 }
//                 catch(...)
//                 {
//                     CPPUNIT_ASSERT(false);
//                 }  
  
//                 try
//                 {
//                     CdmwPlatformMngt::CompleteMonitoringInfo_var monitoring;
//                     monitoring = process_accessor->get_monitoring_info(streampath.str().c_str());
//                     CPPUNIT_ASSERT(true);
//                 }
//                 catch(...)
//                 {
//                     CPPUNIT_ASSERT(false);
//                 }
                    
   
                    
                try
                {
                    CdmwPlatformMngt::ProcessSnapshot_var snapshot = process_accessor->get_snapshot(streampath.str().c_str());
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                
                try
                {
                    CdmwPlatformMngtBase::StartupKind startupkind;
                    startupkind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
                    startupkind.startup_mode = 0;
                    startupkind.specified_state_id = "";
                    process_accessor->initialise(streampath.str().c_str(), startupkind);
                    OsSupport::OS::sleep(timescale*5000);
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }
                

                try
                {
                    process_accessor->next_step(streampath.str().c_str());
                    OsSupport::OS::sleep(timescale*5000);
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    process_accessor->next_step(streampath.str().c_str());
                    OsSupport::OS::sleep(timescale*5000);
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::IncompatibleStatus &)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

                try
                {
                    CdmwPlatformMngtBase::StartupKind startupkind;
                    startupkind.state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
                    startupkind.startup_mode = 0;
                    startupkind.specified_state_id = "";
                    process_accessor->initialise(streampath.str().c_str(), startupkind);
                    OsSupport::OS::sleep(timescale*5000);
                    CPPUNIT_ASSERT(false);
                }
                catch(const CdmwPlatformMngt::IncompatibleStatus &)
                {
                    CPPUNIT_ASSERT(true);
                }
                catch(...)
                {
                    CPPUNIT_ASSERT(false);
                }

            }
                
            try
            {
                system_accessor->stop(false);
                CPPUNIT_ASSERT(true);
            }
            catch(...)
            {
                CPPUNIT_ASSERT(false);
            }

            OsSupport::OS::sleep(timescale*2000);

            
        }
        
        
        TEST_INFO("Stop the system");
        adminArgs = "cdmw_platform_admin --system-corbaloc=corbaloc::localhost:21869/CdmwPlatformMngtSupervision --sys-stop";
        OsSupport::OS::create_process("cdmw_platform_admin", adminArgs);  
        OsSupport::OS::sleep(timescale*5000);

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


