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


#include <ccm_unaware/CCM_ServerHome_impl.hpp>
#include <ccm_unaware/CCM_Server_impl.hpp>

//
// IDL:thalesgroup.com/SimpleCcmServer:1.0
//

extern "C" {

//
// create a new server home
//
Components::HomeExecutorBase* create_CCM_ServerHome()
{
    return new SimpleCcmServer::CCM_ServerHome_impl();
};

};  // End extern C


//
// IDL:thalesgroup.com/SimpleCcmServer/CCM_ServerHome:1.0
//
SimpleCcmServer::CCM_ServerHome_impl::CCM_ServerHome_impl()
{
}

SimpleCcmServer::CCM_ServerHome_impl::~CCM_ServerHome_impl()
{
}


//
// IDL:thalesgroup.com/SimpleCcmServer/CCM_ServerHomeImplicit/create:1.0
//
Components::EnterpriseComponent_ptr
SimpleCcmServer::CCM_ServerHome_impl::create()
    throw(Components::CCMException,
          CORBA::SystemException)
{
  // creation of a new component in the server home
  return new CCM_Server_impl();
}

