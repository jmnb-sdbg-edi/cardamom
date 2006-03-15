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

namespace FederationInvocation
{
  
    ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb,
                                                int niter,
                                                int delay)
        throw(CORBA::SystemException)
        : m_orb (CORBA::ORB::_duplicate(orb)),
          m_applicationName (""),
          m_processName (""),
          m_controlledClock(0)
    {
        m_niter = niter;
        m_delay = delay;
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
    
        // =====================
        // retrieve ClockCatalog
        // =====================
        CORBA::Object_var obj = 
            m_orb->resolve_initial_references("ClockService");

        if (CORBA::is_nil(obj.in()))
            {
                std::cerr << "Client:>> Unable to resolve ClockService initial reference"
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
            entry = clock_catalog->get_entry("FederatedControlledClock");
	m_controlledClock =
            CosClockService::ControlledClock::_narrow(entry->subject.in());


        if (CORBA::is_nil(m_controlledClock.in()))
            {
                std::cerr << "Client:>> Unable to resolve FederatedControlledClock"
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
        TimeBase::TimeT currentTime = 0;
        //vectors containing correct sample + skipped sample
        long double t_res[m_niter];
        long double t_delta[m_niter];
        long double tmp_time=0;
        timespec t_delay;
        if (m_delay) {
          t_delay.tv_sec  = m_delay / 1000;
          t_delay.tv_nsec = (m_delay % 1000)  * 1000000;
        }

        // calculate invocation time of one call to the Time Service
        // Resolution of delta is 1 usec.
        for (int i = 0; i < m_niter; ++i)
        {
            t_res[i] = ctools::time::HighResClock::getTime().toMicrosec();
            currentTime = m_controlledClock->current_time();
            tmp_time = ctools::time::HighResClock::getTime().toMicrosec();
            t_delta[i] = tmp_time - t_res[i];
            if (m_delay)
              nanosleep(& t_delay, NULL);
        }
//-----------------------------------------------------------------------------
        // write file with delta of time invocation
        std::string fname("DELTA_C.dat");
        ofstream fd(fname.c_str());
        fd.precision(6);
        for(int i = 0; i < m_niter; i++)
            fd << t_delta[i] << std::endl;
        fd.close();
//-----------------------------------------------------------------------------

        // write file for shell script execution control
        ofstream feof ("end_of_test.log");
        feof<<"EOF"<< std::endl;
        feof.close();
        std::cout << "End ControlledClock test." << std::endl;
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
    } // on_stop()
  
}; // End namespace FederationInvocation
