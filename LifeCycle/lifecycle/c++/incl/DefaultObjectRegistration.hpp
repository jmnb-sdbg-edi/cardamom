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


#ifndef CDMW_LIFECYCLE_DEFAULT_OBJECT_REGISTRATION_H
#define CDMW_LIFECYCLE_DEFAULT_OBJECT_REGISTRATION_H

#include "LifeCycle/lifecycle/ObjectRegistrationBase.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"

namespace Cdmw
{

    namespace LifeCycle
    {
        /**
         * The ObjectRegistration acts as a singleton factory for the creation
         * of Lifecycle Objects
         */

        class DefaultObjectRegistration : public ObjectRegistrationBase
        {

            public:
                DefaultObjectRegistration(CosNaming::NamingContext_ptr root)
                throw (CORBA::SystemException);

                ~DefaultObjectRegistration()
                throw ();

            protected:
                virtual bool bind(CORBA::Object_ptr object_ref,
                                  const char* object_name);
                virtual bool unbind(const char* object_name);

            private:
                DefaultObjectRegistration();

                // Hide copy constructor/assignment operator
                DefaultObjectRegistration(const DefaultObjectRegistration& rhs)
                throw();

                DefaultObjectRegistration&
                operator=(const DefaultObjectRegistration& rhs)
                throw();

                Cdmw::NamingAndRepository::NamingInterface m_root;
        };

    }

    ; // End namespace Lifecycle
}

; // End namespace Cdmw

#endif // CDMW_LIFECYCLE_DEFAULT_OBJECT_REGISTRATION_H

