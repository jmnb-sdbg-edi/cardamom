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


#include "ObserverHomeExecutor_impl.hpp"
#include "ObserverExecutor_impl.hpp"

//
// IDL:acme.com/Dinner:1.0
//

extern "C" 
{

//
// create a new server home
//
Components::HomeExecutorBase* create_ObserverHomeExecutor()
{
    return new Dinner::ObserverHomeExecutor_impl();
};

};  // End extern C


namespace Dinner 
{
       
//
// IDL:acme.com/Dinner/CCM_ObserverHome:1.0
//
ObserverHomeExecutor_impl::ObserverHomeExecutor_impl()
{
}

ObserverHomeExecutor_impl::~ObserverHomeExecutor_impl()
{
}

//
// IDL:acme.com/Dinner/CCM_ObserverHomeImplicit/create:1.0
//
Components::EnterpriseComponent_ptr ObserverHomeExecutor_impl::create()
    throw(Components::CCMException,
          CORBA::SystemException)
{
    // creation of a new component in the server home
    return new ObserverExecutor_impl();
}


} // End namespace Dinner

