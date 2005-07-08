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



#include "ForkFacetExecutor_impl.hpp"


namespace Dinner 
{


//
// IDL:acme.com/Dinner/CCM_Fork:1.0
//

ForkFacetExecutor_impl::ForkFacetExecutor_impl()
{
	 m_inuse = false;
}

ForkFacetExecutor_impl::~ForkFacetExecutor_impl()
{
    std::cout << "ForkFacetExecutor_impl::~ForkFacetExecutor_impl called!" << std::endl;
}

//
// IDL:acme.com/Dinner/Fork/get:1.0
//
void ForkFacetExecutor_impl::get()
   throw(InUse, CORBA::SystemException)
{
   // if fork is already used throw the exception
   if (m_inuse) 
   {
	   throw InUse ();
   }

   // set fork as used
   m_inuse = true;
}

//
// IDL:acme.com/Dinner/Fork/release:1.0
//
void ForkFacetExecutor_impl::release()
   throw(CORBA::SystemException)
{
    // assert if fork is not used
    assert (m_inuse);
    
    // reset fork as unused
    m_inuse = false;	
}

} // End namespace Dinner
