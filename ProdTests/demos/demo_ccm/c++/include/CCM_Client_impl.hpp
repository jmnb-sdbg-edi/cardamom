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


#ifndef ___BASICDEMO_CCM_CLIENT_IMPL_HPP__
#define ___BASICDEMO_CCM_CLIENT_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <UserBasicDemo.skel.hpp>

namespace BasicDemo
{

//
// IDL:thalesgroup.com/BasicDemo/CCM_Client:1.0
//
class CCM_Client_impl : virtual public ::BasicDemo::User_CCM_Client,
                        virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    CCM_Client_impl(const CCM_Client_impl&);
    void operator=(const CCM_Client_impl&);

    CORBA::String_var                   m_name;
    CORBA::String_var                   m_text;
    ::BasicDemo::CCM_Client_Context_var m_session_context;
	 
public:

    CCM_Client_impl();
    ~CCM_Client_impl();
    //
    // IDL:thalesgroup.com/BasicDemo/CCM_Client_Executor/name:1.0
    //
    virtual char* name()
        throw(CORBA::SystemException);
	 
    //
    // IDL:thalesgroup.com/BasicDemo/CCM_Client_Executor/event:1.0
    //
    virtual BasicDemo::Event * event()
        throw(CORBA::SystemException);
    virtual void event(const BasicDemo::Event&)
        throw(CORBA::SystemException);
	 
	 

    void set_session_context(Components::SessionContext_ptr ctx)
		         throw(CORBA::SystemException);
    void ccm_activate()
		         throw(CORBA::SystemException);
    void ccm_passivate()
		         throw(CORBA::SystemException);
    void ccm_remove()
		         throw(CORBA::SystemException);

    // additional operation
    void set_name(const char* name)
    {
        m_name = CORBA::string_dup(name);
    }
    
    ::BasicDemo::CCM_Client_Context* get_context()
    {
        return ::BasicDemo::CCM_Client_Context::_duplicate(m_session_context.in());
    }

    char* get_text()
    {
        return CORBA::string_dup(m_text.in());
    }


};

}; // End of namespace BasicDemo


#endif // ___BASICDEMO_CCM_CLIENT_IMPL_HPP__

