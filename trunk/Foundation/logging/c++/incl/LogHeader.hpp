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


#ifndef _CDMW_FDS_LOGGING_LOGHEADER_HPP_ 
#define _CDMW_FDS_LOGGING_LOGHEADER_HPP_


#include <string>

#include <Foundation/ossupport/OS.hpp> // Timeval
#include <Foundation/osthreads/ThreadHandle.hpp> // ThreadId


namespace Cdmw
{
    namespace Logging
    {
        /**
         * This structure fully describes the origin of a message,
         * it is used to prefix each message.
         */
        struct LogHeader
        {
            // the thread which has created the message.
            OsSupport::Thread::ThreadId m_tid;
            
            // the line in the file that produced the message.
            long m_fileLine;
            
            // the name of the file that produced the message.
            std::string m_fileName;
            
            // the component name.
            std::string m_componentName;
            
            // the domain of the message.
            std::string m_userDomain;
            
            // the level of the message.
            long m_userLevel;
            
            // the time at which the message has been produced.
            OsSupport::OS::Timeval m_timestamping;
        };
    } // end namespace Logging
} // end namespace Cdmw


#endif // _CDMW_FDS_LOGGING_LOGHEADER_HPP_
