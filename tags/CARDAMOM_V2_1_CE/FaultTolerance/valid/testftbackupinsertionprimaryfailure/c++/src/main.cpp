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
#include <FaultTolerance/ftreplicationmanager/InitUtils.hpp>
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#include "testftbackupinsertionprimaryfailure/TestHello_impl.hpp"
#include "testftbackupinsertionprimaryfailure/TestBackupInsertionPrimaryFailure.hpp"
#include "testftbackupinsertionprimaryfailure/TestInterceptors.hpp"
#include "testftbackupinsertionprimaryfailure/MyProcessBehaviour.hpp"
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>

#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
#include "tao/ORBInitializer_Registry.h"
#endif

using namespace Cdmw;
using Cdmw::OsSupport::OS;
using namespace std;

// ---------------------------------------------------------------------------
//
class ORBThread : public OsSupport::Thread {

public:
        
    ORBThread(int argc, char* argv[], CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA,const std::string&  proc_name )
    {
        
        m_orb = CORBA::ORB::_duplicate(orb);
        m_rootPOA =  PortableServer::POA::_duplicate(rootPOA);
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


        // Are we started as the main test driver or as a server ?
        std::string server = OS::get_option_value( argc, argv, "--server");
        std::string host_name = OS::get_hostname();

        if (server == "no") 
        {


            // get host name
            std::string host_name1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host1");
            // get host name
            std::string host_name2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host2");
            // get host name
            std::string host_name3 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host3");

            std::string faultmanager_ior = OS::get_option_value( argc, argv, "--faultmanager");

            // No option --process, we are the main test driver 
            try
            {
                Cdmw::OrbSupport::StrategyList strategyList;
                strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
                strategyList.add_PoaThreadPerConnection();
                strategyList.add_multicast();
                // Initialises the ORB
                CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList, "main_orb");
        
                // Get the root POA
                CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
                PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
            
                std::cout<<"Get the fault manager reference"<<std::endl;
                CORBA::Object_var fm_obj
                    = orb->string_to_object(faultmanager_ior.c_str());
            
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
                int timescale = Cdmw::TestUtils::Testable::get_timescale();
                OsSupport::OS::sleep(timescale*3000);

                // Do test
                Cdmw::FT::TestBackupInsertionPrimaryFailure test(orb.in(), rootPOA.in(), "Testftbackupinsertionprimaryfailure", argv[0], host_name1, host_name2, host_name3);
                test.start();
            
                // Stop the ORBThread 
                orbThread.shutdown();
                orbThread.join();
            }
            catch( const CORBA::Exception &e )
            {
                cerr << e._name() << endl;
            }
            catch( const Cdmw::Exception &e )
            {
                cerr << e.what() << endl;
            }
            catch(...)
            {
                cerr << "Uncaught exception" << endl;
            }	    
        }
        // Else, we are in a "server" process
        else {
            try
            {
                // Initialise FT service
                Cdmw::FT::FTServiceInit::Init( argc, argv );

                std::cout << "[**** " << host_name << "::main] starting with args:" << std::endl;
                for (int i=1; i<argc; ++i) {
                    std::cout << "  -> argv[" << i << "] \"" << argv[i] << "\"" << std::endl;
                }
            
                Cdmw::OrbSupport::StrategyList strategyList;
                strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
                strategyList.add_PoaThreadPerConnection();
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
                    std::cout << "[**** " << host_name << "]: "  << "method \"" << method 
                              << "\", where \"" << where 
                              << "\", count " << event_count 
                              << std::endl;
                    // Register the ORB initializer
                    PortableInterceptor::ORBInitializer_var initializer
                    = new TestORBInitializer(event_count, method.c_str(), where.c_str(), host_name.c_str());

                    PortableInterceptor::register_orb_initializer(initializer.in());
                }


                bool server_interceptor;
                method = OS::get_option_value( argc, argv, "--server_interceptor");
                if (method == "no") {
                    server_interceptor = false;
                }
                else {
                    server_interceptor = true;
                    std::string where = OS::get_option_value( argc, argv, "--where");
                    std::string event_count_str = OS::get_option_value( argc, argv, "--count");
                    int event_count = atoi(event_count_str.c_str());
                    std::cout << "[**** " << host_name << "]: "  << "method \"" << method 
                              << "\", where \"" << where 
                              << "\", count " << event_count 
                              << std::endl;
                    // Register the ORB initializer
                    PortableInterceptor::ORBInitializer_var initializer
                    = new TestORBInitializer(event_count, method.c_str(), where.c_str(), host_name.c_str(), true);

                    PortableInterceptor::register_orb_initializer(initializer.in());
                }
            
                // Initialises the ORB
                CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList, host_name.c_str());
        
                // Get the root POA
                CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
                PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        

                PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
                poaMgr->activate();
            
                // creates the process behaviour
                std::auto_ptr<MyProcessBehaviour> 
                pProcessBehaviour(new MyProcessBehaviour(orb.in(),rootPOA.in(), host_name));
            
                // init Cdmw
                std::cout << host_name << " test_server: init Cdmw"<<std::endl;
                Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv,  pProcessBehaviour.get());

                // Start the ORBThread 
                ORBThread orbThread(argc, argv, orb.in(), rootPOA.in(), host_name);
                orbThread.start();
                sleep(100);
                // Stop the thread
                orbThread.shutdown();
                orbThread.join();

            }
            catch( const CORBA::Exception &e )
            {
                cerr << e._name() << endl;
            }
            catch( const Cdmw::Exception &e )
            {
                cerr << e.what() << endl;
            }
            catch(...)
            {
                cerr << "Uncaught exception" << endl;
            }
        }
    }


