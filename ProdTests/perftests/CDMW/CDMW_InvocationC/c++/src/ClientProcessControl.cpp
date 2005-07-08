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

namespace Invocation
{
  
  ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb,
                                              int niter,
                                              int delay,
                                              char* invocation_type,
                                              char* fileprefix)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_applicationName (""),
      m_processName ("")
  {
    m_niter = niter;
    m_delay = delay;
    m_invocation_type = invocation_type;
    m_fileprefix = fileprefix;
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
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(), 
                                                         ">>>>>>>>>>>>>> Initialisation requested by supervision");
    // ===================================
    // register Tester into Repository
    // ===================================
    // Get the Cdmw repository
    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();    

    // Get DemoPerf name domain from repository
    CdmwNamingAndRepository::NameDomain_var demoPerfDomain;
    try {
        demoPerfDomain = repository->resolve_name_domain ("perf_test");
    } catch (const CdmwNamingAndRepository::NoNameDomain&) {
        std::cerr << "Can't find NameDomain perf_test" << std::endl;
        CDMW_ASSERT(false);
    } catch (const CdmwNamingAndRepository::InvalidName&) {
        std::cerr << "NameDomain perf_test is invalid !!" << std::endl;
        CDMW_ASSERT(false);
    }


    // Register the TestPing object in the "perf_test" name domain
    std::string registration_name = "TestPing";
    try {
        // reserve the name "TestPing" in the domain
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId 
            = demoPerfDomain->new_name (registration_name.c_str());
    
        // register the object
        demoPerfDomain->register_object (regId.in(), m_testping.in());

    } catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists&) {
        std::cerr << "Name " << registration_name << " already exist into NameDomain perf_test" << std::endl;
        CDMW_ASSERT(false);
    } catch (const CdmwNamingAndRepository::InvalidName&) {
        std::cerr << "Name " << registration_name << " is an invalid name !!" << std::endl;
        CDMW_ASSERT(false);
    } catch (const CdmwNamingAndRepository::NameDomain::InvalidRegistration&) {
        std::cerr << "Invalid Registration of TestPing reference as " << registration_name << std::endl;
        CDMW_ASSERT(false);
    }
    
    // =========================================
    // retrieve Tester object from Repository
    // =========================================
    // get NamingInterface to test_perf
    Cdmw::NamingAndRepository::NamingInterface invocationNamingInterface =
      Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("perf_test");
    
    // Retrieve TestPing from NamingInterface
//    std::string registration_name = "TestPing";
    try {
      CORBA::Object_var obj = invocationNamingInterface.resolve (registration_name.c_str());
      m_testping = Invocation::TestPing::_narrow(obj.in());
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
    throw(CdmwPlatformMngt::Process::NotReadyToRun, CORBA::SystemException)
  {
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(), 
                                                         ">>>>>>>>>>>>>> Run requested by supervision");

    std::cout << "Start testping" << std::endl;
    
    long double tdelta[m_niter];
    long double tabs[m_niter];

    //delay of measure
    timespec t_delay;
    t_delay.tv_sec  = m_delay / 1000;
    t_delay.tv_nsec = (m_delay % 1000)  * 1000000;

    //object for time measurement
    long double time1, time2;
    
    //test CR call and return
    //if(strcmp(m_invocation_type, CALL_RETURN)==0){
    if(m_invocation_type.compare(CALL_RETURN) == 0){
      //start of record
      for(int i = 0; i < m_niter; i++){
        //delay of measure
        if(m_delay!=0){
          // call to nanosleep
          nanosleep(& t_delay, NULL);
        }
        time1 = ctools::time::HighResClock::getTime().toMicrosec();
        m_testping->ping();
        time2 = ctools::time::HighResClock::getTime().toMicrosec();
        //time read in micro sec.       
        tdelta[i] = time2 - time1;
      }//end for
      
      //write file 
        std::string fname(m_fileprefix + "DELTA_C.dat");
        ofstream fd(fname.c_str());
        fd.precision(6);
        for(int i = 0; i < m_niter; i++)
          fd << tdelta[i] << std::endl;
        fd.close();

    }//end call_return
    else{
      //case one_way measure
      CORBA::Object_var obj =
        m_orb->resolve_initial_references ("ORBPolicyManager");
      CORBA::PolicyManager_var policy_manager =
        CORBA::PolicyManager::_narrow (obj.in ());
      
      obj = m_orb->resolve_initial_references ("PolicyCurrent");
      CORBA::PolicyCurrent_var policy_current =
        CORBA::PolicyCurrent::_narrow (obj.in());
      
      static Messaging::SyncScope sync_scope = Messaging::SYNC_WITH_TRANSPORT;
      
      if (m_invocation_type.compare(ONE_WAY)==0) {// Default SyncScope value.
        sync_scope = Messaging::SYNC_NONE;
      }
      else if (m_invocation_type.compare(ONE_WAY_TRANSPORT)==0) {
        sync_scope = Messaging::SYNC_WITH_TRANSPORT;
      }
      else if (m_invocation_type.compare(ONE_WAY_SERVER)==0) {
        sync_scope = Messaging::SYNC_WITH_SERVER;
      }
      else if (m_invocation_type.compare(ONE_WAY_TARGET)==0) {
        sync_scope = Messaging::SYNC_WITH_TARGET;
      }
      
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
      obj = m_testping->_set_policy_overrides (sync_scope_policy_list,
                                               CORBA::ADD_OVERRIDE);
      // Get the new object reference with the updated policy.
      m_testping = Invocation::TestPing::_narrow (obj.in ());
      
      // Initialize server sample collector
      m_testping->init_sdump(m_niter);
      
      //start of record
      for(int i = 0; i < m_niter; i++){
        //delay of measure
        if(m_delay!=0){
          // call to nanosleep
          nanosleep(& t_delay, NULL);
        }
        time1 = ctools::time::HighResClock::getTime().toMicrosec();
        m_testping->pos_ping_sdump(i);
        time2 = ctools::time::HighResClock::getTime().toMicrosec();
        //time read in micro sec.       
        tabs[i] = time1;
        tdelta[i] = time2 - time1;
      }
        
      //write file
      std::string filedeltaname (m_fileprefix + "DELTA_C.dat");
      std::string fileabsname (m_fileprefix + "ABS_C.dat");
      ofstream fd(filedeltaname.c_str());
      ofstream fa(fileabsname.c_str());

      fd.precision(6);
      fa.precision(6);

     for(int i = 0; i < m_niter; i++){
       fd << tdelta[i] << std::endl;
       fa << tabs[i] << std::endl;}

      fd.close();
      fa.close();
      try {      
        // Save server samples
        std::string fname(m_fileprefix + "ABS_S.dat");
        m_testping->save_sdump(fname.c_str());
      }
      catch (const CORBA::SystemException& e){
        std::cerr << " => CORBA::SystemException raised!" << std::endl;
      }    
      
    }

    ofstream feof ("end_of_test.log");
    feof<<"EOF"<< std::endl;
    feof.close();

    std::cout << "End testping" << std::endl;
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
  
}; // End namespace Invocation
