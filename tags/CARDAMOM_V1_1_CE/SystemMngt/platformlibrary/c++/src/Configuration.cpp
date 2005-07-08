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


#include <sstream>
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/ossupport/OS.hpp"

namespace Cdmw {        // Begin namespace Cdmw
namespace PlatformMngt { // Begin namespace PlatformMngt

// set static members
CORBA::ORB_var Configuration::M_orb = CORBA::ORB::_nil();
CdmwPlatformMngt::Timeout Configuration::M_timeout = 0;
    
    
CdmwPlatformMngt::Daemon_ptr Configuration::getDaemon (
                            const char* hostName,
                            const CdmwPlatformMngt::Port systemPort)
   throw (CdmwPlatformMngt::HostNotFound,
          CdmwPlatformMngt::HostNotReachable,
          CORBA::SystemException)
{
    try
    {
        std::ostringstream daemonCorbaloc;

        daemonCorbaloc 
            << "corbaloc::" << hostName
            <<  ":" << systemPort
            <<  "/" << CdmwPlatformMngt::DAEMON_NAME;

        CORBA::Object_var obj =
            M_orb->string_to_object (daemonCorbaloc.str().c_str());
            
        if (CORBA::is_nil (obj.in()))
        {
            throw CdmwPlatformMngt::HostNotFound();
        }
        
        
        CdmwPlatformMngt::Daemon_var daemon =
            CdmwPlatformMngt::Daemon::_narrow (obj.in());
            
        if (CORBA::is_nil (daemon.in()))
        {
            throw CdmwPlatformMngt::HostNotFound();
        }


        return daemon._retn();
    }
    catch (const CORBA::SystemException&)
    {
        throw CdmwPlatformMngt::HostNotReachable();
    }
}

} // End namespace PlatformMngt
} // End namespace Cdmw

