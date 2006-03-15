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


#ifndef __CCM_SERVER_IMPL_HPP__
#define __CCM_SERVER_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include <ccm_unaware/SimpleCcmServer_cif.skel.hpp>

#include <ccm_unaware/CCM_Display_impl.hpp>

namespace SimpleCcmServer
{
//
// IDL:thalesgroup.com/SimpleCcmServer/CCM_Server:1.0
//
class CCM_Server_impl : virtual public ::SimpleCcmServer::CCM_Server, 
                        virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    CCM_Server_impl(const CCM_Server_impl&);
    void operator=(const CCM_Server_impl&);
	CORBA::String_var             m_name;
	::SimpleCcmServer::CCM_Display_var  m_display;


public:

    CCM_Server_impl();
    ~CCM_Server_impl();

    //
    // IDL:thalesgroup.com/SimpleCcmServer/CCM_Server/get_for_clients:1.0
    //
    virtual ::SimpleCcmServer::CCM_Display_ptr get_for_clients()
        throw(CORBA::SystemException);


    //
    // IDL:thalesgroup.com/SimpleCcmServer/CCM_Server/get_name:1.0
    //
    virtual char* get_name()
        throw(CORBA::SystemException);

};


}; // End of namespace SimpleCcmServer

#endif // __CCM_SERVER_IMPL_HPP__

