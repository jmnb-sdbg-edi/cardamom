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
#include <sstream>

#include <orbsvcs/FaultTolerance/FT_ClientService_Activate.h>

#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <FaultTolerance/ftreplicationmanager/InitUtils.hpp>
#include "testftreplicationmanager/TestFaultNotifier.hpp"
#include "testftreplicationmanager/TestPropertyManager.hpp"
#include "testftreplicationmanager/TestGenericFactory.hpp"
#include "testftreplicationmanager/TestObjectGroupManager.hpp"
#include "testftreplicationmanager/TestFailureAndElection.hpp"
#include "testftreplicationmanager/TestFaultDetector.hpp"
using namespace std;
#include "testftreplicationmanager/TypeRepositoryStub.hpp"
#include <idllib/FT.stub.hpp>
#include <idllib/CdmwFTTypeRepository.stub.hpp>
#include "FaultTolerance/ftstatemanager/DataStore.hpp"
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>
using namespace Cdmw;


class ReplicationManagerThread : public OsSupport::Thread {

public:
        
    ReplicationManagerThread( int argc, char* argv[], CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
    }
        
    ~ReplicationManagerThread() throw() {} ;


    void
    shutdown() 
    {
        m_orb->shutdown(true);
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

};


                               
int main( int argc, char* argv[] )
{

    int res = 0;

    try
    {
        // initialize ReplicationManager's interceptor
        Cdmw::FT::ReplicationManager::InitUtils::Pre_init(argc, argv);
        
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
        strategyList.add_multicast();
        // Initialises the ORB
        CORBA::ORB_var orb =
          Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        /*
          TAO_Internal::default_svc_conf_entries
          (
          "dynamic UIPMC_Factory Service_Object * TAO_PortableGroup:_make_TAO_UIPMC_Protocol_Factory() \"\"\nstatic Resource_Factory \"-ORBProtocolFactory IIOP_Factory -ORBProtocolFactory UIPMC_Factory\"",
          "dynamic PortableGroup_Loader Service_Object * TAO_PortableGroup:_make_TAO_PortableGroup_Loader() \"\"",
          ""
          );
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "myORB");
        */

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        using namespace Cdmw::OsSupport;
        OS::ProcessId replMngt_id
        = OS::create_process ("cdmw_ft_manager"," --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml --CdmwLogLevel=128 --CdmwLogFile=/tmp/cdmw_ft_manager.log");
        //= OS::create_process ("replMngt_process"," ");

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*20000);
        

        // Get the fault Manager IOR
        std::cout << "Obtain the fault_manager " << std::endl;
        obj = orb->string_to_object("corbaloc::localhost:4555/fault_manager");

        CdmwFT::FTManager_var ftManager = CdmwFT::FTManager::_narrow(obj.in());
        obj = ftManager->get_replication_manager();
        CdmwFT::ReplicationManager_var replication_manager = CdmwFT::ReplicationManager::_narrow(obj.in());


        orb->register_initial_reference("ReplicationManager", replication_manager.in());

        // Start the ReplicationManagerThread 

        std::string str_opt( "" );

        ReplicationManagerThread RMThread(argc, argv, orb.in(), rootPOA.in());
        RMThread.start();
        OsSupport::OS::sleep(timescale*3000);


        // Do test
        Cdmw::FT::TestPropertyManager test(orb.in(), rootPOA.in(), "TestPropertyManager");
        test.start();
        OsSupport::OS::sleep(timescale*300);

        Cdmw::FT::TestGenericFactory test2(orb.in(), rootPOA.in(), "TestGenericFactory");
        test2.start();
        OsSupport::OS::sleep(timescale*300);

        Cdmw::FT::TestObjectGroupManager test3(orb.in(), rootPOA.in(), "TestObjectGroupManager");
        test3.start();
        OsSupport::OS::sleep(timescale*300);

        Cdmw::FT::TestFailureAndElection test4(orb.in(), rootPOA.in(), "TestFailureAndElection");
        test4.start();
        OsSupport::OS::sleep(timescale*300);

        // Stop the ReplicationManagerThread 
        RMThread.shutdown();
        RMThread.join();
        OS::kill_process(replMngt_id );
    }
    catch( const CORBA::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e << endl;
        res = -1;
    }
    catch( const Cdmw::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e << endl;
        res = -1;
    }
    catch(...)
    {
        cerr << "Uncaught exception" << endl;
        res = -1;
    }

    return res;
}

