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


#include <ctime>
#include <ostream>
#include <sstream>

#include <Foundation/logging/LogLevel.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>

#include "logging/LogManagerDefaultDelegate.hpp"

// Anonymous namespace
namespace
{
    const std::string LOG_LEVEL_OPTION = "--CdmwLogLevel";
    const std::string LOG_FILE_OPTION  = "--CdmwLogFile";

    const std::string sFATAL   = "FATAL";
    const std::string sERROR   = "ERROR";
    const std::string sWARN    = "WARNING";
    const std::string sINFO    = "INFO";
    const std::string sDEBUG   = "DEBUG";
    const std::string sDEBUG_1 = "DEBUG_1";
    const std::string sDEBUG_2 = "DEBUG_2";
    const std::string sDEBUG_3 = "DEBUG_3";
    const std::string sDEBUG_4 = "DEBUG_4";
    const std::string sDEBUG_5 = "DEBUG_5";
    const std::string sDEBUG_6 = "DEBUG_6";
    const std::string sDEBUG_7 = "DEBUG_7";
    const std::string sDEBUG_8 = "DEBUG_8";
    const std::string sDEBUG_9 = "DEBUG_9";


    /**
     * Return the numeric level as string.
     *
     * @param level the level.
     *
     * @return the level as string.
     */
    std::string
    level_to_string(long level)
    {
        switch (level) {
            case Cdmw::Logging::FATAL:   return sFATAL;   break;
            case Cdmw::Logging::ERROR:   return sERROR;   break;
            case Cdmw::Logging::WARN:    return sWARN;    break;
            case Cdmw::Logging::INFO:    return sINFO;    break;
            case Cdmw::Logging::DEBUG:   return sDEBUG;   break;
            case Cdmw::Logging::DEBUG_1: return sDEBUG_1; break;
            case Cdmw::Logging::DEBUG_2: return sDEBUG_2; break;
            case Cdmw::Logging::DEBUG_3: return sDEBUG_3; break;
            case Cdmw::Logging::DEBUG_4: return sDEBUG_4; break;
            case Cdmw::Logging::DEBUG_5: return sDEBUG_5; break;
            case Cdmw::Logging::DEBUG_6: return sDEBUG_6; break;
            case Cdmw::Logging::DEBUG_7: return sDEBUG_7; break;
            case Cdmw::Logging::DEBUG_8: return sDEBUG_8; break;
            case Cdmw::Logging::DEBUG_9: return sDEBUG_9; break;

            default:
                std::ostringstream oss;
                oss << level;
                return oss.str();
        }
    };    
    // TODO: This should be moved to OsSupport
    std::string basename(const std::string & f) 
    {
        const char DELIM='/';
        std::string::size_type i= f.rfind(DELIM);
        if (i != std::string::npos) {
            std::string res = f.substr(i+1,f.size());
            return res;
        } else
            return f;
    }
    
} // End of anonymous namespace


#if defined CDMW_LOG_USE_BASENAME
// Force call to basename() for filenames. This will have a performance impact!
#define CDMW_LOG_BASENAME(X) basename(X)
#else
#define CDMW_LOG_BASENAME(X) X
#endif

namespace Cdmw
{
    namespace Logging
    {
        /**
         * Constructor.
         */
        LogManagerDefaultDelegate::LogManagerDefaultDelegate(int& argc,
                                                             char** argv)
            throw(OutOfMemoryException,
                  InternalErrorException)
            : m_level(INFO),
              m_ofstream(),
              m_postream(&std::cout)
        {
            if (argv != 0) {
                // Parse the command line arguments.
                std::string level_option =
                    Cdmw::OsSupport::OS::get_option_value(argc,
                                                          argv,
                                                          LOG_LEVEL_OPTION);

                if (level_option != "no" && level_option != "yes")
                {
                    long level;
                    std::istringstream str_level(level_option);
                    str_level >> level;

                    if (str_level.fail() || (level < 0))  {
                        // Keep default value.
                    }
                    else {
                        m_level = level;
                    }
                }


                // Is there an output log file
                std::string file_option =
                    Cdmw::OsSupport::OS::get_option_value(argc,
                                                          argv,
                                                          LOG_FILE_OPTION);

                if (file_option != "no" && file_option != "yes")
                {
                    m_ofstream.open(file_option.c_str(), std::ios::out);
                    
                    if (!m_ofstream) {
                        std::cerr << "Couldn't create output file (" << file_option << ") for logger!" 
                                  << std::endl;
                    } else {
                        m_postream = &m_ofstream;
                    }
                }

            }
        }


        /**
         * Destructor.
         */
        LogManagerDefaultDelegate::~LogManagerDefaultDelegate()
            throw()
        {
            try {
                m_postream = &std::cout;
                m_ofstream.close();
            } catch (...) {
                // Ignore ...
            }
        }


        /**
         * Send a message to the logger.
         */
        bool
        LogManagerDefaultDelegate::add_log(const LogHeader& header,
                                           const std::string& msg)
            throw(InternalErrorException)
        {
            CDMW_MUTEX_GUARD(m_mutex);

            time_t ltime = header.m_timestamping.seconds;
            char buffer[256];

            // use time structure to build a customized time string.
            tm now;
            ::localtime_r(&ltime, &now);

            strftime(buffer, 256, "%Y/%m/%d %H:%M:%S", &now);

            (*m_postream) << buffer << ":"
                          << (header.m_timestamping.microseconds) / 1000
                          << ":" << (header.m_timestamping.microseconds) % 1000
                          << " " << level_to_string(header.m_userLevel)
                          << " " << header.m_userDomain
                          << " / [" << header.m_componentName
                          << "] [tid=" << header.m_tid << "]" 
                          << " (" << CDMW_LOG_BASENAME(header.m_fileName) << ":" << header.m_fileLine << ")\n"
                          << "==>" <<  msg << std::endl;

            return true;
        }
    } // end namespace Logging
} // end namespace Cdmw
