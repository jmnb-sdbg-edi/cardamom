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


#ifndef _CDMW_FDS_LOGGING_LOGMANAGERDELEGATE_HPP_
#define _CDMW_FDS_LOGGING_LOGMANAGERDELEGATE_HPP_


#include <string>

#include <Foundation/common/Exception.hpp>
#include <Foundation/logging/LogHeader.hpp>


namespace Cdmw
{
    namespace Logging
    {
        /**
         * Base LogManager delegate.
         */
        class LogManagerDelegate 
        {
            public:
                /**
                 * Destructor.
                 */
                virtual
                ~LogManagerDelegate()
                    throw()
                {
                }


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
                    throw(OutOfMemoryException) = 0;


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
                    throw(OutOfMemoryException) = 0;


                /**
                 * Send a message to the logger.
                 *
                 * @param header the message header.
                 * @param msg the message.
                 *
                 * @return true if successful.
                 */
                virtual bool
                add_log(const LogHeader& header, const std::string& msg)
                    throw(InternalErrorException) = 0;
        };
    } // end namespace Logging
} // end namespace Cdmw


#endif // _CDMW_FDS_LOGGING_LOGMANAGERDELEGATE_HPP_
