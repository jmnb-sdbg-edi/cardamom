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

/**
 * @brief The main for test lbinit.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 *
 */

#include <string>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <idllib/PortableGroup.stub.hpp>
#include <iostream>
#include <fstream>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include <loadbalancingserver/MyProcessGroupCreatorBehaviour.hpp>

// internal cdmw init
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>


using namespace Cdmw;


int main( int argc, char* argv[] )
{
    try
    {
        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();
        

        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        
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
        // creates the process behaviour
        std::auto_ptr<MyProcessGroupCreatorBehaviour> pProcessBehaviour(new MyProcessGroupCreatorBehaviour(orb.in()));
        
        
        // init Cdmw
        std::cout << " test_server: init Cdmw"<<std::endl;
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv,  pProcessBehaviour.get());
        // Enter in the main loop
        orb->run();

        //orb->shutdown() is done by the stop() of the ProcessBehaviour
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup( orb.in() );
        orb->destroy();
    }        
    catch( const CORBA::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << endl;
        return -1;
    }
    catch( const Cdmw::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << endl;
        return -1;
    }
    catch(...)
    {
        cerr << "Unexpected exception" << endl;
        return -1;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////

