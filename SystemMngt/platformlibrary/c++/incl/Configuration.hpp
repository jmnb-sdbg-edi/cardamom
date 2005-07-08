/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#ifndef INCL_PLATFORMMNGT_CONFIGURATION_HPP
#define INCL_PLATFORMMNGT_CONFIGURATION_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"

namespace Cdmw {        // Begin namespace Cdmw
namespace PlatformMngt { // Begin namespace PlatformMngt

/**
 *Purpose:
 *<p>
 */
class Configuration
{
    
public:

    /**
     * The ORB.
     */
    static
    CORBA::ORB_var M_orb;

    /**
     * The timeout needed for technical reasons.
     */
    static
    CdmwPlatformMngt::Timeout M_timeout;

public:

    /**
     *Purpose:
     * Returns the specified (remote or not) daemon.
     * 
     *@param host_name  the host of the remote daemon.
     *@param systemPort the port to contact daemon
     *
     *@exception HostNotFound if the host cannot be found.
     *@exception HostNotReachable if the host doesn't respond.
     *@exception CORBA::SystemException
     */
    static
    CdmwPlatformMngt::Daemon_ptr getDaemon (
                                    const char* hostName,
                                    const CdmwPlatformMngt::Port systemPort)
            throw (CdmwPlatformMngt::HostNotFound,
                   CdmwPlatformMngt::HostNotReachable,
                   CORBA::SystemException);
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_CONFIGURATION_HPP

