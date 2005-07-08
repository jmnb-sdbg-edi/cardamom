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


#ifndef INCL_CDMW_LIFECYCLE_SERVANT_PROVIDER_ALLOCATOR_BASE_HPP
#define INCL_CDMW_LIFECYCLE_SERVANT_PROVIDER_ALLOCATOR_BASE_HPP

#include <string>
#include <list>

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for CDMW LifeCycle Frameworks types and data.
    */

    namespace LifeCycle
    {

        /**
        *Purpose:
        *<p>    Base class for all Servant Provider allocators
        *
        */

        class ServantProviderAllocatorBase
        {

            public:
                /**
                * Purpose:
                * <p> This operation returns a pointer to a Servant Provider Allocator
                * registered with a given name.
                *
                *@param class_name Name of a registered Servant Provider Allocator
                *
                *@return A pointer to a ServantProviderAllocatorBase instance
                */
                static const ServantProviderAllocatorBase *
                GetAllocator(const std::string & class_name)
                throw(); // TODO exception specs


                /**
                * Purpose:
                * <p> This operation returns a list conaining names of all Servant Provider 
                * Allocators within the current process.
                *
                *@param lst A reference to container (list) to hold all the names
                *
                */
                static void GetAllNames(std::list<std::string> & lst)
                throw(); // TODO exception specs

            protected:
                ServantProviderAllocatorBase(const std::string & class_name) throw();
                ServantProviderAllocatorBase(const ServantProviderAllocatorBase &) throw();
                virtual ~ServantProviderAllocatorBase() throw();
        }

        ; // End ServantProviderAllocatorBase

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw
#endif // INCL_CDMW_LIFECYCLE_SERVANT_PROVIDER_ALLOCATOR_BASE_HPP

