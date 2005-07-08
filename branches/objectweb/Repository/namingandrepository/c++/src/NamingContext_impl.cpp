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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "namingandrepository/NamingContext_impl.hpp"
#include "namingandrepository/PersistentNamingContext.hpp"
#include "namingandrepository/BindingIterator_impl.hpp"
#include "namingandrepository/NamingUtilities.hpp"
#include "namingandrepository/ORBFacility.hpp"

#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"

#include "Repository/naminginterface/NamingInterface.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Implementation notes : none
*<p>
*Portability issues :   none
*/

// Initialisation
PortableServer::POA_ptr NamingContext_impl::M_POA = NULL;

const int NamingContext_impl::PREFIX_LENGTH = 2;

const char* NamingContext_impl::ROOT_CONTEXT_PREFIX = "RC";
const char* NamingContext_impl::READ_WRITE_CONTEXT_PREFIX = "RW";
const char* NamingContext_impl::READ_ONLY_CONTEXT_PREFIX = "RO";


/*
* The following template classes are used to factorize
* code for the operations bind, bind_context, rebind and rebind_context
*/

template <class T> class Helper
{
public:

    static void call_bind(CosNaming::NamingContext_ptr namingContext,
            const CosNaming::Name& n,
            T obj)
    {

    }

    static void call_rebind(CosNaming::NamingContext_ptr namingContext,
            const CosNaming::Name& n,
            T obj)
    {

    }

    static void throw_exception(const CosNaming::Name& n)
    {

    }
};


template <> class Helper<CORBA::Object_ptr>
{
public:

    static void call_bind(CosNaming::NamingContext_ptr namingContext,
            const CosNaming::Name& n,
            CORBA::Object_ptr obj)
    {
        namingContext->bind(n, obj);
    }

    static void call_rebind(CosNaming::NamingContext_ptr namingContext,
            const CosNaming::Name& n,
            CORBA::Object_ptr obj)
    {
        namingContext->rebind(n, obj);
    }

    static void throw_exception(const CosNaming::Name& n)
    {

        CosNaming::NamingContext::NotFoundReason reason = 
                CosNaming::NamingContext::not_object;

        CosNaming::NamingContext::NotFound excep(reason, n);

        throw excep;

    }
};


template <> class Helper<CosNaming::NamingContext_ptr>
{
public:

    static void call_bind(CosNaming::NamingContext_ptr namingContext,
            const CosNaming::Name& n,
            CosNaming::NamingContext_ptr nc)
    {
        namingContext->bind_context(n, nc);
    }

    static void call_rebind(CosNaming::NamingContext_ptr namingContext,
            const CosNaming::Name& n,
            CosNaming::NamingContext_ptr nc)
    {
        namingContext->rebind_context(n, nc);
    }

    static void throw_exception(const CosNaming::Name& n)
    {

        CosNaming::NamingContext::NotFoundReason reason = 
                CosNaming::NamingContext::not_context;

        CosNaming::NamingContext::NotFound excep(reason, n);

        throw excep;

    }

};


/*
* The template class providing the actual implementation
* of bind, bind_context, rebind, rebind_context operations
*/
class NamingTemplates
{

public:

    /*
    * The actual implementation of bind and bind_context operations
    */
    template <class T> static void actual_bind(const CosNaming::Name& n, T obj,
        BindingType type, NamingContext_impl* thisContext)
    {

        CORBA::ULong len = n.length();

        if (len == 0)
        {
            // illegal name
            throw CosNaming::NamingContext::InvalidName();
        }

        try
        {

            if (len > 1)
            {
                // the name is a compound name

                // resolve the first name component
                CosNaming::NamingContext_var namingContext =
                    thisContext->resolveNamingContext(n);

                // propagate the operation
                CosNaming::Name_var endPart = NamingUtilities::nameEndPart(n);

                Helper<T>::call_bind(namingContext.in(), endPart.in(), obj);

            }
            else
            {
                // the name is a simple name

                // get the string form of the name
                std::string str_name = NamingUtilities::simple_name_to_string(n);
                
                if (thisContext->m_persistentPeer->existsBinding(str_name))
                {
                    // the name is already bound !!
                    throw CosNaming::NamingContext::AlreadyBound();
                }


                // get the stringified reference of the object
                std::string str_object = ORBFacility::object_to_string(obj);

                Binding binding(str_object, type);
                
                // add the binding
                thisContext->m_persistentPeer->addBinding(str_name, binding);

            }

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const AssertionFailedException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }

    }

    /*
    * The actual implementation of rebind and rebind_context operations
    */
    template <class T> static void actual_rebind(const CosNaming::Name& n, T obj,
        BindingType type, NamingContext_impl* thisContext)
    {

        CORBA::ULong len = n.length();

        if (len == 0)
        {
            // illegal name
            throw CosNaming::NamingContext::InvalidName();
        }

        try
        {

            if (len > 1)
            {
                // the name is a compound name

                // resolve the first name component
                CosNaming::NamingContext_var namingContext =
                    thisContext->resolveNamingContext(n);

                // propagate the operation
                CosNaming::Name_var endPart = NamingUtilities::nameEndPart(n);

                Helper<T>::call_rebind(namingContext.in(), endPart.in(), obj);

            }
            else
            {
                // the name is a simple name

                // get the string form of the name
                std::string str_name = NamingUtilities::simple_name_to_string(n);

                BindingType current_type;

                if (thisContext->m_persistentPeer->existsBinding(str_name, current_type))
                {
                    if (current_type != type)
                    {
                        // attempt to rebind on an existing binding
                        // of a different type

                        Helper<T>::throw_exception(n);

                    }
                }

                // get the stringified reference of the object
                std::string str_object = ORBFacility::object_to_string(obj);

                // update the binding
                thisContext->m_persistentPeer->updateBinding(str_name, str_object, type);

            }

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const AssertionFailedException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }

    }

};



/*
* The implementation of NamingContext_impl
*/

void NamingContext_impl::bind(const CosNaming::Name& n,
                  CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NamingContext_impl", "bind", "in", "n", NamingUtilities::to_string(n));

    try
    {

        NamingTemplates::actual_bind(n, obj, ObjectBinding, this);

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "bind", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "bind");

}


void NamingContext_impl::rebind(const CosNaming::Name& n,
                    CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NamingContext_impl", "rebind", "in", "n", NamingUtilities::to_string(n));

    try
    {

        NamingTemplates::actual_rebind(n, obj, ObjectBinding, this);

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "rebind", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "rebind");

}


void NamingContext_impl::bind_context(const CosNaming::Name& n,
                          CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NamingContext_impl", "bind_context", "in", "n", NamingUtilities::to_string(n));

    try
    {

        if (CORBA::is_nil(nc))
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);
        
        NamingTemplates::actual_bind(n, nc, NamingContextBinding, this);

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "bind_context", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "bind_context");

}


void NamingContext_impl::rebind_context(const CosNaming::Name& n,
                            CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NamingContext_impl", "rebind_context", "in", "n", NamingUtilities::to_string(n));

    try
    {

        if (CORBA::is_nil(nc))
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

        NamingTemplates::actual_rebind(n, nc, NamingContextBinding, this);

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "rebind_context", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "rebind_context");

}


CORBA::Object_ptr NamingContext_impl::resolve(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CORBA::Object_ptr result;

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NamingContext_impl", "resolve", "in", "n", NamingUtilities::to_string(n));

    try
    {

        CORBA::ULong len = n.length();

        if (len == 0)
        {
            // illegal name
            throw CosNaming::NamingContext::InvalidName();
        }

        try
        {
            if (len>1)
            {
                // the name is a compound name

                // resolve the first name component
                CosNaming::NamingContext_var namingContext = resolveNamingContext(n);

                // propagate the resolution
                CosNaming::Name_var endPart = NamingUtilities::nameEndPart(n);
                result = namingContext->resolve(endPart.in());

            }
            else
            {
                // the name is a simple name

                // resolve the unique name component
                result = resolveObject(n);

            }

        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
 
    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "resolve", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "resolve");

    return result;

}


void NamingContext_impl::unbind(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NamingContext_impl", "unbind", "in", "n", NamingUtilities::to_string(n));

    try
    {

        CORBA::ULong len = n.length();

        if (len == 0)
        {
            // illegal name
            throw CosNaming::NamingContext::InvalidName();
        }

        try
        {

            if (len > 1)
            {
                // the name is a compound name

                // resolve the first name component
                CosNaming::NamingContext_var namingContext = resolveNamingContext(n);

                // propagate the unbind
                CosNaming::Name_var endPart = NamingUtilities::nameEndPart(n);
                namingContext->unbind(endPart.in());

            }
            else
            {
                // the name is a simple name

                // get the string form of the name
                std::string str_name = NamingUtilities::simple_name_to_string(n);

                // remove the binding
                bool remove_performed = m_persistentPeer->removeBinding(str_name);

                if (!remove_performed)
                {
                    // not found due to missing node

                    CosNaming::NamingContext::NotFoundReason reason = 
                            CosNaming::NamingContext::missing_node;

                    CosNaming::NamingContext::NotFound excep(reason, n);

                    throw excep;

                }

            }

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }


    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "unbind", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "unbind");

}


CosNaming::NamingContext_ptr NamingContext_impl::new_context()
    throw(CORBA::SystemException)
{

    CDMW_TRACE_ENTRY(CDMW_NREP, m_id, "NamingContext_impl", "new_context");

    try
    {

        try
        {    
            std::string generatedId;
            CosNaming::NamingContextExt_ptr namingContext;

            // -- creates the naming context with the persistent peer --
            namingContext = NamingContext_impl::createWithSystemId(generatedId);

            CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "new_context");

            return namingContext;

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(...)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "new_context", WRN, e._name());

        throw;

    }


}


CosNaming::NamingContext_ptr NamingContext_impl::bind_new_context(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::AlreadyBound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY(CDMW_NREP, m_id, "NamingContext_impl", "bind_new_context");

    try
    {

        // -- creates the naming context --
        CosNaming::NamingContext_var namingContext = new_context();

        try
        {
            bind_context(n, namingContext.in());
        }
        catch(const CosNaming::NamingContext::NotFound &)
        {
            // clean up
            namingContext->destroy();
            // rethrow
            throw;
        }
        catch(const CosNaming::NamingContext::AlreadyBound &)
        {
            // clean up
            namingContext->destroy();
            // rethrow
            throw;
        }
        catch(const CosNaming::NamingContext::CannotProceed &)
        {
            // clean up
            namingContext->destroy();
            // rethrow
            throw;
        }
        catch(const CosNaming::NamingContext::InvalidName &)
        {
            // clean up
            namingContext->destroy();
            // rethrow
            throw;
        }
        catch(const CORBA::SystemException &)
        {
            // clean up
            namingContext->destroy();
            // rethrow
            throw;
        }

        CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "bind_new_context");

        return namingContext._retn();

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "bind_new_context", WRN, e._name());

        throw;

    }


}


void NamingContext_impl::destroy()
    throw(CosNaming::NamingContext::NotEmpty,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY(CDMW_NREP, m_id, "NamingContext_impl", "destroy");

    try
    {

        if (!m_persistentPeer->isEmpty())
        {
            throw CosNaming::NamingContext::NotEmpty();
        }

        m_destroyed = true;

        try
        {
            PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(m_id.c_str());
            M_POA->deactivate_object(oid.in());

        }
        catch(const CORBA::SystemException & )
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "destroy", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "destroy");

}



void NamingContext_impl::list(CORBA::ULong how_many,
                  CosNaming::BindingList_out bl,
                  CosNaming::BindingIterator_out bi)
    throw(CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NamingContext_impl", "list", "in", "how_many", how_many);

    try
    {

        try
        {
            // create the complete list of bindings
            CosNaming::BindingList_var complete_bl = new CosNaming::BindingList;

            PersistentNamingContext::BindingInfoList bindingInfolist = m_persistentPeer->list();

            size_t len = bindingInfolist.size();
            complete_bl->length(len);

            PersistentNamingContext::BindingInfoList::iterator it = bindingInfolist.begin();

            for (CORBA::ULong i = 0; i < len; ++i)
            {

                BindingInfo bindingInfo = *it;
                CosNaming::Binding binding;

                CosNaming::Name_var name = to_name(bindingInfo.m_bindingName.c_str());
                binding.binding_name = name.in();

                if (bindingInfo.m_bindingType == ObjectBinding)
                {
                    binding.binding_type = CosNaming::nobject;
                }
                else
                {
                    binding.binding_type = CosNaming::ncontext;
                }

                complete_bl[i] = binding;
            
                ++it;
            }


            // split the binding list

            CosNaming::BindingList_var temp_bl;
            CosNaming::BindingList_var rest_of_bl;
            temp_bl = BindingIterator_impl::split_binding_list(complete_bl.in(), how_many, rest_of_bl);

            // create the BindingIterator
            CosNaming::BindingIterator_var temp_bi = BindingIterator_impl::create(*rest_of_bl);

            // everything is OK
            bl = temp_bl._retn();
            bi = temp_bi._retn();
        
        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfMemoryException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const AssertionFailedException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "list", WRN, e._name());

        throw;

    }

    CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "list");

}


CosNaming::NamingContextExt::StringName NamingContext_impl::to_string(
    const CosNaming::Name& n)
    throw(CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NamingContext_impl", "to_string", "in", "n", NamingUtilities::to_string(n));

    try
    {

        try
        {
            std::string str;

            try
            {
                str = Cdmw::NamingAndRepository::NamingInterface::to_string(n);
            }
            catch(const Cdmw::NamingAndRepository::InvalidNameException &)
            {
                throw CosNaming::NamingContext::InvalidName();
            }

            char *result = CORBA::string_dup(str.c_str());

            CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "to_string");

            return result;

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "to_string", WRN, e._name());

        throw;

    }

}


CosNaming::Name* NamingContext_impl::to_name(const char* sn)
    throw(CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NamingContext_impl", "to_name", "in", "sn", sn);

    try
    {

        try
        {

            std::string str = sn;

            CosNaming::Name_var name_temp;

            try
            {
                name_temp = Cdmw::NamingAndRepository::NamingInterface::to_name(str);
            }
            catch(const Cdmw::NamingAndRepository::InvalidNameException &)
            {
                throw CosNaming::NamingContext::InvalidName();
            }


            CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "to_name");

            return name_temp._retn();

        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "to_name", WRN, e._name());

        throw;

    }

}


CosNaming::NamingContextExt::URLString NamingContext_impl::to_url(
        const char* addr, const char* sn)
    throw(CosNaming::NamingContextExt::InvalidAddress,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY2(CDMW_NREP, m_id, "NamingContext_impl", "to_url", "in", "addr", addr, "in", "sn", sn);

    try
    {

        CosNaming::NamingContextExt::URLString_var url;

        // TODO : NamingInterface will provide the to_url method
        throw CORBA::NO_IMPLEMENT(OrbSupport::NO_IMPLEMENTNotYetImplemented, CORBA::COMPLETED_NO);


        CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "to_url");

        return url._retn();

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "to_url", WRN, e._name());

        throw;

    }

}


CORBA::Object_ptr NamingContext_impl::resolve_str(const char* sn)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, m_id, "NamingContext_impl", "resolve_str", "in", "sn", sn);

    try
    {

        CosNaming::Name_var name = to_name(sn);

        CORBA::Object_ptr result = resolve(name.in());

        CDMW_TRACE_EXIT(CDMW_NREP, m_id, "NamingContext_impl", "resolve_str");

        return result;


    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, m_id, "NamingContext_impl", "resolve_str", WRN, e._name());

        throw;

    }

}



void NamingContext_impl::initialize(PortableServer::POA_ptr poa)
    throw (AssertionFailedException)
{

    if (!CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }

    M_POA = PortableServer::POA::_duplicate(poa);

}


PortableServer::POA_ptr NamingContext_impl::_default_POA()
{

    return PortableServer::POA::_duplicate(M_POA);

}



CORBA::Object_ptr NamingContext_impl::resolveObject(
    const CosNaming::Name& n)
    throw(CosNaming::NamingContext::InvalidName,
    CosNaming::NamingContext::NotFound,
    OutOfResourcesException, InternalErrorException)
{

    try
    {

        // get the string form of the name
        std::string str_name = NamingUtilities::simple_name_to_string(n);

        std::string strObjectRef = m_persistentPeer->findObject(str_name);

        if (strObjectRef.empty())
        {
            // not found due to missing node

            CosNaming::NamingContext::NotFoundReason reason = 
                    CosNaming::NamingContext::missing_node;

            CosNaming::NamingContext::NotFound excep(reason, n);

            throw excep;
        
        }

        // get the CORBA object from the stringified reference
        CORBA::Object_ptr obj = ORBFacility::string_to_object(strObjectRef);

        return obj;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const AssertionFailedException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}


CosNaming::NamingContext_ptr NamingContext_impl::resolveNamingContext(
    const CosNaming::Name& n)
    throw(CosNaming::NamingContext::InvalidName,
    CosNaming::NamingContext::NotFound,
    CosNaming::NamingContext::CannotProceed,
    OutOfResourcesException, InternalErrorException)
{

    try
    {
        // get the string form of the first name component
        CosNaming::Name_var n_firstPart = NamingUtilities::nameFirstPart(n);
        std::string str_name = NamingUtilities::simple_name_to_string(n_firstPart.in());

        bool isContext;
        std::string strObjectRef = m_persistentPeer->findNamingContext(str_name, isContext);

        if (strObjectRef.empty())
        {
            // The first part of the name cannot be found within this naming context 
            // => not found due to missing node

            CosNaming::NamingContext::NotFoundReason reason = 
                    CosNaming::NamingContext::missing_node;

            CosNaming::NamingContext::NotFound excep(reason, n);

            throw excep;
        
        }
        else if (!isContext)
        {
            // The first part doesn't correspond to a naming context 
            // not found due to not context

            CosNaming::NamingContext::NotFoundReason reason = 
                    CosNaming::NamingContext::not_context;

            CosNaming::NamingContext::NotFound excep(reason, n);

            throw excep;
        }

        // get the CORBA object from the stringified reference
        CORBA::Object_var obj = ORBFacility::string_to_object(strObjectRef);

        // narrowing
        try
	  {
	    CosNaming::NamingContext_ptr namingContext =
	      CosNaming::NamingContext::_narrow(obj.in());
        
	    if (CORBA::is_nil(namingContext))
	      CDMW_THROW(InternalErrorException);

	    return namingContext;
	  }
        catch(const CORBA::OBJECT_NOT_EXIST &)
	  {
            // Typically this will be raised if the context corresponding to strObjectRef
            // has been destroyed but has not been unbound in this parent
            CosNaming::NamingContext::CannotProceed excep(_this(), n);

	    throw excep;
	  }
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const AssertionFailedException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}


CosNaming::NamingContextExt_ptr
NamingContext_impl::create(PersistentNamingContext *persistentPeer)
    throw (OutOfResourcesException,
    AssertionFailedException, InternalErrorException)
{

    try
    {

        PortableServer::ServantBase_var servant = new NamingContext_impl(persistentPeer);

        NamingContext_impl *namingContext_i = dynamic_cast<NamingContext_impl *>(servant.in());

        CosNaming::NamingContextExt_var result =
            ORBFacility::createActivatedReference<CosNaming::NamingContextExt_ptr, NamingContext_impl>
            (persistentPeer->id(), namingContext_i);

        // transfers ownership
        return result._retn();

    }
    catch(const std::bad_alloc &)
    {
        // cleanup persistent data
        PersistentNamingContext::destroy(persistentPeer);
        
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const AssertionFailedException &)
    {
        // cleanup persistent data
        PersistentNamingContext::destroy(persistentPeer);
        
        // rethrow
        throw;
    }
    catch(const InternalErrorException &)
    {
        // cleanup persistent data
        PersistentNamingContext::destroy(persistentPeer);
        
        // rethrow
        throw;
    }

}


CosNaming::NamingContextExt_ptr
NamingContext_impl::createWithUserId(const std::string& id)
    throw (AlreadyExistsException, OutOfResourcesException,
    AssertionFailedException, InternalErrorException)
{

    PersistentNamingContext *persistentPeer = PersistentNamingContext::createWithId(id);
    
    CosNaming::NamingContextExt_ptr result = create(persistentPeer);
    
    return result;

}


CosNaming::NamingContextExt_ptr
NamingContext_impl::createWithSystemId(std::string& id)
    throw (OutOfResourcesException,
    AssertionFailedException, InternalErrorException)
{

    PersistentNamingContext *persistentPeer = 
            PersistentNamingContext::createWithPrefix(NamingContext_impl::READ_WRITE_CONTEXT_PREFIX);
        
    CosNaming::NamingContextExt_ptr result = create(persistentPeer);
    
    id = persistentPeer->id();
    
    return result;

}


bool NamingContext_impl::exists(const std::string& id)
{

    return PersistentNamingContext::exists(id);

}



CosNaming::NamingContextExt_ptr
NamingContext_impl::findById(const std::string& id)
    throw (AssertionFailedException, InternalErrorException)
{

    if (CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }

    CosNaming::NamingContextExt_ptr result = CosNaming::NamingContextExt::_nil();
    
    if (NamingContext_impl::exists(id))
    {

        try
        {
            PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(id.c_str());
            CORBA::RepositoryId_var repId = CORBA::string_dup("IDL:omg.org/CosNaming/NamingContextExt:1.0");

            CORBA::Object_var nc_temp = M_POA->create_reference_with_id(oid.in(), repId.in());

            result = CosNaming::NamingContextExt::_narrow(nc_temp.in());
        }
        catch(const CORBA::SystemException &)
        {
            CDMW_THROW(InternalErrorException);
        }

    }

    return result;

}



NamingContext_impl* NamingContext_impl::findServantById(const std::string& id)
    throw (NotFoundException, OutOfResourcesException, InternalErrorException)
{

    PersistentNamingContext* persistentPeer = PersistentNamingContext::findById(id);

    if (persistentPeer == NULL)
    {
        CDMW_THROW(NotFoundException);
    }

    try
    {
        NamingContext_impl *namingContext_i = new NamingContext_impl(persistentPeer);

        return namingContext_i;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


NamingContext_impl::NamingContext_impl(
    PersistentNamingContext *persistent_peer)
    : m_persistentPeer(persistent_peer)
{

    m_id = persistent_peer->id();
    m_destroyed = false;

}


NamingContext_impl::NamingContext_impl(const std::string& id)
    : m_id(id)
{

    m_persistentPeer = NULL;
    m_destroyed = false;

}


NamingContext_impl::~NamingContext_impl()
{

    if (m_persistentPeer != NULL)
    {
        if (m_destroyed)
        {
            // destroy the persistent data
            PersistentNamingContext::destroy(m_persistentPeer);
        }

        m_persistentPeer = NULL;
    
    }

}


} // End of namespace NamingAndRepository
} // End of namespace Cdmw

