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


#ifndef INCL_PLATFORMMNGT_APPLICATIONAGENT_IMPL_HPP 
#define INCL_PLATFORMMNGT_APPLICATIONAGENT_IMPL_HPP 

#include "Foundation/common/System.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/SchedulerBase.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

#include "platformlibrary/ProcessLauncher.hpp"

#include "platformagent/ProcessMessageBroker_impl.hpp"

#include "idllib/CdmwPlatformMngtDaemon.stub.hpp"
#include "platformfaultdetector/LocalFaultDetector_impl.hpp"

#include "idllib/CdmwPlatformMngtApplicationAgent.skel.hpp"
#include "idllib/CdmwPlatformMngtApplicationBase.stub.hpp"


#include <map>

namespace Cdmw
{
namespace PlatformMngt
{

class UnmanagedProcessCreationCallback;
class ProcessEndingCallback;
class ProcessLauncher;
class UnmanagedProcessCreationRun;
class ManagedProcessCreationRun;


/**
* Purpose:
* <p>
* implements the
* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent:1.0
* interface.
*/
class ApplicationAgent_impl : public POA_CdmwPlatformMngt::ApplicationAgent,
    virtual public PortableServer::RefCountServantBase
{

    friend class UnmanagedProcessCreationCallback;
    friend class ProcessEndingCallback;
    friend class UnmanagedProcessCreationRun;
    friend class ManagedProcessCreationRun;

public:

    /**
    * Purpose:
    * <p> Constructor
    * 
    */ 
    ApplicationAgent_impl(CORBA::ORB_ptr orb,
                          CdmwPlatformMngt::Daemon_ptr daemon,
                          LocalFaultDetector_impl* faultDetector,
                          ProcessLauncher* processLauncher,
                          OsSupport::SchedulerBase* scheduler,
                          const char* applicationName);


    /**
    * Purpose:
    * <p>
    * Activates this application agent servant and returns the associated CORBA reference.
    */
    CdmwPlatformMngt::ApplicationAgent_ptr activate(const char* applicationName);


    /**
    * Purpose:
    * <p>
    * Deactivates this application agent servant.
    */
    void deactivate(const char* applicationName);


    /**
    * Purpose:
    * <p>
    * Ends the creation of the application agent.
    */
    bool endCreation(const CdmwPlatformMngt::ApplicationAgentInfo& creationData);


    /**
    * Purpose:
    * <p> Destructor
    * 
    */ 
    virtual 
    ~ApplicationAgent_impl()
            throw();


    /**
    * Purpose:
    * <p>
    * Returns the specific POA associated with the daemon.
    */
    virtual PortableServer::POA_ptr _default_POA();


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorable/is_alive:1.0
    * operation
    */
/* Application agent is no more a process
    virtual void is_alive()
        throw(CORBA::SystemException);
*/

    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/create_process:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::ProcessCommandResult
                 create_process(const char* process_name,
                                const CdmwPlatformMngt::ProcessInfo& process_info,
                                CORBA::String_out error_info)
        throw(CORBA::SystemException);


    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/create_managed_process:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::ProcessCommandResult 
       create_managed_process(const char* process_name,
                              const CdmwPlatformMngt::ProcessInfo& process_info,
                              CdmwPlatformMngt::Timeout creation_timeout,
                              CdmwPlatformMngt::ProcessDelegateWrapper_out process_wrapper,
                              CORBA::String_out error_info,
                              CORBA::String_out error_issuer)
        throw(CORBA::SystemException);


    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/search_process_delegate_wrapper:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::ProcessDelegateWrapper_ptr
        search_process_delegate_wrapper(const char* process_name)
        throw(CORBA::SystemException);
    
    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/kill_process:1.0
	* operation
	*/
    virtual void kill_process(const char* process_name)
        throw(CdmwPlatformMngt::ProcessNotFound,
              CORBA::SystemException);

    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/get_pid:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::ProcessPID get_pid(const char* process_name)
        throw(CdmwPlatformMngt::ProcessNotFound,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Prepares the agent removal.
    */    
    virtual void prepare_agent_removal();


    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/start_process_monitoring:1.0
	* operation
	*/
    virtual void start_process_monitoring(const char* process_name,
                                          const CdmwPlatformMngt::CompleteMonitoringInfo& process_monitoring_info)
        throw(CdmwPlatformMngt::ProcessNotFound,
              CdmwPlatformMngt::UnsupportedMonitoringModel,
              CdmwPlatformMngt::IncompatibleType,
              CdmwPlatformMngt::IncompatibleMonitoringCommand,
              CdmwPlatformMngt::IncompatibleMonitoringValue,
              CORBA::SystemException);


    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/stop_process_monitoring:1.0
	* operation
	*/
    virtual void stop_process_monitoring(const char* process_name)
        throw(CdmwPlatformMngt::ProcessNotFound,
              CdmwPlatformMngt::IncompatibleType,
              CdmwPlatformMngt::IncompatibleMonitoringCommand,
              CORBA::SystemException);


    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/destroy:1.0
	* operation
	*/
    virtual void destroy()
        throw(CORBA::SystemException);
        
        
#ifdef CDMW_USE_FAULTTOLERANCE

    /**
    * Purpose:
    * <p> Set the Fault Notifier reference.
    * 
    */
    void set_fault_notifier (FT::FaultNotifier_ptr fault_notifier);  
    
#endif

    

private:

    /**
     * The type of process creation exception.
     */
    enum ExceptionRaisedType
    {
        NO_EXCEPTION_RAISED,
        NO_MEMORY_RAISED,
        INTERNAL_RAISED
    };
    

    /**
    * Purpose:
    * <p>  Copy constructor
    * 
    */ 
    ApplicationAgent_impl (const ApplicationAgent_impl& rhs)
         throw();


    /**
    * Purpose:
    * <p> Assignment operator
    * 
    */ 
    ApplicationAgent_impl&
    operator=(const ApplicationAgent_impl& rhs)
          throw();
   
    
    /**
    * Purpose:
    * <p> Put the given process name in the process map
    *     Do nothing if name entry already exist
    */    
    void add_blank_process (const std::string& name, 
                            CdmwPlatformMngt::ProcessType process_type);
    
    /**
    * Purpose:
    * <p> Associates the specified process id and the process name
    *     for unmanaged process.
    */ 
    void fill_created_process (const std::string& name,
                               OsSupport::OS::ProcessId processId)
        throw(BadParameterException);
    
    /**
    * Purpose:
    * <p> Associates the specified process id and the process name
    *     for managed process.
    */                           
    void fill_created_managed_process (const std::string& name,
                                       OsSupport::OS::ProcessId processId,
                                       CdmwPlatformMngt::ProcessDelegateWrapper_ptr process_wrapper)
        throw(BadParameterException);
    
    /**
    * Purpose:
    * <p> Set faulty process creation, command error.
    */                                    
    void fill_faulty_process (const std::string& name,
                              CdmwPlatformMngt::ProcessCommandResult command_result,
                              const std::string & error_issuer,
                              const std::string & error_info)
        throw(BadParameterException);
        
    /**
    * Purpose:
    * <p> Set faulty process creation, exception raised.
    */                           
    void fill_faulty_exception_process (const std::string& name,
                                        ExceptionRaisedType exception_raised)
        throw(BadParameterException);

    
        
          
    /**
    * Purpose:
    * <p> Reset the association between the id and the name of the
    * specified process produced by the process ending.
    *
    * @param processId the identifier of the process.
    * @return the name of the process.
    */    
    std::string reset_process_info_by_end (OsSupport::OS::ProcessId processId);
    
    /**
    * Purpose:
    * <p> Reset the association between the id and the name of the
    * specified process produced by the process killing
    *
    * @param name the name of the process
    * @return the name of the process.
    */    
    void reset_process_info_by_kill (const std::string& name);
    
    /**
    * Purpose:
    * <p> Remove the process info in map
    *
    * @param name the name of the process
    * @return the name of the process.
    */    
    void remove_process_info (const std::string& name);


    /**
    * Purpose:
    * <p> Indicates whether a process already exists with the specified name.
    */    
    bool exists_process(const std::string& name);

    /**
    * Purpose:
    * <p> Indicates whether a process already exists with the specified name (no mutex)
    */    
    bool exists_process_unlock(const std::string& name);
    
    /**
    * Purpose:
    * <p> Indicates whether a process with the specified id exists.
    */    
    bool exists_process(OsSupport::OS::ProcessId processId);


    /**
    * Purpose:
    * <p> Finds the Id of the OS process specified by its name.
    * 
    * @exception BadParameterException if name doesn't denote an existing
    * OS Process.
    */    
    OsSupport::OS::ProcessId find_OS_processId(const std::string& name)
        throw(BadParameterException);

    /**
    * Purpose:
    * <p> Finds the monitoring Id of the process specified by its name.
    * 
    * @exception BadParameterException if name doesn't denote an existing
    * OS Process.
    */    
    ::CdmwPlatformMngt::MonitoringId find_process_monitoringId (const std::string& name)
        throw(BadParameterException);

    /**
    * Purpose:
    * <p> Sets the process monitoring Id of the process specified by its name.
    * 
    * @exception BadParameterException if name doesn't denote an existing
    * OS Process.
    */    
    void set_process_monitoringId (const std::string& name,
                                   ::CdmwPlatformMngt::MonitoringId id)
        throw(BadParameterException);


    /**
    * Purpose:
    * <p> Get the PullMonitorable CORBA object associated with a managed Process.
    * 
    */    
    ::FT::PullMonitorable_ptr get_pull_monitorable(const std::string& name)
        throw(CdmwPlatformMngt::ProcessNotFound, 
              CdmwPlatformMngt::IncompatibleType);

private:

    struct ProcessInfo 
    {
        OsSupport::OS::ProcessId processId;                           // process id
        ::CdmwPlatformMngt::ProcessType processType;                  // process type (Managed/Unmanaged)
        ::CdmwPlatformMngt::MonitoringId monitoringId;                // monitoring id
        CdmwPlatformMngt::ProcessDelegateWrapper_var processWrapper;  // process delegate wrapper reference
        CdmwPlatformMngt::ProcessCommandResult commandResult;         // creation command result        
        std::string errorIssuer;                                      // creation error issuer
        std::string errorInfo;                                        // creation error info
        ExceptionRaisedType exceptionRaised;                          // exception raised flag
        bool process_auto_ended;                                      // process ended itself after starting
        bool process_killed;                                          // process killed by supervision
        bool lock;                                                    // lock flag
    };


    typedef std::map<std::string, ProcessInfo> OsProcesses;

    /**
    * Purpose:
    * <p> Give the iterator corresponding to the given processId (or end if not found)
    */    
    OsProcesses::iterator find_process_iterator_unlock(OsSupport::OS::ProcessId processId);

    /**
    * The processes managed by this application agent.
    */
    OsProcesses m_processes;

    /**
    * The lock protecting concurrent access to m_processes.
    */
    OsSupport::Mutex m_processes_mutex;
    
    /**
    * The condition on the process creation.
    */
    OsSupport::Condition m_creation_condition;
    
    /**
    * The scheduler for process creation.
    */
    OsSupport::SchedulerBase *m_scheduler;

    /**
    * The process ending callback.
    */
    ProcessEndingCallback *m_process_ending_callback;

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;
    
    /**
    * The root POA.
    */
    PortableServer::POA_var m_root_poa;

    /**
    * The specific POA managing the application agent.
    */
    PortableServer::POA_var m_poa;

    /**
    * The local fault detector.
    */
    LocalFaultDetector_impl* m_local_fault_detector;

    /**
    * The name of the application managing this agent.
    */
    std::string m_application_name;

    /**
    * The daemon.
    */
    CdmwPlatformMngt::Daemon_var m_daemon;
    
    /**
    * The associated process message broker servant.
    */
    ProcessMessageBroker_impl *m_process_message_broker;

    /**
    * The ServantBase_var used to manage allocation/deallocation
    * of m_processMessageBroker.
    */
    PortableServer::ServantBase_var m_process_message_broker_servant;

    /**
     * The application's process observer.
     */
    CdmwPlatformMngt::ProcessObserver_var m_process_observer;
        
#ifdef CDMW_USE_FAULTTOLERANCE

    /**
     * The fault notifier
     */
    FT::FaultNotifier_var m_fault_notifier;  
    
#endif

    /**
     * The ProcessLauncher
     */
    ProcessLauncher* m_process_launcher;
    
    /**
     * The local hostname
     */
    std::string m_local_host_name;

}; // End class ApplicationAgent_impl 

} // End namespace PlatformMngt
} // End namespace Cdmw
#endif // INCL_PLATFORMMNGT_APPLICATIONAGENT_IMPL_HPP


