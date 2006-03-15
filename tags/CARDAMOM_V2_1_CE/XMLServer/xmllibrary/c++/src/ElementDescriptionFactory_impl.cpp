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

#include "XMLServer/idllib/CdmwXML.stub.hpp"
#include "xmllibrary/ElementDescriptionFactory_impl.hpp"
#include "XMLServer/xmllibrary/ElementDescription_impl.hpp"

namespace Cdmw
{
namespace XMLUtils
{

// ----------------------------------------------------------------------
// ElementDescriptionFactory_impl private member implementation
// ----------------------------------------------------------------------

CORBA::ValueBase*
ElementDescriptionFactory_impl::create_for_unmarshal()
{
    return new ElementDescription_impl();
}

// ----------------------------------------------------------------------
// ElementDescriptionFactory_impl public member implementation
// ----------------------------------------------------------------------

CdmwXML::ElementDescription*
ElementDescriptionFactory_impl::create(CdmwXML::Element_ptr reference,
                                        const CdmwXML::DOMString& name)
{
    return new ElementDescription_impl(CdmwXML::ELEMENT_NODE, reference, name);
}

}; // End namespace XMLUtils
}; // End namespace Cdmw

