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


#include "simrepository/SimEntityObserver_impl.hpp"
#include <string>

namespace
{
    const char* ENTITY_FUNCTIONING_STRING        = "The entity is functioning correctly";
    const char* ENTITY_DYSFUNCTION_STRING        = "The entity has an applicative dysfunction";
    const char* ENTITY_FAILED_NO_RESPONSE_STRING = "The entity doesn't respond";
    const char* ENTITY_FAILED_DEATH_STRING       = "The entity is technically dead";

    std::string get_status_message(const char* entity_name,
                                   CdmwPlatformMngtEntity::EntityStatus entity_status,
                                   const char* entity_info)
    {
        std::string body("(");
        body += entity_name;
        body += ",";

        switch (entity_status) {
        case CdmwPlatformMngtEntity::ENTITY_FUNCTIONING:
            body += ENTITY_FUNCTIONING_STRING;
            break;
        case CdmwPlatformMngtEntity::ENTITY_DYSFUNCTION:
            body += ENTITY_DYSFUNCTION_STRING;
            break;
        case CdmwPlatformMngtEntity::ENTITY_FAILED_NO_RESPONSE:
            body += ENTITY_FAILED_NO_RESPONSE_STRING;
            break;
        case CdmwPlatformMngtEntity::ENTITY_FAILED_DEATH:
            body += ENTITY_FAILED_DEATH_STRING;
            break;
        };
        body += ",";
        body += entity_info;
        body += ")";
        return body;
    }
};

namespace Cdmw
{
namespace Tools
{


SimEntityObserver_impl::SimEntityObserver_impl (PortableServer::POA_ptr                     parent,
                                                CdmwSimulatedRepository::MessageLogger_ptr logger) 
    throw (CORBA::SystemException)
    : m_poa(PortableServer::POA::_duplicate(parent)),
      m_logger(CdmwSimulatedRepository::MessageLogger::_duplicate(logger))
{
}

SimEntityObserver_impl::~SimEntityObserver_impl() throw()
{
}
    
PortableServer::POA_ptr SimEntityObserver_impl::_default_POA()
{
    return PortableServer::POA::_duplicate(m_poa.in());
}

//
// IDL:thalesgroup.com/CdmwPlatformMngtEntity/SystemEntityObserver/set_system_entity_status:1.0
//
void
SimEntityObserver_impl::set_system_entity_status(const char* entity_name,
                                                 CdmwPlatformMngtEntity::EntityStatus entity_status,
                                                 const char* entity_info)
    throw(CdmwPlatformMngtEntity::EntityNotFound,
          CORBA::SystemException)
{
    std::string header("EntityObserver::set_system_entity_status");
    std::string body(get_status_message(entity_name,entity_status,entity_info));
    
    m_logger->log(header.c_str(),body.c_str());
}

//
// IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityObserver/set_application_entity_status:1.0
//
void
SimEntityObserver_impl::set_application_entity_status(const char* entity_name,
                                                      CdmwPlatformMngtEntity::EntityStatus entity_status,
                                                      const char* entity_info)
    throw(CdmwPlatformMngtEntity::EntityNotFound,
          CORBA::SystemException)
{
    std::string header("EntityObserver::set_application_entity_status");
    std::string body(get_status_message(entity_name,entity_status,entity_info));
    
    m_logger->log(header.c_str(),body.c_str());
}

//
// IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityObserver/set_process_entity_status:1.0
//
void
SimEntityObserver_impl::set_process_entity_status(const char* entity_name,
                                                  CdmwPlatformMngtEntity::EntityStatus entity_status,
                                                  const char* entity_info)
    throw(CdmwPlatformMngtEntity::EntityNotFound,
          CORBA::SystemException)
{   
    std::string header("EntityObserver::set_process_entity_status");
    std::string body(get_status_message(entity_name,entity_status,entity_info));
    
    m_logger->log(header.c_str(),body.c_str()); 
}

} // namespace Tools
} // namespace Cdmw

