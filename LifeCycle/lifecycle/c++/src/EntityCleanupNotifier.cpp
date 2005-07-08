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


#include "LifeCycle/lifecycle/EntityCleanupNotifier.hpp"
#include <map>
#include <algorithm>
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "lifecycle/CdmwEntityData.stub.hpp"

namespace
{

    /**
     * Class responsible for dumping content of an entry to an output stream
     */

    class EntryDumper
    {

        public:
            EntryDumper(std::ostream & o)
                    : m_os(o)
            {}

            void operator()(
                const std::pair<const std::string , const std::string > & p
            ) const
            {
                m_os << "<ObjectId = " << p.first << ", Key = " << p.second << ">\n";
            }

        private:
            std::ostream & m_os;
    };

    /**
     * Class responsible for appending to a CdmwLifeCycle::EntityData sequence
     * a pait of <oid,key>.
     */

    class EntityDataFiller
    {

        public:
            EntityDataFiller(CdmwLifeCycle::EntityStrategy::EntityData & ed)
                    : m_ed(ed)
            {}

            void operator()(const std::pair<const std::string, const std::string > & p)
            const
            {
                CORBA::ULong ed_lg = m_ed.length();
                m_ed.length(ed_lg + 1);
                PortableServer::ObjectId_var oid
                = PortableServer::string_to_ObjectId(p.first.c_str());
                CORBA::ULong lg = oid->length();
                m_ed[ed_lg].object_id.length(lg);

                for (CORBA::ULong i = 0; i < lg;++i)
                    m_ed[ed_lg].object_id[i] = oid[i];

                m_ed[ed_lg].object_key = CORBA::string_dup(p.second.c_str());
            }

        private:
            CdmwLifeCycle::EntityStrategy::EntityData & m_ed;
    };
}

; // End anonymous namespace

namespace Cdmw
{

    namespace LifeCycle
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        class EntityCleanupNotifier::EntriesMap :
                    public std::map<std::string, std::string> // map<oid,key>
        {

            public:
                typedef value_type element_type;

                PortableServer::ObjectId*
                get_ObjectId(const element_type & elem) const
                {
                    PortableServer::ObjectId_var oid
                    = PortableServer::string_to_ObjectId(elem.first.c_str());
                    return oid._retn();
                }

                std::string
                get_key(const element_type & elem) const
                {
                    return elem.second;
                }

                const_iterator
                find_by_key(const std::string & key) const throw()
                {
                    return std::find_if(begin(), end(), KeyFinder(key));
                }

                iterator
                find_by_key(const std::string & key) throw()
                {
                    return std::find_if(begin(), end(), KeyFinder(key));
                }

                const_iterator
                find_by_ObjectId(const PortableServer::ObjectId & oid) const throw()
                {
                    const_iterator where;

                    try
                    {
                        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
                        std::string s_oid(s.in());

                        where = find(s_oid);
                    }
                    catch (...)
                    {
                        where = end();
                    }

                    return where;
                }

                iterator
                find_by_ObjectId(const PortableServer::ObjectId & oid) throw()
                {
                    iterator where;

                    try
                    {
                        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
                        std::string s_oid(s.in());

                        where = find(s_oid);
                    }
                    catch (...)
                    {
                        where = end();
                    }

                    return where;
                }

                bool
                insert_entry(const PortableServer::ObjectId & oid, const std::string & key) throw()
                {
                    bool done = false;

                    try
                    {
                        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
                        std::string s_oid(s.in());

                        iterator where = find(s_oid);

                        if (where == end())
                        {
                            insert(std::make_pair<std::string, std::string>(s_oid, key));
                            done = true;
                        }
                    }
                    catch (...)
                    {}

                    return done;
                }

                bool
                remove_by_ObjectId(const PortableServer::ObjectId & oid) throw()
                {
                    bool done = false;

                    try
                    {
                        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
                        std::string s_oid(s.in());

                        iterator where = find(s_oid);

                        if (where != end())
                        {
                            erase(s_oid);
                            done = true;
                        }
                    }
                    catch (...)
                    {}

                    return done;
                }

                bool
                remove_by_key(const std::string & key) throw()
                {
                    bool done = false;

                    try
                    {

                        iterator where = find_by_key(key);

                        if (where != end())
                        {
                            erase(where->first);
                            done = true;
                        }
                    }
                    catch (...)
                    {}

                    return done;
                }

            private:
                // Function object for the find_if algorithm to search for
                // entries by object_key instead of ObjectId.

                class KeyFinder
                {

                    public:
                        KeyFinder(const std::string & key)
                                : m_key(key)
                        {}

                        bool
                        operator()(const std::pair<const std::string , const std::string > & p) const
                        {
                            return p.second == m_key;
                        }

                    private:
                        const std::string & m_key;
                };
        };



        EntityCleanupNotifier::EntityCleanupNotifier(CdmwLifeCycle::ObjectRegistration_ptr domain)
        throw(CORBA::SystemException,
              Cdmw::OutOfMemoryException)
                : m_domain(CdmwLifeCycle::ObjectRegistration::_duplicate(domain)),
                m_entries(0),
                m_property_set(CosPropertyService::PropertySet::_nil()),
                m_property_name("")
        {
            try
            {
                std::auto_ptr<EntriesMap> entries(new EntriesMap());
                m_entries = entries;
            }
            catch (const std::bad_alloc &)
            {
                CDMW_THROW(Cdmw::OutOfMemoryException);
            }
        }

        EntityCleanupNotifier::~EntityCleanupNotifier()
        throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);
        }

        void EntityCleanupNotifier::unregister_object(const PortableServer::ObjectId & oid)
        throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);

            CDMW_WRITER_LOCK_GUARD(m_readers_writer_lock);

            EntriesMap::iterator where = m_entries->find_by_ObjectId(oid);

            if (where != m_entries->end())
            {
                try
                {
                    std::string key = m_entries->get_key(*where);
                    m_domain->unregister_object(key.c_str());
                    m_entries->erase(where);
                }
                catch (const CosNaming::NamingContext::NotFound & )
                {
                    // This should not happen (TODO): ADD Trace logs
                }
                catch (const CosNaming::NamingContext::InvalidName & )
                {
                    // This should not happen either (TODO): ADD Trace logs
                }
                catch (const CORBA::SystemException & )
                {
                    // What to do!!! (TODO): ADD Trace logs
                }
            }
            else
            {
                // Ignore
            }

        }


        // Inserts a (oid,key) pair. Returns false if entry is already there.
        bool
        EntityCleanupNotifier::insert_entry(const PortableServer::ObjectId & oid,
                                            const std::string & key)
        throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);

            CDMW_WRITER_LOCK_GUARD(m_readers_writer_lock);
            return m_entries->insert_entry(oid, key);
        }

        // Removes the (oid,key) pair.
        bool
        EntityCleanupNotifier::remove_entry(const PortableServer::ObjectId & oid)
        throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);

            CDMW_WRITER_LOCK_GUARD(m_readers_writer_lock);
            return m_entries->remove_by_ObjectId(oid);
        }

        bool
        EntityCleanupNotifier::remove_entry(const std::string & key)
        throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);

            CDMW_WRITER_LOCK_GUARD(m_readers_writer_lock);
            return m_entries->remove_by_key(key);
        }

        bool
        EntityCleanupNotifier::contains(const std::string & key) const
        throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);

            CDMW_READER_LOCK_GUARD(m_readers_writer_lock);
            EntriesMap::const_iterator where = m_entries->find_by_key(key);
            return where != m_entries->end();
        }

        bool
        EntityCleanupNotifier::contains(const PortableServer::ObjectId & oid) const
        throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);

            CDMW_READER_LOCK_GUARD(m_readers_writer_lock);
            EntriesMap::const_iterator where = m_entries->find_by_ObjectId(oid);
            return where != m_entries->end();
        }

        // Returns Number of entries
        size_t
        EntityCleanupNotifier::entries() const throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);

            CDMW_READER_LOCK_GUARD(m_readers_writer_lock);
            return m_entries->size();
        }

        void
        EntityCleanupNotifier::dump(std::ostream & os) const throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);

            CDMW_READER_LOCK_GUARD(m_readers_writer_lock);
            os << "EntityCleanupNofifier  --- Size = " << m_entries->size() << "\n";
            std::for_each(m_entries->begin(), m_entries->end(), EntryDumper(os));
        }


        // These should only be called when associated entry does exist!
        std::string
        EntityCleanupNotifier::ObjectId_to_key(const PortableServer::ObjectId& oid) const
        throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);

            CDMW_READER_LOCK_GUARD(m_readers_writer_lock);
            EntriesMap::const_iterator where = m_entries->find_by_ObjectId(oid);
            CDMW_ASSERT(where != m_entries->end());

            return m_entries->get_key(*where);
        }

        PortableServer::ObjectId*
        EntityCleanupNotifier::key_to_ObjectId(const std::string & key) const
        throw()
        {
            CDMW_ASSERT(m_entries.get() != NULL);

            CDMW_READER_LOCK_GUARD(m_readers_writer_lock);
            EntriesMap::const_iterator where = m_entries->find_by_key(key);
            CDMW_ASSERT(where != m_entries->end());

            return m_entries->get_ObjectId(*where);
        }

        bool
        EntityCleanupNotifier::init(CosPropertyService::PropertySet_ptr property_set,
                                    const std::string & property_name)
        throw()
        {
            bool result = false;

            if (!CORBA::is_nil(property_set))
            {
                CDMW_ASSERT(!property_name.empty());

                // The follows steps are performed:
                // 1. Get value of property named "propety_name"
                // 2. For each value, insert_entry to internal entries database
                // 3. If PropertyNotFound exception is raised, it means it's first usage.
                //    Do not create a property at this time but do it in finish() before
                //    persisting state.

                try
                {
                    m_property_set = CosPropertyService::PropertySet::_duplicate(property_set);
                    m_property_name = property_name;

                    CORBA::Any_var any
                    = property_set->get_property_value(property_name.c_str());

                    // Data is stored as sequence<struct{oid;key}>
                    const CdmwLifeCycle::EntityStrategy::EntityData* pData;
                    any.in() >>= pData;

                    for (CORBA::ULong i = 0; i < pData->length(); ++i)
                    {
                        const CORBA::Octet * buff = (*pData)[i].object_id.get_buffer();
                        CORBA::ULong lg = (*pData)[i].object_id.length();

                        // Avoid copy
                        PortableServer::ObjectId oid(lg,
                                                     lg,
                                                     const_cast<CORBA::Octet*>(buff),
                                                     false);
                        insert_entry(oid, (*pData)[i].object_key.in());
                    }

                    result = true;
                }
                catch (const CosPropertyService::PropertyNotFound&)
                {
                    // Property not found, create at later!
                    result = true;
                }
                catch (...)
                {
                    // Possible exceptions are std::bad_alloc and CORBA::SystemException
                    result = false;
                }
            }

            return result;
        }


        bool
        EntityCleanupNotifier::finish()
        throw()
        {
            bool result = false;

            if (!CORBA::is_nil(m_property_set.in()))
            {
                try
                {
                    // Create a CORBA::Any containing current state
                    CDMW_READER_LOCK_GUARD(m_readers_writer_lock);
                    CORBA::ULong lg = m_entries->size();
                    CdmwLifeCycle::EntityStrategy::EntityData * ed =
                        new CdmwLifeCycle::EntityStrategy::EntityData(lg);
                    ed->length(0);
                    std::for_each(m_entries->begin(), m_entries->end(), EntityDataFiller(*ed));

                    CORBA::Any any;
                    any <<= ed; // consuming insertion

                    // This will either define a new property or set a new value
                    // for the property.
                    m_property_set->define_property(m_property_name.c_str(), any);
                    // State is persisted!
                    result = true;
                }
                catch (const CORBA::Exception &)
                {
                    // Possible exceptions are:
                    //    . CosPropertyService::ReadOnlyProperty
                    //    . CosPropertyService::ConflictingProperty
                    //    . CosPropertyService::UnsupportedProperty
                    //    . CosPropertyService::UnsupportedTypeCode
                    //    . CosPropertyService::InvalidPropertyName
                    //    . CORBA::SystemException

                    // do nothing for this version
                }

            }

            return result;
        }


    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw



