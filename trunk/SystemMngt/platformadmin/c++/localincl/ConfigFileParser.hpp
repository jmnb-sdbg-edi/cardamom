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


#ifndef INCL_PLATFORMADMIN_CONFIGFILEPARSER_HPP
#define INCL_PLATFORMADMIN_CONFIGFILEPARSER_HPP

#include <string>
#include <parsers/DOMParser.hpp>
#include <sax/ErrorHandler.hpp>

#include "Foundation/common/Exception.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class ConfigException;
class NotFoundException;
class SystemParser;
class HostParser;
class ServiceParser;
class ApplicationParser;
class ProcessParser;
class EntityParser;
class GraphParser;
class LifeCycleParser;
class PropertyParser;

class ConfigErrorHandler : public virtual ErrorHandler
{
public:
    void warning( const SAXParseException& e );
    void error( const SAXParseException& e );
    void fatalError( const SAXParseException& e );
    void resetErrors();
};

class ConfigFileParser
{
private:
    std::string         m_configFileName;
    DOMParser*          m_parser;
    ConfigErrorHandler* m_error_handler;

public:
    enum ConfigFileType
    {
        CONFIG_UNKNOWN_TYPE,
        CONFIG_SYSTEM_TYPE,
        CONFIG_HOST_TYPE,
        CONFIG_ENTITY_TYPE,
        CONFIG_SERVICE_TYPE,
        CONFIG_APPLICATION_TYPE,
        CONFIG_PROCESS_TYPE,
        CONFIG_INIT_GRAPH_TYPE,
        CONFIG_STOP_GRAPH_TYPE,
        CONFIG_LIFE_CYCLE_TYPE,
    	CONFIG_PROPERTY_TYPE
    };
    
    ConfigFileParser( const char* configFileName );

    virtual ~ConfigFileParser();

    ConfigFileType get_config_type() const;

    SystemParser get_system() const;

    HostParser get_host() const;

    ProcessParser get_process() const;
    
    EntityParser get_entity() const;

    ServiceParser get_service() const;

    ApplicationParser get_application() const;

    GraphParser get_init_graph() const;

    GraphParser get_stop_graph() const;

    LifeCycleParser get_life_cycle() const;
 	
    PropertyParser get_property() const; 
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_CONFIGFILEPARSER_HPP

