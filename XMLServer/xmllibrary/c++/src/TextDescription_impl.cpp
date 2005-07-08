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


#include "Foundation/orbsupport/CORBA.hpp"

#include "XMLServer/idllib/CdmwXML.stub.hpp"
#include "XMLServer/xmllibrary/TextDescription_impl.hpp"

namespace Cdmw
{
namespace XMLUtils
{

// ----------------------------------------------------------------------
// TextDescription_impl constructors
// ----------------------------------------------------------------------

TextDescription_impl::TextDescription_impl()
{
}

TextDescription_impl::TextDescription_impl(CdmwXML::NodeType type,
                        	 CdmwXML::Node_ptr reference,
                        	 const CdmwXML::DOMString& data)
{
    this -> s_type(type);
    this -> s_reference(reference);
    this -> s_data(data);
}


// ----------------------------------------------------------------------
// TextDescription_impl public member implementation
// ----------------------------------------------------------------------

	CORBA::ValueBase*
	TextDescription_impl::_copy_value()
	{
		TextDescription_impl* result = new TextDescription_impl;
		result -> s_type(type());
		result -> s_reference(reference());
		result -> s_data(*data());
		return result;
	}

	//
	// IDL:thalesgroup.com/CdmwXML/NodeDescription/type:1.0
	//
	CdmwXML::NodeType TextDescription_impl::type() {
		return s_type();
	}

	//
	// IDL:thalesgroup.com/CdmwXML/NodeDescription/reference:1.0
	//
    CdmwXML::Node_ptr TextDescription_impl::reference() {
    	return s_reference();
    }
    
   //
   	// IDL:thalesgroup.com/CdmwXML/TextDescription/data:1.0
   	//
   	CdmwXML::DOMString* TextDescription_impl::data() {
   		return &(s_data());
   	}
   	
	void TextDescription_impl::data(const CdmwXML::DOMString& newData) {
		this -> s_data(newData);
    }

}; // End namespace XMLUtils
}; // End namespace Cdmw

