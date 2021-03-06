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
#include "Foundation/commonsvcs/federation/MiopTopicUpdateManager_impl.hpp"
#include "Foundation/commonsvcs/federation/UpdateProtocolHandler.hpp"



Cdmw::CommonSvcs::MiopTopicUpdateManager_impl::MiopTopicUpdateManager_impl
(CORBA::ORB_ptr orb,
 ::Cdmw::CommonSvcs::UpdateProtocolHandler& protocol_handler)
    
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_protocol_handler(protocol_handler) 
{
    // No-Op
}

Cdmw::CommonSvcs::MiopTopicUpdateManager_impl::~MiopTopicUpdateManager_impl()
    throw()
{
    // No-Op
}

void
Cdmw::CommonSvcs::MiopTopicUpdateManager_impl::distribute(const ::Cdmw::CommonSvcs::TopicMessage& msg)
    throw (CORBA::SystemException)
{
    m_protocol_handler.notify_topic_update(msg);
}

