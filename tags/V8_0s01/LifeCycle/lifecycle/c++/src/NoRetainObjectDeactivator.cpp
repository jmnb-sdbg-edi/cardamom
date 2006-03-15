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


#include "LifeCycle/lifecycle/NoRetainObjectDeactivator.hpp"


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


        NoRetainObjectDeactivator::NoRetainObjectDeactivator(CORBA::ORB_ptr orb)
        throw(CORBA::SystemException)
                : ObjectDeactivator(orb)
        {}

        NoRetainObjectDeactivator::~NoRetainObjectDeactivator()
        throw()
        {}

        // This is called within a context of a request invocation
        void
        NoRetainObjectDeactivator::deactivate_object(PortableServer::POA_ptr poa,
                const PortableServer::ObjectId& oid) const
        throw(CORBA::SystemException)
        {
            // Does nothing for NO_RETAIN policy since the POA has no Active Objects Map
            // A call to poa->deactivate_object(oid) would raise PortableServer::POA::ObjectNotActive
            // exception
        }

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw

