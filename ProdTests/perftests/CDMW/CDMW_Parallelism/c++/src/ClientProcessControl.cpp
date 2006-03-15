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
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"
#include <fstream>

namespace Parallelism
{

  ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb,
					      int nstart, 
					      int niter,
					      int nthread,
					      char* fileprefix)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_applicationName (""),
      m_processName ("")
  {
    m_nstart = nstart;
    m_niter = niter;
    m_nthread = nthread;
    //strcpy(m_fileprefix, fileprefix);
    m_fileprefix = fileprefix;
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
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
							 m_processName.c_str(), 
							 ">>>>>>>>>>>>>> Initialisation requested by supervision");
    
    // =========================================
    // retrieve Tester object from Repository
    // =========================================
    // get NamingInterface to perf_test
    Cdmw::CommonSvcs::Naming::NamingInterface ParallelismNamingInterface =
      Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("perf_test");
    
    // Retrieve Tester from NamingInterface
    std::string registration_name = "Tester";
    try {
      CORBA::Object_var obj = ParallelismNamingInterface.resolve (registration_name.c_str());
      m_tester = Parallelism::Tester::_narrow(obj.in());
      
      //Set the policy to Oneway SYNC_NONE 
      obj =
        m_orb->resolve_initial_references ("ORBPolicyManager");
      CORBA::PolicyManager_var policy_manager =
        CORBA::PolicyManager::_narrow (obj.in ());
      
      obj = m_orb->resolve_initial_references ("PolicyCurrent");
      CORBA::PolicyCurrent_var policy_current =
        CORBA::PolicyCurrent::_narrow (obj.in());
      
      static Messaging::SyncScope sync_scope = Messaging::SYNC_WITH_TRANSPORT;
      
      // Set up the sync scope any.
      CORBA::Any sync_scope_any;
      sync_scope_any <<= sync_scope;
      
      // Set the length of the policy list.
      CORBA::PolicyList sync_scope_policy_list (1);
      sync_scope_policy_list.length (1);
      
      // Set up the sync scope policy.
      sync_scope_policy_list[0] =
	m_orb->create_policy (Messaging::SYNC_SCOPE_POLICY_TYPE,
			      sync_scope_any);
      
      // Set the sync scope policy at the object level.
      obj = m_tester->_set_policy_overrides (sync_scope_policy_list,
					       CORBA::ADD_OVERRIDE);
      // Get the new object reference with the updated policy.
      m_tester = Parallelism::Tester::_narrow(obj.in ());
      
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
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
          CORBA::SystemException)
  {
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
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
      m_bench_thread[i].setTester(m_tester.in());
      m_bench_thread[i].setNIter(n_iter_x_thread);
      m_bench_thread[i].setNStart(m_nstart);
    }


    //    int status1 = ::gettimeofday(& timeval1, NULL);
    long double time1 = ctools::time::HighResClock::getTime().toMicrosec();
    
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

    std::cout << "timestart main= " << time1 << std::endl;
    std::cout << "timestart 1st thread= " << time_start_min << std::endl;
    std::cout << "timeend main= " << time2 << std::endl;
    std::cout << "timeend last thread= " << time_end_max << std::endl;

    std::cout << "Total time of work main= " << tdeltatot << std::endl;
    std::cout << "Total time of work thread= " << tdeltatot2 << std::endl;
    std::cout << "Mean time for invocation= " << tdeltatot2*1.0/(n_iter_x_thread *  m_nthread) << std::endl;

    //write file

    //FILE * fd = fopen(strcat(m_fileprefix, "DELTA_C.dat"), "w");
    //fprintf(fd, " %'.3Lf", tdeltatot2*1.0/(n_iter_x_thread *  m_nthread));
    //fclose(fd);
    
    std::string fname (m_fileprefix + "DELTA_C.dat");
    ofstream fd (fname.c_str());
    fd.precision(6);
    float temp = ((tdeltatot2*1.0) / (n_iter_x_thread *  m_nthread));
    fd << temp << std::endl;
    fd.close();
    
    std::cout << "End testping" << std::endl;

  //    FILE * feof = fopen("end_of_test.log", "w");
  //    fprintf(feof, "%s","EOF");
  //    fclose(feof);

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
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
							 m_processName.c_str(), 
							 ">>>>>>>>>>>>>> Stop requested by supervision");
    for (int i = 0; i< m_nthread; i++) {
      m_bench_thread[i].stop();
      m_bench_thread[i].join();
      delete [] m_bench_thread;
    }
    
  }

}; // End namespace Parallelism


