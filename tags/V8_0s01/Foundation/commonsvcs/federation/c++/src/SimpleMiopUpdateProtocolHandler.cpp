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
#include <uuid/uuid.h>
#include <sys/time.h>

#include <iostream>

#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/commonsvcs/federation/SimpleMiopUpdateProtocolHandler.hpp"
#include "Foundation/commonsvcs/federation/LocalTopicUpdateManager.hpp"
#include "Foundation/commonsvcs/federation/MiopTopicUpdateManager_impl.hpp"
#include "TopicUpdateManagerC.h"

bool operator==(const ::Cdmw::CommonSvcs::TimeStamp& ts1,
                const ::Cdmw::CommonSvcs::TimeStamp& ts2)
{
    return (ts1.sec == ts2.sec) && (ts1.usec == ts2.usec);
}

bool operator>(const ::Cdmw::CommonSvcs::TimeStamp& ts1,
               const ::Cdmw::CommonSvcs::TimeStamp& ts2)
{
    return (ts1.sec > ts2.sec)
        || ((ts1.sec == ts2.sec) && (ts1.usec > ts2.usec));
}

bool operator>=(const ::Cdmw::CommonSvcs::TimeStamp& ts1,
               const ::Cdmw::CommonSvcs::TimeStamp& ts2)
{
    return (ts1 > ts2) || (ts1 == ts2);
}

Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler::SimpleMiopUpdateProtocolHandler
(CORBA::ORB_ptr           orb,
 PortableServer::POA_ptr  poa,
 std::string              corbaloc,
 size_t                   msg_count)
    : m_miop_servant(orb, *this),
      m_msg_count(msg_count)
{
    uuid_generate(m_uuid);

    // Register CORBA Object used for MCast communication
    CORBA::String_var ior = CORBA::string_dup(corbaloc.c_str());

    CORBA::Object_var group = orb->string_to_object(ior.in());

    PortableServer::ObjectId_var id =
        poa->create_id_for_reference(group.in());

    poa->activate_object_with_id(id.in(), &m_miop_servant);

    m_topic_update_manager =
        Cdmw::CommonSvcs::MiopTopicUpdateManager::_unchecked_narrow(group.in());

}

Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler::~SimpleMiopUpdateProtocolHandler()
    throw()
{
    // No-Op
}

void
Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler::update_topic(long                topic_key,
                                                                const RcvPolicy&    rcv_policy,
                                                                const TopicEncoder& encoder)
{
    TopicMessage msg;
    memcpy(msg.source, m_uuid, sizeof(Cdmw::CommonSvcs::UUID));

    struct timeval tv;
    gettimeofday(&tv, 0);
    msg.ts.sec  = tv.tv_sec;
    msg.ts.usec = tv.tv_usec;

    msg.t.key        = topic_key;
    msg.t.rcv_policy = rcv_policy;
    encoder.encode(msg.t.data);

    for (size_t i = 0; i < m_msg_count; ++i)
        m_topic_update_manager->distribute(msg);
}

bool
Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler::is_new(const TopicMessage& msg)
    throw()
{
    bool retVal = true;
    ::Cdmw::OsSupport::MutexGuard guard(m_mutex);
    if (m_message_cache.find(msg.t.key) != m_message_cache.end())
    {
        int uuid_comparison =
            uuid_compare(m_message_cache[msg.t.key].source, msg.source);
                         
        if ((uuid_comparison == 0)
            &&
            (m_message_cache[msg.t.key].ts >= msg.ts))
        {
            retVal = false;
        }
        else if (m_message_cache[msg.t.key].ts >= msg.ts) {
            retVal = false;
        }
    }
    return retVal;
}

void
Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler::notify_topic_update
(const TopicMessage& msg)
{
    // If this protocol handler has received the message it sent, then skip message.
    if (uuid_compare(m_uuid, msg.source) == 0) return;

    // Check validity of message according to the specified reception policy
    switch (msg.t.rcv_policy)
    {
        case Cdmw::CommonSvcs::ACCEPT_ALL:
             Cdmw::CommonSvcs::LocalTopicUpdateManager::instance().notify_topic_update(msg.t);
             break;

        case Cdmw::CommonSvcs::ACCEPT_NEWER:
             if (this->is_new(msg))
             {
                 // -- Scoped Locking idiom
                 {
                     Cdmw::OsSupport::MutexGuard guard(m_mutex);
                     memcpy(m_message_cache[msg.t.key].source,
                            msg.source,
                            sizeof(Cdmw::CommonSvcs::UUID));
                     m_message_cache[msg.t.key].ts = msg.ts;
                 }
                 Cdmw::CommonSvcs::LocalTopicUpdateManager::instance().notify_topic_update(msg.t); 
             }
             break;

        default:
             // The reception policy specified is unknown: skip message, (invalid format).
             break;
    }
}                         
