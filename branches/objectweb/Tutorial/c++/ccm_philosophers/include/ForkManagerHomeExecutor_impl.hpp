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


#ifndef ___DINNER_CCM_ForkManagerHome_IMPL_HPP__
#define ___DINNER_CCM_ForkManagerHome_IMPL_HPP__


#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include "Philo_cif.skel.hpp"


namespace Dinner
{

//
// IDL:thalesgroup.com/Dinner/CCM_ForkManagerHome:1.0
//
class ForkManagerHomeExecutor_impl : virtual public CCM_ForkManagerHome,
                                     virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    
  public:

    ForkManagerHomeExecutor_impl();
    ~ForkManagerHomeExecutor_impl();

    //
    // IDL:thalesgroup.com/Dinner/CCM_ForkManagerHomeImplicit/create:1.0
    //
    virtual Components::EnterpriseComponent_ptr create()
        throw(Components::CCMException,
              CORBA::SystemException);
              
  private:
  
    ForkManagerHomeExecutor_impl(const ForkManagerHomeExecutor_impl&);
    void operator=(const ForkManagerHomeExecutor_impl&);

  
};

}; // End of namespace Dinner 

#endif // ___DINNER_CCM_ForkManagerHome_IMPL_HPP__
