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


// Cdmw files
#include <Foundation/orbsupport/CORBA.hpp>

// Cdmw CCM files
#include <CCMContainer/ccmcif/CCMContext_impl.hpp>

namespace 
{

} // end anonymous namespace


namespace Cdmw {

namespace CCM {

namespace CIF {

//
// IDL:omg.org/Components/CCMContext:1.0
//  
CCMContext_impl::CCMContext_impl(Components::CCMContext_ptr ctx)
: m_context(Components::CCMContext::_duplicate(ctx))
{
}

CCMContext_impl::~CCMContext_impl()
{
}

//
// IDL:omg.org/Components/CCMContext/get_caller_principal:1.0
//
Components::Principal_ptr 
CCMContext_impl::get_caller_principal()
    throw(CORBA::SystemException)
{
    return m_context->get_caller_principal();
}

//
// IDL:omg.org/Components/CCMContext/get_CCM_home:1.0
//
Components::CCMHome_ptr  
CCMContext_impl::get_CCM_home()
    throw(CORBA::SystemException)
{
    return m_context->get_CCM_home();
}

//
// IDL:omg.org/Components/CCMContext/get_rollback_only:1.0
//
CORBA::Boolean  
CCMContext_impl::get_rollback_only()
    throw(Components::IllegalState,
          CORBA::SystemException)
{
    return m_context->get_rollback_only();
}

//
// IDL:omg.org/Components/CCMContext/get_user_transaction:1.0
//
Components::Transaction::UserTransaction_ptr  
CCMContext_impl::get_user_transaction()
    throw(Components::IllegalState,
          CORBA::SystemException)
{
    return m_context->get_user_transaction();
}

//
// IDL:omg.org/Components/CCMContext/is_caller_in_role:1.0
//
CORBA::Boolean  
CCMContext_impl::is_caller_in_role(const char* role)
    throw(CORBA::SystemException)
{
    return m_context->is_caller_in_role(role);
}

//
// IDL:omg.org/Components/CCMContext/set_rollback_only:1.0
//
void  
CCMContext_impl::set_rollback_only()
    throw(Components::IllegalState,
          CORBA::SystemException)
{
    m_context->set_rollback_only();
}

} // end namespace CIF
} // end namespace CCM
} // end namespace Cdmw



