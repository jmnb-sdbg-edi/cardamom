/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#ifndef CARDAMOM_CLOCK_COS_CLOCK_HPP
#define CARDAMOM_CLOCK_COS_CLOCK_HPP

/**
 * 
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

#include "Foundation/orbsupport/OrbSupport.hpp"

#include <Time/clockservice/config.hpp>
#include <Time/clockservice/CosClockService.stub.hpp>
#include <Time/clockservice/CdmwTime.stub.hpp>

#include <memory>

/**
 * @namespace Cdmw
 * @brief This namespace is the root namespace of all CARDAMOM's API.
 */
namespace Cdmw  { 

    /**
     * @namespace Cdmw::clock
     * @brief This namespace is the root namespace of all EVoT API.
     * 
     * @note EVoT: Enhanced View Of Time
     */    
    namespace clock {

    /**
     * @namespace Cdmw::clock::CosClock
     * @brief This namespace contains the funtions required to init a 
     * EVoT service.
     */
    namespace   CosClock
    {
        /** 
         * Start service and allocate resources.
         * @param theOrb A reference to the ORB.
         * @param argc The argc from main.
         * @param argc The argv from main.
         * @return true if all is ok, false is some problem.
         */
        CLOCK_API   bool 
            init(CORBA::ORB_ptr theOrb,
            	 int argc, 
                 char *argv[]);

        /**
         * Cleanup correctly services.
         */
        CLOCK_API void
            close();

        /** 
         * Return a reference to POA.
         * @return The initialized POA.
         */
        CLOCK_API PortableServer::POA_ptr
            get_POA();
    }    

} /* clock */ } /* Cdmw */

#endif /* CARDAMOM_CLOCK_COS_CLOCK_HPP */

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
