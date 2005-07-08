/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include <ccm_ft/CCM_ClientHome_impl.hpp>
#include <ccm_ft/CCM_Client_impl.hpp>

//
// IDL:thalesgroup.com/CcmFtTest:1.0
//

extern "C" {

//
// create a new client home
//
Components::HomeExecutorBase* create_CCM_ClientHome()
{
  return new CcmFtTest::CCM_ClientHome_impl();
};

};  // End extern C


//
// IDL:thalesgroup.com/CcmFtTest/CCM_ClientHome:1.0
//
CcmFtTest::CCM_ClientHome_impl::CCM_ClientHome_impl()
{
}

CcmFtTest::CCM_ClientHome_impl::~CCM_ClientHome_impl()
{
}

//
// IDL:thalesgroup.com/CcmFtTest/CCM_ClientHomeImplicit/create:1.0
//
Components::EnterpriseComponent_ptr
CcmFtTest::CCM_ClientHome_impl::create()
    throw(Components::CCMException,
          CORBA::SystemException)
{
  // creation of a new component in the client home
  return new CCM_Client_impl();
}




