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


#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#include <math.h>


#include "SingleRootBinome_impl.hpp"


namespace Cdmw
{
namespace Tutorial
{

//
// constructor
//
SingleRootBinome_impl::SingleRootBinome_impl (const Cdmw::LifeCycle::ObjectDeactivator & obj_d,
                                              CORBA::Double a, CORBA::Double b)
    throw(CORBA::SystemException)
    : Cdmw::LifeCycle::LifeCycleSingleObject(obj_d), m_a(a), m_b(b)
{
}


//
// destructor
//
SingleRootBinome_impl::~SingleRootBinome_impl()
    throw()
{
}


//
// Compute roots of equations
//
void SingleRootBinome_impl::computeRoots()
    throw(CORBA::SystemException)
{
    m_x1 = -m_b / (2*m_a);
}


//
// Print the roots
//
void SingleRootBinome_impl::printRoots()
    throw(CORBA::SystemException)
{
    std::cout << "x1 = " << m_x1 << std::endl;
    std::cout << std::endl;
}


  
    
//
// Check if value is in roots interval
//
CORBA::Boolean SingleRootBinome_impl::isInBracket(CORBA::Double x)
    throw(CORBA::SystemException)
{
    return (x == m_x1);   
}

}; // End namespace Tutorial
}; // End namespace Cdmw
