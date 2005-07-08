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
#include "xmllibrary/AttrDescriptionFactory_impl.hpp"
#include "XMLServer/xmllibrary/AttrDescription_impl.hpp"

namespace Cdmw
{
namespace XMLUtils
{

// ----------------------------------------------------------------------
// AttrDescriptionFactory_impl private member implementation
// ----------------------------------------------------------------------

CORBA::ValueBase*
AttrDescriptionFactory_impl::create_for_unmarshal()
{
    return new AttrDescription_impl();
}

// ----------------------------------------------------------------------
// AttrDescriptionFactory_impl public member implementation
// ----------------------------------------------------------------------

CdmwXML::AttrDescription*
AttrDescriptionFactory_impl::create(CdmwXML::Attr_ptr reference,
									const CdmwXML::DOMString& name,
                    				const CdmwXML::DOMString& value)
{
    return new AttrDescription_impl(CdmwXML::ATTRIBUTE_NODE, reference, name, value);
}

}; // End namespace XMLUtils
}; // End namespace Cdmw

