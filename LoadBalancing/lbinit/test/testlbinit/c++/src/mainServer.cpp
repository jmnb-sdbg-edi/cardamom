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
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
#include <testlbinit/TestHello_impl.hpp>
#include <Foundation/common/String.hpp>
#include <idllib/PortableGroup.stub.hpp>
#include <idllib/CdmwLBGroupManager.stub.hpp>

namespace
{

    const std::string FILE_PATH_OPTION  = "--FilePath";
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
       << "[" << FILE_PATH_OPTION <<"]=path          path of the file where hello IOR should be stored.\n"
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

    // get Name File to save hello interface
    std::string file_name = Cdmw::OsSupport::OS::get_option_value (argc, argv, FILE_PATH_OPTION);

    if (file_name  == "no")
    {
        file_name = "hello.ior";
    }

    try
    {
        char** ORB_init_argv = 0;
        int ORB_init_argc = argc;
        std::string str_opt( "" );

        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";
        str_opt += std::string( "--LBInterceptorSide=Server");
        ORB_init_argc += 1;
        ORB_init_argv = Cdmw::Common::String::to_char_array(
            Cdmw::Common::String::to_strings( str_opt ) );

        Cdmw::LB::LBServiceInit::Init(ORB_init_argc, ORB_init_argv);
                
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



        Cdmw::HelloInterface_impl * hello_interface
            = new Cdmw::HelloInterface_impl(host_name.c_str());
        CdmwLBInit::HelloInterface_var hello_interface_ref
            =  hello_interface->_this();

        // export the object reference to a file
        CORBA::String_var ref_string = orb->object_to_string(hello_interface_ref.in());
        std::ofstream os(file_name.c_str());
        os << ref_string.in();
        os.close();

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

    return 0;
}
        
