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

#include <iostream>

#include "testphilosophers/CCM_Fork_impl.hpp"

//
// IDL:acme.com/Dinner/CCM_Fork:1.0
//
Dinner::CCM_Fork_impl::CCM_Fork_impl()
{
     m_inuse = false;
}

Dinner::CCM_Fork_impl::~CCM_Fork_impl()
{
    std::cout << "CCM_Fork_impl::~CCM_Fork_impl called!" << std::endl;
}

//
// IDL:acme.com/Dinner/Fork/get:1.0
//
void 
Dinner::CCM_Fork_impl::get()
   throw(Dinner::InUse, CORBA::SystemException)
{
   if (m_inuse) 
    {
       throw Dinner::InUse ();
   }

   m_inuse = true;
}

//
// IDL:acme.com/Dinner/Fork/release:1.0
//
void 
Dinner::CCM_Fork_impl::release()
   throw(CORBA::SystemException)
{
    assert (m_inuse);
    m_inuse = false;    
}


