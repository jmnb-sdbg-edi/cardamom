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


#ifndef INCL_PLATFORMMNGT_ELEMENTPATHSITERATOR_IMPL_HPP
#define INCL_PLATFORMMNGT_ELEMENTPATHSITERATOR_IMPL_HPP

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
*Implementation of IDL:thalesgroup.com/CdmwPlatformMngt/ElementPathsIterator:1.0 interface.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see Iterator.
*/
class ElementPathsIterator_impl : virtual public POA_CdmwPlatformMngt::ElementPathsIterator,
                                  virtual public PortableServer::RefCountServantBase
{

public:

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ElementPathsIterator/reset:1.0
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
    * IDL:thalesgroup.com/CdmwPlatformMngt/ElementPathsIterator/next_one:1.0
    * operation
    */
    virtual CORBA::Boolean next_one(CdmwPlatformMngt::ElementPath_out element_path)
        throw(CORBA::SystemException)
    {
        return m_delegate.next_one(element_path);
    }


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ElementPathsIterator/next_n:1.0
    * operation
    */
    virtual CORBA::Boolean next_n(CORBA::ULong how_many,
                                  CdmwPlatformMngt::ElementPaths_out element_paths)
        throw(CORBA::SystemException)
    {
        return m_delegate.next_n(how_many, element_paths);
    }


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ElementPathsIterator/destroy:1.0
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
    virtual ~ElementPathsIterator_impl() {}


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
    static CdmwPlatformMngt::ElementPathsIterator_ptr create(
        const std::list<CdmwPlatformMngt::ElementPath_var> elementList)
        throw (OutOfMemoryException, InternalErrorException)
    {

        try
        {
            PortableServer::ServantBase_var servant = new ElementPathsIterator_impl(elementList);
            ElementPathsIterator_impl *servant_impl =
                dynamic_cast<ElementPathsIterator_impl*>(servant.in());

            CORBA::Object_var object =
                IteratorManager::createIterator(servant.in(), &(servant_impl->m_delegate));

            return CdmwPlatformMngt::ElementPathsIterator::_narrow(object.in());

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
    Iterator<CdmwPlatformMngt::ElementPath,
        CdmwPlatformMngt::ElementPath_var,
        CdmwPlatformMngt::ElementPath_out,
        CdmwPlatformMngt::ElementPaths,
        CdmwPlatformMngt::ElementPaths_var,
        CdmwPlatformMngt::ElementPaths_out,
        StringElementAllocator<CdmwPlatformMngt::ElementPath,
            CdmwPlatformMngt::ElementPath_out>
        > m_delegate;

private:

    /**
    * Purpose:
    * <p>
    * Constructor.
    */
    ElementPathsIterator_impl(const std::list<CdmwPlatformMngt::ElementPath_var> elementList)
        : m_delegate(elementList)
    {
    }



}; //End of ElementPathsIterator_impl

} // End of namespace PlatformMngt
} // End of namespace Cdmw

#endif  // INCL_PLATFORMMNGT_ELEMENTPATHSITERATOR_IMPL_HPP

