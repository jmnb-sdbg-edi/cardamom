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


#include <string>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#include "testftlocationmanager2/TestHello_impl.hpp"
#include "testftlocationmanager2/TestFTActivationHandler_impl.hpp"
#include "testftlocationmanager2/TestClientThread.hpp"
#include "testftlocationmanager2/TestLocationManager.hpp"
#include <testftlocationmanager2/TestStatefullPrimaryBackupAdmin.hpp>
#include <fstream>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

using namespace Cdmw;
using Cdmw::OsSupport::OS;
using namespace std;

class ORBThread : public OsSupport::Thread {

public:
        
    ORBThread(int argc, char* argv[], CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA,const std::string&  application_name )
    {
        m_has_shutdown = false;
        m_orb = CORBA::ORB::_duplicate(orb);
        m_rootPOA =  PortableServer::POA::_duplicate(rootPOA);
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

    }
        
    ~ORBThread() throw() {} ;


    void
    shutdown() 
    {
        if (!m_has_shutdown) {
            m_has_shutdown = true;
            m_orb->shutdown(false);
            m_orb->destroy();
        }
    }

protected:

    void
    run() throw() 
    {
        m_orb->run();   
    }

        
public:

    CORBA::ORB_var  m_orb;
    PortableServer::POA_var m_rootPOA;
    bool m_has_shutdown;

};


ORBThread* orbThread = NULL;

void exit_handler(int sig) {
    if (orbThread != NULL) {
        try {
            orbThread->shutdown();
        } catch (const CORBA::SystemException& e) {
            std::cerr << "Error while shuting ORB down in exit_handler:\n"
                      << e << " - minor code: " << e.minor() << std::endl;
        }
    }
}


int main( int argc, char* argv[] )
{

    int res = 0;

    // Are we started as the CORBA server ?
    if ( OS::get_option_value( argc, argv, "--client") == "yes" ) 
    {
        try
        {
            
            std::string application_name;
        
            if ( OS::get_option_value( argc, argv, "APPL1") == "yes" ) 
            {
                application_name = "APPL1";
            } else {
                application_name = "APPL2";
            }

            Cdmw::OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
            strategyList.add_multicast();
            // Initialises the ORB
            CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList, application_name.c_str());
        
            // Get the root POA
            CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
            PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
            std::cout<<"Get the fault manager reference"<<std::endl;
            CORBA::Object_var fm_obj
            = orb->string_to_object("corbaloc::localhost:4555/fault_manager");
            
            if ( CORBA::is_nil( fm_obj.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid corbaloc" );
            
            ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );
            
            CdmwFT::ReplicationManager_var rm =  fault_manager->get_replication_manager();
            
            if ( CORBA::is_nil( rm.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid replication manager reference" );
            
            orb->register_initial_reference("ReplicationManager", rm.in());
            // Start the ORBThread 
            orbThread = new ORBThread(argc, argv, orb.in(), rootPOA.in(), application_name);

            struct sigaction action;
            action.sa_handler=exit_handler;
            sigemptyset(&action.sa_mask);
            sigaction(SIGTERM, &action, NULL);
            sigaction(SIGINT, &action, NULL);
            sigaction(SIGQUIT, &action, NULL);
            sigaction(SIGABRT, &action, NULL);

            orbThread->start();         

            TestClientThread testClientThread(orb.in(), rootPOA.in(), application_name);
            testClientThread.start( argc, argv);
            std::cout<<"**********"<<std::endl;
            sleep(100);
            std::cout<<"----------"<<std::endl;
            // Stop the thread
            orbThread->shutdown();
            orbThread->join();

        }
        catch( const CORBA::Exception &e )
        {
            cerr << e._name() << endl;
            res = -1;
        }
        catch( const Cdmw::Exception &e )
        {
            cerr << e.what() << endl;
            res = -1;
        }
        catch(...)
        {
            cerr << "Uncaught exception" << endl;
            res = -1;
        }
	    
    }
    // Else, we are the main test driver
    else if  ( OS::get_option_value( argc, argv, "--init") == "yes" ) 
    {

        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
        strategyList.add_multicast();
        // Initialises the ORB
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        
        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
        std::cout<<"Get the fault manager reference"<<std::endl;
        CORBA::Object_var fm_obj
        = orb->string_to_object("corbaloc::localhost:4555/fault_manager");
        
        if ( CORBA::is_nil( fm_obj.in() ) )
            CDMW_THROW2(Exception,
                        Exception::FATAL,
                        "Invalid corbaloc" );
        
        ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );
        
        CdmwFT::ReplicationManager_var rm =  fault_manager->get_replication_manager();
        
        if ( CORBA::is_nil( rm.in() ) )
            CDMW_THROW2(Exception,
                        Exception::FATAL,
                        "Invalid replication manager reference" );
        
        orb->register_initial_reference("ReplicationManager", rm.in());
        
        // Start the ORBThread 
        orbThread = new ORBThread(argc, argv, orb.in(), rootPOA.in(), "");

        struct sigaction action;
        action.sa_handler=exit_handler;
        sigemptyset(&action.sa_mask);
        sigaction(SIGTERM, &action, NULL);
        sigaction(SIGINT, &action, NULL);
        sigaction(SIGQUIT, &action, NULL);
        sigaction(SIGABRT, &action, NULL);

        orbThread->start();         
        
        ::FT::Location loc;
        loc.length(3);
        loc[0].kind = "hostname";
        loc[1].kind = "applicationname";
        loc[2].kind = "processname";
        loc[0].id = "HOST1";
        loc[1].id = "APPL1";
        loc[2].id = "PROC1";

        std::cout<<" : Set_Location_Name"<<std::endl;
        Cdmw::FT::DataStoreBase::Set_Location_Name(Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc));


        Cdmw::HelloInterface3_impl * helloInterface3_impl = new Cdmw::HelloInterface3_impl(orb.in(), rootPOA.in());
        CdmwReplicationManager::HelloInterface3_var helloInterface3 =  helloInterface3_impl->_this();
        
        // export the object reference to a file
        CORBA::String_var ref_string = orb->object_to_string(helloInterface3.in());
        
        std::ofstream os("init.ior");
        os << ref_string.in();
        os.close();

        std::cout<<"**********"<<std::endl;
        sleep(100);
        std::cout<<"----------"<<std::endl;
        // Stop the thread
        orbThread->shutdown();
        orbThread->join();
    }
    else
    {
        try
        {
            Cdmw::OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
            strategyList.add_multicast();
            // Initialises the ORB
            CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList, "main_orb");
        
            // Get the root POA
            CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
            PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());


            std::cout<<"Start the fault manager"<<std::endl;
             std::string managerArgs(" --CdmwXMLFile=CdmwFaultToleranceManager_loose_conf.xml");
             OsSupport::OS::ProcessId ft_manager_id = OsSupport::OS::create_process("cdmw_ft_manager", managerArgs);
            
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            OsSupport::OS::sleep(timescale*15000);
            
            
            std::cout<<"Get the fault manager reference"<<std::endl;
            CORBA::Object_var fm_obj
            = orb->string_to_object("corbaloc::localhost:4555/fault_manager");
            
            if ( CORBA::is_nil( fm_obj.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid corbaloc" );
            
            ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );
            
            CdmwFT::ReplicationManager_var rm =  fault_manager->get_replication_manager();
            
            if ( CORBA::is_nil( rm.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid replication manager reference" );
            
            orb->register_initial_reference("ReplicationManager", rm.in());

            // Start the ORBThread 
            orbThread = new ORBThread(argc, argv, orb.in(), rootPOA.in(), "main");

            struct sigaction action;
            action.sa_handler=exit_handler;
            sigemptyset(&action.sa_mask);
            sigaction(SIGTERM, &action, NULL);
            sigaction(SIGINT, &action, NULL);
            sigaction(SIGQUIT, &action, NULL);
            sigaction(SIGABRT, &action, NULL);

            orbThread->start();

            // Do test
            OsSupport::OS::sleep(timescale*3000);

            // Do test
            Cdmw::FT::TestLocationManager test1(orb.in(), rootPOA.in(), "TestLocationManager2", argv[0]);
            test1.start();            
           
            Cdmw::FT::TestStatefullPrimaryBackupAdmin test2(orb.in(), rootPOA.in(), "TestStateFullAdmin", argv[0]);
            test2.start();

            // Stop the ORBThread 
            orbThread->shutdown();
            orbThread->join();
            res = 1;
            std::cout<<"kill the replication manager"<<std::endl; 
            OsSupport::OS::kill_process(ft_manager_id);
            OsSupport::OS::sleep(timescale*5000);
        
        }
        catch( const CORBA::Exception &e )
        {
            cerr << e._name() << endl;
            res = -1;
        }
        catch( const Cdmw::Exception &e )
        {
            cerr << e.what() << endl;
            res = -1;
        }
        catch(...)
        {
            cerr << "Uncaught exception" << endl;
            res = -1;
        }
    }
    
    if (orbThread != NULL)
        delete orbThread;
}


