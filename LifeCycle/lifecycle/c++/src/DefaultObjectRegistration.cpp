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


#include "LifeCycle/lifecycle/DefaultObjectRegistration.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/common/Assert.hpp"

namespace Cdmw
{

    namespace LifeCycle
    {

        DefaultObjectRegistration::DefaultObjectRegistration(CosNaming::NamingContext_ptr root)
        throw (CORBA::SystemException)
                : m_root(root)
        {}


        DefaultObjectRegistration::~DefaultObjectRegistration()
        throw ()
        {}


        bool
        DefaultObjectRegistration::bind(CORBA::Object_ptr object_ref,
                                        const char* object_name)
        {
            bool result = false;

            using namespace Cdmw::CommonSvcs::Naming;

            try
            {
                CosNaming::Name_var name = NamingInterface::to_name(object_name);

                if (name->length() != 1)
                    throw CosNaming::NamingContext::InvalidName();
            }
            catch (const InvalidNameException & )
            {
                throw CosNaming::NamingContext::InvalidName();
            }

            if (!CORBA::is_nil(object_ref))
            {
                try
                {
                    m_root.bind(object_name, object_ref, false);
                    result = true;
                }
                catch (const CosNaming::NamingContext::NotFound & )
                {
                    // Should not happen as object_name length is 1..
                    CDMW_ASSERT(false);
                }
            }

            return result;
        }

        bool
        DefaultObjectRegistration::unbind(const char* object_name)
        {
            bool result = false;

            using namespace Cdmw::CommonSvcs::Naming;

            try
            {
                CosNaming::Name_var name = NamingInterface::to_name(object_name);

                if (name->length() != 1)
                    throw CosNaming::NamingContext::InvalidName();

                m_root.unbind(object_name);

                result = true;
            }
            catch (const InvalidNameException & )
            {
                throw CosNaming::NamingContext::InvalidName();
            }

            return result;
        }

    }

    ; // End namespace Lifecycle
}

; // End namespace Cdmw

