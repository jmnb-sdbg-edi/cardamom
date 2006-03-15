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
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Cdmw Files
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Foundation/orbsupport/CosNaming.stub.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>


#include "time.h"
#include "sys/time.h"

// Local Files
#include "CORBAManager.hpp"
#include "TestUtils.hpp"
#include "ace/High_Res_Timer.h"

using namespace std;
using namespace Cdmw;

namespace
  {
  const int SUCCESS = 0;
  const int FAILURE = 1;
  const int SLEEP_VAL=10000;
  const int DEFAULT = 1024;
  const std::string IOR_FILE_NAME  = "hello_server.IOR";
  const std::string IOGR_FILE_NAME = "lb_pingable_group.IOGR";
  const std::string REPLICA_NAME  = "--ReplicaName";
  const std::string NAMING_REPOS_URL_OPTION = "--namingRepos-url";

  static void
  usage(std::ostream & os, const std::string &program_name)
  {
    os << "\nUsage:\n"
    << program_name
    << "Options:\n\n"
    << REPLICA_NAME << "=<NAM1>   string name for a remote Replica. \n"
    << NAMING_REPOS_URL_OPTION << "=<URL>  URL of the CARDAMOM Naming and Repository service.\n"
    << std:: endl;
  }


}// end anonymous namespace

int main( int argc, char* argv[] )
{

  try
    {
      OsSupport::OS::sleep(SLEEP_VAL);
      std::ofstream os_performance_lb("DELTA_C_LB.dat");
      std::ofstream os_performance("DELTA_C.dat");


      //number of Invocation
      int niter=0;

      //delay of Invocation
      int delay=0;


      std::cerr	<< "-------------------------" << std::endl
      << "      Start Client" << std::endl
      << "-------------------------" << std::endl;
      for (int n=0; n < argc; n++)
        std::cerr << "argv[" << n <<"]: " << argv[n] << std::endl;
      std::cerr 	<< "-------------------------" << std::endl;
      std::cout << "Reading Configuration File..." << std::endl;

      //read configuration file

      FILE * fd;
      if ((fd = fopen("client.cfg", "r"))!=NULL)
        {
          fscanf(fd,"%d",&niter);
          fscanf(fd,"%d",&delay);
          if (ferror(fd))
            {
              std::cerr << "Error in reading file client.cfg\n";
              niter=DEFAULT;
            }
          fclose(fd);
        }
      else
        {
          std::cerr << "Error: client.cfg don't exist!\n";
          niter=DEFAULT;
        }
      char** ORB_init_argv = 0;
      int ORB_init_argc = argc;
      std::string str_opt( "" );

      for (int i=0; i<argc; i++)
        str_opt += std::string( argv[i] ) + " ";
      str_opt += std::string( "--LBInterceptorSide=Client");

      ORB_init_argc += 1;
      ORB_init_argv = Cdmw::Common::String::to_char_array(
                        Cdmw::Common::String::to_strings( str_opt ) );
      Cdmw::LB::LBServiceInit::Init(ORB_init_argc, ORB_init_argv);

      // Initialise the ORB
      Cdmw::OrbSupport::StrategyList strategyList;
      strategyList.add_OrbThreaded();
      strategyList.add_PoaThreadPerConnection();

      CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

      CdmwLBPerf::PingableInterface_var pingable_group =
        Cdmw::LB::TestUtils::Get_pingable_ref_from_file(orb.in(), IOGR_FILE_NAME.c_str());
      std::cerr<<"PingClient>> calling a Load Balanced server.." << std::endl;
      std::vector<ACE_hrtime_t> data(niter);
      std::vector<ACE_hrtime_t> datalb(niter);
      //delay of measure
      timespec t_delay;
      t_delay.tv_sec  = delay / 1000;
      t_delay.tv_nsec = (delay % 1000)  * 1000000;

      ACE_High_Res_Timer timer;
      timer.calibrate();
      ACE_hrtime_t measurement;

      for(int i = 0; i<niter; i++)
        {
          if(delay!=0)
            {
              // call to nanosleep
              nanosleep(& t_delay, NULL);
            }
          timer.start();
          pingable_group->ping();
          timer.stop();
          timer.elapsed_time(measurement);
          datalb[i]=measurement;
        }

      // Get replica reference from the repository

      std::string namingReposURL = Cdmw::OsSupport::OS::get_option_value (argc, argv,NAMING_REPOS_URL_OPTION);

      if (namingReposURL == "no")
        {
          std::cerr << "naming and repository url undefined" << std::endl;
          usage(std::cerr, argv[0]);
          return FAILURE;
        }

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
      std::string replica_name = Cdmw::OsSupport::OS::get_option_value (argc, argv, REPLICA_NAME);
      if (replica_name == "no")
        {
          std::cerr << "Replica url undefined:" << std::endl;
          usage(std::cerr, argv[0]);
          return FAILURE;
        }

      CORBA::Object_var replica_ref = ni.resolve (replica_name);

      // get the PingableInterface proxy
      CdmwLBPerf::PingableInterface_var pingable_object =
        CdmwLBPerf::PingableInterface::_narrow(replica_ref.in());

      std::cerr << "PingClient>> calling a single normal server..(id: " 
                << replica_name << ")" <<std::endl;

      for(int i = 0; i<niter; i++)
        {
          if(delay!=0)
            {
              // call to nanosleep
              nanosleep(& t_delay, NULL);
            }
          timer.start();
          pingable_object->ping();
          timer.stop();
          timer.elapsed_time(measurement);
          data[i]=measurement;
        }

      for (int i = 0; i < niter; ++i)
        {
          os_performance_lb << datalb[i]<<std::endl;
          os_performance << data[i] <<std::endl;
        }
      os_performance_lb.close();
      os_performance.close();
      // write file for shell script execution control
      ofstream feof ("end_of_test.log");
      feof<<"EOF"<< std::endl;
      feof.close();

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

