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


#include "ClientProcessControl.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"
#include "TraceAndPerf/performancelibrary/PerformanceMngr.hpp"
#include "Value_impl.hpp"

namespace DemoPerf
{


ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_applicationName (""),
      m_processName (""),
      m_I_impl(NULL),
      m_bench_thread(NULL)
{
}

ClientProcessControl::~ClientProcessControl()
    throw()
{
    if (m_I_impl != NULL)
        delete m_I_impl;
    if (m_bench_thread != NULL)
        delete m_bench_thread;
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
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                     m_processName.c_str(), 
                                     ">>>>>>>>>>>>>> Initialisation requested by supervision");
                                     
    // ===============================
    // create I  object
    // ===============================
    m_I_impl = new I_impl();
    Benchmark::I_var i = m_I_impl->_this();
    
    // =========================================
    // retrieve Tester object from Repository
    // =========================================
    // get NamingInterface to demo_perf
    Cdmw::NamingAndRepository::NamingInterface demoPerfNamingInterface =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("demo_perf");
    
     // Retrieve Tester from NamingInterface
     std::string registration_name = "Tester";
     try {
         CORBA::Object_var obj = demoPerfNamingInterface.resolve (registration_name.c_str());
         m_tester = Benchmark::Tester::_narrow(obj.in());
     
     }
     catch(const CosNaming::NamingContext::NotFound& e)
     {
         std::cerr << "Resolve " << registration_name 
                   << " => NamingContext::NotFound raised!" << std::endl;
     }
     catch(const CosNaming::NamingContext::CannotProceed& e)
     {
         std::cerr << "Resolve " << registration_name 
                   << " => NamingContext::CannotProceed raised!" << std::endl;
     }
     catch(const CosNaming::NamingContext::InvalidName& e)
     {
         std::cerr << "Resolve " << registration_name 
                   << " => NamingContext::InvalidName raised!" << std::endl;
     }
     catch (const CORBA::SystemException& e)
     {
         std::cerr << "Resolve " << registration_name 
                   << " => CORBA::SystemException raised!" << std::endl;
     }                

     // ============================================
     // Register Value valuetype factory into ORB
     // ============================================
     CORBA::ValueFactoryBase_var factory, oldFactory;
     factory = new DemoPerf::ValueFactory();
     oldFactory = m_orb->register_value_factory(
         "IDL:thalesgroup.com/Benchmark/Value:1.0",
         factory.in());

    // =======================================
    // Activate the performance measurement
    // =======================================
    try {
        Cdmw::Performance::PerformanceMngr::activate();
    }
    catch (const CORBA::SystemException&) 
    {
        // Shoudn't happen: PerformanceMngr::init() has been 
        //                  called by CDMW_init()
        CDMW_ASSERT(false);
    }

}
  
  
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
    // run Benchmark thread
    // ======================
    Benchmark::I_var i = m_I_impl->_this();
    m_bench_thread = new BenchmarkThread(m_tester.in(), i.in());

    m_bench_thread->start();
}


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
                                     
   m_bench_thread->stop();
   m_bench_thread->join();
   
   delete m_bench_thread;
   m_bench_thread = NULL;
}


}; // End namespace DemoPerf


