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

#include <fstream>

// Cdmw Files
#include <Foundation/common/Options.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
// internal cdmw init
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>

#include "LoadBalancing/lbinit/LBServiceInit.hpp"
#include <loadbalancingserver/TestHello_impl.hpp>
#include <loadbalancingserver/MyProcessHelloServerBehaviour.hpp>
#include <idllib/CdmwLBGroupManager.stub.hpp>

using namespace Cdmw;
namespace
{

    const std::string REP_NAME_OPTION  = "--RepName";
    const std::string GM_IOR_OPTION  = "--GroupMngRef";
    const std::string HOST_NAME_OPTION  = "--HostName";

}// end anonymous namespace

static void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
       << program_name << " options\n"
       << "\n"
       << "Options:\n"
       << "--help                                         Display this information.\n"
       << "\n"
       << "-ORBInitRef LBGroupManager=<rep_corbaloc>  ReplicationManager corbaloc\n"
       << "[" << REP_NAME_OPTION <<"]=repository name Name used to register Server in the Repository.\n"
       << "[" << HOST_NAME_OPTION <<"]=hostname       hostname where server is launched.\n"
       << std:: endl;
}

int main( int argc, char* argv[] )
{
  CORBA::ORB_var orb;
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

    // get Name to save hello interface
    std::string rep_name = Cdmw::OsSupport::OS::get_option_value (argc, argv, REP_NAME_OPTION);

    if (rep_name  == "no")
    {
        rep_name = "hello";
    }

    try
    {
        // Initialise LB service
        Cdmw::LB::LBServiceInit::Init( argc, argv);
        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();
	strategyList.add_multicast();

        
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        std::string full_name = host_name + "/" + rep_name;
        // creates the process behaviour
        std::auto_ptr<MyProcessHelloServerBehaviour> pProcessBehaviour(new MyProcessHelloServerBehaviour(orb.in(), full_name, rep_name));

        // init Cdmw
        std::cout << host_name << " test_server: init Cdmw"<<std::endl;
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv,  pProcessBehaviour.get());
        
        orb->run();
    }catch (const CORBA::SystemException &e )
        {
            cerr <<__FILE__<<" "<<__LINE__<<" "<< e << endl;
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

    if ( !CORBA::is_nil( orb.in() ) )
    {
        try
	{
	    Cdmw::CdmwInit::CDMW_cleanup(orb.in());  
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
	    //            orb->destroy();
        }
        catch ( const CORBA::Exception &e )
        {
            std::cerr << e << std::endl;
        }
    }

    return 0;
}
        
