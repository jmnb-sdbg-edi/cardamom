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


#ifndef INCL_PLATFORMMNGT_LOCALFAULTDETECTOR_IMPL_HPP
#define INCL_PLATFORMMNGT_LOCALFAULTDETECTOR_IMPL_HPP

#include <map>
#include <string>

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "platformfaultdetector/SingleShotTimer.hpp"
#include "platformfaultdetector/TimeOutPolicyService.hpp"

#include "idllib/CdmwPlatformMngtLocalFaultDetector.skel.hpp"

#include "Foundation/idllib/FTPullMonitorable.skel.hpp"

#include "Foundation/idllib/FTPullMonitorable.stub.hpp"

#ifdef CDMW_USE_FAULTTOLERANCE
#include "FaultTolerance/idllib/FT.stub.hpp"
#endif

#include "idllib/CdmwPlatformMngtApplication.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


  class FaultDetectorTimerObserver;
  class AMIProcessPullMonitorableHandler_impl;

/**
* Purpose:
* <p>
* implements the
* IDL:thalesgroup.com/CdmwPlatformMngt/LocalFaultDetector:1.0
* interface.
*/
  class LocalFaultDetector_impl : virtual public POA_CdmwPlatformMngt::LocalFaultDetector,
    virtual public PortableServer::RefCountServantBase
{

  friend class FaultDetectorTimerObserver;
  friend class AMIProcessPullMonitorableHandler_impl;

public:

    /**
    * Purpose:
    * <p>
    * Constructor.
    */
    LocalFaultDetector_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa);


    /**
    * Purpose:
    * <p>
    * Destructor.
    */
    virtual ~LocalFaultDetector_impl()
       throw();


//     /**
//     * Purpose:
//     * <p>
//     * Activates this LocalFaultDetector servant and returns the associated CORBA reference.
//     */
//     CdmwPlatformMngt::LocalFaultDetector_ptr activate();


//     /**
//     * Purpose:
//     * <p>
//     * Deactivates this LocalFaultDetector servant.
//     */
//     void deactivate();


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/LocalFaultDetector/start_monitoring:1.0
    * operation
    */
    virtual CdmwPlatformMngt::MonitoringId start_monitoring (
        ::FT::PullMonitorable_ptr monitorable_object,
        const ::FT::Location & location,
        const ::FT::FaultMonitoringIntervalAndTimeoutValue & timeout
      )
      throw (
        CORBA::SystemException,
        ::FT::ObjectNotFound,
        ::CdmwPlatformMngt::BadLocation
      );


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/LocalFaultDetector/stop_monitoring:1.0
    * operation
    */
    virtual void stop_monitoring (
        ::CdmwPlatformMngt::MonitoringId monitoring_id
      )
      throw (
        CORBA::SystemException,
        ::CdmwPlatformMngt::InvalidMonitoringId
      );


    /**
    * Purpose:
    * <p>
    * implements the
    * set_timer_interval
    * operation
    */
    void set_timer_interval (::TimeBase::TimeT monitoring_interval)
      throw (CORBA::SystemException);

#ifdef CDMW_USE_FAULTTOLERANCE
    /**
    * Purpose:
    * <p>
    * implements the
    * set_fault_notifier
    * operation
    */
    virtual void set_fault_notifier (::FT::FaultNotifier_ptr fault_notifier)
      throw (CORBA::SystemException);
#else
    /**
    * Purpose:
    * <p>
    * implements the
    * set_process_observer
    * operation
    */
    virtual void set_process_observer (CdmwPlatformMngt::ProcessObserver_ptr process_observer)
      throw (CORBA::SystemException);
#endif


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:omg.org/FT/is_alive:1.0
    * operation
    */
    virtual CORBA::Boolean is_alive (

      )
      throw (
        CORBA::SystemException
      );


private:

    /**
    * Purpose:
    * <p>
    * Processes timer awakenings.
    */
    void wakeUp();

    /**
    * Purpose:
    * <p>
    * Processes is_alive invocation reply.
    */
    void is_alive_notification (
        CORBA::Boolean ami_return_val,
        ::CdmwPlatformMngt::MonitoringId id
      )
      throw (
        CORBA::SystemException
      );


    /**
    * Purpose:
    * <p>
    * Processes is_alive invocation exception.
    */
    void is_alive_excep_notification (
        ::FT::AMI_PullMonitorableExceptionHolder * excep_holder,
        ::CdmwPlatformMngt::MonitoringId monitoring_id
      )
      throw (
        CORBA::SystemException
      );


    /**
    * Purpose:
    * <p> Indicates whether the detector manages an associated monitorable
    * identified by its monitoring id.
    */    
    bool has_monitorable(CdmwPlatformMngt::MonitoringId monitoring_id);


    /**
    * Purpose:
    * <p> Indicates whether the detector manages an associated monitorable
    * identified by its location.
    */    
    bool has_monitorable(::FT::Location location);

    struct MonitorableInfo;

    /**
    * Purpose:
    * <p> Stores the monitorableInfo. 
    */
    void add_monitorable(CdmwPlatformMngt::MonitoringId monitoring_id,
                        MonitorableInfo info);


    /**
    * Purpose:
    * <p> Removes the monitorableInfo.
    */
    LocalFaultDetector_impl::MonitorableInfo remove_monitorable(CdmwPlatformMngt::MonitoringId monitoring_id);


    /**
     * Purpose:
     * <p> Notification of the failure of the process to the observer
     */ 
    void notify_failure(::FT::Location location);
    

    /**
    * A PullMonitorable CORBA object processing state
    * INIT        : State of a PullMonitorable CORBA object that has just been rgistered and
    *               interrogated for the first time
    * ACK_WAITING : State of a PullMonitorable CORBA object that has not acknowledged a
    *               fault detector interrogation
    * ACK_OK      : State of a PullMonitorable CORBA object that has acknowledged a
    *               fault detector interrogation
    */
    enum ProcessingState {INIT, ACK_WAITING, ACK_OK};

    struct MonitorableInfo {
       unsigned int countMax;
       unsigned int currentCount;
       ProcessingState state;
       ::FT::Location location;
       ::FT::FaultMonitoringIntervalAndTimeoutValue timeout;
       ::FT::PullMonitorable_var pullMonitorable;
       ::FT::AMI_PullMonitorableHandler_var handler;
    };


    typedef std::map<CdmwPlatformMngt::MonitoringId, MonitorableInfo> MonitorableMap;

    /**
    * The Monitoring id.
    */
    CdmwPlatformMngt::MonitoringId m_monitoringId;

    /**
    * The map of Monitorable used to monitor objects and check their acknowledgement.
    */
    MonitorableMap m_monitorableMap;

    /**
    * The mutex protecting concurrent access to m_monitorableMap
    */
    OsSupport::Mutex m_mutex;

    /**
    * The local hostname
    */
    std::string m_localHostname;

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;

    /**
    * The root POA.
    */
    PortableServer::POA_var m_poaHandler;

    /**
    * A specific POA.
    */
    PortableServer::POA_var m_poa;

    /**
    * The Timeout policy Service 
    */
    TimeOutPolicyService* m_timeoutPolicyService;

    /**
    * The object id returned after the object activation.
    */
    PortableServer::ObjectId_var m_objectId;

    /**
    * The local fault detector timer observer.
    */
    FaultDetectorTimerObserver* m_pTimerObserver;

    /**
    * The local fault detector timer.
    */
    SingleShotTimer* m_pTimer;

    const static TimeBase::TimeT TIMER_INTERVAL_MAX = 0xFFFFFFFFFFFFFFFF;

    /**
    * The timer interval.
    */
    TimeBase::TimeT m_timerInterval;

    /**
    * The timer interval.
    */
    bool m_timerStarted;

    /**
    * The single AMIProcessPullMonitorableHandler_impl implementation 
    */
    AMIProcessPullMonitorableHandler_impl* m_pHandler_impl;

#ifdef CDMW_USE_FAULTTOLERANCE
    /**
    * The fault notifier 
    */
    ::FT::FaultNotifier_var m_faultNotifier;
#else
    /**
    * The process observer
    */
    CdmwPlatformMngt::ProcessObserver_var m_processObserver;
#endif
};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_LOCALFAULTDETECTOR_IMPL_HPP

