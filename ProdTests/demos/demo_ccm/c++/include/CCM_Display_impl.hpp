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


#ifndef ___BASICDEMO_CCM_DISPLAY_IMPL_HPP__
#define ___BASICDEMO_CCM_DISPLAY_IMPL_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"
#include <Basic_cif.skel.hpp>


namespace BasicDemo
{

//
// IDL:thalesgroup.com/BasicDemo/CCM_Display:1.0
//
class CCM_Display_impl : virtual public ::BasicDemo::CCM_Display,
                         virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    CCM_Display_impl(const CCM_Display_impl&);
    void operator=(const CCM_Display_impl&);

public:

    CCM_Display_impl();
    ~CCM_Display_impl();

    //
    // IDL:thalesgroup.com/BasicDemo/Display/print:1.0
    //
    virtual void print(const char* text)
        throw(CORBA::SystemException);

    //
    // IDL:thalesgroup.com/BasicDemo/Display/print_event:1.0
    //
    virtual void print_event(const BasicDemo::Event&)
        throw(CORBA::SystemException);
};

}; // End of namespace BasicDemo

#endif // ___BASICDEMO_CCM_DISPLAY_IMPL_HPP__

