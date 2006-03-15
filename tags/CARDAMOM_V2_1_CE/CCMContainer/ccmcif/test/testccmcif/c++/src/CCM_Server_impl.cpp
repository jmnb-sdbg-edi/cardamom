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

#include <iostream>

#include "testccmcif/CCM_Server_impl.hpp"

//
// IDL:thalesgroup.com/BasicDemo/CCM_Server:1.0
//
BasicDemo::CCM_Server_impl::CCM_Server_impl()
{
   m_indice = 0;
   m_name = "";
    m_display = new BasicDemo::CCM_Display_impl();
}

BasicDemo::CCM_Server_impl::~CCM_Server_impl()
{
    std::cout << "CCM_Server_impl::~CCM_Server_impl called!" << std::endl;
}

//
// IDL:thalesgroup.com/BasicDemo/CCM_Server/get_for_clients:1.0
//
::BasicDemo::CCM_Display_ptr
BasicDemo::CCM_Server_impl::get_for_clients()
    throw(CORBA::SystemException)
{
    return BasicDemo::CCM_Display::_duplicate(m_display.in());
}

//
// IDL:thalesgroup.com/BasicDemo/CCM_Server_Executor/name:1.0
//
char*
BasicDemo::CCM_Server_impl::name()
    throw(CORBA::SystemException)
{
    // TODO: Implementation
    char* name = CORBA::string_dup(m_name.in());
    return name;
}

void
BasicDemo::CCM_Server_impl::name(const char* a)
    throw(CORBA::SystemException)
{
    m_name = a;
}


//
// IDL:thalesgroup.com/BasicDemo/Registration/register:1.0
//
char*
BasicDemo::CCM_Server_impl::_cxx_register()
    throw(CORBA::SystemException)
{
    // TODO: Implementation
    //return a string with a register number - ex: "Register 5"
      char tmp[42];
      sprintf (tmp, "Register %d", ++m_indice);
      return CORBA::string_dup (tmp);
 
}


