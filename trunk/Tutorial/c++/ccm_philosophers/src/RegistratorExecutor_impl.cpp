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


#include <stdio.h>

#include "RegistratorExecutor_impl.hpp"

namespace Dinner 
{
  
  
char * RegistratorExecutor_impl::m_philosophers_name[] =
                  {"Plato",
                   "Socrates", 
                   "Descartes", 
                   "Pascal", 
                   "Aristotle", 
                   "Pythagoras", 
                   "Zeno",
	               "Heraclitus", 
	               "Democritus",  
	               "Kant", 
	               "Protagoras"
                  };

  
//
// IDL:acme.com/Dinner/CCM_Registrator:1.0
//
RegistratorExecutor_impl::RegistratorExecutor_impl()
{
	m_philosophers = 0;
}


RegistratorExecutor_impl::~RegistratorExecutor_impl()
{
    std::cout << "RegistratorExecutor_impl::~RegistratorExecutor_impl called!" << std::endl;
}


//
// IDL:acme.com/Dinner/Registrator/register:1.0
//
char* RegistratorExecutor_impl::_cxx_register()
    throw(CORBA::SystemException)
{
    // return the next philosopher name
    
    char tmp[42];
    
    if (m_philosophers <= 10) 
    {
        sprintf (tmp, m_philosophers_name[m_philosophers]);       
    }
    else
    {
        sprintf (tmp, "Philosopher # %lu", (unsigned long) m_philosophers);
    }
    
    m_philosophers++;
      
    return CORBA::string_dup (tmp);
}

} // End namespace Dinner
