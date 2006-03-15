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


#include <Foundation/common/Assert.hpp>

#include "MyComponentInstallation.hpp"

namespace TestCardamomCCM
{
    
MyComponentInstallation::MyComponentInstallation
(PortableServer::POA_ptr           poa,
 std::map<std::string,std::string> implems,
 std::map<std::string,std::string> valuetypefactory_locations)
    throw (CORBA::SystemException)
    : m_poa(PortableServer::POA::_duplicate(poa)),
      m_implementations(implems),
      m_valuetypefactory_locations(valuetypefactory_locations)
{
}

    
MyComponentInstallation::~MyComponentInstallation()
    throw ()
{
}
    
//
// IDL:omg.org/Components/Deployment/ComponentInstallation/install:1.0
//
void
MyComponentInstallation::install(const char* implUUID,
                                 const char* component_loc)
    throw(Components::Deployment::InvalidLocation,
          Components::Deployment::InstallationFailure,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT();
}

//
// IDL:omg.org/Components/Deployment/ComponentInstallation/replace:1.0
//
void
MyComponentInstallation::replace(const char* implUUID,
                                 const char* component_loc)
    throw(Components::Deployment::InvalidLocation,
          Components::Deployment::InstallationFailure,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT();
}

//
// IDL:omg.org/Components/Deployment/ComponentInstallation/remove:1.0
//
void
MyComponentInstallation::remove(const char* implUUID)
    throw(Components::Deployment::UnknownImplId,
          Components::RemoveFailure,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT();
}

//
// IDL:omg.org/Components/Deployment/ComponentInstallation/get_implementation:1.0
//
Components::Deployment::Location
MyComponentInstallation::get_implementation(const char* implUUID)
    throw(Components::Deployment::UnknownImplId,
          Components::Deployment::InstallationFailure,
          CORBA::SystemException)
{
    std::string uuid(implUUID);
    std::string implem("");
    std::map<std::string,std::string>::iterator i = m_implementations.find(uuid);
    if (i != m_implementations.end())
    {
        implem = i->second;
    } else
        throw Components::Deployment::UnknownImplId();
    Components::Deployment::Location_var location = CORBA::string_dup(implem.c_str());
    return location._retn();
}

//
// IDL:thalesgroup.com/CdmwDeployment/ComponentInstallation/get_valuetypefactory_location:1.0
//
Components::Deployment::Location
MyComponentInstallation::get_valuetypefactory_location(const char* implUUID,
                                                       const char* repid)
    throw(Components::Deployment::UnknownImplId,
          CdmwDeployment::UnknownValuetype,
          Components::Deployment::InstallationFailure,
          CORBA::SystemException)
{
    std::string type_id(repid);
    std::string loc("");
    std::map<std::string,std::string>::iterator i = m_valuetypefactory_locations.find(type_id);
    if (i != m_valuetypefactory_locations.end())
    {
        loc = i->second;
    } else
        throw CdmwDeployment::UnknownValuetype();
    Components::Deployment::Location_var location = CORBA::string_dup(loc.c_str());
    return location._retn();
}


}; // End namespace TestCardamomCCM
