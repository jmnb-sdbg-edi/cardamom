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
#include "namingandrepository/FactoryFinder_impl.hpp"
#include "namingandrepository/NamingUtilities.hpp"
#include "namingandrepository/SystemFactoryFinder.hpp"
#include "namingandrepository/NameDomain_impl.hpp"
#include "namingandrepository/PersistentNameDomain.hpp"

#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"

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
* The criteria constants
*/
static const char* LOCATION_CRITERIUM = "location";
static const char* LOCATION_LOCAL = "local";
static const char* LOCATION_GLOBAL = "global";

static const char* OBJECT_INTERFACE_CRITERIUM = "object interface";
static const char* FACTORY_NAME_CRITERIUM = "factory name";


FactoryFinder_impl::FactoryFinder_impl(CosNaming::NamingContextExt_ptr defaultRootContext)
{

    m_defaultRootContext = CosNaming::NamingContextExt::_duplicate(defaultRootContext);

}


FactoryFinder_impl::~FactoryFinder_impl()
{

}


Criteria FactoryFinder_impl::analyze_key(const CosLifeCycle::Key& factory_key, CORBA::ULong& relevantDepth)
    throw(BadParameterException, OutOfResourcesException)
{

    try
    {
        Criteria criteria;

        CORBA::ULong criteriaSize = 0;
        
        CORBA::ULong len = factory_key.length();

        // analyze the key
        for (CORBA::ULong i = 0; i < len; ++i)
        {

            std::string id = factory_key[i].id.in();
            std::string kind = factory_key[i].kind.in();


            if (kind.compare(LOCATION_CRITERIUM) == 0)
            {
                if (id.compare(LOCATION_LOCAL) == 0)
                {
                    criteria.m_location = Local_Criterion;
                }
                else if (id.compare(LOCATION_GLOBAL) == 0)
                {
                    criteria.m_location = Global_Criterion;
                }
                else
                {
                    CDMW_THROW2(BadParameterException, "Location", "Invalid");
                }
                
                ++criteriaSize;

            }
            else if (kind.compare(OBJECT_INTERFACE_CRITERIUM) == 0)
            {
                if (id.empty())
                {
                    CDMW_THROW2(BadParameterException, "Object Interface", "Undefined");
                }

                criteria.m_selection = ObjectInterface_Criterion;
                criteria.m_selectionFilter = id;

                ++criteriaSize;

            }
            else if (kind.compare(FACTORY_NAME_CRITERIUM) == 0)
            {
                if (id.empty())
                {
                    CDMW_THROW2(BadParameterException, "Factory Name", "Undefined");
                }

                criteria.m_selection = FactoryName_Criterion;
                criteria.m_selectionFilter = id;

                ++criteriaSize;

            }
            else
            {
                // Not a criterion

                // check if a criterion has already been found
                if (criteriaSize != 0)
                {
                    // Criteria must be defined at the end of the key
                    CDMW_THROW2(BadParameterException, "factory_key", "Invalid");
                }

            }

        }

        relevantDepth = len - criteriaSize;

        return criteria;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


CosLifeCycle::Factories* FactoryFinder_impl::find_factories(const CosLifeCycle::Key& factory_key)
    throw(CosLifeCycle::NoFactory,
    CORBA::SystemException)
{
    CDMW_TRACE_ENTRY1(CDMW_NREP, "", "FactoryFinder_impl", "find_factories", "in", "factory_key", NamingUtilities::to_string(factory_key));

    try
    {

        try
        {


            CORBA::ULong relevantDepth = 0;
            Criteria criteria = analyze_key(factory_key, relevantDepth);
            
            std::string rootNameDomainName = NamingUtilities::stringifiedNameFirstPart(factory_key);

            std::string rootNameDomainId = NameDomain_impl::findRootNameDomainId(m_defaultRootContext.in(), rootNameDomainName);

            if (rootNameDomainId.empty())
            {
                throw CosLifeCycle::NoFactory();
            }

            // Delegates to a system factory finder
            CosLifeCycle::Key_var restOfKey = NamingUtilities::nameEndPart(factory_key);
            PersistentNameDomain *systemNameDomain = PersistentNameDomain::findById(rootNameDomainId);

            if (systemNameDomain == NULL)
            {
                CDMW_THROW(InternalErrorException);
            }

            SystemFactoryFinder sysFacFinder(systemNameDomain);

            CosLifeCycle::Factories_var factories =
                sysFacFinder.find_factories(restOfKey.in(), criteria, relevantDepth-1);

            if (factories->length() == 0)
            {
                throw CosLifeCycle::NoFactory();
            }

            CDMW_TRACE_EXIT(CDMW_NREP, "", "FactoryFinder_impl", "find_factories");

            return factories._retn();
        
        }
        catch(const std::bad_alloc &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const OutOfResourcesException &)
        {
            throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
        }
        catch(const BadParameterException &)
        {
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAM, CORBA::COMPLETED_NO);
        }
        catch(const InternalErrorException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, "", "FactoryFinder_impl", "find_factories", WRN, e._name());

        throw;

    }

}    
    
    

CdmwLifeCycle::FactoryBase_ptr FactoryFinder_impl::find_factory(const char* factory_key)
    throw(CosLifeCycle::NoFactory,
    CORBA::SystemException)
{

    CDMW_TRACE_ENTRY1(CDMW_NREP, "", "FactoryFinder_impl", "find_factory", "in", "factory_key", factory_key);

    try
    {

        CdmwLifeCycle::FactoryBase_var factory;

        try
        {

            CORBA::Object_var obj = m_defaultRootContext->resolve_str(factory_key);

            factory = CdmwLifeCycle::FactoryBase::_narrow(obj.in());

            if (CORBA::is_nil(factory.in()))
                throw CosLifeCycle::NoFactory();

        }
        catch(const CORBA::SystemException &)
        {
            // rethrow
            throw;
        }
        catch(const CORBA::UserException &)
        {
            throw CosLifeCycle::NoFactory();
        }

        CDMW_TRACE_EXIT(CDMW_NREP, "", "FactoryFinder_impl", "find_factory");

        return factory._retn();

    }
    catch(const CORBA::Exception& e)
    {

        CDMW_TRACE_EXCEPTION(CDMW_NREP, "", "FactoryFinder_impl", "find_factory", WRN, e._name());

        throw;

    }

}



} // End namespace NamingAndRepository
} // End namespace Cdmw

