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


#include "platformadmin/LifeCycleParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

/**
 * InitStepParser class.
 */
LifeCycleParser::InitStepParser::InitStepParser(
    const DOM_Node& initStepParser )
: ConfigElementParser( initStepParser )
{
}

std::string LifeCycleParser::InitStepParser::name() const
{
    return get_attribute_value( "Name" );
}

unsigned long LifeCycleParser::InitStepParser::timeout() const
{
    std::string timeout = get_attribute_value( "Timeout" );
    return atol( timeout.c_str() );
}

/**
 * RunStepParser class.
 */
LifeCycleParser::RunStepParser::RunStepParser(
    const DOM_Node& runStepParser )
: ConfigElementParser( runStepParser )
{
}

unsigned long LifeCycleParser::RunStepParser::timeout() const
{
    std::string timeout = get_attribute_value( "Timeout" );
    return atol( timeout.c_str() );
}

/**
 * StopStepParser class.
 */
LifeCycleParser::StopStepParser::StopStepParser(
    const DOM_Node& stopStepParser )
: ConfigElementParser( stopStepParser )
{
}

unsigned long LifeCycleParser::StopStepParser::timeout() const
{
    std::string timeout = get_attribute_value( "Timeout" );
    return atol( timeout.c_str() );
}

/**
 * LifeCycleParser class.
 */
LifeCycleParser::LifeCycleParser(
    const DOM_Node& lifeCycleParser )
: ConfigElementParser( lifeCycleParser )
{
}

unsigned int LifeCycleParser::get_init_step_count() const
{
    return get_child_count( "Init-step" );
}

std::string LifeCycleParser::get_init_step_name(
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

unsigned long LifeCycleParser::get_init_step_timeout(
    unsigned int initStepIndex ) const
{
    InitStepParser init_step_parser(
        get_child_element( "Init-step", initStepIndex ) );

    return init_step_parser.timeout();
}

unsigned long LifeCycleParser::get_run_timeout() const
{
    RunStepParser run_step_parser(
        get_child_element( "Run-timeout", 0 ) );

    return run_step_parser.timeout();
}

unsigned long LifeCycleParser::get_stop_timeout() const
{
    StopStepParser stop_step_parser(
        get_child_element( "Stop-timeout", 0 ) );

    return stop_step_parser.timeout();
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

