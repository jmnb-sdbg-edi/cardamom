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


#ifndef INCL_MESSAGEQUEUEWRITER_HPP
#define INCL_MESSAGEQUEUEWRITER_HPP

#include "testplatformlibrary/MessageQueueWrapper.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/TestManager.hpp"
#include <string>
#include <memory>
#include <sstream>

using namespace Cdmw;

class MessageQueueWriter : virtual public OsSupport::Thread
{

public:

    MessageQueueWriter(MessageQueueWrapper& messageQueue,
        const std::string& messageHeader, size_t nbMessages, unsigned int delay)
        : m_messageQueue(messageQueue), m_messageHeader(messageHeader),
        m_nbMessages(nbMessages), m_delay(delay)
    {
    }

    virtual ~MessageQueueWriter() throw() {}

protected:

    MessageQueueWrapper& m_messageQueue;
    std::string m_messageHeader;
    size_t m_nbMessages;
    unsigned int m_delay;

    void run() throw()
    {
        size_t messageCount = 0;

        do
        {
            try
            {

                std::ostringstream temp;
                temp << m_messageHeader << messageCount << std::ends;

                std::auto_ptr<std::string> pMessage(new std::string(temp.str()));
				int timescale = Cdmw::TestUtils::get_timescale();
                OsSupport::OS::sleep(timescale*m_delay);

                m_messageQueue.push(pMessage.get());
                pMessage.release();

                ++messageCount;

            }
            catch(const std::bad_alloc&)
            {

            }

        }
        while (messageCount < m_nbMessages);

    }

};

#endif // INCL_MESSAGEQUEUEWRITER_HPP

