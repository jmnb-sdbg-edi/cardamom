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


#ifndef _CDMW_FDS_LOGGING_LOGGER_IMPL_HPP_
#define _CDMW_FDS_LOGGING_LOGGER_IMPL_HPP_


#include <Foundation/logging/Logger.hpp>
#include <Foundation/logging/LogLevel.hpp>
#include <Foundation/logging/LogManager.hpp>


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
        class Logger_impl: public Logger
        {
            public:
                /**
                 * Constructor.
                 *
                 * @param logMngr the LogManager.
                 * @param entity_name the entity.
                 * @param domain the domain.
                 */
                Logger_impl(LogManager* logMngr,
                            const std::string& entity_name,
                            const std::string& domain)
                        : m_logMngr(logMngr),
                          m_entity_name(entity_name),
                          m_domain(domain)
                {
                }


                /**
                 * Destructor.
                 */
                virtual
                ~Logger_impl()
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
                    long line);


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
                      long line);


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
                     long line);


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
                     long line);


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
                      long line);


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
                      long line);


                /**
                 * Check whether a log level is enabled.
                 *
                 * @param line the log level
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_enabled_for(long level);


                /**
                 * Check whether the DEBUG log level is enabled.
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_debug_enabled();


                /**
                 * Check whether the INFO log level is enabled.
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_info_enabled();


                /**
                 * Check whether the WARN log level is enabled.
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_warn_enabled();


                /**
                 * Check whether the ERROR log level is enabled.
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_error_enabled();


                /**
                 * Check whether the FATAL log level is enabled.
                 *
                 * @return true if enabled.
                 */
                virtual bool
                is_fatal_enabled();


        private:
            /**
             * Default constructor.
             */
            Logger_impl();


        private:
            LogManager*       m_logMngr;
            const std::string m_entity_name;
            const std::string m_domain;
        };

    } // end namespace Logging
} // end namespace Cdmw


#endif // _CDMW_FDS_LOGGING_LOGGER_IMPL_HPP_
