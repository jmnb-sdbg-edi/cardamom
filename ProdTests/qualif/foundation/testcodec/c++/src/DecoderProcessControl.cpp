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

#include "testcodec/DecoderProcessControl.hpp"
#include "testcodec/DataTransfer_impl.hpp"

namespace Cdmw
{
namespace TestCodec
{ 


DecoderProcessControl::DecoderProcessControl (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb))
{
}


DecoderProcessControl::~DecoderProcessControl()
    throw()
{
}
    
    
    
// process initialisation called by platformmngt 
void DecoderProcessControl::on_initialise (
                const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
{
    //
    // Create and register DataTransfer
    //
    std::cout << "Create and Register DataTransfer into the Repository" << std::endl;
    
    // Get the naming and repository from Repository interface
    CdmwNamingAndRepository::Repository_var repository =
                              CdmwNamingAndRepository::Repository::_nil();  
    repository = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
      
    // create the DataTransfer servant
    Cdmw::TestCodec::DataTransfer_var dataTransfer;
    {
        Cdmw::TestCodec::DataTransfer_impl* pDataTransferServant = 
            new Cdmw::TestCodec::DataTransfer_impl(m_orb.in());
    
        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the block end)
        PortableServer::ServantBase_var servant_var = pDataTransferServant;
    
        // Get the root POA 
        CORBA::Object_var rootPOAobj = m_orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(rootPOAobj.in());

        // activate servant on POA (ref count is incremented)
        PortableServer::ObjectId_var oid = 
            rootPOA->activate_object(pDataTransferServant);
                       
        CORBA::Object_var object = rootPOA->id_to_reference(oid.in());
        dataTransfer = Cdmw::TestCodec::DataTransfer::_narrow(object.in());
    }
    
    // Get dom1 name domain from repository
    CdmwNamingAndRepository::NameDomain_var domain =
        repository->resolve_name_domain ("dom1");
         
         
    // Register the DataTransfer object in the dom1 name domain
    CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = 
        domain->new_name ("DataTransfer");
    domain->register_object (regId.in(), dataTransfer.in());
       
}
  
  
// process to run called by platformmngt    
void DecoderProcessControl::on_run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
          CORBA::SystemException)
{
    // do nothing
}


// process to stop called by platformmngt 
void DecoderProcessControl::on_stop()
    throw(CORBA::SystemException)
{
    // do nothing
}    
    

}; // End namespace TestCodec
}; // End namespace Cdmw


