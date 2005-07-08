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


#include <ClientHomeExecutor_impl.hpp>
#include <ClientExecutor_impl.hpp>

//
// IDL:acme.com/Hello:1.0
//

extern "C" 
{

//
// create a new client home
//
Components::HomeExecutorBase* create_ClientHomeExecutor()
{
    return new Hello::ClientHomeExecutor_impl();
};

};  // End extern C


namespace Hello
{
    
//
// IDL:acme.com/Hello/CCM_ClientHome:1.0
//
ClientHomeExecutor_impl::ClientHomeExecutor_impl()
{
}

ClientHomeExecutor_impl::~ClientHomeExecutor_impl()
{
}

//
// IDL:acme.com/Hello/CCM_ClientHomeImplicit/create:1.0
//
Components::EnterpriseComponent_ptr ClientHomeExecutor_impl::create()
    throw(Components::CCMException,
          CORBA::SystemException)
{
    // creation of a new component in the client home
    return new ClientExecutor_impl();
}


}; // End of namespace Hello


