/* ========================================================================== *
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
 * ========================================================================= */

// Standard Files
#include <string>

// Cdmw Files
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <LoadBalancing/idllib/PortableGroup.stub.hpp>
#include <LoadBalancing/idllib/CdmwLBGroupManager.stub.hpp>
#include <LoadBalancing/lbgroupmanager/LBGroupManager_impl.hpp>
#include <fstream>


using namespace Cdmw;

namespace
{
    const std::string GROUP_MANAGER_PORT = "5040";
}

int main( int argc, char* argv[] )
{
    try
    {
        // Initialises the ORB
        OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();
        strategyList.add_multicast();

        // set localisation_port
        char** ORB_init_argv = 0;
        int ORB_init_argc(0);
        std::string arguments( Common::Options::LOCALISATION_SERVICE_OPTION );
        arguments += "=" + GROUP_MANAGER_PORT;
        std::string strArgv = argv[0] + std::string(" ") + arguments;
        ORB_init_argv = Common::String::to_char_array (Common::String::to_strings(strArgv));
        ORB_init_argc = 2;
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(ORB_init_argc, ORB_init_argv, strategyList);

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();


        Cdmw::LB::LBGroupManager_impl* groupManager =
            new Cdmw::LB::LBGroupManager_impl(orb.in(),
                                              rootPOA.in(),
                                              CORBA::string_dup("LB_DOMAIN_ID"),
                                              std::cerr);
        CdmwLB::LBGroupManager_var gm  = groupManager->_this();
        
        orb->register_initial_reference("LBGroupManager", gm.in());
        
        std::string group_ior_str = orb->object_to_string(gm.in());
        
        // export the object reference to a file
        std::ofstream os("group_manager.ior");
        os << group_ior_str;
        os.close();
        
        OrbSupport::OrbSupport::bind_object_to_corbaloc(
           orb.in(), "group_mgr", gm.in() );
        
        orb->run();
        
    }
    catch(const CORBA::SystemException& e)
    {
        std::cout<<"Bad end :"<<e<<std::endl;
    }
}
