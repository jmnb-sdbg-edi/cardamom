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

#ifndef ___HELLO_SERVER_EXECUTOR_IMPL_HPP__
#define ___HELLO_SERVER_EXECUTOR_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include "Hello_cif.skel.hpp"


namespace Hello
{
    
//
// IDL:thalesgroup.com/Hello/CCM_Server:1.0
//
class ServerExecutor_impl : virtual public CCM_Server, 
                            virtual public Cdmw::OrbSupport::RefCountLocalObject
{
  public:

    ServerExecutor_impl();
    
    ~ServerExecutor_impl();
    
    //
    // IDL:thalesgroup.com/Hello/CCM_Server/get_server_facet:1.0
    //
    virtual CCM_Display_ptr get_server_facet()
        throw(CORBA::SystemException);
       
  private:
  
    CCM_Display_var m_display;
    
    
    ServerExecutor_impl(const ServerExecutor_impl&);
    
    void operator=(const ServerExecutor_impl&);
        
};


}; // End of namespace Hello

#endif // ___HELLO_SERVER_EXECUTOR_IMPL_HPP__
