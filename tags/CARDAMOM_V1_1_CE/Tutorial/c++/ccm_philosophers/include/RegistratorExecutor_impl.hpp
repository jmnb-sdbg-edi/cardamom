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


#ifndef ___DINNER_CCM_REGISTRATOR_IMPL_HPP__
#define ___DINNER_CCM_REGISTRATOR_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include "Philo_cif.skel.hpp"


namespace Dinner
{
    
//
// IDL:acme.com/Dinner/CCM_Registrator:1.0
//
class RegistratorExecutor_impl : virtual public CCM_Registrator, 
                                 virtual public Cdmw::OrbSupport::RefCountLocalObject
{   
  public:

    RegistratorExecutor_impl();
    ~RegistratorExecutor_impl();

    //
    // IDL:acme.com/Dinner/Registration/register:1.0
    //
    virtual char* _cxx_register()
        throw(CORBA::SystemException);
        
  private:
  
    RegistratorExecutor_impl(const RegistratorExecutor_impl&);
    void operator=(const RegistratorExecutor_impl&);
    
    static char * m_philosophers_name[];

    CORBA::ULong m_philosophers;
  
};


}; // End of namespace Dinner

#endif // ___DINNER_CCM_REGISTRATOR_IMPL_HPP__
