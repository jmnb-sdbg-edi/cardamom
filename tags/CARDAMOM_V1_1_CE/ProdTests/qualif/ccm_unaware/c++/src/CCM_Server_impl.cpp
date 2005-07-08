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
#include <ccm_unaware/CCM_Server_impl.hpp>

//
// IDL:thalesgroup.com/SimpleCcmServer/CCM_Server:1.0
//
SimpleCcmServer::CCM_Server_impl::CCM_Server_impl()
{
   m_name = "TestName";
   m_display = new SimpleCcmServer::CCM_Display_impl();
}

SimpleCcmServer::CCM_Server_impl::~CCM_Server_impl()
{
    std::cout << "CCM_Server_impl::~CCM_Server_impl called!" << std::endl;
}

//
// IDL:thalesgroup.com/SimpleCcmServer/CCM_Server/get_for_clients:1.0
//
::SimpleCcmServer::CCM_Display_ptr
SimpleCcmServer::CCM_Server_impl::get_for_clients()
    throw(CORBA::SystemException)
{
    return SimpleCcmServer::CCM_Display::_duplicate(m_display.in());
}



//
// IDL:thalesgroup.com/SimpleCcmServer/CCM_Server/get_name:1.0
//
char*
SimpleCcmServer::CCM_Server_impl::get_name()
    throw(CORBA::SystemException)
{
    char* name = CORBA::string_dup(m_name);
    return name;
}

