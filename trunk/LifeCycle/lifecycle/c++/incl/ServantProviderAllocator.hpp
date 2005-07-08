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


#ifndef INCL_LIFECYCLE_SERVANT_PROVIDER_ALLOCATOR_HPP
#define INCL_LIFECYCLE_SERVANT_PROVIDER_ALLOCATOR_HPP

#include "Foundation/orbsupport/CORBA.hpp"

#include "LifeCycle/lifecycle/ServantProviderBase.hpp"
#include "LifeCycle/lifecycle/ServantProviderAllocatorBase.hpp"
#include "LifeCycle/lifecycle/ObjectDeactivator.hpp"

#include <typeinfo>

//----------------------------------------------------------------------------//
//                                 Export Macros                              //
//----------------------------------------------------------------------------//

/**
 *  This macro is to be used whever there a need to declare a specific singleton.
 *  ServantProvviderAllocator which shall follow this pattern to be called
 *  by the lifecycle framework.
 */
#define DECLARE_SERVANT_PROVIDER_ALLOCATOR(classname) \
    static ServantProviderAllocator<classname> TheAllocator

/**
 *  This macro is to be used to define a previously declared ServantProviderAllocator
 *  singleton.
 */
#define DEFINE_SERVANT_PROVIDER_ALLOCATOR(classname) \
    template <typename classname> \
    ServantProviderAllocator<classname> ServantProviderAllocator<classname>::TheAllocator


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


        // TODO: add comment that T(orb,adapter,deactivator,argc,argv) may only raise std::bad_alloc
        // exception.
        /**
        *Purpose:
        *<p>    Template class for servant provider allocators. Template parameter <B>T</B> has
        * to be a ServantProviderBase subtype.
        * Users may use explicit instantiation to change behaviour of default allocation
        * and disposal of servant providers.
        * <p><STRONG>NOTA:</STRONG>
        * Default implementation expects servant providers to have the following constructor:
        * <p> <I>Parameters:</I>
        * <DL>
        * <DT>CORBA::ORB_ptr <DD>A reference to the ORB pseudo-object
        * <DT>PortableServer::POA_ptr <DD>A POA that will be used for object activaion
        * <DT>const ObjectDeactivator & <DD>An ObjectDeactivator for use whenever there is a
        * need to deactiate an object.
        * <DT>int & argc <DD>Number of additional arguments 
        * <DT>char** argv <DD>Additional program arguments
        * </DL>
        * <p> <I>Exceptions:</I>
        * <UL>
        * <LI>std::bad_alloc
        * <LI>CORBA::SystemException
        * </UL>
        */

        template <typename T>

        class ServantProviderAllocator :
                    public ServantProviderAllocatorBase
        {

            public:
                /**
                * Purpose:
                * <p>Allocates a ServantProvider for use by generated CDMW Factories
                * implementantions.
                * This is ideal place to retreive the ServantProvider state from persistence
                * support, if any.
                *
                *@param orb A reference to the ORB pseudo-object
                *@param adapter A POA where CORBA objects incarnated by provided servants
                * will be activated.
                *@param deactivator An ObjectDeactivator for use whenever there is a need
                * to deactiate an object.
                *@param argc Number of additional arguments
                *@param argv Additional program arguments
                *
                *@return A pointer to a ServantProviderBase object for use by the LifeCycle Framework.
                *
                *@exception std::bad_alloc Failure to allocate memory for the servant provider
                *@exception CORBA::SystemException Any CORBA system exception
                */
                ServantProviderBase *
                allocate(CORBA::ORB_ptr orb,
                         PortableServer::POA_ptr adapter,
                         const ObjectDeactivator & deactivator,
                         int & argc, char** argv)
                throw(std::bad_alloc,
                      CORBA::SystemException)
                {
                    // Ideal place to retreive state from persistence
                    // support
                    return new T(orb, adapter, deactivator, argc, argv);
                }

                /**
                * Purpose:
                * <p> This operation is called whenever the Life Cycle Framework is done
                * with the ServantProvider object.
                * This is the ideal place to persist state of the ServantProvider to a
                * persistent support.
                *
                *@param provider A pointer to the ServantProvider object
                */
                void
                dispose(ServantProviderBase * provider)
                throw ()
                {
                    // Ideal place to persist state
                    delete provider;
                }

                /**
                * A static instance of the ServantProvider Allocator 
                */
                DECLARE_SERVANT_PROVIDER_ALLOCATOR(T);

#ifndef _MSC_VER

            protected:
                // FIXME: Compilation problem with VisualC++ 6.0; This will be sorted out once
                // Windows platforms are officially supported.
#endif

                ServantProviderAllocator()
                throw()
                        : ServantProviderAllocatorBase(typeid(T).name())
                {}

                ServantProviderAllocator(const ServantProviderAllocator<T> & other)
                throw()
                        : ServantProviderAllocatorBase(other)
                {}

                ~ServantProviderAllocator()
                throw()
                {}

        }

        ; // End template ServantProviderAllocator

        DEFINE_SERVANT_PROVIDER_ALLOCATOR(T);


        // Example of a user-defined Servant Provider Allocator
        //template <>
        //class ServantProviderAllocator<MyServantProvider> :
        //    public ServantProviderAllocatorBase
        //{
        //public:
        //    ServantProviderBase *
        //    allocate(CORBA::ORB_ptr          orb,
        //             PortableServer::POA_ptr adapter,
        //             int & argc, char** argv)
        //         throw(std::bad_alloc)
        //    {
        //        // Restore state from persistence support
        //        return lookup_MyServantProvider(orb,adapter,argc,argv);
        //    }
        //
        //    void
        //    dispose(ServantProviderBase * provider)
        //        throw ()
        //    {
        //        // Persiste state
        //        dispose_MyServantProvider(provider);
        //    }
        //
        //
        //    DeclareServantProviderExportedType(MyServantProvider);
        //
        //
        //protected:
        //    ServantProviderAllocator()
        //        throw()
        //        : ServantProviderAllocatorBase(typeid(MyServantProvider).name())
        //    {
        //    }
        //
        //    ServantProviderAllocator(const ServantProviderAllocator<T> & other)
        //        throw()
        //        : ServantProviderAllocatorBase(other)
        //    {
        //    }
        //
        //    ~ServantProviderAllocator()
        //        throw()
        //    {
        //    }
        //private:
        //    ServantProviderBase *
        //    lookup_MyServantProvider(CORBA::ORB_ptr          orb,
        //                             PortableServer::POA_ptr adapter,
        //                             int & argc, char** argv)
        //         throw(std::bad_alloc)
        //    {
        //        // do what ever...
        //    }
        //
        //    void
        //    dispose_MyServantProvider(MyServantProvider * provider)
        //        throw ()
        //    {
        //        // do what ever...
        //    }
        //}; // End template ServantProviderAllocator
        //
        //RegisterServantProviderExportedType(MyServantProvider);
        //

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw
#endif // INCL_LIFECYCLE_SERVANT_PROVIDER_ALLOCATOR_HPP

