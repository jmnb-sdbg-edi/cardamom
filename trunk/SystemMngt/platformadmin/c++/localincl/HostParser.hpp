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


#ifndef INCL_PLATFORMADMIN_HOSTPARSER_HPP
#define INCL_PLATFORMADMIN_HOSTPARSER_HPP

#include "platformadmin/ConfigElementParser.hpp"
#include "platformadmin/EntityParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class HostMonitoringParser;
class PropertyParser;
class HostParser;

class HostParser : public virtual ConfigElementParser, public virtual EntityContainerParser
{
public:
    HostParser( const DOM_Node& hostParser);

    HostMonitoringParser get_monitoring() const;

    std::string name() const;

    std::string app_exec_path() const;

    PropertyParser get_property() const;	

    PropertyParser HostParser::get_property(unsigned int propertyIndex) const;
                                                                                                                             
    unsigned int HostParser::get_property_count() const;

};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_HOSTPARSER_HPP

