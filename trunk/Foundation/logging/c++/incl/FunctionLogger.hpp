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


#ifndef _CDMW_FDS_LOGGING_FUNCTION_LOGGER_HPP_
#define _CDMW_FDS_LOGGING_FUNCTION_LOGGER_HPP_

#include <Foundation/logging/Logger.hpp>

namespace Cdmw
{
    namespace Logging
    {
        class FunctionLogger
        {
        public:
            static const long LEVEL;

            FunctionLogger(Logger * logger, const std::string & args,
                           const char * file, const char* function, long line);
            ~FunctionLogger() throw();
            void log_return(const std::string & message, long line);
            void log_exception(const std::string & message, long line);
            inline bool is_enabled() { 
                return m_logger->is_enabled_for(LEVEL);
            }
        private:
            FunctionLogger();

            Logger *       m_logger;
            const char*    m_file;
            const char*    m_function;
            long           m_line;
            std::string    m_message;
        };

    } // end namespace Logging
} // end namespace Cdmw


#if defined __GNUC__

// TODO: Append __FUNCTION__ to variable name

#define _CDMW_MAKE_FUNCTION_LOGGER_NAME_() _CDMW_FUNCTION_LOGGER__
#define _CDMW_MAKE_FUNCTION_LOGGER_ARGS_STRING_NAME_() _CDMW_FUNCTION_LOGGER_ARGS_STRING__
#else

#define _CDMW_MAKE_FUNCTION_LOGGER_NAME_() _CDMW_FUNCTION_LOGGER__
#define _CDMW_MAKE_FUNCTION_LOGGER_ARGS_STRING_NAME_() _CDMW_FUNCTION_LOGGER_ARGS_STRING__

#endif

//#define CDMW_LOG_FUNCTION(logger)  Cdmw::Logging::FunctionLogger _CDMW_MAKE_FUNCTION_LOGGER_NAME_() (logger,__FILE__,__FUNCTION__,__LINE__)

#if defined CDMW_DISABLE_LOG_FUNCTION

#define CDMW_LOG_FUNCTION(logger, X)
    
// Let's define helper macros!
#define CDMW_LOG_FUNCTION_RETURN(ret)  
#define CDMW_LOG_FUNCTION_EXCEPTION(ex) 

#else

#define CDMW_LOG_FUNCTION(logger, X)                                        \
    std::string _CDMW_MAKE_FUNCTION_LOGGER_ARGS_STRING_NAME_()("");         \
    if (logger->is_enabled_for(Cdmw::Logging::FunctionLogger::LEVEL)) {     \
        std::ostringstream oss;                                             \
        oss << X;                                                           \
        _CDMW_MAKE_FUNCTION_LOGGER_ARGS_STRING_NAME_()=oss.str();           \
    }                                                                       \
    Cdmw::Logging::FunctionLogger _CDMW_MAKE_FUNCTION_LOGGER_NAME_()        \
          (logger,_CDMW_MAKE_FUNCTION_LOGGER_ARGS_STRING_NAME_(),__FILE__,__FUNCTION__,__LINE__)



// Let's define helper macros!
#define CDMW_LOG_FUNCTION_RETURN(ret)                                         \
    if (_CDMW_MAKE_FUNCTION_LOGGER_NAME_().is_enabled()) {                    \
        std::ostringstream oss;                                               \
        oss << ret;                                                           \
        _CDMW_MAKE_FUNCTION_LOGGER_NAME_().log_return(oss.str(), __LINE__);   \
    }
#define CDMW_LOG_FUNCTION_EXCEPTION(ex)                                       \
    if (_CDMW_MAKE_FUNCTION_LOGGER_NAME_().is_enabled()) {                    \
        std::ostringstream oss;                                               \
        oss << ex;                                                            \
        _CDMW_MAKE_FUNCTION_LOGGER_NAME_().log_exception(oss.str(), __LINE__);\
    }
#endif

#endif // _CDMW_FDS_LOGGING_FUNCTION_LOGGER_HPP_
