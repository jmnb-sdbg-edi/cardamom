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

//#define DEBUG_ON

namespace TimerPrecision
{
  
    ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb,
                                                size_t niter,
                                                TimeBase::TimeT deltaT,
						TimeBase::TimeT delay)
        throw(CORBA::SystemException)
        : m_orb (CORBA::ORB::_duplicate(orb)),
          m_applicationName (""),
          m_processName (""),
          m_clock(0),
          m_controller(0),
          m_periodic(0),
		  m_counter(0)
    {
        m_niter = niter;
        m_deltaT = deltaT;
	m_delay.tv_sec  = delay / 1000;
	m_delay.tv_nsec = (delay % 1000) * 1000000;
	m_t0.resize(niter);
        m_t1.resize(niter);
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

#ifdef DEBUG_ON
        std::cerr << "ClientProcessControl::on_initialise()..\n";
#endif
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
#ifdef DEBUG_ON
       std::cerr << "ClientProcessControl::on_run()..\n";
#endif
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

		m_periodic= new perfPeriodic::myPeriodic(m_t1, m_counter, this);
        CosClockService::PeriodicExecution::Periodic_var
            periodic = m_periodic->_this();

        m_controller = executor->enable_periodic_execution( periodic.in() );

		startAt();
//-----------------------------------------------------------------------------
		ifstream fin;
		fin.open("perf_timer.eof");
		while (!fin.is_open()) {
					std::cerr << "ClientProcessControl::on_run(): Waiting for Timer Test..\n";
					sleep(1);
					fin.open("perf_timer.eof");
		}
		fin.close();

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
#ifdef DEBUG_ON
        std::cerr << "ClientProcessControl::on_stop()..\n";
#endif

        // =================================================================
        // example of using the PlatformInterface for notifying a message
        // =================================================================
        Cdmw::PlatformMngt::PlatformInterface
            ::Notify_message(CdmwPlatformMngtBase::INF,
                            m_processName.c_str(),
                            ">>>>>>>>>>>>>> Stop requested by supervision");
        m_controller->terminate();
		writeRes();
    } // on_stop()

///////////////////////////////////////////////////////////////////////////////

    void ClientProcessControl::startAt()
        throw(CORBA::SystemException)
    {
        // =================================================================
        // 
        // =================================================================
		static CORBA::Any aPar;

#ifdef DEBUG_ON
       std::cerr << "ClientProcessControl::startAt()..\n";
#endif
		//delay of measure
       if ((m_delay.tv_sec !=0) || (m_delay.tv_nsec !=0))
		{
            // call to nanosleep
            nanosleep(&m_delay, NULL);
        }
		// get timestamp
		m_t0[m_counter] = ctools::time::HighResClock::getTime().toMicrosec();
		// because the executions_limit = 0 (no limit) the deltaT 
                // must be greather than m_delay.
		m_controller->start(
			m_deltaT, // period
			m_deltaT, // with_offset
			0, // executions_limit
			aPar);
	} // startAt()

///////////////////////////////////////////////////////////////////////////////

    void ClientProcessControl::writeRes()
{
#ifdef DEBUG_ON
       std::cerr << "ClientProcessControl::writeRes()..\n";
#endif
    std::ofstream fp("DELTA_C.dat");
    fp << std::ios_base::fixed;
    for (size_t n=0; n < m_counter; n++) 
	{
		// write absolute deltas between consecutive waking ups.
    	// Pay attention: delta is expressed in *** usec *** units (10^-6 sec).
	    CDMW_ASSERT(m_t1[n] >= m_t0[n]);
		fp << m_t1[n] - m_t0[n] << std::endl;
    }
    fp.close();
}
	
}; // End namespace TimerPrecision
