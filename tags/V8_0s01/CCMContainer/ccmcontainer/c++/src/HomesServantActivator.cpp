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


#include <CCMContainer/ccmcontainer/HomesServantActivator.hpp>
#include <CCMContainer/ccmcontainer/HomeAllocatorRegistry.hpp>
#include <Foundation/common/Assert.hpp>

#include "ccmcontainer/MacroDefs.hpp"

namespace Cdmw
{
namespace CCM
{
namespace Container
{


HomesServantActivator::HomesServantActivator () 
{
}

HomesServantActivator::~HomesServantActivator () 
{
}

PortableServer::Servant 
HomesServantActivator::incarnate(const PortableServer::ObjectId& oid, 
                                 PortableServer::POA_ptr         poa)
    throw(PortableServer::ForwardRequest, CORBA::SystemException)
{
    try {
        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
        DEBUG_ECHO << "Incarnating " << s.in() << "..." << std::endl;        
    } catch (...) {
        // ignore ...
    }
    // Should not happen (at this stage!)
    CDMW_ASSERT(0); 
    return 0; // TODO: Maybe raise CORBA::TRANSIENT()
}

void 
HomesServantActivator::etherealize(const PortableServer::ObjectId& oid,
                                   PortableServer::POA_ptr         poa,
                                   PortableServer::Servant         servant,
                                   CORBA::Boolean                  cleanup_in_progress,
                                   CORBA::Boolean                  remaining_activations)
    throw(CORBA::SystemException)
{
    try {
        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
        DEBUG_ECHO << "Etherealizing " << s.in() << "..." << std::endl;   
        using namespace Cdmw::CCM::Container;
        HomeAllocatorBase * home_allocator =
            HomeAllocatorRegistry::GetAllocator(s.in());
        CDMW_ASSERT(home_allocator != 0);
        home_allocator->dispose(servant);
        HomeAllocatorRegistry::Unregister(s.in());
        
    } catch (...) {
        CDMW_ASSERT(0); // Shouldn't happen
    }
}

}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw


