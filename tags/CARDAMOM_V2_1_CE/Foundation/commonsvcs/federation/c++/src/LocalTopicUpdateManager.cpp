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
#include <assert.h>
#include <Foundation/commonsvcs/federation/LocalTopicUpdateManager.hpp>

::Cdmw::CommonSvcs::LocalTopicUpdateManager*
Cdmw::CommonSvcs::LocalTopicUpdateManager::M_instance = 0;


::Cdmw::CommonSvcs::LocalTopicUpdateManager::LocalTopicUpdateManager
(::Cdmw::CommonSvcs::UpdateProtocolHandler* handler)
    : m_update_protocol_handler(handler)
{
    // No-op
}

::Cdmw::CommonSvcs::LocalTopicUpdateManager::~LocalTopicUpdateManager()
    throw()
{
    // No-Op
}

void
::Cdmw::CommonSvcs::LocalTopicUpdateManager::init
(::Cdmw::CommonSvcs::UpdateProtocolHandler* handler)
{
    M_instance = new ::Cdmw::CommonSvcs::LocalTopicUpdateManager(handler); 
}

::Cdmw::CommonSvcs::LocalTopicUpdateManager&
::Cdmw::CommonSvcs::LocalTopicUpdateManager::instance()
    throw()
{
    // The init method has to be called as from the main.
    assert(M_instance != 0);
    return *M_instance;
}

void
::Cdmw::CommonSvcs::LocalTopicUpdateManager::set_topic_handler
(long topic_key,
 ::Cdmw::CommonSvcs::TopicUpdateHandler* handler)
    throw()
{
    m_topic_handler_map[topic_key] = handler;
}

void
::Cdmw::CommonSvcs::LocalTopicUpdateManager::update_topic
(long                                    topic_key,
 const ::Cdmw::CommonSvcs::TopicEncoder& encoder,
 const ::Cdmw::CommonSvcs::RcvPolicy&    rcv_policy)
{
    m_update_protocol_handler->update_topic(topic_key,
                                            rcv_policy,
                                            encoder);
}

void
::Cdmw::CommonSvcs::LocalTopicUpdateManager::notify_topic_update
(const ::Cdmw::CommonSvcs::Topic& topic)
{
    TopicHandlerMap_t::iterator iter = m_topic_handler_map.find(topic.key);
    if ( iter != m_topic_handler_map.end())
        iter->second->handle_update(topic.data);
}
