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


#include "platformfaultdetector/LocalFaultDetector_impl.hpp"
#include "SystemMngt/platformlibrary/MacroDefs.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/ProcessKillMngr.hpp"
#include "platformfaultdetector/TimeOutPolicyServiceFactory.hpp"
#include "platformfaultdetector/TimeoutObserver.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include <sstream>

namespace Cdmw
{
    namespace PlatformMngt
    {
        bool ActivePerfTrace = false;
    
        void active_perf_trace(int sig)
        {
            if (ActivePerfTrace == true)
                ActivePerfTrace = false;
            else
                ActivePerfTrace = true;
        };


        /**
         * Purpose:
         * <p>
         * Implementation of a single shot timer timeout observer.
         */
        class FaultDetectorTimerObserver : public TimeoutObserver
        {
        public:

            FaultDetectorTimerObserver(LocalFaultDetector_impl* faultDetector)
                : m_faultDetector(faultDetector)
            {
            }

            virtual ~FaultDetectorTimerObserver() { }

            void wake_up() throw()
            {
                m_faultDetector->wakeUp();
            }

        private:
            LocalFaultDetector_impl *m_faultDetector;

        };


        /**
         * Purpose:
         * <p>
         * Implementation of the callback model AMI handler.
         */
        class AMIProcessPullMonitorableHandler_impl : public POA_FT::AMI_PullMonitorableHandler,
                                                                     virtual public PortableServer::RefCountServantBase
        {
        public:

            AMIProcessPullMonitorableHandler_impl(CORBA::ORB_ptr orb, 
                                                  LocalFaultDetector_impl* faultDetector)
                : m_faultDetector(faultDetector),
                  m_orb(CORBA::ORB::_duplicate(orb))
            {
                if (CORBA::is_nil(orb))
                    CDMW_THROW2(BadParameterException, "orb", "nil");
                m_current =  PortableServer::Current::_nil();
            }

            virtual ~AMIProcessPullMonitorableHandler_impl() { }

            /**
             * Purpose:
             * <p>
             * Processes the results when the is_alive method
             * is invoked in the server side.
             */
            void is_alive (CORBA::Boolean ami_return_val)
                throw (CORBA::SystemException)
            {
                // Defines the monitoring id of the acknowledging PullMonitorable object
                std::string oid = getCurrentName();
                CdmwPlatformMngt::MonitoringId id = atoi(oid.c_str());

                // Delegates the result processing to the fault detector
                m_faultDetector->is_alive_notification (ami_return_val, id);
            }

            /**
             * Purpose:
             * <p>
             * Processes exceptions raised in the server side when the is_alive method
             * is invoked.
             */
            void is_alive_excep (::FT::AMI_PullMonitorableExceptionHolder * excep_holder)
                throw (CORBA::SystemException)
            {
                // Defines the monitoring id of the exception raising PullMonitorable object
                std::string oid = getCurrentName();
                CdmwPlatformMngt::MonitoringId id = atoi(oid.c_str());

                // Delegates the exception processing to the fault detector
                m_faultDetector->is_alive_excep_notification (excep_holder, id);
            }
 

            /**
             * Purpose:
             * <p> Returns the name embedded in the ObjectId of the CORBA object
             * in whose context it is called.
             */ 
            std::string getCurrentName()
                throw (OutOfMemoryException, InternalErrorException)
            {
                try
                {

                    // gets the POA Current reference
                   if (CORBA::is_nil(m_current.in()))
                    {
                        CORBA::Object_var current = m_orb->resolve_initial_references("POACurrent");
                        m_current = PortableServer::Current::_narrow(current.in());
                    }

                    PortableServer::ObjectId_var oid = m_current->get_object_id();
                    CORBA::String_var oid_str = PortableServer::ObjectId_to_string(oid.in());

                    std::string currentName(oid_str.in());

                    return currentName;

                }
                catch(const PortableServer::Current::NoContext&)
                {
                    CDMW_THROW(InternalErrorException);
                }
                catch(const std::bad_alloc&)
                {
                    CDMW_THROW(OutOfMemoryException);
                }
            }


        private:
            LocalFaultDetector_impl *m_faultDetector;
            /** 
             * the Orb
             */
            CORBA::ORB_var m_orb;
            
            /**
             * The PortableServer::Current.
             */
            PortableServer::Current_var m_current;
        };


        /**
         * Purpose:
         * <p>
         * Calculates a delay.
         */
        int getDelay(Cdmw::OsSupport::OS::Timeval time)
        {
            Cdmw::OsSupport::OS::Timeval timenow;
	
            timenow = Cdmw::OsSupport::OS::get_time();

            //Return a delay in ms
            return ((timenow.seconds - time.seconds)*(int)1E6 + timenow.microseconds - time.microseconds)/1000;
        }


        /**
         * Purpose:
         * <p>
         * Constructor.
         */
        LocalFaultDetector_impl::LocalFaultDetector_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
            : m_orb(CORBA::ORB::_duplicate(orb)),
              m_poa(PortableServer::POA::_duplicate(poa))
        {
            static struct sigaction action;
            action.sa_handler=active_perf_trace;
            sigemptyset(&action.sa_mask);
            sigaction(SIGUSR1, &action, NULL);
    
    
            if (CORBA::is_nil(orb))
                CDMW_THROW2(BadParameterException, "orb", "nil");

            m_timeoutPolicyService = TimeOutPolicyServiceFactory::getService();

            m_localHostname = Cdmw::OsSupport::OS::get_hostname();

            PortableServer::POAManager_var manager = m_poa->the_POAManager();
            
            CORBA::PolicyList policies;
            OrbSupport::StrategyList poaStrategyList;
            
            policies.length(5);
            policies[0] = m_poa->create_id_uniqueness_policy(PortableServer::MULTIPLE_ID);
            policies[1] = m_poa->create_lifespan_policy(PortableServer::TRANSIENT);
            policies[2] = m_poa->create_servant_retention_policy(PortableServer::RETAIN);
            policies[3] = m_poa->create_id_assignment_policy(PortableServer::USER_ID);
            policies[4] = m_poa->create_request_processing_policy(PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY);
            
            poaStrategyList.add_PoaThreadPerConnection();
            std::string amiHandlerPoaName = "AMIProcessPullMonitorableHandler";


            m_poaHandler = OrbSupport::OrbSupport::create_POA(m_poa.in(), amiHandlerPoaName.c_str(),
                                                              manager.in(), policies, poaStrategyList);

            // Instanciates the callback model AMI handler
            m_pHandler_impl = new AMIProcessPullMonitorableHandler_impl(m_orb.in(), this);

            m_monitoringId = 0;
            m_timerInterval = TIMER_INTERVAL_MAX;    

            // Prepares the single shot timer
            m_timerStarted = false;
            m_pTimerObserver = new FaultDetectorTimerObserver(this);
            m_pTimer = new SingleShotTimer(m_pTimerObserver, m_timerInterval);
            m_pTimer->start();
        }


        /**
         * Purpose:
         * <p>
         * Destructor.
         */
        LocalFaultDetector_impl::~LocalFaultDetector_impl()
            throw()
        {
            delete m_pTimerObserver;
            m_pTimerObserver = NULL;
            delete m_pTimer;
            m_pTimer = NULL;
            delete m_pHandler_impl;
            m_pHandler_impl = NULL;
            delete m_timeoutPolicyService;
            m_timeoutPolicyService = NULL;
        }


//         /**
//          * Purpose:
//          * <p>
//          * Activates this LocalFaultDetector servant and returns the associated CORBA reference.
//          */
//         CdmwPlatformMngt::LocalFaultDetector_ptr LocalFaultDetector_impl::activate()
//         {
//             std::string detectorName = "CdmwLocalFaultDetector";

//             PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(detectorName.c_str());
    
//             m_poa->activate_object_with_id(oid.in(), this);

//             return _this();
//         }


//         /**
//          * Purpose:
//          * <p>
//          * Deactivates this LocalFaultDetector servant.
//          */
//         void LocalFaultDetector_impl::deactivate()
//         {
//             std::string detectorName = "CdmwLocalFaultDetector";

//             PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(detectorName.c_str());

//             m_poa->deactivate_object(oid.in());
//         }


        /**
         * Purpose:
         * <p>
         * implements the
         * IDL:thalesgroup.com/CdmwPlatformMngt/LocalFaultDetector/start_monitoring:1.0
         * operation
         */
        CdmwPlatformMngt::MonitoringId LocalFaultDetector_impl::start_monitoring (
                                                                                  ::FT::PullMonitorable_ptr monitorable_object,
                                                                                  const ::FT::Location & location,
                                                                                  const ::FT::FaultMonitoringIntervalAndTimeoutValue & timeout)
            throw (CORBA::SystemException,
                   ::FT::ObjectNotFound,
                   ::CdmwPlatformMngt::BadLocation)
        {
            CDMW_MUTEX_GUARD(m_mutex);
            try
            {
                
            // Checks the location validity
            if (m_localHostname != std::string(location[0].id))
            {
                throw CdmwPlatformMngt::BadLocation();
            }

            // Checks the existence of a PullMonitorable object
            if (!has_monitorable(location))
            {
                // Checks the consistency of time values
                if (timeout.timeout >= timeout.monitoring_interval)
                {
                    throw  CORBA::BAD_PARAM(OrbSupport::BAD_PARAMIncompatibleMonitoringTimeout, CORBA::COMPLETED_NO); 
                    //throw CdmwPlatformMngt::IncompatibleMonitoringTimeout();
                }
                else if (timeout.monitoring_interval < m_timerInterval)
                {
                    // Recordes a new value for the timer interval
                    m_timerInterval = timeout.monitoring_interval;

                    MonitorableMap::iterator it;

                    // Calculates for each entry the number of timer ticks before interrogating
                    // the associated PullMonitorable CORBA object
                    for (it = m_monitorableMap.begin(); it != m_monitorableMap.end(); it++)
                    {
                        it->second.countMax = (it->second.timeout.monitoring_interval)/m_timerInterval;
                    }

                    // Checks the validity of the current monitoring interval against
                    // the recorded timeouts
                    for (it = m_monitorableMap.begin(); it != m_monitorableMap.end(); it++)
                    {
                        if (it->second.timeout.timeout >= (timeout.monitoring_interval * it->second.countMax))
                        {
                            throw  CORBA::BAD_PARAM(OrbSupport::BAD_PARAMIncompatibleMonitoringInterval, CORBA::COMPLETED_NO); 
                            //throw CdmwPlatformMngt::IncompatibleMonitoringInterval();
                        }
                    }
                }

                MonitorableInfo info;

                m_monitoringId += 1;

                std::stringstream buffer;
                buffer << m_monitoringId << std::endl;

                // Sets the new PullMonitorable CORBA object information
                info.countMax = timeout.monitoring_interval/m_timerInterval;
                info.currentCount = 0;
                info.state = INIT;
                info.location = location;
                info.timeout = timeout;

                // Set the timeout policy for the current PullMonitorable CORBA object
                CORBA::Object_var capsule = m_timeoutPolicyService->SetTimeoutPolicy
                (m_orb.in(), monitorable_object, timeout.timeout);

                info.pullMonitorable = FT::PullMonitorable::_narrow(capsule.in());

                PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(buffer.str().c_str());
                
                m_poaHandler->activate_object_with_id(oid.in(), m_pHandler_impl);

                CORBA::Object_var object = m_poaHandler->id_to_reference(oid.in());

                FT::AMI_PullMonitorableHandler_var handler = FT::AMI_PullMonitorableHandler::_narrow(object.in());

                info.handler = FT::AMI_PullMonitorableHandler::_duplicate(handler.in());

                // Checks the new PullMonitorable CORBA object reference validity
                if (CORBA::is_nil(info.pullMonitorable.in()))
                {
                    m_monitoringId -= 1;
                    throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);
                }

                // Undertakes the first interrogation of the new PullMonitorable CORBA object
                try
                {
                    info.pullMonitorable->sendc_is_alive(info.handler.in());
                }
                catch (CORBA::SystemException& e)
                {
                    // std::cerr << "LocalFaultDetector_impl push_structured_fault calling exception : " 
                    //           << e << std::endl;
                }
                catch (...)
                {
                    // nothing to do
                } 

                // Starts the single shot timer if not already done
                if (!m_timerStarted)
                {
                    m_timerStarted = true;
                    m_pTimer->setup(m_timerInterval);
                    m_pTimer->start_timer();
                }

                // Recordes the new PullMonitorable information
                add_monitorable(m_monitoringId, info);
            } 
            else 
            {
                //throw CdmwPlatformMngt::MonitoringAlreadyStarted();
            }

            return m_monitoringId;
            }
            catch(const CORBA::Exception& ex)
            {
                std::cout<<"LocalFaultDetector_impl::start_monitoring"<<ex<<std::endl;
                throw;
            }
            
        }


        /**
         * Purpose:
         * <p>
         * implements the
         * IDL:thalesgroup.com/CdmwPlatformMngt/LocalFaultDetector/stop_monitoring:1.0
         * operation
         */
        void LocalFaultDetector_impl::stop_monitoring (
                                                       ::CdmwPlatformMngt::MonitoringId monitoring_id)
            throw (CORBA::SystemException,
                   ::CdmwPlatformMngt::InvalidMonitoringId)
        {
            CDMW_MUTEX_GUARD(m_mutex);

            // Checks the existence of the PullMonitorable object
            if (has_monitorable(monitoring_id))
            {
                MonitorableInfo info = remove_monitorable(monitoring_id);

                if (m_monitorableMap.size() == 0) 
                {
                    m_pTimer->cancel_timer();
                    m_timerInterval = TIMER_INTERVAL_MAX;
                    m_timerStarted = false;
                }
            } 
            else 
            {
                throw CdmwPlatformMngt::InvalidMonitoringId();
            }
        }


        /**
         * Purpose:
         * <p>
         * implements the
         * IDL:thalesgroup.com/CdmwPlatformMngt/LocalFaultDetector/set_timer_interval:1.0
         * operation
         */
        void LocalFaultDetector_impl::set_timer_interval (::TimeBase::TimeT monitoring_interval)
            throw (CORBA::SystemException)
        {
            CDMW_MUTEX_GUARD(m_mutex);

            MonitorableMap::iterator it;

            // Checks the validity of the current monitoring interval against
            // the recorded timeouts and intervals
            for (it = m_monitorableMap.begin(); it != m_monitorableMap.end(); it++)
            {
                if ((it->second.timeout.monitoring_interval < monitoring_interval) ||
                    (it->second.timeout.timeout >= monitoring_interval))
                {
                    throw  CORBA::BAD_PARAM(OrbSupport::BAD_PARAMIncompatibleMonitoringInterval, CORBA::COMPLETED_NO); 
                    //throw CdmwPlatformMngt::IncompatibleMonitoringInterval();
                }
            }

            m_timerInterval = monitoring_interval;

            // Calculates for each entry the number of timer ticks before interrogating
            // the associated PullMonitorable CORBA object
            for (it = m_monitorableMap.begin(); it != m_monitorableMap.end(); it++)
            {
                it->second.countMax = (it->second.timeout.monitoring_interval)/m_timerInterval;
            }
        }

#ifdef CDMW_USE_FAULTTOLERANCE
        /**
         * Purpose:
         * <p>
         * implements the
         * set_fault_notifier
         * operation
         */
        void LocalFaultDetector_impl::set_fault_notifier (
                                                          ::FT::FaultNotifier_ptr fault_notifier)
            throw (CORBA::SystemException)
        {
            m_faultNotifier = ::FT::FaultNotifier::_duplicate(fault_notifier);
        }
#else
        /**
         * Purpose:
         * <p>
         * implements the
         * set_process_observer
         * operation
         */
        void LocalFaultDetector_impl::set_process_observer (
                                                            CdmwPlatformMngt::ProcessObserver_ptr process_observer)
            throw (CORBA::SystemException)
        {
            m_processObserver = CdmwPlatformMngt::ProcessObserver::_duplicate(process_observer);
        }
#endif

        /**
         * Purpose:
         * <p>
         * implements the
         * IDL:omg.org/FT/is_alive:1.0
         * operation
         */
        CORBA::Boolean LocalFaultDetector_impl::is_alive ()
            throw (CORBA::SystemException)
        {
            return true;
        }


        /**
         * Purpose:
         * <p>
         * Processes timer awakenings.
         */
        void LocalFaultDetector_impl::wakeUp()
        {
            CDMW_MUTEX_GUARD(m_mutex);
    
            // Gets the current time before the processing
            // That time will be used to calculate the delay before setting again the single shot timer
            // That delay will be taken away from the next interval to avoid any drift
            Cdmw::OsSupport::OS::Timeval timenow;
            timenow = Cdmw::OsSupport::OS::get_time();
            MonitorableMap::iterator it;
    
            // Checks all the PullMonitorable CORBA objects liveliness
            for (it = m_monitorableMap.begin(); it != m_monitorableMap.end(); it++)
            {

                // Checks if an old timer timeout has expired in the wake of the current PullMonitorable
                // CORBA object registration, the CORBA object has not yet acknowledged an
                // interrogation but a whole timer timeout has not passed
                if (it->second.state == INIT) it->second.state = ACK_WAITING;
                else it->second.currentCount += 1;

                // Checks if the current PullMonitorable CORBA object has acknowledged
                // the previous interrogation
                if (it->second.state == ACK_OK)
                {
                    // Checks if the current PullMonitorable object must be interrogated
                    if (it->second.currentCount >= it->second.countMax)
                    {
                        it->second.state = ACK_WAITING;
	         
                        //std::cout << "pullMonitorable->sendc_is_alive : " << it->first << std::endl;
             
                        try
                        {
                            it->second.pullMonitorable->sendc_is_alive(it->second.handler.in());
                        }
                        catch (CORBA::SystemException& e)
                        {
                            // std::cerr << "LocalFaultDetector_impl push_structured_fault calling exception : " 
                            //           << e << std::endl;
                        }
                        catch (...)
                        {
                            // nothing to do
                        } 
                    }
                }
                else if (it->second.state == ACK_WAITING && 
                         it->second.currentCount >= it->second.countMax)
                {
                    notify_failure(it->second.location);
                    
                    std::cout << "LocalFaultDetector_impl::wakeUp : " 
                              << it->first << " has not answered" << std::endl;
                    
                    // kill the process having faulty monitoring
                    // if failure of process started by daemon
                    if (std::string(it->second.location[1].kind) == "processname")
                    {             
                        ProcessKillMngr::Instance().kill_process ("", it->second.location[1].id);
                    }
                    // else it a failure of process started by agent
                    else
                    {             
                        ProcessKillMngr::Instance().kill_process (
                                                                  it->second.location[1].id, 
                                                                  it->second.location[2].id);
                    }
            
                    remove_monitorable(it->first);
                }
            }

            if (m_monitorableMap.size() != 0)
            {
                // Restarts the timer and takes into account the current method processing delay
                m_pTimer->setup(m_timerInterval - getDelay(timenow));
                m_pTimer->start_timer();
            }

            if (ActivePerfTrace)
            {
                Cdmw::OsSupport::OS::Timeval timeend;
                timeend = Cdmw::OsSupport::OS::get_time();
                std::cout<<"Elapsed time in function LocalFaultDetector_impl::wakeUp(): "<<((timeend.seconds*1000000)+ timeend.microseconds) - ((timenow.seconds*1000000) + timenow.microseconds)<<" microseconds"<<std::endl;
            }
    

        }


        /**
         * Purpose:
         * <p>
         * Processes is_alive invocation reply.
         */
        void LocalFaultDetector_impl::is_alive_notification (CORBA::Boolean ami_return_val,
                                               ::CdmwPlatformMngt::MonitoringId id)
            throw (CORBA::SystemException)
        {
            CDMW_MUTEX_GUARD(m_mutex);

            if (has_monitorable(id))
            {
                // get monitorable info from map
                MonitorableInfo& monitorable_info = m_monitorableMap[id];
    
                // Checks the acknowledging PullMonitorable CORBA object processing status
                if (ami_return_val)
                {
                    // The acknowledging PullMonitorable CORBA object has notified 
                    // a normal processing

                    // Resets the tick counter and records that the PullMonitorable CORBA object
                    // has acknowledged
                    monitorable_info.currentCount = 0;
                    monitorable_info.state = ACK_OK;

                }
                else
                {
                    // The acknowledging PullMonitorable CORBA object has notified 
                    // a crippled processing

                    notify_failure(monitorable_info.location);
                    
                    // kill the process having faulty monitoring
                    // if failure of process started by daemon
                    if (std::string(monitorable_info.location[1].kind) == "processname")
                    {        
                        std::cout << "LocalFaultDetector_impl::is_alive_notification : " 
                                  << monitorable_info.location[1].id
                                  << " processing status false" << std::endl;
	        
                        ProcessKillMngr::Instance().kill_process ("", monitorable_info.location[1].id);
                    }
                    // else it a failure of process started by agent
                    else
                    {    
                        std::cout << "LocalFaultDetector_impl::is_alive_notification : " 
                                  << monitorable_info.location[1].id 
                                  << "/" << monitorable_info.location[2].id 
                                  << " processing status false" << std::endl;
	               
                        ProcessKillMngr::Instance().kill_process (
                                                                  monitorable_info.location[1].id, 
                                                                  monitorable_info.location[2].id);
                    }
	                    
                    remove_monitorable(id);

                    if (m_monitorableMap.size() == 0) 
                    {
                        m_pTimer->cancel_timer();
                        m_timerInterval = TIMER_INTERVAL_MAX;
                        m_timerStarted = false;
                    }
                }
            }
        }


        /**
         * Purpose:
         * <p>
         * Processes is_alive invocation exception.
         */
        void LocalFaultDetector_impl::is_alive_excep_notification (
                                                    ::FT::AMI_PullMonitorableExceptionHolder * excep_holder,
                                                    ::CdmwPlatformMngt::MonitoringId monitoring_id)
            throw (CORBA::SystemException)
        {   
            CDMW_MUTEX_GUARD(m_mutex);

            try
            {
                excep_holder->raise_is_alive();
            }
            catch (const CORBA::Exception& e)
            {
                std::cout << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) << std::endl;     
            }
            catch (...)
            {
                std::cout << "Unknown non-CORBA exception" << std::endl;
            }

            if (has_monitorable(monitoring_id))
            {
                // get monitorable info from map
                MonitorableInfo& monitorable_info = m_monitorableMap[monitoring_id];
    
                notify_failure(monitorable_info.location);

                // if failure of process started by daemon
                if (std::string(monitorable_info.location[1].kind) == "processname")
                {        
                    std::cout << "LocalFaultDetector_impl::is_alive_notification : " 
                              << monitorable_info.location[1].id
                              << " is alive exception" << std::endl;
	        
                    ProcessKillMngr::Instance().kill_process ("", monitorable_info.location[1].id);
                }
                // else it a failure of process started by agent
                else
                {    
                    std::cout << "LocalFaultDetector_impl::is_alive_notification : " 
                              << monitorable_info.location[1].id 
                              << "/" << monitorable_info.location[2].id 
                              << " is alive exception" << std::endl;
	               
                    ProcessKillMngr::Instance().kill_process (
                                                              monitorable_info.location[1].id, 
                                                              monitorable_info.location[2].id);
                }
    
                remove_monitorable(monitoring_id);

                if (m_monitorableMap.size() == 0) 
                {
                    m_pTimer->cancel_timer();
                    m_timerInterval = TIMER_INTERVAL_MAX;
                    m_timerStarted = false;
                }
            }
        }


        /**
         * Purpose:
         * <p> Indicates whether the detector manages an associated monitorable.
         */    
        bool LocalFaultDetector_impl::has_monitorable(CdmwPlatformMngt::MonitoringId monitoring_id)
        {
            bool ret = false;

            if (m_monitorableMap.count(monitoring_id) != 0) ret = true;

            return ret;
        }


        /**
         * Purpose:
         * <p> Indicates whether the detector manages an associated monitorable.
         */    
        bool LocalFaultDetector_impl::has_monitorable(::FT::Location location)
        {
            bool ret = false;

            MonitorableMap::iterator it;

            // Checks all the entries until it finds one that matches
            for (it = m_monitorableMap.begin(); it != m_monitorableMap.end(); it++)
            {
                if (location.length() == it->second.location.length())
                {
                    std::string str1 = "";
                    std::string str2 = "";

                    // Stringifies the locations for comparison
                    for (CORBA::ULong i=0; i<location.length(); i++)
                    {
                        str1 = str1 + std::string(it->second.location[i].id);
                        str1 = str1 + std::string(it->second.location[i].kind);

                        str2 = str2 + std::string(location[i].id);
                        str2 = str2 + std::string(location[i].kind);
                    }

                    // Compares the locations
                    std::cout<<str1<<" "<<str2<<std::endl;
                    if (str1 == str2)
                    {
                        ret = true;
                        break; 
                    }
                }
            }

            return ret;
        }

        /**
         * Purpose:
         * <p> Stores the monitorableInfo.
         */
        void LocalFaultDetector_impl::add_monitorable(CdmwPlatformMngt::MonitoringId monitoring_id,
                                                     MonitorableInfo info)
        {
            m_monitorableMap[monitoring_id] = info;
        }


        /**
         * Purpose:
         * <p> Removes the monitorableInfo.
         */
        LocalFaultDetector_impl::MonitorableInfo LocalFaultDetector_impl::remove_monitorable(CdmwPlatformMngt::MonitoringId monitoring_id)
        {

            MonitorableInfo info;

            MonitorableMap::iterator it = m_monitorableMap.find(monitoring_id);
    
            if (it != m_monitorableMap.end())
            {
                //std::cout << "LocalFaultDetector_impl::remove_monitorable : " << monitoring_id << std::endl;
                info = it->second;
                m_monitorableMap.erase(monitoring_id);
            }

            return info;
        }

        /**
         * Purpose:
         * <p> Notification of the failure of the process to the observer
         */ 
        void  
        LocalFaultDetector_impl::notify_failure(::FT::Location location)
        {
            // The current PullMonitorable CORBA object is considered failed
            
#ifdef CDMW_USE_FAULTTOLERANCE
         
            if (!CORBA::is_nil(m_faultNotifier.in()))
            {
                // Prepare the event for the fault notification
                CosNotification::StructuredEvent fault_event;
                fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup ("FT_CORBA");
                fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup ("ObjectCrashFault");
                fault_event.filterable_data.length (3);
                fault_event.filterable_data[0].name = CORBA::string_dup ("FTDomainId");
                fault_event.filterable_data[0].value <<= "thalesgroup.com";
                fault_event.filterable_data[1].name = CORBA::string_dup ("Location");
                fault_event.filterable_data[1].value <<= location;
                CdmwPlatformMngtBase::TimeStamp time_stamp =
                PlatformMngt::EventHeaderFactory::createTimeStamp();
                fault_event.filterable_data[2].name = CORBA::string_dup( "TimeStamp" );
                fault_event.filterable_data[2].value <<= time_stamp;                
                
                try
                {
                    m_faultNotifier->push_structured_fault(fault_event); 
                        }
                catch (CORBA::SystemException& e)
                {
                    // std::cerr << "LocalFaultDetector_impl push_structured_fault calling exception : " 
                    //           << e << std::endl;
                }
                catch (...)
                {
                    // nothing to do
                }               
            }
#else
//ADDED BY SELEX
    MonitorableMap::iterator it ; //= m_monitorableMap.find(monitoring_id);
    
    assert(false);
    if (it != m_monitorableMap.end())
    {
        //std::cout << "LocalFaultDetector_impl::removeMonitorable : " << monitoring_id << std::endl;
    }
//ADDED BY SELEX
            if (!CORBA::is_nil(m_processObserver.in()))
            {
                if (std::string(it->second.location[2].kind) == "processname")
                {
                    CdmwPlatformMngtBase::TimeStamp timeStamp =
                    EventHeaderFactory::createTimeStamp();
                    
                    m_processObserver->notify_monitoring_failure(location[1].id,
                                                                 location[2].id,
                                                                 location[0].id,
                                                                 timeStamp);
                }
            }
#endif
            
            

        }
        
        



    } // End namespace PlatformMngt
} // End namespace Cdmw
