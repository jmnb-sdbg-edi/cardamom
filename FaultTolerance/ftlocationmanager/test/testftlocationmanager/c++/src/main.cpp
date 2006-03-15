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
#include "testftlocationmanager/TestFTActivationHandler_impl.hpp"
#include "testftlocationmanager/TestClientThread.hpp"
#include "testftlocationmanager/TestLocationManager.hpp"
#include "testftlocationmanager/TestHello_impl.hpp"


using namespace Cdmw;
using Cdmw::OsSupport::OS;

class ReplicationManagerThread : public OsSupport::Thread {

public:
        
    ReplicationManagerThread(int argc, char* argv[], CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA,const std::string&  application_name )
    {
        
        m_orb = CORBA::ORB::_duplicate(orb);
        m_rootPOA =  PortableServer::POA::_duplicate(rootPOA);
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
//         CdmwFT::ReplicationManager_var rm ;
        
//         if (application_name == "main")
//         {           
//             Cdmw::OsSupport::OS::Timeval cohort_timeout;
//             cohort_timeout.seconds=2;
//             cohort_timeout.microseconds=0;
//             Cdmw::OsSupport::OS::Timeval coordinator_timeout;
//             coordinator_timeout.seconds=2;
//             coordinator_timeout.microseconds=0;
//             // get the Replication Manager
//             using namespace Cdmw::FT::ReplicationManager;
//             rm = InitUtils::create_replication_manager (argc,
//                                                         argv,
//                                                         this->m_orb.in(),
//                                                         this->m_rootPOA.in(), 
//                                                         "My_FTdomain1",
//                                                         "5555",
//                                                         "ftdomain",
//                                                         "225.1.1.8",
//                                                         100, 100,
//                                                         cohort_timeout,
//                                                         coordinator_timeout,
//                                                         "SAFE_SECURITY_LEVEL",
//                                                         "STRICT",
//                                                         std::cerr);

//             // export the object reference to a file
//             CORBA::String_var ref_string = orb->object_to_string(rm.in());
//             std::ofstream os("rm.ior");
//             os << ref_string.in();
//             os.close();
            
//         } else {
//             CORBA::Object_var objref;
//             // import the object reference from the file
//             char *file_name1 = "rm.ior";
//             std::ifstream is1(file_name1);
//             if (is1.good())
//             {
//                 std::string objref_str;
//                 is1 >> objref_str;
//                 try
//                 {
//                     objref = m_orb->string_to_object(objref_str.c_str());
//                     rm = CdmwFT::ReplicationManager::_narrow(objref.in());

//                 }
//                 catch( CORBA::Exception& e )
//                 {
//                     std::cerr << e._name() << std::endl;
//                     std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
//                               << " IN FILE: " << __FILE__ << std::endl; 
//                 }
//                 is1.close();
//             }
//             else
//             {
//                 std::cerr << "Error opening reference filename: " << file_name1 << std::endl;
//             }
            
//         }
        
//         orb->register_initial_reference("ReplicationManager", rm.in());

        poaMgr->activate();

    }
        
    ~ReplicationManagerThread() throw() {} ;


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


                               
int main( int argc, char* argv[] )
{

    int res = 0;

    std::string str_opt( "" );
    char** argv_tmp = NULL;
    int argc_tmp = argc;
    
    for (int i=0; i<argc; i++)
        str_opt += std::string( argv[i] ) + " ";
    str_opt += std::string( "--SecurityLevel=MEDIUM_SECURITY_LEVEL --ReplicationLevel=LOOSE" );
    argc_tmp += 2;
    argv_tmp = Cdmw::Common::String::to_char_array(Cdmw::Common::String::to_strings( str_opt ) );

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
            
            // Start the ReplicationManagerThread 
            ReplicationManagerThread RMThread(argc_tmp, argv_tmp, orb.in(), rootPOA.in(), application_name);
            RMThread.start();
        
            TestClientThread testClientThread(orb.in(), rootPOA.in(), application_name);
            testClientThread.start( argc, argv);
            // Stop the thread
            RMThread.shutdown();
            RMThread.join();

        }
        catch( const CORBA::Exception &e )
        {
            std::cerr << e._name() << std::endl;
            res = -1;
        }
        catch( const Cdmw::Exception &e )
        {
            std::cerr << e.what() << std::endl;
            res = -1;
        }
        catch(...)
        {
            std::cerr << "Uncaught exception" << std::endl;
            res = -1;
        }
	    
    }
    // Else, we are the main test driver
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

            // Start the ReplicationManagerThread 
            ReplicationManagerThread RMThread(argc_tmp, argv_tmp, orb.in(), rootPOA.in(), "main");
            RMThread.start();

            // Do test
            OsSupport::OS::sleep(timescale*3000);

            // Do test
            Cdmw::FT::TestLocationManager test1(orb.in(), rootPOA.in(), "TestLocationManager", argv[0]);
            test1.start();
 
            // Stop the ReplicationManagerThread 
            RMThread.shutdown();
            RMThread.join();
            res = 1;
            std::cout<<"kill the replication manager"<<std::endl; 
            OsSupport::OS::kill_process(ft_manager_id);
            OsSupport::OS::sleep(timescale*5000);
        
        }
        catch( const CORBA::Exception &e )
        {
            std::cerr << e._name() << std::endl;
            res = -1;
        }
        catch( const Cdmw::Exception &e )
        {
            std::cerr << e.what() << std::endl;
            res = -1;
        }
        catch(...)
        {
            std::cerr << "Uncaught exception" << std::endl;
            res = -1;
        }
    }
    
    return res;
}


