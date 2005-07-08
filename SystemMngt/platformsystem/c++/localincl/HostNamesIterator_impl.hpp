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


#ifndef INCL_PLATFORMMNGT_HOSTNAMES_ITERATOR_IMPL_HPP
#define INCL_PLATFORMMNGT_HOSTNAMES_ITERATOR_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHostProxy.skel.hpp"
#include "SystemMngt/platformlibrary/Iterator.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


/**
*Purpose:
*<p>
*Implementation of IDL:thalesgroup.com/CdmwPlatformMngt/HostNamesIterator:1.0 interface.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see Iterator.
*/
class HostNamesIterator_impl : virtual public POA_CdmwPlatformMngt::HostNamesIterator,
                               virtual public PortableServer::RefCountServantBase
{

public:

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/HostNamesIterator/reset:1.0
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
    * IDL:thalesgroup.com/CdmwPlatformMngt/HostNamesIterator/next_one:1.0
    * operation
    */
    virtual CORBA::Boolean next_one(CdmwPlatformMngt::HostName_out host_name)
        throw(CORBA::SystemException)
    {
        return m_delegate.next_one(host_name);
    }


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/HostNamesIterator/next_n:1.0
    * operation
    */
    virtual CORBA::Boolean next_n(CORBA::ULong how_many,
                                  CdmwPlatformMngt::HostNames_out host_names)
        throw(CORBA::SystemException)
    {
        return m_delegate.next_n(how_many, host_names);
    }


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/HostNamesIterator/destroy:1.0
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
    virtual ~HostNamesIterator_impl() {}


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
    static CdmwPlatformMngt::HostNamesIterator_ptr create(
        const std::list<CdmwPlatformMngt::HostName_var> elementList)
        throw (OutOfMemoryException, InternalErrorException)
    {

        try
        {
            PortableServer::ServantBase_var servant = new HostNamesIterator_impl(elementList);
            HostNamesIterator_impl *servant_impl =
                dynamic_cast<HostNamesIterator_impl*>(servant.in());

            CORBA::Object_var object =
                IteratorManager::createIterator(servant.in(), &(servant_impl->m_delegate));

            return CdmwPlatformMngt::HostNamesIterator::_narrow(object.in());

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
    Iterator<CdmwPlatformMngt::HostName,
             CdmwPlatformMngt::HostName_var,
             CdmwPlatformMngt::HostName_out,
             CdmwPlatformMngt::HostNames,
             CdmwPlatformMngt::HostNames_var,
             CdmwPlatformMngt::HostNames_out,
             StringElementAllocator<CdmwPlatformMngt::HostName,
                                    CdmwPlatformMngt::HostName_out>
            > m_delegate;

private:

    /**
    * Purpose:
    * <p>
    * Constructor.
    */
    HostNamesIterator_impl(const std::list<CdmwPlatformMngt::HostName_var> elementList)
        : m_delegate(elementList)
    {
    }



}; //End of HostNamesIterator_impl

} // End of namespace PlatformMngt
} // End of namespace Cdmw


#endif // INCL_PLATFORMMNGT_HOSTNAMES_ITERATOR_IMPL_HPP

