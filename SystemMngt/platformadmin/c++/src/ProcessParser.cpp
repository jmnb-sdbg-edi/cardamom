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


#include "platformadmin/ApplicationParser.hpp"
#include "platformadmin/ProcessParser.hpp"
#include "platformadmin/TimeoutParser.hpp"
#include "platformadmin/MonitoringParser.hpp"
#include "platformadmin/ServiceParser.hpp"
#include "platformadmin/EntityParser.hpp"
#include "platformadmin/PropertyParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

/**
 * InitStepParser class.
 */
ProcessParser::InitStepParser::InitStepParser(
    const DOM_Node& initStepParser )
: ConfigElementParser( initStepParser )
{
}

std::string ProcessParser::InitStepParser::name() const
{
    return get_attribute_value( "Name" );
}

unsigned long ProcessParser::InitStepParser::timeout() const
{
    std::string timeout = get_attribute_value( "Timeout" );

    if( timeout == "" )
        throw NotFoundException();
        
    return atol( timeout.c_str() );
}

/**
 * RunStepParser class.
 */
ProcessParser::RunStepParser::RunStepParser(
    const DOM_Node& runStepParser )
: ConfigElementParser( runStepParser )
{
}

unsigned long ProcessParser::RunStepParser::timeout() const
{
    std::string timeout = get_attribute_value( "Timeout" );
    return atol( timeout.c_str() );
}

/**
 * StopStepParser class.
 */
ProcessParser::StopStepParser::StopStepParser(
    const DOM_Node& stopStepParser )
: ConfigElementParser( stopStepParser )
{
}

unsigned long ProcessParser::StopStepParser::timeout() const
{
    std::string timeout = get_attribute_value( "Timeout" );
    return atol( timeout.c_str() );
}

/**
 * ProcessParser class.
 */
ProcessParser::ProcessParser(
    const DOM_Node& processParser )
: ConfigElementParser( processParser ),
  ServiceContainerParser( processParser ),
  PropertyContainerParser( processParser ),
  EntityContainerParser( processParser ),
  ActivityPointContainerParser( processParser )
{
}

std::string ProcessParser::name() const
{
    return get_attribute_value( "Name" );
}

std::string ProcessParser::host_name() const
{
    return get_attribute_value( "Host-name" );
}

CdmwPlatformMngt::ProcessType ProcessParser::type() const
{
    std::string type = get_attribute_value( "Type" );

    if( type == "MANAGED-PROCESS")
        return CdmwPlatformMngt::MANAGED_PROCESS;
    else if( type == "SERVICE-PROVIDER")
        return CdmwPlatformMngt::SERVICE_PROVIDER;
    else if( type == "COMPONENT-SERVER")
        return CdmwPlatformMngt::COMPONENT_SERVER;
    else
        return CdmwPlatformMngt::UNMANAGED_PROCESS;
}

bool ProcessParser::is_autoended() const
{
    return get_attribute_value( "Autoended" ) == "TRUE";
}

std::string ProcessParser::executable_path() const
{
    return get_attribute_value( "Exec-path" );
}

unsigned long ProcessParser::port() const
{
    return atol( get_attribute_value( "Port" ).c_str() );
}

std::string ProcessParser::arguments() const
{
    return get_attribute_value( "Args" );
}

std::string ProcessParser::run_directory() const
{
    return get_attribute_value( "Run-dir" );
}

TimeoutParser ProcessParser::get_timeout() const
{
    try
    {
        DOM_Node timeout_parser( get_child_element( "Timeout", 0 ) ); 
        return TimeoutParser( timeout_parser );
    }
    catch( const NotFoundException& e )
    {
        ApplicationParser application( get_parent_element() ); 
        return application.get_timeout();
    }
}

unsigned int ProcessParser::get_init_step_count() const
{
    return get_child_count( "Init-step" );
}

std::string ProcessParser::get_init_step_name(
    unsigned int initStepIndex ) const
{
    try
    {
        InitStepParser init_step_parser(
            get_child_element( "Init-step", initStepIndex ) );

        return init_step_parser.name();
    }
    catch( const NotFoundException& e )
    {
        return std::string( "" );
    }
}

unsigned long ProcessParser::get_init_step_timeout(
    unsigned int initStepIndex ) const
{
    try
    {
        InitStepParser init_step_parser(
            get_child_element( "Init-step", initStepIndex ) );

        return init_step_parser.timeout();
    }
    catch( const NotFoundException& e )
    {
        ApplicationParser application( get_parent_element() ); 
        return application.get_timeout().init();
    }
}

unsigned long ProcessParser::get_run_timeout() const
{
    try
    {
        RunStepParser run_step_parser(
            get_child_element( "Run-timeout", 0 ) );

        return run_step_parser.timeout();
    }
    catch( const NotFoundException& e )
    {
        ApplicationParser application( get_parent_element() ); 
        return application.get_timeout().run();
    }
}

unsigned long ProcessParser::get_stop_timeout() const
{
    try
    {
        StopStepParser stop_step_parser(
            get_child_element( "Stop-timeout", 0 ) );

        return stop_step_parser.timeout();
    }
    catch( const NotFoundException& e )
    {
        ApplicationParser application( get_parent_element() ); 
        return application.get_timeout().stop();
    }
}

ProcessMonitoringParser ProcessParser::get_monitoring() const
{
    try
    {
        DOM_Node monitoring_parser(
            get_child_element( "Proc-monitoring", 0 ) );

        return ProcessMonitoringParser( monitoring_parser );
    }
    catch( const NotFoundException& )
    {
        ApplicationParser application( get_parent_element() );
        return application.get_proc_monitoring();
    }
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

