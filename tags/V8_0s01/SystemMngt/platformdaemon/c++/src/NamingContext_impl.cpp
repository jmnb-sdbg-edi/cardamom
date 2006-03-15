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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "platformdaemon/NamingContext_impl.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"

#include <iostream>

namespace Cdmw
{
namespace PlatformMngt
{



NamingContext_impl::NamingContext_impl(ServiceRegistration_impl* serviceRegistration)
    :m_serviceRegistration(serviceRegistration)
{

}


NamingContext_impl::~NamingContext_impl()
{
}



void NamingContext_impl::bind(const CosNaming::Name& n,
                  CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


void NamingContext_impl::rebind(const CosNaming::Name& n,
                    CORBA::Object_ptr obj)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    
}


void NamingContext_impl::bind_context(const CosNaming::Name& n,
                          CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    
}


void NamingContext_impl::rebind_context(const CosNaming::Name& n,
                            CosNaming::NamingContext_ptr nc)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    
}


CORBA::Object_ptr NamingContext_impl::resolve(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    CORBA::Object_ptr obj = CORBA::Object::_nil();
    obj = m_serviceRegistration->get_service(n[0].id);
    if (CORBA::is_nil(obj))
        throw CosNaming::NamingContext::NotFound();
    
    if (n.length() > 1)
    {
        CosNaming::Name name(n.length() - 1);
        name.length(n.length() - 1);
        
        for (unsigned int i = 1; i< n.length(); i++)
        {
            name[i-1] = n[i];
        }
        
        CosNaming::NamingContextExt_var nc = CosNaming::NamingContextExt::_narrow(obj);
        if (CORBA::is_nil(obj))
        {
            throw CosNaming::NamingContext::CannotProceed();
        }
        
        obj = nc->resolve(name);
    }
    
    return obj;
}
    


void NamingContext_impl::unbind(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    
}


CosNaming::NamingContext_ptr NamingContext_impl::new_context()
    throw(CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    return CosNaming::NamingContext::_nil();
}


CosNaming::NamingContext_ptr NamingContext_impl::bind_new_context(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::AlreadyBound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    
    return CosNaming::NamingContext::_nil();
}


void NamingContext_impl::destroy()
    throw(CosNaming::NamingContext::NotEmpty,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    
}



void NamingContext_impl::list(CORBA::ULong how_many,
                  CosNaming::BindingList_out bl,
                  CosNaming::BindingIterator_out bi)
    throw(CORBA::SystemException)
{
//    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO); 
    ServiceRegistration_impl::ServiceMap service_map = m_serviceRegistration->get_known_services();

    CORBA::ULong nb_services = static_cast<CORBA::ULong>(service_map.size());
    CosNaming::BindingList_var complete_bl = new CosNaming::BindingList(nb_services);
    complete_bl->length(nb_services);

    CORBA::ULong i = 0;
    for (ServiceRegistration_impl::ServiceMap::const_iterator it = service_map.begin();
         it != service_map.end();
         ++it)
    {
        CosNaming::Binding binding;
        
        CosNaming::Name_var name = Cdmw::CommonSvcs::Naming::NamingInterface::to_name((*it).first);
        binding.binding_name = name.in();

        binding.binding_type = CosNaming::nobject;
        // Is it a Context
        try {
            CosNaming::NamingContext_var nc = CosNaming::NamingContext::_narrow((*it).second.in());
            if (!CORBA::is_nil(nc.in()))
                binding.binding_type = CosNaming::ncontext;
        } catch (...) {
            // ignore ...
        }
          
        complete_bl[i] = binding;
        
        ++i;
    }

    bl = complete_bl._retn();
    bi = CosNaming::BindingIterator::_nil();   
}


CosNaming::NamingContextExt::StringName NamingContext_impl::to_string(
    const CosNaming::Name& n)
    throw(CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    try
    {
        std::string str = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(n);
        return CORBA::string_dup(str.c_str());
    }
    catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &)
    {
        throw CosNaming::NamingContext::InvalidName();
    }
}


CosNaming::Name* NamingContext_impl::to_name(const char* sn)
    throw(CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    try
    {
        CosNaming::Name_var name = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(sn);
        return name._retn();
    }
    catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &)
    {
        throw CosNaming::NamingContext::InvalidName();
    }
}


CosNaming::NamingContextExt::URLString NamingContext_impl::to_url(
        const char* addr, const char* sn)
    throw(CosNaming::NamingContextExt::InvalidAddress,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);    

}


CORBA::Object_ptr NamingContext_impl::resolve_str(const char* sn)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

    CosNaming::Name_var name = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(sn);
    return resolve(name.in());

}
   


} // End of namespace PlatformMngt
} // End of namespace Cdmw

