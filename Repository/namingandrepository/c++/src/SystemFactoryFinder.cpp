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
#include "namingandrepository/SystemFactoryFinder.hpp"
#include "LifeCycle/idllib//CdmwLifeCycle.stub.hpp"
#include "namingandrepository/NameDomainContext_impl.hpp"
#include "namingandrepository/NameDomain_impl.hpp"
#include "namingandrepository/PersistentNameDomain.hpp"
#include "namingandrepository/PersistentNamingContext.hpp"
#include "namingandrepository/NamingUtilities.hpp"
#include "namingandrepository/ORBFacility.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{

/**
*Implementation notes:
*The location criterion is not currently taken into account as
*distributed name domains are not yet implemented.
*<p>
*Portability issues: none
*<p>
*/


/**
*
*/
Criteria::Criteria()
    :m_location(Local_Criterion), m_selection(All_Criterion)
{

}



SystemFactoryFinder::SystemFactoryFinder(PersistentNameDomain *systemNameDomain)
    :m_systemNameDomain(systemNameDomain)
{

}


CosLifeCycle::Factories* SystemFactoryFinder::concatenate(const CosLifeCycle::Factories& f1,
    const CosLifeCycle::Factories& f2)
    throw (OutOfResourcesException)
{

    try
    {
        CORBA::ULong len1 = f1.length();
        CORBA::ULong len2 = f2.length();

        CosLifeCycle::Factories_var result = new CosLifeCycle::Factories(f1);

        if (len2 > 0)
        {

            result->length(len1+len2);

            // append the n2 name components
            for (CORBA::ULong i = 0; i < len2; ++i)
            {

                (*result)[len1+i] = f2[i];

            }

        }

        return result._retn();

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}




CosLifeCycle::Factories* SystemFactoryFinder::find_factories(const CosLifeCycle::Key& factory_key,
    const Criteria& criteria, CORBA::ULong relevantDepth)
    throw(OutOfResourcesException, InternalErrorException)
{
    try
    {
        CosLifeCycle::Factories_var factories;

        if (relevantDepth > 0)
        {
            // propagates the search to the sub name domain
            std::string subNameDomainName = NamingUtilities::stringifiedNameFirstPart(factory_key);

            bool isSystem;
            bool nameDomainFound = m_systemNameDomain->existsNameDomain(subNameDomainName, isSystem);

            if (!nameDomainFound)
            {
                throw CosLifeCycle::NoFactory();
            }

            CosLifeCycle::Key_var restOfKey = NamingUtilities::nameEndPart(factory_key);

            if (isSystem)
            {
                // propagates the search to the system sub name domain

                std::string nameDomainRef = m_systemNameDomain->findNameDomain(subNameDomainName);

                CORBA::Object_var nameDomain = ORBFacility::string_to_object(nameDomainRef);

                std::string nameDomainId = NameDomain_impl::getId(nameDomain.in());
 
                PersistentNameDomain *systemNameDomain = PersistentNameDomain::findById(nameDomainId);

                if (systemNameDomain == NULL)
                {
                    throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                }

                SystemFactoryFinder sysFacFinder(systemNameDomain);

                factories =
                    sysFacFinder.find_factories(restOfKey.in(), criteria, relevantDepth-1);

            }
            else
            {
                // propagates the search to the user-supplied factory finder

                std::string factoryFinderRef = m_systemNameDomain->findFactoryFinder(subNameDomainName);

                CORBA::Object_var temp_factoryFinder = ORBFacility::string_to_object(factoryFinderRef);

                CdmwLifeCycle::FactoryFinder_var factoryFinder = CdmwLifeCycle::FactoryFinder::_narrow(temp_factoryFinder.in());

                if (CORBA::is_nil(factoryFinder.in()))
                {
                    throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                }

                factories = factoryFinder->find_factories(restOfKey.in());

            }

        }
        else
        {
            // search within this name domain
            CosLifeCycle::Factories_var nameDomainFactories = find_factories(criteria);
            
            // search within the sub-name domains
            CosLifeCycle::Factories_var subNameDomainFactories = find_factories(factory_key, criteria);

            factories = concatenate(*nameDomainFactories, *subNameDomainFactories);

        }

        return factories._retn();
    
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const OutOfMemoryException &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const AssertionFailedException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}    


CosLifeCycle::Factories* SystemFactoryFinder::find_factories(const Criteria& criteria)
    throw(OutOfResourcesException, InternalErrorException) 
{

    // Search within the name domain attached to this factory finder

    try
    {

        // initialize the factories list
        CosLifeCycle::Factories_var factories = new CosLifeCycle::Factories;
        CORBA::ULong nbFactories = 0;
        factories->length(nbFactories);

        // get the persistent naming context corresponding to the factories context of the name domain
        std::string factoriesContextId = NameDomainContext_impl::getActualFactoriesContextId(m_systemNameDomain->id());
        PersistentNamingContext *factoriesContext = PersistentNamingContext::findById(factoriesContextId);

        if (factoriesContext == NULL)
        {
            CDMW_THROW(InternalErrorException);
        }

        // get the list of factory bindings
        PersistentNamingContext::DetailedBindingInfoList bindingInfolist = factoriesContext->detailedList();

        for (PersistentNamingContext::DetailedBindingInfoList::iterator it = bindingInfolist.begin();
            it != bindingInfolist.end(); ++it)
        {

            DetailedBindingInfo bindingInfo = *it;

            CORBA::Object_var temp_factory = ORBFacility::string_to_object(bindingInfo.m_objectRefString);
            CdmwLifeCycle::FactoryBase_var factory = CdmwLifeCycle::FactoryBase::_narrow(temp_factory.in());

            if (criteria.m_selection == FactoryName_Criterion)
            {
                if ((bindingInfo.m_bindingName).compare(criteria.m_selectionFilter) != 0)
                {
                    // the name doesn't match
                    continue;
                }
            }
            else if (criteria.m_selection == ObjectInterface_Criterion)
            {

                CORBA::Boolean supported = false;

                try
                {
                    supported = factory->supports(criteria.m_selectionFilter.c_str());
                }
                catch(const CORBA::SystemException &)
                {
                    // ignore cases of factory failure
                    // The operation do its best !!
                }

                if (!supported)
                {
                    // the factory doesn't support the requested object interface
                    continue;
                }
            }

            // add the factory
            ++nbFactories;
            factories->length(nbFactories);
            factories[nbFactories-1] = temp_factory;

            if (criteria.m_selection == FactoryName_Criterion)
            {
                // As a naming context doesn't allow duplicate names, the search
                // is finished for the first name that matches the requested
                // factory name.
                break;
            }

        }

        return factories._retn();

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const OutOfMemoryException &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const AssertionFailedException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}


CosLifeCycle::Factories* SystemFactoryFinder::find_factories(
    const CosLifeCycle::Key& factory_key, const Criteria& criteria)
    throw(OutOfResourcesException, InternalErrorException) 
{

    // Search within the sub name domain of the name domain attached to this factory finder

    try
    {

        CosLifeCycle::Factories_var factories = new CosLifeCycle::Factories;
        factories->length(0);

        std::list<std::string> systemNameDomains;
        std::list<std::string> userFactoryFinders;

        m_systemNameDomain->list(systemNameDomains, userFactoryFinders);

        // search the factories within each system sub name domains
        for (std::list<std::string>::iterator it_sysNameDomain = systemNameDomains.begin();
            it_sysNameDomain != systemNameDomains.end(); ++it_sysNameDomain)
        {

            CORBA::Object_var temp_sysNameDomain = ORBFacility::string_to_object(*it_sysNameDomain);
            CdmwNamingAndRepository::NameDomain_var sysNameDomain =
                CdmwNamingAndRepository::NameDomain::_narrow(temp_sysNameDomain.in());

            std::string sysNameDomainId = NameDomain_impl::getId(sysNameDomain.in());
            if (sysNameDomainId.empty())
            {
                CDMW_THROW(InternalErrorException);
            }

            PersistentNameDomain *systemNameDomain_peer = PersistentNameDomain::findById(sysNameDomainId);

            if (systemNameDomain_peer == NULL)
            {
                CDMW_THROW(InternalErrorException);
            }

            SystemFactoryFinder sysFacFinder(systemNameDomain_peer);

            CosLifeCycle::Factories_var found_factories =
                sysFacFinder.find_factories(factory_key, criteria, 0);

            CosLifeCycle::Factories_var temp_factories = concatenate(*factories, *found_factories);
            factories = temp_factories;

        }

        // search the factories within each user sub name domains
        // thanks to the associated factory finder
        for (std::list<std::string>::iterator it_userFactoryFinder = userFactoryFinders.begin();
            it_userFactoryFinder != userFactoryFinders.end(); ++it_userFactoryFinder)
        {

            CORBA::Object_var temp_factoryFinder = ORBFacility::string_to_object(*it_userFactoryFinder);
            CdmwLifeCycle::FactoryFinder_var factoryFinder =
                CdmwLifeCycle::FactoryFinder::_narrow(temp_factoryFinder.in());

            try
            {

                CosLifeCycle::Factories_var found_factories =
                    factoryFinder->find_factories(factory_key);

                CosLifeCycle::Factories_var temp_factories =
                    concatenate(*factories, *found_factories);
                factories = temp_factories;
            }
            catch(const CosLifeCycle::NoFactory &)
            {

            }
            catch(const CORBA::SystemException &)
            {
                // Ignored
                // The operation do its best !!
            }

        }

        return factories._retn();

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const OutOfMemoryException &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const AssertionFailedException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}

    
} // End namespace NamingAndRepository
} // End namespace Cdmw

