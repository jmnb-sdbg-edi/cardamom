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


#ifndef INCL_LIFECYCLE_LIFECYCLE_MULTIPLE_OBJECT_HPP
#define INCL_LIFECYCLE_LIFECYCLE_MULTIPLE_OBJECT_HPP
#include "LifeCycle/lifecycle/LifeCycleObjectBase.hpp"

#include <set>
#include <string>


namespace Cdmw
{

    namespace LifeCycle
    {

        /**
        *Purpose:
        *<p>    This class specialises LifeCycleObjectBase for servant incarnating multiple
        * CORBA objects.
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class LifeCycleMultipleObject :
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
                LifeCycleMultipleObject(const ObjectDeactivator & deactivator)
                throw(CORBA::SystemException);

                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~LifeCycleMultipleObject()
                throw();

                /**
                * Purpose:
                * <p>This checks whether one of the CORBA objects incarnated by the current
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
                * <p>This registers the object Id <I>oid</I> of an object incarnated by
                * self as removed.
                *
                *@param oid The Object Id of an object incarnated by self
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
                LifeCycleMultipleObject()
                throw(CORBA::SystemException);

                // Hide copy constructor/assignment operator
                LifeCycleMultipleObject(const LifeCycleMultipleObject& rhs)
                throw();

                LifeCycleMultipleObject&
                operator=(const LifeCycleMultipleObject& rhs)
                throw();
                /**
                * A set of ObjectIds incarnated by self that have been removed.
                * <B>NOTA:</B> Current implementation stores ObjecIDs as strings.
                */

                typedef std::set
                    <std::string> RemovedObjectSet;

                RemovedObjectSet m_ros;

        }

        ; // End class LifeCycleMultipleObject

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw
#endif // INCL_LIFECYCLE_LIFECYCLE_MULTIPLE_OBJECT_HPP

