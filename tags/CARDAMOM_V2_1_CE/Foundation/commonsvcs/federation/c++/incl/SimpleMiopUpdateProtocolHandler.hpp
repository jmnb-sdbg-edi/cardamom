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
#ifndef _CDMW_CommonSvcs_SIMPLE_MIOP_UPDATE_PROTOCOL_HANDLER_HPP_
#define _CDMW_CommonSvcs_SIMPLE_MIOP_UPDATE_PROTOCOL_HANDLER_HPP_

#include "Foundation/commonsvcs/federation/UpdateProtocolHandler.hpp"
#include "Foundation/commonsvcs/federation/MiopTopicUpdateManager_impl.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/osthreads/Mutex.hpp"

#include <string>
#include <map>

namespace Cdmw {
    namespace CommonSvcs {
        class SimpleMiopUpdateProtocolHandler;
    }
}

/**
 * This class implements a very simple update protocol which simple
 * sends an update <b>N</b> times, where N is configurable, and by
 * default is one.The re-transmissions are done back-to-back.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 */
class Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler
    : public virtual Cdmw::CommonSvcs::UpdateProtocolHandler 
{
public:
    /**
     * Create a <code>SimpleMiopUpdateProtocolHandler</code>.
     *
     * @param orb the ORB
     * @param poa the POA on which the MIOP-based
     *        CORBA Object used for distribution of topics
     *        should be activated.
     * @param corbaloc the corbaloc to be used for the MIOP object
     * @param msg_count the number of message that have to be sent
     *        of each update.
     */
    SimpleMiopUpdateProtocolHandler(CORBA::ORB_ptr           orb,
                                    PortableServer::POA_ptr  poa,
                                    std::string              corbaloc,
                                    size_t                   msg_count = 1);

    /**
     * Finalize a <code>SimpleMiopUpdateProtocolHandler</code> instance.
     */
    virtual ~SimpleMiopUpdateProtocolHandler() throw();

public:
    /**
     * Update a given topic.
     *
     * @param topic_key  the key of the topic to be updated.
     * @param rcv_policy the reception policy to be used when topic is received.
     * @param encoder    the encoder that will provide an encapsulation for the topic to be updated.
     */
    virtual
    void update_topic(long                                  topic_key,
                      const Cdmw::CommonSvcs::RcvPolicy&    rcv_policy,
                      const Cdmw::CommonSvcs::TopicEncoder& encoder);

    /**
     * Notify a topic update.
     *
     * @param message the message containing the update.
     */
    virtual
    void notify_topic_update(const Cdmw::CommonSvcs::TopicMessage& message);

private:
    // -- Forbid Copy Ctor and Assignment Operator
    SimpleMiopUpdateProtocolHandler(const SimpleMiopUpdateProtocolHandler&);
    SimpleMiopUpdateProtocolHandler& operator=(const SimpleMiopUpdateProtocolHandler&);
    
private:
    bool is_new(const Cdmw::CommonSvcs::TopicMessage& message) throw();
    
private:
    struct MessageSignature {
        ::Cdmw::CommonSvcs::UUID      source;
        ::Cdmw::CommonSvcs::TimeStamp ts;
    };
    
    typedef std::map<int, MessageSignature> MessageSignatureCache;

private:
    Cdmw::CommonSvcs::MiopTopicUpdateManager_var   m_topic_update_manager;
    MessageSignatureCache                          m_message_cache;
    Cdmw::CommonSvcs::UUID                         m_uuid;
    Cdmw::CommonSvcs::MiopTopicUpdateManager_impl  m_miop_servant;
    size_t                                         m_msg_count;
    ::Cdmw::OsSupport::Mutex                       m_mutex;
};

bool operator==(const ::Cdmw::CommonSvcs::TimeStamp& ts1,
                const ::Cdmw::CommonSvcs::TimeStamp& ts2);

bool operator>(const ::Cdmw::CommonSvcs::TimeStamp& ts1,
               const ::Cdmw::CommonSvcs::TimeStamp& ts2);

bool operator>=(const ::Cdmw::CommonSvcs::TimeStamp& ts1,
               const ::Cdmw::CommonSvcs::TimeStamp& ts2);


#endif /* _CDMW_CommonSvcs_SIMPLE_MIOP_UPDATE_PROTOCOL_HANDLER_HPP_ */
