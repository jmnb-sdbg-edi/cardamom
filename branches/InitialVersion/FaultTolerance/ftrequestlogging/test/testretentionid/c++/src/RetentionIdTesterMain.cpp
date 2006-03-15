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
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include <Foundation/ossupport/OS.hpp>

// internal cdmw init
#include <testretentionid/CdmwInterface.hpp>
#include <testretentionid/RetentionIdTester_impl.hpp>



namespace 
{

    const std::string FILE_PATH_OPTION  = "--FilePath";

} // end anonymous namespace


static void
usage(std::ostream & os, const std::string &program_name)
{
    os << "Usage:\n"
        << program_name << " options\n"
        << "\n"
        << "Options:\n"
        << "--help                             Display this information.\n"
        << "\n"
        << "[" << FILE_PATH_OPTION <<"]=path   path of the file where IOR should be stored.\n"
        << std::endl;
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
    if (argc >= 2 && strcmp(argv[1], "--help") == 0)
    {
        usage (std::cout, argv[0]);
        return EXIT_SUCCESS;
    }
     
    // get Name File to save RetentionIdTester IOR
    std::string ior_file = Cdmw::OsSupport::OS::get_option_value (argc, argv, FILE_PATH_OPTION);
    
    if (ior_file  == "no") 
    {
        ior_file = "hello.ior";
    } 
    std::cout << " retention_id_tester : ior file = " << ior_file << std::endl;



    try
    {
        // Initialise FT service
        Cdmw::FT::FTServiceInit::Init(argc, argv, false);

        // Initialises the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPool(4);

        std::cout << " retention_id_tester: initialises the ORB" << std::endl;
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
  
        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // init Cdmw
        std::cout << " retention_id_tester: Init Cdmw" << std::endl;
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv);
        
        // create RetentionIdTester object
        std::cout << " retention_id_tester: Create RetentionIdTester object" << std::endl;
        CdmwFT::Test::RetentionIdTester_impl * ret_id_tester 
            = new CdmwFT::Test::RetentionIdTester_impl(orb.in());
        CdmwFT::Test::RetentionIdTester_var ret_id_tester_ref 
            =  ret_id_tester->_this();


        // export the object reference to a file
        std::cout << " retention_id_tester: Save IOR into a file" << std::endl;
        CORBA::String_var ref_string2 = 
            orb->object_to_string(ret_id_tester_ref.in());
        std::ofstream os2(ior_file.c_str());
        os2 << ref_string2.in();
        os2.close();

        struct sigaction action;
        action.sa_handler=exit_handler;
        sigemptyset(&action.sa_mask);
        sigaction(SIGTERM, &action, NULL);
        sigaction(SIGINT, &action, NULL);
        sigaction(SIGQUIT, &action, NULL);
        sigaction(SIGABRT, &action, NULL);

        std::cout << " retention_id_tester: ORB run" << std::endl;
        orb->run();

    }
    catch (const CORBA::SystemException &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e << std::endl;
        return EXIT_FAILURE;
    }
    catch( const CORBA::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << std::endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr << "Uncaught exception" << std::endl;
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

