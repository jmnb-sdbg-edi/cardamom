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
#include "testftmanagerreplication/TestHello_impl.hpp"
#include "testftmanagerreplication/TestFTManager.hpp"
#include <iostream>

using namespace std;

using namespace Cdmw;
using Cdmw::OsSupport::OS;

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
int main( int argc, char* argv[] )
{

        
    // Are we started as the main test driver or as a server ?
    std::string ftmanager = OS::get_option_value( argc, argv, "--faultmanager");
    std::string host_name = OS::get_hostname();
        
    // get host name
    std::string host_name1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host1");
    // get host name
    std::string host_name2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host2");

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
        = orb->string_to_object(ftmanager.c_str());
            
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
        Cdmw::FT::TestFTManager test(orb.in(), rootPOA.in(), "TestFTManager", argv[0], host_name1, host_name2);
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
                        
                        
