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


#include "namingandrepository/TestNamingContext_impl.hpp"
#include "namingandrepository/PersistentNamingContext.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include "namingandrepository/ORBFacility.hpp"

#include <iostream>


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Implementation notes : none
*<p>
*Portability issues :   none
*/


void TestNamingContext_impl::enterMethod(const std::string& method)
{

    CDMW_MUTEX_GUARD(m_nbThreads_mutex);
    m_nbThreads++;

    if (m_nbThreads > 1)
    {
        std::cout << "Entering " << method << " with " << m_nbThreads << " thread(s)" << std::endl;
    }

}


void TestNamingContext_impl::exitMethod(const std::string& method)
{

    CDMW_MUTEX_GUARD(m_nbThreads_mutex);
    m_nbThreads--;

}



void TestNamingContext_impl::bind(const CosNaming::Name& n,
                  CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{


    try
    {
        enterMethod("NamingContext_impl::bind");
        NamingContext_impl::bind(n, obj);
        exitMethod("NamingContext_impl::bind");
    }
    catch(const CORBA::Exception &)
    {
        // catch all CORBA exceptions
	
        exitMethod("NamingContext_impl::bind");
	
        // rethrow
        throw;
    }
	        

}


void TestNamingContext_impl::rebind(const CosNaming::Name& n,
                    CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    try
    {
        enterMethod("NamingContext_impl::rebind");
        NamingContext_impl::rebind(n, obj);
        exitMethod("NamingContext_impl::rebind");
    }
    catch(const CORBA::Exception &)
    {
        // catch all CORBA exceptions
	
        exitMethod("NamingContext_impl::rebind");
	
        // rethrow
        throw;
    }

}


void TestNamingContext_impl::bind_context(const CosNaming::Name& n,
                          CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

    try
    {
        enterMethod("NamingContext_impl::bind_context");
        NamingContext_impl::bind_context(n, nc);
        exitMethod("NamingContext_impl::bind_context");
    }
    catch(const CORBA::Exception &)
    {
        // catch all CORBA exceptions
	
        exitMethod("NamingContext_impl::bind_context");
	
        // rethrow
        throw;
    }

}


void TestNamingContext_impl::rebind_context(const CosNaming::Name& n,
                            CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    try
    {
        enterMethod("NamingContext_impl::rebind_context");
        NamingContext_impl::rebind_context(n, nc);
        exitMethod("NamingContext_impl::rebind_context");
    }
    catch(const CORBA::Exception &)
    {
        // catch all CORBA exceptions
	
        exitMethod("NamingContext_impl::rebind_context");
	
        // rethrow
        throw;
    }
	        
}


CORBA::Object_ptr TestNamingContext_impl::resolve(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CORBA::Object_var obj;

    try
    {
        enterMethod("NamingContext_impl::resolve");
        obj = NamingContext_impl::resolve(n);
        exitMethod("NamingContext_impl::resolve");
    }
    catch(const CORBA::Exception &)
    {
        // catch all CORBA exceptions
	
        exitMethod("NamingContext_impl::resolve");
	
        // rethrow
        throw;
    }
	        
    return obj._retn();

}


void TestNamingContext_impl::unbind(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    try
    {
        enterMethod("NamingContext_impl::unbind");
        NamingContext_impl::unbind(n);
        exitMethod("NamingContext_impl::unbind");
    }
    catch(const CORBA::Exception &)
    {
        // catch all CORBA exceptions
	
        exitMethod("NamingContext_impl::unbind");
	
        // rethrow
        throw;
    }
	        
}


CosNaming::NamingContext_ptr TestNamingContext_impl::new_context()
    throw(CORBA::SystemException)
{

    CosNaming::NamingContext_var nc;

    try
    {
        enterMethod("NamingContext_impl::new_context");
        nc = NamingContext_impl::new_context();
        exitMethod("NamingContext_impl::new_context");
    }
    catch(const CORBA::SystemException &)
    {
        // catch all CORBA exceptions
	
        exitMethod("NamingContext_impl::new_context");
	
        // rethrow
        throw;
    }
	        
    return nc._retn();

}


void TestNamingContext_impl::destroy()
    throw(CosNaming::NamingContext::NotEmpty,
          CORBA::SystemException)
{

    try
    {
        enterMethod("NamingContext_impl::destroy");
        NamingContext_impl::destroy();
        exitMethod("NamingContext_impl::destroy");
    }
    catch(const CORBA::SystemException &)
    {
        // catch all CORBA exceptions
	
        exitMethod("NamingContext_impl::destroy");
	
        // rethrow
        throw;
    }

}


void TestNamingContext_impl::list(CORBA::ULong how_many,
                  CosNaming::BindingList_out bl,
                  CosNaming::BindingIterator_out bi)
    throw(CORBA::SystemException)
{

    try
    {
        enterMethod("NamingContext_impl::list");
        NamingContext_impl::list(how_many, bl, bi);
        exitMethod("NamingContext_impl::list");
    }
    catch(const CORBA::SystemException &)
    {
        // catch all CORBA exceptions
	
        exitMethod("NamingContext_impl::list");
	
        // rethrow
        throw;
    }
	        

}


CosNaming::NamingContextExt_ptr
TestNamingContext_impl::create(const std::string& id)
    throw (AlreadyExistsException, OutOfResourcesException, AssertionFailedException, InternalErrorException)
{

    PersistentNamingContext *persistentPeer =
        PersistentNamingContext::createWithId(id);

    try
    {

        PortableServer::ServantBase_var servant = new TestNamingContext_impl(persistentPeer);

        TestNamingContext_impl *namingContext_i = dynamic_cast<TestNamingContext_impl *>(servant.in());

        CosNaming::NamingContextExt_var result =
            ORBFacility::createActivatedReference<CosNaming::NamingContextExt_ptr, TestNamingContext_impl>
            (persistentPeer->id(), namingContext_i);
    
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



TestNamingContext_impl::TestNamingContext_impl(
    PersistentNamingContext *persistent_peer)
    : NamingContext_impl(persistent_peer), m_nbThreads(0)
{

}



TestNamingContext_impl::~TestNamingContext_impl()
{


}


} // End of namespace NamingAndRepository
} // End of namespace Cdmw

