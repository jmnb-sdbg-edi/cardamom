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


#include <Foundation/common/Assert.hpp>
#include <CCMContainer/idllib/CdmwCcmContainer.stub.hpp>
#include <CCMContainer/ccmcif/Context.hpp>

namespace Cdmw {

namespace CCM {

namespace CIF {
   
/*
 * This class is the base class for ccmcif context embedding specific CCM 
 * context.
 */
Context::Context(CdmwCcmContainer::CCM2Context_ptr ctx)
   : m_ccm_context(CdmwCcmContainer::CCM2Context::_duplicate(ctx)) 
{
   CDMW_ASSERT(!CORBA::is_nil(ctx));
}

Context::~Context() 
{
   m_ccm_context = CdmwCcmContainer::CCM2Context::_nil();
}

CORBA::Object_ptr 
Context::resolve_initial_references(const char* identifier)
   throw (CdmwCcmContainer::CCM2Context::InvalidName,
          CORBA::SystemException)
{
   return m_ccm_context->resolve_initial_references(identifier);
}

Components::CCMHome_ptr 
Context::get_CCM_home() throw(CORBA::SystemException)
{
   return m_ccm_context->get_CCM_home();
}
   
::CosEventChannelAdmin::EventChannel_ptr
Context::obtain_event_channel_for_emitter(::Components::PortDescription* port)
   throw (CORBA::SystemException)
{
   return m_ccm_context->obtain_event_channel_for_emitter(port);
}

::CosEventChannelAdmin::EventChannel_ptr
Context::create_event_channel_for_publisher(::Components::PortDescription* port)
   throw (CORBA::SystemException)
{
   return m_ccm_context->create_event_channel_for_publisher(port);
}

} // namespace CIF 
} // end namespace Cdmw 
} // end namespace CCM 

