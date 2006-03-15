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

#include "ftmanager/TypeRepository_impl.hpp"



namespace Cdmw {
namespace FT {

    
TypeRepository_impl::TypeRepository_impl(SupportedInterfacesMap supportedInterfacesMap)
    : m_supportedInterfacesMap(supportedInterfacesMap)
{}
        
    
CORBA::Boolean 
TypeRepository_impl::has_base_interface(const char* type_id, const char* base_type_id)
    throw(::CdmwFT::TypeRepository::TypeNotFound)
{
    // if RepositoryId are the same, return true
    if (strcmp(type_id, base_type_id) == 0) {
        return true;
    }
    
    // find list of supported interfaces for type_id
    SupportedInterfacesMap::iterator mapIt = 
        m_supportedInterfacesMap.find(type_id);
    
    if (mapIt == m_supportedInterfacesMap.end()) {
        throw ::CdmwFT::TypeRepository::TypeNotFound();
    }
    
    // search base_type_id in list of supported interfaces
    for (InterfacesList::iterator interfacesIt = mapIt->second.begin();
         interfacesIt != mapIt->second.end();
         ++interfacesIt) 
    {
        if (*interfacesIt == base_type_id)
            return true;
    }

    return false;
}
    

    
}  // End namespace FT
}  // End namespace Cdmw
