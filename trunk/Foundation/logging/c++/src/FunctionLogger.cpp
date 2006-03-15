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


#include <Foundation/logging/FunctionLogger.hpp>
#include <Foundation/common/Assert.hpp>
#include <sstream>

/**
 * Anonymous namespace
 */
namespace {
    const char * ENTERING="Entering ";
    const char * EXITING="Exiting ";
    const char * RETURN_MSG_START="Returning <";
    const char * RETURN_MSG_END=">";
    const char * EXCEPTION_MSG_START="Raising exception <";
    const char * EXCEPTION_MSG_END=">";
}; // End of anonymous namespace

namespace Cdmw
{
    namespace Logging
    {
        const long FunctionLogger::LEVEL = DEBUG_3;
        
        FunctionLogger::FunctionLogger(Logger * logger, const std::string & args,
                                       const char * file, const char* function, long line)
            : m_logger(logger), m_file(file), m_function(function), m_line(line), m_message("")
        {
            CDMW_ASSERT(logger != 0);
            if (m_logger->is_enabled_for(LEVEL)) { 
                std::ostringstream oss;
                oss << ENTERING << m_function << "(" << args << ") ...";
                m_logger->log(LEVEL, oss.str(), m_file, m_line);
            }
            m_line = -1;
        }

        FunctionLogger::~FunctionLogger() throw()
        {
            try {
                if (m_logger->is_enabled_for(LEVEL)) { 
                    std::ostringstream oss;
                    oss << EXITING << m_function << "() ... " << m_message;
                    m_logger->log(LEVEL, oss.str(), m_file, m_line);
                }
            } catch (...) {
                // ignore
            }
        }
        void FunctionLogger::log_return(const std::string & message, long line)
        {
            std::ostringstream oss;
            oss << "[" << RETURN_MSG_START << message << RETURN_MSG_END << "]";
            m_message = oss.str();
            m_line = line;
        }
        void FunctionLogger::log_exception(const std::string & message, long line)
        {
            std::ostringstream oss;
            oss << "[" << EXCEPTION_MSG_START << message << EXCEPTION_MSG_END << "]";
            m_message = oss.str();
            m_line = line;
        }
    } // end namespace Logging
} // end namespace Cdmw
