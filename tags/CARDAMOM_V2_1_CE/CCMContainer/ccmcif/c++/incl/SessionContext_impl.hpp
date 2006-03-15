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


#ifndef INCL_CCM_CIF_SESSIONCONTEXT_IMPL_HPP
#define INCL_CCM_CIF_SESSIONCONTEXT_IMPL_HPP

#include <CCMContainer/idllib/Components.skel.hpp>
#include <CCMContainer/ccmcif/CCMContext_impl.hpp>




namespace Cdmw {

namespace CCM {

namespace CIF {


/**
*Purpose:
*<p>    [class description]
*    This class implement the Components::SessionContext interface.
*    It will be inherited by all session components context implementation.
*
*/

//
// IDL:omg.org/Components/SessionContext:1.0
//
class SessionContext_impl : virtual public Components::SessionContext,
                            public CCMContext_impl
{
public:

    SessionContext_impl(Components::CCMContext_ptr ctx)
        throw (CORBA::SystemException);

     ~SessionContext_impl();

    //
    // IDL:omg.org/Components/SessionContext/get_CCM_object:1.0
    //
    virtual CORBA::Object_ptr get_CCM_object()
        throw(Components::IllegalState,
              CORBA::SystemException);

private:
    SessionContext_impl();
    SessionContext_impl(const SessionContext_impl&);
    void operator=(const SessionContext_impl&);

    Components::SessionContext_var m_session_context;  // session context given by the container

};  // End SessionContext class

} // End namespace CIF

} // End namespace CCM

} // End namespace Cdmw


#endif  // INCL_CCM_CIF_SESSIONCONTEXT_IMPL_HPP



