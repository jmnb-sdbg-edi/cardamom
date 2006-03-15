/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#ifndef __INCL_CCMCIF_NONCDMWSTATUSINFOCONSUMER_IMPL_HPP__
#define __INCL_CCMCIF_NONCDMWSTATUSINFOCONSUMER_IMPL_HPP__

#include "testmonophilosophers/Philo_cif.skel.hpp"
#include "Foundation/orbsupport/RefCountServantBase.hpp"
#include "CCMContainer/ccmcif/LifeCycleObjectBase.hpp"

   
namespace Cdmw {

namespace CCM {

namespace CIF {


   // Forward declaration
   class CCMObject_impl;

   namespace Dinner { 

   class NonCdmwStatusInfoConsumer_impl : virtual public POA_Dinner::StatusInfoConsumer,
               virtual public OrbSupport::RefCountServantBase
   {
      public:
         NonCdmwStatusInfoConsumer_impl(PortableServer::POA_ptr  poa);
         ~NonCdmwStatusInfoConsumer_impl();     
             
            virtual PortableServer::POA_ptr _default_POA()                 
         {
            return PortableServer::POA::_duplicate(m_poa.in());
         };


         virtual void
             push_StatusInfo(::Dinner::StatusInfo* the_statusinfo)
             throw (CORBA::SystemException);

         virtual void 
             push_event(Components::EventBase * evt)
             throw (Components::BadEventType, CORBA::SystemException);

      private:
         NonCdmwStatusInfoConsumer_impl(const NonCdmwStatusInfoConsumer_impl&);
         void operator=(const NonCdmwStatusInfoConsumer_impl&);

            PortableServer::POA_var m_poa;

   };

   
} // end of namespace Dinner

} // end of namespace CIF

} // end of namespace CCM

} // end of namespace Cdmw

#endif // __INCL_CCMCIF_STATUSINFOCONSUMER_IMPL_HPP__

