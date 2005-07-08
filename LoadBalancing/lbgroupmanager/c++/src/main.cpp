/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>

#include "orbsvcs/CosNamingC.h"
#define SUCCESS 0;


#include "LoadBalancing/lbgroupmanager/LBGroupManager_impl.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    int status = SUCCESS;
    CORBA::ORB_var orb;
    try
    {
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        
        orb =
            Cdmw::OrbSupport::OrbSupport::ORB_init(argc,
                                                   argv,
                                                   orb_strategies); 
        
        CORBA::Object_var obj=
            orb->resolve_initial_references("RootPOA");
        
        PortableServer::POA_var rootPOA =
            PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poa_manager =
            rootPOA->the_POAManager();

        Cdmw::LB::LBGroupManager_impl groupManager(orb.in(), rootPOA.in (), CORBA::string_dup("LB_DOMAIN_ID"), std::cout);
        CdmwLB::LBGroupManager_var gm = groupManager._this();
        
        obj =
            orb->resolve_initial_references("NameService");
        CosNaming::NamingContext_var ns =
            CosNaming::NamingContext::_narrow(obj.in());
            
        if (CORBA::is_nil(ns.in())) {
            std::cout << "Unable to resolve the Naming Service" << std::endl;
        }
        else {
            CosNaming::Name gmName;
            gmName.length (1);
            gmName[0].id = CORBA::string_dup ("GroupManager");
            ns->rebind(gmName, gm.in());
        }
        
        CORBA::String_var ior =
            orb->object_to_string(gm.in());
        std::cout << "GroupManager>> Group Manager Started." << std::endl; 
        
        poa_manager->activate();
        orb->run(); 
    }
    catch (CORBA::SystemException& e)
    {
        std::cout << "GroupManager>> Unhandled CORBA::SystemException..."
                  << e << std::endl;
        status = -1;
    }
    
    return status;

}
