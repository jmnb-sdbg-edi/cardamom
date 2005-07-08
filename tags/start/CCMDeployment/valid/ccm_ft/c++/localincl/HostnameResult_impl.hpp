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

#ifndef ___CCM_FT_TEST_HOSTNAME_RESULT_IMPL_HPP__
#define ___CCM_FT_TEST_HOSTNAME_RESULT_IMPL_HPP__

#include <list>

#include <Foundation/orbsupport/CORBA.hpp>
#include <ccm_ft/ccm_ft_cif.skel.hpp>


namespace {


} // End anonymous space


namespace CcmFtTest {


//
// IDL:omg.org/Components/HostnameResult:1.0
//
class HostnameResult_impl : virtual public OBV_CcmFtTest::HostnameResult,
                        virtual public CORBA::DefaultValueRefCountBase
{
    void operator=(const HostnameResult_impl&);

public:

    HostnameResult_impl();
    HostnameResult_impl(const HostnameResult_impl&);
    ~HostnameResult_impl();
    
    virtual CORBA::ValueBase* _copy_value();
    
};


class HostnameResultFactory : virtual public CORBA::ValueFactoryBase
{
    virtual CORBA::ValueBase* create_for_unmarshal();

public:
    HostnameResultFactory();
    virtual ~HostnameResultFactory();
};


} // End namespce CcmFtTest

#endif // ___CCM_FT_TEST_HOSTNAME_RESULT_IMPL_HPP__

