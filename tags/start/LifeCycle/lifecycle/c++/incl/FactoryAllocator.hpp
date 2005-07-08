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


#ifndef INCL_LIFECYCLE_FACTORY_ALLOCATOR_HPP
#define INCL_LIFECYCLE_FACTORY_ALLOCATOR_HPP

#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/POAThreadingPolicy.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"
#include <string>

//----------------------------------------------------------------------------//
//                                 Export Macros                              //
//----------------------------------------------------------------------------//
#define DECLARE_NAMED_OBJECTS_FACTORY_ALLOCATOR(classname) \
    static NamedObjectsFactoryAllocator<classname> TheAllocator

#define DECLARE_ANONYMOUS_OBJECTS_FACTORY_ALLOCATOR(classname) \
    static AnonymousObjectsFactoryAllocator<classname> TheAllocator


#define DEFINE_NAMED_OBJECTS_FACTORY_ALLOCATOR(classname) \
    template <typename classname> \
    NamedObjectsFactoryAllocator<classname> NamedObjectsFactoryAllocator<classname>::TheAllocator

#define DEFINE_ANONYMOUS_OBJECTS_FACTORY_ALLOCATOR(classname) \
    template <typename classname> \
    AnonymousObjectsFactoryAllocator<classname> AnonymousObjectsFactoryAllocator<classname>::TheAllocator



namespace Cdmw
{

    namespace LifeCycle
    {

        /**
        * Abstract base class for all CDMW factories allocators.
        */

        class FactoryAllocatorBase
        {

            public:
                virtual ~FactoryAllocatorBase() throw()
                {}

                /**
                * Purpose:
                * <p> This operation is called whenever the Life Cycle Framework is done
                * with the CDMW factory allocated by a derived factory allocator.
                *
                *@param factory a CDMW factory servant.
                */
                virtual void dispose(PortableServer::Servant factory) throw() = 0;

        };

        /**
        *Purpose:
        *<p> This abstract class provides a base class for all CDMW
        * Named Objects Factories.
        */

        class NamedObjectsFactoryAllocatorBase : public FactoryAllocatorBase
        {

            public:
                /**
                * Purpose:
                * <p>Allocates a named objects factory for use by the LifeCycle framework.
                * This is ideal place to retreive the factory state from persistence support.
                *
                *@param orb A reference to the ORB pseudo-object
                *@param parent Parent POA for the factory
                *@param name_domain Object reference of the name domain where the
                * factory will register objects it creates
                *@param name_domain_name The fully qualified name of the NameDomain
                *@param factory_name Fully qualified name of the CDMW factory
                *@param argc Number of additional arguments
                *@param argv Additional program arguments
                *
                *@return A pointer to the allocated CDMW factory
                *
                *@exception Cdmw::OutOfMemoryException Failure to allocate memory
                *@exception Cdmw::BadParameterException Wrong parameter value, such as 
                * threading ... etc.
                *@exception Cdmw::InternalErrorException Internal Error
                *@exception CORBA::SystemException Any CORBA system exception
                */
                virtual
                PortableServer::Servant
                allocate(CORBA::ORB_ptr orb,
                         PortableServer::POA_ptr parent,
                         CdmwLifeCycle::ObjectRegistration_ptr name_domain,
                         const std::string & name_domain_name,
                         const std::string & factory_name,
                         Cdmw::OrbSupport::POAThreadingPolicy & threading_policy,
                         int & argc, char** argv)
                throw(Cdmw::OutOfMemoryException,
                      Cdmw::BadParameterException,
                      Cdmw::InternalErrorException,
                      CORBA::SystemException) = 0;
        }

        ; // End NamedObjectsFactoryAllocatorBase

        /**
        *Purpose:
        *<p> This abstract class provides a base class for all CDMW
        * Anonymous Objects Factories.
        */

        class AnonymousObjectsFactoryAllocatorBase : public FactoryAllocatorBase
        {

            public:
                /**
                * Purpose:
                * <p>Allocates an anonymous objects factory for use by the LifeCycle framework.
                * This is ideal place to retreive the factory state from persistence support.
                *
                *@param orb A reference to the ORB pseudo-object
                *@param parent Parent POA for the factory
                *@param factory_name Fully qualified name of the CDMW factory
                *@param argc Number of additional arguments
                *@param argv Additional program arguments
                *
                *@return A pointer to the allocated CDMW factory
                *
                *@exception Cdmw::OutOfMemoryException Failure to allocate memory
                *@exception Cdmw::BadParameterException Wrong parameter value, such as 
                * threading ... etc.
                *@exception Cdmw::InternalErrorException Internal Error
                *@exception CORBA::SystemException Any CORBA system exception
                */
                virtual
                PortableServer::Servant
                allocate(CORBA::ORB_ptr orb,
                         PortableServer::POA_ptr parent,
                         const std::string & factory_name,
                         Cdmw::OrbSupport::POAThreadingPolicy & threading_policy,
                         int & argc, char** argv)
                throw(Cdmw::OutOfMemoryException,
                      Cdmw::BadParameterException,
                      Cdmw::InternalErrorException,
                      CORBA::SystemException) = 0;
        }

        ; // End AnonymousObjectsFactoryAllocatorBase

        /**
        *Purpose:
        *<p>    Template class for CDMW named objects factories allocators.
        * Users may use explicit instantiation to change behaviour of default allocation
        * and disposal of factories.
        *
        */

        template <class T>

        class NamedObjectsFactoryAllocator : public NamedObjectsFactoryAllocatorBase
        {

            public:
                /**
                * Purpose:
                * <p>Allocates a named objects factory for use by the LifeCycle framework.
                * This is ideal place to retreive the factory state from persistence support.
                *
                *@param orb A reference to the ORB pseudo-object
                *@param parent Parent POA for the factory
                *@param name_domain Object reference of the name domain where the
                * factory will register objects it creates
                *@param name_domain_name The fully qualified name of the NameDomain
                *@param factory_name Fully qualified name of the CDMW factory
                *@param argc Number of additional arguments
                *@param argv Additional program arguments
                *
                *@return A pointer to the allocated CDMW factory
                *
                *@exception Cdmw::OutOfMemoryException Failure to allocate memory
                *@exception Cdmw::BadParameterException Wrong parameter value, such as 
                * threading ... etc.
                *@exception Cdmw::InternalErrorException Internal Error
                *@exception CORBA::SystemException Any CORBA system exception
                */
                virtual
                PortableServer::Servant
                allocate(CORBA::ORB_ptr orb,
                         PortableServer::POA_ptr parent,
                         CdmwLifeCycle::ObjectRegistration_ptr name_domain,
                         const std::string & name_domain_name,
                         const std::string & factory_name,
                         Cdmw::OrbSupport::POAThreadingPolicy & threading_policy,
                         int & argc, char** argv)
                throw(Cdmw::OutOfMemoryException,
                      Cdmw::BadParameterException,
                      Cdmw::InternalErrorException,
                      CORBA::SystemException)
                {
                    // Ideal place to retreive state from persistence
                    // support

                    try
                    {
                        return new T(orb,
                                     parent,
                                     name_domain,
                                     name_domain_name,
                                     factory_name,
                                     threading_policy,
                                     argc,
                                     argv);
                    }
                    catch (const std::bad_alloc &)
                    {
                        CDMW_THROW(Cdmw::OutOfMemoryException);
                    }
                    catch (const Cdmw::OutOfMemoryException &)
                    {
                        throw;
                    }
                    catch (const Cdmw::BadParameterException &)
                    {
                        throw;
                    }
                    catch (const Cdmw::InternalErrorException &)
                    {
                        throw;
                    }
                    catch (const CORBA::SystemException & e)
                    {
                        throw;
                    }
                }

                /**
                * Purpose:
                * <p> This operation is called whenever the Life Cycle Framework is done
                * with the CDMW factory allocated by a derived factory allocator.
                *
                *@param factory a CDMW factory servant.
                */
                virtual
                void
                dispose(PortableServer::Servant factory)
                throw ()
                {
                    T* fac = dynamic_cast<T*>(factory);
                    CDMW_ASSERT(fac != 0);
                    cleanup(fac);
                }

                /**
                * A static instance of the factory Allocator 
                */
                DECLARE_NAMED_OBJECTS_FACTORY_ALLOCATOR(T);

            protected:
                /**
                * Purpose:
                * <p> This operation perfoms actual disposal of the CDMW factory.
                * This is an ideal place to persist state of the factory.
                *
                *@param fac A pointer to the CDMW factory.
                */
                void cleanup(T* fac)
                throw ()
                {
                    fac->_remove_ref(); //deletes fac IF ref-counted!;
                }

#ifndef _MSC_VER

            protected:
#else 
                // FIXME: Compilation problem with VisualC++ 6.0; This will be sorted out once
                // Windows platforms are officially supported.

            public:
#endif

                NamedObjectsFactoryAllocator()
                throw()
                {}

                NamedObjectsFactoryAllocator(const NamedObjectsFactoryAllocator<T> & other)
                throw()
                {}

                ~NamedObjectsFactoryAllocator()
                throw()
                {}

        }

        ; // End template NamedObjectsFactoryAllocator





        /**
        *Purpose:
        *<p>    Template class for CDMW anonymous factories allocators.
        * Users may use explicit instantiation to change behaviour of default allocation
        * and disposal of factories.
        *
        */
        template <class T>

        class AnonymousObjectsFactoryAllocator : public AnonymousObjectsFactoryAllocatorBase
        {

            public:
                /**
                * Purpose:
                * <p>Allocates an anonymous objects factory for use by the LifeCycle framework.
                * This is ideal place to retreive the factory state from persistence support.
                *
                *@param orb A reference to the ORB pseudo-object
                *@param parent Parent POA for the factory
                *@param factory_name Fully qualified name of the CDMW factory
                *@param argc Number of additional arguments
                *@param argv Additional program arguments
                *
                *@return A pointer to the allocated CDMW factory
                *
                *@exception Cdmw::OutOfMemoryException Failure to allocate memory
                *@exception Cdmw::BadParameterException Wrong parameter value, such as 
                * threading ... etc.
                *@exception Cdmw::InternalErrorException Internal Error
                *@exception CORBA::SystemException Any CORBA system exception
                */
                virtual
                PortableServer::Servant
                allocate(CORBA::ORB_ptr orb,
                         PortableServer::POA_ptr parent,
                         const std::string & factory_name,
                         Cdmw::OrbSupport::POAThreadingPolicy & threading_policy,
                         int & argc, char** argv)
                throw(Cdmw::OutOfMemoryException,
                      Cdmw::BadParameterException,
                      Cdmw::InternalErrorException,
                      CORBA::SystemException)
                {
                    // Ideal place to retreive state from persistence
                    // support

                    try
                    {
                        return new T(orb,
                                     parent,
                                     factory_name,
                                     threading_policy,
                                     argc,
                                     argv);
                    }
                    catch (const std::bad_alloc &)
                    {
                        CDMW_THROW(Cdmw::OutOfMemoryException);
                    }
                    catch (const Cdmw::OutOfMemoryException &)
                    {
                        throw;
                    }
                    catch (const Cdmw::BadParameterException &)
                    {
                        throw;
                    }
                    catch (const Cdmw::InternalErrorException &)
                    {
                        throw;
                    }
                    catch (const CORBA::SystemException &)
                    {
                        throw;
                    }
                }


                /**
                * Purpose:
                * <p> This operation is called whenever the Life Cycle Framework is done
                * with the CDMW factory allocated by a derived factory allocator.
                *
                *@param factory a CDMW factory servant.
                */
                virtual
                void
                dispose(PortableServer::Servant factory)
                throw ()
                {
                    T* fac = dynamic_cast<T*>(factory);
                    CDMW_ASSERT(fac != 0);
                    cleanup(fac);
                }

                /**
                * A static instance of the factory Allocator 
                */
                DECLARE_ANONYMOUS_OBJECTS_FACTORY_ALLOCATOR(T);

            protected:
                /**
                * Purpose:
                * <p> This operation perfoms actual disposal of the CDMW factory.
                * This is an ideal place to persist state of the factory.
                *
                *@param fac A pointer to the CDMW factory.
                */
                void cleanup(T* fac)
                throw ()
                {
                    // Ideal place to persist state
                    fac->_remove_ref(); //deletes fac IF ref-counted!;
                }

#ifndef _MSC_VER

            protected:
#else 
                // FIXME: Compilation problem with VisualC++ 6.0; This will be sorted out once
                // Windows platforms are officially supported.

            public:
#endif

                AnonymousObjectsFactoryAllocator()
                throw()
                {}

                AnonymousObjectsFactoryAllocator(const AnonymousObjectsFactoryAllocator<T> & other)
                throw()
                {}

                ~AnonymousObjectsFactoryAllocator()
                throw()
                {}

        }

        ; // End template AnonymousObjectsFactoryAllocator

        DEFINE_NAMED_OBJECTS_FACTORY_ALLOCATOR(T);

        DEFINE_ANONYMOUS_OBJECTS_FACTORY_ALLOCATOR(T);


    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw
#endif // INCL_LIFECYCLE_FACTORY_ALLOCATOR_HPP

