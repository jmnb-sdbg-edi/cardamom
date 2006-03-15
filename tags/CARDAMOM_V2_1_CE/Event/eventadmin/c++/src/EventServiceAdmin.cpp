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


#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>

#include <Foundation/common/System.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/ThreadHandle.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <SystemMngt/platforminterface/ProcessBehaviour.hpp>
#include "SystemMngt/platforminterface/ServiceNames.hpp"

#include "eventadmin/EventServiceAdminTool.hpp"


using namespace Cdmw::OsSupport;

namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;  

    const int POA_THREAD_POOL_SIZE = 5;    

    const std::string COMMAND_FILE_NAME_OPTION = "--commandFile-name";
    
    static void
    usage(std::ostream & os, const std::string &program_name)
    {
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help                       Display this information.\n"
             << "[--commandFile-name=<name>]  Name of command file.\n"
           << std:: endl;
    };

    // Thread launched in run operation of ProcessBehaviour
    class AdminThread : public Cdmw::OsSupport::Thread 
    {
    public:
        AdminThread(Cdmw::EventAdmin::EventServiceAdminTool* evt_adm)
            throw (CORBA::SystemException)
          : m_evt_adm(evt_adm)
        {
        };
        
        ~AdminThread() throw(){};
        
    protected:
        void
            run() throw()
        {
            if (m_evt_adm)
                m_evt_adm->run(std::cin,std::cout,false);
            else
                std::cerr << "AdminThread: nothing to run!" << std::endl;
        };
        
    private:
        AdminThread();
        AdminThread(const AdminThread&);
        AdminThread& operator = (const AdminThread&);
        
        Cdmw::EventAdmin::EventServiceAdminTool* m_evt_adm;   
    };

    // Default Process Behaviour
    class MyProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
    {
        
      public:
        MyProcessBehaviour(CORBA::ORB_ptr orb,
                           const std::string &command_file_name)
            throw(CORBA::SystemException)
            : m_orb(CORBA::ORB::_duplicate(orb)),
              m_naming_context(CosNaming::NamingContext::_nil()),
              m_command_file_name(command_file_name),
              m_run_thread(0)
        {
        };

         ~MyProcessBehaviour()
            throw()
        {
        };
      
        virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CORBA::SystemException)
        {
            std::cout << "INFO: cdmw_event_admin init" << std::endl;
                        
            // get the repository service
            try 
            {
                CORBA::Object_var obj 
                    = Cdmw::PlatformMngt::PlatformInterface::Get_service(Cdmw::PlatformMngt::ServiceNames::NAMING_AND_REPOSITORY_SERVICE);
                      
                CdmwNamingAndRepository::Repository_var repository 
                    = CdmwNamingAndRepository::Repository::_narrow(obj.in());
                m_naming_context 
                    = repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
            } 
            catch (const CdmwPlatformMngtService::ServiceNotFound &) 
            {
                // Cannot Proceed with no CDMW Repository Ior
                throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMRepositoryNotFound,
                                       CORBA::COMPLETED_NO);
            } 
            catch (const CdmwPlatformMngtService::ServiceUnavailable &) 
            {
                // Cannot Proceed with no CDMW Repository Ior
                throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMCannotGetRepositoryReference,
                                       CORBA::COMPLETED_NO);
            } 
            catch (const CORBA::SystemException & ex) 
            {
                throw;
            } 
            catch (const Cdmw::BadOrderException & ex) 
            {
                throw;
            } 
            catch (const Cdmw::BadParameterException & ex) 
            {
                throw;
            }            

            Cdmw::EventAdmin::EventServiceAdminTool adm(m_orb.in(), 
                                                         m_naming_context.in(), 
                                                         m_command_file_name);
            
            try
            {
                adm.init();
            }
            catch (const Cdmw::EventAdmin::InitException& e)
            {
                std::cout << e.what() << std::endl;
                Cdmw::PlatformMngt::PlatformInterface::Notify_fatal_error("cdmw_event_admin", e.what());
                return;
            }
            
            // if no command file, get input command from standard input
            if (m_command_file_name == "")
            {
                m_run_thread = new AdminThread(&adm);
            }
            // else command got from command file
            else
            {
                std::ifstream command_file (m_command_file_name.c_str(), std::ios::in);
                
                if (!command_file)
                {
                    std::cout << "cannot open file : " << m_command_file_name << std::endl;
                    Cdmw::PlatformMngt::PlatformInterface::Notify_fatal_error("cdmw_event_admin", "cannot open command file");
                }
                else
                {
                    adm.run(command_file,std::cout,true);
                }
            }  
        }
        
        virtual void run()
            throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, CORBA::SystemException)
        {
            std::cout << "INFO: cdmw_event_admin run" << std::endl;

            if (m_run_thread)
            {
                // wait for command
                m_run_thread->start();
            }
            else
            {
                // a command file was given and execute during init
                // now stop process
                m_orb->shutdown(false);
            }
        }
        
        virtual void stop()
            throw(CORBA::SystemException)
        {
            std::cout << "INFO: cdmw_event_admin stop" << std::endl;
            m_orb->shutdown(false);

            if (m_run_thread)
            {
                m_run_thread->join();
            }
        }
       

    virtual bool is_alive()
        throw(CORBA::SystemException)
    {
         return true;
    }
 
      private:
      
        CORBA::ORB_var               m_orb;
        CosNaming::NamingContext_var m_naming_context;
        std::string                  m_command_file_name;
        AdminThread*                 m_run_thread;
    };
    
}; // End anonymous namespace


int 
run(CORBA::ORB_ptr orb, 
    CosNaming::NamingContext_ptr nc, 
    const std::string &command_file_name)
{
    Cdmw::EventAdmin::EventServiceAdminTool adm(orb, nc, command_file_name);
     
     try
     {
        adm.init();
     }
     catch (const Cdmw::EventAdmin::InitException& e)
     {
         std::cout << e.what() << std::endl;
         return FAILURE;
     }
    
    // if no command file, get input command from standard input
    if (command_file_name == "")
    {
        adm.run(std::cin,std::cout,false);
    }
    // else command got from command file
    else
    {
        std::ifstream command_file (command_file_name.c_str(), std::ios::in);
        
        if (!command_file)
        {
            std::cout << "cannot open file : " << command_file_name << std::endl;
        }
        else
        {
            adm.run(command_file,std::cout,true);
        }
    }  
    
    return SUCCESS;
}

//----------------------------------------------------------------
void exceptionHandler(void)
{
    std::cout << "Event Service Admin : UNEXPECTED EXCEPTION HANDLER" << std::endl;
}

//----------------------------------------------------------------
int main(int argc, char* argv[])
{
#   ifndef _MSC_VER    

    std::set_unexpected(&exceptionHandler);

    // VC++ contrary to the clause 18.6 of the C++ standard, 
    //  set_unexpected, is not inside the std:: namespace
#   else

    set_unexpected(&exceptionHandler);

#   endif  


    int status = SUCCESS;
    CORBA::ORB_var orb;

    //
    // help argument
    //
     if(argc > 1)
     {
        if    (strcmp(argv[1], "--help") == 0)
           {
              usage(std::cout, argv[0]);
              return SUCCESS;
         }
     }

    // Initialize the ORB
    Cdmw::OrbSupport::StrategyList orb_strategies;
    orb_strategies.add_OrbThreaded();
    orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
    
    orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);

    CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(poa_obj.in());
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();

    // get Input Command File name argument
    //
    std::string commandFile = OS::get_option_value (argc, argv, COMMAND_FILE_NAME_OPTION);
    if (commandFile == "no")
    {
        commandFile = "";
    }
    
    try
    {
        using namespace Cdmw::PlatformMngt;
        bool platformManaged =  PlatformInterface::Is_launched_by_PlatformManagement(argc, argv);
            
        if (platformManaged) 
        {
            poa_manager->activate();
            
            // initialise the platform interface
            PlatformInterface::Setup(orb.in(), argc, argv);
            
            // Create a Process Behaviour
            // acknowledge the creation of the process
            PlatformInterface::Acknowledge_creation(new MyProcessBehaviour(orb.in(), commandFile));  
            
            orb->run();
            
            // cleanup the platform interface
            PlatformInterface::Cleanup();
        }
        else
        {
            // Get naming service
            CORBA::Object_var obj = orb -> resolve_initial_references("NameService");
            
            std::cout << "NameService resolved!" << std::endl;
            if(CORBA::is_nil(obj.in()))
            {
                std::cerr << "`NameService' is a nil object reference" << std::endl;
                throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                    CORBA::COMPLETED_NO);
            }
            
            CosNaming::NamingContext_var nc =
                CosNaming::NamingContext::_narrow(obj.in());
            
            std::cout << "NameService is a naming context!" << std::endl;
            
            if (CORBA::is_nil(nc.in()))
            {
                std::cerr << "`NameService' is not a NamingContext object reference" << std::endl;
                throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                    CORBA::COMPLETED_NO);
            }
            
            status = run (orb.in(), nc.in(), commandFile);
        }
    }
    catch(const CORBA::ORB::InvalidName&)
    {
        std::cerr << "Can't resolve `NameService'" << std::endl;
        status = FAILURE;
    }
    catch(const CORBA::Exception& ex)
    {
        std::cerr << ex << std::endl;
        status = FAILURE;
    }
    catch (...) 
    {
        std::cerr << "unknown exception in main" << std::endl;
        status = FAILURE;
    }
    
    if(!CORBA::is_nil(orb.in()))
    {
        try
        {
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        
    
    return status; 
}
    

