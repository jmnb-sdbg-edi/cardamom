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


#include <logging/Logger_impl.hpp>


namespace Cdmw
{
    namespace Logging
    {
        /**
         * Log a generic message.
         */
        void
        Logger_impl::log(long level,
                         const std::string& message,
                         const char* file,
                         long line)
        {
            LogHeader header;

            header.m_fileLine = line;
            header.m_fileName = file;
            header.m_userLevel = level;
            header.m_userDomain = this->m_domain;
            header.m_componentName = this->m_entity_name;
            header.m_tid = OsSupport::Thread::self();
            header.m_timestamping = OsSupport::OS::get_time();

            m_logMngr->add_log(header, message);
        }


        /**
         * Log a debug message.
         */
        void
        Logger_impl::debug(const std::string& message,
                           const char* file,
                           long line)
        {
            log(::Cdmw::Logging::DEBUG, message, file, line);
        }


        /**
         * Log an info message.
         */
        void
        Logger_impl::info(const std::string& message,
                          const char* file,
                          long line)
        {
            log(::Cdmw::Logging::INFO, message, file, line);
        }


        /**
         * Log a warning message.
         */
        void
        Logger_impl::warn(const std::string& message,
                          const char* file,
                          long line)
        {
            log(::Cdmw::Logging::WARN, message, file, line);
        }


        /**
         * Log an error message.
         */
        void
        Logger_impl::error(const std::string& message,
                           const char* file,
                           long line)
        {
            log(::Cdmw::Logging::ERROR, message, file, line);
        }


        /**
         * Log a fatal error message.
         */
        void
        Logger_impl::fatal(const std::string& message,
                           const char* file,
                           long line)
        {
            log(::Cdmw::Logging::FATAL, message, file, line);
        }


        /**
         * Check whether a log level is enabled.
         */
        bool
        Logger_impl::is_enabled_for(long level)
        {
            return m_logMngr->is_enabled_for(m_entity_name, m_domain, level);
        }


        /**
         * Check whether the DEBUG log level is enabled.
         */
        bool
        Logger_impl::is_debug_enabled()
        {
            return m_logMngr->is_enabled_for(m_entity_name,
                                             m_domain,
                                             Cdmw::Logging::DEBUG);
        }


        /**
         * Check whether the INFO log level is enabled.
         */
        bool
        Logger_impl::is_info_enabled()
        {
            return m_logMngr->is_enabled_for(m_entity_name,
                                             m_domain,
                                             Cdmw::Logging::INFO);
        }


        /**
         * Check whether the WARN log level is enabled.
         */
        bool
        Logger_impl::is_warn_enabled()
        {
            return m_logMngr->is_enabled_for(m_entity_name,
                                             m_domain,
                                             Cdmw::Logging::WARN);
        }


        /**
         * Check whether the ERROR log level is enabled.
         */
        bool
        Logger_impl::is_error_enabled()
        {
            return m_logMngr->is_enabled_for(m_entity_name,
                                             m_domain,
                                             Cdmw::Logging::ERROR);
        }

        /**
         * Check whether the FATAL log level is enabled.
         */
        bool
        Logger_impl::is_fatal_enabled()
        {
            return m_logMngr->is_enabled_for(m_entity_name,
                                             m_domain,
                                             Cdmw::Logging::FATAL);
        }
    } // End namespace Logging
} // End namespace Cdmw
