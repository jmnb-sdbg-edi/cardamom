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


#include "naming/Cdmw_NamingInterface_NameParserContext.hpp"


/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/

namespace
{
    // TODO: [i18n]put strings in a message catalog
    const char* TRAILING_DOT_NOT_ALLOWED = "Trailing '.' not permitted!";
    const char* UNEXPECTED_ENDS = "Unexpected end of string!";
    const char* MISSING_DOT = "Invalid empty name component : '.' may be missing";
}

Cdmw_NamingInterface_NameParserContext::Cdmw_NamingInterface_NameParserContext()
throw(Cdmw::OutOfMemoryException)
        : m_char('\0'),
        m_components(0)
{
    // INit of std::istringstream m_istringstream,
    // std::string m_id, std::string m_kind;

    try
    {

        m_name = new CosNaming::Name(0);

    }
    catch ( const std::bad_alloc& )
    {

        CDMW_THROW(Cdmw::OutOfMemoryException);
    }
}

Cdmw_NamingInterface_NameParserContext::~Cdmw_NamingInterface_NameParserContext()
throw()
{
    // doses nothing
}

void Cdmw_NamingInterface_NameParserContext::FSMError(const char* t, const char* s)
throw (Cdmw::CommonSvcs::Naming::InvalidNameException)
{
    std::string str = "Transition error: ";
    str += t;
    str += " in state ";
    str += s;

    CDMW_THROW1(Cdmw::CommonSvcs::Naming::InvalidNameException, str.c_str()); // TODO: use named constants
}


void Cdmw_NamingInterface_NameParserContext::add_name_component()
{
    append_name_component(m_id.c_str(), m_kind.c_str());
}

void Cdmw_NamingInterface_NameParserContext::add_last_id()
{
    append_name_component(m_id.c_str(), "");
}

void Cdmw_NamingInterface_NameParserContext::add_last_kind()
throw (Cdmw::CommonSvcs::Naming::InvalidNameException)
{
    // If a name component has a non-empty id field and an empty kind field,
    // the stringified representation consists only of the id field. A trailing
    // '.' character is not permitted.

    if (!m_id.empty() && m_kind.empty())
        CDMW_THROW1(Cdmw::CommonSvcs::Naming::InvalidNameException, TRAILING_DOT_NOT_ALLOWED);

    append_name_component(m_id.c_str(), m_kind.c_str());
}

void Cdmw_NamingInterface_NameParserContext::unexpected_eos()
throw (Cdmw::CommonSvcs::Naming::InvalidNameException)
{
    CDMW_THROW1(Cdmw::CommonSvcs::Naming::InvalidNameException, UNEXPECTED_ENDS);
}


void Cdmw_NamingInterface_NameParserContext::check_id_not_empty()
throw (Cdmw::CommonSvcs::Naming::InvalidNameException)
{
    // The single '.' character is the only representation of a name
    // component with empty id and kind fields.

    if (m_id.empty())
        CDMW_THROW1(Cdmw::CommonSvcs::Naming::InvalidNameException, MISSING_DOT);
}

void
Cdmw_NamingInterface_NameParserContext::append_name_component(const char* id_value,
        const char* kind_value)
{
    m_name->length(++m_components);
    m_name[m_components - 1].id = id_value;
    m_name[m_components - 1].kind = kind_value;
    // reset m_id and m_kind content to ""
    m_id = "";
    m_kind = "";
}


