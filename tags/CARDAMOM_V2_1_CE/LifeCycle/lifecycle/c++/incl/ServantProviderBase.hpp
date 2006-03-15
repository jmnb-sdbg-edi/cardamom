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


#ifndef INCL_LIFECYCLE_SERVANT_PROVIDER_BASE_HPP
#define INCL_LIFECYCLE_SERVANT_PROVIDER_BASE_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"
#include <string>

namespace Cdmw
{

    namespace LifeCycle
    {

        class CleanupNotifier;
        /**
        *Purpose:
        *<p>    Base class for all servant providers
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class ServantProviderBase
        {

            public:
                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~ServantProviderBase()
                throw();

                /**
                * Purpose:
                * <p>This method acts as a hand shaking between the Servant Provider
                * and the generated Factory implementation. It indicates whether
                * a Servant provided by this Servant Provider may incarnate multiple
                * objects (with ObjectIds provided by this Servant Provider)
                *
                *@return <B>true</B> if a servant may incarnate multiple objects, 
                * <B>false</B> otherwise.
                */
                virtual bool may_incarnate_multiple_objects() const throw() = 0;

                /**
                * Purpose:
                * <p>This method provides a servant for the CORBA object with
                * the specified ObjectId. It is called by the Life Cycle Framework
                * whenever a servant is needed to incarnate an object.
                * 
                *@param oid ObjectId of the object to be incarnated
                *
                *@return A servant incarnating the object 
                *
                *@exception PortableServer::ForwardRequest Indicates to the Life Cycle
                * Framework ORB that the SevantProvider cannot handle this object 
                * and requests can be handled to the returned IOR. If this exception 
                * is translated/forwarded to the client-side ORB, the request invocation 
                * will be transparently re-transmitted to the returnded object.
                *@exception CORBA::SystemException Any CORBA system exception
                */
                virtual PortableServer::Servant
                lookup_servant(const PortableServer::ObjectId& oid)
                throw(PortableServer::ForwardRequest,
                      CORBA::SystemException) = 0; // TODO: do we realy need CORBA::SystemException or CORBA::OBJECT_NOT_EXIST only

                /**
                * Purpose:
                * <p>This method is called by the Life Cycle Framework whenever it is done
                * with the specified servant. The ServantProvider is free to handle it at
                * will.
                * 
                *@param oid ObjectId of the object incarnated by the servant
                *@param poa The POA where the object is activated
                *@param servant The servant in question
                *
                *@exception CORBA::SystemException Any CORBA system exception
                */
                virtual void
                release_servant(const PortableServer::ObjectId& oid,
                                PortableServer::POA_ptr poa,
                                PortableServer::Servant servant)
                throw(CORBA::SystemException) = 0; // TODO: do we realy need CORBA::SystemException

                /**
                * Purpose:
                * <p>This method returns the RepositoryID of the object associated to the
                * <I>oid</I>.
                * 
                *@param oid ObjectId of the object
                *
                *@return The RepositoryId of the object
                *
                *@exception CORBA::OBJECT_NOT_EXIST The object does not exist any more.
                */
                virtual std::string
                get_supported_interface(const PortableServer::ObjectId& oid) const
                throw(CORBA::OBJECT_NOT_EXIST) = 0;

                /**
                * Purpose:
                * <p>This method indicates whether the ServantProvider supports objects
                * with the specified Repository Id.
                * 
                *@param repository_id RepositoryID of an object
                *
                *@return <B>true</B> if objects with the specified repository ID are
                * supported by the ServantProvider, <B>false</B> otherwise.
                */
                bool supports(const std::string & repository_id) const
                throw();

                /**
                * Purpose:
                * <p>This returns a sequence of all RepositoryIDs of objects supported by
                * the current ServantProvider. Note that this operations follows CORBA/C++
                * programming style; and that the caller is responsible for deallocating/
                * freeing returned sequence.
                * 
                *@return A sequence of RepositoryIds supported by the ServantProvider.
                */
                CdmwLifeCycle::RepositoryIdSeq* get_supported_objects() const
                throw();

                // NOTA: notifier should not be destroyed!
                void set_cleanup_notifier(CleanupNotifier * notifier) throw();

            protected:

                /**
                * Purpose:
                * <p>This operation be called by the user-defined servant provider whenever
                * an object is destroyed. It enables the Life Cycle Framework to perform
                * further cleanup operations following the destruction of a Cdmw object.
                * 
                *@param The ObjectId of the destroyed object
                */
                void cleanup_object(const PortableServer::ObjectId & oid) throw();

                /**
                * This returns an array containing repository IDs of all objects
                * supported by the current ServantProvider.
                * <B>NOTA:</B> Repository IDs should be statically allocated; thus derived classes
                * should not delete return pointer(s).
                */
                virtual const char ** get_ids() const throw() = 0;

                /**
                * This returns the number of repository Ids supported bt the current ServantProvider.
                * <B>NOTA:</B> This should correspond to the number of null terminated strings
                * returned by get_ids() operation.
                */
                virtual CORBA::Long get_number_of_ids() const throw() = 0;
                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                ServantProviderBase()
                throw();

            private:
                CleanupNotifier * m_cleanup_notifier;

                // Hide copy constructor/assignment operator
                ServantProviderBase(const ServantProviderBase& rhs)
                throw();
                ServantProviderBase&
                operator=(const ServantProviderBase& rhs)
                throw();

        }

        ; // End class ServantProviderBase

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw
#endif // INCL_LIFECYCLE_SERVANT_PROVIDER_BASE_HPP

