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


#include "Foundation/common/System.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "testccmcontainer/MyComponentServer.hpp"
#include "testccmcontainer/MyComponentInstallation.hpp"
#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"
#include "testccmcontainer/TestDriver.hpp"
#include <iostream>
#include <memory>
#include <fstream>

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;    
}; // End anonymous namespace



int main(int argc, char* argv[])
{    
    int status = SUCCESS;

    CORBA::ORB_var orb;    
   
    try {
        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv);

        //
        // Get the root POA 
        //
        CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        //
        // Run implementation (Activate the POA manager)
        //
        PortableServer::POAManager_var poaManager = rootPOA -> the_POAManager();
        poaManager -> activate();
        
        std::string server = Cdmw::OsSupport::OS::get_option_value( argc, argv, "--server");
        if (server == "no") 
        {
            
            std::auto_ptr<TestCdmwCCM::TestDriver> thr( new TestCdmwCCM::TestDriver(orb.in(), rootPOA.in(), argv[0] ));
            thr->start();
            orb->run();
            thr->join();
        } else {

            std::cout<<"start component server process"<<std::endl;
            using namespace Components;

            // Creating the Component Installtion
            TestCdmwCCM::MyComponentInstallation* cmp_inst=new TestCdmwCCM::MyComponentInstallation(rootPOA.in());
            PortableServer::ServantBase_var cmp_inst_servant = cmp_inst;
            CdmwDeployment::ComponentInstallation_var component_installation = cmp_inst->_this();
            // Creating the Component Server
            TestCdmwCCM::MyComponentServer* cmp_svr = new TestCdmwCCM::MyComponentServer(orb.in(),
                                                                                         rootPOA.in(),
                                                                                         component_installation.in());
            PortableServer::ServantBase_var cmp_svr_servant = cmp_svr;
            
            Deployment::ComponentServer_var component_server = cmp_svr->_this();

            ::CORBA::String_var ref_string = orb->object_to_string(component_server.in());
            std::ofstream os("componentServer.ior");
            os << ref_string.in();
            os.close();

            orb->run();
        }
        

    } catch (const Cdmw::Exception & ex) {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    } catch(const CORBA::Exception& ex)  {        
        std::cerr << ex << std::endl;
        status = FAILURE;
    }

    if(!CORBA::is_nil(orb.in()))
    {
        try {
            Cdmw::CdmwInit::CDMW_cleanup(orb.in());
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    return status;    
}


