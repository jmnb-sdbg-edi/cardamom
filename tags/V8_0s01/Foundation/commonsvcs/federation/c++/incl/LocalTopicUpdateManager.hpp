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
#ifndef _CDMW_FOUNDATION_CommonSvcs_LOCAL_DISTRIBUTION_MANAGER_
#define _CDMW_FOUNDATION_CommonSvcs_LOCAL_DISTRIBUTION_MANAGER_

#include "Foundation/commonsvcs/federation/TopicUpdateHandler.hpp"
#include "Foundation/commonsvcs/federation/TopicEncoder.hpp"
#include "Foundation/commonsvcs/federation/UpdateProtocolHandler.hpp"

#include <map>

namespace Cdmw {
    namespace CommonSvcs {
        class LocalTopicUpdateManager;
    }
}

/**
 * The <code>LocalTopicUpdateManager</code> provides the local point of
 * access for the management and distribution of topic. <br>
 * <b>NOTE:</b>This interface might be subject to change in future
 * CARDAMOM releases.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0

 */
class Cdmw::CommonSvcs::LocalTopicUpdateManager
{
public:
    /**
     * Finalize the <code>LocalTopicUpdateManager</code>
     */
    ~LocalTopicUpdateManager() throw();
    
public:
    /**
     * Return the unique instance.
     */
    static LocalTopicUpdateManager& instance() throw();

    /**
     * Initialize the <code>LocalTopicUpdateManager</code>. This
     * method has to be called right after the ORB initialization is
     * completed.
     *
     * @param handler the protocol handler.
     */
    static void init(::Cdmw::CommonSvcs::UpdateProtocolHandler* handler);
    
public:
    /**
     * Register a handler for a given topic. The handler will be
     * invoked each time an update is received for the given topic.
     *
     * @param topic_key the key associated to the topic.
     * @param handler the topic update handler.
     */ 
    void set_topic_handler(long topic_key, 
                           ::Cdmw::CommonSvcs::TopicUpdateHandler* handler)
        throw();
    
    /**
     * Update a given topic content.
     *
     * @param topic_key    the topic to be updated.
     * @param TopicEncoder the encoder which will create an encapsulation for the topic content. 
     * @param rcv_policy   the reception policy to be used when topic is received.
     */
    void update_topic(long                                    topic_key,
                      const ::Cdmw::CommonSvcs::TopicEncoder& encoder,
                      const ::Cdmw::CommonSvcs::RcvPolicy&    rcv_policy = Cdmw::CommonSvcs::ACCEPT_NEWER);
    
    
    /**
     * Handles an incoming update message for a given topic.
     *
     * @param topic the topic.
     */
    void notify_topic_update(const ::Cdmw::CommonSvcs::Topic& topic);
    
protected:
    LocalTopicUpdateManager(::Cdmw::CommonSvcs::UpdateProtocolHandler* handler);

private:
    LocalTopicUpdateManager(const LocalTopicUpdateManager&);
    LocalTopicUpdateManager& operator=(const LocalTopicUpdateManager&);
    
protected:
    typedef std::map<long, ::Cdmw::CommonSvcs::TopicUpdateHandler*>
    TopicHandlerMap_t;
    
protected:
    ::Cdmw::CommonSvcs::UpdateProtocolHandler*  m_update_protocol_handler;
    TopicHandlerMap_t                           m_topic_handler_map; 
    static LocalTopicUpdateManager*             M_instance;
};

#endif /* _CDMW_FOUNDATION_CommonSvcs_LOCAL_DISTRIBUTION_MANAGER_ */
