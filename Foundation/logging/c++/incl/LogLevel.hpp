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


#ifndef _CDMW_FDS_LOGGING_LOGLEVEL_HPP_
#define _CDMW_FDS_LOGGING_LOGLEVEL_HPP_


namespace Cdmw
{
    namespace Logging
    {
        /**
         * Default log levels.
         */
        enum LogLevel
        {
            // this level reports a fatal error from CDMW; CDMW is no more
            // able to process safely.
            FATAL = 1,

            // this level reports an erorr from CDMW.
            ERROR = 2,

            // maybe the user has made an error, warn him.
            WARN = 4,

            // used for information purposes.
            INFO = 8,

            // used for debug purposes.
            DEBUG = 16,

            /**
             * Additional debug levels for CDMW.
             */
            DEBUG_1 = 32,
            DEBUG_2 = 64,
            DEBUG_3 = 128,
            DEBUG_4 = 256,
            DEBUG_5 = 512,
            DEBUG_6 = 1024,
            DEBUG_7 = 2048,
            DEBUG_8 = 4096,
            DEBUG_9 = 8192
        };

    } // end namespace Logging
} // end namespace Cdmw


#endif // _CDMW_FDS_LOGGING_LOGLEVEL_HPP_
