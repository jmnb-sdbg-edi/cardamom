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


#include <sstream>
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/ossupport/OS.hpp"

namespace Cdmw {        // Begin namespace Cdmw
namespace PlatformMngt { // Begin namespace PlatformMngt

// set static members
CORBA::ORB_var Configuration::M_orb = CORBA::ORB::_nil();
CdmwPlatformMngt::Timeout Configuration::M_timeout = 0;
long Configuration::M_daemon_port = -1;
bool Configuration::M_fault_tolerant = false;
bool Configuration::M_first_primary = false;
bool Configuration::M_primary = false;
bool Configuration::M_synchronising = false;
CORBA::ULong Configuration::M_managed_element_datastore_id = 0;
CORBA::ULong Configuration::M_entity_datastore_id = 0;
CORBA::ULong Configuration::M_host_datastore_id = 0;
CORBA::ULong Configuration::M_system_datastore_id = 0;
CORBA::ULong Configuration::M_application_datastore_id = 0;
CORBA::ULong Configuration::M_process_datastore_id = 0;
CORBA::ULong Configuration::M_system_observer_registration_datastore_id = 0;
CORBA::ULong Configuration::M_event_report_datastore_id = 0;
CORBA::ULong Configuration::M_event_notifier_datastore_id = 0;
CORBA::ULong Configuration::M_service_defs_datastore_id = 0;
CORBA::ULong Configuration::M_runset_datastore_id = 0;

const char* Configuration::M_centralised_option = "--centralised";
const char* Configuration::M_distributed_option = "--distributed";
const char* Configuration::M_event_timeout = "--event-timeout";
const char* Configuration::M_creation_timeout_option = "--creation-timeout";
const char* Configuration::M_daemon_port_option = "--daemon_port";
const char* Configuration::M_fault_manager_corbaloc_option = "--FaultManagerRegistration";

std::string Configuration::M_FTConverter_group_name = "SMG_FTConverter";
std::string Configuration::M_FTProcessObserver_group_name = "SMG_FTProcessObserver";
std::string Configuration::M_FTServiceBroker_group_name = "SMG_FTServiceBroker";
std::string Configuration::M_SystemAccessor_group_name = "SMG_SystemAccessor";
std::string Configuration::M_HostAccessor_group_name = "SMG_HostAccessor";
std::string Configuration::M_ApplicationAccessor_group_name = "SMG_ApplicationAccessor";
std::string Configuration::M_ProcessAccessor_group_name = "SMG_ProcessAccessor";
std::string Configuration::M_EntityAccessor_group_name = "SMG_EntityAccessor";


const CORBA::ORB_ptr Configuration::Get_orb()
{
    return M_orb.in();
}


CdmwPlatformMngt::Timeout Configuration::Get_timeout()
{
    return M_timeout;
}


long Configuration::Get_daemon_port()
{
    return M_daemon_port;
}

    

bool Configuration::Is_fault_tolerant()
{
    return M_fault_tolerant;
}

bool Configuration::Is_first_primary()
{
    return M_first_primary;
}    
    
bool Configuration::Is_primary()
{
    return M_primary;
}
    

bool Configuration::Is_synchronising()
{
    return M_synchronising;
}

CORBA::ULong Configuration::Get_managed_element_datastore_id()
{
    return M_managed_element_datastore_id;
}

CORBA::ULong Configuration::Get_entity_datastore_id()
{
    return M_entity_datastore_id;
}


CORBA::ULong Configuration::Get_host_datastore_id()
{
    return M_host_datastore_id;
}


CORBA::ULong Configuration::Get_system_datastore_id()
{
    return M_system_datastore_id;
}


CORBA::ULong Configuration::Get_application_datastore_id()
{
    return M_application_datastore_id;
}


CORBA::ULong Configuration::Get_process_datastore_id()
{
    return M_process_datastore_id;
}


CORBA::ULong Configuration::Get_system_observer_registration_datastore_id()
{
    return M_system_observer_registration_datastore_id;
}


CORBA::ULong Configuration::Get_event_report_datastore_id()
{
    return M_event_report_datastore_id;
}


CORBA::ULong Configuration::Get_event_notifier_datastore_id()
{
    return M_event_notifier_datastore_id;
}


CORBA::ULong Configuration::Get_service_defs_datastore_id()
{
    return M_service_defs_datastore_id;
}


CORBA::ULong Configuration::Get_runset_datastore_id()
{
    return M_runset_datastore_id;
}

    
std::string Configuration::Get_FTConverter_group_name()
{
    return M_FTConverter_group_name;
}

    
std::string Configuration::Get_FTProcessObserver_group_name()
{
    return M_FTProcessObserver_group_name;
}
    
std::string Configuration::Get_FTServiceBroker_group_name()
{
    return M_FTServiceBroker_group_name;
}
    
std::string Configuration::Get_SystemAccessor_group_name()
{
    return M_SystemAccessor_group_name;
}

std::string Configuration::Get_HostAccessor_group_name()
{
    return M_HostAccessor_group_name;
}
    
std::string Configuration::Get_ApplicationAccessor_group_name()
{
    return M_ApplicationAccessor_group_name;
}
    
std::string Configuration::Get_ProcessAccessor_group_name()
{
    return M_ProcessAccessor_group_name;
}

std::string Configuration::Get_EntityAccessor_group_name()
{
    return M_EntityAccessor_group_name;
}

void Configuration::Set_orb (CORBA::ORB_ptr orb)
{
    M_orb = CORBA::ORB::_duplicate(orb);
}


void Configuration::Set_timeout(const CdmwPlatformMngt::Timeout timeout)
{
    M_timeout = timeout;
}

void Configuration::Set_daemon_port(long port)
{
    M_daemon_port = port;
}

void Configuration::Set_fault_tolerant()
{
    M_fault_tolerant = true;
}

    
void Configuration::Set_first_primary()
{
    M_first_primary = true;
}

void Configuration::Set_primary()
{
    M_primary = true;
}
    

void Configuration::Set_backup()
{
    M_primary = false;
}
    

void Configuration::Set_synchronising()
{
    M_synchronising = true;
}

void Configuration::Set_synchronised()
{
    M_synchronising = false;
}

    
    
CdmwPlatformMngt::Daemon_ptr Configuration::Get_daemon (
                            const char* hostName,
                            const CdmwPlatformMngt::Port systemPort)
   throw (CdmwPlatformMngt::HostNotFound,
          CdmwPlatformMngt::HostNotReachable,
          CORBA::SystemException)
{
    try
    {
        std::ostringstream daemonCorbaloc;

        daemonCorbaloc 
            << "corbaloc::" << hostName
            <<  ":" << systemPort
            <<  "/" << CdmwPlatformMngt::DAEMON_NAME;


        CORBA::Object_var obj =
            M_orb->string_to_object (daemonCorbaloc.str().c_str());
            
        if (CORBA::is_nil (obj.in()))
        {
            throw CdmwPlatformMngt::HostNotFound();
        }
        
        
        CdmwPlatformMngt::Daemon_var daemon =
            CdmwPlatformMngt::Daemon::_narrow (obj.in());
            
        if (CORBA::is_nil (daemon.in()))
        {
            throw CdmwPlatformMngt::HostNotFound();
        }

        return daemon._retn();
    }
    catch (const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::HostNotReachable();
    }
}

void Configuration::Set_managed_element_datastore_id(CORBA::ULong datastore_id)
{
    M_managed_element_datastore_id = datastore_id;
}

void Configuration::Set_entity_datastore_id(CORBA::ULong datastore_id)
{
    M_entity_datastore_id = datastore_id;
}


void Configuration::Set_host_datastore_id(CORBA::ULong datastore_id)
{
    M_host_datastore_id = datastore_id;
}


void Configuration::Set_system_datastore_id(CORBA::ULong datastore_id)
{
    M_system_datastore_id = datastore_id;
}


void Configuration::Set_application_datastore_id(CORBA::ULong datastore_id)
{
    M_application_datastore_id = datastore_id;
}


void Configuration::Set_process_datastore_id(CORBA::ULong datastore_id)
{
    M_process_datastore_id = datastore_id;
}


void Configuration::Set_system_observer_registration_datastore_id(CORBA::ULong datastore_id)
{
    M_system_observer_registration_datastore_id = datastore_id;
}


void Configuration::Set_event_report_datastore_id(CORBA::ULong datastore_id)
{
    M_event_report_datastore_id = datastore_id;
}


void Configuration::Set_event_notifier_datastore_id(CORBA::ULong datastore_id)
{
    M_event_notifier_datastore_id = datastore_id;
}


void Configuration::Set_service_defs_datastore_id(CORBA::ULong datastore_id)
{
    M_service_defs_datastore_id = datastore_id;
}


void Configuration::Set_runset_datastore_id(CORBA::ULong datastore_id)
{
    M_runset_datastore_id = datastore_id;
}


} // End namespace PlatformMngt
} // End namespace Cdmw

