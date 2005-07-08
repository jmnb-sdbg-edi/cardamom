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


#include "LifeCycle/lifecycle/ObjectDeactivator.hpp"
#include "Foundation/common/Assert.hpp"

namespace Cdmw
{

    namespace LifeCycle
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        //ObjectDeactivator::ObjectDeactivator()
        //    throw(CORBA::SystemException)
        //    : m_current(PortableServer::Current::_nil())
        //{
        //
        //}

        ObjectDeactivator::ObjectDeactivator(CORBA::ORB_ptr orb)
        throw(CORBA::SystemException)
                : m_current(PortableServer::Current::_nil())
        {
            CDMW_ASSERT(!CORBA::is_nil(orb));
            CORBA::Object_var obj =
                orb->resolve_initial_references("POACurrent");
            m_current = PortableServer::Current::_narrow(obj.in());
        }

        ObjectDeactivator::~ObjectDeactivator()
        throw()
        {}

        void
        ObjectDeactivator::deactivate_object() const
        throw(PortableServer::Current::NoContext,
              CORBA::SystemException)
        {
            PortableServer::POA_var poa = m_current->get_POA();
            PortableServer::ObjectId_var oid = m_current->get_object_id();

            deactivate_object(poa.in(), oid.in());
        }

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw

