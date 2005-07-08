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


#ifndef INCL_CCM_CIF_CCMCONTEXT_IMPL_HPP
#define INCL_CCM_CIF_CCMCONTEXT_IMPL_HPP

#include <CCMContainer/idllib/Components.skel.hpp>

#include <Foundation/orbsupport/RefCountLocalObject.hpp>


namespace Cdmw {

namespace CCM {

namespace CIF {


/**
*Purpose:
*<p>    [class description]
*    This class implement the Components::CCMContext interface.
*    It will be inherited by all components context implementation.
*
*/

//
// IDL:omg.org/Components/CCMContext:1.0
//
class CCMContext_impl : virtual public Components::CCMContext,
                        virtual public OrbSupport::RefCountLocalObject
{
public:

    CCMContext_impl(Components::CCMContext_ptr ctx);
     ~CCMContext_impl();

    //
    // IDL:omg.org/Components/CCMContext/get_caller_principal:1.0
    //
    virtual Components::Principal_ptr get_caller_principal()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/CCMContext/get_CCM_home:1.0
    //
    virtual Components::CCMHome_ptr get_CCM_home()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/CCMContext/get_rollback_only:1.0
    //
    virtual CORBA::Boolean get_rollback_only()
        throw(Components::IllegalState,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/CCMContext/get_user_transaction:1.0
    //
    virtual Components::Transaction::UserTransaction_ptr get_user_transaction()
        throw(Components::IllegalState,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/CCMContext/is_caller_in_role:1.0
    //
    virtual CORBA::Boolean is_caller_in_role(const char* role)
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/CCMContext/set_rollback_only:1.0
    //
    virtual void set_rollback_only()
        throw(Components::IllegalState,
              CORBA::SystemException);

private:
    CCMContext_impl();
    CCMContext_impl(const CCMContext_impl&);
    void operator=(const CCMContext_impl&);
     
    Components::CCMContext_var m_context;   // container context

};  // End CCMContext_impl class

} // End namespace CIF

} // End namespace CCM

} // End namespce Cdmw


#endif  // INCL_CCM_CIF_CCMCONTEXT_IMPL_HPP

