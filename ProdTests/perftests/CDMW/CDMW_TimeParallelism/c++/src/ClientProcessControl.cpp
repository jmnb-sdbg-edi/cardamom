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
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"
#include <fstream>

namespace TimeParallelism
{

  ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb,
                                              int nstart, 
                                              int niter,
                                              int nthread)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_applicationName (""),
      m_processName (""),
      m_clock(0)
  {
    m_nstart = nstart;
    m_niter = niter;
    m_nthread = nthread;
  }

  ClientProcessControl::~ClientProcessControl()
    throw()
  {
    delete [] m_bench_thread;
  }
  
  // process initialisation called by platformmngt     
  void ClientProcessControl::on_initialise (const CdmwPlatformMngtBase::StartupKind& startup_kind)
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
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
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
  
  // process to run called by platformmngt    
  void ClientProcessControl::on_run()
    throw(CdmwPlatformMngt::Process::NotReadyToRun, 
          CORBA::SystemException)
  {
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(), 
                                                         ">>>>>>>>>>>>>> Run requested by supervision");
    
    // ======================
    // run Parallelism thread
    // ======================
    int n_iter_x_thread = m_niter / m_nthread;  // number of sample invocation x thread
    
    
    //    long double tdelta[m_nthread];

    // Thread initialization
    m_bench_thread = new ParallelismThread[m_nthread];
    for (int i = 0; i< m_nthread; i++) {
      m_bench_thread[i].setClock(m_clock.in());
      m_bench_thread[i].setNIter(n_iter_x_thread);
      m_bench_thread[i].setNStart(m_nstart);
    }

    // calculate statical distribution for thread 0.
    m_bench_thread[0].reqPerfAnalisys();

    //    int status1 = ::gettimeofday(& timeval1, NULL);
    long double time1 = ctools::time::HighResClock::getTime().toMicrosec();
    
    std::cout << "Start Clock parallelism performance test" << std::endl;
    // Start threads
    for (int i = 0; i< m_nthread; i++) {
      m_bench_thread[i].start();
    } 
    
    // Wait threads
    for (int i = 0; i< m_nthread; i++) {
      m_bench_thread[i].join();
    }
    
    long double time2 = ctools::time::HighResClock::getTime().toMicrosec();

    long double time_start_min = m_bench_thread[0].getStartTime();
    long double time_end_max = m_bench_thread[0].getEndTime();

    //Read time of invocation of each thread
    for (int i = 1; i< m_nthread; i++) {
      if (time_start_min > m_bench_thread[i].getStartTime()) {
        time_start_min = m_bench_thread[i].getStartTime();
      };
      if (time_end_max < m_bench_thread[i].getEndTime()) {
        time_end_max = m_bench_thread[i].getEndTime();
      };
    }

    long double tdeltatot = time2 - time1;
    long double tdeltatot2 = time_end_max - time_start_min;

    std::string fname("TimeParallelism.out");
    ofstream fp (fname.c_str());

    fp.precision(6);
    fp << "Total time of work main= " << tdeltatot << " usec" << std::endl;
    fp << "Total time of work thread= " << tdeltatot2 << " usec" << std::endl;
    fp << "Mean time for invocation= " << tdeltatot2*1.0/(n_iter_x_thread *  m_nthread) << " usec" << std::endl;
    fp.close();
    
    fname = "MEAN_TP.dat";
    ofstream fd (fname.c_str());
    fd.precision(6);
    float temp = ((tdeltatot2*1.0) / (n_iter_x_thread *  m_nthread));
    fd << temp << std::endl;
    fd.close();
    
    std::cout << "End Clock test." << std::endl;

    ofstream feof ("end_of_test.log"); 
    feof<<"EOF"<< std::endl;
    feof.close();
  } // on_run()
  

  // process to stop called by platformmngt
  void ClientProcessControl::on_stop()
    throw(CORBA::SystemException)
  {
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(), 
                                                         ">>>>>>>>>>>>>> Stop requested by supervision");
    for (int i = 0; i< m_nthread; i++) {
      m_bench_thread[i].stop();
      m_bench_thread[i].join();
      delete [] m_bench_thread;
      m_bench_thread = 0; // preventing problem in destructor
    }
  } // on_stop()
}; // End namespace TimeParallelism

