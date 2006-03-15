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


#include "platformadmin/Property.hpp"
#include "platformadmin/PropertyParser.hpp"
#include "platformadmin/ConfigFileParser.hpp"
#include "platformadmin/AdminLogMessageId.hpp"

#include "SystemMngt/platformlibrary/LogMngr.hpp"

extern bool check_only;

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

/**
 * class Property 
 */
CdmwPlatformMngt::PropertyDef* Property::get_definition(
	const PropertyParser& propertyParser)
{
	try
    {
        CdmwPlatformMngt::PropertyDef_var propertyDef
            = new CdmwPlatformMngt::PropertyDef();

        // Get the property's name
	propertyDef->property_name = CORBA::string_dup(propertyParser.get_name().c_str());
	
	std::string tmp = CORBA::string_dup(propertyParser.get_mode().c_str());
        propertyDef->mode_type = CdmwPlatformMngt::PropertyModeType(0);
	if (tmp=="RO")  propertyDef->mode_type = CdmwPlatformMngt::PropertyModeType(1);

	std::string value_type = propertyParser.get_type();
	
	CORBA::Any any_value;
	
	
	if (value_type=="String") propertyDef->value_type = CdmwPlatformMngt::PropertyValueType(2);
	if (value_type=="Float") propertyDef->value_type = CdmwPlatformMngt::PropertyValueType(1);
	if (value_type=="Short") propertyDef->value_type = CdmwPlatformMngt::PropertyValueType(0);

	if (propertyDef->value_type==2)
			{
 			        std::string tmp_str_value = propertyParser.get_value();
				any_value <<= tmp_str_value.c_str();
				propertyDef->initial_value=any_value;
			}
			
	if (propertyDef->value_type==1)
			{
				float tmp_float_value = (float)atof(propertyParser.get_value().c_str());
				any_value <<= tmp_float_value;
				propertyDef->initial_value=any_value;
			}
		
	if (propertyDef->value_type==0)
			{
				short tmp_short_value = (short)atoi(propertyParser.get_value().c_str());
				any_value <<= tmp_short_value;
				propertyDef->initial_value=any_value;
			}

	return propertyDef._retn();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
}


} // End namespace PlatformAdmin
} // End namespace Cdmw

