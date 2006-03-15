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

#include <Foundation/orbsupport/OrbSupport.hpp>

#include "testcodec_lib/DecoderInit.hpp"
#include "testcodec_lib/DataTransfer_impl.hpp"

namespace Cdmw
{
namespace TestCodec
{ 


DecoderInit::DecoderInit ()
    throw(CORBA::SystemException)
{
}


DecoderInit::~DecoderInit()
    throw()
{
}
    
    
void DecoderInit::init (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
{
    //
    // Create and register DataTransfer
    //
    std::cout << "Create DataTransfer and bind it to a corbaloc" << std::endl;
    
    // create the DataTransfer servant
    Cdmw::TestCodec::DataTransfer_var dataTransfer;
    {
        Cdmw::TestCodec::DataTransfer_impl* pDataTransferServant = 
            new Cdmw::TestCodec::DataTransfer_impl(orb);
    
        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the block end)
        PortableServer::ServantBase_var servant_var = pDataTransferServant;
    
        // Get the root POA 
        CORBA::Object_var rootPOAobj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(rootPOAobj.in());

        // activate servant on POA (ref count is incremented)
        PortableServer::ObjectId_var oid = 
            rootPOA->activate_object(pDataTransferServant);
                       
        CORBA::Object_var object = rootPOA->id_to_reference(oid.in());
        dataTransfer = Cdmw::TestCodec::DataTransfer::_narrow(object.in());
    }
    
    OrbSupport::OrbSupport::bind_object_to_corbaloc(orb, "DataTransfer", dataTransfer.in());
    
}
  
}; // End namespace TestCodec
}; // End namespace Cdmw


