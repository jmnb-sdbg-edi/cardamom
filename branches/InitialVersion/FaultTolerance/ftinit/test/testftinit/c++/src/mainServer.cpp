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

// internal cdmw init
#include <testftinit/CdmwInterface.hpp>


#include <testftinit/TestHello_impl.hpp>
// #include <testftinit/OrbThread.hpp>



#include <idllib/CdmwFTActivation.skel.hpp>

#include <idllib/CdmwFTReplicationManager.stub.hpp>
#include <idllib/CdmwFTGroupRepository.stub.hpp>
#include <idllib/FT.stub.hpp>

using namespace std;

namespace 
{

    const std::string FILE_PATH_OPTION  = "--FilePath";
    const std::string RM_IOR_OPTION  = "--ReplicationMngRef";
    const std::string HOST_NAME_OPTION  = "--HostName";

    const int POA_THREAD_POOL_SIZE = 5;   

    class HelloActivationHandler : virtual public CdmwFT::Location::ActivationHandler, 
                                   virtual public Cdmw::OrbSupport::RefCountLocalObject
    {
        std::string m_location;

    public:
        HelloActivationHandler(std::string location) : m_location(location) { };

        virtual void activate() throw(CORBA::SystemException)
        {
            std::ofstream os("hello.act");
            os << m_location;
            os.close();

            std::cout << m_location << " <####### HELLO ACTIVATION ######>" << std::endl;
        };


        virtual void deactivate() throw(CORBA::SystemException)
        {
            std::ofstream os("hello.dct");
            os << m_location;
            os.close();

            std::cout << m_location << " <####### HELLO DEACTIVATION ######>" << std::endl;
        };
    };

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
        << "[" << FILE_PATH_OPTION <<"]=path          path of the file where hello IOR should be stored.\n"
        << "[" << HOST_NAME_OPTION <<"]=hostname      hostname where server is launched.\n"
        << std:: endl;
}

CORBA::ORB_var orb = CORBA::ORB::_nil();

void exit_handler(int sig) {
    if (!CORBA::is_nil(orb.in())) {
        try {
            orb->shutdown(false);
        } catch (const CORBA::SystemException& e) {
            std::cerr << "Error while shuting ORB down in exit_handler:\n"
                      << e << " - minor code: " << e.minor() << std::endl;
        }
    }
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
    std::string file_name = Cdmw::OsSupport::OS::get_option_value (argc, argv, FILE_PATH_OPTION);
    
    if (file_name  == "no") 
    {
        file_name = "hello.ior";
    } 
    std::cout << host_name << " test_server : file_name = " << file_name << std::endl;
    

    try
    {
        // Initialise FT service
        std::cout << "test_server: initialise FT service" << std::endl;
        Cdmw::FT::FTServiceInit::Init(argc, argv);

        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        std::cout << host_name << " test_server: initialises the ORB" << std::endl;
        orb= Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
  
        // Get the root POA
        std::cout << host_name << " test_server: get the root POA" << std::endl;
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        std::cout << host_name << " test_server: activate the root POA" << std::endl;
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // init Cdmw
        std::cout << host_name << " test_server: init Cdmw"<<std::endl;
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv);


        std::cout << host_name << " test_server: create HelloInterface_impl"<<std::endl;
        // Create activation handler, it will be register in init operation of Hello
        std::cout<< host_name << " test_server: create activation handler and register it"<<std::endl;
        CdmwFT::Location::ActivationHandler_var activation_handler 
            = new HelloActivationHandler(host_name);

        Cdmw::Test::HelloInterface_impl * hello_interface 
            = new Cdmw::Test::HelloInterface_impl(orb.in(), host_name, activation_handler.in());
        CdmwFT::Test::HelloInterface_var hello_interface_ref 
            =  hello_interface->_this();
        
        // Start the HelloThread to run the orb!
//         std::cout << "Start the OrbThread " << std::endl;
//         Cdmw::FT::FTInit::OrbThread orb_thread(orb.in(), 
//                                                host_name);
//         orb_thread.start();
        


        // export the object reference to a file
        std::cout << host_name << " test_server: export the hello object reference to a file"<<std::endl;
        CORBA::String_var ref_string = orb->object_to_string(hello_interface_ref.in());
        std::ofstream os(file_name.c_str());
        os << ref_string.in();
        os.close();

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        Cdmw::OsSupport::OS::sleep(timescale*20000);

        struct sigaction action;
        action.sa_handler=exit_handler;
        sigemptyset(&action.sa_mask);
        sigaction(SIGTERM, &action, NULL);
        sigaction(SIGINT, &action, NULL);
        sigaction(SIGQUIT, &action, NULL);
        sigaction(SIGABRT, &action, NULL);

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

    if (!CORBA::is_nil(orb.in()))
    {
         try
         {
             Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
             orb->destroy();
         }
         catch(const CORBA::Exception& ex)
         {
             std::cerr << ex << std::endl;
             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                       << " IN FILE: " << __FILE__ << std::endl;
             return EXIT_FAILURE;
         }
    }        
    
    return EXIT_SUCCESS;
}

