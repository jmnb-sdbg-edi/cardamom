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


#include <Foundation/logging/LogLevel.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>

#include <TraceAndPerf/tracelibrary/FlushAreaMngr.hpp>
#include <TraceAndPerf/tracelibrary/Message.hpp>

#include "tracelibrary/LogManagerTraceDelegate.hpp"


namespace Cdmw
{
namespace Trace
{


/**
 * Constructor.
 */
LogManagerTraceDelegate::LogManagerTraceDelegate(FlushAreaMngr& flushAreaMngr)
    throw(OutOfMemoryException,
          InternalErrorException)
    : m_flushAreaMngr(flushAreaMngr)
{
}


/**
 * Destructor.
 */
LogManagerTraceDelegate::~LogManagerTraceDelegate()
    throw()
{
}


/**
 * Send a message to the Trace service.
 */
bool
LogManagerTraceDelegate::add_log(const Cdmw::Logging::LogHeader& header,
                                 const std::string& msg)
    throw(InternalErrorException)
{
    CDMW_MUTEX_GUARD(m_mutex);

    try {
        // map LogHeader into MessageHeader.
        Message::MessageHeader msg_header;

        msg_header.m_fileLine      = header.m_fileLine;
        msg_header.m_fileName      = header.m_fileName;
        msg_header.m_userLevel     = header.m_userLevel;
        msg_header.m_userDomain    = header.m_userDomain;
        msg_header.m_componentName = header.m_componentName;
        msg_header.m_tid           = header.m_tid;
        msg_header.m_timestamping  = header.m_timestamping;

        m_flushAreaMngr.add_message(msg_header, msg);
    }
    catch(const std::bad_alloc&) {
        CDMW_THROW(OutOfMemoryException);
    }

    return true;
}


} // namespace Trace
} // namespace Cdmw
