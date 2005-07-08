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


#ifndef INCL_PLATFORMADMIN_APPLICATIONPARSER_HPP
#define INCL_PLATFORMADMIN_APPLICATIONPARSER_HPP

#include "platformadmin/ConfigElementParser.hpp"
#include "platformadmin/ServiceParser.hpp"
#include "platformadmin/EntityParser.hpp"
#include "platformadmin/PropertyParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class TimeoutParser;
class ApplicationMonitoringParser;
class ProcessMonitoringParser;
class ProcessParser;
class GraphParser;

class ApplicationParser : public virtual ConfigElementParser, public virtual ServiceContainerParser, public virtual EntityContainerParser, public virtual PropertyContainerParser
{
public:
    ApplicationParser( const DOM_Node& applicationParser );

    std::string name() const;

    std::string host_name() const;

    unsigned long app_port() const;

    std::string app_args() const;

    std::string app_run_dir() const;

    unsigned long agent_port() const;

    std::string agent_args() const;

    std::string agent_run_dir() const;

    TimeoutParser get_timeout() const;

    ApplicationMonitoringParser get_monitoring() const;

    ProcessMonitoringParser get_proc_monitoring() const;

    unsigned int get_managed_process_count() const;

    ProcessParser get_managed_process( unsigned int processIndex ) const;

    unsigned int get_unmanaged_process_count() const;

    ProcessParser get_unmanaged_process( unsigned int processIndex ) const;

    GraphParser get_init_graph() const;

    GraphParser get_stop_graph() const;
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_APPLICATIONPARSER_HPP

