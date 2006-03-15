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


#ifndef CDMW_LIFECYCLE_OBJECT_REGISTRATION_BASE_H
#define CDMW_LIFECYCLE_OBJECT_REGISTRATION_BASE_H

#include "Foundation/orbsupport/CORBA.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.skel.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"

namespace Cdmw
{

    namespace LifeCycle
    {
        /**
         * The ObjectRegistration acts as a singleton factory for the creation
         * of Lifecycle Objects
         */

        class ObjectRegistrationBase : public CdmwLifeCycle::ObjectRegistration,
                    public Cdmw::OrbSupport::RefCountLocalObject
        {

            public:

                ~ObjectRegistrationBase()
                throw ();

                //
                // IDL:thalesgroup.com/CdmwLifeCycle/ObjectRegistration/register_object:1.0
                //
                virtual void register_object(CORBA::Object_ptr object_ref,
                                             const char* object_name)
                throw (CosNaming::NamingContext::InvalidName,
                       CosNaming::NamingContext::AlreadyBound,
                       CORBA::SystemException);

                //
                // IDL:thalesgroup.com/CdmwLifeCycle/ObjectRegistration/unregister_object:1.0
                //
                virtual void unregister_object(const char* object_name)
                throw (CosNaming::NamingContext::NotFound,
                       CosNaming::NamingContext::InvalidName,
                       CORBA::SystemException);

            protected:
                ObjectRegistrationBase()
                throw();
                virtual bool bind(CORBA::Object_ptr home_ref,
                                  const char* object_name) = 0;
                virtual bool unbind(const char* object_name) = 0;

            private:

                // Hide copy constructor/assignment operator
                ObjectRegistrationBase(const ObjectRegistrationBase& rhs)
                throw();

                ObjectRegistrationBase&
                operator=(const ObjectRegistrationBase& rhs)
                throw();


        };

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw

#endif // CDMW_LIFECYCLE_OBJECT_REGISTRATION_BASE_H

