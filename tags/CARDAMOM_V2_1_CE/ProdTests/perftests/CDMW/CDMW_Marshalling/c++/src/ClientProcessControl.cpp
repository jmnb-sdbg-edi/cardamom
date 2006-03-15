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

namespace Marshalling
{
    
  ClientProcessControl::ClientProcessControl (CORBA::ORB_ptr orb,
                                              int niter,
                                              int delay,
                                              char* invocation_type,
                                              char* fileprefix,
                                              char* data_type,
                                              int data_size,
                                              ClockTime* measure_type)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_applicationName (""),
      m_processName ("")
  {
    m_niter = niter;
    m_delay = delay;
    m_invocation_type = invocation_type;
    m_fileprefix = fileprefix;
    m_data_type = data_type;
    m_data_size = data_size;
    m_measure = measure_type;
  }
  
  ClientProcessControl::~ClientProcessControl()
    throw()
  {
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
    // get NamingInterface to test_perf
    Cdmw::CommonSvcs::Naming::NamingInterface marshallingNamingInterface =
      Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("perf_test");
    
    // Retrieve TestPing from NamingInterface
    std::string registration_name = "TestPing";
    try {
      CORBA::Object_var obj = marshallingNamingInterface.resolve (registration_name.c_str());
      m_testping = Marshalling::TestPing::_narrow(obj.in());
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
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, CORBA::SystemException)
  {
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(), 
                                                         ">>>>>>>>>>>>>> Run requested by supervision");
    
    std::cout << "Start testping" << std::endl;
    long double tdelta[m_niter];

    //delay of measure
    timespec t_delay;
    t_delay.tv_sec  = m_delay / 1000;
    t_delay.tv_nsec = (m_delay % 1000) * 1000000;
    
    Marshalling::LongSeq_var lseq;        
    Marshalling::FloatSeq_var fseq;    
    CORBA::Any_var any(new CORBA::Any());
    Marshalling::RecordSeq_var rseq;
    
    //creating object    
    if(m_data_type.compare(TYPE_LONG)==0){
      unsigned int lSeq_size = m_data_size / 4;
      CORBA::Long* lSeq_buffer;
      lSeq_buffer = new CORBA::Long[lSeq_size];
      lseq = new Marshalling::LongSeq(lSeq_size, lSeq_size, lSeq_buffer, 0);
      for(unsigned int i = 0; i < lSeq_size; i++){
        lseq[i] = i;
      }
    }
    else if (m_data_type.compare(TYPE_FLOAT)==0){
      unsigned int fSeq_size = m_data_size / 4;
      CORBA::Float * fSeq_buffer = new CORBA::Float[fSeq_size];
      fseq = new Marshalling::FloatSeq(fSeq_size, fSeq_size, fSeq_buffer, 0);
      for(unsigned int i = 0; i < fSeq_size; i++){
        fseq[i] = i + 0.6;
      }
    }
    else if (m_data_type.compare(TYPE_ANY)==0){    
      char* txt = new char[m_data_size + 1];
      for (int i = 0; i< m_data_size; i++) {
        txt[i] = 'A';
      } 
      txt[m_data_size] = '\0';
      CORBA::String_var str = CORBA::string_dup(txt);
      any <<= str.in();
      delete txt;
    }
    else if (m_data_type.compare(TYPE_RECORD)==0){    
      unsigned int rSeq_size = m_data_size / 8;
      Marshalling::RecordStruct * rSeq_buffer = new Marshalling::RecordStruct[rSeq_size];
      rseq = new Marshalling::RecordSeq(rSeq_size, rSeq_size, rSeq_buffer, 0);
      for(unsigned int i = 0; i < rSeq_size; i++){
        rseq[i].l= i;
        rseq[i].f= i + 0.9;
      }
    }


    //result of time measurement    
    long double time1, time2;
    
    //test CR: call and return    
    if(m_invocation_type.compare(CALL_RETURN)==0){
      
      //select for data type
      if(m_data_type.compare(TYPE_LONG)==0){
        //start of record
        for(int i = 0; i < m_niter; i++){
          
          //delay of measure
          if(m_delay!=0){
            // call to nanosleep
            nanosleep(& t_delay, NULL);
          }
          time1 = m_measure->getTime();
          m_testping->long_ping(lseq.in());
          time2 = m_measure->getTime();
          tdelta[i] = time2 - time1;
        }//end for
      }//end type data: LONG
      else if(m_data_type.compare(TYPE_FLOAT)==0){
        //start of record
        for(int i = 0; i < m_niter; i++){
          
          //delay of measure
          if(m_delay!=0){
            // call to nanosleep
            nanosleep(& t_delay, NULL);
          }
          time1 = m_measure->getTime();
          m_testping->float_ping(fseq.in());
          time2 = m_measure->getTime();
          tdelta[i] = time2 - time1;

        }//end for
      } //end type data: FLOAT
      else if(m_data_type.compare(TYPE_ANY)==0){
        //start of record
        for(int i = 0; i < m_niter; i++){
          
          //delay of measure
          if(m_delay!=0){
            // call to nanosleep
            nanosleep(& t_delay, NULL);
          }
          time1 = m_measure->getTime();
          m_testping->any_ping(any.in());
          time2 = m_measure->getTime();
          tdelta[i] = time2 - time1;

        }//end for
      } //end type data: ANY
      else if(m_data_type.compare(TYPE_RECORD)==0){
        //start of record
        for(int i = 0; i < m_niter; i++){
          
          //delay of measure
          if(m_delay!=0){
            // call to nanosleep
            nanosleep(& t_delay, NULL);
          }
          time1 = m_measure->getTime();
          m_testping->record_ping(rseq.in());
          time2 = m_measure->getTime();
          tdelta[i] = time2 - time1;

        }//end for
      } //end type data: RECORD
      else if(m_data_type.compare(TYPE_NONE)==0){
        //start of Invocation test
        for(int i = 0; i < m_niter; i++){
          
          //delay of measure
          if(m_delay!=0){
            // call to nanosleep
            nanosleep(& t_delay, NULL);
          }
          time1 = m_measure->getTime();
          m_testping->invocation_ping();
          time2 = m_measure->getTime();
          tdelta[i] = time2 - time1;

        }//end for
      } //end type data: NONE
      
    } //end case Call and Return
    
    else {
      
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
      m_testping = Marshalling::TestPing::_narrow (obj.in ());
      
      //select for type of data
      if(m_data_type.compare(TYPE_LONG)==0){
        //start of record
        for(int i = 0; i < m_niter; i++){
          //delay of measure
          if(m_delay!=0){
            // call to nanosleep
            nanosleep(& t_delay, NULL);
          }
          time1 = m_measure->getTime();
          m_testping->ow_long_ping(lseq.in());
          time2 = m_measure->getTime();
          //time read in micro sec.     
          tdelta[i] = time2 - time1;

        }//end for
      }//end data type: long
      
      else if(m_data_type.compare(TYPE_FLOAT)==0){
        //start of record
        
        for(int i = 0; i < m_niter; i++){
          //delay of measure
          if(m_delay!=0){
            // call to nanosleep
            nanosleep(& t_delay, NULL);
          }
          time1 = m_measure->getTime();
          m_testping->ow_float_ping(fseq.in());
          time2 = m_measure->getTime();
          //time read in micro sec.     
          tdelta[i] = time2 - time1;

        }//end for
      }//end data type: float

      else if(m_data_type.compare(TYPE_ANY)==0){
        //start of record
        
        for(int i = 0; i < m_niter; i++){
          //delay of measure
          if(m_delay!=0){
            // call to nanosleep
            nanosleep(& t_delay, NULL);
          }
          time1 = m_measure->getTime();
          m_testping->ow_any_ping(any.in());
          time2 = m_measure->getTime();
          //time read in micro sec.     
          tdelta[i] = time2 - time1;

        }//end for
      }//end data type: any
      
      else if(m_data_type.compare(TYPE_RECORD)==0){
        //start of record
        
        for(int i = 0; i < m_niter; i++){
          //delay of measure
          if(m_delay!=0){
            // call to nanosleep
            nanosleep(& t_delay, NULL);
          }
          time1 = m_measure->getTime();
          m_testping->ow_record_ping(rseq.in());
          time2 = m_measure->getTime();
          //time read in micro sec.     
          tdelta[i] = time2 - time1;

        }//end for
      }//end data type: RECORD
       else if(m_data_type.compare(TYPE_NONE)==0){
        //start of Invocation test
        
        for(int i = 0; i < m_niter; i++){
          //delay of measure
          if(m_delay!=0){
            // call to nanosleep
            nanosleep(& t_delay, NULL);
          }
          time1 = m_measure->getTime();
          m_testping->ow_invocation_ping();
          time2 = m_measure->getTime();
          //time read in micro sec.     
          tdelta[i] = time2 - time1;

        }//end for
      }//end data type: NONE
     
    }

    //write file
    std::string filedeltaname (m_fileprefix + "DELTA_C.dat");
    ofstream fd (filedeltaname.c_str());
    fd.precision(6);
    for(int i = 0; i < m_niter; i++){
      fd<<tdelta[i]<< std::endl;
    }
    fd.close();
    
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
	//m_orb->shutdown(true);
    
  }

} // End namespace Marshalling

  
  
