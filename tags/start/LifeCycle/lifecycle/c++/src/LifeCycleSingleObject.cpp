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


#include "LifeCycle/lifecycle/LifeCycleSingleObject.hpp"
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

        //LifeCycleSingleObject::LifeCycleSingleObject()
        //    throw(CORBA::SystemException)
        //    : LifeCycleObjectBase(),
        //      m_removed(false)
        //{
        ////     std::cerr
        ////         << "LifeCycleSingleObject::LifeCycleSingleObject()"
        ////         << std::endl;
        //}

        LifeCycleSingleObject::LifeCycleSingleObject(const ObjectDeactivator & deactivator)
        throw(CORBA::SystemException)
                : LifeCycleObjectBase(deactivator),
                m_removed(false)
        {
            //     std::cerr
            //         << "LifeCycleSingleObject::LifeCycleSingleObject(CORBA::ORB_ptr orb)"
            //         << std::endl;
        }

        LifeCycleSingleObject::~LifeCycleSingleObject()
        throw()
        {
            // do nothing
        }

        // Returns true if object with oid is removed
        bool
        LifeCycleSingleObject::is_removed(const PortableServer::ObjectId & oid) const
        throw()
        {
            return m_removed;
        }

        // This registers oid as removed.
        void
        LifeCycleSingleObject::register_object_as_removed(const PortableServer::ObjectId & oid)
        throw()
        {
            m_removed = true;
        }


    }

    ; // End namespace Entity
}

; // End namespace Cdmw

