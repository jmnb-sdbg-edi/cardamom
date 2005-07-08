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


#ifndef INCL_PLATFORMADMIN_SYSTEMPARSER_HPP
#define INCL_PLATFORMADMIN_SYSTEMPARSER_HPP

#include "platformadmin/ConfigElementParser.hpp"
#include "platformadmin/ServiceParser.hpp"
#include "platformadmin/EntityParser.hpp"
#include "platformadmin/PropertyParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class TimeoutParser;
class HostMonitoringParser;
class ApplicationMonitoringParser;
class ProcessMonitoringParser;
class ApplicationParser;
class HostParser;
class RunSetParser;
class GraphParser;

class SystemParser : public virtual ConfigElementParser, public virtual ServiceContainerParser, public virtual EntityContainerParser, public virtual PropertyContainerParser
{
public:
    SystemParser( const DOM_Node& systemParser );

    std::string name() const;

    unsigned long port() const;

    TimeoutParser get_timeout() const;

    HostMonitoringParser get_host_monitoring() const;

    ApplicationMonitoringParser get_application_monitoring() const;

    ProcessMonitoringParser get_proc_monitoring() const;

    unsigned int get_application_count() const;

    ApplicationParser get_application( unsigned int applicationIndex ) const;

    unsigned int get_host_count() const;

    HostParser get_host( unsigned int hostIndex ) const;

    unsigned int get_run_set_count() const;

    RunSetParser get_run_set( unsigned int runSetIndex ) const;

    GraphParser get_init_graph() const;

    GraphParser get_stop_graph() const;
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_SYSTEMPARSER_HPP

