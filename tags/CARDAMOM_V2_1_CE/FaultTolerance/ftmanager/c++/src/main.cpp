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


#include <FaultTolerance/ftmanager/InitUtils.hpp>

namespace 
{
  std::string TimevalToString(const Cdmw::OsSupport::OS::Timeval & time)
  {
    std::ostringstream ostr;
    ostr << time.seconds << "sec " << time.microseconds << "microsec";
    return ostr.str();
  };
};
//using namespace CdmwPlatformMngt;
using namespace Cdmw;
using namespace Common;
using namespace Cdmw::FT;
//using namespace PlatformMngt;








CORBA::ORB_var orb = CORBA::ORB::_nil();

void exit_handler(int sig) {
    if (!CORBA::is_nil(orb.in())) {
        try {
            CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"Shutdown FTManager's orb");
            Cdmw::FT::FTManager::InitUtils::Cleanup();
            orb->shutdown(false);
            CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"Signal was trapped. FTManager abort...");
            CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"########## FTManager abort date: " 
                      << TimevalToString(Cdmw::OsSupport::OS::get_time()) );
        } catch (const CORBA::SystemException& e) {
            CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"Error while shuting ORB down in exit_handler:\n"
                      << e << " - minor code: " << e.minor());
        }
    }
}




int main( int argc, char* argv[] )
{
int res = EXIT_SUCCESS;

    try
    {
        
       std::string system_port = Cdmw::FT::FTManager::InitUtils::Pre_init(argc, argv);
        
       // Start the orb
        OrbSupport::StrategyList orbStrategies;
        orbStrategies.add_OrbThreaded();
        orbStrategies.add_PoaThreadPool( 30 );
        orbStrategies.add_multicast();
        
//         std::string str_opt(argv[0]);
//         str_opt += " ";
//         str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
//         str_opt += "=";
//         str_opt += system_port;
// //         str_opt += " ";
// //         str_opt += "-ORBDebugLevel";
// //         str_opt += " ";
// //         str_opt += "10";

//         char** argv__ = Cdmw::Common::String::to_char_array
//             ( Cdmw::Common::String::to_strings(str_opt) );
//         int argc__(2);
// //        int argc__(4);




        std::string str_opt;
        int nb_args = 0;
        for (int i = 0; i< argc; i++)
        {
            str_opt += argv[i];
            str_opt += " ";
            nb_args++;
        }
        
        str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
        str_opt += "=";
        str_opt += system_port;
        nb_args++;

        char** argv__ = Cdmw::Common::String::to_char_array
            ( Cdmw::Common::String::to_strings(str_opt) );
        int argc__(nb_args);

        orb = OrbSupport::OrbSupport::ORB_init( argc__, argv__, orbStrategies );

        for (int i=0; i < argc__; ++i)
        {
            delete[] argv__[i];
        }
        delete[] argv__;

        Cdmw::FT::FTManager::InitUtils::Post_init(argc, argv, orb.in());

        struct sigaction action;
        action.sa_handler=exit_handler;
        sigemptyset(&action.sa_mask);
        sigaction(SIGTERM, &action, NULL);
        sigaction(SIGINT, &action, NULL);
        sigaction(SIGQUIT, &action, NULL);
                                                      
        orb->run();

    }
    catch ( const CORBA::Exception &e )
    {
        CDMW_ERROR(Cdmw::FT::FTLogger::GetLogger(),Cdmw::OrbSupport::OrbSupport::exception_to_string( e ));
        res = EXIT_FAILURE;
    }
    catch ( const ConfigurationException &ex )
    {
        res = EXIT_FAILURE;
        if (ex.getLineNumber() != 0)
        {
            CDMW_ERROR(Cdmw::FT::FTLogger::GetLogger(),"At Line :" << ex.getLineNumber()
                       << "\n" << ex.getMessage());
        } else {
            CDMW_ERROR(Cdmw::FT::FTLogger::GetLogger(), ex.getMessage());
        }
        
    }
    
    if( !CORBA::is_nil( orb.in() ) )
    {
        // Destroy the orb
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup( orb.in() );
        orb->destroy();
    }
    return res;
}

