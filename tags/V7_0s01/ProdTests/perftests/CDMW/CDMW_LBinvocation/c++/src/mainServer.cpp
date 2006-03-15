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
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it>
 * @author Raffaele Mele <rmele@progesi.it>
 */

// Standard Files
#include <fstream>
#include <iostream>

// Cdmw Files
#include <Foundation/common/Options.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
#include <Foundation/common/String.hpp>
#include <idllib/CdmwLBGroupManager.stub.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>


// Local Files
#include "TestPingable_impl.hpp"

namespace
  {

  const std::string REPLICA_NAME  = "--ReplicaName";
  const std::string GM_IOR_OPTION  = "--GroupMngRef";
  const std::string HOST_NAME_OPTION  = "--HostName";
  const std::string NAMING_REPOS_URL_OPTION = "--namingRepos-url";
  const std::string IOR_FILE_NAME = "hello_server.IOR";
}

using namespace std;

static void
usage(std::ostream & os, const std::string &program_name)
{
  os << "\nUsage:\n"
  << program_name << " options\n"
  << "\n"
  << "Options:\n"
  << "--help                                         Display this information.\n"
  << "\n"
  << "-ORBInitRef LBGroupManager=<rep_corbaloc>  ReplicationManager corbaloc\n"
  << "[" << REPLICA_NAME <<"]=path          path of the file where interface IOR should be stored.\n"
  << "[" << HOST_NAME_OPTION <<"]=hostname      hostname where server is launched.\n"
  << std:: endl;
}

int main( int argc, char* argv[] )
{
  // help argument
  if (argc <=2)
    {
      usage (std::cerr, argv[0]);
      return 0;
    }

  if (argc >= 2)
    {
      if (strcmp(argv[1], "--help") == 0)
        {
          usage (std::cerr, argv[0]);
          return 0;
        }
    }

  // get host name
  std::string host_name = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--HostName");

  // get Name File to save interface interface
  std::string replica_name = Cdmw::OsSupport::OS::get_option_value (argc, argv, REPLICA_NAME);

  if (replica_name  == "no")
    {
      replica_name = "interface.ior";
    }

  try
    {
      std::string namingReposURL =
        Cdmw::OsSupport::OS::get_option_value (argc, argv, NAMING_REPOS_URL_OPTION);

      if (namingReposURL == "no")
        {
          std::cerr << "naming and repository url undefined" << std::endl;
          usage(std::cerr, argv[0]);
          return 0;
        }

      // Initialises the ORB
      Cdmw::OrbSupport::StrategyList strategyList;
      strategyList.add_OrbThreaded();
      strategyList.add_PoaThreadPerConnection();

      CORBA::ORB_var orb
      = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

      // Get the root POA
      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

      PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
      poaMgr->activate();

      Cdmw::PingableInterface_impl * pingable_interface
      = new Cdmw::PingableInterface_impl();
      CdmwLBPerf::PingableInterface_var pingable_interface_ref
      =  pingable_interface->_this();

      // ===================================================
      // Get the naming and repository
      //     from the input URL
      // ===================================================
      CdmwNamingAndRepository::Repository_var repository =
        CdmwNamingAndRepository::Repository::_nil();

      CORBA::Object_var obj2 = orb->string_to_object(namingReposURL.c_str());
      repository = CdmwNamingAndRepository::Repository::_narrow(obj2.in());

      CdmwNamingAndRepository::NameDomain_var helloDomain =
        repository->resolve_name_domain ("PERFTEST/HELLO_LB");

      // ========================================================
      // Register the HelloInterface object in the HELLO_LB name domain
      // ========================================================

      // reserve the name "HelloInterface" in the domain
      CdmwNamingAndRepository::NameDomain::RegistrationId_var regId =
        helloDomain->new_name (replica_name.c_str());

      // register the object
      helloDomain->register_object (regId.in(), pingable_interface_ref.in());
      std::cerr << "HelloServer>> Hello Server Started and Registered in the Interface Repository with name: "
      << replica_name << std::endl;

      orb->run();
    }
  catch (const CORBA::SystemException &e )
    {
      cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << endl;
      return -1;
    }
  catch( const CORBA::Exception &e )
    {
      cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << endl;
      return -1;
    }
  catch(...)
    {
      cerr << "Uncaught exception" << endl;
      return -1;
    }

  return 0;
}

