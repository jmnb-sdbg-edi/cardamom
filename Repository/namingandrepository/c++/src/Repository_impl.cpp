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
#include "Foundation/common/Assert.hpp"
#include "namingandrepository/Repository_impl.hpp"
#include "namingandrepository/RootNamingContext_impl.hpp"
#include "namingandrepository/NameDomain_impl.hpp"
#include "namingandrepository/FactoryFinder_impl.hpp"
#include "namingandrepository/ORBFacility.hpp"
#include "namingandrepository/NamingUtilities.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"

#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{


CosNaming::NamingContext_ptr Repository_impl::resolve_root_context(const char* root_id)
        throw(CdmwNamingAndRepository::Repository::NoRootContext,
              CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, "", "Repository_impl", "resolve_root_context", "in", "root_id", root_id);

    try
    {

        try
        {

            std::string rootContextId = NamingContext_impl::ROOT_CONTEXT_PREFIX;
            rootContextId += root_id;

            CosNaming::NamingContext_ptr result = NamingContext_impl::findById(rootContextId);
            
            if (CORBA::is_nil(result))
                throw CdmwNamingAndRepository::Repository::NoRootContext();


            CDMW_TRACE_EXIT(CDMW_NREP, "", "Repository_impl", "resolve_root_context");

            return result;

        }
        catch(const std::bad_alloc &)
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

        CDMW_TRACE_EXCEPTION(CDMW_NREP, "", "Repository_impl", "resolve_root_context", WRN, e._name());

        throw;

    }

}



CdmwNamingAndRepository::NameDomain_ptr Repository_impl::resolve_name_domain(const char* domain_name)
        throw(CdmwNamingAndRepository::NoNameDomain,
              CdmwNamingAndRepository::InvalidName,
              CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, "", "Repository_impl", "resolve_name_domain", "in", "domain_name", domain_name);

    try
    {


        try
        {
            CdmwNamingAndRepository::NameDomain_var nameDomain;

            CosNaming::Name_var name = Cdmw::NamingAndRepository::NamingInterface::to_name(domain_name);

            // get the root name domain corresponding to the first name component
            std::string firstPartStr = NamingUtilities::stringifiedNameFirstPart(name.in());

            CdmwNamingAndRepository::NameDomain_var rootNameDomain =
                NameDomain_impl::findRootNameDomain(m_defaultRootContext.in(), firstPartStr);

            if (name->length() > 1)
            {
                // propagates the resolution
                std::string endPartStr = NamingUtilities::stringifiedNameEndPart(name.in());

                nameDomain = rootNameDomain->resolve_name_domain(endPartStr.c_str());
            }
            else
            {
                nameDomain = rootNameDomain._retn();
            }

            CDMW_TRACE_EXIT(CDMW_NREP, "", "Repository_impl", "resolve_name_domain");

            return nameDomain._retn();

        }
        catch(const Cdmw::NamingAndRepository::InvalidNameException &)
        {
            // invalid name
            throw CdmwNamingAndRepository::InvalidName();
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
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, "", "Repository_impl", "resolve_name_domain", WRN, e._name());

        throw;

    }

}


bool Repository_impl::start(CORBA::ORB_ptr orb)
{

    bool result = false;

    CDMW_ASSERT(!CORBA::is_nil(orb));

    PortableServer::POA_var rootPOA;

    try
    {
        // Initialize ORBFacility
        ORBFacility::initialize(orb);

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        rootPOA = PortableServer::POA::_narrow(obj.in());

        // Retrieve the default root context 
        CosNaming::NamingContext_var root_ctx = resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
        m_defaultRootContext = CosNaming::NamingContextExt::_narrow(root_ctx.in());

        result = true;

    }
    catch(const CdmwNamingAndRepository::Repository::NoRootContext &)
    {
        // the default root context doesn't exist
        try
        {
            // Create the default root context
            m_defaultRootContext = 
                RootNamingContext_impl::createContext(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
            result = true;

        }
        catch(...)
        {
    
        }
    
    }
    catch(const CORBA::Exception &)
    {
        // Error

    }

    if (result)
    {
        result = false;

        try
        {

            FactoryFinder_impl *facFinder_i = new FactoryFinder_impl(m_defaultRootContext.in());

            PortableServer::ObjectId_var oid = rootPOA->activate_object(facFinder_i);
            facFinder_i->_remove_ref(); // drop ref count due to previous call

            CdmwLifeCycle::FactoryFinder_var facFinder = facFinder_i->_this();

            CosNaming::Name_var name = Cdmw::NamingAndRepository::NamingInterface::to_name(
                CdmwNamingAndRepository::FACTORY_FINDER);

            m_defaultRootContext->rebind(name.in(), facFinder.in());

            result = true;

        }
        catch(...)
        {

        }

    }
    
    return result;

}


} // End of namespace NamingAndRepository
} // End of namespace Cdmw

