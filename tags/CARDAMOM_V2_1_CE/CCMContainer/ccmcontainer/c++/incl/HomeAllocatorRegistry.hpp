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


#ifndef INCL_CDMW_CCM_CONTAINER_HOME_ALLOCATOR_REGISTRY_HPP
#define INCL_CDMW_CCM_CONTAINER_HOME_ALLOCATOR_REGISTRY_HPP

#include <CCMContainer/ccmcontainer/HomeAllocator.hpp>
#include <list>
#include <map>

namespace Cdmw
{

namespace CCM
{
namespace Container
{
    /**
     * A Registry/Repository for home allocators
     */
    class HomeAllocatorRegistry
    {
    public:
        /**
        * Purpose:
        * <p> Registers a home allocator using <I>id</I> identifier.
        *
        *@param id Identifier for the registration.
        *@param allocator A pointer to a home allocator object.
        *
        *@return bool <B>true</B> if the registration was successful; <B>false</B> otherwise
        *(if there already was a home allocator registered with the same <I>id</id>).
        */    
        static bool Register(const std::string & id,
                             HomeAllocatorBase * allocator) 
            throw();

       
        /**
        * Purpose:
        * <p> Unregisters the home allocator for the given identifier.
        *
        *@param id Identifier to a previously registered home allocator.
        *
        *@return <B>true</B> if <I>id</I> was previously registered; <B>false</B> otherwise.
        */    
        static bool Unregister(const std::string & id) 
            throw();
        
        /**
        * Purpose:
        * <p> Return a pointer to a home allocator with the given identifier.
        *
        *@param id Identifier of the home allocator
        *
        *@return A pointer to a HomeAllocatorBase instance or NULL if not found.
        */
        static HomeAllocatorBase *
        GetAllocator(const std::string & id)
            throw();
        

       
        /**
        * Purpose:
        * <p> Appends all registered identifiers to <I>lst</I>.
        *
        *@param lst A reference to std::list<std::string>
        *
        */    
        static void GetAllIds(std::list<std::string> & lst) 
            throw();
        
    private:
        // Need not be instantiated
        HomeAllocatorRegistry();
        HomeAllocatorRegistry(const HomeAllocatorRegistry&);
        ~HomeAllocatorRegistry();

        typedef std::map<std::string,HomeAllocatorBase *>  Registry;
        static Registry M_Registry;
    };
}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

#endif // INCL_CDMW_CCM_CONTAINER_HOME_ALLOCATOR_REGISTRY_HPP

