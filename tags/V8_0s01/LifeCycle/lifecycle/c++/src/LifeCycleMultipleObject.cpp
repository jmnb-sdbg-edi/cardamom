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


#include "LifeCycle/lifecycle/LifeCycleMultipleObject.hpp"
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

        // LifeCycleMultipleObject::LifeCycleMultipleObject()
        //     throw(CORBA::SystemException)
        //     : LifeCycleObjectBase()
        // {
        // }

        LifeCycleMultipleObject::LifeCycleMultipleObject(const ObjectDeactivator & deactivator)
        throw(CORBA::SystemException)
                : LifeCycleObjectBase(deactivator)
        {}

        LifeCycleMultipleObject::~LifeCycleMultipleObject()
        throw()
        {
            // do nothing
        }

        // Returns true if object with oid is removed
        bool
        LifeCycleMultipleObject::is_removed(const PortableServer::ObjectId & oid) const
        throw()
        {
            bool result = false;

            try
            {
                CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
                RemovedObjectSet::const_iterator pos = m_ros.find(s.in());

                if (pos != m_ros.end())
                    result = true;
            }
            catch (const std::bad_alloc & )
            {
                // TODO
            }
            catch (const CORBA::BAD_PARAM & )
            {
                // TODO
            }

            return result;
        }

        // This registers oid as removed.
        void
        LifeCycleMultipleObject::register_object_as_removed(const PortableServer::ObjectId & oid)
        throw()
        {
            try
            {
                CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
                m_ros.insert(s.in());
            }
            catch (const std::bad_alloc & )
            {
                // TODO
            }
            catch (const CORBA::BAD_PARAM & )
            {
                // TODO
            }
        }


    }

    ; // End namespace Entity
}

; // End namespace Cdmw

