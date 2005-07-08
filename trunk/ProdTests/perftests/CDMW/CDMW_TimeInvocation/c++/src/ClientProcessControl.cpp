/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
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

namespace TimeInvocation
{
  
    ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb,
                                                int nstart, 
                                                int niter,
                                                int delay)
        throw(CORBA::SystemException)
        : m_orb (CORBA::ORB::_duplicate(orb)),
          m_applicationName (""),
          m_processName (""),
          m_clock(0)
    {
        m_nstart = nstart;
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
            ::notifyMessage(CdmwPlatformMngtBase::INF,
                            m_processName.c_str(),
                            ">>>>>>>>>>>>>> Initialisation requested by supervision");
    
        // ====================
        // retrieve LocalClock 
        // ====================
        CORBA::Object_var obj = 
            m_orb->resolve_initial_references("LocalClock");

        m_clock = CosClockService::Clock::_narrow(obj.in()); 
    
        if (CORBA::is_nil(m_clock.in()))
            {
                std::cerr << "Client:>> Unable to resolve LocalClock"
                          << std::endl;
                exit(1);
            }
    } // on_initialise()

///////////////////////////////////////////////////////////////////////////////

    // process to run called by platformmngt    
    void ClientProcessControl::on_run()
        throw(CdmwPlatformMngt::Process::NotReadyToRun, CORBA::SystemException)
    {

        // =================================================================
        // example of using the PlatformInterface for notifying a message
        // =================================================================
        Cdmw::PlatformMngt::PlatformInterface
            ::notifyMessage(CdmwPlatformMngtBase::INF,
                            m_processName.c_str(),
                            ">>>>>>>>>>>>>> Run requested by supervision");

//-----------------------------------------------------------------------------
        TimeBase::TimeT currentTime = 0;
        //vectors containing correct sample + skipped sample
        long double t_res[m_niter+m_nstart];
        long double t_delta[m_niter+m_nstart];
        long double tmp_time=0; 
        timespec t_delay;
        if (m_delay) {
          t_delay.tv_sec  = m_delay / 1000;
          t_delay.tv_nsec = (m_delay % 1000)  * 1000000;
        }

        // calculate invocation time of one call to the Time Service
        // Resolution of delta is 1 usec.
        for (int i = 0; i < m_niter+m_nstart; ++i)
        {          
            t_res[i] = ctools::time::HighResClock::getTime().toMicrosec();
            currentTime = m_clock->current_time(); 
            tmp_time = ctools::time::HighResClock::getTime().toMicrosec();
            t_delta[i] = tmp_time - t_res[i];
            if (m_delay)
              nanosleep(& t_delay, NULL);
        }
//-----------------------------------------------------------------------------
        //vectors containing correct sample + skipped sample
        TimeBase::TimeT local_time[m_niter+m_nstart];
        TimeBase::TimeT ts_delta[m_niter+m_nstart];
        TimeBase::TimeT ts_remoteVal=0;
        // calculate difference between the local clock and time provided
        // by the Time Service.
        // Resolution of delta is 10 usec.
        for (int i = 0; i < m_niter+m_nstart; ++i)
        {          
            local_time[i]  = ctools::localclock::compute_current_time();
            ts_remoteVal =  m_clock->current_time();
            // TODO:(??) need llabs() in case of different host (it's very
            // possible that remote time is'nt synchronized with local time)
            ts_delta[i]= ts_remoteVal - local_time[i];
            if (m_delay)
              nanosleep(& t_delay, NULL);
        }
        
//-----------------------------------------------------------------------------
        // write file with delta of time invocation
        std::string fname("DELTA_C.dat");
        ofstream fd(fname.c_str());
        fd.precision(6);
        // skip the m_nstart initial sample
        for(int i = 0; i < m_niter; i++)
            fd << t_delta[m_nstart + i] << std::endl;
        fd.close();
//-----------------------------------------------------------------------------
        // write file with delta of Time Service
        fname = "DELTA_TI.dat";
        fd.open(fname.c_str());
        fd.precision(6);
        // skip the m_nstart initial sample
        for(int i = 0; i < m_niter; i++)
            fd << ts_delta[m_nstart + i] << std::endl;
        fd.close();
//-----------------------------------------------------------------------------
        // write file with granularity of Time Service
        fname = "TI_granularity.dat";
        fd.open(fname.c_str());
        fd.precision(6);
        // skip the m_nstart initial sample
        fd << ((t_res[m_nstart+m_niter-1] -
                t_res[m_nstart])/(long double)m_niter) << std::endl;
        fd.close();
//-----------------------------------------------------------------------------
        
        // write file for shell script execution control
        ofstream feof ("end_of_test.log");
        feof<<"EOF"<< std::endl;
        feof.close();

        std::cout << "End Clock test." << std::endl;
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
            ::notifyMessage(CdmwPlatformMngtBase::INF,
                            m_processName.c_str(),
                            ">>>>>>>>>>>>>> Stop requested by supervision");
    } // on_stop()
  
}; // End namespace TimeInvocation
