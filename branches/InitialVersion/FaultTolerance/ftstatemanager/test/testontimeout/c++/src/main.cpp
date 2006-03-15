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
#include "testontimeout/TestHello_impl.hpp"
#include "testontimeout/TestServerThread.hpp"
#include "testontimeout/TestOnTimeout.hpp"
#include "testontimeout/TestInterceptors.hpp"


#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
#include <tao/ORBInitializer_Registry.h>
#endif


using namespace Cdmw;
using namespace std;
using Cdmw::OsSupport::OS;

class ORBThread : public OsSupport::Thread {

public:
        
    ORBThread(int argc, char* argv[], CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA,const std::string&  application_name )
    {
        
        m_orb = CORBA::ORB::_duplicate(orb);
        m_rootPOA =  PortableServer::POA::_duplicate(rootPOA);
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

    }
        
    ~ORBThread() throw() {} ;


    void
    shutdown() 
    {
        m_orb->shutdown(false);
	    m_orb->destroy();
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

};

// ---------------------------------------------------------------------------
//
class TestORBInitializer : public PortableInterceptor::ORBInitializer,
                           public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    // constructor
    TestORBInitializer( int expected_event_count, const char* method
                        , const char* where, const char* process, bool is_server_interceptor = false)
        : m_expected_event_count(expected_event_count)
        , m_method(method)
        , m_where(where)
        , m_process(process)
        , m_is_server_interceptor(is_server_interceptor) { }

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
        if (!m_is_server_interceptor)
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
        } else {
            //
            // Create and register the interceptors
            //
            PortableInterceptor::ServerRequestInterceptor_var server_interceptor
            = new Cdmw::TestServerInterceptor( m_expected_event_count, m_method.in()
                                               , m_where.in(), m_process.in());
            
            try {
                info->add_server_request_interceptor(server_interceptor.in());
            }
            catch(const PortableInterceptor::ORBInitInfo::DuplicateName&) {
                // can't happened while interceptor is not named!
                CDMW_ASSERT(false);
            }
        
        }
    }
    
private:
    
    int m_expected_event_count;
    CORBA::String_var m_method;
    CORBA::String_var m_where;
        CORBA::String_var m_process;
        bool m_is_server_interceptor;
    };

                               
// ---------------------------------------------------------------------------
//

                               
int main( int argc, char* argv[] )
{

    int res = 0;

    // Are we started as the CORBA server ?
    if ( OS::get_option_value( argc, argv, "--server") == "yes" ) 
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

            std::string where = "request";
            std::string event_count_str = "1";
            std::string method = "commit";
            int event_count = atoi(event_count_str.c_str());
            
            // Register the ORB initializer
            PortableInterceptor::ORBInitializer_var initializer
               = new TestORBInitializer(event_count, 
                                        method.c_str(), 
                                        where.c_str(), 
                                        application_name.c_str(),
                                        true);

            PortableInterceptor::register_orb_initializer(initializer.in());

            Cdmw::OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
            strategyList.add_multicast();
            // Initialises the ORB
            CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(
                  argc, argv, strategyList, application_name.c_str());
        
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
            ORBThread orbThread(argc, argv, orb.in(), rootPOA.in(), application_name);
            orbThread.start();         

            TestServerThread testServerThread(orb.in(), rootPOA.in(), application_name);
            testServerThread.start( argc, argv);
            std::cout<<"**********"<<std::endl;
            sleep(100);
            std::cout<<"----------"<<std::endl;
            // Stop the thread
            orbThread.shutdown();
            orbThread.join();

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
    else 
    {
        try
        {
            std::string where = "";
            std::string event_count_str = "0";
            std::string method = "";
            std::string application_name = "APPL3";
            int event_count = atoi(event_count_str.c_str());
            
            // Register the ORB initializer
            PortableInterceptor::ORBInitializer_var initializer
               = new TestORBInitializer(event_count, 
                                        method.c_str(), 
                                        where.c_str(), 
                                        application_name.c_str(), 
                                        false);

            PortableInterceptor::register_orb_initializer(initializer.in());

            Cdmw::OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
            strategyList.add_multicast();
            // Initialises the ORB
            CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, 
                                                                        argv, 
                                                                        strategyList, 
                                                                        "main_orb");
        
            // Get the root POA
            CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
            PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

            std::cout<<"Start the fault manager"<<std::endl;
            std::string managerArgs(
                  " --CdmwXMLFile=CdmwFaultToleranceManager_loose_conf.xml -ORBDebugLevel 10");
            OsSupport::OS::ProcessId ft_manager_id = 
               OsSupport::OS::create_process("cdmw_ft_manager", managerArgs);
            
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
            ORBThread orbThread(argc, argv, orb.in(), rootPOA.in(), "main");
            orbThread.start();

            // Do test
            OsSupport::OS::sleep(timescale*3000);

            // Do test
            Cdmw::FT::TestOnTimeout test1(orb.in(), rootPOA.in(), "TestOnTimeout", argv[0]);
            test1.start();
 
            // Stop the ORBThread 
            orbThread.shutdown();
            orbThread.join();
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
}


