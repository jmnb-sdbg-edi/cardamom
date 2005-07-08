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
 
#ifndef INCL_PARALLELISM_THREAD_HPP
#define INCL_PARALLELISM_THREAD_HPP

#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "Time/clockservice/CosClockService.stub.hpp"

#include <iostream>
#include "ctools/time/HighResTimer.h"
#include "ctools/time/HighResTime.h"
#include "ctools/time/HighResClock.h"
#include "ctools/time/LocalClock.h"

namespace TimeParallelism {
  
  class ParallelismThread: public virtual Cdmw::OsSupport::Thread
  {
    
  public:
    
    ParallelismThread();
    
    ParallelismThread(CosClockService::Clock_ptr clock,
                      int niter, int nstart);
    
    void setNIter(int niter);
    void setNStart(int nstart); 
    void setClock(CosClockService::Clock_ptr clock);
    long double getTimeOfTest();    
    long double getStartTime();    
    long double getEndTime();
    void reqPerfAnalisys();
    
    virtual ~ParallelismThread() throw() {};
    
    void stop() {
      m_stopped = true;
    }
    
  protected:
    
    void run() throw();
        
  private:
    
    void doPerfAnalisys()
      throw (CORBA::SystemException);
    
    /**
    * Clock reference
    */
    CosClockService::Clock_var m_clock;
    
    /**
     * thread stop flag
     */
    bool m_stopped;

      /**
     * allow estimanting of statical distribution for this thread.
     */
    bool m_reqPerfAnalisys;
    
    /**
     * Number of Parallelism call for each type 
     */
    int m_niter;
    
    /**
     * Number of Parallelism call for each type 
     */
    int m_nstart;
    
    /**
     * Start and end time 
     */
    long double m_timeval1, m_timeval2;
    
  }; // End of class ParallelismThread
  
}; // End of namespace TimeParallelism

#endif  // INCL_PARALLELISM_THREAD_HPP
