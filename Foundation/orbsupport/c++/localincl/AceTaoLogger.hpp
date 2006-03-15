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


#ifndef INCL_ORBSUPPORT_ACETAO_LOGGER_HPP
#define INCL_ORBSUPPORT_ACETAO_LOGGER_HPP

#if CDMW_ORB_VDR == tao


#include "ace/Log_Msg_Backend.h"
#include "ace/Log_Record.h"
#include <Foundation/logging/Logger.hpp>
#include <Foundation/logging/LogManager.hpp>
#include <Foundation/common/Assert.hpp>


namespace Cdmw
{

    namespace OrbSupport
    {
        const char* ACE_TAO_LOG_DOMAIN = "ACE/TAO";
        /**
        *Purpose:
        *<p>    This class allows collection of All ACE/TAO logs
        *
        *Features:
        *<p> Thread safe
        *
        */
        class AceTaoLogger : public ACE_Log_Msg_Backend
        {
        public:
            AceTaoLogger()
                : m_logger(Cdmw::Logging::Logger::Get_Logger(ACE_TAO_LOG_DOMAIN))
            {
                CDMW_ASSERT(m_logger != 0);
            }
            
            /// Destructor
            ~AceTaoLogger () throw()
            {
                (void) this->close ();
                delete m_logger;
                m_logger = 0;
            }
            
            /// Open a new event log.
            /**
             * Initialize the event logging facility.
             * @param logger_key The name of the calling program. This name is
             *                   used as the @arg ident in the syslog entries. If
             *                   it is 0 (no name), the application name as
             *                   returned from ACE_Log_Msg::program_name() is used.
             */
            int open (const ACE_TCHAR *logger_key)
            {
                return 0;
            }
            
            /// Reset the backend.
            int reset (void)
            {
                return this->close();
            }
            
            /// Close the backend completely.
            int close (void)
            {
                return 0;
            }
            
            // Thiss called when we want to log a message.
            int log (ACE_Log_Record &log_record) 
            {
                long level = (long) this->convert_level (log_record.type ());
                if (m_logger->is_enabled_for(level)) {
                    /// Get the message data of the <Log_Record>.
                    const ACE_TCHAR * msg_data = log_record.msg_data ();
                    // NOTA: ACE_TCHAR is a typedef to char
                    std::string msg (msg_data);
                    m_logger->log(level,msg,"",-1);
                }
                return 0;
            }
            
        private:
            Cdmw::Logging::Logger * m_logger;
            
            int convert_level(int acelevel)
            {
                int level = acelevel;
                
                switch (acelevel) {
                case LM_STARTUP:
                case LM_SHUTDOWN:
                case LM_TRACE:
                case LM_INFO:
                    level = Cdmw::Logging::INFO;
                    break;
                case LM_DEBUG:
                    level = Cdmw::Logging::DEBUG;
                    break;
                case LM_NOTICE:
                case LM_WARNING:
                    level = Cdmw::Logging::WARN;
                    break;
                case LM_ERROR:
                    level = Cdmw::Logging::ERROR;
                    break;
                case LM_CRITICAL:
                case LM_ALERT:
                case LM_EMERGENCY:
                default:
                    level = Cdmw::Logging::FATAL;
                    break;
                }
                return level;
            }
        }; // End of class AceTaoLogger
    } // End namespace OrbSupport
} // End namespace Cdmw

#endif //CDMW_ORB_VDR == tao

#endif // INCL_ORBSUPPORT_ACETAO_LOGGER_HPP
