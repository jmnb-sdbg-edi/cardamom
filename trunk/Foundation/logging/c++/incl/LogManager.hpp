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


#ifndef _CDMW_FDS_LOGGING_LOGMANAGER_HPP_
#define _CDMW_FDS_LOGGING_LOGMANAGER_HPP_


#include <string>

#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/osthreads/ReaderWriterLock.hpp>
#include <Foundation/logging/LogHeader.hpp>


namespace Cdmw
{
    namespace Logging
    {
        class LogManagerDelegate;

        /**
         *
         */
        class LogManager
        {
            public:
                /**
                 * Destructor.
                 */
                virtual
                ~LogManager()
                    throw();


                /**
                 * Return the singleton instance.
                 */
                inline static
                LogManager* Instance()
                    throw() 
                {
                    CDMW_ASSERT(M_Instance != 0);
                    return M_Instance;
                }


                /**
                 * Initialise the LogManager.
                 *
                 * It creates a default delegate, a target to the logs.
                 * Command-line arguments will be used to initialise the
                 * default logger.
                 *
                 * @return the LogManager that has been initialised.
                 */
                static LogManager*
                Init(int& argc, char** argv)
                    throw(OutOfMemoryException,
                         InternalErrorException);


                /**
                 * Check whether the target level of the specified domain
                 * is enabled.
                 *
                 * @param domain the domain.
                 * @param level the level.
                 *
                 * @return true if enabled.
                 */
                bool 
                is_enabled_for(const std::string& domain, long level)
                    throw(OutOfMemoryException);


                /**
                 * Check whether the target level of the specified entity/domain
                 * is enabled.
                 *
                 * @param entity_name the entity.
                 * @param domain the domain.
                 * @param level the level.
                 *
                 * @return true if enabled.
                 */
                bool
                is_enabled_for(const std::string& entity_name,
                               const std::string& domain,
                               long level)
                    throw(OutOfMemoryException);


                /**
                 * Send a message to the logger.
                 *
                 * @param header the message header.
                 * @param msg the message.
                 *
                 * @return true if successful.
                 */
                bool
                add_log(const LogHeader& header, const std::string& msg)
                    throw(InternalErrorException);


                /**
                 * Register an alternate delegate.
                 *
                 * @param logmgr the alternate delegate.
                 *
                 * @return the previous registered alternate delegate.
                 */
                LogManagerDelegate*
                register_alternate_delegate(LogManagerDelegate* logmgr);


                /**
                 * Unregister the alternate delegate.
                 *
                 * @param logmgr the alternate delegate.
                 *
                 * @return true if the alternate delegate was previously registered.
                 */
                bool
                unregister_alternate_delegate(LogManagerDelegate* logmgr);


                /**
                 * Get the alternate delegate.
                 *
                 * @return the alternate delegate.
                 */
                LogManagerDelegate*
                get_alternate_delegate() const;


            protected:
                /**
                 * Constructor.
                 */
                LogManager()
                    throw();


                /**
                 * Constructor.
                 *
                 * @param argc number of arguments in argv.
                 * @param argv list of arguments.
                 */
                LogManager(int& argc, char** argv)
                    throw(OutOfMemoryException,
                          InternalErrorException);


            private:
                // the default delegate.
                LogManagerDelegate* m_default_delegate;

                // the alternate delegate supersedes the default
                // delegate if defined.
                LogManagerDelegate* m_alternate_delegate;
                
                // mutex protecting all access to the LogManager.
                static OsSupport::ReaderWriterLock M_logMngr_mtx;
                
                // the singleton object.
                static LogManager* M_Instance;
        };
    } // end namespace Logging
} // end namespace Cdmw


#endif // _CDMW_FDS_LOGGING_LOGMANAGER_HPP_
