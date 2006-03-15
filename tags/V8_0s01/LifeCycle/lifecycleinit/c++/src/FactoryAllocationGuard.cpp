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


#include "LifeCycle/lifecycleinit/FactoryAllocationGuard.hpp"


namespace Cdmw
{
namespace LifeCycle
{

/**
 * Purpose:
 * <p> [Constructor description if necessary]
 * 
 *@param allocator Reference to the Factory Allocator
 *@param factory   A factory servant previously allocated by 'allocator'
 */ 
FactoryAllocationGuard::FactoryAllocationGuard(FactoryAllocatorBase &  allocator,
                                               PortableServer::Servant factory)
    throw()
    : m_allocator(allocator),
      m_factory(factory),
      m_release(true)
{
}
    
/**
 * Purpose:
 * <p> Destructor calls dispose on the Factory Allocator for the internal factory
 *  if not canceled.
 * 
 */ 
FactoryAllocationGuard::~FactoryAllocationGuard()
    throw()
{
    if (m_release)
        m_allocator.dispose(m_factory);
}    

/**
 * Purpose:
 * <p>This operation cancels factory disposal at object destruction.
 */
void FactoryAllocationGuard::cancel() throw()
{
    m_release = false;
}
    

}; // End namespace LifeCycle
}; // End namespace Cdmw

