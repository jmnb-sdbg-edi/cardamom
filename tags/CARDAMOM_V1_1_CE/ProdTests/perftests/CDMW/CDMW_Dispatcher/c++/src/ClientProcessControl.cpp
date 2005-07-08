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

namespace Dispatcher
{

  ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb,
					      int nstart, 
					      int niter,
					      char* fileprefix)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_applicationName (""),
      m_processName ("")
  {
    m_nstart = nstart;
    m_niter = niter;
    m_fileprefix = fileprefix;
  }
  
  ClientProcessControl::~ClientProcessControl() throw(){}
  


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
                                     
    // =========================================
    // retrieve Tester object from Repository
    // =========================================
    // get NamingInterface to perf_test
    Cdmw::NamingAndRepository::NamingInterface DispatcherNamingInterface =
      Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("perf_test");
    
    // Retrieve Tester from NamingInterface
    std::string registration_name = "Tester";
    try {
      CORBA::Object_var obj = DispatcherNamingInterface.resolve (registration_name.c_str());
      m_tester = Dispatcher::Tester::_narrow(obj.in());
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
 
    std::cout << "Start testping" << std::endl;
    
    long double tdelta[m_niter + m_nstart];
    
    //object for time measurement
    long double time1, time2;

    //struct for time delay
    timespec t_delay;
    t_delay.tv_sec = 0;
    t_delay.tv_nsec = 100 * 1000000;
    
    //start of record
    for(int i = 0; i < m_nstart + m_niter; i++){
      nanosleep(& t_delay, NULL);
      time1 = ctools::time::HighResClock::getTime().toMicrosec();
      m_tester->ping();
      time2 = ctools::time::HighResClock::getTime().toMicrosec();
      //time read in micro sec.	
      tdelta[i] = time2 - time1;
    }//end for
    //write file
    std::string fname (m_fileprefix+ "DELTA_C.dat");
    ofstream fd (fname.c_str());
    fd.precision(6);
    for(int i = 0; i < m_niter; i++)
      fd<<tdelta[m_nstart + i]<< std::endl;
    fd.close();
    std::cout << "End testping" << std::endl;

    ofstream feof ("end_of_test.log");
    feof<<"EOF"<< std::endl;
    feof.close();
    
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
    
  }
  
}; // End namespace Dispatcher


