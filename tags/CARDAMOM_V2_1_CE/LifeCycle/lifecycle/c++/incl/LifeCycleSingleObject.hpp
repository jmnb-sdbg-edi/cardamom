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


#ifndef INCL_LIFECYCLE_LIFECYCLE_SINGLE_OBJECT_HPP
#define INCL_LIFECYCLE_LIFECYCLE_SINGLE_OBJECT_HPP
#include "LifeCycle/lifecycle/LifeCycleObjectBase.hpp"

namespace Cdmw
{

    namespace LifeCycle
    {

        /**
        *Purpose:
        *<p>    This class specialises LifeCycleObjectBase for servant incarnating a single
        * CORBA object.
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class LifeCycleSingleObject :
                    public LifeCycleObjectBase
        {

            public:

                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param deactivator A reference to an Object Deactivator to be used
                * whenever there is a need to deactivate a CORBA object incarnated by
                * the current servant (self).
                *
                *@exception CORBA::SystemException Any CORBA system exception.
                */
                LifeCycleSingleObject(const ObjectDeactivator & deactivator)
                throw(CORBA::SystemException);

                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~LifeCycleSingleObject()
                throw();

                /**
                * Purpose:
                * <p>This checks whether the CORBA object incarnated by the current
                * object (this) has been removed (<I>remove</I> operation has been 
                * called).
                *
                *@param oid The Object Id of the object incarnated by self
                *
                *@return <B>true</B> if the object has been removed, <B>false</B> otherwise.
                */
                virtual bool is_removed(const PortableServer::ObjectId & oid) const
                throw();

                /**
                *[Attribute description]
                */


            protected:
                /**
                * Purpose:
                * <p>This registers the object Id <I>oid</I> of the object incarnated by
                * self as removed.
                *
                *@param oid The Object Id of the object incarnated by self
                *
                */
                virtual void register_object_as_removed(const PortableServer::ObjectId & oid)
                throw();

            private:
                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                LifeCycleSingleObject()
                throw(CORBA::SystemException);


                // Hide copy constructor/assignment operator
                LifeCycleSingleObject(const LifeCycleSingleObject& rhs)
                throw();

                LifeCycleSingleObject&
                operator=(const LifeCycleSingleObject& rhs)
                throw();
                /**
                * Indicates whether the remove operation has been called or not.
                */
                bool m_removed;

        }

        ; // End class LifeCycleSingleObject

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw
#endif // INCL_LIFECYCLE_LIFECYCLE_SINGLE_OBJECT_HPP

