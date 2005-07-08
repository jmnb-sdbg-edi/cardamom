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

   
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
   
#include "testphilosophers/NonCdmwStatusInfoConsumer_impl.hpp"
#include "CCMContainer/ccmcommon/CCMUtils.hpp"
   
   
namespace Cdmw {

namespace CCM {

namespace CIF {

namespace Dinner {

NonCdmwStatusInfoConsumer_impl::NonCdmwStatusInfoConsumer_impl(PortableServer::POA_ptr  poa)
    : m_poa(PortableServer::POA::_duplicate(poa))
   {
   }

   NonCdmwStatusInfoConsumer_impl::~NonCdmwStatusInfoConsumer_impl()
   {
      PRINT_INFO("NonCdmwStatusInfoConsumer destructor called");
   }

   void
   NonCdmwStatusInfoConsumer_impl::push_StatusInfo(::Dinner::StatusInfo* the_statusinfo)
      throw (CORBA::SystemException)
   {
      PRINT_INFO("NonCdmwStatusInfoConsumer received a status info message");

        std::cout << "NonCdmwStatusInfoConsumer: " << the_statusinfo->name() << ": " ;

      switch (the_statusinfo->state()) 
       {
            case ::Dinner::EATING:   
              std::cout << "Eating     "; 
                break;
            case ::Dinner::THINKING: 
              std::cout <<  "Thinking   "; 
              break;
            case ::Dinner::HUNGRY:   
              std::cout <<  "Hungry     "; 
              break;
            case ::Dinner::STARVING: 
              std::cout <<  "Starving   "; 
              break;
            case ::Dinner::DEAD:     
              std::cout <<  "Dead       "; 
              break;
      }

      if (the_statusinfo->has_left_fork()) 
       {
            std::cout << "* ";
      }
      else 
       {
           std::cout << "  ";
      }

      if (the_statusinfo->has_right_fork()) 
       {
            std::cout << "* ";
      }
      else 
       {
            std::cout << "  ";
      }

       std::cout <<  the_statusinfo->secs_since_last_meal() << "s since last meal" << std::endl;

       std::cout << std::endl;
   }

   void 
   NonCdmwStatusInfoConsumer_impl::push_event(Components::EventBase * evt)
      throw (Components::BadEventType, CORBA::SystemException)
   {
      PRINT_INFO("NonCdmwStatusInfoConsumer received an event");

        if (!evt)
      {
         PRINT_INFO("NonCdmwStatusInfoConsumer received a nil event type");
         throw Components::BadEventType();
      }

      ::Dinner::StatusInfo_var info_evt 
         = ::Dinner::StatusInfo::_downcast(evt);
      CORBA::add_ref(info_evt);

      if (!info_evt.in())
      {
         PRINT_INFO("NonCdmwStatusInfoConsumer received a bad event type");
         throw Components::BadEventType();
      }
      else
      {
         PRINT_INFO("NonCdmwStatusInfoConsumer push_StatusInfo called");
         this->push_StatusInfo(info_evt.in());
      }
   }

} // end of namespace Dinner

} // end of namespace CIF

} // end of namespace CCM

} // end of namespace Cdmw

