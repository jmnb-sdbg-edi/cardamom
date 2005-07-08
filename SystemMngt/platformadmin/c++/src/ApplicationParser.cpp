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
#include "platformadmin/ApplicationParser.hpp"
#include "platformadmin/TimeoutParser.hpp"
#include "platformadmin/MonitoringParser.hpp"
#include "platformadmin/ProcessParser.hpp"
#include "platformadmin/ServiceParser.hpp"
#include "platformadmin/EntityParser.hpp"
#include "platformadmin/PropertyParser.hpp"
#include "platformadmin/GraphParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

ApplicationParser::ApplicationParser(
    const DOM_Node& applicationParser )
: ConfigElementParser( applicationParser ),
  ServiceContainerParser( applicationParser ),
  PropertyContainerParser( applicationParser ),
  EntityContainerParser( applicationParser )
{
}

std::string ApplicationParser::host_name() const
{
    return get_attribute_value( "Host-name" ).c_str();
}

std::string ApplicationParser::name() const
{
    return get_attribute_value( "Name" ).c_str();
}

unsigned long ApplicationParser::app_port() const
{
    return atol( get_attribute_value( "App-port" ).c_str() );
}

std::string ApplicationParser::app_args() const
{
    return get_attribute_value( "App-args" ).c_str();
}

std::string ApplicationParser::app_run_dir() const
{
    return get_attribute_value( "App-run-dir" ).c_str();
}

unsigned long ApplicationParser::agent_port() const
{
    return atol( get_attribute_value( "Agent-port" ).c_str() );
}

std::string ApplicationParser::agent_args() const
{
    return get_attribute_value( "Agent-args" ).c_str();
}

std::string ApplicationParser::agent_run_dir() const
{
    return get_attribute_value( "Agent-run-dir" ).c_str();
}

TimeoutParser ApplicationParser::get_timeout() const
{
    try
    {
        DOM_Node timeout_parser( get_child_element( "Timeout", 0 ) ); 
        return TimeoutParser( timeout_parser );
    }
    catch( const NotFoundException& )
    {
        SystemParser system( get_parent_element() ); 
        return system.get_timeout();
    }
}

ApplicationMonitoringParser ApplicationParser::get_monitoring() const
{
    try
    {
        DOM_Node application_monitoring_parser(
            get_child_element( "App-monitoring", 0 ) );

        return ApplicationMonitoringParser(
            application_monitoring_parser );
    }
    catch( const NotFoundException& )
    {
        SystemParser system( get_parent_element() );
        return system.get_application_monitoring();
    }
}

ProcessMonitoringParser ApplicationParser::get_proc_monitoring() const
{
    try
    {
        DOM_Node proc_monitoring_parser(
            get_child_element( "Proc-monitoring", 0 ) );

        return ProcessMonitoringParser(
            proc_monitoring_parser );
    }
    catch( const NotFoundException& )
    {
        SystemParser system( get_parent_element() );
        return system.get_proc_monitoring();
    }
}

unsigned int ApplicationParser::get_managed_process_count() const
{
    return get_child_count( "Managed-process" );
}

ProcessParser ApplicationParser::get_managed_process(
    unsigned int processIndex ) const
{
    DOM_Node process_Parser(
        get_child_element( "Managed-process", processIndex ) );
    return ProcessParser( process_Parser );
}

unsigned int ApplicationParser::get_unmanaged_process_count() const
{
    return get_child_count( "Unmanaged-process" );
}

ProcessParser ApplicationParser::get_unmanaged_process(
    unsigned int processIndex ) const
{
    DOM_Node process_Parser(
        get_child_element( "Unmanaged-process", processIndex ) );
    return ProcessParser( process_Parser );
}

GraphParser ApplicationParser::get_init_graph() const
{
    DOM_Node graph_Parser( get_child_element( "Init-graph", 0 ) );
    return GraphParser( graph_Parser );
}

GraphParser ApplicationParser::get_stop_graph() const
{
    DOM_Node graph_Parser( get_child_element( "Stop-graph", 0 ) );
    return GraphParser( graph_Parser );
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

