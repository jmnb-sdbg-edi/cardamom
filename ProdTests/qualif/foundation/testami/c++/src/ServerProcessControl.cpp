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


#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

#include "testami/ServerProcessControl.hpp"
#include "testami/Server_impl.hpp"

namespace Cdmw
{
namespace TestAMI
{ 


ServerProcessControl::ServerProcessControl (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb))
{
}


ServerProcessControl::~ServerProcessControl()
    throw()
{
}
    
    
    
// process initialisation called by platformmngt 
void ServerProcessControl::on_initialise (
        const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
{
    //
    // Create and register Server
    //
    std::cout << "Create and Register Server into the Repository" << std::endl;
    
    // Get the naming and repository from Repository interface
    CdmwNamingAndRepository::Repository_var repository =
                              CdmwNamingAndRepository::Repository::_nil();  
    repository = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
      
    // create the Server servant
    Cdmw::TestAMI::Server_var server;
    {
        Cdmw::TestAMI::Server_impl* pServerServant = 
            new Cdmw::TestAMI::Server_impl(m_orb.in());
    
        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the block end)
        PortableServer::ServantBase_var servant_var = pServerServant;
    
        // Get the root POA 
        CORBA::Object_var rootPOAobj = m_orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(rootPOAobj.in());

        // activate servant on POA (ref count is incremented)
        PortableServer::ObjectId_var oid = 
            rootPOA->activate_object(pServerServant);
                       
        CORBA::Object_var object = rootPOA->id_to_reference(oid.in());
        server = Cdmw::TestAMI::Server::_narrow(object.in());
    }
    
    // Get dom1 name domain from repository
    CdmwNamingAndRepository::NameDomain_var domain =
        repository->resolve_name_domain ("dom1");
         
         
    // Register the Server object in the dom1 name domain
    CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = 
        domain->new_name ("Server");
    domain->register_object (regId.in(), server.in());
       
}
  
  
// process to run called by platformmngt    
void ServerProcessControl::on_run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
          CORBA::SystemException)
{
    // do nothing
}


// process to stop called by platformmngt 
void ServerProcessControl::on_stop()
    throw(CORBA::SystemException)
{
    // do nothing
}    
    

}; // End namespace TestAMI
}; // End namespace Cdmw


