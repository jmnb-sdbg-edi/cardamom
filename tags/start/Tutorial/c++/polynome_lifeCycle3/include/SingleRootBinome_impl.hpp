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


#ifndef INCL_SINGLEROOTBINOME_HPP
#define INCL_SINGLEROOTBINOME_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <LifeCycle/lifecycle/LifeCycleSingleObject.hpp>

#include "PolynomeModule.skel.hpp"


namespace Cdmw
{
namespace Tutorial
{


class SingleRootBinome_impl :
        public virtual POA_PolynomeModule::Polynome,
        public Cdmw::LifeCycle::LifeCycleSingleObject,
        public virtual Cdmw::OrbSupport::RefCountServantBase
{ 
    
  public:

    SingleRootBinome_impl (const Cdmw::LifeCycle::ObjectDeactivator & obj_d,
                           CORBA::Double a, 
                           CORBA::Double b)
        throw(CORBA::SystemException);
                    
                    
    ~SingleRootBinome_impl()
        throw();

    //
    // Compute roots of equations
    //
    virtual void computeRoots()
        throw(CORBA::SystemException);
    
    
    //
    // Print the roots
    //
    virtual void printRoots()
        throw(CORBA::SystemException);
    
    
    //
    // Check if value is in roots interval
    //
    virtual CORBA::Boolean isInBracket(CORBA::Double x)
        throw(CORBA::SystemException);
    

  private:
  
    SingleRootBinome_impl(const SingleRootBinome_impl&);
    void operator=(const SingleRootBinome_impl&);
    
    // polynome coefficients
    CORBA::Double m_a;
    CORBA::Double m_b;

    // roots
    CORBA::Double m_x1;

};

}; // End namespace Tutorial
}; // End namespace Cdmw

#endif // INCL_SINGLEROOTBINOME_HPP

