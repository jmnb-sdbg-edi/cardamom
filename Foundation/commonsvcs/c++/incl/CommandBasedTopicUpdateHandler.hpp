/* ===================================================================== */
/*
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
*/
/* ===================================================================== */
#ifndef _CDMW_FOUNDATION_COMMONSVCS_COMMAND_BASED_TOPIC_UPDATE_HANDLER_HPP_
#define _CDMW_FOUNDATION_COMMONSVCS_COMMAND_BASED_TOPIC_UPDATE_HANDLER_HPP_

// -- STD C++ Includes --
#include <map>

// -- CARDAMOM Includes --
#include "Foundation/orbsupport/Codec.hpp"
#include "Foundation/commonsvcs/TopicUpdateHandler.hpp"
#include "Foundation/commonsvcs/ParametricCommand.hpp"

namespace Cdmw {
    namespace CommonSvcs {
        template <typename TOPIC_TYPE, typename COMMAND_GENERATOR>
        class CommandBasedTopicUpdateHandler;
    }
}

/**
 * This class can be used to handle updates by executing commands. It
 * assumes that the a command code can be extracted from the given
 * TOPIC_TYPE by using a COMMAND_GENERATOR. The command is used in
 * order to dispatch the update.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 *
 */
template <typename TOPIC_TYPE,
          typename COMMAND_GENERATOR>
class Cdmw::CommonSvcs::CommandBasedTopicUpdateHandler
    : public virtual ::Cdmw::CommonSvcs::TopicUpdateHandler
{
public:
    typedef Cdmw::OrbSupport::Codec<TOPIC_TYPE>              Codec_t;
    typedef Cdmw::CommonSvcs::ParametricCommand<TOPIC_TYPE>  Handler_t;

public:
    /**
     * Creates a <code>CommandBasedTopicUpdateHandler</code>.
     *
     * @param codec the coded used in order to decode the topic.
     */
    explicit CommandBasedTopicUpdateHandler(Codec_t& codec) throw()
        : m_codec(codec) { }

    /**
     * Destroy a <code>CommandBasedTopicUpdateHandler</code>.
     */
    virtual ~CommandBasedTopicUpdateHandler() throw() { }

public:
    /**
     * Register a command with this
     * <code>CommandBasedTopicUpdateHandler</code>. Commands are
     * executed for handling topic dispatch.
     *
     * @param id the command id
     * @param cmd the command
     */
    virtual void register_command(int         id,
                                  Handler_t*  cmd)
        throw()
        {
            CDMW_ASSERT(cmd != 0);
            m_commands[id] = cmd;
        }

    /**
     * Handles a topic update.
     *
     * @param data the encoded topic.
     */
    virtual void handle_update(const CORBA::OctetSeq& data)
    {
        TOPIC_TYPE topic;
        m_codec.decode(data, topic);
        COMMAND_GENERATOR cgen;
        Handler_t* handler = m_commands[cgen(topic)];
        CDMW_ASSERT(handler);
        handler->execute(topic); 
    }

private:
    // -- Forbid copy ctor and assignment
    CommandBasedTopicUpdateHandler
    (const CommandBasedTopicUpdateHandler&);
    
    CommandBasedTopicUpdateHandler& operator=
    (const CommandBasedTopicUpdateHandler&);

protected:
    typedef std::map<int, Handler_t*> CommandMap_t;
    
protected:
    Codec_t       m_codec; 
    CommandMap_t  m_commands;
};

#endif /* _CDMW_FOUNDATION_COMMONSVCS_COMMAND_BASED_TOPIC_UPDATE_HANDLER_HPP_ */
