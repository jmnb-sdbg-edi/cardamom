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

#include <stdio.h>
#include <iostream>

#include <ccm_philo/CCM_Registrator_impl.hpp>


//
// IDL:acme.com/Dinner/CCM_Registrator:1.0
//
Dinner::CCM_Registrator_impl::CCM_Registrator_impl()
{
	m_philosophers = 0;
}

Dinner::CCM_Registrator_impl::~CCM_Registrator_impl()
{
    std::cout << "CCM_Registrator_impl::~CCM_Registrator_impl called!" << std::endl;
}

//
char*
Dinner::CCM_Registrator_impl::_cxx_register()
    throw(CORBA::SystemException)
{
	 char * philosophers[] = {
	"Socrates", "Descartes", "Pascal", "Plato", "Aristotle", "Pythagoras", "Zeno",
	"Heraclitus", "Democritus", "Descartes", "Kant", "Protagoras"
      };

      if (m_philosophers <= 12) 
		{
	       return CORBA::string_dup (philosophers[m_philosophers++]);
      }
		
      char tmp[42];
      sprintf (tmp, "Philosopher # %lu", (long unsigned int)m_philosophers);
		m_philosophers++;
      
		return CORBA::string_dup (tmp);
}

