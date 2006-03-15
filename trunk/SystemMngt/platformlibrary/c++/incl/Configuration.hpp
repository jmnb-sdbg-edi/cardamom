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


#ifndef INCL_PLATFORMMNGT_CONFIGURATION_HPP
#define INCL_PLATFORMMNGT_CONFIGURATION_HPP


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"

#include <string>

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformMngt { // Begin namespace PlatformMngt

/**
 *Purpose: Static class for configuration data
 *<p>
 */
class Configuration
{
    
public:

    /**
     * Options of the SystemMgnt process
     */
    static const char* M_centralised_option;
    static const char* M_distributed_option;
    static const char* M_event_timeout;
    static const char* M_creation_timeout_option;
    static const char* M_daemon_port_option;
    static const char* M_fault_manager_corbaloc_option;


    /**
     * Get the ORB. !!!DoNot deallocate the return value!!
     */
    static
    const CORBA::ORB_ptr Get_orb();

    /**
     * Get the timeout needed for technical reasons.
     */
    static
    CdmwPlatformMngt::Timeout Get_timeout();
    
    /**
     * Get the daemon port
     */
    static
    long Get_daemon_port();
    
    /**
     * Check if the process is fault tolerant
     */
    static
    bool Is_fault_tolerant();
    
    /**
     * Check if the process is primary.
     */
    static
    bool Is_first_primary();
    
    /**
     * Check if the process is primary.
     */
    static
    bool Is_primary();
    
    /**
     * Check if the process is synchronising for primary setting.
     */
    static
    bool Is_synchronising();

    /*
     * Set the ManagedElementDataStore ID
     */
    static 
    CORBA::ULong Get_managed_element_datastore_id();

    /*
     * Set the EntityDataStore ID
     */
    static 
    CORBA::ULong Get_entity_datastore_id();

    /*
     * Set the HostDataStore ID
     */
    static 
    CORBA::ULong Get_host_datastore_id();

    /*
     * Set the SystemDataStore ID
     */
    static 
    CORBA::ULong Get_system_datastore_id();

    /*
     * Set the ApplicationDataStore ID
     */
    static 
    CORBA::ULong Get_application_datastore_id();

    /*
     * Set the ProcessDataStore ID
     */
    static 
    CORBA::ULong Get_process_datastore_id();

    /*
     * Set the SystemObserverRegistrationDataStore ID
     */
    static 
    CORBA::ULong Get_system_observer_registration_datastore_id();

    /*
     * Set the EventReportDataStore ID
     */
    static 
    CORBA::ULong Get_event_report_datastore_id();

    /*
     * Set the EventNotifierDataStore ID
     */
    static 
    CORBA::ULong Get_event_notifier_datastore_id();

    /*
     * Set the ServiceDefsDataStore ID
     */
    static 
    CORBA::ULong Get_service_defs_datastore_id();

    /*
     * Set the RunsetDataStore ID
     */
    static 
    CORBA::ULong Get_runset_datastore_id();

    
    /**
     * Get Group name for FTConverter object.
     */
    static
    std::string Get_FTConverter_group_name();
    
    /**
     * Get Group name for FTProcessObserver object.
     */
    static
    std::string Get_FTProcessObserver_group_name();
    
    /**
     * Get Group name for FTServiceBroker object.
     */
    static
    std::string Get_FTServiceBroker_group_name();
    
    /**
     * Get Group name for SystemAccessor object.
     */
    static
    std::string Get_SystemAccessor_group_name();
    
    /**
     * Get Group name for HostAccessor object.
     */
    static
    std::string Get_HostAccessor_group_name();
    
    /**
     * Get Group name for ApplicationAccessor object.
     */
    static
    std::string Get_ApplicationAccessor_group_name();
    
    /**
     * Get Group name for ProcessAccessor object.
     */
    static
    std::string Get_ProcessAccessor_group_name();
    
    /**
     * Get Group name for EntityAccessor object.
     */
    static
    std::string Get_EntityAccessor_group_name();
    
        
    /**
     * Set the ORB.
     */
    static
    void Set_orb (CORBA::ORB_ptr orb);

    /**
     * Set the timeout needed for technical reasons.
     */
    static
    void Set_timeout(const CdmwPlatformMngt::Timeout timeout);
    
    /**
     * Set the daemon port
     */
    static
    void Set_daemon_port(long port);
    
    /**
     * Set the process fault tolerant.
     */
    static
    void Set_fault_tolerant();
    
    /**
     * Set the process as first primary.
     */
    static
    void Set_first_primary();
    
    /**
     * Set the process as primary.
     */
    static
    void Set_primary();
    
    /**
     * Set the process as backup.
     */
    static
    void Set_backup();
    
    /**
     * Set the process synchronising to become primary.
     */
    static
    void Set_synchronising();
    
    /**
     * Set the process synchronised as primary.
     */
    static
    void Set_synchronised();


    /*
     * Set the ManagedElementDataStore ID
     */
    static 
    void Set_managed_element_datastore_id(CORBA::ULong datastore_id);

    /*
     * Set the EntityDataStore ID
     */
    static 
    void Set_entity_datastore_id(CORBA::ULong datastore_id);

    /*
     * Set the HostDataStore ID
     */
    static 
    void Set_host_datastore_id(CORBA::ULong datastore_id);

    /*
     * Set the SystemDataStore ID
     */
    static 
    void Set_system_datastore_id(CORBA::ULong datastore_id);

    /*
     * Set the ApplicationDataStore ID
     */
    static 
    void Set_application_datastore_id(CORBA::ULong datastore_id);

    /*
     * Set the ProcessDataStore ID
     */
    static 
    void Set_process_datastore_id(CORBA::ULong datastore_id);

    /*
     * Set the SystemObserverRegistrationDataStore ID
     */
    static 
    void Set_system_observer_registration_datastore_id(CORBA::ULong datastore_id);

    /*
     * Set the EventReportDataStore ID
     */
    static 
    void Set_event_report_datastore_id(CORBA::ULong datastore_id);

    /*
     * Set the EventNotifierDataStore ID
     */
    static 
    void Set_event_notifier_datastore_id(CORBA::ULong datastore_id);

    /*
     * Set the ServiceDefsDataStore ID
     */
    static 
    void Set_service_defs_datastore_id(CORBA::ULong datastore_id);

    /*
     * Set the RunsetDataStore ID
     */
    static 
    void Set_runset_datastore_id(CORBA::ULong datastore_id);

public:

    /**
     *Purpose:
     * Returns the specified (remote or not) daemon.
     * 
     *@param host_name  the host of the remote daemon.
     *@param systemPort the port to contact daemon
     *
     *@exception HostNotFound if the host cannot be found.
     *@exception HostNotReachable if the host doesn't respond.
     *@exception CORBA::SystemException
     */
    static
    CdmwPlatformMngt::Daemon_ptr Get_daemon (
                                    const char* hostName,
                                    const CdmwPlatformMngt::Port systemPort)
            throw (CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::HostNotReachable,
                   CORBA::SystemException);
                   
                   
private:

    /**
     * The ORB.
     */
    static
    CORBA::ORB_var M_orb;

    /**
     * The timeout needed for technical reasons.
     */
    static
    CdmwPlatformMngt::Timeout M_timeout;
    
    /**
     * The daemon port
     */
    static
    long M_daemon_port;
    
    /**
     * The fault tolerant setting.
     */
    static
    bool M_fault_tolerant;
    
    /**
     * The ft first primary setting.
     */
    static
    bool M_first_primary;
    
    /**
     * The ft primary setting.
     */
    static
    bool M_primary;
    
    /**
     * The ft synchronisation setting.
     */
    static
    bool M_synchronising;


    /**
     * Datastore ID of all SystemMngt datastores.
     */
    static CORBA::ULong M_managed_element_datastore_id;
    static CORBA::ULong M_entity_datastore_id;
    static CORBA::ULong M_host_datastore_id;
    static CORBA::ULong M_system_datastore_id;
    static CORBA::ULong M_application_datastore_id;
    static CORBA::ULong M_process_datastore_id;
    static CORBA::ULong M_system_observer_registration_datastore_id;
    static CORBA::ULong M_event_report_datastore_id;
    static CORBA::ULong M_event_notifier_datastore_id;
    static CORBA::ULong M_service_defs_datastore_id;
    static CORBA::ULong M_runset_datastore_id;
    
    
    
    /**
     * The Group name for FTConverter object.
     */
    static
    std::string M_FTConverter_group_name;
    
    /**
     * The Group name for FTProcessObserver object.
     */
    static
    std::string M_FTProcessObserver_group_name;
    
    /**
     * The Group name for FTServiceBroker object.
     */
    static
    std::string M_FTServiceBroker_group_name;
    
    /**
     * The Group name for SystemAccessor object.
     */
    static
    std::string M_SystemAccessor_group_name;
    
    /**
     * The Group name for HostAccessor object.
     */
    static
    std::string M_HostAccessor_group_name;
    
    /**
     * The Group name for ApplicationAccessor object.
     */
    static
    std::string M_ApplicationAccessor_group_name;
    
    /**
     * The Group name for ProcessAccessor object.
     */
    static
    std::string M_ProcessAccessor_group_name;
    
    /**
     * The Group name for EntityAccessor object.
     */
    static
    std::string M_EntityAccessor_group_name;
    
    
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_CONFIGURATION_HPP

