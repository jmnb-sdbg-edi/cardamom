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


#ifndef INCL_MESSAGEQUEUEDESTRUCTOR_HPP
#define INCL_MESSAGEQUEUEDESTRUCTOR_HPP

#include "SystemMngt/platformlibrary/MessageQueue.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/TestManager.hpp"
#include <string>


class MessageQueueDestructor : virtual public Cdmw::OsSupport::Thread
{

public:

    MessageQueueDestructor(Cdmw::PlatformMngt::MessageQueue<std::string>& messageQueue,
        unsigned int delay) : m_messageQueue(messageQueue), m_delay(delay)
    {
    }

    virtual ~MessageQueueDestructor() throw() {}

protected:

    Cdmw::PlatformMngt::MessageQueue<std::string>& m_messageQueue;
    unsigned int m_delay;

    void run() throw()
    {
	    int timescale = Cdmw::TestUtils::get_timescale();
        Cdmw::OsSupport::OS::sleep(timescale*m_delay);

        m_messageQueue.destroy();

    }

};

#endif // INCL_MESSAGEQUEUEDESTRUCTOR_HPP

