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


#include <dom/DOMString.hpp>
#include <parsers/DOMParser.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>

#include "Foundation/ossupport/OS.hpp"

#include "platformadmin/ConfigFileParser.hpp"
#include "platformadmin/SystemParser.hpp"
#include "platformadmin/TimeoutParser.hpp"
#include "platformadmin/HostParser.hpp"
#include "platformadmin/ServiceParser.hpp"
#include "platformadmin/ApplicationParser.hpp"
#include "platformadmin/ProcessParser.hpp"
#include "platformadmin/EntityParser.hpp"
#include "platformadmin/PropertyParser.hpp"
#include "platformadmin/GraphParser.hpp"
#include "platformadmin/LifeCycleParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

/**
 * Configuration error handler.
 */
void ConfigErrorHandler::warning(
    const SAXParseException& e )
{
}

void ConfigErrorHandler::error(
    const SAXParseException& e )
{
    throw e;
}

void ConfigErrorHandler::fatalError(
    const SAXParseException& e )
{
    error( e );
}

void ConfigErrorHandler::resetErrors()
{
}

/**
 * Configuration parser.
 */
ConfigFileParser::ConfigFileParser(
    const char* configFileName )
{
    try
    {
        if( !Cdmw::OsSupport::OS::file_exists( configFileName ) )
            CDMW_THROW1(
                ConfigException,
                std::string( "file \"" ) + configFileName + "\" not found" );

        // Attache a parser to this config parser
        std::auto_ptr< DOMParser > parser( new DOMParser() );
        
        parser->setValidationScheme( DOMParser::Val_Auto );
        parser->setDoNamespaces( false );

        // Attaches a parser error handler to this config parser
        std::auto_ptr< ConfigErrorHandler > handler(
            new ConfigErrorHandler() );

        parser->setErrorHandler( handler.get() );

        // Parse the configuration file
        parser->parse( configFileName );

        // Release the temporary objects
        m_parser = parser.release();
        m_error_handler = handler.release();
        m_configFileName = configFileName;
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}

ConfigFileParser::~ConfigFileParser()
{
    if( m_parser )
    {
        delete m_parser;
        m_parser = NULL;
    }
    if( m_error_handler )
    {
        delete m_error_handler;
        m_error_handler = NULL;
    }
}

ConfigFileParser::ConfigFileType ConfigFileParser::get_config_type() const
{
    std::string doc_type_name = to_string(
        m_parser->getDocument().getDoctype().getName() );

    if( doc_type_name == "System" )
        return CONFIG_SYSTEM_TYPE;
    else if( doc_type_name == "Host")
        return CONFIG_HOST_TYPE;
    else if( doc_type_name == "Service")
        return CONFIG_SERVICE_TYPE;
    else if( doc_type_name == "Application")
        return CONFIG_APPLICATION_TYPE;
    else if( doc_type_name == "Unmanaged-process")
        return CONFIG_PROCESS_TYPE;
    else if( doc_type_name == "Managed-process")
        return CONFIG_PROCESS_TYPE;
    else if( doc_type_name == "Init-graph")
        return CONFIG_INIT_GRAPH_TYPE;
    else if( doc_type_name == "Stop-graph")
        return CONFIG_STOP_GRAPH_TYPE;
    else if( doc_type_name == "Life-cycle")
        return CONFIG_LIFE_CYCLE_TYPE;
    else
        return CONFIG_UNKNOWN_TYPE;
}

SystemParser ConfigFileParser::get_system() const
{
    if( get_config_type() != CONFIG_SYSTEM_TYPE )
        CDMW_THROW1(
            ConfigException,
            std::string( "file '" ) + m_configFileName + "' \n" +
            "is not a 'System'" );

    return SystemParser(
        m_parser->getDocument().getDocumentElement() );
}

HostParser ConfigFileParser::get_host() const
{
    if( get_config_type() != CONFIG_HOST_TYPE )
        CDMW_THROW1(
            ConfigException,
            std::string( "file '" ) + m_configFileName + "' \n" +
            "is not a 'Host'" );

    return HostParser(
        m_parser->getDocument().getDocumentElement() );
}

ApplicationParser ConfigFileParser::get_application() const
{
    if( get_config_type() != CONFIG_APPLICATION_TYPE )
        CDMW_THROW1(
            ConfigException,
            std::string( "file '" ) + m_configFileName + "' \n" +
            "is not an 'Application'" );

    return ApplicationParser(
        m_parser->getDocument().getDocumentElement() );
}

ProcessParser ConfigFileParser::get_process() const
{
    if( get_config_type() != CONFIG_PROCESS_TYPE )
        CDMW_THROW1(
            ConfigException,
            std::string( "file '" ) + m_configFileName + "' \n" +
            "is not a 'Process'" );

    return ProcessParser(
        m_parser->getDocument().getDocumentElement() );
}

ServiceParser ConfigFileParser::get_service() const
{
    if( get_config_type() != CONFIG_SERVICE_TYPE )
        CDMW_THROW1(
            ConfigException,
            std::string( "file '" ) + m_configFileName + "' \n" +
            "is not a 'Service'" );

    return ServiceParser(
        m_parser->getDocument().getDocumentElement() );
}

GraphParser ConfigFileParser::get_init_graph() const
{
    if( get_config_type() != CONFIG_INIT_GRAPH_TYPE )
        CDMW_THROW1(
            ConfigException,
            std::string( "file '" ) + m_configFileName + "' \n" +
            "is not an 'Init-graph'" );

    return GraphParser(
        m_parser->getDocument().getDocumentElement() );
}

GraphParser ConfigFileParser::get_stop_graph() const
{
    if( get_config_type() != CONFIG_STOP_GRAPH_TYPE )
        CDMW_THROW1(
            ConfigException,
            std::string( "file '" ) + m_configFileName + "' \n" +
            "is not a 'Stop-graph'" );

    return GraphParser(
        m_parser->getDocument().getDocumentElement() );
}

LifeCycleParser ConfigFileParser::get_life_cycle() const
{
    if( get_config_type() != CONFIG_LIFE_CYCLE_TYPE )
        CDMW_THROW1(
            ConfigException,
            std::string( "file '" ) + m_configFileName + "' \n" +
            "is not a 'Life-cycle'" );

    return LifeCycleParser(
        m_parser->getDocument().getDocumentElement() );
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

