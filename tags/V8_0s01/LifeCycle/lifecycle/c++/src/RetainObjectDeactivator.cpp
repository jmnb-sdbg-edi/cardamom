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


#include "LifeCycle/lifecycle/RetainObjectDeactivator.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"


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


        RetainObjectDeactivator::RetainObjectDeactivator(CORBA::ORB_ptr orb)
        throw(CORBA::SystemException)
                : ObjectDeactivator(orb)
        {}

        RetainObjectDeactivator::~RetainObjectDeactivator()
        throw()
        {}

        // This is called within a context of a request invocation
        void
        RetainObjectDeactivator::deactivate_object(PortableServer::POA_ptr poa,
                const PortableServer::ObjectId& oid) const
        throw(CORBA::SystemException)
        {
            // This is used with a POA with RETAIN policy

            try
            {
                poa->deactivate_object(oid);
            }
            catch (const PortableServer::POA::ObjectNotActive & )
            {
                throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                      CORBA::COMPLETED_NO);
                // This should not happen with RETAIN policy and within a request invocation
            }
            catch (const PortableServer::POA::WrongPolicy & )
            {
                throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                      CORBA::COMPLETED_NO);
                // This should not happen with RETAIN policy
            }
            catch (const CORBA::SystemException & )
            {
                // rethrow
                throw;
            }
        }

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw

