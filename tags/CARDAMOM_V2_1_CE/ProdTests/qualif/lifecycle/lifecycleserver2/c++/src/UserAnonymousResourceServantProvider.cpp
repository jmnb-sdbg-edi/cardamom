/* =========================================================================== *
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
 * =========================================================================== */

#include "lifecycleserver2/UserAnonymousResourceServantProvider.hpp"
#include "lifecycleserver2/Device_impl.hpp"
#include "lifecycleserver2/Application_impl.hpp"
#include "Foundation/common/Assert.hpp"
#include "lifecycleserver2/MacroDefs.hpp"

#include <sstream>

namespace {
    const CF::ResourceNumType DEV = 1;
    const CF::ResourceNumType APP = 2;
    
    const char* DEV_REP_ID = "IDL:thalesgroup.com/CF/Device:1.0";
    const char* APP_REP_ID = "IDL:thalesgroup.com/CF/Application:1.0";
    
    // Properties Identifiers
    const char* LABEL_ID = "LABEL";
    const char* NAME_ID  = "NAME";

};

namespace Cdmw
{
namespace Validation
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/

UserAnonymousResourceServantProvider::~UserAnonymousResourceServantProvider()
    throw()
{
}

// UserAnonymousResourceServantProvider::UserAnonymousResourceServantProvider()
//     throw()
//   : m_orb(CORBA::ORB::_nil()),
//     m_poa(PortableServer::POA::_nil())
// {
// }

UserAnonymousResourceServantProvider::UserAnonymousResourceServantProvider
(CORBA::ORB_ptr                              orb,
 PortableServer::POA_ptr                     poa,
 const Cdmw::LifeCycle::ObjectDeactivator & deactivator)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_object_deactivator(deactivator)
{
}

PortableServer::ObjectId * 
UserAnonymousResourceServantProvider::create_resource(CF::ResourceNumType   resource_number,
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
UserAnonymousResourceServantProvider::create_device(const char* label)
    throw(CF::Device::InvalidLocation)
{
    try {
        std::ostringstream ostr;
        ostr << "DEV" << " " << label;
        PortableServer::ObjectId_var oid
            = PortableServer::string_to_ObjectId(ostr.str().c_str());

        ECHO_CDMW << "DEVICE <" << ostr.str() << "> created with LABEL='" << label << "'" << std::endl;

        return oid._retn();
    } catch (...) {
        throw CF::Device::InvalidLocation();
    }
}

PortableServer::ObjectId * 
UserAnonymousResourceServantProvider::create_application(const char* name)
    throw(CF::CreateApplicationError)
{
    try {
        std::ostringstream oid_s;
        oid_s << "APP" << " " << name;
        PortableServer::ObjectId_var oid
            = PortableServer::string_to_ObjectId(oid_s.str().c_str());
        
        ECHO_CDMW << "APPLICATION <" << oid_s.str() << "> created with NAME='" 
             << name << "'" << std::endl;
        return oid._retn();
    } catch (...) {
        throw CF::CreateApplicationError();
    }
}

PortableServer::Servant 
UserAnonymousResourceServantProvider::lookup_servant(const PortableServer::ObjectId& oid) 
    throw(PortableServer::ForwardRequest,
          CORBA::SystemException)
{    
    try  {
        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
        std::istringstream istr(s.in());        
        
        std::string type;
        istr >> type;

        std::string info;
        istr >> info;

        PortableServer::Servant servant = 0;
        if (type == "DEV") {
            // create it
            servant = new Cdmw::Resources::Device_impl(m_poa.in(),
                                                        m_object_deactivator,
                                                        CF::Device::IDLE,
                                                        info.c_str());
        } else if (type == "APP") {
            // create it
            servant = new Cdmw::Resources::Application_impl(m_object_deactivator,info);
        } else {
            throw CORBA::OBJECT_NOT_EXIST(); // TODO Minor codes
        }
        ECHO_CDMW << "Activating " << s.in() << " ... " << std::endl;
        return servant;
            
    } catch (const CORBA::BAD_PARAM & ) { // Pb with ObjectId
        throw CORBA::OBJECT_NOT_EXIST(); // TODO Minor codes
    } catch (const std::bad_alloc & ) { // failed to allocate memory
        throw CORBA::OBJECT_NOT_EXIST(); // TODO Minor codes
    }
}

void 
UserAnonymousResourceServantProvider::release_servant(const PortableServer::ObjectId& oid,
                                             PortableServer::POA_ptr         poa,
                                             PortableServer::Servant         servant) 
    throw(CORBA::SystemException)
{
    
    try  {
        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
        servant->_remove_ref();
        ECHO_CDMW << "Deactivating " << s.in() << " ... " << std::endl;
            
    } catch (const CORBA::BAD_PARAM & ) { // Pb with ObjectId
        // TODO throw an exception
    } catch (const std::bad_alloc & ) { // failed to allocate memory
        // TODO throw an exception
    }
}

std::string
UserAnonymousResourceServantProvider::get_supported_interface(const PortableServer::ObjectId& oid) 
    const throw(CORBA::OBJECT_NOT_EXIST)
{
    try {
        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
        std::istringstream istr(s.in());        
        
        std::string type;
        istr >> type;

        std::string result("");
        if (type == "DEV") {
            result = DEV_REP_ID;
        } else if (type == "APP") {
            result = APP_REP_ID;
        } else {
            throw CORBA::OBJECT_NOT_EXIST(); // TODO Minor codes
        }
        return result;
            
    } catch (const CORBA::BAD_PARAM & ) { // Pb with ObjectId
        // TODO throw an exception
    } catch (const std::bad_alloc & ) { // failed to allocate memory
        // TODO throw an exception
    }
    return ""; // to silence the compiler
}

        
}; // End namespace Validation
}; // End namespace Cdmw

