/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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


#ifndef ___CCM_LB_TEST_CCM_CLIENTHOME_FT_IMPL_HPP__
#define ___CCM_LB_TEST_CCM_CLIENTHOME_FT_IMPL_HPP__


#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include <ccm_lb/ccm_lb_cif.skel.hpp>


namespace CcmLbTest
{

//
// IDL:thalesgroup.com/CcmLbTest/CCM_ClientHome:1.0
//
class CCM_ClientHome_FT_impl : virtual public ::CcmLbTest::CCM_ClientHome,
                            virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    private:
        CCM_ClientHome_FT_impl(const CCM_ClientHome_FT_impl&);
        void operator=(const CCM_ClientHome_FT_impl&);

    public:
        CCM_ClientHome_FT_impl();
        ~CCM_ClientHome_FT_impl();

        //
        // IDL:thalesgroup.com/CcmLbTest/CCM_ClientHomeImplicit/create:1.0
        //
        virtual Components::EnterpriseComponent_ptr
        create()
            throw(Components::CCMException,
                  CORBA::SystemException);
};

}; // End of namespace CcmLbTest


#endif // ___CCM_LB_TEST_CCM_CLIENTHOME_FT_IMPL_HPP__
