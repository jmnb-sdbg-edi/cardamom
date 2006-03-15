/* =========================================================================== *
 * Copyright (c) 2003-2005 THALES All rights reserved.
 * Software commonly developed by THALES and AMS.
 *
 * This file is part of CARDAMOM.
 *
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with CARDAMOM; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */


#include "starttime/DaemonStarter.hpp"

#include <iostream>
#include <iomanip>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp>



namespace Cdmw
{


const std::string DaemonStarter::DAEMON_EXE = "cdmw_platform_daemon.sh";
const std::string DaemonStarter::DAEMON_CORBALOC = "corbaloc::localhost:21869/CdmwPlatformMngtDaemon";

    
DaemonStarter::DaemonStarter(const std::string& args,
                             CORBA::ORB_ptr orb)
    : ProcessStarter(DAEMON_EXE, args),
      m_orb(CORBA::ORB::_duplicate(orb))
{
}
    
    
    
bool DaemonStarter::check_if_started() 
    throw (ProcessError)
{
    try {
        CORBA::Object_var obj = m_orb->string_to_object(DAEMON_CORBALOC.c_str());
        
        CdmwPlatformMngt::Daemon_var daemon = 
            CdmwPlatformMngt::Daemon::_narrow(obj.in());
        
        daemon->is_alive();
        
        set_as_started();
        
    } catch (const CORBA::SystemException& e) {
        // not yet started        
    }
    
    return m_is_started;
}


} // namespace 

