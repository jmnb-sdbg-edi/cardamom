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

#ifndef ___DINNER_CCM_FORK_IMPL_HPP__
#define ___DINNER_CCM_FORK_IMPL_HPP__


#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <ccm_philo/Philo_cif.skel.hpp>


namespace Dinner
{

//
// IDL:acme.com/Dinner/CCM_Fork:1.0
//
class CCM_Fork_impl : virtual public ::Dinner::CCM_Fork,
                         virtual public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    CCM_Fork_impl();
    ~CCM_Fork_impl();

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

private:
    CCM_Fork_impl(const CCM_Fork_impl&);
    void operator=(const CCM_Fork_impl&);

	 // Attributes
    bool m_inuse;

};

}; // End of namespace Dinner

#endif // ___DINNER_CCM_FORK_IMPL_HPP__

