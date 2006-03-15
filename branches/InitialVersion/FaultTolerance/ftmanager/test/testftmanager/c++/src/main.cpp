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


#include <fstream>
#include "testftmanager/FaultManagementTester.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "FaultTolerance/ftcommon/FTConfiguration.hpp"

using namespace std;

namespace
{
    
	class OrbThread : public Cdmw::OsSupport::Thread {
        
    public:
        
        OrbThread(CORBA::ORB_ptr orb)
            : m_orb(CORBA::ORB::_duplicate(orb))
        {
        }
        
        
        
        ~OrbThread() throw()
        {
        }
        
        
        
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
            std::cout <<"orb->run."<<std::endl;
            m_orb->run();   
        }
        
        
    public:
        
        CORBA::ORB_var  m_orb;
    };
    
}









int main( int argc, char* argv[])
{

    try
    {
        // Initialise FT service
        Cdmw::FT::FTServiceInit::Init(argc, argv, true);

        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();
        strategyList.add_multicast();

        
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        
        // TO be removed walk around!!!!!
        Cdmw::OrbSupport::OrbSupport::set_endpoint_selector_factory(Cdmw::FT::FTConfiguration::Get_FT_endpoint_selector_factory());

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        
        std::cout << "Obtain the ORB’s PolicyManager " << std::endl;
        obj = orb->resolve_initial_references("ORBPolicyManager");
        
        CORBA::PolicyManager_var policy_manager 
            = CORBA::PolicyManager::_narrow(obj.in());
        
        // Create and apply an ORB-wide Routed policy
        std::cout << "Create and apply an ORB-wide Routed policy " << std::endl;
        std::cout << "Creating REQUEST_DURATION_POLICY ..." << std::endl;
        CORBA::Any any;
        TimeBase::TimeT duration = 20000000;
        any <<= duration;
        CORBA::PolicyList policies(1);
        policies.length(1);
        policies[0] =
            orb->create_policy(::FT::REQUEST_DURATION_POLICY, any);
        
        policy_manager->set_policy_overrides(policies, CORBA::SET_OVERRIDE);
        
        OrbThread orb_thread(orb.in());
        orb_thread.start();

        Cdmw::FT::FaultManagementTester test_fault_manager(orb.in(), rootPOA.in(),"TestFTManager");
        test_fault_manager.start();

        orb_thread.shutdown();
        orb_thread.join();

    }
    catch( const CORBA::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << endl;
        return EXIT_FAILURE;
    }
    catch(...)
    {
        cerr << "Unexpected exception" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;   
}
