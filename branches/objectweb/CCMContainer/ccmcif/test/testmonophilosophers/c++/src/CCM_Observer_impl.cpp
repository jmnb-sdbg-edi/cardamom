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

#include "testmonophilosophers/CCM_Observer_impl.hpp"

#include "testmonophilosophers/StatusInfo_impl.hpp"


//
// IDL:acme.com/Dinner/CCM_Observer:1.0
//
Dinner::CCM_Observer_impl::CCM_Observer_impl()
{
    m_indice = 0;
    m_name = "";
}

Dinner::CCM_Observer_impl::~CCM_Observer_impl()
{
    std::cout << "CCM_Observer_impl::~CCM_Observer_impl called!" << std::endl;
}

//
// IDL:acme.com/Dinner/Observer/push_info:1.0
//
void
Dinner::CCM_Observer_impl::push_info (Dinner::StatusInfo* event)
    throw(CORBA::SystemException)
{
    std::cout << "Observer: " << event->name() << ": " ;

    switch (event->state()) 
     {
      case Dinner::EATING:   
            std::cout << "Eating     "; 
            break;
      case Dinner::THINKING: 
            std::cout <<  "Thinking   "; 
            break;
      case Dinner::HUNGRY:   
            std::cout <<  "Hungry     "; 
            break;
      case Dinner::STARVING: 
            std::cout <<  "Starving   "; 
            break;
      case Dinner::DEAD:     
            std::cout <<  "Dead       "; 
            break;
    }

    if (event->has_left_fork()) 
     {
          std::cout << "* ";
    }
    else 
     {
         std::cout << "  ";
    }

    if (event->has_right_fork()) 
     {
          std::cout << "* ";
    }
    else 
     {
          std::cout << "  ";
    }

     std::cout <<  event->secs_since_last_meal() << "s since last meal" << std::endl;

     std::cout << std::endl;
}

