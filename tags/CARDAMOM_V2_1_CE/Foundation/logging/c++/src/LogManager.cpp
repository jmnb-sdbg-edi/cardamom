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


#include <Foundation/osthreads/ReaderLockGuard.hpp>
#include <Foundation/osthreads/WriterLockGuard.hpp>
#include <Foundation/logging/LogManager.hpp>

#include "logging/LogManagerDefaultDelegate.hpp"


namespace Cdmw
{
    namespace Logging
    {
        // initialize the static variables.
        OsSupport::ReaderWriterLock LogManager::M_logMngr_mtx;
        LogManager* LogManager::M_Instance(0);


        /**
         * Default constructor.
         */
        LogManager::LogManager()
            throw()
                : m_default_delegate(0),
                  m_alternate_delegate(0)
        {
        }


        /**
         * Constructor.
         *
         * @param argc number of arguments in argv.
         * @param argv list of arguments.
         */
        LogManager::LogManager(int& argc, char** argv)
            throw(OutOfMemoryException,
                  InternalErrorException)
                : m_default_delegate (0),
                  m_alternate_delegate(0)
        {
            m_default_delegate = new LogManagerDefaultDelegate(argc, argv);
        }


        /**
         * Destructor.
         */
        LogManager::~LogManager()
            throw()
        {
            CDMW_WRITER_LOCK_GUARD(M_logMngr_mtx);

            // we are not assuming the ownership of the pointer so
            // we'll just set it to NULL.
            m_alternate_delegate = 0;

            delete m_default_delegate;
            m_default_delegate = 0;
        }


        /**
         * Initialise the LogManager.
         */
        LogManager*
        LogManager::Init(int& argc, char** argv)
            throw(OutOfMemoryException,
                  InternalErrorException)
        {
            try {
                CDMW_WRITER_LOCK_GUARD(M_logMngr_mtx);

                if (M_Instance == 0) {
                    M_Instance = new LogManager(argc, argv);
                }

                return M_Instance;
            } catch (const std::bad_alloc&) {
                throw OutOfMemoryException();
            }
        }


        /**
         * Check whether the target level of the specified domain
         * is enabled.
         */
        bool 
        LogManager::is_enabled_for(const std::string& domain, long level)
            throw(OutOfMemoryException)
        {
            // FIXME: The lock will be removed once the FilterMngr is moved 
            // to the Logging CSC.
            CDMW_READER_LOCK_GUARD(M_logMngr_mtx);

            if (m_alternate_delegate) {
                return m_alternate_delegate->is_enabled_for(domain, level);
            }
            else {
                return m_default_delegate->is_enabled_for(domain, level);
            }
        }


        /**
         * Check whether the target level of the specified entity/domain
         * is enabled.
         */
        bool
        LogManager::is_enabled_for(const std::string& entity_name,
                                   const std::string& domain,
                                   long level)
            throw(OutOfMemoryException)
        {
            // FIXME: The lock will be removed once the FilterMngr is moved 
            // to the Logging CSC.
            CDMW_READER_LOCK_GUARD(M_logMngr_mtx);

            if (m_alternate_delegate) {
                return m_alternate_delegate->is_enabled_for(entity_name,
                                                            domain,
                                                            level);
            }
            else {
                return m_default_delegate->is_enabled_for(entity_name,
                                                          domain,
                                                          level);
            }
        }


        /**
         * Send a message to the logger.
         */
        bool
        LogManager::add_log(const LogHeader& header, const std::string& msg)
            throw(InternalErrorException)
        {
            // FIXME: The lock will be removed once the FilterMngr is moved 
            // to the Logging CSC.
            CDMW_READER_LOCK_GUARD(M_logMngr_mtx);

            if (m_alternate_delegate) {
                return m_alternate_delegate->add_log(header, msg);
            }
            else {
                return m_default_delegate->add_log(header, msg);
            }
        }


        /**
         * Register an alternate delegate.
         */
        LogManagerDelegate*
        LogManager::register_alternate_delegate(LogManagerDelegate* alt)
        {
            CDMW_WRITER_LOCK_GUARD(M_logMngr_mtx);

            LogManagerDelegate* old = m_alternate_delegate;
            m_alternate_delegate = alt;

            return old;
        }


        /**
         * Unregister the alternate delegate.
         */
        bool
        LogManager::unregister_alternate_delegate(LogManagerDelegate* alt)
        {
            CDMW_WRITER_LOCK_GUARD(M_logMngr_mtx);

            bool result = false;

            if (alt == m_alternate_delegate) {
                result = true;
                m_alternate_delegate = 0;
            }

            return result;
        }


        /**
         * Get the alternate delegate.
         *
         * @return the alternate delegate.
         */
        LogManagerDelegate*
        LogManager::get_alternate_delegate() const
        {
            return m_alternate_delegate;
        }
    } // end namespace Logging
} // end namespace Cdmw
