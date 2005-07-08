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


#include "platformadmin/SystemParser.hpp"
#include "platformadmin/MonitoringParser.hpp"
#include "platformadmin/PropertyParser.hpp"
#include "platformadmin/HostParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

HostParser::HostParser(
    const DOM_Node& hostParser )
: ConfigElementParser( hostParser ),
  PropertyContainerParser( hostParser )
{
}

HostMonitoringParser HostParser::get_monitoring() const
{
    try
    {
        DOM_Node monitoring_parser(
            get_child_element( "Host-monitoring", 0 ) );

        return HostMonitoringParser( monitoring_parser );
    }
    catch( const NotFoundException& )
    {
        SystemParser system( get_parent_element() );
        return system.get_host_monitoring();
    }
}

std::string HostParser::name() const
{
    return get_attribute_value( "Name" );
}

std::string HostParser::app_exec_path() const
{
    return get_attribute_value( "Agent-exec-path" );
}

std::string HostParser::agent_exec_path() const
{
    return get_attribute_value( "Agent-exec-path" );
}

std::string HostParser::probe_exec_path() const
{
    return get_attribute_value( "Probe-exec-path" );
}

std::string HostParser::probe_args() const
{
    return get_attribute_value( "Probe-args" );
}

std::string HostParser::probe_run_dir() const
{
    return get_attribute_value( "Probe-run-dir" );
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

