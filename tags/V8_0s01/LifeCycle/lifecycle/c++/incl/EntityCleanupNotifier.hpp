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


#ifndef INCL_LIFECYCLE_ENTITY_CLEANUP_NOTIFIER_HPP
#define INCL_LIFECYCLE_ENTITY_CLEANUP_NOTIFIER_HPP

#include "Foundation/orbsupport/CORBA.hpp"

#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"
#include "LifeCycle/lifecycle/CleanupNotifier.hpp"
#include "Foundation/idllib/CosPropertyService.stub.hpp"
#include <string>
#include <iostream>
#include <memory>

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

        /**
        *Purpose:
        *<p>    This class provides a Cleanup Notifier for Life Cycle Framewotk 
        * with "Entity" strategy. It maintains (object key, object Id) associations
        * of CDMW objects registered with a NameDomain. This enables unregistration
        * of the object key from the NameDomain when needed.
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        * TODO: Add a ReadersWriter lock for Thread Safety
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class EntityCleanupNotifier : public CleanupNotifier
        {

            public:
                /**
                 * Purpose:
                 * <p> [Constructor description if necessary]
                 * 
                 *@param domain The CDMW NameDomain where stored object keys are 
                 * registered.
                 *
                 *@exception CORBA::SystemException any CORBA system exception that may
                 * be raised by the _duplicate CORBA operation.
                 *@exception Cdmw::OutOfMemoryException A failure to allocate memory 
                 * for internal entries database.
                 */
                EntityCleanupNotifier(CdmwLifeCycle::ObjectRegistration_ptr domain)
                throw(CORBA::SystemException,
                      Cdmw::OutOfMemoryException);
                /**
                 * Purpose:
                 * <p> [ Destructor description if necessary]
                 * 
                 */
                virtual
                ~EntityCleanupNotifier()
                throw();

                /**
                 * Purpose:
                 * <p>Unregisters the objet key associated to the specified <I>oid</I> from
                 * the NameDomain and deletes its entry from the internal entries database.
                 *
                 *@param oid The Object Id of the CDMW object registered with the NameDomain.
                 *
                 */
                virtual void unregister_object(const PortableServer::ObjectId & oid) throw();

                /**
                 * Purpose:
                 * <p> Inserts an (oid,key) pair to the internal database.
                 *
                 *@param oid The Object Id of a CDMW object registered within the NameDomain.
                 *@param key The Object Key used for the object's registration in the NameDomain.
                 *
                 *@return <B>true</B> if inserted successfuly, otherwise <B>false</B> is returned
                 * (the entry may already be there).
                 */
                bool insert_entry(const PortableServer::ObjectId & oid, const std::string & key)
                throw();

                /**
                 * Purpose:
                 * <p> Removes the entry with the specified <I>oid</I> from the internal database.
                 *
                 *@param oid The object Id field of the pair to be removed.
                 *
                 *@return <B>true</B> if removed successfuly, otherwise <B>false</B> is returned
                 * (the entry may not be present).
                 */
                bool remove_entry(const PortableServer::ObjectId & oid)
                throw();

                /**
                 * Purpose:
                 * <p> Removes the entry with the specified <I>key</I> from the internal database.
                 *
                 *@param key The object key field of the pair to be removed.
                 *
                 *@return <B>true</B> if removed successfuly, otherwise <B>false</B> is returned
                 * (the entry may not be present).
                 */
                bool remove_entry(const std::string & key)
                throw();

                /**
                 * Purpose:
                 * <p> This tests whether a entry with the specified <I>key</I> is present
                 * in the internal database.
                 *
                 *@param key The object key of the requested entry.
                 *
                 *@return <B>true</B> if an entry with the specified <I>key</I> field is
                 * present in the internal database. Otherwise return <B>false</B>.
                 */
                bool contains(const std::string & key) const throw();

                /**
                 * Purpose:
                 * <p> This tests whether a entry with the specified <I>oid</I> is present
                 * in the internal database.
                 *
                 *@param oid The object Id of the requested entry.
                 *
                 *@return <B>true</B> if an entry with the specified <I>oid</I> field is
                 * present in the internal database. Otherwise return <B>false</B>.
                 */
                bool contains(const PortableServer::ObjectId & oid) const throw();

                /**
                 * Purpose:
                 * <p> Returns the number of entries in the internal database.
                 *
                 *@return The number of entries.
                 */
                size_t entries() const throw();

                /**
                 * Purpose:
                 * <p> This dumps content of the internal database to the specified
                 * output stream.
                 *
                 *@param os A reference to an output stream.
                 */
                void dump(std::ostream & os) const throw();


                /**
                 * Purpose:
                 * <p> This operation initialises internal database using a property as
                 * defined by the OMG CosPropertyService.
                 *
                 *@param property_set  A reference to the PropertySet containg the property.
                 *@param property_name Name of the property within the property set above.
                 *
                 *@return <B>true</B> if <I>init</I> was successful, <B>false</B> otherwise.
                 */
                bool init(CosPropertyService::PropertySet_ptr property_set,
                          const std::string & property_name) throw();

                /**
                 * Purpose:
                 * <p> This operation instructs the EntityCleanupNotifier that the LifeCycle
                 * framework is done with it and can then persist its state for next sessions.
                 *
                 *@return <B>true</B> if done successfuly, <B>false</B> otherwise.
                 */
                bool finish() throw();

            protected:
                /**
                 * Purpose:
                 * <p> This returns the <I>key</I> value of the entry with the specified
                 * object Id.
                 *
                 *@return The object key value of the entry.
                 */
                std::string ObjectId_to_key(const PortableServer::ObjectId& oid) const
                throw();

                /**
                 * Purpose:
                 * <p> This returns the Object Id value of the entry with the specified
                 * object key.
                 *
                 *@return The Object Id value of the entry.
                 */
                PortableServer::ObjectId* key_to_ObjectId(const std::string & key) const
                throw();

            private:

                /**
                 * Purpose:
                 * <p> [Constructor description if necessary]
                 * 
                 *@param
                 *@exception
                 */
                EntityCleanupNotifier()
                throw(CORBA::SystemException,
                      Cdmw::OutOfMemoryException);

                /**
                 * Object reference of the NameDomain where CDMW objects whose
                 * (oid,key) pairs are stored in self are registered.
                 */
                CdmwLifeCycle::ObjectRegistration_var m_domain;

                /**
                 * Internal Entries Database.
                 */

                class EntriesMap;
                std::auto_ptr<EntriesMap> m_entries;

                /**
                 * Property set for state storage/retreival
                 */
                CosPropertyService::PropertySet_var m_property_set;

                /**
                 * Name of the property within the property set
                 */
                std::string m_property_name;

                /**
                 * Readers/Writer Lock to protect from concurrent access to internal data
                 */
                mutable Cdmw::OsSupport::ReaderWriterLock m_readers_writer_lock;

                // Hide copy constructor/assignment operator
                EntityCleanupNotifier(const EntityCleanupNotifier& rhs)
                throw();
                EntityCleanupNotifier&
                operator=(const EntityCleanupNotifier& rhs)
                throw();
        }

        ; // End class EntityCleanupNotifier

    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw
#endif // INCL_LIFECYCLE_ENTITY_CLEANUP_NOTIFIER_HPP


