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

#ifndef ___HELLO_CLIENT_EXECUTOR_IMPL_HPP__
#define ___HELLO_CLIENT_EXECUTOR_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include "UserHello.skel.hpp"

namespace Hello
{

//
// IDL:thalesgroup.com/Hello/CCM_Client:1.0
//
class ClientExecutor_impl : virtual public User_Client,
                            virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    	 
  public:

    ClientExecutor_impl();
    
    ~ClientExecutor_impl();
    

    void set_session_context(Components::SessionContext_ptr ctx)
		         throw(CORBA::SystemException);
    void ccm_activate()
		         throw(CORBA::SystemException);
    void ccm_passivate()
		         throw(CORBA::SystemException);
    void ccm_remove()
		         throw(CORBA::SystemException);

    virtual char* name()
                 throw(CORBA::SystemException);
        
     
    void set_name(const char* name);

    
  private:
  
    ClientExecutor_impl(const ClientExecutor_impl&);
    
    void operator=(const ClientExecutor_impl&);

    CCM_Client_Context_var m_session_context;
    
    CORBA::String_var m_name;
    
    
};

}; // End of namespace Hello


#endif // ___HELLO_CLIENT_EXECUTOR_IMPL_HPP__
