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


#include "simrepository/MessageLogger_impl.hpp"

namespace Cdmw
{
namespace Tools
{


MessageLogger_impl::MessageLogger_impl (PortableServer::POA_ptr  parent,
                                        const char*              filename) 
    throw (CORBA::SystemException)
    : m_poa(PortableServer::POA::_duplicate(parent)),
      m_os(filename)
{
}

MessageLogger_impl::~MessageLogger_impl() throw()
{
}
    
PortableServer::POA_ptr MessageLogger_impl::_default_POA()
{
    return PortableServer::POA::_duplicate(m_poa.in());
}

//
// IDL:thalesgroup.com/CdmwSimulatedRepository/MessageLogger/log:1.0
//
void
MessageLogger_impl::log(const char* header,
                        const char* body)
    throw(CORBA::SystemException)
{
    m_os << "<SIM_Logger header=\"" << header << "\" >\n"
         << "\t<message>" << body << "</message>\n"
         << "</SIM_Logger>"
         << std::endl;
}


} // namespace Tools
} // namespace Cdmw

