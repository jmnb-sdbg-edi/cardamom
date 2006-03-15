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


#ifndef INCL_PLATFORMMNGT_ELEMENTNAMESITERATOR_IMPL_HPP
#define INCL_PLATFORMMNGT_ELEMENTNAMESITERATOR_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementBase.skel.hpp"
#include "SystemMngt/platformlibrary/Iterator.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


/**
*Purpose:
*<p>
*Implementation of IDL:thalesgroup.com/CdmwPlatformMngt/ElementNamesIterator:1.0 interface.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see Iterator.
*/
class ElementNamesIterator_impl : virtual public POA_CdmwPlatformMngt::ElementNamesIterator,
        virtual public PortableServer::RefCountServantBase
{

public:

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ElementNamesIterator/reset:1.0
    * operation
    */
    virtual void reset()
        throw(CORBA::SystemException)
    {
        m_delegate.reset();
    }


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ElementNamesIterator/next_one:1.0
    * operation
    */
    virtual CORBA::Boolean next_one(CdmwPlatformMngt::ElementName_out element_name)
        throw(CORBA::SystemException)
    {
        return m_delegate.next_one(element_name);
    }


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ElementNamesIterator/next_n:1.0
    * operation
    */
    virtual CORBA::Boolean next_n(CORBA::ULong how_many,
                                  CdmwPlatformMngt::ElementNames_out element_names)
        throw(CORBA::SystemException)
    {
        return m_delegate.next_n(how_many, element_names);
    }


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ElementNamesIterator/destroy:1.0
    * operation
    */
    virtual void destroy()
        throw(CORBA::SystemException)
    {
        m_delegate.destroy();
    }


    /**
    * The destructor.
    */
    virtual ~ElementNamesIterator_impl() {}


public:

    /**
    * Purpose:
    * <p>
    * Creates the iterator servant, activates this servant, and returns the corresponding
    * CORBA object.
    *
    * @param elementList The list of elements referenced by the created iterator.
    * @return   The reference to the CORBA object
    */
    static CdmwPlatformMngt::ElementNamesIterator_ptr create(
        const std::list<CdmwPlatformMngt::ElementName_var> elementList)
        throw (OutOfMemoryException, InternalErrorException)
    {

        try
        {
            PortableServer::ServantBase_var servant = new ElementNamesIterator_impl(elementList);
            ElementNamesIterator_impl *servant_impl =
                dynamic_cast<ElementNamesIterator_impl*>(servant.in());

            CORBA::Object_var object =
                IteratorManager::createIterator(servant.in(), &(servant_impl->m_delegate));

            return CdmwPlatformMngt::ElementNamesIterator::_narrow(object.in());

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


private:

    /**
    * The actual implementation of the iterator.
    */
    Iterator<CdmwPlatformMngt::ElementName,
        CdmwPlatformMngt::ElementName_var,
        CdmwPlatformMngt::ElementName_out,
        CdmwPlatformMngt::ElementNames,
        CdmwPlatformMngt::ElementNames_var,
        CdmwPlatformMngt::ElementNames_out,
        StringElementAllocator<CdmwPlatformMngt::ElementName,
            CdmwPlatformMngt::ElementName_out>
        > m_delegate;

private:

    /**
    * Purpose:
    * <p>
    * Constructor.
    */
    ElementNamesIterator_impl(const std::list<CdmwPlatformMngt::ElementName_var> elementList)
        : m_delegate(elementList)
    {
    }



}; //End of ElementNamesIterator_impl

} // End of namespace PlatformMngt
} // End of namespace Cdmw

#endif  // INCL_PLATFORMMNGT_ELEMENTNAMESITERATOR_IMPL_HPP

