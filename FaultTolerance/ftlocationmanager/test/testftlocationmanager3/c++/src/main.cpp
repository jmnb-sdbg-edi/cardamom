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
#include "testftlocationmanager3/TestHello_impl.hpp"
#include "testftlocationmanager3/TestClientThread.hpp"
#include "testftlocationmanager3/TestFTLocation.hpp"
#include "testftlocationmanager3/TestInterceptors.hpp"

#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
#include "tao/ORBInitializer_Registry.h"
#endif

using namespace std;
using namespace Cdmw;
using Cdmw::OsSupport::OS;

// ---------------------------------------------------------------------------
//
class ORBThread : public OsSupport::Thread {

public:
        
    ORBThread(int argc, char* argv[], CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA,const std::string&  proc_name )
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


// ---------------------------------------------------------------------------
//
class TestORBInitializer : public PortableInterceptor::ORBInitializer,
                           public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    // constructor
    TestORBInitializer( int expected_event_count, const char* method
                      , const char* where, const char* process)
        : m_expected_event_count(expected_event_count)
        , m_method(method)
        , m_where(where)
        , m_process(process) { }

    //destructor
    ~TestORBInitializer() { }

    //
    // IDL to C++ Mapping
    //
    virtual void
    pre_init(PortableInterceptor::ORBInitInfo_ptr info) 
       throw (CORBA::SystemException) { }

    
    //
    // Note: the interceptors are registered in post_init()
    //       if their constructors require
    //       ORBInitInfo::resolve_initial_reference(), which XXX
    //       cannot be called in pre_init().
    //
    virtual void
    post_init(PortableInterceptor::ORBInitInfo_ptr info) 
       throw (CORBA::SystemException)
    {
        //
        // Create and register the interceptors
        //
        PortableInterceptor::ClientRequestInterceptor_var client_interceptor
            = new Cdmw::TestClientInterceptor( m_expected_event_count, m_method.in()
                                             , m_where.in(), m_process.in());

        try {
            info->add_client_request_interceptor(client_interceptor.in());
        }
        catch(const PortableInterceptor::ORBInitInfo::DuplicateName&) {
            // can't happened while interceptor is not named!
            CDMW_ASSERT(false);
        }
    }
    
private:

    int m_expected_event_count;
    CORBA::String_var m_method;
    CORBA::String_var m_where;
    CORBA::String_var m_process;
};



// ---------------------------------------------------------------------------
//

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

    // Are we started as the main test driver or as a server ?
    std::string proc_name = OS::get_option_value( argc, argv, "--process");
    if (proc_name == "no") 
    {
        // No option --process, we are the main test driver 
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
            Cdmw::FT::TestFTLocation test(orb.in(), rootPOA.in(), "TestFTLocation", argv[0]);
            test.start();
            
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
    // Else, we are in a "server" process
    else {
        try
        {
            std::cout << "[**** " << proc_name << "::main] starting with args:" << std::endl;
            for (int i=1; i<argc; ++i) {
                std::cout << "  -> argv[" << i << "] \"" << argv[i] << "\"" << std::endl;
            }
            
            Cdmw::OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
            strategyList.add_multicast();

            bool client_interceptor;
            std::string method = OS::get_option_value( argc, argv, "--client_interceptor");
            if (method == "no") {
                client_interceptor = false;
            }
            else {
                client_interceptor = true;
                std::string where = OS::get_option_value( argc, argv, "--where");
                std::string event_count_str = OS::get_option_value( argc, argv, "--count");
                int event_count = atoi(event_count_str.c_str());
                std::cout << "[**** " << proc_name << "]: "  << "method \"" << method 
                                                        << "\", where \"" << where 
                                                        << "\", count " << event_count 
                                                        << std::endl;
                // Register the ORB initializer
                PortableInterceptor::ORBInitializer_var initializer
                    = new TestORBInitializer(event_count, method.c_str(), where.c_str(), proc_name.c_str());
                //initializer->_add_ref();
                CORBA::ULong ref_count = 0;
                Cdmw::OrbSupport::RefCountLocalObject* ref_counter 
                    = dynamic_cast < Cdmw::OrbSupport::RefCountLocalObject* > (initializer.in());
                //ref_count = ref_counter->_get_ref_count();
                //std::cout << "**** ORBInitializer ref_count " << ref_count << std::endl;
                PortableInterceptor::register_orb_initializer(initializer.in());
                ref_count = ref_counter->_get_ref_count();
                std::cout << "**** ORBInitializer ref_count " << ref_count << std::endl;
            }
            
            int step = 0;
            std::string step_str = OS::get_option_value( argc, argv, "--step");
            if ((step_str != "no") && (step_str != "yes")) {
                step = atoi(step_str.c_str());
                std::cout << "[**** " << proc_name << "]: " << " step " << step << std::endl;
            }
            
            // Initialises the ORB
            CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList, proc_name.c_str());
        
            // Get the root POA
            CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
            PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
            std::cout<< "[" << proc_name << "main ]"  << " Get the fault manager reference"<<std::endl;
            CORBA::Object_var fm_obj
            = orb->string_to_object("corbaloc::localhost:4555/fault_manager");
            
            if ( CORBA::is_nil( fm_obj.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid corbaloc" );
            
            ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );
            
            CdmwFT::ReplicationManager_var rm = fault_manager->get_replication_manager();
            
            if ( CORBA::is_nil( rm.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid replication manager reference" );
            
            orb->register_initial_reference("ReplicationManager", rm.in());
            // Start the ORBThread 
            orbThread = new ORBThread(argc, argv, orb.in(), rootPOA.in(), proc_name);

            struct sigaction action;
            action.sa_handler=exit_handler;
            sigemptyset(&action.sa_mask);
            sigaction(SIGTERM, &action, NULL);
            sigaction(SIGINT, &action, NULL);
            sigaction(SIGQUIT, &action, NULL);
            sigaction(SIGABRT, &action, NULL);

            orbThread->start();         

            TestClientThread testClientThread(orb.in(), rootPOA.in(), proc_name, step);
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
    if (orbThread != NULL)
        delete orbThread;

}


