/* =========================================================================== *
 * Copyright (c) 2003-2005 THALES All rights reserved.
 * Software commonly developed by THALES and AMS.
 *
 * This file is part of CARDAMOM.
 *
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with CARDAMOM; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */

#include "starttime/DaemonStarter.hpp"
#include "starttime/SystemStarter.hpp"
#include "starttime/SupervisionStarter.hpp"
#include "starttime/FTManagerStarter.hpp"
#include "starttime/ManagedProcessStarter.hpp"

#include <Foundation/osthreads/Thread.hpp>
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include <fstream>



static const std::string DAEMON_ARGS = 
    "--CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml";
static const std::string SUPERVISION_ARGS = 
" --CdmwXMLFile=CdmwPlatformMngtSystemStart.xml --validate";
static const std::string FT_MANAGER_ARGS = 
    "--CdmwXMLFile=CdmwFaultToleranceManager_conf.xml  --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml ";



static const std::string DAEMON_OPTION = "--daemon";
static const std::string FT_MANAGER_OPTION = "--ft-manager";
static const std::string ALL_SYS_MNGT_OPTION = "--all-sys-mngt";
static const std::string MNGD_PROC_OPTION = "--managed-proc";
static const std::string MNGD_PROC_HOST_OPTION = "--host-managed-proc";
static const std::string SYSTEM_OPTION = "--system";
static const std::string LOG_FILE = "--log";
    
void print_usage(const char* proc_name)
{
    std::cout << "Usage: " << proc_name << " <option>\n"
              << "     " << LOG_FILE << " : Log file to save the resutl\n" 
              << "   with 1 of the following option:\n" 
              << "     " << DAEMON_OPTION << " : test Daemon start time\n"
              << "     " << FT_MANAGER_OPTION << " : test FTManager start time\n"
              << "     " << ALL_SYS_MNGT_OPTION << " : test System Management on 1 host start time\n"
              << "     " << MNGD_PROC_OPTION << " : test managed process start time\n"
              << "            " << MNGD_PROC_HOST_OPTION << " : host where the managed process must be run\n"
              << "     " << SYSTEM_OPTION << " : test system start time\n"

              << std::endl;
}
    
    

class StarterInThread : public Cdmw::OsSupport::Thread
{
    public:
        StarterInThread(Cdmw::ProcessStarter& starter)
            : m_starter(starter),
              m_stop(false)
                
        {};
        
    void stop()
    {
        m_stop = true;
    }
    
    protected:
        
        void run()
            throw()
        {
            m_starter.start();
            m_starter.loop_until_started(100);
            while (!m_stop)
                Cdmw::OsSupport::OS::sleep(2000);
            m_starter.stop();
        }
    
    private:
        Cdmw::ProcessStarter& m_starter;
        bool m_stop;
    
};
    
    
float test_daemon_start_time(CORBA::ORB_ptr orb)
    throw (Cdmw::ProcessError)
{
    Cdmw::DaemonStarter starter(DAEMON_ARGS, orb);
        
    std::cout << "Starting daemon..." << std::endl;
    starter.start();
        
    starter.loop_until_started(100);
    std::cout << "daemon started." << std::endl;
        
    starter.stop();
    return starter.get_start_delay();    
}

float test_ft_manager_start_time(CORBA::ORB_ptr orb)
    throw (Cdmw::ProcessError)
{
    Cdmw::FTManagerStarter starter(FT_MANAGER_ARGS, orb);
    
    std::cout << "Starting FTManager..." << std::endl;
    starter.start();
    
    starter.loop_until_started(100);
    std::cout << "FTManager started." << std::endl;
    
    starter.stop();
    return starter.get_start_delay();
}


float test_all_system_management_start_time(CORBA::ORB_ptr orb)
{
    Cdmw::DaemonStarter daemon_starter(DAEMON_ARGS, orb);
    Cdmw::FTManagerStarter ft_manager_starter(FT_MANAGER_ARGS, orb);
    Cdmw::SupervisionStarter supervision_starter(SUPERVISION_ARGS, orb);


    Cdmw::OsSupport::OS::Timeval  start_time = Cdmw::OsSupport::OS::get_time();
    
    std::cout << "========= Starting daemon =======" << std::endl;
    // start daemon in a thread as it can be started parallel
    StarterInThread daemon_thread(daemon_starter);
    daemon_thread.start();


     std::cout << "======= Starting FTManager ======" << std::endl;
     ft_manager_starter.start();

     // wait FTManager to start supervision
     ft_manager_starter.loop_until_started(100);
     std::cout << "====== Starting Supervision =====" << std::endl;
     supervision_starter.start();
     // wait Supervision
     supervision_starter.loop_until_started(100);
     Cdmw::OsSupport::OS::Timeval  started_time = Cdmw::OsSupport::OS::get_time();
     
     daemon_thread.stop();
     daemon_thread.join();
     ft_manager_starter.stop();
     
     float delay;
     delay = (float)started_time.seconds - (float)start_time.seconds;
     delay += ((float)started_time.microseconds - (float)start_time.microseconds) / 1000000;
    
    float daemon_delay = daemon_starter.get_start_delay();
    float ft_manager_delay = ft_manager_starter.get_start_delay();
    float supervision_delay = supervision_starter.get_start_delay();
    
    float global_delay = delay;

    std::cout << "============ Results ============\n"     
              << "   Daemon start time      : " << daemon_delay << " sec\n"       
              << "   FTManager start time   : " << ft_manager_delay << " sec\n"       
              << "   Supervision start time : " << supervision_delay << " sec\n"
              << std::endl;


    return global_delay;
}



float test_managed_process_start_time(CORBA::ORB_ptr orb, std::string hostname)
    throw (Cdmw::ProcessError)
{
    Cdmw::ManagedProcessStarter starter(orb, hostname);
    
    std::cout << "Starting managed process..." << std::endl;
    starter.start();
    
    starter.loop_until_started(100);
    std::cout << "managed process started." << std::endl;
    
    starter.stop();
    return starter.get_start_delay();
}


float test_system_start_time(CORBA::ORB_ptr orb)
    throw (Cdmw::ProcessError)
{
    Cdmw::SystemStarter starter(orb);
    
    std::cout << "Starting system..." << std::endl;
    starter.start();
    
    starter.loop_until_started(1000);
    std::cout << "system started." << std::endl;
    
    starter.stop();
    return starter.get_start_delay();
}



int main(int argc, char* argv[])
{

    int ret_code = EXIT_FAILURE;
    
    CORBA::ORB_var orb;
    try
    {
        std::string str_opt(argv[0]);
        str_opt += " ";
        str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
        str_opt += "=";
        str_opt += "9876";

        char** argv__ = Cdmw::Common::String::to_char_array
            ( Cdmw::Common::String::to_strings(str_opt) );
        int argc__(2);

        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPool(5);
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init( argc__, argv__, strategyList);
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());
        PortableServer::POAManager_var poaMgr = poa->the_POAManager();
        poaMgr->activate();

        std::string file_name = Cdmw::OsSupport::OS::get_option_value(argc, argv, LOG_FILE);
        

        if (strcmp(file_name.c_str(), "yes") && strcmp(file_name.c_str(),"no")) 
        {
            
            
            if (Cdmw::OsSupport::OS::get_option_value(argc, argv, DAEMON_OPTION) == "yes") 
            {
                float delay = test_daemon_start_time(orb.in());
                std::cout << "Daemon start time : " 
                          << delay << " seconds" << std::endl;
                std::ofstream ofs(file_name.c_str());
                ofs << "Daemon start time : " 
                    << delay << " seconds" << std::endl;
                ofs.close();
                
                ret_code = EXIT_SUCCESS;
            }
            else if (Cdmw::OsSupport::OS::get_option_value(argc, argv, FT_MANAGER_OPTION) == "yes") 
            {
                float delay = test_ft_manager_start_time(orb.in());
                std::cout << "FTManager start time : " 
                          << delay << " seconds" << std::endl;
                std::ofstream ofs(file_name.c_str());
                ofs << "FTManager start time : " 
                    << delay << " seconds" << std::endl;
                ofs.close();
                ret_code = EXIT_SUCCESS;
            }
            else if (Cdmw::OsSupport::OS::get_option_value(argc, argv, ALL_SYS_MNGT_OPTION) == "yes") 
            {
                float delay = test_all_system_management_start_time(orb.in());
                std::cout << "All System Management start time : " 
                          << delay << " seconds" << std::endl;
                std::ofstream ofs(file_name.c_str());
                ofs << "All System Management start time : " 
                    << delay << " seconds" << std::endl;
                ofs.close();
                ret_code = EXIT_SUCCESS;
            }
            else if (Cdmw::OsSupport::OS::get_option_value(argc, argv, SYSTEM_OPTION) == "yes") 
            {
                float delay = test_system_start_time(orb.in());
                std::cout << "All System start time : " 
                          << delay << " seconds" << std::endl;
                std::ofstream ofs(file_name.c_str());
                ofs << "All System start time : " 
                    << delay << " seconds" << std::endl;
                ofs.close();
                ret_code = EXIT_SUCCESS;
            }
            else if (Cdmw::OsSupport::OS::get_option_value(argc, argv, MNGD_PROC_OPTION) == "yes")
            {
                std::string hostname = Cdmw::OsSupport::OS::get_option_value(argc, argv,MNGD_PROC_HOST_OPTION);
                
                if ((hostname != "yes") && (hostname != "no")) 
                {
                    float delay = test_managed_process_start_time(orb.in(), hostname);
                    std::cout << "Manager Process start time : " 
                              << delay << " seconds" << std::endl;
                    std::ofstream ofs(file_name.c_str());
                    ofs  << "Manager Process start time : " 
                         << delay << " seconds" << std::endl;
                    ofs.close();
                    ret_code = EXIT_SUCCESS;
                }
                else
                {
                    print_usage(argv[0]);
                }
            }
            else
            {
                print_usage(argv[0]);
            }
        }
        else
        {
            print_usage(argv[0]);
        }
        
    } catch (const CORBA::SystemException& e) {
        std::cout << "Error during ORB_init !" << std::endl;
        std::cout << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) << std::endl;
    } catch (const Cdmw::ProcessError& e) {
        std::cout << e.what() << std::endl;
    }
    
    
    if (!CORBA::is_nil(orb.in())) {
        orb->destroy();
    }
    sleep(2);
    
    return ret_code;
    
    
    
}
 
 
