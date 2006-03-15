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


#include "testftcommon/TestFTCommon.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/String.hpp"
#include <Foundation/common/Assert.hpp>




int main (int argc, char* argv[])
{
    
    int ret_code = 0;

    try {

        char** ORB_init_argv = 0;
        int ORB_init_argc(1);
        std::string strArgv( argv[0] );
        ORB_init_argv = Cdmw::Common::String::to_char_array (Cdmw::Common::String::to_strings(strArgv));
        
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPool( 10 );
        strategyList.add_multicast();
        
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(ORB_init_argc, ORB_init_argv, strategyList ); 
        
        // Activate the POA manager
        CORBA::Object_var obj
        = orb->resolve_initial_references( "RootPOA" );
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow( obj.in() );
        
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();
    
        TestFTCommon commonTest(orb.in(), rootPOA.in(),ORB_init_argc, ORB_init_argv);
        commonTest.start();
    
        orb->shutdown(false);
        
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
        
    } catch (const CORBA::SystemException& e) {
        std::cerr << "Unexpected CORBA::SystemException:\n"
                  << e << " - minor code: " << e.minor()
                  << std::endl;
    }

    return ret_code;
}

