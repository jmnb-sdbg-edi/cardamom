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


#include "LifeCycle/lifecycleinit/StandardObjectRegistration.hpp"
#include "Foundation/common/Assert.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"

namespace Cdmw
{
namespace LifeCycle
{
    
StandardObjectRegistration::StandardObjectRegistration(CdmwNamingAndRepository::NameDomain_ptr dom)
    throw (CORBA::SystemException)
  : m_domain(CdmwNamingAndRepository::NameDomain::_duplicate(dom))
{
}

    
StandardObjectRegistration::~StandardObjectRegistration()
    throw ()
{
}


bool
StandardObjectRegistration::bind(CORBA::Object_ptr object_ref,
                                 const char* object_name)
{
    bool result = false;
    using namespace Cdmw::NamingAndRepository;  
    try {
        if (!CORBA::is_nil(object_ref)) {
            m_domain->register_new_object(object_name, object_ref);
            result = true; 
        }
	// CdmwNamingAndRepository exceptions translated into CosNaming exceptions
    } catch (const InvalidNameException & ) {
        throw CosNaming::NamingContext::InvalidName();
    } catch (const CdmwNamingAndRepository::InvalidName & ) {
        throw CosNaming::NamingContext::InvalidName();
    } catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists & ) {
        throw CosNaming::NamingContext::AlreadyBound();
    }
    return result;
}

bool
StandardObjectRegistration::unbind(const char* object_name)
{
    bool result = false;
    using namespace Cdmw::NamingAndRepository;    
    try {
        m_domain->release_name(object_name);
        result = true;
	// CdmwNamingAndRepository exceptions translated into CosNaming exceptions
    } catch (const InvalidNameException & ) {
        throw CosNaming::NamingContext::InvalidName();
    } catch (const CdmwNamingAndRepository::NameDomain::NotRegistered & ) {
        throw CosNaming::NamingContext::NotFound();
    } catch (const CdmwNamingAndRepository::InvalidName & ) {
        throw CosNaming::NamingContext::InvalidName();
    }

    return result;
}

}; // End namespace Lifecycle
}; // End namespace Cdmw

