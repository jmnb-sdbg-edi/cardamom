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


#ifndef _CDMW_TRC_TRACELIBRARY_LOGMANAGERTRACEDELEGATE_HPP_
#define _CDMW_TRC_TRACELIBRARY_LOGMANAGERTRACEDELEGATE_HPP_


#include <string>

#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/logging/LogManagerDelegate.hpp>
#include <TraceAndPerf/tracelibrary/FlushAreaMngr.hpp>


namespace Cdmw
{
namespace Trace
{


/**
 * An alternate log manager delegate based on the Trace service.
 */
class LogManagerTraceDelegate: public Cdmw::Logging::LogManagerDelegate
{
    public:
        /**
         * Constructor.
         *
         * @param level
         */
        LogManagerTraceDelegate(FlushAreaMngr& flushAreaMngr)
            throw(OutOfMemoryException,
                  InternalErrorException);


        /**
         * Destructor.
         */
        ~LogManagerTraceDelegate()
            throw();


        /**
         * Check whether the target level of the specified domain
         * is enabled.
         *
         * @param domain the domain.
         * @param level the level.
         *
         * @return true if enabled.
         */
        virtual bool
        is_enabled_for(const std::string& domain, long level)
            throw(OutOfMemoryException)
        {
            return m_flushAreaMngr.is_to_be_traced(domain,level); // level <= m_level;
        }


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
        virtual bool
        is_enabled_for(const std::string& entity_name,
                       const std::string& domain,
                       long level)
            throw(OutOfMemoryException)
        {
            return m_flushAreaMngr.is_to_be_traced(entity_name,domain,level); // level <= m_level;
        }


        /**
         * Send a message to the logger.
         *
         * @param header the message header.
         * @param msg the message.
         *
         * @return true if successful.
         */
        virtual bool
        add_log(const Cdmw::Logging::LogHeader& header, const std::string& msg)
            throw(InternalErrorException);


    private:
        OsSupport::Mutex m_mutex;
    FlushAreaMngr& m_flushAreaMngr;
};


} // namespace Trace
} // namespace Cdmw


#endif // _CDMW_TRC_TRACELIBRARY_LOGMANAGERTRACEDELEGATE_HPP_
