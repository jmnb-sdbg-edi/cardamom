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


#include "starttime/GetTimeManagedProcessBehaviour2.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <starttime/TimeCollector.stub.hpp>

static Cdmw::OsSupport::OS::Timeval main_timeval;


namespace Cdmw
{


    
    
    GetTimeManagedProcessBehaviour::GetTimeManagedProcessBehaviour(CORBA::ORB_ptr orb, std::string hostname)
        : m_orb(CORBA::ORB::_duplicate(orb)),
          m_hostname(hostname)
{
    // nothing to do
    m_ctor_timeval = Cdmw::OsSupport::OS::get_time();
}

GetTimeManagedProcessBehaviour::~GetTimeManagedProcessBehaviour()
    throw ()
{
    // nothing to do
}


void GetTimeManagedProcessBehaviour::initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException)
{
        std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
    // nothing to do
    //m_init_timeval = Cdmw::OsSupport::OS::get_time();
}


void GetTimeManagedProcessBehaviour::run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
          CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
          CORBA::SystemException)
{
        std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
    Cdmw::OsSupport::OS::Timeval run_timeval = Cdmw::OsSupport::OS::get_time();
    
    std::string collector_corbaloc = "corbaloc::";
    collector_corbaloc += m_hostname;
    collector_corbaloc += ":9876/TIME_COLLECTOR";
    std::cout<<"collector_corbaloc"<<collector_corbaloc<<std::endl;
    
        std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
    CORBA::Object_var obj = m_orb->string_to_object(collector_corbaloc.c_str());
        std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
    Cdmw::Test::collector_var collector = Cdmw::Test::collector::_narrow(obj.in());
        std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
    collector->set_time_creation(run_timeval.seconds, run_timeval.microseconds);
        std::cout<<__FILE__<<" "<<__LINE__<<std::endl;
    

    std::cout << "main date : " << main_timeval.seconds << " sec " << main_timeval.microseconds << " ms\n"
              << "run date  : " << run_timeval.seconds << " sec " << run_timeval.microseconds << " ms" 
              << std::endl;
              
}

void GetTimeManagedProcessBehaviour::stop()
    throw(CORBA::SystemException)
{
    m_orb->shutdown(false);
}

    
}  // namespace



const int POA_THREAD_POOL_SIZE = 3;

int main(int argc, char* argv[])
{
    main_timeval = Cdmw::OsSupport::OS::get_time();
    
    int ret_code = EXIT_FAILURE;
    
    CORBA::ORB_var orb;
    Cdmw::GetTimeManagedProcessBehaviour* p_proc_behaviour = NULL;
    try
    {
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init( argc, argv, strategyList);

        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());
        PortableServer::POAManager_var poaMgr = poa->the_POAManager();
        poaMgr->activate();

        Cdmw::PlatformMngt::PlatformInterface::Setup(orb.in(), argc, argv);
        std::string hostname = Cdmw::OsSupport::OS::get_option_value(argc, argv, "--host");
        p_proc_behaviour = new Cdmw::GetTimeManagedProcessBehaviour(orb.in(), hostname);
        Cdmw::PlatformMngt::PlatformInterface::Acknowledge_creation(p_proc_behaviour);

        orb->run();
        
        // cleanup the platform interface
        Cdmw::PlatformMngt::PlatformInterface::Cleanup();

        ret_code = EXIT_SUCCESS;
        
    } catch (const CORBA::SystemException& e) {
        std::cout << "Error during ORB_init !" << std::endl;
        std::cout << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) << std::endl;
    }
    
    
    if (!CORBA::is_nil(orb.in())) {
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
    }
        
    
    return ret_code;
}


