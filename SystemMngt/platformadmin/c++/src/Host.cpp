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


#include "platformadmin/Host.hpp"
#include "platformadmin/ConfigFileParser.hpp"
#include "platformadmin/SystemParser.hpp"
#include "platformadmin/MonitoringParser.hpp"
#include "platformadmin/HostParser.hpp"
#include "platformadmin/PropertyParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

Host::Host(
    CdmwPlatformMngt::HostProxy_ptr host )
: PropertyContainer( host )
{
    m_host = CdmwPlatformMngt::HostProxy::_duplicate( host );
}

Host::~Host()
{
}

CdmwPlatformMngt::HostDef* Host::get_definition(
    const HostParser& hostParser )
{
    try
    {
        CdmwPlatformMngt::HostDef_var hostDef
            = new CdmwPlatformMngt::HostDef();

        // Get the host's name
        hostDef->host_name = CORBA::string_dup( hostParser.name().c_str() );

        // Set the application process's info
        hostDef->platform_application_exe
            = CORBA::string_dup( hostParser.app_exec_path().c_str() );

        // Set the agent's info
        hostDef->application_agent_exe
            = CORBA::string_dup( hostParser.agent_exec_path().c_str() );

        // Set the probe's info
        // TODO: what about the process_port ??
        hostDef->probe_process_info.process_exe
            = CORBA::string_dup( hostParser.probe_exec_path().c_str() );
        hostDef->probe_process_info.process_args
            = CORBA::string_dup( hostParser.probe_args().c_str() );
        hostDef->probe_process_info.working_directory
            = CORBA::string_dup( hostParser.probe_run_dir().c_str() );
        
        // Set the monitoring's info
        hostDef->daemon_monitoring_info.monitoring_interval
            = hostParser.get_monitoring().pull_interval();
        hostDef->daemon_monitoring_info.monitoring_timeout
            = hostParser.get_monitoring().pull_timeout();

        return hostDef._retn();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

