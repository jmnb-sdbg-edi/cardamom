/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */


#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationAgentDeath_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/UserEvent_impl.hpp"

#include "Foundation/testutils/Testable.hpp"
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include "testcrashtimedetection/MyProcessBehaviour.hpp"

#include <cstdlib>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>


const int POA_THREAD_POOL_SIZE = 5;


int main(int argc, char* argv[])
{  

    using namespace Cdmw;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::PlatformMngt;

    OrbSupport::StrategyList strategy;
    strategy.add_OrbThreaded();
    //strategy.add_PoaThreadPerRequest();
    strategy.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

    CORBA::ORB_var orb;


    try
    {

	    
            
        orb =  Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategy); 

        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");

        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = poa->the_POAManager();

        poaMgr->activate();

        try
        {
            std::string observer = 
            Cdmw::OsSupport::OS::get_option_value(argc, argv, "--observer");
            
            std::string killer = 
            Cdmw::OsSupport::OS::get_option_value(argc, argv, "--killer");
            
            std::string nbobserver = 
            Cdmw::OsSupport::OS::get_option_value(argc, argv, "--nb-observer");

            std::string nbtest = 
            Cdmw::OsSupport::OS::get_option_value(argc, argv, "--nb-test");
            
            std::string supervision = 
            Cdmw::OsSupport::OS::get_option_value(argc, argv, "--supervision");

            std::string limittime = 
            Cdmw::OsSupport::OS::get_option_value(argc, argv, "--limit-time");

            std::string percentcase = 
            Cdmw::OsSupport::OS::get_option_value(argc, argv, "--in-percent-case");
            


            // Register event factories
            CORBA::ValueFactoryBase_var factory, old_factory;

            factory = new SystemModeChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/SystemModeChange:1.0",
                              factory.in() );

            factory = new SystemStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/SystemStatusChange:1.0",
                              factory.in() );

            factory = new HostStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/HostStatusChange:1.0",
                              factory.in() );

            factory = new ApplicationAgentDeathFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentDeath:1.0",
                              factory.in() );

            factory = new ApplicationModeChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationModeChange:1.0",
                              factory.in() );

            factory = new ApplicationStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationStatusChange:1.0",
                              factory.in() );

            factory = new ProcessMessageFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessage:1.0",
                              factory.in() );

            factory = new ProcessStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessStatusChange:1.0",
                              factory.in() );

            factory = new SystemEntityStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/SystemEntityStatusChange:1.0",
                              factory.in() );

            factory = new ApplicationEntityStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationEntityStatusChange:1.0",
                              factory.in() );

            factory = new ProcessEntityStatusChangeFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessEntityStatusChange:1.0",
                              factory.in() );

            factory = new UserEventFactory;
            old_factory = orb->register_value_factory(
                              "IDL:thalesgroup.com/CdmwPlatformMngt/UserEvent:1.0",
                              factory.in() );

            
            // creates the process behaviour
            std::auto_ptr<MyProcessBehaviour> pMyProcess(new MyProcessBehaviour(orb.in(),
                                                                                poa.in(),
                                                                                "TestCrashTimeDetection",
                                                                                observer,
                                                                                killer,
                                                                                nbobserver,
                                                                                nbtest,
                                                                                supervision,
                                                                                limittime,
                                                                                percentcase));
            // acknowledge the creation of the process
            // init Cdmw
            Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv,  pMyProcess.get());
            
            // start orb
            orb->run();
        }
        
        catch(...)
        {
            // cleanup the ORB to exit completely
            orb->shutdown(false);
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
            throw;
        }
        
        
        
        //orb->shutdown(false) is done by the ProcessBehaviour
        Cdmw::CdmwInit::CDMW_cleanup(orb.in());
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
        
    }
    catch (const CORBA::Exception& e)
    {  
    	std::cerr << "FAILURE : CORBA exception raised :" << std::endl
            << e._name() << std::endl;
    }
    catch(const std::exception &e)
    {
        std::cerr << "FAILURE : Exception raised : " << std::endl
            << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "FAILURE : Unexpected exception" << std::endl;
    }
}


