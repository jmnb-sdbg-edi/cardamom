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


#include <CCMContainer/ccmcontainer/HomeAllocatorRegistry.hpp>
#include <algorithm>   // for std::for_each
#include <iterator>    // for std::back_inserter

#include "ccmcontainer/MacroDefs.hpp"

namespace {
    
    class BackInserter
    {
    public:
        BackInserter(std::list<std::string> & l) 
            : m_list(l) {}
        void operator()(const  std::pair<const std::string ,
                        const Cdmw::CCM::Container::HomeAllocatorBase* > & p
                        ) const
            {
                m_list.push_back(p.first);
            }
    private:
        std::list<std::string> & m_list;
    };
}; // End of Anonymous namespace

namespace Cdmw
{

namespace CCM
{
namespace Container
{

HomeAllocatorRegistry::Registry HomeAllocatorRegistry::M_Registry;

bool
HomeAllocatorRegistry::Register(const std::string & id,
                                HomeAllocatorBase * allocator) 
    throw()
{
    DEBUG_ECHO << "Registering Home Allocator '" << id << "'" << std::endl;
    return M_Registry.insert(Registry::value_type(id, allocator)).second;
}

bool
HomeAllocatorRegistry::Unregister(const std::string & id) 
    throw()
{
    DEBUG_ECHO << "Unregistering Home Allocator '" << id << "'" << std::endl;
    return M_Registry.erase(id) == 1;
}



HomeAllocatorBase *
HomeAllocatorRegistry::GetAllocator(const std::string & id)
    throw()
{
    HomeAllocatorBase *  home_allocator = 0;
    Registry::iterator i = M_Registry.find(id);
    if (i != M_Registry.end())
    {
        home_allocator = i->second;
    }
    return home_allocator;
}

void
HomeAllocatorRegistry::GetAllIds(std::list<std::string> & lst) 
    throw()
{
    try {
        std::for_each(M_Registry.begin(),M_Registry.end(),BackInserter(lst));
    } catch (...) {
    }
}

}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw


