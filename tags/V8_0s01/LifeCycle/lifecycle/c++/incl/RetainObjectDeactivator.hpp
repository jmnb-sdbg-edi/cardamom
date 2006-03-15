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


#ifndef INCL_LIFECYCLE_RETAIN_OBJECT_DEACTIVATOR_HPP
#define INCL_LIFECYCLE_RETAIN_OBJECT_DEACTIVATOR_HPP

#include "LifeCycle/lifecycle/ObjectDeactivator.hpp"

namespace Cdmw
{

    namespace LifeCycle
    {

        /**
        *Purpose:
        *<p>    Object Deactivator class for POAs with RETAIN policy
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class RetainObjectDeactivator : public ObjectDeactivator
        {

            public:
                /**
                 * Purpose:
                 * <p> [Constructor description if necessary]
                 * 
                 *@param orb A reference to the ORB pseudo-object
                 *
                 *@exception CORBA::SystemException Any CORBA system exception
                 */
                explicit RetainObjectDeactivator(CORBA::ORB_ptr orb)
                throw(CORBA::SystemException);
                /**
                 * Purpose:
                 * <p> [ Destructor description if necessary]
                 * 
                 */
                virtual
                ~RetainObjectDeactivator()
                throw();

            protected:

                /**
                 * Purpose:
                 * <p>This method does the actual deactivation of the calling object. It deactivates the object
                 * with <I>oid</oid> ObjectId in <I>poa</I> Portable Object Adapter.
                 * 
                 *@param poa A reference to the POA pseudo-object where the object is activated
                 *@param oid ObjectId of the object to deactivate.
                 *
                 *@exception CORBA::SystemException Any CORBA system exception
                 */
                virtual void deactivate_object(PortableServer::POA_ptr poa,
                                               const PortableServer::ObjectId& oid) const
                throw(CORBA::SystemException);

            private:
                /**
                 * Purpose:
                 * <p> [Constructor description if necessary]
                 * 
                 *@param
                 *@exception
                 */
                RetainObjectDeactivator()
                throw(CORBA::SystemException);
                // Hide copy constructor/assignment operator
                RetainObjectDeactivator(const RetainObjectDeactivator& rhs)
                throw();
                RetainObjectDeactivator&
                operator=(const RetainObjectDeactivator& rhs)
                throw();
                /**
                 *[Attribute description]
                 */
        }

        ; // End class RetainObjectDeactivator

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw
#endif // INCL_LIFECYCLE_RETAIN_OBJECT_DEACTIVATOR_HPP

