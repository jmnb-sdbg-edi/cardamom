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


#include <ccm/CCM_ServerHome_impl.hpp>
#include <ccm/CCM_Server_impl.hpp>

#include <iostream>

//
// IDL:thalesgroup.com/BasicDemo:1.0
//

extern "C" {

//
// create a new server home
//
Components::HomeExecutorBase* create_CCM_ServerHome()
{
    return new BasicDemo::CCM_ServerHome_impl();
};

};  // End extern C


//
// IDL:thalesgroup.com/BasicDemo/CCM_ServerHome:1.0
//
BasicDemo::CCM_ServerHome_impl::CCM_ServerHome_impl()
{
}

BasicDemo::CCM_ServerHome_impl::~CCM_ServerHome_impl()
{
}

//
// IDL:thalesgroup.com/BasicDemo/CCM_NamedComponentHomeExplicit/home_name:1.0
//
char*
BasicDemo::CCM_ServerHome_impl::home_name()
    throw(CORBA::SystemException)
{
    char* name = CORBA::string_dup(m_name);
    return name;
}

void
BasicDemo::CCM_ServerHome_impl::home_name(const char* a)
    throw(CORBA::SystemException)
{
    m_name = a;
    std::cout << "ServerHome's name is " << m_name << std::endl;
}

//
// IDL:thalesgroup.com/BasicDemo/CCM_NamedComponentHomeExplicit/create_named_component:1.0
//
Components::EnterpriseComponent_ptr 
BasicDemo::CCM_ServerHome_impl::create_named_component(const char* name)
    throw(CORBA::SystemException)
{
    BasicDemo::CCM_Server_var server = new CCM_Server_impl();

    server->name(name);

    return server._retn();
}

//
// IDL:thalesgroup.com/BasicDemo/CCM_ServerHomeImplicit/create:1.0
//
Components::EnterpriseComponent_ptr
BasicDemo::CCM_ServerHome_impl::create()
    throw(Components::CCMException,
          CORBA::SystemException)
{
  // creation of a new component in the server home
  return new CCM_Server_impl();
}

