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


#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>


#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/ossupport/OS.hpp" 
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "platforminterface/PlatformInterface.hpp"
#include "platforminterface/ServiceNames.hpp"

#include "testvalidpersistenceuserdefinedattributes/HelloProcessBehaviour.hpp"


namespace
  {
  const int SUCCESS = 0;
  const int FAILURE = 1;

  const int POA_THREAD_POOL_SIZE = 5;
}
; // End anonymous namespace



int main(int argc, char* argv[])
{
  int status = SUCCESS;

  // wait for actviation of other process
  sleep(1);


  std::cerr << "-------------------------" << std::endl
  << "      Start Process" << std::endl
  << "-------------------------" << std::endl;
  for (int n=0; n < argc; n++)
    std::cerr << "argv[" << n <<"]: " << argv[n] << std::endl;
  std::cerr 	<< "-------------------------" << std::endl;

  CORBA::ORB_var orb;                        // orb reference

  try
    {
      // ===================================================
      // Initialize the ORB
      // ===================================================
      Cdmw::OrbSupport::StrategyList orb_strategies;
      orb_strategies.add_OrbThreaded();
      orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

      orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

      // ===================================================
      // Get the root POA
      // ===================================================
      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

      // ===================================================
      // Activate the root POA manager
      // ===================================================
      PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
      poaManager->activate();


      // ==============================================
      // Initialise the platform management interface
      // ==============================================

      // Call platform interface setup
      Cdmw::PlatformMngt::PlatformInterface::Setup(orb.in(), argc, argv);

      // creates the hello process behaviour
      std::auto_ptr<HelloProcessBehaviour> pHelloProcess(
        new HelloProcessBehaviour(orb.in()));

      // acknowledge the creation of the process
      Cdmw::PlatformMngt::PlatformInterface::Acknowledge_creation(pHelloProcess.get());
      pHelloProcess.release();

      // ===================================================
      // wait for CORBA message
      // ===================================================
      orb->run();

    }

  catch (const Cdmw::BadParameterException & ex)
    {
      std::cerr << "BAD PARAMETER " << ex.what() << std::endl;
      status = FAILURE;
    }


  catch (const Cdmw::Exception & ex)
    {
      std::cerr << " generic exception " << ex.what() << std::endl;
      status = FAILURE;
    }


  catch(const CORBA::Exception& ex)
    {
      std::cerr << ex._name() << std::endl;
      status = FAILURE;
    }


  // ========================================================
  // program stopping
  // ========================================================

  //
  // Call ORB cleanup
  // ===================================================
  Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());

  // =====================================================
  // Destroy orb
  // =====================================================
  if (!CORBA::is_nil(orb.in()))
    {
      try
        {
          orb -> destroy();
        }
      catch(const CORBA::Exception& ex)
        {
          std::cerr << ex._name() << std::endl;
          status = FAILURE;
        }
    }

  return status;
}

