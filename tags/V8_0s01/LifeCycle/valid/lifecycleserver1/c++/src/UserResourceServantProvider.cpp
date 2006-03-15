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

#include "lifecycleserver1/UserResourceServantProvider.hpp"
#include "lifecycleserver1/Device_impl.hpp"
#include "lifecycleserver1/Application_impl.hpp"
#include "Foundation/common/Assert.hpp"
#include "lifecycleserver1/MacroDefs.hpp"

#include <sstream>

namespace {
    const CF::ResourceNumType DEV = 1;
    const CF::ResourceNumType APP = 2;
    
    const char* DEV_PREFIX = "DEV ";
    const char* APP_PREFIX = "APP ";
    
    const char* DEV_REP_ID = "IDL:thalesgroup.com/CF/Device:1.0";
    const char* APP_REP_ID = "IDL:thalesgroup.com/CF/Application:1.0";
    
    // Properties Identifiers
    const char* LABEL_ID = "LABEL";
    const char* NAME_ID  = "NAME";
};

namespace Cdmw
{
namespace Resources
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/

UserResourceServantProvider::~UserResourceServantProvider()
    throw()
{
    // Delete all servants
    ActiveObjectMap::iterator info_pos = m_aom.begin();

    while (info_pos != m_aom.end())
    {
        PortableServer::Servant serv = (info_pos->second).m_serv;
        CDMW_ASSERT(serv != 0);
        serv->_remove_ref();
	info_pos++;
    }
}

// UserResourceServantProvider::UserResourceServantProvider()
//     throw()
//   : m_orb(CORBA::ORB::_nil()),
//     m_poa(PortableServer::POA::_nil())
// {
// }

UserResourceServantProvider::UserResourceServantProvider(CORBA::ORB_ptr          orb,
                                                         PortableServer::POA_ptr poa,
                                                         const Cdmw::LifeCycle::ObjectDeactivator & deactivator)
  throw(CORBA::SystemException)
  : m_orb(CORBA::ORB::_duplicate(orb)),
    m_poa(PortableServer::POA::_duplicate(poa)),
    m_object_deactivator(deactivator)
{
}

PortableServer::ObjectId * 
UserResourceServantProvider::create_resource(CF::ResourceNumType   resource_number,
                                             const CF::Properties& qualifiers)
    throw(CF::InvalidResourceNumber,
          CF::InvalidProperties,
          CF::CreationError)
{ 
  if (resource_number == DEV) {
      if ((qualifiers.length() == 1) &&
          (strcmp(qualifiers[0].id.in(),LABEL_ID) == 0)) {
          try {
              const char* label;
              qualifiers[0].value >>= label;
              PortableServer::ObjectId_var oid
                  = create_device(label);
              return oid._retn();
          } catch (const CORBA::DATA_CONVERSION & ) {
              CF::InvalidProperties ex;
              ex.error_messages.length(1);
              ex.error_messages[0] = CORBA::string_dup("Wrong label value.");
              throw  ex;
          } catch (const CF::Device::InvalidLocation & ) {
              throw  CF::CreationError();
          } 
      } 
      else {
          CF::InvalidProperties ex;
          ex.error_messages.length(1);
          ex.error_messages[0] = CORBA::string_dup("Label property is missing.");
          throw  ex;
      }
  } else if (resource_number == APP) {
      if ((qualifiers.length() == 1) &&
          (strcmp(qualifiers[0].id.in(),NAME_ID) == 0)) {
          try {
              const char* name;
              qualifiers[0].value >>= name;
              return create_application(name);
          } catch (const CORBA::DATA_CONVERSION & ) {
              CF::InvalidProperties ex;
              ex.error_messages.length(1);
              ex.error_messages[0] = CORBA::string_dup("Wrong name value.");
              throw  ex;
          } 
      }
      else {
          CF::InvalidProperties ex;
          ex.error_messages.length(1);
          ex.error_messages[0] = CORBA::string_dup("Name property is missing.");
          throw  ex;
      }
  } else
    throw CF::InvalidResourceNumber();
}

PortableServer::ObjectId * 
UserResourceServantProvider::create_device(const char* label)
    throw(CF::Device::InvalidLocation)
{
    try {
        std::string key(DEV_PREFIX);
        key += label;
        ActiveObjectMap::iterator serv_pos = m_aom.find(key);
        if (serv_pos  != m_aom.end())
            throw CF::Device::InvalidLocation();

        PortableServer::ObjectId_var oid
            = PortableServer::string_to_ObjectId(key.c_str());

        ObjInfo info(DEV,new Device_impl(m_poa.in(),m_object_deactivator,CF::Device::IDLE,label));
        m_aom[key] = info;
        ECHO_CDMW << "DEVICE <" << key << "> created with LABEL='" 
             << label << "'" << std::endl;
        return oid._retn();
    } catch (const CF::Device::InvalidLocation & ) {
        throw; // rethrow
    } catch (...) {
        throw CF::Device::InvalidLocation();
    }
}

PortableServer::ObjectId * 
UserResourceServantProvider::create_application(const char* name)
    throw(CF::CreateApplicationError)
{
    try {
        std::string key(APP_PREFIX);
        key += name;
        ActiveObjectMap::iterator serv_pos = m_aom.find(key);
        if (serv_pos  != m_aom.end())
            throw CF::CreateApplicationError();
        PortableServer::ObjectId_var oid
            = PortableServer::string_to_ObjectId(key.c_str());

        ObjInfo info(APP,new Application_impl(m_object_deactivator,name));
        m_aom[key] = info;
        ECHO_CDMW << "APPLICATION <" << key << "> created with NAME='" 
             << name << "'" << std::endl;
        return oid._retn();
    } catch (const CF::CreateApplicationError & ) {
        throw; // rethrow
    } catch (...) {
        throw CF::CreateApplicationError();
    }
}

PortableServer::Servant 
UserResourceServantProvider::lookup_servant(const PortableServer::ObjectId& oid) 
    throw(PortableServer::ForwardRequest,
          CORBA::SystemException)
{
    try  {
        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
        std::string key(s.in());
        ActiveObjectMap::iterator serv_pos = m_aom.find(key);
        if (serv_pos  == m_aom.end()) {
            throw CORBA::OBJECT_NOT_EXIST(); // TODO Minor codes
        }
        else {
            ECHO_CDMW << "Activating " << key << " ... " << std::endl;
            return m_aom[key].m_serv;
        }
    } catch (const CORBA::BAD_PARAM & ) { // Pb with ObjectId
            throw CORBA::OBJECT_NOT_EXIST(); // TODO Minor codes
    } catch (const std::bad_alloc & ) { // failed to allocate memory
            throw CORBA::OBJECT_NOT_EXIST(); // TODO Minor codes
    }
}

void 
UserResourceServantProvider::release_servant(const PortableServer::ObjectId& oid,
                                             PortableServer::POA_ptr         poa,
                                             PortableServer::Servant         servant) 
    throw(CORBA::SystemException)
{
    
    Cdmw::LifeCycle::LifeCycleObjectBase * removable
        = dynamic_cast<Cdmw::LifeCycle::LifeCycleObjectBase *>(servant);
    try {
        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
        std::string key(s.in());
        ECHO_CDMW << "Deactivating " << key << " ... ";
        if ((removable) && removable->is_removed(oid)) {
            cleanup_object(oid);
            m_aom.erase(key);            
            servant->_remove_ref();
            ECHO_CDMW << " deleted.";
        }
//         m_aom.erase(key);            
//         servant->_remove_ref();
        ECHO_CDMW << std::endl;
    } catch (const std::bad_alloc & ) { // string alloc pb
            // TODO throw an exception (no memory)
    } catch (const CORBA::BAD_PARAM & ) { // A problem with oid
        // TODO throw an exception
    }
}

std::string
UserResourceServantProvider::get_supported_interface(const PortableServer::ObjectId& oid) const
    throw(CORBA::OBJECT_NOT_EXIST)
{
    try  {
        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
        std::string name(s.in());
        ActiveObjectMap::const_iterator serv_pos = m_aom.find(name);
        if (serv_pos  == m_aom.end()) {
            throw CORBA::OBJECT_NOT_EXIST(); // TODO Minor codes
        }
        else {
            std::string result("");
            if ((*serv_pos).second.m_type == DEV)
                result = DEV_REP_ID;
            else
                result = APP_REP_ID;
            return result;
        }
    } catch (const std::bad_alloc & ) { // Failed to allocate memory
            throw CORBA::OBJECT_NOT_EXIST(); // TODO Minor codes
    } catch (const CORBA::BAD_PARAM & ) { // Pb with ObjectId
            throw CORBA::OBJECT_NOT_EXIST(); // TODO Minor codes
    }
}
UserResourceServantProvider::ObjInfo::ObjInfo()
    throw()
    : m_type(0), m_serv(0)
{
}

UserResourceServantProvider::ObjInfo::ObjInfo(CF::ResourceNumType     type,
                                              PortableServer::Servant serv)
    throw()
    : m_type(type), m_serv(serv)
{
}
 
UserResourceServantProvider::ObjInfo::ObjInfo(const UserResourceServantProvider::ObjInfo & oi)
    throw()
    : m_type(oi.m_type), m_serv(oi.m_serv)
{
}
 
UserResourceServantProvider::ObjInfo & 
UserResourceServantProvider::ObjInfo::operator=(const UserResourceServantProvider::ObjInfo & rhs)
    throw()
{
    if (this != &rhs) {
        m_type = rhs.m_type;
        m_serv = rhs.m_serv;
    }
    return *this;
}

        
}; // End namespace Resources
}; // End namespace Cdmw

