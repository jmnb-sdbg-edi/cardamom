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

#ifndef _CDMW_FT_LOGGER_HPP_
#define _CDMW_FT_LOGGER_HPP_

#include <Foundation/logging/Logger.hpp>
#include <Foundation/common/Assert.hpp>


namespace Cdmw {
namespace FT {


    class FTLogger
    {
    public:
        
        /**
         * Constructor
         */       

        FTLogger()
        {}
        
        /**
         * Destructor
         */
        ~FTLogger()
        {}

        
        /**
         * FLT Domain for Logs
         */
        static const char* FLT_LOG_DOMAIN;

        /**
         * Set the client_id
         *
         * @param orb the orb.
         * @param client_id the client_id for the orb.
         *
         * @return void 
         */
        inline static Cdmw::Logging::Logger * GetLogger() throw () 
        {
            static Cdmw::Logging::Logger * Logger__ = Cdmw::Logging::Logger::Get_Logger(FLT_LOG_DOMAIN);
            CDMW_ASSERT(Logger__ != 0);
            return Logger__;
        }
        
    };    
    

 
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_LOGGER_HPP_

