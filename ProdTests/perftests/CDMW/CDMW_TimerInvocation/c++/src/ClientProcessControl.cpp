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

#include "ClientProcessControl.hpp"

namespace TimerInvocation
{
  
    ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb,
                                                int niter,
                                                TimeBase::TimeT period)
        throw(CORBA::SystemException)
        : m_orb (CORBA::ORB::_duplicate(orb)),
          m_applicationName (""),
          m_processName (""),
          m_clock(0),
          m_controller(0),
          m_periodic(0)
    {
        m_niter = niter;
        m_period = period;
    }
  
    ClientProcessControl::~ClientProcessControl()
        throw()
    {
    }
  
  
    // process initialisation called by platformmngt     
    void ClientProcessControl::on_initialise (
        const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CORBA::SystemException)
    {


        // ===================================================
        // Get the process name and application name
        // ===================================================
        m_processName = Cdmw::CdmwInit::InitUtils::get_cdmw_process_name();
    
        m_applicationName = Cdmw::CdmwInit::InitUtils::get_cdmw_application_name();
    
        // =================================================================
        // example of using the PlatformInterface for notifying a message
        // =================================================================
        Cdmw::PlatformMngt::PlatformInterface
            ::Notify_message(CdmwPlatformMngtBase::INF,
                            m_processName.c_str(),
                            ">>>>>>>>>>>>>> Initialisation requested by supervision");
    
        // ======================
        // retrieve LocalExecutor
        // ======================
        CORBA::Object_var obj = 
            m_orb->resolve_initial_references("ClockService");

        if (CORBA::is_nil(obj.in()))
            {
                std::cerr << "Client:>> Unable to resolve initial reference"
                          << std::endl;
                exit(1);
            }
        //=====================================================================
        CosClockService::ClockCatalog_var clock_catalog =
        CosClockService::ClockCatalog::_narrow(obj.in());
        if (CORBA::is_nil(clock_catalog.in()))
            {
                std::cerr << "Client:>> Unable to resolve ClockCatalog"
                          << std::endl;
                exit(1);
            }
        //=====================================================================
        CosClockService::ClockCatalog::ClockEntry_var
            entry = clock_catalog->get_entry("LocalExecutor");
        m_clock = entry->subject;

        if (CORBA::is_nil(m_clock.in()))
            {
                std::cerr << "Client:>> Unable to resolve LocalExecutor"
                          << std::endl;
                exit(1);
            }
    } // on_initialise()

///////////////////////////////////////////////////////////////////////////////

    // process to run called by platformmngt    
    void ClientProcessControl::on_run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, CORBA::SystemException)
    {

        // =================================================================
        // example of using the PlatformInterface for notifying a message
        // =================================================================
        Cdmw::PlatformMngt::PlatformInterface
            ::Notify_message(CdmwPlatformMngtBase::INF,
                            m_processName.c_str(),
                            ">>>>>>>>>>>>>> Run requested by supervision");


//-----------------------------------------------------------------------------
        CosClockService::PeriodicExecution::Executor_var executor
           = CosClockService::PeriodicExecution::Executor::_narrow(m_clock.in());

        m_periodic= new perfPeriodic::myPeriodic(m_niter);

        CosClockService::PeriodicExecution::Periodic_var
            periodic = m_periodic->_this();

        m_controller = executor->enable_periodic_execution( periodic.in() );

        CORBA::Any aPar;
        m_controller->start(m_period,
                            0,
                            m_niter,
                            aPar);
//-----------------------------------------------------------------------------
        
        // write file for shell script execution control
        ofstream feof ("end_of_test.log");
        feof<<"EOF"<< std::endl;
        feof.close();

        std::cout << "End Clock test."<< std::endl;
    } // on_run()

///////////////////////////////////////////////////////////////////////////////

    // process to stop called by platformmngt
    void ClientProcessControl::on_stop()
        throw(CORBA::SystemException)
    {
        // =================================================================
        // example of using the PlatformInterface for notifying a message
        // =================================================================
        Cdmw::PlatformMngt::PlatformInterface
            ::Notify_message(CdmwPlatformMngtBase::INF,
                            m_processName.c_str(),
                            ">>>>>>>>>>>>>> Stop requested by supervision");
        m_controller->terminate();
        m_periodic->writeRes(m_period);
    } // on_stop()
  
}; // End namespace TimerInvocation
