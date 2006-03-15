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
#include "testftinitsequencing/TestHello_impl.hpp"
#include "testftinitsequencing/TestInterceptors.hpp"
#include "testftinitsequencing/MyProcessBehaviour.hpp"
#include "testftinitsequencing/TestFTInitSequencing.hpp"
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

static void create_ref(std::string the_host,
                       std::string host1,
                       std::string host2,
                       CdmwFT::ReplicationManager_ptr rm,
                       CdmwNamingAndRepository::Repository_ptr repository,
                       CdmwReplicationManager::time_collector_ptr time_collector)
{
    std::cout << "Creating group reference" << std::endl << std::flush;
    const char * rid1 = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface:1.0";

    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;

    CORBA::Object_var obj1;
        
    // set default properties


    ::FT::Properties prop;
    
    prop.length(2);
    prop[0].nam.length(1);
    prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
    prop[0].val <<= (CORBA::UShort)1;
 
    ::FT::Locations locs(2);
    locs.length(2);
    locs[0].length(3);
    locs[0][0].id = host1.c_str();
    locs[0][0].kind = "hostname";
    locs[0][1].id = "APPL1";
    locs[0][1].kind = "applicationname";
    locs[0][2].id = "P11";
    locs[0][2].kind = "processname";

    locs[1].length(3);
    locs[1][0].id = host2.c_str();
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
         std::cerr << '[' << i << "] " << " --- " 
                   << Cdmw::CommonSvcs::Naming::NamingInterface::to_string(
                      locs[i]) << std::endl;
            
         factoryInfos[i].the_location = locs[i];
         ::FT::Criteria factoryCrit;        
         factoryCrit.length(0);
         factoryInfos[i].the_criteria = factoryCrit;
    }

    prop[1].nam.length(1);
    prop[1].nam[0].id="org.omg.ft.Factories";
    prop[1].val <<= factoryInfos;
    
    ::CdmwFT::StateTransfer::DataStoreIDs dsids;
    dsids.length(1);
    dsids[0] = 1;
    
    ::FT::Criteria crit1;        
    crit1.length(2);
    crit1[0].nam.length(1);
    crit1[0].nam[0].id="org.omg.ft.FTProperties";
    crit1[0].val <<= prop;
    
    crit1[1].nam.length(1);
    crit1[1].nam[0].id="com.thalesgroup.cdmw.ft.DataStoreIdentifiers";
    crit1[1].val <<= dsids;
    
    
    std::cout << "About to create group reference" << std::endl << std::flush;
    try
    {
        obj1 = rm->create_object(rid1,
                                 crit1,
                                 factory_creation_id1.out());
    }
    catch (CORBA::Exception& e)
    {
        std::cerr << e._name() << std::endl;
        abort();
    }
    std::cout << "After create group reference" << std::endl << std::flush;

    std::string full_name = "P31HelloGroup";
    std::string time_collector_name = "TimeCollector";

    // ======================================================
    // Get HELLO name domain from repository
    // ======================================================
    CdmwNamingAndRepository::NameDomain_var helloDomain =
        repository->resolve_name_domain ("dom1/dom2/dom3");

    // ========================================================
    // Register a group object in the HELLO name domain
    // ========================================================
    // reserve the name "HelloGroup" in the domain
    std::cout << "Registering object" << std::endl << std::flush;

    try
    {
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (full_name.c_str());

        // register the object
        helloDomain->register_object (regId.in(), obj1.in ());
    }
    catch (const CORBA::SystemException& e)
    {
            
        std::cerr << "ERROR: ------------------------"
                  << std::endl;
        std::cerr << e << std::endl;
        abort();
    }


    


    try
    {
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (time_collector_name.c_str());

        // register the object
        helloDomain->register_object (regId.in(), time_collector);
    }
    catch (const CORBA::SystemException& e)
    {
            
        std::cerr << "ERROR: ------------------------"
                  << std::endl;
        std::cerr << e << std::endl;
        abort();
    }

}


// ---------------------------------------------------------------------------
//
class TestORBInitializer : public PortableInterceptor::ORBInitializer,
                                                       public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    // constructor
    TestORBInitializer(PortableInterceptor::ServerRequestInterceptor_ptr server_interceptor)
    {
        m_server_interceptor = PortableInterceptor::ServerRequestInterceptor::_duplicate(server_interceptor);
    }


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

            
            try {
                info->add_server_request_interceptor(m_server_interceptor.in());
            }
            catch(const PortableInterceptor::ORBInitInfo::DuplicateName&) {
                // can't happened while interceptor is not named!
                CDMW_ASSERT(false);
            }
    }
    
private:
    PortableInterceptor::ServerRequestInterceptor_var m_server_interceptor;
    
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
            std::string host_name2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host2");
            std::string faultmanager_ior = OS::get_option_value( argc, argv, "--faultmanager");

            // No option --server, we are the main test driver 
            try
            {
                Cdmw::FT::FTServiceInit::Init(argc, argv);

                Cdmw::OrbSupport::StrategyList strategyList;
                strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
                strategyList.add_PoaThreadPerConnection();
                strategyList.add_multicast();
                // Initialises the ORB
                CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList, "main_orb");
                // Get the root POA
                CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
                PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        

                PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
                poaMgr->activate();


            
                std::cout << "Get the fault manager reference" << std::endl;
                CORBA::Object_var fm_obj =
                    orb->string_to_object(faultmanager_ior.c_str());
                std::cout << "Obtained fault manager" << std::endl << std::flush;
            
                std::cout << "Check fault manager" << std::endl << std::flush;
                if ( CORBA::is_nil( fm_obj.in() ) )
                    CDMW_THROW2(Exception,
                                Exception::FATAL,
                                "Invalid corbaloc" );
                std::cout << "Checked fault manager" << std::endl << std::flush;
            
                ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );
            
                std::cout << "Get the replication manager" << std::endl << std::flush;
                CdmwFT::ReplicationManager_var rm =  fault_manager->get_replication_manager();
            
                if ( CORBA::is_nil( rm.in() ) )
                    CDMW_THROW2(Exception,
                                Exception::FATAL,
                                "Invalid replication manager reference" );
            
                // Start the ORBThread 
                ORBThread orbThread(argc, argv, orb.in(), rootPOA.in(), "main");
                orbThread.start();

                std::cout << "About to obtain repository" << std::endl << std::flush;
                CdmwNamingAndRepository::Repository_var repository =
                    CdmwNamingAndRepository::Repository::_nil();

                CORBA::Object_var obj2 = orb->string_to_object("corbaloc::localhost:42562/CdmwRepository");
                repository = CdmwNamingAndRepository::Repository::_narrow(obj2.in());
                std::cout << "After obtain repository" << std::endl << std::flush;

                std::cout << "Invoke create_ref" << std::endl << std::flush;

                // Do test
                Cdmw::time_collector_impl* time_collector_impl = new Cdmw::time_collector_impl();
                CdmwReplicationManager::time_collector_var time_collector = time_collector_impl->_this();
                
                create_ref(host_name, host_name1, host_name2, rm.in(), repository.in(), time_collector.in());
                Cdmw::FT::TestFTInitSequencing test(orb.in(), rootPOA.in(), "TestFTInitSequencing", rm.in(), host_name1, host_name2, time_collector.in());
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

                Cdmw::TestServerInterceptor* server_interceptor_impl 
                = new Cdmw::TestServerInterceptor("request", host_name.c_str());
                
                PortableInterceptor::ORBInitializer_var initializer
                = new TestORBInitializer(server_interceptor_impl);

                PortableInterceptor::register_orb_initializer(initializer.in());
                // Initialises the ORB
                CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList, host_name.c_str());
        
                // Get the root POA
                CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
                PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        

                PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
                poaMgr->activate();
            
                // creates the process behaviour
                std::auto_ptr<MyProcessBehaviour> 
                pProcessBehaviour(new MyProcessBehaviour(orb.in(),rootPOA.in(), host_name, server_interceptor_impl));
            
                // init Cdmw
                std::cout << host_name << " -> test_server: init Cdmw"<<std::endl;
                Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv,  pProcessBehaviour.get());
                std::cout << host_name << " -> test_server: end init Cdmw"<<std::endl;

            
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
