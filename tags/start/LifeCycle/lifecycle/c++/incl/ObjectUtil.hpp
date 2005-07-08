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


#ifndef INCL_LIFECYCLE_OBJECT_UTIL_HPP
#define INCL_LIFECYCLE_OBJECT_UTIL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/ObjectTraits.hpp"
#include "LifeCycle/lifecycle/ServantProviderBase.hpp"
#include <string>


namespace Cdmw
{

    namespace LifeCycle
    {

        /**
        *Purpose:
        *<p>    A utility class for use by the LifeCycle Framewotk
        * for object creation and activation.
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        template <typename T, class OT = Cdmw::OrbSupport::object_traits<T> >

        class ObjectUtil
        {

            public:
                /**
                * An alias to a CORBA _ptr type of the object
                */
                typedef typename OT::_ptr_type _ptr_type;

                /**
                * An alias to a CORBA _var type of the object
                */
                typedef typename OT::_var_type _var_type;

                /**
                * Purpose:
                * <p>This creates a corba object without activating it. It first gets
                * the repository Id of the object from the servant provider, creates
                * the object reference and then asserts the object is really of the expected 
                * type.
                *
                *@param poa The POA responsible for creating the object reference
                *@param oid The ObjectId to use for the object reference
                *@param servant_provider A pointer to the Servant Provider
                *
                *@return A new object reference narrowed to the appropriate type
                *
                *@exception CORBA::INTERNAL Indicates that a ServantProvider or a LifeCycle
                * Framework error
                */
                static _ptr_type
                create_object_reference(PortableServer::POA_ptr poa,
                                        const PortableServer::ObjectId & oid,
                                        ServantProviderBase* servant_provider)
                throw (CORBA::INTERNAL)
                {
                    // [a] Get the repository Id
                    // May raise CORBA::OBJECT_NOT_EXIST, but this is not possible since we just
                    // got the object ID from the servant provider.
                    std::string repid;

                    try
                    {
                        repid = servant_provider->get_supported_interface(oid) ;
                    }
                    catch (const CORBA::OBJECT_NOT_EXIST & )
                    {
                        // This should not happen at this stage since the object is just
                        // about to be created and the servant provider has created the object id.
                        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALServantProviderError,
                                              CORBA::COMPLETED_YES);
                    }

                    _var_type ref = T::_nil();
                    // [b] Create the object reference

                    try
                    {
                        CORBA::Object_var obj
                        = poa->create_reference_with_id(oid, repid.c_str());
                        ref = T::_narrow(obj.in());
                        // [c] Assert the object is really of the expected type
                        CDMW_ASSERT(!CORBA::is_nil(ref.in()));
                    }
                    catch (const PortableServer::POA::WrongPolicy & )
                    {
                        // Thrown by create_reference_with_id.
                        // This should not happen as wa are using USER_ID policy.
                        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                              CORBA::COMPLETED_YES);
                    }
                    catch (const CORBA::SystemException & )
                    {
                        // This should not happen at this stage since the object is just
                        // about to be created and the servant provider has created the object id.
                        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALServantProviderError,
                                              CORBA::COMPLETED_YES);
                    }

                    return ref._retn();
                }

                /**
                * Purpose:
                * <p>This activates an object with the specified <I>oid</I> and <I>servant</I>.
                * It activates the servant through the specified POA, retrieves its object's reference
                * and then asserts it is really of the expected type.
                *
                *@param poa The POA responsible for activating the object
                *@param oid The ObjectId of the CORBA object
                *@param servant A pointer to the Servant
                *
                *@return The object reference of the CORBA object narrowed to the appropriate type
                *
                *@exception CORBA::INTERNAL Indicates that a ServantProvider or a LifeCycle
                * Framework error
                */
                static _ptr_type
                activate_object(PortableServer::POA_ptr poa,
                                const PortableServer::ObjectId & oid,
                                PortableServer::Servant servant)
                throw (CORBA::INTERNAL)
                {
                    try
                    {
                        // [b] Activate object
                        // We are using a servant activator
                        poa -> activate_object_with_id (oid, servant);
                    }
                    catch (const PortableServer::POA::ServantAlreadyActive &)
                    {
                        // Thrown by activate_object_with_id.
                        // We use UNIQUE_ID policy so the servant should not be reused for
                        // another ObjectId with this POA.
                        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALServantProviderError,
                                              CORBA::COMPLETED_YES);
                    }
                    catch (const PortableServer::POA::ObjectAlreadyActive &)
                    {
                        // Thrown by activate_object_with_id.
                        // The object is already active! Has the servant provider activated it?
                        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALServantProviderError,
                                              CORBA::COMPLETED_YES);
                    }
                    catch (const PortableServer::POA::WrongPolicy &)
                    {
                        // Thrown by activate_object_with_id.
                        // This should not happen as wa are using RETAIN policy.
                        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                              CORBA::COMPLETED_YES);
                    }

                    _var_type ref = T::_nil();

                    // [c] Create object reference
                    // This may raise
                    //    PortableServer::POA::ObjectNotActive,
                    //    PortableServer::POA::WrongPolicy, and
                    //    CORBA::SystemException

                    try
                    {
                        CORBA::Object_var obj = poa -> id_to_reference (oid);

                        ref = T::_narrow (obj.in());
                        // [d] Assert the object is really of the expected type
                        CDMW_ASSERT(!CORBA::is_nil(ref.in()));
                    }
                    catch (...)
                    {
                        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                              CORBA::COMPLETED_YES);
                    }

                    return ref._retn();
                }

                /**
                * Purpose:
                * <p> This acquire servant from user for oid
                *
                *@param oid The ObjectId of the CORBA object
                *@param servant_provider A pointer to the Servant Provider
                *
                *@return The object reference of the CORBA object narrowed to the appropriate type
                *
                *@exception CORBA::INTERNAL Indicates that a ServantProvider or a LifeCycle
                * Framework error
                */
                static PortableServer::Servant
                lookup_servant( const PortableServer::ObjectId & oid,
                                ServantProviderBase* servant_provider)
                throw (CORBA::INTERNAL)
                {
                    PortableServer::Servant servant;
                    // [a] Acquire servant from user for oid

                    try
                    {
                        servant = servant_provider->lookup_servant(oid);

                    }
                    catch (const PortableServer::ForwardRequest &)
                    {
                        // Thrown by lookup_servant.
                        // This should not happen at this stage since the object is just
                        // about to be created and the previous call to create_resource should
                        // have failed.
                        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALServantProviderError,
                                              CORBA::COMPLETED_YES);
                    }
                    catch (const CORBA::SystemException &)
                    {
                        // This should not happen at this stage since the object is just
                        // about to be created and the previous call to create_resource should
                        // have failed.
                        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALServantProviderError,
                                              CORBA::COMPLETED_YES);
                    }

                    return servant;
                }

                /**
                * Purpose:
                * <p>This activates an object with the specified <I>oid</I>. It first acquires
                * a servant incarnating the object from the servant provider, activates it through
                * the specified POA, retrieves its object's reference and then asserts it is really
                * of the expected type.
                *
                *@param poa The POA responsible for activating the object
                *@param oid The ObjectId of the CORBA object
                *@param servant_provider A pointer to the Servant Provider
                *
                *@return The object reference of the CORBA object narrowed to the appropriate type
                *
                *@exception CORBA::INTERNAL Indicates that a ServantProvider or a LifeCycle
                * Framework error
                */
                static _ptr_type
                activate_object(PortableServer::POA_ptr poa,
                                const PortableServer::ObjectId & oid,
                                ServantProviderBase* servant_provider)
                throw (CORBA::INTERNAL)
                {
                    // [a] Acquire servant from user for oid
                    PortableServer::Servant servant
                    = lookup_servant(oid, servant_provider);

                    return activate_object(poa, oid, servant);
                }
        };


    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw

#endif // INCL_LIFECYCLE_OBJECT_UTIL_HPP

