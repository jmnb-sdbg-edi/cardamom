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


#ifndef ___HELLO_SERVER_HOME_EXECUTOR_IMPL_HPP__
#define ___HELLO_SERVER_HOME_EXECUTOR_IMPL_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"

#include "Hello_cif.skel.hpp"


namespace Hello
{

//
// IDL:acme.com/Hello/CCM_ServerHome:1.0
//
class ServerHomeExecutor_impl : virtual public CCM_ServerHome,
                                virtual public Cdmw::OrbSupport::RefCountLocalObject
{   
  public:

    ServerHomeExecutor_impl();
    ~ServerHomeExecutor_impl();

    //
    // IDL:acme.com/Hello/CCM_ServerHomeImplicit/create:1.0
    //
    virtual Components::EnterpriseComponent_ptr create()
        throw(Components::CCMException,
              CORBA::SystemException);
              
  private:
  
    ServerHomeExecutor_impl(const ServerHomeExecutor_impl&);
    void operator=(const ServerHomeExecutor_impl&);

};

}; // End of namespace Hello 

#endif // ___HELLO_SERVER_HOME_EXECUTOR_IMPL_HPP__


