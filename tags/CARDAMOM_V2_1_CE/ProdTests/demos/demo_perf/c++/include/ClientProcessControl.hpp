/* =========================================================================== *
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
 * =========================================================================== */


#ifndef INCL_CLIENT_PROCESS_CONTROL_HPP
#define INCL_CLIENT_PROCESS_CONTROL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "Benchmark.stub.hpp"
#include "I_impl.hpp"
#include "BenchmarkThread.hpp"

#include <iostream>

namespace DemoPerf
{

class ClientProcessControl : public Cdmw::CdmwInit::ProcessControl
{
    
  public:

    ClientProcessControl (CORBA::ORB_ptr orb)
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
    * The I servant
    */
    I_impl* m_I_impl;
    
    /**
    * Tester reference
    */
    Benchmark::Tester_var m_tester;
    
    /**
    * Benchmark thread
    */
    BenchmarkThread* m_bench_thread;
    
    
};  // End class ClientProcessControl

}; // End namespace DemoPerf

#endif // INCL_CLIENT_PROCESS_CONTROL_HPP

