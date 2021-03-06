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


#include "testphilosophers/CCM_PhilosopherHome_impl.hpp"
#include "testphilosophers/CCM_Philosopher_impl.hpp"

//
// IDL:acme.com/Dinner:1.0
//

extern "C" {

//
// create a new client home
//
Components::HomeExecutorBase* create_CCM_PhilosopherHome()
{
  return new Dinner::CCM_PhilosopherHome_impl();
};

};  // End extern C


//
// IDL:acme.com/Dinner/CCM_PhilosopherHome:1.0
//
Dinner::CCM_PhilosopherHome_impl::CCM_PhilosopherHome_impl()
{
}

Dinner::CCM_PhilosopherHome_impl::~CCM_PhilosopherHome_impl()
{
}

//
// IDL:acme.com/Dinner/CCM_PhilosopherHomeImplicit/create:1.0
//
Components::EnterpriseComponent_ptr
Dinner::CCM_PhilosopherHome_impl::create()
    throw(Components::CCMException,
          CORBA::SystemException)
{
  // creation of a new component in the client home
  return new CCM_Philosopher_impl();
}




