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

#ifndef ___DINNER_CCM_FORKMANAGER_IMPL_HPP__
#define ___DINNER_CCM_FORKMANAGER_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

//#include <ccm_philo/Philo_cif.skel.hpp>
#include "ccm_philo/UserPhilo.skel.hpp"
#include <ccm_philo/CCM_Fork_impl.hpp>

namespace Dinner
{
//
// IDL:thalesgroup.com/Dinner/CCM_ForkManager:1.0
//
class CCM_ForkManager_impl : virtual public ::UserDinner::User_CCM_ForkManager, 
                             virtual public Cdmw::OrbSupport::RefCountLocalObject,
                        virtual public CCM_Fork_impl
{
    CCM_ForkManager_impl(const CCM_ForkManager_impl&);
    void operator=(const CCM_ForkManager_impl&);

public:

    CCM_ForkManager_impl();
    ~CCM_ForkManager_impl();

    //
    // IDL:thalesgroup.com/Dinner/CCM_ForkManager/get_the_fork:1.0
    //
    virtual ::Dinner::CCM_Fork_ptr get_the_fork()
        throw(CORBA::SystemException);

    //
    // IDL:acme.com/Dinner/Fork/get:1.0
    //
    virtual void get()
        throw(Dinner::InUse, CORBA::SystemException);

    //
    // IDL:acme.com/Dinner/Fork/release:1.0
    //
    virtual void release()
        throw(CORBA::SystemException);

};


}; // End of namespace Dinner

#endif // ___DINNER_CCM_FORKMANAGER_IMPL_HPP__

