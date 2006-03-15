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


#include <fstream>

// Cdmw Files
#include <Foundation/common/Options.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include <FaultTolerance/ftreplicationmanager/InitUtils.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>
// internal cdmw init
#include <testftreplicationmanagerinit/CdmwInterface.hpp>

#include <testftreplicationmanagerinit/TestHello_impl.hpp>
#include <testftreplicationmanagerinit/OrbThread.hpp>

#include <idllib/CdmwFTActivation.skel.hpp>

#include <idllib/CdmwFTReplicationManager.stub.hpp>
#include <idllib/CdmwFTGroupRepository.stub.hpp>
#include <idllib/FT.stub.hpp>

using namespace std;

namespace 
{

    const std::string FILE_PATH_OPTION1  = "--FilePath1";
    const std::string FILE_PATH_OPTION2  = "--FilePath2";
    const std::string RM_IOR_OPTION  = "--ReplicationMngRef";
    const std::string HOST_NAME_OPTION  = "--HostName";


} // end anonymous namespace


static void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
        << program_name << " options\n"
        << "\n"
        << "Options:\n"
        << "--help                                         Display this information.\n"
        << "\n"
//        << RM_IOR_OPTION    <<"=IOR  IOR of the ReplicationManager\n"
        << "-ORBInitRef ReplicationManager=<rep_corbaloc>  ReplicationManager corbaloc\n"
        << "[" << FILE_PATH_OPTION1 <<"]=path          path of the file where Hello IOR should be stored.\n"
        << "[" << HOST_NAME_OPTION <<"]=hostname      hostname where server is launched.\n"
        << std:: endl;
}

                               
int main( int argc, char* argv[] )
{
    // help argument
    if (argc <=2)
    {
        usage (std::cout, argv[0]);
        return EXIT_SUCCESS;
    }

    if (argc >= 2)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            usage (std::cout, argv[0]);
            return EXIT_SUCCESS;
        }
    }

    // get host name
    std::string host_name = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--HostName");

    // get Name File to save hello interface
    std::string file_name1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, FILE_PATH_OPTION1);
    
    if (file_name1  == "no") 
    {
        file_name1 = "hello.ior";
    } 
    std::cout << host_name << " test_server : file_name = " << file_name1 << std::endl;



    try
    {
        // Initialise FT service
        std::cout << "test_server: initialise FT service" << std::endl;
        Cdmw::FT::ReplicationManager::InitUtils::Pre_init(argc, argv);
       // Initialise FT service
        //Cdmw::FT::FTServiceInit::Init(argc, argv, false, true);


        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();

        std::cout << host_name << " test_server: initialises the ORB" << std::endl;
        CORBA::ORB_var orb 
            = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
  
        // Get the root POA
        std::cout << host_name << " test_server: get the root POA" << std::endl;
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        std::cout << host_name << " test_server: activate the root POA" << std::endl;
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // Get the fault Manager IOR
        std::cout << "Obtain the fault_manager " << std::endl;
        obj = orb->string_to_object("corbaloc::localhost:4555/fault_manager");
        CdmwFT::FTManager_var ftManager = CdmwFT::FTManager::_narrow(obj.in());

        obj = ftManager->get_replication_manager();        
        CdmwFT::ReplicationManager_var replication_manager 
            = CdmwFT::ReplicationManager::_narrow(obj.in());

        // post_init the replication manager creation
        Cdmw::FT::ReplicationManager::InitUtils::Post_init(replication_manager.in());

        // init Cdmw
        std::cout << host_name << " test_server: init Cdmw"<<std::endl;
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv);
        
        Cdmw::Test::HelloInterface_impl * hello_interface 
            = new Cdmw::Test::HelloInterface_impl(orb.in(), host_name);
        CdmwFT::Test::HelloInterface_var hello_interface_ref 
            =  hello_interface->_this();


        // Start the FaultDetectorThread to run the orb!
        std::cout << "Start the OrbThread " << std::endl;
        Cdmw::FT::FTInit::OrbThread orb_thread(orb.in(), 
                                               host_name);
        orb_thread.start();
        



        // export the object reference to a file
        std::cout << host_name << " test_server: export the hello object reference to a file"<<std::endl;
        CORBA::String_var ref_string2 = orb->object_to_string(hello_interface_ref.in());
        std::ofstream os2(file_name1.c_str());
        os2 << ref_string2.in();
        os2.close();

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        Cdmw::OsSupport::OS::sleep(timescale*20000);

        std::cout<< host_name << " test_server: orb run"<<std::endl;
        orb->run();

    }
    catch (const CORBA::SystemException &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e << endl;
        return EXIT_FAILURE;
    }
    catch( const CORBA::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << endl;
        return EXIT_FAILURE;
    }
    catch(...)
    {
        cerr << "Uncaught exception" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

