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


#include "Foundation/orbsupport/CORBA.hpp"

#include "SystemMngt/platformlibrary/Configuration.hpp"

#include "platformsystem/DaemonMonitor.hpp"
#include "platformsystem/System_impl.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

/**
*Implementation notes: 
*<p>
*Portability issues:  none 
*<p>
*/





//
// Ctor
//    
DaemonMonitor::DaemonMonitor (System_impl* system,
                              const char* hostName,
                              unsigned long monitoringInterval,
                              unsigned long monitoringTimeout)
    throw(BadParameterException)
    : PullMonitor (CdmwPlatformMngt::PullMonitorable::_nil(), monitoringInterval, monitoringTimeout)
      
{
    if (system == NULL)
    {
        CDMW_THROW2 (BadParameterException, "system", "NULL");
    }
     
    // Set the internal data
    m_system = system;
    m_hostName = CORBA::string_dup (hostName);
    m_daemon = CdmwPlatformMngt::Daemon::_nil();
}




//
// Dtor
//
DaemonMonitor::~DaemonMonitor()
    throw()
{
}





//
// get pull monitable
//
const CdmwPlatformMngt::PullMonitorable_ptr DaemonMonitor::get_pullMonitorable()
            throw()
{
    // set return object to nil 
    CdmwPlatformMngt::Daemon_var daemon;
    
    if (CORBA::is_nil (m_daemon.in()))
    {
        try
        {
            // Get the (local or remote) daemon
            m_daemon = Configuration::getDaemon (m_hostName.in(),                  
                                                 m_system->systemPort());
                                                 
            daemon = CdmwPlatformMngt::Daemon::_duplicate (m_daemon.in());
        }
        catch (const CdmwPlatformMngt::HostNotFound&)
        {
            // return nil daemon object
        }
        catch (const CdmwPlatformMngt::HostNotReachable&)
        {
            // return nil daemon object
        }
        catch (...)
        {
            // return nil daemon object
        }
    }
    
    else
    {    
        daemon = CdmwPlatformMngt::Daemon::_duplicate (m_daemon.in());
    }
           
    return daemon._retn();
}
            

} // End namespace PlatformMngt
} // End namespace Cdmw

