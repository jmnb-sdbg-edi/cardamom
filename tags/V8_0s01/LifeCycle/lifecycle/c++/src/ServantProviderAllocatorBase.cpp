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


#include "LifeCycle/lifecycle/ServantProviderAllocatorBase.hpp"
#include "LifeCycle/lifecycle/Dictionary.hpp"

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
        *<p>    Base class for all allocators
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */


        ServantProviderAllocatorBase::ServantProviderAllocatorBase(const std::string & class_name)
        throw()
        {
            Dictionary<ServantProviderAllocatorBase*>::Get()[class_name] = this;
        }

        ServantProviderAllocatorBase::ServantProviderAllocatorBase(const ServantProviderAllocatorBase & other)
        throw()
        {}

        ServantProviderAllocatorBase::~ServantProviderAllocatorBase()
        throw()
        {}

        const ServantProviderAllocatorBase *
        ServantProviderAllocatorBase::GetAllocator(const std::string & class_name)
        throw() // TODO exception specs
        {
            typedef Dictionary<ServantProviderAllocatorBase* > DicoType;
            ServantProviderAllocatorBase * result = 0;
            DicoType & dico = DicoType::Get();
            DicoType::iterator info_pos = dico.find(class_name);

            if (info_pos != dico.end())
            {
                result = (*info_pos).second; // dico[class_name];
            }

            return result;
        }

        void
        ServantProviderAllocatorBase::GetAllNames(std::list<std::string> & lst)
        throw() // TODO exception specs
        {

            try
            {
                typedef Dictionary<ServantProviderAllocatorBase* > DicoType;
                DicoType & dico = DicoType::Get();
                DicoType::iterator info_pos = dico.begin();

                while (info_pos != dico.end())
                {
                    lst.push_back((*info_pos).first);
                    ++info_pos;
                }
            }
            catch (...)
            {}

        }


    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw

