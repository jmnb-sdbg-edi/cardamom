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


#ifndef INCL_CDMW_CCM_CONTAINER_HOME_ALLOCATOR_HPP
#define INCL_CDMW_CCM_CONTAINER_HOME_ALLOCATOR_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/common/Assert.hpp>
#include "CCMContainer/idllib/Components.skel.hpp"
#include <LifeCycle/lifecycle/FactoryAllocator.hpp>
#include <LifeCycle/lifecycle/ObjectDeactivator.hpp>
#include <string>
#include <list>

#include <typeinfo>

#define DECLARE_HOME_ALLOCATOR(classname) \
static HomeAllocator<classname> TheAllocator

#define DEFINE_HOME_ALLOCATOR(classname) \
template <typename classname> \
HomeAllocator<classname> HomeAllocator<classname>::TheAllocator


namespace Cdmw
{

namespace CCM
{
namespace Container
{
    /**
    * Purpose:
    * <p> Base class for all Component homes allocators.
    *
    */
    class HomeAllocatorBase : public Cdmw::LifeCycle::FactoryAllocatorBase
    {
    public:
        /**
        * Purpose:
        * <p> This operation is called by the container during homes installation
        * to allocate a servant for the home.
        *
        *@param home_executor Home executor acquired from the loaded library.
        *@param deactivator Object Deactivator for use for home object deactivation.
        *
        *@exception CORBA::SystemException Any CORBA System Exception
        *@std::bad_alloc Failure to allocate memory/resources for the home.
        */
        virtual
        PortableServer::Servant 
        allocate(Components::HomeExecutorBase_ptr           home_executor,
                 const Cdmw::LifeCycle::ObjectDeactivator & deactivator,
                 const Components::ConfigValues&            config)
            throw (CORBA::SystemException,
                   std::bad_alloc) = 0;                
    protected:    
        HomeAllocatorBase() throw() {}
        HomeAllocatorBase(const HomeAllocatorBase &) throw() {}
        virtual ~HomeAllocatorBase() throw() {}
    };

    /**
     * A template class for Home Allocators.
     */
    template <typename HOME>
        class  HomeAllocator : public HomeAllocatorBase
    {
    public:
        /**
        *@see HomeAllocatorBase::allocate
        */
        PortableServer::Servant 
            allocate(Components::HomeExecutorBase_ptr            home_executor,
                     const Cdmw::LifeCycle::ObjectDeactivator & deactivator,
                     const Components::ConfigValues&            config)
            throw (CORBA::SystemException,
                   std::bad_alloc)
        {
            return new HOME(home_executor, deactivator, config);
        }
        /**
        * Purpose:
        * <p> This operation is called whenever the Container is done
        * with the Home servant previuously allocated by the current allocator.
        *
        *@param factory a Home servant.
        */
        virtual void dispose(PortableServer::Servant factory) throw()
        {            
            HOME* home = dynamic_cast<HOME*>(factory);
            CDMW_ASSERT(home != 0);
            cleanup(home);
        }
    
        /**
        * A static instance of the Home Allocator 
        */
        DECLARE_HOME_ALLOCATOR(HOME);
    protected:    
        /**
         * Purpose:
         * <p> This operation perfoms actual disposal of the CDMW factory.
         * This is an ideal place to persist state of the factory.
         *
         *@param fac A pointer to the CDMW factory.
         */
        void cleanup(HOME* home) 
            throw () 
        { 
            home->_remove_ref(); //deletes fac IF ref-counted!;
        }
    protected:
        HomeAllocator() throw() {}
    };

    DEFINE_HOME_ALLOCATOR(HOME);

}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

#endif // INCL_CDMW_CCM_CONTAINER_HOME_ALLOCATOR_HPP

