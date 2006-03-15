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


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "XMLServer/xmllibrary/XMLHelper.hpp"

#include "XMLServer/idllib/CdmwXML.stub.hpp"
#include "xmllibrary/DocumentDescriptionFactory_impl.hpp"
#include "xmllibrary/ElementDescriptionFactory_impl.hpp"
#include "xmllibrary/AttrDescriptionFactory_impl.hpp"
#include "xmllibrary/TextDescriptionFactory_impl.hpp"
#include "xmllibrary/CDATASectionDescriptionFactory_impl.hpp"

namespace Cdmw
{
namespace XMLUtils
{


char*
XMLHelper::to_char_array(CdmwXML::DOMString* string)
{
	CORBA::ULong str_length = (*string).length();
	char* result = new char[str_length+1];
	result[str_length] = '\0';
	for(CORBA::ULong i = 0 ; i < str_length ; i++) {
		result[i] = (char)((*string)[i]);
	}
	return result;
}
    
CdmwXML::DOMString&
XMLHelper::to_DOM_string(const char* string)
{
	CdmwXML::DOMString* val = new CdmwXML::DOMString;
	val -> length(strlen(string));
	for(CORBA::ULong i = 0 ; i < val -> length() ; i++) {		
		(*val)[i] = (CORBA::UShort)(string[i]);
	}
	return (*val);
}
    

void
XMLHelper::register_value_factories(CORBA::ORB_ptr orb)
{

	//
	// Create & install value factory - if we forget to do this,
	// the client will receive a MARSHAL exception when attempting
	// to transmit a Employee value.
	//
	CORBA::ValueFactoryBase_var factory, oldFactory;
	factory = new Cdmw::XMLUtils::DocumentDescriptionFactory_impl();
	oldFactory = orb -> register_value_factory(
		"IDL:thalesgroup.com/CdmwXML/DocumentDescription:1.0", factory.in());
	factory = new Cdmw::XMLUtils::ElementDescriptionFactory_impl();
	oldFactory = orb -> register_value_factory(
		"IDL:thalesgroup.com/CdmwXML/ElementDescription:1.0", factory.in());
	factory = new Cdmw::XMLUtils::AttrDescriptionFactory_impl();
	oldFactory = orb -> register_value_factory(
		"IDL:thalesgroup.com/CdmwXML/AttrDescription:1.0", factory.in());
	factory = new Cdmw::XMLUtils::TextDescriptionFactory_impl();
	oldFactory = orb -> register_value_factory(
		"IDL:thalesgroup.com/CdmwXML/TextDescription:1.0", factory.in());
	factory = new Cdmw::XMLUtils::CDATASectionDescriptionFactory_impl();
	oldFactory = orb -> register_value_factory(
		"IDL:thalesgroup.com/CdmwXML/CDATASectionDescription:1.0", factory.in());

}

}; // End namespace XMLUtils
}; // End namespace Cdmw

