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


#ifndef INCL_PLATFORMMNGT_APPLICATIONS_ITERATOR_IMPL_HPP
#define INCL_PLATFORMMNGT_APPLICATIONS_ITERATOR_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.skel.hpp"
#include "SystemMngt/platformlibrary/Iterator.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
*Purpose:
*<p>
*Implementation of IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationsIterator:1.0 interface.
*<p>
*Features:
*<p>
*Thread safe.
*
*@see Iterator.
*/
class ApplicationsIterator_impl : virtual public POA_CdmwPlatformMngt::ApplicationsIterator,
                                  virtual public PortableServer::RefCountServantBase
{
    
public:

    /**
    * The destructor.
    */
    virtual ~ApplicationsIterator_impl() {}

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationsIterator/reset:1.0
    * operation
    */
    virtual void reset()
        throw (CORBA::SystemException)
    {
        m_delegate.reset();
    }

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationsIterator/next_one:1.0
    * operation
    */
    virtual CORBA::Boolean next_one (CdmwPlatformMngt::Application_out application)        
            throw (CORBA::SystemException)
    {
        return m_delegate.next_one (application);
    }

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationsIterator/next_n:1.0
    * operation
    */
    virtual CORBA::Boolean next_n (CORBA::ULong how_many,        
                                   CdmwPlatformMngt::Applications_out applications)
            throw (CORBA::SystemException)
    {
        return m_delegate.next_n (how_many, applications);
    }

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationsIterator/destroy:1.0
    * operation
    */
    virtual void destroy()
        throw (CORBA::SystemException)
    {
        m_delegate.destroy();
    }


    /**
    * Purpose:
    * <p>
    * Creates the iterator servant, activates this servant, and returns the corresponding
    * CORBA object.
    *
    * @param elementList The list of elements referenced by the created iterator.
    * @return   The reference to the CORBA object
    */
    static CdmwPlatformMngt::ApplicationsIterator_ptr create (
                     const std::list <CdmwPlatformMngt::Application_var> elementList)
            throw (OutOfMemoryException, InternalErrorException)
    {

        try
        {
            PortableServer::ServantBase_var servant = new ApplicationsIterator_impl(elementList);
            ApplicationsIterator_impl *servant_impl =
                                  dynamic_cast<ApplicationsIterator_impl*>(servant.in());

            CORBA::Object_var object =
                IteratorManager::createIterator (servant.in(), &(servant_impl->m_delegate));

            return CdmwPlatformMngt::ApplicationsIterator::_narrow(object.in());
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
    Iterator <CdmwPlatformMngt::Application,
              CdmwPlatformMngt::Application_var,
              CdmwPlatformMngt::Application_out,
              CdmwPlatformMngt::Applications,
              CdmwPlatformMngt::Applications_var,
              CdmwPlatformMngt::Applications_out,
//               StructElementAllocator <CdmwPlatformMngt::Application,
//                                       CdmwPlatformMngt::Application_out>
              // TODO: H. SOUAMI
              InterfaceElementAllocator<CdmwPlatformMngt::Application,
                                        CdmwPlatformMngt::Application_out>
              > m_delegate;


    /**
    * Purpose:
    * <p>
    * Constructor.
    */
    ApplicationsIterator_impl (
        const std::list<CdmwPlatformMngt::Application_var> elementList)
            : m_delegate (elementList)
    {
    }

}; //End of ApplicationsIterator_impl

} // End of namespace PlatformMngt
} // End of namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATIONS_ITERATOR_IMPL_HPP

