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


#include "testrepositoryext/UserFactoryFinder_impl.hpp"


/**
* The criteria constants
*/
/*
static const char* LOCATION_CRITERIUM = "location";
static const char* LOCATION_LOCAL = "local";
static const char* LOCATION_GLOBAL = "global";

static const char* OBJECT_INTERFACE_CRITERIUM = "object interface";
static const char* FACTORY_NAME_CRITERIUM = "factory name";
*/

UserFactoryFinder_impl::UserFactoryFinder_impl(CosNaming::NamingContextExt_ptr context)
{

    m_context = CosNaming::NamingContextExt::_duplicate(context);

}


UserFactoryFinder_impl::~UserFactoryFinder_impl()
{

}


CosLifeCycle::Factories* UserFactoryFinder_impl::find_factories(const CosLifeCycle::Key& factory_key)
    throw(CosLifeCycle::NoFactory,
    CORBA::SystemException)
{

    CORBA::ULong nbFactories = 0;
    CosLifeCycle::Factories_var factories = new CosLifeCycle::Factories;
    factories->length(nbFactories);

    const CORBA::ULong CHUNK = 1000;        // should be enough to avoid BindingIterator
                                            // (just for unit tests)
    CosNaming::BindingIterator_var bi;
    CosNaming::BindingList_var bl;
    m_context->list(CHUNK,bl,bi);                  

    for (CORBA::ULong i = 0; i < bl->length(); ++i)
    {

        if (bl[i].binding_type == CosNaming::nobject)
        {

            try
            {
                CORBA::Object_var temp_factory = m_context->resolve(bl[i].binding_name);
                CdmwLifeCycle::FactoryBase_var factory = CdmwLifeCycle::FactoryBase::_narrow(temp_factory.in());

                if (!CORBA::is_nil(factory.in()))
                {
                    ++nbFactories;
                    factories->length(nbFactories);
                    
                    factories[nbFactories-1] = temp_factory;
                }

            }
            catch(...)
            {

            }
        }
    }

    if (factories->length() == 0)
    {
        throw CosLifeCycle::NoFactory();
    }

    return factories._retn();
    
}    
    
    

CdmwLifeCycle::FactoryBase_ptr UserFactoryFinder_impl::find_factory(const char* factory_key)
    throw(CosLifeCycle::NoFactory,
    CORBA::SystemException)
{

    CdmwLifeCycle::FactoryBase_var factory;

    try
    {

        CORBA::Object_var obj = m_context->resolve_str(factory_key);

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

    return factory._retn();

}


