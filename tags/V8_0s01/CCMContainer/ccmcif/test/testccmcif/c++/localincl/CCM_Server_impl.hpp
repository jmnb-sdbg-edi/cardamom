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


#ifndef ___BASICDEMO_CCM_SERVER_IMPL_HPP__
#define ___BASICDEMO_CCM_SERVER_IMPL_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"

#include "testccmcif/Basic_cif.skel.hpp"

#include "testccmcif/CCM_Display_impl.hpp"

namespace BasicDemo
{
//
// IDL:thalesgroup.com/BasicDemo/CCM_Server:1.0
//
class CCM_Server_impl : virtual public ::BasicDemo::CCM_Server, 
                        virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    CCM_Server_impl(const CCM_Server_impl&);
    void operator=(const CCM_Server_impl&);
    int                           m_indice;
     CORBA::String_var             m_name;
     ::BasicDemo::CCM_Display_var  m_display;


public:

    CCM_Server_impl();
    ~CCM_Server_impl();

    //
    // IDL:thalesgroup.com/BasicDemo/CCM_Server/get_for_clients:1.0
    //
    virtual ::BasicDemo::CCM_Display_ptr get_for_clients()
        throw(CORBA::SystemException);

    //
    // IDL:thalesgroup.com/BasicDemo/CCM_Server_Executor/name:1.0
    //
    virtual char* name()
        throw(CORBA::SystemException);
    virtual void name(const char*)
        throw(CORBA::SystemException);


    //
    // IDL:thalesgroup.com/BasicDemo/Registration/register:1.0
    //
    virtual char* _cxx_register()
        throw(CORBA::SystemException);
};


}; // End of namespace BasicDemo

#endif // ___BASICDEMO_CCM_SERVER_IMPL_HPP__

