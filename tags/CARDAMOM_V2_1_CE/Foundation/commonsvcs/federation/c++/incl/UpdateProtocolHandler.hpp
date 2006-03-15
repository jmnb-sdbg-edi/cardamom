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
#ifndef _CDMW_CommonSvcs_UPDATE_PROTOCOL_HANDLER_HPP_
#define _CDMW_CommonSvcs_UPDATE_PROTOCOL_HANDLER_HPP_

#include "Foundation/commonsvcs/federation/TopicUpdateManager.stub.hpp"
#include "Foundation/commonsvcs/federation/TopicEncoder.hpp"

namespace Cdmw {
    namespace CommonSvcs {
        class UpdateProtocolHandler;
    }
}

/**
 * The <code>UpdateProtocolHandler</code> takes care of implementing the
 * update protocol. The protocol provided can be as simple as invoking
 * the a method on an update manager, or as complex managing retransmission
 * etc. While the framework provides some concrete implementation of this
 * class, the user can add new implementation that better satisfy its
 * usage scenario.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 */
class Cdmw::CommonSvcs::UpdateProtocolHandler
{
public:
    /**
     * Destroys an <code>UpdateProtocolHandler</code> instance.
     */
    virtual ~UpdateProtocolHandler() throw();

public:
    /**
     *  Update a topic.
     *
     * @param topic_key  the topic key.
     * @param rcv_policy the reception policy to be used when topic is received.
     * @param encoder    the encoder to be used to generate and encapsulation of the topic.
     */
    virtual void update_topic(long                                  topic_key,
                              const Cdmw::CommonSvcs::RcvPolicy&    rcv_policy,
                              const Cdmw::CommonSvcs::TopicEncoder& encoder) = 0;

    /**
     * Notify an incoming topic update.
     *
     * @param message the message containing the topic update.
     */
    virtual void notify_topic_update(const Cdmw::CommonSvcs::TopicMessage& message) = 0;
};

#endif /* _CDMW_CommonSvcs_UPDATE_PROTOCOL_HANDLER_HPP_ */
