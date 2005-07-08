/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef INCL_PLATFORMMNGT_APPLICATIONAGENT_IMPL_HPP 
#define INCL_PLATFORMMNGT_APPLICATIONAGENT_IMPL_HPP 

#include "Foundation/common/System.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "idllib/CdmwPlatformMngtApplicationAgent.skel.hpp"
#include "idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "idllib/CdmwPlatformMngtProcess.stub.hpp"
#include "idllib/CdmwPlatformMngtProcessCallback.stub.hpp"

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "platformlibrary/ProcessLauncher.hpp"

#include "platformagent/ProcessMessageBroker_impl.hpp"

#include <map>

namespace Cdmw
{
namespace PlatformMngt
{

class ProcessCreationCallback;
class ProcessEndingCallback;

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

    friend class ProcessCreationCallback;
    friend class ProcessEndingCallback;

public:

    /**
    * Purpose:
    * <p> Constructor
    * 
    */ 
    ApplicationAgent_impl(CORBA::ORB_ptr orb, ProcessLauncher* processLauncher);


    /**
    * Purpose:
    * <p>
    * Activates this application agent servant and returns the associated CORBA reference.
    */
    CdmwPlatformMngt::ApplicationAgent_ptr activate();


    /**
    * Purpose:
    * <p>
    * Ends the creation of the application agent.
    */
    bool endCreation(const CdmwPlatformMngt::ApplicationAgentAck& acknowledgementData);


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
    virtual void is_alive()
        throw(CORBA::SystemException);


    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/create_process:1.0
	* operation
	*/
    virtual void create_process(const char* process_name,
                                const CdmwPlatformMngt::ProcessInfo& process_info)
        throw(CdmwPlatformMngt::ProcessAlreadyExists,
              CdmwPlatformMngt::InvalidInfo,
              CORBA::SystemException);


    /**
	* Purpose:
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/create_managed_process:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::Process_ptr create_managed_process(const char* process_name,
            const CdmwPlatformMngt::ProcessInfo& process_info,
            CdmwPlatformMngt::Timeout creation_timeout,
            CdmwPlatformMngtEntity::EntityObserver_ptr entity_observer,
            CdmwPlatformMngtService::ServiceBroker_ptr service_broker)
        throw(CdmwPlatformMngt::CreationErrorExt,
              CdmwPlatformMngt::ProcessAlreadyExists,
              CdmwPlatformMngt::InvalidInfo,
              CdmwPlatformMngt::CreationTimeout,
              CORBA::SystemException);


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
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgent/start_process_monitoring:1.0
	* operation
	*/
    virtual void start_process_monitoring(const char* process_name,
                                          const CdmwPlatformMngt::CompleteMonitoringInfo& process_monitoring_info,
                                          const CdmwPlatformMngt::MonitoringInfos& activity_point_monitoring_infos)
        throw(CdmwPlatformMngt::ProcessNotFound,
              CdmwPlatformMngt::UnsupportedMonitoringModel,
              CdmwPlatformMngt::OutOfActivityPoints,
              CdmwPlatformMngt::IncompatibleType,
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
        
      
    /**
    * Purpose:
    * <p> Failure exit during agent initialisation
    */      
    void set_init_exit_failure();
	
	  

private:

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
    * <p> Associates the specified process id and the process name.
    */    
    void addProcess(OsSupport::OS::ProcessId processId,
            const std::string& name);


    /**
    * Purpose:
    * <p> Removes the association between the id and the name of the
    * specified process.
    *
    * @param processId the identifier of the process.
    * @return the name of the process.
    */    
    std::string removeProcess(OsSupport::OS::ProcessId processId);


    /**
    * Purpose:
    * <p> Indicates whether a process already exists with the specified name.
    */    
    bool existsProcess(const std::string& name);


    /**
    * Purpose:
    * <p> Indicates whether a process with the specified id exists.
    */    
    bool existsProcess(OsSupport::OS::ProcessId processId);

    /**
    * Purpose:
    * <p> Finds the Id of the OS process specified by its name.
    * 
    * @exception BadParameterException if name doesn't denote an existing
    * OS Process.
    */    
    OsSupport::OS::ProcessId findOSProcessId(const std::string& name)
        throw(BadParameterException);
        
        
	/**
    * Purpose:
    * <p> Initialisation is done
    */ 
    void set_init_done();



private:


    typedef std::map<OsSupport::OS::ProcessId, std::string> OsProcesses;

    /**
    * The processes managed by this application agent.
    */
    OsProcesses m_processes;

    /**
    * The mutex protecting concurrent access to m_processes.
    */
    OsSupport::Mutex m_mutex;

    /**
    * The process ending callback.
    */
    ProcessEndingCallback *m_processEndingCallback;

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;
    
    /**
    * The root POA.
    */
    PortableServer::POA_var m_rootPoa;

    /**
    * The specific POA managing the application agent.
    */
    PortableServer::POA_var m_poa;

    /**
    * The name of the application managing this agent.
    */
    std::string m_applicationName;

    /**
    * The associated process message broker servant.
    */
    ProcessMessageBroker_impl *m_processMessageBroker;

    /**
    * The ServantBase_var used to manage allocation/deallocation
    * of m_processMessageBroker.
    */
    PortableServer::ServantBase_var m_processMessageBroker_servantBase;

    /**
     * The application's process observer.
     */
    CdmwPlatformMngt::ProcessObserver_var m_processObserver;
        
#ifdef CDMW_USE_FAULTTOLERANCE
    /**
     * The fault notifier
     */
    FT::FaultNotifier_var m_faultNotifier;  
#endif

    /**
     * The ProcessLauncher
     */
    ProcessLauncher* m_processLauncher;
    
    
    bool m_init_exception_raised;
    bool m_init_done;
    OsSupport::Condition* m_pinit_condition;
    OsSupport::Mutex      m_init_mutex;
     
}; // End class ApplicationAgent_impl 

} // End namespace PlatformMngt
} // End namespace Cdmw
#endif // INCL_PLATFORMMNGT_APPLICATIONAGENT_IMPL_HPP


