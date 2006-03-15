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


#include "platformadmin/SystemParser.hpp"
#include "platformadmin/TimeoutParser.hpp"
#include "platformadmin/MonitoringParser.hpp"
#include "platformadmin/ServiceParser.hpp"
#include "platformadmin/ApplicationParser.hpp"
#include "platformadmin/HostParser.hpp"
#include "platformadmin/RunSetParser.hpp"
#include "platformadmin/GraphParser.hpp"
#include "platformadmin/PropertyParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

SystemParser::SystemParser(
    const DOM_Node& systemParser )
: ConfigElementParser( systemParser ),
  ServiceContainerParser( systemParser ),
  EntityContainerParser( systemParser )
{
}

std::string SystemParser::name() const
{
    return get_attribute_value( "Name" ).c_str();
}

unsigned long SystemParser::port() const
{
    return atol( get_attribute_value( "Port" ).c_str() );
}

TimeoutParser SystemParser::get_timeout() const
{
    DOM_Node timeout_parser( get_child_element( "Timeout", 0 ) );
    return TimeoutParser( timeout_parser );
}

HostMonitoringParser SystemParser::get_host_monitoring() const
{
    DOM_Node host_monitoring_parser(
        get_child_element( "Host-monitoring", 0 ) );

    return HostMonitoringParser( host_monitoring_parser );
}

ApplicationMonitoringParser SystemParser::get_application_monitoring() const
{
    DOM_Node application_monitoring_parser(
        get_child_element( "App-monitoring", 0 ) );

    return ApplicationMonitoringParser( application_monitoring_parser );
}

ProcessMonitoringParser SystemParser::get_proc_monitoring() const
{
    DOM_Node proc_monitoring_parser(
        get_child_element( "Proc-monitoring", 0 ) );

    return ProcessMonitoringParser( proc_monitoring_parser );
}

unsigned int SystemParser::get_host_count() const
{
    return get_child_count( "Host" );
}

HostParser SystemParser::get_host(
    unsigned int hostIndex ) const
{
    DOM_Node host_parser(
        get_child_element( "Host", hostIndex ) );

    return HostParser( host_parser );
}

unsigned int SystemParser::get_application_count() const
{
    return get_child_count( "Application" );
}

ApplicationParser SystemParser::get_application(
    unsigned int applicationIndex ) const
{
    DOM_Node application_parser(
        get_child_element( "Application", applicationIndex ) );

    return ApplicationParser( application_parser );
}

unsigned int SystemParser::get_run_set_count() const
{
    return get_child_count( "Run-set" );
}

RunSetParser SystemParser::get_run_set(
    unsigned int runSetIndex ) const
{
    DOM_Node run_set_parser(
        get_child_element( "Run-set", runSetIndex ) );

    return RunSetParser( run_set_parser );
}

GraphParser SystemParser::get_init_graph() const
{
    DOM_Node graph_parser( get_child_element( "Init-graph", 0 ) );
    return GraphParser( graph_parser );
}

GraphParser SystemParser::get_stop_graph() const
{
    DOM_Node graph_parser( get_child_element( "Stop-graph", 0 ) );
    return GraphParser( graph_parser );
}

PropertyParser SystemParser::get_property(unsigned int propertyIndex) const
{
    DOM_Node property_parser(get_child_element("Property", propertyIndex ));
    return PropertyParser(property_parser);
}

unsigned int SystemParser::get_property_count() const
{ 
     return get_child_count("Property"); 
};


} // End namespace PlatformAdmin
} // End namespace Cdmw

