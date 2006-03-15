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


#include "platformdaemon/Daemon_impl.hpp"
#include "platformdaemonlibrary/DaemonLogMessageId.hpp"
#include "platformdaemon/PullMonitorableRegistration_impl.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"
#include "SystemMngt/platformlibrary/ProcessLauncher.hpp"
#include "SystemMngt/platformlibrary/ProcessKillMngr.hpp"
// To put after localfaultdetector integration:
#include "platformfaultdetector/LocalFaultDetector_impl.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"

#include <sstream>
#include <memory>

namespace
{
    const char* PULLMONITOR_REGISTER_REP_ID = 
        "IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorableRegistration_impl/register_pull_monitorable:1.0";
         
    const int MAX_RETRY_ON_FTMGR = 4;
    const int SLEEP_RETRY_ON_FTMGR_MS = 1000;
    
    const std::string DAEMON_PORT_OPTION = "--DaemonPort";
    const std::string PULLMON_REGISTER_IOR_OPTION = "--PullMonRegisterIor";
    const int NB_SCHEDULER_THREADS = 5;
}


namespace Cdmw
{
namespace PlatformMngt
{

class SimpleProcessCreationCallback : public OsSupport::ProcessCallback
{
    
public:

    SimpleProcessCreationCallback(Daemon_impl* daemon, const std::string& processName)
        : m_daemon(daemon), m_processName(processName)
    {
    }

    virtual ~SimpleProcessCreationCallback() { }

    void execute(OsSupport::OS::ProcessId processId) throw()
    {
        m_daemon->addSimpleProcess(processId, m_processName);
        
        // register the process to killer
        ProcessKillMngr::Instance().register_process ("", m_processName.c_str(), processId);
        
        LogMngr::logMessage(INF, MSG_ID_DAEMON_PROCESS_CREATED, m_processName);
    }

private:

    Daemon_impl *m_daemon;
    std::string m_processName;


};


class SimpleProcessEndingCallback : public EndingCallback
{
    
public:

    SimpleProcessEndingCallback(Daemon_impl* daemon,
                                CdmwPlatformMngt::LocalFaultDetector_ptr localFaultDetector)
        : m_daemon(daemon)
    {
        // set fault detector
        m_localFaultDetector = 
           CdmwPlatformMngt::LocalFaultDetector::_duplicate(localFaultDetector);
    }

    virtual ~SimpleProcessEndingCallback()
    throw()
    { 
    }

    void execute(OsSupport::OS::ProcessId processId) 
    throw()
    {
        try
        {
            ::CdmwPlatformMngt::MonitoringId monitoringId;
            
            std::string processName = m_daemon->removeSimpleProcess(processId, monitoringId);
            
            if (!processName.empty())
            {
                // logs the process ending
                LogMngr::logMessage(INF, MSG_ID_DAEMON_PROCESS_ENDED, processName);
                
                // stop the process monitoring
                try
                {
                    m_localFaultDetector->stop_monitoring(monitoringId);
                    
                    std::cout << "<<SimpleProcessEndingCallback::execute()>> "
                              << processName.c_str()
                              << " Monitoring stopped" << std::endl;
                }
                catch(...)
                {
                }
            
#ifdef CDMW_USE_FAULTTOLERANCE

                // get the host name
                std::string localHostName = OsSupport::OS::get_hostname();
      
                // get the fault notifier
                FT::FaultNotifier_var faultNotifier = 
                             m_daemon->get_fault_notifier();
    
                if (!CORBA::is_nil(faultNotifier.in()))
                {
                    ::FT::Location location;
               
                    location.length(2);
                    location[0].kind = CORBA::string_dup ("hostname");
                    location[0].id = CORBA::string_dup (localHostName.c_str());
                    location[1].kind = CORBA::string_dup ("processname" );
                    location[1].id = CORBA::string_dup (processName.c_str());
                    
                    // Prepare the event for the fault notification
                    CosNotification::StructuredEvent fault_event;
                    fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup ("FT_CORBA");
                    fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup ("ObjectCrashFault");
                    fault_event.filterable_data.length(3);
                    fault_event.filterable_data[0].name = CORBA::string_dup("FTDomainId");
                    fault_event.filterable_data[0].value <<= "thalesgroup.com";
                    fault_event.filterable_data[1].name = CORBA::string_dup("Location");
                    fault_event.filterable_data[1].value <<= location;
                    CdmwPlatformMngtBase::TimeStamp time_stamp =
                    PlatformMngt::EventHeaderFactory::createTimeStamp();
                    fault_event.filterable_data[2].name = CORBA::string_dup( "TimeStamp" );
                    fault_event.filterable_data[2].value <<= time_stamp;                

                    try
                    {
                        faultNotifier->push_structured_fault(fault_event);
                    }
                    catch (CORBA::SystemException& e)
                    {
                        // std::cerr << "Daemon_impl push_structured_fault calling exception : " 
                        //           << e << std::endl;
                    }
                    catch (...)
                    {
                        // nothing to do
                    }                                      
                }
                
#endif

            }
        
            // remove the process from killer
            ProcessKillMngr::Instance().remove_process (processId);
        }
        catch(const std::bad_alloc&)
        {
            LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        }
        catch(...)
        {
            LogMngr::logMessage(ERR, MSG_ID_DAEMON_INTERNAL);
        }        
    }

private:

    /**
    * The daemon.
    */
    Daemon_impl *m_daemon;

    /**
    * The local fault detector.
    */
    CdmwPlatformMngt::LocalFaultDetector_var m_localFaultDetector;
    

};

//
// Constructor.
//
Daemon_impl::Daemon_impl(CORBA::ORB_ptr orb,
                         const std::string& systemPortString,
                         unsigned long notificationCallTimeout,
                         unsigned long pullInterval,
                         unsigned long pullTimeout,
                         ProcessLauncher* processLauncher,
                         ServiceRegistration_impl* serviceRegistration,
                         size_t duration_time,
                         const std::string& fault_manager_corbaloc) 
    :m_systemPortString(systemPortString), 
     m_processLauncher(processLauncher),
     m_serviceRegistration(serviceRegistration),
     m_duration_time(duration_time),
     m_scheduler(NB_SCHEDULER_THREADS)
{

    if (CORBA::is_nil(orb))
        CDMW_THROW2(BadParameterException, "orb", "nil");

    m_hostName = OsSupport::OS::get_hostname();

    // creates the specific POA
    CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
    m_rootPoa = PortableServer::POA::_narrow(object.in());

    PortableServer::POAManager_var manager = m_rootPoa->the_POAManager();

    CORBA::PolicyList policies;
    OrbSupport::StrategyList poaStrategyList;

    policies.length(2);
    policies[0] = m_rootPoa->create_id_assignment_policy(PortableServer::USER_ID);
    policies[1] = m_rootPoa->create_lifespan_policy(PortableServer::PERSISTENT);

// the PERSISTENT policy is incompatible with the THREADPOOL policy for TAO
#if CDMW_ORB_VDR == orbacus
    poaStrategyList.add_PoaThreadPerRequest();

#elif CDMW_ORB_VDR == tao
    poaStrategyList.add_PoaThreadPerConnection();
#endif

    m_poa = OrbSupport::OrbSupport::create_POA(m_rootPoa.in(), "CdmwDaemonPOA", 
        manager.in(), policies, poaStrategyList);

    m_orb = CORBA::ORB::_duplicate(orb);

    // initialise the BoundSyncCallFactory 
    BoundSyncCallFactory::initialise(orb);

    
    
#ifdef CDMW_USE_FAULTTOLERANCE
    
    // 
    // get the Fault Manager reference from corbaloc argument
    //
    if (!fault_manager_corbaloc.empty())
    {
        CdmwFT::FTManager_var faultManager = CdmwFT::FTManager::_nil();
        CdmwFT::ReplicationManager_var replicationManager = CdmwFT::ReplicationManager::_nil();
        FT::FaultNotifier_var faultNotifier = FT::FaultNotifier::_nil(); 
    
        CORBA::Object_var fm_obj = orb->string_to_object (fault_manager_corbaloc.c_str());
        if (CORBA::is_nil (fm_obj.in()))
        {
            CDMW_THROW2(Cdmw::Exception, Cdmw::Exception::FATAL,
                        "Invalid fault manager corbaloc"); 
        }
    
        try 
        {
            faultManager = ::CdmwFT::FTManager::_narrow (fm_obj.in());
        } 
        catch (const CORBA::SystemException & e) 
        {
            std::cerr << "Exception while contacting FTManager: "
                      << e << "\n"
                      << fault_manager_corbaloc << " is probably invalid"
                      << std::endl;
                  
            CDMW_THROW2(Cdmw::Exception, Cdmw::Exception::FATAL,
                        "Invalid fault manager corbaloc");
        }

        if (CORBA::is_nil (faultManager.in()))
        {
            CDMW_THROW2(Cdmw::Exception, Cdmw::Exception::FATAL,
                        "Invalid fault manager reference" );
        }
    
        // get the replication manager, fault notifier
        try
        {        
            bool ftmgr_not_active = true;
            int nbRetries = 0;
        
            do 
            {
                try 
                {
                    // get the replication manager from the FTManager
                    CORBA::Object_var rm_obj = faultManager->get_replication_manager();                
                    replicationManager = CdmwFT::ReplicationManager::_narrow (rm_obj.in());
                    ftmgr_not_active = false;
                }
                catch (const CdmwFT::FTManager::NoAlreadyActivated&)
                {
                    if (++nbRetries > MAX_RETRY_ON_FTMGR) 
                    {
                        std::cerr << "ERROR: FTManager not yet activated." << std::endl;
                        throw;
                    }
                    std::cerr << "Retry to contact FTManager in " << SLEEP_RETRY_ON_FTMGR_MS 
                              << " ms ..." << std::endl;
                    Cdmw::OsSupport::OS::sleep(SLEEP_RETRY_ON_FTMGR_MS);
                }
            } while (ftmgr_not_active);
            
            
            // register the replication manager in global service
            m_serviceRegistration->register_local_service("ReplicationManager",
                                                          replicationManager.in());
        
       
            // get the fault notifier from the FTManager
            CORBA::Object_var fn_obj = faultManager->get_fault_notifier();
        
            faultNotifier = FT::FaultNotifier::_narrow (fn_obj.in());
            
            // register the Fault Notifier in global service
            m_serviceRegistration->register_local_service("FaultNotifier",
                                                          faultNotifier.in());                   
        }
        catch(const CdmwFT::FTManager::NoAlreadyActivated&)
        {
            CDMW_THROW2 (BadParameterException, "fault manager", "Not the primary");
        }
    }
           
#endif
        
    //
    // create the PullMonitorableRegistration servant
    //
    
    // create the specific poa  
    CORBA::PolicyList pmr_poa_policies;
    OrbSupport::StrategyList pmr_poa_strategy_list;

    pmr_poa_policies.length(6);
    pmr_poa_policies[0] = m_rootPoa->create_id_uniqueness_policy(PortableServer::MULTIPLE_ID);
    pmr_poa_policies[1] = m_rootPoa->create_lifespan_policy(PortableServer::TRANSIENT);
    pmr_poa_policies[2] = m_rootPoa->create_servant_retention_policy(PortableServer::NON_RETAIN);
    pmr_poa_policies[3] = m_rootPoa->create_id_assignment_policy(PortableServer::USER_ID);
    pmr_poa_policies[4] = m_rootPoa->create_request_processing_policy(PortableServer::USE_DEFAULT_SERVANT);
    pmr_poa_policies[5] = m_rootPoa->create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);

    m_pullMonitorRegistrationPoa = OrbSupport::OrbSupport::create_POA(
                                               m_rootPoa.in(), "PullMonitorableRegistrationPOA", 
                                               manager.in(), pmr_poa_policies, pmr_poa_strategy_list);
                                               

    m_localFaultDetector_impl = new LocalFaultDetector_impl(m_orb.in(), m_rootPoa.in());
    PortableServer::ServantBase_var localFaultDetectorServant_var = m_localFaultDetector_impl;
    m_rootPoa->activate_object(localFaultDetectorServant_var.in());
    m_localFaultDetector = m_localFaultDetector_impl->_this();

//     // Servant var to take pointer ownership and to release
//     // ref counter from 2 to 1 after object activation
//     PortableServer::ServantBase_var localFaultDetectorServantBase;

//     localFaultDetectorServantBase = localFaultDetector_impl;
    

    // create the PullMonitorableRegistration default servant
    PullMonitorableRegistration_impl* defaultServant =
            new PullMonitorableRegistration_impl(m_orb.in(), m_localFaultDetector.in(),
                                                 this, pullInterval, pullTimeout);
    // create an object var to take pointer ownership
    // (ref count will be decremented when var will be destroyed at the method end)
    PortableServer::ServantBase_var servant_var = defaultServant;
        
    // set default servant to the specific poa
    m_pullMonitorRegistrationPoa->set_servant(defaultServant);
    
    // creates the simple process ending callback
    m_simpleProcessEndingCallback = 
       new SimpleProcessEndingCallback(this, m_localFaultDetector.in());
    
    // start the scheduler
    m_scheduler.start();

}


CdmwPlatformMngt::Daemon_ptr Daemon_impl::activate()
{

    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId("CdmwDaemon");
    
    m_poa->activate_object_with_id(oid.in(), this);

    return _this();

}



//
// Destructor.
//
Daemon_impl::~Daemon_impl()
{
    delete m_processLauncher;
}


PortableServer::POA_ptr Daemon_impl::_default_POA()
{

    return PortableServer::POA::_duplicate(m_poa.in());

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorable/is_alive:1.0
//
void Daemon_impl::is_alive()
    throw(CORBA::SystemException)
{
    //TODO : check the liveliness deeper
}



//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/resolve_corbaloc:1.0
//
CORBA::Object_ptr Daemon_impl::resolve_corbaloc(const char* corbaloc)
    throw(CdmwPlatformMngt::InvalidCorbaloc,
          CORBA::SystemException)
{

    try
    {
        return m_orb->string_to_object(corbaloc);
    }
    catch(const CORBA::BAD_PARAM&)
    {
        throw CdmwPlatformMngt::InvalidCorbaloc();
    }
    
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/get_remote_daemon:1.0
//
CdmwPlatformMngt::Daemon_ptr Daemon_impl::get_remote_daemon(const char* host_name)
    throw(CdmwPlatformMngt::HostNotFound,
          CdmwPlatformMngt::HostNotReachable,
          CORBA::SystemException)
{

    try
    {
        std::string remoteDaemonCorbaloc("corbaloc::");
        remoteDaemonCorbaloc += host_name;
        remoteDaemonCorbaloc += ":";
        remoteDaemonCorbaloc += m_systemPortString + "/" + CdmwPlatformMngt::DAEMON_NAME;

        CORBA::Object_var object = m_orb->string_to_object(remoteDaemonCorbaloc.c_str());
        CdmwPlatformMngt::Daemon_var daemon = CdmwPlatformMngt::Daemon::_narrow(object.in());

        if (CORBA::is_nil(daemon.in()))
        {
            throw CdmwPlatformMngt::HostNotFound();
        }

        return daemon._retn();

    }
    catch(const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const CORBA::BAD_PARAM&)
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAM, CORBA::COMPLETED_NO);
    }
    catch(const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::HostNotReachable();
    }

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_process:1.0
//          
void Daemon_impl::create_process (const char * process_name,
                                  const CdmwPlatformMngt::ProcessInfo& process_info)
    throw(CdmwPlatformMngt::ProcessAlreadyExists,
          CdmwPlatformMngt::InvalidInfo,
          CORBA::SystemException)
{    
    OsSupport::OS::ProcessId processId;

    if (hasSimpleProcess(process_name, processId))
    {
        throw CdmwPlatformMngt::ProcessAlreadyExists();
    }

    // set object oid
    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(process_name);
    
    // Make a new reference for the PullMonitorableRegistration object.
    CORBA::Object_var obj = 
        m_pullMonitorRegistrationPoa->create_reference_with_id(
                                         oid, PULLMONITOR_REGISTER_REP_ID);

    CdmwPlatformMngt::PullMonitorableRegistration_var pull_monitor_reg = 
        CdmwPlatformMngt::PullMonitorableRegistration::_narrow(obj.in());
        
    try
    {

        // creates the arguments for the OS process
        std::string args;
        
        if (process_info.process_port > 0)
        {            
            args += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
            args += "=";
            std::ostringstream port_str;
            port_str << process_info.process_port;
            args += port_str.str();
            args += " ";
        }
        
        args += Cdmw::Common::Options::PROCESS_NAME_OPTION;
        args += "=";
        args += process_name;
        
        args += " ";
        args += DAEMON_PORT_OPTION;
        args += "=";
        args += m_systemPortString;
        
        args += " ";
        args += PULLMON_REGISTER_IOR_OPTION;
        args += "=";
        std::string pullMonitorRegistrationIOR = m_orb->object_to_string(pull_monitor_reg.in());
        args += pullMonitorRegistrationIOR; 

        args += " ";
        args += process_info.process_args.in();
        
        
        //// argument process callback

        // creates the OS process
        SimpleProcessCreationCallback processCreationCallback(this, process_name);

        LogMngr::logMessage(INF, MSG_ID_DAEMON_CREATE_PROCESS, process_name);
            
            
        // creation of process using ProcessLauncher
        if (m_processLauncher != NULL) 
        {
            processId = m_processLauncher->create_process (
                process_name,
                process_info.process_exe.in(),
                args,
                process_info.working_directory.in(),
                &processCreationCallback, m_simpleProcessEndingCallback);

        } 
        else 
        {
            processId = OsSupport::OS::create_process (
                process_info.process_exe.in(),
                args,
                process_info.working_directory.in(),
                &processCreationCallback, m_simpleProcessEndingCallback);
        }
    }
    catch (const BadParameterException& e)
    {
        LogMngr::logMessage(WRN, MSG_ID_DAEMON_INVALID_PROCESS_INFO, process_name,
                            e.m_parameterValue);
                    

        CdmwPlatformMngt::InvalidInfo exception;
        exception.reason = CORBA::string_dup((e.m_parameterValue).c_str());
        throw exception;
    }
    catch(const std::bad_alloc&)
    {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException&)
    {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException&)
    {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_INTERNAL);
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

          
              
//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_process:1.0
//         
void Daemon_impl::kill_process (const char * process_name)
    throw(CdmwPlatformMngt::ProcessNotFound,
          CORBA::SystemException)
{
    OsSupport::OS::ProcessId processId;
    ::CdmwPlatformMngt::MonitoringId monitoringId;

    if (!removeSimpleProcess(process_name, processId, monitoringId))
    {
        throw CdmwPlatformMngt::ProcessNotFound();
    }

    LogMngr::logMessage(INF, MSG_ID_DAEMON_KILL_PROCESS, process_name);
    
    // stop the process monitoring
    try
    {
        m_localFaultDetector->stop_monitoring(monitoringId);
        
        std::cout << "<<Daemon_impl::kill_process()>> "
                      << process_name
                      << " Monitoring stopped" << std::endl;
    }
    catch(...)
    {
    }
    
    // kills the process
    try
    {
        if (m_processLauncher != NULL) 
        {
            m_processLauncher->kill_process(process_name);
        } 
        else
        {
            OsSupport::OS::kill_process(processId);
        }
    }
    catch(...)
    {
    }
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_application:1.0
//
CdmwPlatformMngt::Application_ptr Daemon_impl::create_application(
        const CdmwPlatformMngt::ApplicationInfo& application_info,
        CdmwPlatformMngt::Timeout creation_timeout,
        CdmwPlatformMngt::ApplicationControl_out application_control)
    throw(CdmwPlatformMngt::CreationError,
        CdmwPlatformMngt::ApplicationAlreadyExists,
        CdmwPlatformMngt::InvalidInfo,
        CdmwPlatformMngt::CreationTimeout,
        CORBA::SystemException)
{
    //TODO
    throw CORBA::NO_IMPLEMENT(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_application:1.0
//
void Daemon_impl::kill_application(const char* application_name)
    throw(CdmwPlatformMngt::ApplicationNotFound, CORBA::SystemException)
{
    //TODO
    throw CORBA::NO_IMPLEMENT(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/create_application_agent:1.0
//
CdmwPlatformMngt::ApplicationAgent_ptr Daemon_impl::create_application_agent(
        const CdmwPlatformMngt::ApplicationAgentInfo& agent_info)
    throw(CdmwPlatformMngt::CreationError,
        CdmwPlatformMngt::ApplicationAgentAlreadyExists,
        CdmwPlatformMngt::InvalidInfo, 
        CORBA::SystemException)
{

    // Servant var to take pointer ownership and to release
    // ref counter from 2 to 1 after object activation
    PortableServer::ServantBase_var applicationAgentServantBase;

    const char* applicationName = agent_info.application_name.in();

    if (hasApplicationAgent(applicationName))
    {
        throw CdmwPlatformMngt::ApplicationAgentAlreadyExists();
    }
    else
    {
        try 
        {
           // creates the application agent servant
           ApplicationAgent_impl* pAgent_impl =
              new ApplicationAgent_impl(m_orb.in(), this->_this(), 
                                        m_localFaultDetector_impl, 
                                        m_processLauncher, &m_scheduler,
                                        applicationName);
       
           applicationAgentServantBase = pAgent_impl;

#ifdef CDMW_USE_FAULTTOLERANCE
           
           pAgent_impl->set_fault_notifier (get_fault_notifier());
           
#endif

           pAgent_impl->endCreation(agent_info);

           // activates the associated corba object and gets a reference on it
           CdmwPlatformMngt::ApplicationAgent_var applicationAgent =
              pAgent_impl->activate(applicationName);

           // adds the servant in the map
           addApplicationAgent(applicationName, pAgent_impl);           

           // returns the corba object reference
           return applicationAgent._retn();
        }
        catch(const std::bad_alloc&)
        {
           LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
           throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
        }
        catch(const OutOfMemoryException&)
        {
           LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
           throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException& ex)
        {
           LogMngr::logMessage(ERR, MSG_ID_DAEMON_INTERNAL, ex.get_info());
           throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
    } 
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/search_application_agent:1.0
//
CdmwPlatformMngt::ApplicationAgent_ptr Daemon_impl::search_application_agent(
        const char* application_name)
    throw(CORBA::SystemException)
{
    CdmwPlatformMngt::ApplicationAgent_ptr applicationAgent =
        CdmwPlatformMngt::ApplicationAgent::_nil();
        
    CDMW_MUTEX_GUARD(m_agentsMutex);

    std::string name(application_name);

    for (Agents::iterator it = m_agents.begin(); it != m_agents.end(); ++it)
    {
      if (name == it->first)
      {
        applicationAgent = (it->second)->_this();

        if (CORBA::is_nil(applicationAgent))
        {
          // the returned processObject is not actually an application agent !
          throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }

        break;
      }
    }

    return applicationAgent;
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/kill_application_agent:1.0
//
void Daemon_impl::kill_application_agent(
        const char* application_name)
    throw(CdmwPlatformMngt::ApplicationAgentNotFound, CORBA::SystemException)
{
    if (!Daemon_impl::hasApplicationAgent(application_name))
    {
        throw CdmwPlatformMngt::ApplicationAgentNotFound();
    }

    LogMngr::logMessage(INF, MSG_ID_DAEMON_KILL_APPLICATION_AGENT, application_name);

    try 
    { 
      // suppress agent from map
      ApplicationAgent_impl* agentImpl = removeApplicationAgent(application_name);

      // prepare agent for removal
      agentImpl->prepare_agent_removal();
  
    } catch(const OutOfMemoryException&) {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    } catch (const InternalErrorException& ex) {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_INTERNAL, ex.get_info());
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/register_local_service:1.0
//   
CORBA::Object_ptr Daemon_impl::register_local_service (const char * service_name,
                                                       CORBA::Object_ptr service_ref)
    throw(CORBA::SystemException)
{
    std::cout << "<<Daemon_impl::register_local_service (" << service_name
              << ")>>" << std::endl;
              
    CORBA::Object_var old_service_ref = CORBA::Object::_nil();
    
    if (!strcmp (service_name, ""))
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMInvalidServiceName,
                               CORBA::COMPLETED_NO);
    }
    
    if (CORBA::is_nil(service_ref))
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference,
                               CORBA::COMPLETED_NO);
    }
    
    CDMW_MUTEX_GUARD(m_localServicesMutex);
    
    Services::iterator it = m_localServices.find(service_name);
    
    if (it != m_localServices.end())
    {
        old_service_ref = it->second;                     
    }
    
    m_localServices[service_name] = 
             CORBA::Object::_duplicate(service_ref);
      
    // bind the service to corbaloc         
    OrbSupport::OrbSupport::bind_object_to_corbaloc(
            m_orb.in(), service_name, service_ref);
            
    return old_service_ref._retn();
}
    
        
//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/unregister_local_service:1.0
//    
CORBA::Object_ptr Daemon_impl::unregister_local_service (const char * service_name)
     throw(CORBA::SystemException)
{
    std::cout << "<<Daemon_impl::unregister_local_service (" << service_name
              << ")>>" << std::endl;
              
    CORBA::Object_var service_ref = CORBA::Object::_nil();
    
    if (!strcmp (service_name, ""))
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMInvalidServiceName,
                               CORBA::COMPLETED_NO);
    }
    
    CDMW_MUTEX_GUARD(m_localServicesMutex);
    
    Services::iterator it = m_localServices.find(service_name);
    
    if (it != m_localServices.end())
    {
        service_ref = it->second;
        
        m_localServices.erase(service_name);                        
    }
    
    //unbind the corbaloc
    OrbSupport::OrbSupport::remove_corbaloc_binding (m_orb.in(), service_name);
    
    return service_ref._retn();
}
        
//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/resolve_local_service:1.0
//  
CORBA::Object_ptr Daemon_impl::resolve_local_service (const char * service_name)
      throw(CORBA::SystemException)
{
    CORBA::Object_var service_ref = CORBA::Object::_nil();
    
    if (!strcmp (service_name, ""))
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMInvalidServiceName,
                               CORBA::COMPLETED_NO);
    }
    
    CDMW_MUTEX_GUARD(m_localServicesMutex);
    
    Services::iterator it = m_localServices.find(service_name);
                            
    if (it != m_localServices.end())
    {
        service_ref = it->second;                  
    }
    
    return service_ref._retn();
}

        
//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/discover_global_service:1.0
//   
CORBA::Object_ptr Daemon_impl::discover_global_service (const char * service_name)
      throw(CORBA::SystemException)
{
    return m_serviceRegistration->get_service(service_name);
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/shutdown:1.0
//
void Daemon_impl::shutdown()
    throw(CORBA::SystemException)
{

#ifdef CDMW_POSIX

    // resets the process attributes to remain root
    OsSupport::OS::set_process_attributes(0, 0);

#endif

    try
    {
        OsSupport::OS::create_process("cdmw_host_shutdown.sh", "", "", NULL, NULL);
        LogMngr::logMessage(INF, MSG_ID_DAEMON_SHUTDOWN);
    }
    catch (...)
    {
        // the deployment of the daemon doesn't allow the operation
        LogMngr::logMessage("cdmw_host_shutdown.sh failed");
        throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
    }
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/cold_reboot:1.0
//
void Daemon_impl::cold_reboot()
    throw(CORBA::SystemException)
{

#ifdef CDMW_POSIX

    // resets the process attributes to remain root
    OsSupport::OS::set_process_attributes(0, 0);

#endif

    try
    {
        OsSupport::OS::create_process("cdmw_host_reboot.sh", "", "", NULL, NULL);
        LogMngr::logMessage(INF, MSG_ID_DAEMON_REBOOT);
    }
    catch (...)
    {
        // the deployment of the daemon doesn't allow the operation
        LogMngr::logMessage("cdmw_host_reboot.sh failed");
        throw CORBA::NO_PERMISSION(OrbSupport::NO_PERMISSION, CORBA::COMPLETED_NO);
    }
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Daemon/hot_reboot:1.0
//
void Daemon_impl::hot_reboot()
    throw(CORBA::SystemException)
{
    //TODO
    throw CORBA::NO_IMPLEMENT(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


//
// kill all processes which have been created by the agent of the specified application.
// The agent is not killed. 
//
void Daemon_impl::kill_application_processes(const std::string& applicationName) 
    throw (OutOfMemoryException,
           InternalErrorException)
{
    try {
        m_processLauncher->kill_all_processes(applicationName);

    } catch(const OutOfMemoryException&) {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_NO_MEMORY);
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    } catch (const InternalErrorException& ex) {
        LogMngr::logMessage(ERR, MSG_ID_DAEMON_INTERNAL, ex.get_info());
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }        
}


//
// Indicates whether the daemon manages the specified application agent.
//
bool Daemon_impl::hasApplicationAgent(const char* applicationName)
{

    CDMW_MUTEX_GUARD(m_agentsMutex);

    bool ret = false;

    std::string name(applicationName);

    if (m_agents.count(name) != 0) ret = true;

    return ret;

}


//
// Stores the information about the application agent.
//
void Daemon_impl::addApplicationAgent(const char* applicationName,
                                      ApplicationAgent_impl* agentImpl)
{

    CDMW_MUTEX_GUARD(m_agentsMutex);

    std::string name(applicationName);

    m_agents[name] = agentImpl;

}

//
// Removes the application agent.
//
ApplicationAgent_impl* Daemon_impl::removeApplicationAgent(const char* applicationName)
{

    ApplicationAgent_impl* pAgentImpl = NULL;

    CDMW_MUTEX_GUARD(m_agentsMutex);

    std::string name(applicationName);

    Agents::iterator it = m_agents.find(name);
    
    if (it != m_agents.end())
    {
        pAgentImpl = it->second;
        m_agents.erase(name);
    }

    return pAgentImpl;

}


bool Daemon_impl::hasSimpleProcess(OsSupport::OS::ProcessId processId)
{

    bool ret = false;

    CDMW_MUTEX_GUARD(m_simpleProcessesMutex);

    SimpleProcesses::iterator it = m_simpleProcesses.find(processId);
    
    if (it != m_simpleProcesses.end())
    {
        ret = true;
    }

    return ret;

}


bool Daemon_impl::hasSimpleProcess(const std::string& processName,
                                   OsSupport::OS::ProcessId& processId)
{

    bool ret = false;

    CDMW_MUTEX_GUARD(m_simpleProcessesMutex);

    for (SimpleProcesses::iterator it = m_simpleProcesses.begin(); it != m_simpleProcesses.end(); ++it)
    {

        if (!processName.compare(it->second.processName))
        {
            ret = true;
            processId = it->first;
            break;
        }

    }

    return ret;

}


void Daemon_impl::addSimpleProcess(OsSupport::OS::ProcessId processId,
        const std::string& processName)
    throw(OutOfMemoryException, InternalErrorException)
{

    CDMW_MUTEX_GUARD(m_simpleProcessesMutex);

    try
    {
        SimpleProcessInfo info;
        
        info.processName = processName;
        info.monitoringId = 0;
        
        m_simpleProcesses[processId] = info;
    }
    catch(const std::bad_alloc&)
    {
        CDMW_THROW(OutOfMemoryException);
    }

}


std::string Daemon_impl::removeSimpleProcess(OsSupport::OS::ProcessId processId,
                                             ::CdmwPlatformMngt::MonitoringId& monitoringId)
{

    std::string processName = "";
    monitoringId = 0;

    CDMW_MUTEX_GUARD(m_simpleProcessesMutex);

    SimpleProcesses::iterator it = m_simpleProcesses.find(processId);
    
    if (it != m_simpleProcesses.end())
    {
        processName = it->second.processName;
        monitoringId = it->second.monitoringId;
        m_simpleProcesses.erase(processId);
    }

    return processName;

}

bool Daemon_impl::removeSimpleProcess(const std::string& processName,
                                      OsSupport::OS::ProcessId& processId,
                                      ::CdmwPlatformMngt::MonitoringId& monitoringId)
{
    bool found = false;

    CDMW_MUTEX_GUARD(m_simpleProcessesMutex);

    for (SimpleProcesses::iterator it = m_simpleProcesses.begin(); it != m_simpleProcesses.end(); ++it)
    {
        if (!processName.compare(it->second.processName))
        {
            found = true;
            processId = it->first;
            monitoringId = it->second.monitoringId;
            break;
        }
    }

    // if process found
    if (found)
    {
        // remove the process from map
        m_simpleProcesses.erase (processId);
    }

    return found;
}

void Daemon_impl::set_process_monitoringId(const std::string& processName,
                                           ::CdmwPlatformMngt::MonitoringId id)
        throw(BadParameterException)
{
    bool found = false;

    CDMW_MUTEX_GUARD(m_simpleProcessesMutex);
    
    SimpleProcesses::iterator it;
    
    for (it = m_simpleProcesses.begin(); it != m_simpleProcesses.end(); ++it)
    {
        if (!processName.compare(it->second.processName))
        {
            found = true;
            break;
        }
    }
    
    if (found)
    {
        it->second.monitoringId = id;
    }    
    else
    {
        CDMW_THROW2(BadParameterException, processName, "not found");
    }
}



#ifdef CDMW_USE_FAULTTOLERANCE

//
// Get the Fault Notifier reference.
//
FT::FaultNotifier_ptr Daemon_impl::get_fault_notifier()
{
    FT::FaultNotifier_var faultNotifier = FT::FaultNotifier::_nil();
    
    // if already discovered, return it
    if (!CORBA::is_nil(m_faultNotifier.in()))
    {
        faultNotifier =
            FT::FaultNotifier::_duplicate (m_faultNotifier.in());
    } 
    else 
    {
        //try to find it in service registration map
        CORBA::Object_var obj = m_serviceRegistration->get_service("FaultNotifier");
    
        if (!CORBA::is_nil(obj.in()))
        {            
            faultNotifier = FT::FaultNotifier::_narrow(obj.in());
            
            // memorise it (duplicate by var copy)
            m_faultNotifier = faultNotifier;
        }
    }
             
    return faultNotifier._retn();   
}

#endif


} // End namespace PlatformMngt
} // End namespace Cdmw


