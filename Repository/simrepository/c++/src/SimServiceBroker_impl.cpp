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


#include "simrepository/SimServiceBroker_impl.hpp"

namespace Cdmw
{
namespace Tools
{


SimServiceBroker_impl::SimServiceBroker_impl (PortableServer::POA_ptr parent,
                                              CosNaming::NamingContext_ptr root,
                                              CdmwSimulatedRepository::MessageLogger_ptr logger) 
    throw (CORBA::SystemException)
    : m_poa(PortableServer::POA::_duplicate(parent)),
      m_root(root),
      m_logger(CdmwSimulatedRepository::MessageLogger::_duplicate(logger))
{
}

SimServiceBroker_impl::~SimServiceBroker_impl() throw()
{
}
    
PortableServer::POA_ptr SimServiceBroker_impl::_default_POA()
{
    return PortableServer::POA::_duplicate(m_poa.in());
}

void
SimServiceBroker_impl::set_service(const char*       service_name,
                                   CORBA::Object_ptr object_ref)
    throw (CORBA::SystemException)
{
    try {
        m_root.bind(service_name,object_ref,true);
    } catch (...) {
        throw CORBA::INTERNAL();
    }
}
    
//
// IDL:thalesgroup.com/CdmwPlatformMngtService/ServiceBroker/get_service:1.0
//
CORBA::Object_ptr
SimServiceBroker_impl::get_service(const char* service_name)
    throw(CdmwPlatformMngtService::ServiceNotFound,
          CdmwPlatformMngtService::ServiceUnavailable,
          CORBA::SystemException)
{
    std::string header("ServiceBroker::get_service(");
    header += service_name;
    header += ")";
    
    CORBA::Object_var obj = CORBA::Object::_nil();
    try {
        obj = m_root.resolve(service_name);
    } catch (const CosNaming::NamingContext::NotFound & ) {
        m_logger->log(header.c_str(),"throw CdmwPlatformMngtService::ServiceNotFound");
        throw CdmwPlatformMngtService::ServiceNotFound();
    } catch (...) {
        m_logger->log(header.c_str(),"throw CORBA::INTERNAL");
        throw CORBA::INTERNAL();
    }

    m_logger->log(header.c_str(),"service found.");
    
    return obj._retn();
}


#ifdef CDMW_USE_FAULTTOLERANCE

void SimServiceBroker_impl::set_global_service(const char* service_name, CORBA::Object_ptr service_object)
    throw( CdmwPlatformMngtService::ServiceAlreadyExists,
           CORBA::SystemException )
{
}
#endif

} // namespace Tools
} // namespace Cdmw

