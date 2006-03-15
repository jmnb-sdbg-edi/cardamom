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


#ifndef _CDMW_FDS_LOGGING_LOGGER_HPP_
#define _CDMW_FDS_LOGGING_LOGGER_HPP_


#include <sstream>
#include <string>

#include <Foundation/logging/LogLevel.hpp>


namespace Cdmw
{
    namespace Logging
    {
        /**
         * Standard logger interface.
         *
         * This class provides standard CDMW logger interface.
         * It is (highly) inspired by Apache's Log4cxx.
         */
        class Logger
        {
            public:
                /**
                 * Destructor.
                 */
                virtual
                ~Logger()
                    throw()
                {
                }


                /**
                 * Log a generic message.
                 *
                 * @param level the message level.
                 * @param message the message.
                 * @param file the file from which the message is output.
                 * @param line the line number in the file.
                 */
                virtual void
                log(long level,
                    const std::string& message,
                    const char* file,
                    long line) = 0;


                /**
                 * Log a debug message.
                 *
                 * @param message the message.
                 * @param file the file from which the message is output.
                 * @param line the line number in the file.
                 */
                virtual void
                debug(const std::string& message,
                      const char* file,
                      long line) = 0;


                /**
                 * Log an info message.
                 *
                 * @param message the message.
                 * @param file the file from which the message is output.
                 * @param line the line number in the file.
                 */
                virtual void
                info(const std::string& message,
                     const char* file,
                     long line) = 0;


                /**
                 * Log a warning message.
                 *
                 * @param message the message.
                 * @param file the file from which the message is output.
                 * @param line the line number in the file.
                 */
                virtual void
                warn(const std::string& message,
                     const char* file,
                     long line) = 0;


                /**
                 * Log an error message.
                 *
                 * @param message the message.
                 * @param file the file from which the message is output.
                 * @param line the line number in the file.
                 */
                virtual void
                error(const std::string& message,
                      const char* file,
                      long line) = 0;


                /**
                 * Log a fatal error message.
                 *
                 * @param message the message.
                 * @param file the file from which the message is output.
                 * @param line the line number in the file.
                 */
                virtual void
                fatal(const std::string& message,
                      const char* file,
                      long line) = 0;


                /**
                 * Check whether a log level is enabled.
                 *
                 * @param line the log level
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_enabled_for(long level) = 0;


                /**
                 * Check whether the DEBUG log level is enabled.
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_debug_enabled() = 0;


                /**
                 * Check whether the INFO log level is enabled.
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_info_enabled() = 0;


                /**
                 * Check whether the WARN log level is enabled.
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_warn_enabled() = 0;


                /**
                 * Check whether the ERROR log level is enabled.
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_error_enabled() = 0;


                /**
                 * Check whether the FATAL log level is enabled.
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_fatal_enabled() = 0;


                /**
                 * Find the logger associated to the specified domain name.
                 * 
                 * NOTA: the caller must assume the ownership of the returned
                 * Logger object.
                 *
                 * @param domain the domain name.
                 *
                 * @return the logger if found, NULL otherwise.
                 */
                static Logger*
                Get_Logger(const std::string& domain);


                /**
                 * Find the logger associated to the specified entity name and
                 * domain name.
                 * 
                 * NOTA: the caller must assume the ownership of the returned
                 * Logger object.
                 *
                 * @param domain the domain name.
                 *
                 * @return the logger if found, NULL otherwise.
                 */
                static Logger*
                Get_Logger(const std::string& entity_name, 
                           const std::string& domain);
        };
    } // end namespace Logging
} // end namespace Cdmw


// Let's define helper macros!
#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 6 )
#define CDMW_LOG(logger, level, msg)                                                \
        if (logger->is_enabled_for(level)) {                                        \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(level, __log_oss__.str(), __FILE__, __LINE__);                      \
        }
#else
#define CDMW_LOG(logger, level, msg)
#endif


#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 1 )
#define CDMW_FATAL(logger, msg)                                                     \
        if (logger->is_fatal_enabled()) {                                           \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::FATAL, __log_oss__.str(), __FILE__, __LINE__);     \
        }
#else
#define CDMW_FATAL(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 2 )
#define CDMW_ERROR(logger, msg)                                                     \
        if (logger->is_error_enabled()) {                                           \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::ERROR, __log_oss__.str(), __FILE__, __LINE__);     \
        }
#else
#define CDMW_ERROR(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 3 )
#define CDMW_WARN(logger, msg)                                                      \
        if (logger->is_warn_enabled()) {                                            \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::WARN, __log_oss__.str(), __FILE__, __LINE__);      \
        }
#else
#define CDMW_WARN(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 4 )
#define CDMW_INFO(logger, msg)                                                      \
        if (logger->is_info_enabled()) {                                            \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::INFO, __log_oss__.str(), __FILE__, __LINE__);      \
        }
#else
#define CDMW_INFO(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 5 )
#define CDMW_DEBUG(logger, msg)                                                     \
        if (logger->is_debug_enabled()) {                                           \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::DEBUG, __log_oss__.str(), __FILE__, __LINE__);     \
        }
#else
#define CDMW_DEBUG(logger, msg)
#endif

// Additional macros for CARDAMOM
#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 6 )
#define CDMW_DEBUG_1(logger, msg)                                                   \
        if (logger->is_enabled_for(::Cdmw::Logging::DEBUG_1)) {                     \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::DEBUG_1, __log_oss__.str(), __FILE__, __LINE__);   \
        }
#else
#define CDMW_DEBUG_1(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 6 )
#define CDMW_DEBUG_2(logger, msg)                                                   \
        if (logger->is_enabled_for(::Cdmw::Logging::DEBUG_2)) {                     \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::DEBUG_2, __log_oss__.str(), __FILE__, __LINE__);   \
        }
#else
#define CDMW_DEBUG_2(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 6 )
#define CDMW_DEBUG_3(logger, msg)                                                   \
        if (logger->is_enabled_for(::Cdmw::Logging::DEBUG_3)) {                     \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::DEBUG_3, __log_oss__.str(), __FILE__, __LINE__);   \
        }
#else
#define CDMW_DEBUG_3(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 6 )
#define CDMW_DEBUG_4(logger, msg)                                                   \
        if (logger->is_enabled_for(::Cdmw::Logging::DEBUG_4)) {                     \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::DEBUG_4, __log_oss__.str(), __FILE__, __LINE__);   \
        }
#else
#define CDMW_DEBUG_4(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 6 )
#define CDMW_DEBUG_5(logger, msg)                                                   \
        if (logger->is_enabled_for(::Cdmw::Logging::DEBUG_5)) {                     \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::DEBUG_5, __log_oss__.str(), __FILE__, __LINE__);   \
        }
#else
#define CDMW_DEBUG_5(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 6 )
#define CDMW_DEBUG_6(logger, msg)                                                   \
        if (logger->is_enabled_for(::Cdmw::Logging::DEBUG_6)) {                     \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::DEBUG_6, __log_oss__.str(), __FILE__, __LINE__);   \
        }
#else
#define CDMW_DEBUG_6(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 6 )
#define CDMW_DEBUG_7(logger, msg)                                                   \
        if (logger->is_enabled_for(::Cdmw::Logging::DEBUG_7)) {                     \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::DEBUG_7, __log_oss__.str(), __FILE__, __LINE__);   \
        }
#else
#define CDMW_DEBUG_7(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 6 )
#define CDMW_DEBUG_8(logger, msg)                                                   \
        if (logger->is_enabled_for(::Cdmw::Logging::DEBUG_8)) {                     \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::DEBUG_8, __log_oss__.str(), __FILE__, __LINE__);   \
        }
#else
#define CDMW_DEBUG_8(logger, msg)
#endif

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 6 )
#define CDMW_DEBUG_9(logger, msg)                                                   \
        if (logger->is_enabled_for(::Cdmw::Logging::DEBUG_9)) {                     \
            std::ostringstream __log_oss__;                                                 \
            __log_oss__ << msg;                                                             \
            logger->log(::Cdmw::Logging::DEBUG_9, __log_oss__.str(), __FILE__, __LINE__);   \
        }

#else
#define CDMW_DEBUG_9(logger, msg)
#endif

#endif // _CDMW_FDS_LOGGING_LOGGER_HPP_
