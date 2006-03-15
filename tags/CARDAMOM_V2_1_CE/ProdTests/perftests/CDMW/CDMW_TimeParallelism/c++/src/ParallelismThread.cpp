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

#include "ParallelismThread.hpp"
#include <fstream>

namespace TimeParallelism {
  
    ParallelismThread::ParallelismThread():
        m_clock(NULL),
        m_stopped(false),
        m_reqPerfAnalisys(false)
    {
    }
  
    ParallelismThread::ParallelismThread(CosClockService::Clock_ptr clock,
                                         int niter, 
                                         int nstart):
        m_clock(CosClockService::Clock::_duplicate(clock)),
        m_stopped(false)
    {
        m_niter           = niter;
        m_nstart           = nstart;
    }
  
    void ParallelismThread::setNIter(int niter) {
        m_niter           = niter;
    } 
  
    void ParallelismThread::setNStart(int nstart) {
        m_nstart           = nstart;
    } 
  
    void ParallelismThread::reqPerfAnalisys() {
        m_reqPerfAnalisys   = true;
    } 

    void ParallelismThread::setClock(CosClockService::Clock_ptr clock)
    {
        m_clock = CosClockService::Clock::_duplicate(clock);
    } 
  
    long double ParallelismThread::getTimeOfTest() {
        return m_timeval2 - m_timeval1;
    }

    long double ParallelismThread::getStartTime() {
        return m_timeval1;
    }

    long double ParallelismThread::getEndTime()
    {
        return m_timeval2;
    }
  
    // execute the same operation for all Threads, but only selected Threads
    // store information about static data.
    void ParallelismThread::run ()
        throw ()
    {
        try
            {
            
                //vectors containing correct sample + skipped sample
                long double t_res[m_niter+m_nstart];
                long double t_delta[m_niter+m_nstart];
                long double tmp_time=0; 

                m_timeval1 = ctools::time::HighResClock::getTime().toMicrosec();
                // calculate invocation time of one call to the Time Service
                // Resolution of delta is 1 usec.
                for (int i = 0; i < m_niter+m_nstart; ++i)
                    {          
                        t_res[i] = ctools::time::HighResClock::getTime().toMicrosec();
                        m_clock->current_time(); 
                        tmp_time = ctools::time::HighResClock::getTime().toMicrosec();
                        t_delta[i] = tmp_time - t_res[i];
                    }
                 m_timeval2 = ctools::time::HighResClock::getTime().toMicrosec();
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
                    }
                // save statical distribution data only for selected thread.
                if (m_reqPerfAnalisys) {
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
                    fname = "DELTA_TS.dat";
                    fd.open(fname.c_str());
                    fd.precision(6);
                    // skip the m_nstart initial sample
                    for(int i = 0; i < m_niter; i++)
                        fd << ts_delta[m_nstart + i] << std::endl;
                    fd.close();
//-----------------------------------------------------------------------------
                    // write file with granularity of Time Service
                    fname = "TP_granularity.dat";
                    fd.open(fname.c_str());
                    fd.precision(6);
                    // skip the m_nstart initial sample
                    fd << ((t_res[m_nstart+m_niter-1] -
                            t_res[m_nstart])/(long double)m_niter) << std::endl;
                    fd.close();
                }
            }
        catch (const CORBA::SystemException& e) 
            {
                std::cerr << "SystemException: " 
                          << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                          << std::endl;
            }
        catch (...)
            {
                 std::cerr << "Unknown Exception in ParallelismThread::run() " 
                           << std::endl;
                 throw;
            }

    } // run()
      
};  // End of namespace TimeParallelism

