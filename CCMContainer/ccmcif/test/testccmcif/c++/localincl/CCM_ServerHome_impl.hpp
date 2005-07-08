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


#ifndef ___BASICDEMO_CCM_SERVERHOME_IMPL_HPP__
#define ___BASICDEMO_CCM_SERVERHOME_IMPL_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"
#include "testccmcif/Basic_cif.skel.hpp"


namespace BasicDemo
{

//
// IDL:thalesgroup.com/BasicDemo/CCM_ServerHome:1.0
//
class CCM_ServerHome_impl : virtual public ::BasicDemo::CCM_ServerHome,
                            virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    CCM_ServerHome_impl(const CCM_ServerHome_impl&);
    void operator=(const CCM_ServerHome_impl&);

public:

    CCM_ServerHome_impl();
    ~CCM_ServerHome_impl();

    //
    // IDL:thalesgroup.com/BasicDemo/CCM_NamedComponentHomeExplicit/home_name:1.0
    //
    virtual char* home_name()
        throw(CORBA::SystemException);

    virtual void home_name(const char*)
        throw(CORBA::SystemException);

    //
    // IDL:thalesgroup.com/BasicDemo/CCM_NamedComponentHomeExplicit/create_named_component:1.0
    //
     virtual Components::EnterpriseComponent_ptr create_named_component(const char* name)
        throw(CORBA::SystemException);

    //
    // IDL:thalesgroup.com/BasicDemo/CCM_ServerHomeImplicit/create:1.0
    //
    virtual Components::EnterpriseComponent_ptr create()
        throw(Components::CCMException,
              CORBA::SystemException);

private:

    CORBA::String_var m_name;

};

}; // End of namespace BasicDemo 

#endif // ___BASICDEMO_CCM_SERVERHOME_IMPL_HPP__

