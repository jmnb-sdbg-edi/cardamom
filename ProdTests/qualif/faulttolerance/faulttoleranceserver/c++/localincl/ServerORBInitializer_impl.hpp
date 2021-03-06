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

#ifndef _SERVER_ORB_INITIALIZER_HPP_
#define _SERVER_ORB_INITIALIZER_HPP_


#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/PortableInterceptor.hpp>

class ServerORBInitializer_impl : public PortableInterceptor::ORBInitializer,
                                  public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    //
    // IDL to C++ Mapping
    //
    virtual void
    pre_init(PortableInterceptor::ORBInitInfo_ptr info) throw (CORBA::SystemException);

    
    //
    // Note: the interceptors are registered in post_init()
    //       if their constructors require
    //       ORBInitInfo::resolve_initial_reference(), which XXX
    //       cannot be called in pre_init().
    //
    virtual void
    post_init(PortableInterceptor::ORBInitInfo_ptr info) throw (CORBA::SystemException);

    /// Return the created server request interceptor.  Only valid after
    /// post_init(), i.e. ORB_init(), has been called.
    PortableInterceptor::ServerRequestInterceptor_ptr server_interceptor (void);
    
private:

    /// Pointer to the server request interceptor.  ORB is responsible
    /// for storage.
    PortableInterceptor::ServerRequestInterceptor_var server_interceptor_;

};


#endif  // _SERVER_ORB_INITIALIZER_HPP_

