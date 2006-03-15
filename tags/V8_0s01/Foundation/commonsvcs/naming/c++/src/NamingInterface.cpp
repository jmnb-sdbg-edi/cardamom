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


#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "naming/NameParser.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

namespace Cdmw
{
namespace CommonSvcs
{
    
    namespace Naming
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */

        // Default chunk size for CosNaming::NamingContext::list operation
        const CORBA::ULong NamingInterface::LIST_CHUNK_SIZE(100);

        // CORBA 2.3 C++ mapping states that _nil() may not throw
        // any CORBA exception. Exception specification is added to
        // have the same exception specification for all constructors
        NamingInterface::NamingInterface()
        throw(CORBA::SystemException)
                : m_context(CosNaming::NamingContext::_nil())
        {
            // does nothing by default
        }

        // _duplicate() may throw CORBA system exceptions
        NamingInterface::NamingInterface(CosNaming::NamingContext_ptr nc)
        throw(CORBA::SystemException)
                : m_context(CosNaming::NamingContext::_duplicate(nc))
        {

            using namespace Cdmw::OrbSupport;

            if (CORBA::is_nil(nc))
                throw CORBA::BAD_PARAM(BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);
        }

        NamingInterface::NamingInterface(const NamingInterface & rhs)
        throw(CORBA::SystemException)
                : m_context(CosNaming::NamingContext::_duplicate(rhs.m_context.in()))
        {
            // does nothing
        }

        NamingInterface::~NamingInterface()
        throw()
        {
            // does nothing
        }

        // This is not thread safe
        NamingInterface& NamingInterface::operator=(const NamingInterface & rhs)
        throw(CORBA::SystemException)
        {
            if (&rhs != this)
            {
                NamingInterface temp(rhs);
                m_context = temp.m_context._retn(); // transfer ownership (should not throw)
            }

            return *this;
        }

        void
        NamingInterface::append_chunk(const CosNaming::BindingList & bl,
                                      BindingList& blist)
        {
            for (CORBA::ULong i = 0; i < bl.length(); ++i)
            {
                blist.push_back(bl[i]);
            }
        }

        // Returns true if list is done, false if interrupted
        // If how_many is set to zero, the client is requesting to use only the
        // BindingIterator bi to access the bindings and list returns a zero length sequence
        // in bl.
        // NOTA: BindingIterator::next_n : A zero value of how_many is illegal and raises
        // a BAD_PARAM system exception.
        bool
        NamingInterface::list_context(BindingList& blist,
                                      CORBA::ULong how_many)
        throw()
        {
            bool done = true;
            CosNaming::BindingIterator_var it;                  // Iterator reference
            CosNaming::BindingList_var bl;                  // Binding list

            try
            {
                CORBA::ULong chunk_size = how_many;

                m_context->list(chunk_size, bl, it);             // Get first chunk
                append_chunk(bl.in(), blist);                       // Append first chunk

                if (how_many == 0L)
                    chunk_size = LIST_CHUNK_SIZE;

                if (!CORBA::is_nil(it.in()))
                {                       // More bindings?
                    CORBA::Boolean more;

                    do
                    {
                        more = it->next_n(chunk_size, bl);       // Get next chunk
                        append_chunk(bl.in(), blist);                // Append chunk
                    }
                    while (more);                             // While not done

                    it->destroy();                              // Clean up iterator
                }
            }
            catch (const CORBA::SystemException &)
            {
                done = false;
            }
            catch (...)
            { // std::bad_alloc is the most elligible exception ...
                done = false;
            }

            return done;
        }

        bool
        NamingInterface::clear_chunk(CosNaming::NamingContext_ptr nc,
                                     const CosNaming::BindingList & bl)
        throw()
        {
            bool done = true;

            for (CORBA::ULong i = 0; i < bl.length(); ++i)
            {
                // Note from CosNaming specification:
                //   In struct Binding, binding_name is incorrectly defined
                //   as a Name instead of a NameComponent. This definition is
                //   unchanged for compatibility reasons.

                try
                {
                    if (bl[i].binding_type == CosNaming::ncontext)
                    {
                        // For contexts, first destroy then unbind
                        CORBA::Object_var obj = nc->resolve(bl[i].binding_name);
                        CosNaming::NamingContext_var __nc__
                        = CosNaming::NamingContext::_narrow(obj.in());

                        // Some CosNaming Implementations may return a nil object reference
                        // if the naming context have been removed!

                        if (CORBA::is_nil(__nc__.in()))
                        {
                            // keep silent (ignore)
                            done = false;
                        }
                        else
                        {
                            try
                            {
                                __nc__->destroy();
                                nc->unbind(bl[i].binding_name);
                                done &= true;
                            }
                            catch (const CosNaming::NamingContext::NotEmpty &)
                            {
                                bool context_done = clear_context(__nc__.in());

                                if (context_done)
                                {
                                    __nc__->destroy();
                                    nc->unbind(bl[i].binding_name);
                                    done &= true;
                                }
                            }
                            catch (const CORBA::SystemException & )
                            {
                                throw;
                            }
                        }
                    }
                    else // object
                        nc->unbind(bl[i].binding_name);
                }
                catch (const CORBA::Exception & )
                {
                    // keep silent (ignore)
                    done = false;
                }
            }

            return done;
        }

        // Depth first
        bool
        NamingInterface::clear_context(CosNaming::NamingContext_ptr nc)
        throw()
        {
            bool all_done = false;
            CosNaming::BindingIterator_var it;                  // Iterator reference
            CosNaming::BindingList_var bl;                  // Binding list

            try
            {
                nc->list(LIST_CHUNK_SIZE, bl, it);                // Get first chunk
                bool chunk_done = clear_chunk(nc, bl.in());          // Clear first chunk

                if (!CORBA::is_nil(it.in()))
                {                       // More bindings?
                    CORBA::Boolean more;

                    do
                    {
                        more = it->next_n(LIST_CHUNK_SIZE, bl);  // Get next chunk
                        chunk_done &= clear_chunk(nc, bl.in());      // Clear chunk
                    }
                    while (more);                             // While not done

                    it->destroy();                              // Clean up iterator
                }

                all_done = chunk_done;
            }
            catch (const CORBA::SystemException &)
            {
                // keep silent (ignore)
            }

            return all_done;
        }

        // Forces creation of intermediate naming contexts if requested
        CosNaming::NamingContext_ptr
        NamingInterface::bind_new_context(const CosNaming::Name & name, bool force)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::AlreadyBound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException)
        {

            using namespace CosNaming;

            NamingContext_var nc
            = NamingContext::_duplicate(m_context.in());

            try
            {
                nc = m_context -> bind_new_context(name);
            }
            catch (const NamingContext::AlreadyBound &)
            {
                // re-throw the exception as AlreadyBound will also be received
                // if the name is bound to an object instead of a naming context
                throw;
            }
            catch (const NamingContext::NotFound & ex)
            {
                if (force && ex.why == NamingContext::missing_node)
                {
                    // This may raise an exception if it fails
                    const CORBA::ULong rest = ex.rest_of_name.length();
                    CORBA::ULong lg = name.length() - rest;

                    if (lg > 0)
                    {
                        Name base(lg);
                        base.length(lg);

                        for (CORBA::ULong i = 0;i < lg;++i)
                            base[i] = name[i];

                        CORBA::Object_var obj = m_context -> resolve(base);

                        nc = NamingContext::_narrow(obj.in());

                        // Some CosNaming Implementations may return a nil object reference
                        // if the naming context have been removed meanwhile...
                        if (CORBA::is_nil(nc.in()))
                            throw NamingContext::CannotProceed();
                    }

                    //  create remaining bindings
                    Name n(1);

                    n.length(1);

                    for (CORBA::ULong j = 0;j < rest;++j)
                    {
                        n[0] = ex.rest_of_name[j];
                        nc = nc -> bind_new_context(n);
                    }
                }
                else
                    throw;
            }
            catch (const NamingContext::CannotProceed &)
            {
                throw;
            }
            catch (const NamingContext::InvalidName &)
            {
                throw;
            }
            catch (const CORBA::SystemException &)
            {
                throw;
            }

            return nc._retn();
        }


        //
        // Operations from CosNaming::NamingContext
        //
        void
        NamingInterface::bind(const std::string& s, CORBA::Object_ptr o, bool force)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CosNaming::NamingContext::AlreadyBound,
              CORBA::SystemException)
        {
            if (!force)
            {
                CosNaming::Name_var name;

                try
                {
                    name = NameParser::to_name(s);
                }
                catch (const InvalidNameException&)
                {
                    throw CosNaming::NamingContext::InvalidName();
                }

                m_context->bind(name.in(), o);
            }
            else
                rebind (s, o, true);
        }

        void
        NamingInterface::rebind(const std::string& s, CORBA::Object_ptr o, bool force)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException)
        {

            using namespace CosNaming;
            Name_var name;

            try
            {
                name = NameParser::to_name(s);
            }
            catch (const InvalidNameException&)
            {
                throw CosNaming::NamingContext::InvalidName();
            }

            if (!force)
                m_context->rebind(name.in(), o);
            else
            {
                try
                {
                    m_context->rebind(name.in(), o);
                }
                catch (const NamingContext::NotFound & ex)
                {
                    if (ex.why == NamingContext::missing_node)
                    {
                        // This may raise an exception if it fails

                        // Recreate whole path and redo binding
                        // Get basename (parent context)

                        // REM: lg__ > 1 otherwise rebind would have succeeded
                        const CORBA::ULong lg__ = name->length() - 1;
                        Name name__(lg__, lg__, name->get_buffer(), false);
                        NamingContext_var nc__ = bind_new_context(name__, force); // force == true
                        CosNaming::Name n__(1);
                        n__.length(1);
                        n__[0] = name[lg__]; // last element
                        nc__ -> rebind(n__, o);
                    }
                    else
                        throw;
                }
                catch (const NamingContext::CannotProceed &)
                {
                    throw;
                }
                catch (const NamingContext::InvalidName &)
                {
                    throw;
                }
                catch (const CORBA::SystemException &)
                {
                    throw;
                }
            }
        }

        // NOTA: if (force == true), rebind_context is called and this may cause
        // an orphaned naming context if it is already bound.
        void
        NamingInterface::bind_context(const std::string& s,
                                      CosNaming::NamingContext_ptr nc,
                                      bool force)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CosNaming::NamingContext::AlreadyBound,
              CORBA::SystemException)
        {
            if (!force)
            {
                CosNaming::Name_var name;

                try
                {
                    name = NameParser::to_name(s);
                }
                catch (const InvalidNameException&)
                {
                    throw CosNaming::NamingContext::InvalidName();
                }

                m_context->bind_context(name.in(), nc);
            }
            else
                rebind_context (s, nc, true);
        }

        void
        NamingInterface::bind_context(const std::string& s,
                                      const NamingInterface& ni,
                                      bool force)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CosNaming::NamingContext::AlreadyBound,
              CORBA::SystemException)
        {
            CosNaming::NamingContext_var nc = ni.name_context();

            if (!force)
            {
                CosNaming::Name_var name;

                try
                {
                    name = NameParser::to_name(s);
                }
                catch (const InvalidNameException&)
                {
                    throw CosNaming::NamingContext::InvalidName();
                }

                m_context->bind_context(name.in(), nc.in());
            }
            else
                rebind_context (s, nc.in(), true);
        }


        void
        NamingInterface::rebind_context(const std::string& s,
                                        CosNaming::NamingContext_ptr nc,
                                        bool force)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException)
        {

            using namespace CosNaming;
            Name_var name;

            try
            {
                name = NameParser::to_name(s);
            }
            catch (const InvalidNameException&)
            {
                throw CosNaming::NamingContext::InvalidName();
            }

            if (!force)
                m_context->rebind_context(name.in(), nc);
            else
            {
                try
                {
                    m_context->rebind_context(name.in(), nc);
                }
                catch (const NamingContext::NotFound & ex)
                {
                    if (ex.why == NamingContext::missing_node)
                    {
                        // This may raise an exception if it fails

                        // Recreate whole path and redo binding
                        // Get basename (parent context)

                        // REM: lg__ > 1 otherwise rebind_context would have succeeded
                        const CORBA::ULong lg__ = name->length() - 1;
                        Name name__(lg__, lg__, name->get_buffer(), false);
                        NamingContext_var nc__ = bind_new_context(name__, force); // force == true
                        CosNaming::Name n__(1);
                        n__.length(1);
                        n__[0] = name[lg__]; // last element
                        nc__ -> rebind_context(n__, nc);
                    }
                    else
                        throw;
                }
                catch (const NamingContext::CannotProceed &)
                {
                    throw;
                }
                catch (const NamingContext::InvalidName &)
                {
                    throw;
                }
                catch (const CORBA::SystemException &)
                {
                    throw;
                }
            }
        }


        void
        NamingInterface::rebind_context(const std::string& s,
                                        const NamingInterface& ni,
                                        bool force)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException)
        {
            CosNaming::NamingContext_var nc = ni.name_context();
            rebind_context(s, nc.in(), force);
        }





        CORBA::Object_ptr
        NamingInterface::resolve(const std::string& s)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException)
        {
            CosNaming::Name_var name;

            try
            {
                name = NameParser::to_name(s);
            }
            catch (const InvalidNameException&)
            {
                throw CosNaming::NamingContext::InvalidName();
            }

            return m_context->resolve(name.in());
        }

        void
        NamingInterface::unbind(const std::string& s)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException)
        {
            CosNaming::Name_var name;

            try
            {
                name = NameParser::to_name(s);
            }
            catch (const InvalidNameException&)
            {
                throw CosNaming::NamingContext::InvalidName();
            }

            m_context->unbind(name.in());
        }

        NamingInterface
        NamingInterface::new_context()
        throw(CORBA::SystemException)
        {
            CosNaming::NamingContext_var nc
            = m_context->new_context();
            return NamingInterface(nc.in());
        }

        NamingInterface
        NamingInterface::bind_new_context(const std::string& s, bool force)
        throw(CosNaming::NamingContext::NotFound,
              CosNaming::NamingContext::AlreadyBound,
              CosNaming::NamingContext::CannotProceed,
              CosNaming::NamingContext::InvalidName,
              CORBA::SystemException)
        {

            using namespace CosNaming;
            Name_var name;

            try
            {
                name = NameParser::to_name(s);
            }
            catch (const InvalidNameException&)
            {
                throw CosNaming::NamingContext::InvalidName();
            }

            NamingContext_var nc = bind_new_context(name.in(), force);

            return NamingInterface(nc.in());
        }

        void
        NamingInterface::destroy()
        throw(CosNaming::NamingContext::NotEmpty,
              CORBA::SystemException)
        {
            m_context->destroy();
        }

        bool
        NamingInterface::destroy_all()
        throw(CORBA::SystemException)
        {
            bool done = false;

            if (clear_context(m_context.in()))
                try
                {
                    m_context->destroy();
                    done = true;
                }
                catch (const CosNaming::NamingContext::NotEmpty &)
                {
                    // ignore
                }
                catch (const CORBA::SystemException &)
                {
                    throw;
                }

            return done;
        }

        void
        NamingInterface::list(CORBA::ULong how_many,
                              CosNaming::BindingList_out bl,
                              CosNaming::BindingIterator_out bi)
        throw(CORBA::SystemException)
        {
            m_context->list(how_many, bl, bi);
        }

        //
        // Additional local operations from CosNaming::NamingContextExt
        // interface.
        //
        std::string NamingInterface::to_string(const CosNaming::Name& name)
        throw(InvalidNameException)
        {
            return NameParser::to_string(name);
        }

        CosNaming::Name* NamingInterface::to_name(const std::string& s)
        throw(InvalidNameException)
        {
            return NameParser::to_name(s);
        }


        std::string NamingInterface::basename(const std::string& s)
        throw(InvalidNameException)
        {
            CosNaming::Name_var name = to_name(s);

            const CORBA::ULong dirname_lg = name->length() - 1;

            CosNaming::Name basename(1);
            basename.length(1);
            basename[0] = name[dirname_lg];

            return to_string(basename);
        }

        std::string NamingInterface::dirname(const std::string& s)
        throw(InvalidNameException)
        {
            CosNaming::Name_var name = to_name(s);

            const CORBA::ULong dirname_lg = name->length() - 1;
            std::string the_dirname("");

            if (dirname_lg != 0)
            {
                CosNaming::Name dirname(dirname_lg, dirname_lg, name->get_buffer(), false);
                the_dirname = to_string(dirname);
            }

            return the_dirname;
        }

        std::string NamingInterface::addEscape(const std::string& s)
	{
	  return NameParser::escape_field(s.c_str());
	}


    } // End namespace Naming

} // End namespace CommonSvcs
} // End namespace Cdmw


