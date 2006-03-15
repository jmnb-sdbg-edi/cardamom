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


#include "platformadmin/MonitoringParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

/**
 * Monitoring class.
 */
MonitoringParser::MonitoringParser(
    const DOM_Node& monitoringParser )
: ConfigElementParser( monitoringParser )
{
}

CdmwPlatformMngt::MonitoringModel MonitoringParser::model() const
{
    std::string model = get_attribute_value( "Model" );

    if( model == "NO_MONITORING" )
        return CdmwPlatformMngt::NO_MONITORING;
    else if( model == "PULL_MONITORING")
        return CdmwPlatformMngt::PULL_MONITORING_MODEL;
    else if( model == "PUSH_MONITORING")
        return CdmwPlatformMngt::PUSH_MONITORING_MODEL;
    else if( model == "FULL_MONITORING" )
        return CdmwPlatformMngt::FULL_MONITORING_MODEL;
    else
        CDMW_THROW1(
            ConfigException,
            std::string( "Invalid monitoring model" ) );
}

unsigned long MonitoringParser::pull_interval() const
{
    return atol( get_attribute_value( "Pull-interval" ).c_str() );
}

unsigned long MonitoringParser::pull_timeout() const
{
    return atol( get_attribute_value( "Pull-timeout" ).c_str() );
}

unsigned long MonitoringParser::push_interval() const
{
    return atol( get_attribute_value( "Push-interval" ).c_str() );
}

unsigned long MonitoringParser::push_timeout() const
{
    return atol( get_attribute_value( "Push-timeout" ).c_str() );
}

/**
 * HostMonitoring class.
 */
HostMonitoringParser::HostMonitoringParser(
    const DOM_Node& monitoringParser )
: MonitoringParser( monitoringParser )
{
}

/**
 * ApplicationMonitoring class.
 */
ApplicationMonitoringParser::ApplicationMonitoringParser(
    const DOM_Node& monitoringParser )
: MonitoringParser( monitoringParser )
{
}

/**
 * ProcessMonitoring class.
 */
ProcessMonitoringParser::ProcessMonitoringParser(
    const DOM_Node& monitoringParser )
: MonitoringParser( monitoringParser )
{
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

