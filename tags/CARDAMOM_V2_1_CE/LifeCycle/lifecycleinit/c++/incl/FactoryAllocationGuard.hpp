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


#ifndef INCL_LIFECYCLE_FACTORY_ALLOCATION_GUARD_HPP
#define INCL_LIFECYCLE_FACTORY_ALLOCATION_GUARD_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "LifeCycle/lifecycle/FactoryAllocator.hpp"

namespace Cdmw
{
namespace LifeCycle
{

    /**
     *Purpose:
     *<p>    This class provides Guard objects to be used whenever a factory is allocated
     * but should be 'disposed' in case an exception is raised.
     *
     */
    class FactoryAllocationGuard
    {
    public:
        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param allocator Reference to the Factory Allocator
         *@param factory   A factory servant previously allocated by 'allocator'
         */ 
        FactoryAllocationGuard(FactoryAllocatorBase &  allocator,
                               PortableServer::Servant factory)
            throw();
        
        /**
         * Purpose:
         * <p> Destructor calls dispose on the Factory Allocator for the internal factory
         *  if not canceled.
         * 
         */ 
        virtual 
        ~FactoryAllocationGuard()
            throw();
        
        /**
         * Purpose:
         * <p>This operation cancels factory disposal at object destruction.
         */
        void cancel() throw();
    
    private:
        /**
         * Reference to the Factory Allocator.
         */
        FactoryAllocatorBase &      m_allocator;
        
        /**
         * A pointer to the factory allocated by the allocator above
         */
        PortableServer::Servant     m_factory;
        
        /**
         * Flag to indicate whether the desctructor should call dispose on the allocator
         */
        bool                        m_release;
        
        // Hide constructor/copy constructor/assignment operator 
        FactoryAllocationGuard()
            throw();
        FactoryAllocationGuard(const FactoryAllocationGuard& rhs)
            throw();        
        FactoryAllocationGuard &
        operator=(const FactoryAllocationGuard & rhs)
            throw();
    }; // End FactoryAllocationGuard

}; // End namespace LifeCycle
}; // End namespace Cdmw
#endif // INCL_LIFECYCLE_FACTORY_ALLOCATION_GUARD_HPP

