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


#include "teststorageinterface/TestStateManager.hpp"
#include "teststorageinterface/TestStateTransfer.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

int main (int argc, char* argv[])
{
    
    int ret_code = 0;
    
    Cdmw::OrbSupport::StrategyList strategyList;
    strategyList.add_OrbThreaded();
    strategyList.add_PoaThreadPerRequest();
    strategyList.add_multicast();

    CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

    CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
    PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
    poaMgr->activate();

    TestStateManager myTestStateManager(orb.in(), rootPOA.in());
    myTestStateManager.start();

    Cdmw::FT::TestStateTransfer myTestStateTransfer(orb.in(), rootPOA.in());
    myTestStateTransfer.start();
    
    return ret_code;
}

