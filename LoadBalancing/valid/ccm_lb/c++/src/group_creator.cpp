/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>

#include <ccm_lb/GroupCreatorProcessBehaviour.hpp>


namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;
};


int main(int argc, char* argv[])
{
  CORBA::ORB_var orb;
  try {
      // Initialise LB service
      // Cdmw::LB::LBServiceInit::Init(argc, argv);
      
      // Initialise the ORB
      Cdmw::OrbSupport::StrategyList strategyList;
      strategyList.add_OrbThreaded();
      strategyList.add_PoaThreadPerConnection();
      strategyList.add_multicast();
      
      orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
	  
      // Get the root POA
      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var rootPOA
	  = PortableServer::POA::_narrow(obj.in());
      
      PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
      poaMgr->activate();
      
      // Create the process behaviour
      std::auto_ptr<GroupCreatorProcessBehaviour> pProcessBehaviour(
	  new GroupCreatorProcessBehaviour(orb.in()));
      
      // Initialise Cdmw
      Cdmw::CdmwInit::CDMW_init(orb.in(),
				argc,
				argv,
				pProcessBehaviour.get());
      
      // Enter the main loop
      orb->run();
      
  } catch (const CORBA::Exception& e) {
      std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
      return EXIT_FAILURE;
  } catch (const Cdmw::Exception& e) {
      std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
      return EXIT_FAILURE;
  } catch ( ... ) {
      std::cerr << "Unexpected exception" << std::endl;
      return EXIT_FAILURE;
  }
 
  if ( !CORBA::is_nil( orb.in() ) )
    {
        try
	{
	    Cdmw::CdmwInit::CDMW_cleanup(orb.in());  
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    //    orb->destroy();
        }
        catch ( const CORBA::Exception &e )
        {
            std::cerr << e << std::endl;
        }
    }
 
  return EXIT_SUCCESS;
}
