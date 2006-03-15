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


#ifndef INCL_SYSTEMMNGT_NAMINGCONTEXT_IMPL_HPP
#define INCL_SYSTEMMNGT_NAMINGCONTEXT_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.skel.hpp"
#include "platformdaemon/ServiceRegistration_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


/**
*Purpose:
*<p>
*Actual read/write implementation of the CosNaming::NamingContext
*IDL interface.
*<p>
*Features:
*<p>
*Thread safe.
*/
class NamingContext_impl 
    : virtual public POA_CosNaming::NamingContextExt,
      virtual public PortableServer::RefCountServantBase
{

public:

    /**
    * Purpose:
    * <p>
    * Creates this instance.
    */
    NamingContext_impl(ServiceRegistration_impl* serviceRegistration);

    /**
    * Purpose:
    * <p>
    * Destroys this instance. If the flag m_destroyed is true, the persistent
    * data is actually destroyed from the persistent storage.
    */
    virtual ~NamingContext_impl();

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/bind:1.0
    * operation
    */
    virtual void bind(const CosNaming::Name& n,
                      CORBA::Object_ptr obj)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CosNaming::NamingContext::AlreadyBound,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/rebind:1.0
    * operation
    */
    virtual void rebind(const CosNaming::Name& n,
                        CORBA::Object_ptr obj)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/bind_context:1.0
    * operation
    */
    virtual void bind_context(const CosNaming::Name& n,
                              CosNaming::NamingContext_ptr nc)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CosNaming::NamingContext::AlreadyBound,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/rebind_context:1.0
    * operation
    */
    virtual void rebind_context(const CosNaming::Name& n,
                                CosNaming::NamingContext_ptr nc)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/resolve:1.0
    * operation
    */
    virtual CORBA::Object_ptr resolve(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/unbind:1.0
    * operation
    */
    virtual void unbind(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/new_context:1.0
    * operation
    */
    virtual CosNaming::NamingContext_ptr new_context()
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/bind_new_context:1.0
    * operation
    */
    virtual CosNaming::NamingContext_ptr bind_new_context(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::AlreadyBound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/destroy:1.0
    * operation
    */
    virtual void destroy()
        throw(CosNaming::NamingContext::NotEmpty,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContext/list:1.0
    * operation
    */
    virtual void list(CORBA::ULong how_many,
                      CosNaming::BindingList_out bl,
                      CosNaming::BindingIterator_out bi)
        throw(CORBA::SystemException);  


    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContextExt/to_string:1.0
    * operation
    */
    virtual CosNaming::NamingContextExt::StringName to_string(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContextExt/to_name:1.0
    * operation
    */
    virtual CosNaming::Name* to_name(const char* sn)
        throw(CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContextExt/to_url:1.0
    * operation
    *
    * Not yet implemented.
    */
    virtual CosNaming::NamingContextExt::URLString to_url(const char* addr,
                                                          const char* sn)
        throw(CosNaming::NamingContextExt::InvalidAddress,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosNaming/NamingContextExt/resolve_str:1.0
    * operation
    */
    virtual CORBA::Object_ptr resolve_str(const char* sn)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException);


private:
    /**
     * the servide Registration.
     */
    ServiceRegistration_impl* m_serviceRegistration;


}; //End of NamingContext_impl

} // End of namespace PlatformMngt
} // End of namespace Cdmw

#endif

