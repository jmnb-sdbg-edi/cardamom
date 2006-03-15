/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "platformftsystem/PrimaryActivationHandler_impl.hpp"

#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"

#include "platformsystemlibrary/CentralisedSupervisionSystem_impl.hpp"

#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationControl_impl.hpp"
#include "platformapplicationlibrary/ProcessObserver_impl.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>

#include "FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp"

#include <iostream>

namespace Cdmw 
{
namespace PlatformMngt
{
    //    
    // Dumper of event notification from datastore
    // operator ()
    //    
    void EventNotifDumper::operator()(const PairType &elt)
    {
        // for the extracted record from datastore
        // store it in temporary list in synchro processing
        m_synchro_proc->add_event_notif ((elt.second).in());
    }    
    
    //    
    // Dumper of event report from datastore
    // operator ()
    //    
    void EventReportDumper::operator()(const PairType &elt)
    {
        // for the extracted record from datastore
        // store it in temporary list in synchro processing
        
        // make a deep copy (elt returns a var)
        CdmwPlatformMngtBase::EventKey_var event_key = elt.first;
        CdmwPlatformMngt::EventReportRecord_var event_report = elt.second;
        
        // and store in synchro processing
        m_synchro_proc->add_event_report (event_key._retn(),event_report._retn());
    }   
    
    

    SynchroProcessing::SynchroProcessing (CORBA::ORB_ptr orb,
                                          CdmwFTSystMngt::SystemAccessor_ptr system_accessor_ref,
                                          CentralisedSupervisionSystem_impl* system,
                                          EventNotifierDataStore* event_notif_ds,
                                          EventReportDataStore* event_report_ds,
                                          PrimaryActivationHandler_impl* handler)
            : m_system (system),
              m_event_notif_ds (event_notif_ds),
              m_event_report_ds (event_report_ds),
              m_handler (handler)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
        m_system_accessor_ref = CdmwFTSystMngt::SystemAccessor::_duplicate(system_accessor_ref);
    }


    SynchroProcessing::~SynchroProcessing()
            throw()
    {
    }

    void SynchroProcessing::add_event_notif(CdmwPlatformMngt::Event * event_notif)
    {
        // duplicate the reference
        CORBA::add_ref(event_notif);
        CdmwPlatformMngt::Event_var event_notif_var = event_notif;
        
        // store in the list
        m_event_notif_list.push_back (event_notif_var);
    }
    
    void SynchroProcessing::add_event_report(CdmwPlatformMngtBase::EventKey event_key,
                                             CdmwPlatformMngt::EventReportRecord * event_report)
    {
        m_event_key_list.push_back (event_key);
        m_event_report_list.push_back (event_report);
    }
    

    void SynchroProcessing::run()
            throw()
    {
        
        std::cout << "<<SynchroProcessing::run() starting>>" << std::endl; 
        
        // set synchronising in progress
        Configuration::Set_synchronising();
                            
        // set process as primary
        std::cout << "<<SynchroProcessing::run() !!set process as primary !!>>" << std::endl;
        Configuration::Set_primary();
           
        // if it is the first primary
        if (Configuration::Is_first_primary())
        { 
            // indicate to handler it can return to set primary in FT group
            m_handler->set_synchro_ready();
        }
        
        // else it is not the first primary (backup becomes primary)
        else
        {  
            // synchro the event notification
            std::cout << "<<SynchroProcessing::run() send event notifications>>" << std::endl;        
            m_event_notif_ds->for_each(EventNotifDumper(this));
            
            // get the system event notifier
            SupervisionEventNotifier* event_notifier = m_system->get_event_notifier();
            
            // execute the appropriate event notif
            EventNotifList::iterator event_notif_it;
            
            int i;
    
            // for each event of the list
            for (i=1, event_notif_it = m_event_notif_list.begin();
                 event_notif_it != m_event_notif_list.end();
                 i++, event_notif_it++)
            {
                try
                {    
                    std::cout << "<<SynchroProcessing::run() send notification " << i << ">>" << std::endl;
                                   
                    // notify the event (event reference is removed in notifier)                  
                    event_notifier->pushEvent ((*event_notif_it)._retn());
                }
                catch (...)
                {
                    // nothing to do
                }
            }
            
            // clear the list of notifications
            m_event_notif_list.clear();
                


            // get the event reports stored in datastore
            std::cout << "<<SynchroProcessing::run() get event reports>>" << std::endl;
            m_event_report_ds->for_each(EventReportDumper(this));

            
            // freeze all event handlers to not postpone the received event
            std::cout << "<<SynchroProcessing::run() freeze all event handlers>>" << std::endl;
            m_system->freeze_event_handlers();                     
             


            // indicate to handler it can return to set primary in FT group
            m_handler->set_synchro_ready();
            
            
            // wait objects are set to primary            
            CdmwFT::Location::PrimaryBackupGroupRepository_var ft_group_repository;
                        
            try
            {
                CORBA::Object_var obj = m_orb->resolve_initial_references("FTGroupRepository");

                ft_group_repository = 
                     CdmwFT::Location::PrimaryBackupGroupRepository::_narrow(obj.in());
            }
            catch (const PortableInterceptor::ORBInitInfo::InvalidName& )
            {
                std::cerr << "<<SynchroProcessing::run() Impossible to resolve initial reference : InvalidName>>" 
                          << std::endl;
                CDMW_ASSERT(false);
            }
            catch (const CORBA::SystemException& ex)
            {
                std::cerr << "<<SynchroProcessing::run() Impossible to resolve initial reference : SystemException>>" 
                          << ex << std::endl;
                CDMW_ASSERT(false);
            }
            
            try
            {
                bool is_primary = false;
                
                while (!is_primary)
                {
                    // wait the setting of primary
                    OsSupport::OS::sleep(20);
                    
                    is_primary = ft_group_repository->is_primary(m_system_accessor_ref.in());                                
                }
            }
            catch (const ::FT::MemberNotFound &)
            {
                std::cerr << "<<SynchroProcessing::run() Member not Found exception raised while checking the primary>>" 
                          << std::endl;
                CDMW_ASSERT(false);
            }
            catch (const CORBA::SystemException& ex)
            {
                std::cerr << "<<SynchroProcessing::run() System exception raised while checking the primary>>" 
                          << ex << std::endl;
                CDMW_ASSERT(false);
            }

            // now the FT objects are set to primary
            // FT request will be received and processed
            // and coordinator of datastore is set
            
                    
            // synchronise the processing of system processes
            std::cout << "<<SynchroProcessing::run() processes sync>>" << std::endl;
                        
            try
            {
                m_system->synchro_processing_of_processes();
            }
            catch (...)
            {
                std::cout << "<<SynchroProcessing::run() processes sync exception>>" << std::endl;
            }
                
            // synchronise the processing of the applications
            std::cout << "<<SynchroProcessing::run() applications sync>>" << std::endl;
            
            try
            {
                m_system->synchro_processing_of_applications();  
            }
            catch (...)
            {
                std::cout << "<<SynchroProcessing::run() applications sync exception>>" << std::endl;
            }
            
            // synchronise the system processing
            std::cout << "<<SynchroProcessing::run() system sync>>" << std::endl;
            
            try
            {
                m_system->synchro_processing();
            }
            catch (...)
            {
                std::cout << "<<SynchroProcessing::run() system sync exception>>" << std::endl;
            }
            
            // synchronise the event reports
            std::cout << "<<SynchroProcessing::run() send event reports>>" << std::endl;

            // execute the appropriate event report got from datastore
            // and registered in the list
            EventReportList::iterator event_report_it;
    
            // for each event of the list
            for (i=1, event_report_it = m_event_report_list.begin();
                 event_report_it != m_event_report_list.end();
                 i++, event_report_it++)
            {
                std::cout << "<<SynchroProcessing::run() send report " << i << ">>" << std::endl;
                
                CdmwPlatformMngt::EventReportRecord * event_report = *event_report_it;
                    
                // take owner ship of the data structure                                              
                CdmwPlatformMngt::EventReportRecord_var event_report_var = event_report;
                    
                try
                {                                            
                    if (strcmp(event_report->application_name.in(),"") != 0)
                    {     
                        Application_impl* application;
                        ApplicationControl_impl* app_control;
                  
                        // get the appropriate application control
                        m_system->getApplicationServants (event_report->application_name.in(),
                                                          application, app_control);
            
                        // if application control event type
                        if (event_report->event_type == CdmwPlatformMngt::INVALIDATE_PROCESS_EVENT)
                        {
                            std::cout << "<<SynchroProcessing::run() ApplicationControl synchro_event calling: invalidate process event>>" << std::endl;
                            
                            // execute the event report                  
                            app_control->synchro_event (*event_report);
                        }
                
                        // else it is a process event type
                        else
                        {
                            // get the process observer of the application
                            ProcessObserver_impl* process_observer =
                            app_control->get_process_observer_servant();
                   
                            std::cout << "<<SynchroProcessing::run() ProcessObserver synchro_event calling>>" << std::endl;
                            
                            // execute the event report                  
                            process_observer->synchro_event (*event_report);  
                        }
                    }  
                    
                    else
                    {
                        std::cout << "<<SynchroProcessing::run() send report not processed>>" << std::endl;
                    }
                                      
                }
                catch (...)
                {
                    // nothing to do
                }
            }
            
            // clear the list
            m_event_report_list.clear();
            
            
            // delete event records in datastore
            // stored by previous primary
            EventKeyList::iterator event_key_it;
    
            // for each key of the list
            for (event_key_it = m_event_key_list.begin();
                 event_key_it != m_event_key_list.end();
                 event_key_it++)
            {
                // remove event from datastore
                try
                {
                    m_event_report_ds->remove(*event_key_it);
                }
                catch (Common::NotFoundException&)
                {
                    // nothing to do
                }
            }
            
            // clear the list
            m_event_key_list.clear(); 
            
            // unfreeze all event handlers to process received event
            // during synchronisation
            
            std::cout << "<<SynchroProcessing::run() unfreeze all event handlers>>" << std::endl;
            m_system->unfreeze_event_handlers();                                            
        }
        
        // set synchronising done
        Configuration::Set_synchronised();
        
        std::cout << "<<SynchroProcessing::run() done>>" << std::endl;
    }
        


    PrimaryActivationHandler_impl::PrimaryActivationHandler_impl(
             CORBA::ORB_ptr orb,
             CdmwFTSystMngt::SystemAccessor_ptr system_accessor_ref,
             CentralisedSupervisionSystem_impl* system)
        : m_system (system),
          m_sync_ready_condition(m_sync_ready_mutex)
    {
        m_synchro_processing = NULL;
        
        try
        {
            // get the event notification datastore interface
            Cdmw::CommonSvcs::DataStore::DataStoreBase* ds =
            Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(
                   Configuration::Get_event_notifier_datastore_id());
            m_event_notif_ds = 
                 dynamic_cast<Cdmw::PlatformMngt::EventNotifierDataStore*>(ds);
                 
            // get the event report datastore interface
            Cdmw::CommonSvcs::DataStore::DataStoreBase* ds2 =
                Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(
                   Configuration::Get_event_report_datastore_id());
            m_event_report_ds = 
                 dynamic_cast<Cdmw::PlatformMngt::EventReportDataStore*>(ds2);
            
            // create the synchro processing class    
            m_synchro_processing = new SynchroProcessing (
                        orb, system_accessor_ref, m_system, 
                        m_event_notif_ds, m_event_report_ds, this);
        }
        catch (const NotExistException& e)
        {
            CDMW_ASSERT(false);
        }
    }
    
    PrimaryActivationHandler_impl::~PrimaryActivationHandler_impl()
    {
        if (m_synchro_processing != NULL)
        {
            delete m_synchro_processing;
        }
    }
    
    //
    // IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/activate:1.0
    //
    void PrimaryActivationHandler_impl::activate()
        throw(CORBA::SystemException)
    {
        // The server will be set as primary by the FT layer just after
        // the return of this handler, then all FT requests received
        // by this new primary will be accepted
        
        std::cout << "<<PrimaryActivationHandler_impl::activate()>>" << std::endl;       
        
        // synchronise the system with its datastore
        m_system->synchro_with_datastore();
        
        // synchronise the applications and processes with their datastore
        m_system->synchro_applications_with_datastore();
        
        // lock the mutex of ready flag
        CDMW_MUTEX_GUARD(m_sync_ready_mutex);
        
        // reset the ready flag
        m_sync_ready = false;
        
        // start the synchro processing thread
        m_synchro_processing->start();
                
        /*****
        // wait the termination of thread
        m_synchro_processing->join();
        *****/
        
        // wait the ready flag
        while (!m_sync_ready)
        {
            m_sync_ready_condition.wait();
        }
        
        // now the FT group can be updated to receive the FT request
        
        std::cout << "<<PrimaryActivationHandler_impl::activate() ending>>" << std::endl;

        Cdmw::OsSupport::OS::Timeval time = Cdmw::OsSupport::OS::get_time();
        std::cout << "########## Supervision is active. Date: " 
                  << time.seconds << "sec " << time.microseconds << "microsec" 
                  << std::endl;
    }
    

    //
    // IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/deactivate:1.0
    //
    void PrimaryActivationHandler_impl::deactivate()
        throw(CORBA::SystemException)
    {
        std::cout<<"PrimaryActivationHandler_impl::deactivate()"<<std::endl;
       
        // set process as backup
        Configuration::Set_backup();
        
    }
    
    //
    // set synchro ready to start
    //
    void PrimaryActivationHandler_impl::set_synchro_ready()
    {
        // first lock the mutex of ready flag
        CDMW_MUTEX_GUARD(m_sync_ready_mutex);
        
        // set the ready flag
        m_sync_ready = true;
        
        // broadcast the condition
        m_sync_ready_condition.broadcast();
    }
    


} // End of namespace PlatformMngt
} // End of namespace Cdmw

