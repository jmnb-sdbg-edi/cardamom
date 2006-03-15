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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "namingandrepository/BindingIterator_impl.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include <memory>

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

PortableServer::POA_ptr BindingIterator_impl::M_POA = NULL;


//
// IDL:omg.org/CosNaming/BindingIterator/next_one:1.0
//
CORBA::Boolean BindingIterator_impl::next_one(CosNaming::Binding_out b)
        throw(CORBA::SystemException)
{
    
    CORBA::Boolean ret = false;
    
    CDMW_MUTEX_GUARD(m_bindingList_mutex);

    try
    {
        CosNaming::Binding_var temp_b;

        if (m_bindingList->length() > 0)
        {
            CosNaming::BindingList_var rest_of_binding_list;

            temp_b = extract_binding(m_bindingList.in(), rest_of_binding_list);
            m_bindingList = rest_of_binding_list;
            ret = true;

        }
        else
        {
            // just for not returning an invalid pointer
            temp_b = new CosNaming::Binding;
        }

        // transfers ownership
        b = temp_b._retn();
        return ret;

    }
    catch(const std::bad_alloc &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }

}

//
// IDL:omg.org/CosNaming/BindingIterator/next_n:1.0
//
CORBA::Boolean BindingIterator_impl::next_n(CORBA::ULong how_many,
                      CosNaming::BindingList_out bl)
        throw(CORBA::SystemException)
{

    if (how_many == 0)
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAM, CORBA::COMPLETED_NO);
    }

    try
    {
        CORBA::Boolean ret = false;
        
        CosNaming::BindingList_var temp_bl;

        CDMW_MUTEX_GUARD(m_bindingList_mutex);

        if (m_bindingList->length() > 0)
        {
            CosNaming::BindingList_var rest_of_binding_list;

            temp_bl = split_binding_list(m_bindingList.in(), how_many, rest_of_binding_list);
            m_bindingList = rest_of_binding_list;

            ret = true;
        }
        else
        {
            // zero length binding list
            temp_bl = new CosNaming::BindingList;
        }

        // transfers ownership
        bl = temp_bl._retn();
        return ret;

    }
    catch(const std::bad_alloc &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }

}


//
// IDL:omg.org/CosNaming/BindingIterator/destroy:1.0
//
void BindingIterator_impl::destroy()
        throw(CORBA::SystemException)
{

    M_POA->deactivate_object(m_oid.in());

}



CosNaming::Binding* BindingIterator_impl::extract_binding(const CosNaming::BindingList& bl,
                        CosNaming::BindingList_out rest_of_binding_list)
            throw (BadParameterException, OutOfMemoryException)
{
    
    // the number of bindings in the initial binding list
    CORBA::ULong len = bl.length();

    if (len == 0)
    {
        CDMW_THROW2(BadParameterException, "bl", "empty");
    }

    try
    {

        // the rest of the binding list
        CosNaming::BindingList_var temp_rest_of_binding_list = new CosNaming::BindingList;

        std::auto_ptr<CosNaming::Binding> pBinding(new CosNaming::Binding(bl[0]));

        if (len > 1)
        {
            
            temp_rest_of_binding_list->length(len-1);

            // construct the rest of the binding list
            for (CORBA::ULong i = 1; i < len; ++i)
            {
                temp_rest_of_binding_list[i-1] = bl[i];
            }

        }

        // transfers ownership
        rest_of_binding_list = temp_rest_of_binding_list._retn();
        return pBinding.release();

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfMemoryException);
    }

}



CosNaming::BindingList* BindingIterator_impl::split_binding_list(const CosNaming::BindingList& bl,
                        CORBA::ULong how_many,
                        CosNaming::BindingList_out rest_of_binding_list)
    throw (OutOfMemoryException)
{

    try
    {

        // the binding list to return
        CosNaming::BindingList_var ret_bl = new CosNaming::BindingList;

        // the rest of the binding list
        CosNaming::BindingList_var temp_rest_of_binding_list = new CosNaming::BindingList;

        // the number of bindings in the initial binding list
        CORBA::ULong len = bl.length();

        if (how_many > 0)
        {

            // number of bindings to return
            CORBA::ULong nb_ret;

            if (len > how_many)
            {

                nb_ret = how_many;
                
                // number of the rest of bindings
                CORBA::ULong nb = len - how_many;
                temp_rest_of_binding_list->length(nb);

                // construct the rest of the binding list
                for (CORBA::ULong i = nb; i > 0; --i)
                {
                    temp_rest_of_binding_list[nb-i] = bl[len-i];
                }
            }
            else
            {
                nb_ret = len;
            }

            // construct the binding list to return
            ret_bl->length(nb_ret);

            for (CORBA::ULong i = 0; i < nb_ret; ++i)
            {
                ret_bl[i] = bl[i];
            }

        }
        else
        {
            // the returned binding list is not relevant

            *temp_rest_of_binding_list = bl;

        }

        // transfers ownership
        rest_of_binding_list = temp_rest_of_binding_list._retn();

        return ret_bl._retn();

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfMemoryException);
    }

}



BindingIterator_impl::BindingIterator_impl(const CosNaming::BindingList& bl)
{
    m_bindingList = new CosNaming::BindingList(bl);
}



BindingIterator_impl::~BindingIterator_impl()
{
}



void BindingIterator_impl::initialize(PortableServer::POA_ptr poa)
    throw (AssertionFailedException)
{

    if (!CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }

    M_POA = PortableServer::POA::_duplicate(poa);

}


CosNaming::BindingIterator_ptr
BindingIterator_impl::create(const CosNaming::BindingList& bl)
    throw (OutOfMemoryException, AssertionFailedException, InternalErrorException)
{

    if (CORBA::is_nil(M_POA))
    {
        CDMW_THROW(AssertionFailedException);
    }

    // create, activate the BindingIterator
    
    try
    {
        std::auto_ptr<BindingIterator_impl> pBi(new BindingIterator_impl(bl));

        BindingIterator_impl *bi_i = pBi.get();
        
        bi_i->m_oid = M_POA->activate_object(bi_i);
        bi_i->_remove_ref(); // drop ref count due to previous call
        
        // transfers ownership
        bi_i = pBi.release();
        return bi_i->_this();

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfMemoryException);
    }
    catch(const CORBA::SystemException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}



} // End of namespace NamingAndRepository
} // End of namespace Cdmw

