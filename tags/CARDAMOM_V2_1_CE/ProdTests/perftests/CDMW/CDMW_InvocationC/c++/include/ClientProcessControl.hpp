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
 
 

#ifndef INCL_CLIENT_PROCESS_CONTROL_HPP
#define INCL_CLIENT_PROCESS_CONTROL_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "ConfAndPlug/cdmwinit/InitUtils.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Invocation.skel.hpp"
#include "TestPing_i.h"
#include "tao/Messaging/Messaging.h"

#include <iostream>
#include <fstream>

#include "ctools/time/HighResTimer.h"
#include "ctools/time/HighResTime.h"
#include "ctools/time/HighResClock.h"
#include "ClockTime.hpp"

#define CALL_RETURN       "CR"
#define ONE_WAY           "OW"
#define ONE_WAY_TRANSPORT "OW_TRANSPORT"
#define ONE_WAY_SERVER    "OW_SERVER"
#define ONE_WAY_TARGET    "OW_TARGET"

#define MEASURE_HR "TIME_HR" // cTools HighRes Time
#define MEASURE_TH "TIME_TH" // clock thread cpu time 
#define MEASURE_TK "TIME_TK" // cTools HighRes clock tick count

namespace Invocation
{

class ClientProcessControl : public Cdmw::CdmwInit::ProcessControl
{
    
  public:

  //    ClientProcessControl (CORBA::ORB_ptr orb)
  //    throw(CORBA::SystemException);


  ClientProcessControl (CORBA::ORB_ptr orb,
                        int niter,
                        int delay,
                        char* invocation_type,
                        char* fileprefix,
                        ClockTime *measure_type)
    throw(CORBA::SystemException);  

    ~ClientProcessControl()
        throw();
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
     * operation
     */
    virtual  
    void on_initialise (const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CORBA::SystemException);
    
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
     * operation
     */
    virtual 
    void on_run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
                  CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    virtual 
    void on_stop()
        throw(CORBA::SystemException);

  private:
  
    /**
    * The orb reference.
    */
    CORBA::ORB_var m_orb;
    
    /**
    * The application name.
    */
    std::string m_applicationName;
      
    /**
    * The process name.
    */
    std::string m_processName;
    
    /**
    * Benchmark thread
    */
  //    BenchmarkThread* m_bench_thread;
    
    
    /**
    * TestPing pointer
    */
  Invocation::TestPing_i* testping_i;
    /**
    * TestPing reference
    */
  Invocation::TestPing_var m_testping;

  int m_niter;
  int m_delay;
  std::string m_invocation_type;
  std::string m_fileprefix;
  ClockTime *m_measure;


};  // End class ClientProcessControl

}; // End namespace Invocation

#endif // INCL_CLIENT_PROCESS_CONTROL_HPP
