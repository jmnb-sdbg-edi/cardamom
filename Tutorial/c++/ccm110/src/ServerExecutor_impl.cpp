/* =========================================================================== *
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
 * =========================================================================== */

#include <iostream>
#include "ServerExecutor_impl.hpp"

#include "DisplayFacetExecutor_impl.hpp"


namespace Hello
{
    
//
// IDL:thalesgroup.com/Hello/CCM_Server:1.0
//
ServerExecutor_impl::ServerExecutor_impl()
{
    m_name = "";
    m_display = new DisplayFacetExecutor_impl (this);
}

ServerExecutor_impl::~ServerExecutor_impl()
{
    std::cout << "===> ServerExecutor_impl::~ServerExecutor_impl called!" << std::endl;
}


//
// IDL:thalesgroup.com/Hello/CCM_Server_Executor/name:1.0
//
char* ServerExecutor_impl::name()
    throw(CORBA::SystemException)
{
    char* name = CORBA::string_dup(m_name.in());
    return name;
}

void ServerExecutor_impl::name(const char* a)
    throw(CORBA::SystemException)
{
    m_name = a;
}



//
// IDL:thalesgroup.com/Hello/CCM_Server/get_server_facet:1.0
//
CCM_Display_ptr ServerExecutor_impl::get_server_facet()
    throw(CORBA::SystemException)
{
    std::cout << "===> get_server_facet called!" << std::endl;
    
    return CCM_Display::_duplicate(m_display.in());
}



}; // End of namespace Hello

