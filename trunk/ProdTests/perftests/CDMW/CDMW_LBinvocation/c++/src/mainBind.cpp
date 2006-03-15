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
#include <iostream>
#include <string>
#include <fstream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Foundation/orbsupport/CosNaming.stub.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>


// Local Files
#include "TestUtils.hpp"
#include "CORBAManager.hpp"


using namespace std;
using namespace Cdmw;
using namespace Cdmw::OsSupport;


namespace
  {
  const std::string REPLICA_NAME1  = "--ReplicaName1";
  const std::string REPLICA_NAME2  = "--ReplicaName2";
  const std::string REPLICA_NAME3  = "--ReplicaName3";
  const std::string GROUP_MANAGER_URL  = "--GroupMng-url";

  const char * rep_id_pingable = "IDL:CdmwLBPerf/PingableInterface:1.0";
  const std::string lb_policy = "default";
  const std::string IOGR_FILE_NAME = "lb_pingable_group.IOGR";
  const std::string NAMING_REPOS_URL_OPTION = "--namingRepos-url";
  const int SLEEP_VAL = 5000;

  const int SUCCESS = 0;
  const int FAILURE = 1;

  static void
  usage(std::ostream & os, const std::string &program_name)
  {
    os << "\nUsage:\n"
    << program_name
    << "Options:\n\n"
    << GROUP_MANAGER_URL << "=<URL>   URL of CARDAMOM Load Balancing Group Manager.\n"
    << REPLICA_NAME1 << "=<NAM1>   string name for Replica n.1 \n"
    << REPLICA_NAME2 << "=<NAM2>   string name for Replica n.2 \n"
    << REPLICA_NAME3 << "=<NAM3>   string name for Replica n.3 \n"
    << "--namingRepos-url=<URL>  URL of the CARDAMOM Naming and Repository service.\n"
    << std:: endl;
  }
};


int main( int argc, char* argv[] )
{
  try
    {
      std::string namingReposURL = Cdmw::OsSupport::OS::get_option_value (argc, argv,NAMING_REPOS_URL_OPTION);
      std::string groupManagerURL = Cdmw::OsSupport::OS::get_option_value (argc, argv,GROUP_MANAGER_URL);

      if (namingReposURL == "no")
        {
          std::cerr << "naming and repository url undefined" << std::endl;
          usage(std::cerr, argv[0]);
          return FAILURE;
        }

      if (groupManagerURL == "no")
        {
          std::cerr << "LB Group Manager url undefined" << std::endl;
          usage(std::cerr, argv[0]);
          return FAILURE;
        }

      OsSupport::OS::sleep(SLEEP_VAL);

      std::cerr << "BindReplicas>> Start binding replicas to Group Manager..." << std::endl;
      // Initialises the ORB
      Cdmw::OrbSupport::StrategyList strategyList;
      strategyList.add_OrbThreaded();
      strategyList.add_PoaThreadPerConnection();

      CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

      // Get the LB Group Manager IOR
      CORBA::Object_var obj = orb->string_to_object(groupManagerURL.c_str());
      if (CORBA::is_nil(obj.in()))
        {
          std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
          << "could not _narrow " << groupManagerURL << std::endl;
          return -1;
        }

      CdmwLB::LBGroupManager_var group_manager
      = CdmwLB::LBGroupManager::_narrow(obj.in());

      if (CORBA::is_nil(group_manager.in()))
        {
          std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
          << "could not _narrow LBGroupManager" << std::endl;
          return -1;
        }

      ////////////////////////////////////////////////////////////////////////
      //
      // Create a group with default load balancing policy (RoundRobin)
      //
      ::PortableGroup::Properties prop;

      prop.length(1);
      prop[0].nam.length(1);
      prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
      prop[0].val <<=  lb_policy.c_str();

      ::PortableGroup::Criteria crit;
      crit.length(1);
      crit[0].nam.length(1);
      crit[0].nam[0].id="cdmw.lb.LBProperties";
      crit[0].val <<= prop;
      ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;

      ////////////////////

      CdmwNamingAndRepository::Repository_var repository =
        CdmwNamingAndRepository::Repository::_nil();

      CORBA::Object_var obj2 = orb->string_to_object(namingReposURL.c_str());
      if (CORBA::is_nil(obj2.in()))
        {
          std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
          << "could not convert to object NamingAndRepository from string" << std::endl;
          return -1;
        }

      repository = CdmwNamingAndRepository::Repository::_narrow(obj2.in());
      if (CORBA::is_nil(repository.in()))
        {
          std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
          << "could not _narrow NamingAndRepository from URL" << std::endl;
          return -1;
        }

      // ======================================================
      // Initialise the repository interface
      //     "CARDAMOM" has been chosen as the default domain name
      // ======================================================
      Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW", repository.in());

      // ======================================================
      // Get Naming Interface of PERFTEST name domain
      // ======================================================
      Cdmw::CommonSvcs::Naming::NamingInterface ni =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface (
          "PERFTEST/HELLO_LB");

      // ===================================================
      // Get the PingableInterface object from repository
      // ===================================================
      std::string file_name1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, REPLICA_NAME1);
      if (file_name1 == "no")
        {
          std::cerr << REPLICA_NAME1 << " name undefined" << std::endl;
          usage(std::cerr, argv[0]);
          return FAILURE;
        }

      CORBA::Object_var objref1 = ni.resolve (file_name1);

      // get the PingableInterface proxy
      CdmwLBPerf::PingableInterface_var pingableInterface1 =
        CdmwLBPerf::PingableInterface::_narrow(objref1.in());

      if (CORBA::is_nil(pingableInterface1.in()))
        {
          std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
          << "could not _narrow object to type PingableInterface" << std::endl;
          return -1;
        }

      std::string file_name2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, REPLICA_NAME2);
      if (file_name2 == "no")
        {
          std::cerr << REPLICA_NAME2 << " name undefined" << std::endl;
          usage(std::cerr, argv[0]);
          return FAILURE;
        }
      CORBA::Object_var objref2 = ni.resolve (file_name2);
      // get the PingableInterface proxy
      CdmwLBPerf::PingableInterface_var pingableInterface2 =
        CdmwLBPerf::PingableInterface::_narrow(objref2.in());

      if (CORBA::is_nil(pingableInterface2.in()))
        {
          std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
          << "could not _narrow object to type PingableInterface" << std::endl;
          return -1;
        }

      std::string file_name3 = Cdmw::OsSupport::OS::get_option_value (argc, argv, REPLICA_NAME3);
      if (file_name3 == "no")
        {
          std::cerr << REPLICA_NAME3 << " name undefined" << std::endl;
          usage(std::cerr, argv[0]);
          return FAILURE;
        }
      CORBA::Object_var objref3 = ni.resolve (file_name3);

      // get the PingableInterface proxy
      CdmwLBPerf::PingableInterface_var pingableInterface3 =
        CdmwLBPerf::PingableInterface::_narrow(objref3.in());

      if (CORBA::is_nil(pingableInterface3.in()))
        {
          std::cerr << "File : " << __FILE__ << " Line : " << __LINE__
          << "could not _narrow object to type PingableInterface" << std::endl;
          return -1;
        }

      obj = group_manager->create_object(rep_id_pingable,
                                         crit,
                                         factory_creation_id1.out());
      ////////////////////////////////////////////////////////////////////////
      // Add replicas
      ::PortableGroup::Location loc;
      loc.length(3);
      loc[0].id="HOST1";
      loc[0].kind="hostname1";
      loc[1].id="APPL1";
      loc[1].kind="applicationname1";
      loc[2].id="PROC1";
      loc[2].kind="processname1";

      obj = group_manager->add_member(obj.in(),
                                      loc,
                                      objref1.in());

      loc[0].id = "HOST2";
      obj = group_manager->add_member(obj.in(),
                                      loc,
                                      objref2.in());

      loc[0].id = "HOST3";
      obj = group_manager->add_member(obj.in(),
                                      loc,
                                      objref3.in());

      // export the group object reference to a file
      CORBA::String_var ref_string = orb->object_to_string(obj.in());
      std::ofstream os1(IOGR_FILE_NAME.c_str());
      os1 << ref_string.in();
      os1.close();
      std::cerr << "BindReplicas>> End binding replicas to Group Manager..." << std::endl;

      try
        {
          CORBAManager::instance()->init(orb.in());
        }
      catch(std::logic_error& e)
        {
          std::cerr<<e.what()<<std::endl;
          return -1;
        }
      catch(...)
        {
          return -1;
        }

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

}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////

