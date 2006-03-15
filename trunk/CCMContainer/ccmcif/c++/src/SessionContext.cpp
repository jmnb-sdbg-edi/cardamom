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


#include <CCMContainer/idllib/CdmwCcmContainer.stub.hpp>
#include <CCMContainer/ccmcif/SessionContext.hpp>
#include <CCMContainer/ccmcommon/CCMUtils.hpp>

namespace Cdmw {

namespace CCM {

namespace CIF {
   
/*
 * This class is the base class for ccmcif context embedding specific CCM 
 * context.
 */
SessionContext::SessionContext(CdmwCcmContainer::Session2Context_ptr ctx)
   : Context(ctx)
{
}
   
SessionContext::~SessionContext() 
{
   PRINT_INFO("SessionContext destructor called")
}

CORBA::Object_ptr 
SessionContext::create_ref(const std::string& rep_id) 
   throw(CORBA::SystemException)
{
   CdmwCcmContainer::Session2Context_var sc = get_session_context();
   return sc->create_ref(rep_id.c_str());
}

CORBA::Object_ptr 
SessionContext::create_ref_from_oid_str(const std::string& oid,
                                        const std::string& rep_id)   
   throw (CORBA::SystemException)
{
   CORBA::OctetSeq_var comp_oid 
      = Cdmw::CCM::Common::string_to_OctetSeq(oid.c_str());

   CdmwCcmContainer::Session2Context_var sc = get_session_context();

   return sc->create_ref_from_oid(comp_oid.in(), rep_id.c_str());
}

char* 
SessionContext::get_oid_str_from_ref(const CORBA::Object_ptr comp_ref)
   throw(CORBA::SystemException)
{
   CdmwCcmContainer::Session2Context_var sc = get_session_context();

   CORBA::OctetSeq_var comp_oid 
      = sc->get_oid_from_ref(comp_ref);

   return Cdmw::CCM::Common::OctetSeq_to_string(comp_oid.in());
}

Components::PrimaryKeyBase* 
SessionContext::get_primary_key() 
   throw(Components::NoKeyAvailable, CORBA::SystemException)
{
   // this does not exist with a session container but with an entity container.
   throw Components::NoKeyAvailable();
}

CORBA::Object_ptr 
SessionContext::get_CCM_object()
   throw (CORBA::SystemException)
{
   CdmwCcmContainer::Session2Context_var sc = get_session_context();
   return sc->get_CCM_object();
}

void 
SessionContext::req_passivate()
   throw (CORBA::SystemException)
{
   CdmwCcmContainer::Session2Context_var sc = get_session_context();
   sc->req_passivate();
}

   

} // namespace CIF 
} // end namespace Cdmw 
} // end namespace CCM 


