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

#include "testftccmcif/ComponentInstallationStub.hpp"

namespace TestFtCcmCif
{


ComponentInstallationStub::ComponentInstallationStub(PortableServer::POA_ptr poa)
    : m_poa(PortableServer::POA::_duplicate(poa))
{}


PortableServer::POA_ptr 
ComponentInstallationStub::_default_POA() 
{
    return PortableServer::POA::_duplicate(m_poa.in());
}


//
// IDL:omg.org/Components/Deployment/ComponentInstallation/install:1.0
//
void 
ComponentInstallationStub::install(const char* implUUID,
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
ComponentInstallationStub::replace(const char* implUUID,
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
ComponentInstallationStub::remove(const char* implUUID)
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
ComponentInstallationStub::get_implementation(const char* implUUID)
    throw(Components::Deployment::UnknownImplId,
          Components::Deployment::InstallationFailure,
          CORBA::SystemException)
{
    Components::Deployment::Location_var location = CORBA::string_dup("");
    return location._retn();
}

//
// IDL:thalesgroup.com/CdmwDeployment/ComponentInstallation/get_valuetypefactory_location:1.0
//
Components::Deployment::Location 
ComponentInstallationStub::get_valuetypefactory_location(const char* implUUID,
                                                         const char* repid)
    throw(Components::Deployment::UnknownImplId,
          CdmwDeployment::UnknownValuetype,
          Components::Deployment::InstallationFailure,
          CORBA::SystemException)
{
    Components::Deployment::Location_var location = CORBA::string_dup("");
    return location._retn();
}

          
              
              
}  // end of namespace
             
