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


#ifndef INCL_PLATFORMMNGT_DAEMON_MONITOR_HPP 
#define INCL_PLATFORMMNGT_DAEMON_MONITOR_HPP 

#include <string>
#include <iostream>
#include <fstream>


#include "SystemMngt/platformlibrary/PullMonitor.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW PlatformMngt.
*/
namespace PlatformMngt
{

class System_impl;


/**
*Purpose:
*<p>    This class implement the Pull Monitoring of Platform Daemon
*
*Features:
*<p> Thread safe, exception safe
*
*
*/
class DaemonMonitor : public PullMonitor
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param system    the system responsible for the host. 
        *@param hostName  the Host name
        *@param monitoringInterval monitoring interval used to reactivate the thread in msec
        *@param monitoringTimeout  timeout used by DaemonMonitoringCall object
        *                          to wait response of DaemonMonitorable object during polling
        *
        *@exception BadParameterException if the parameter is invalid.
        *
        */ 
        DaemonMonitor (System_impl* system,
                       const char* hostName,
                       unsigned long monitoringInterval,
                       unsigned long monitoringTimeout)
                throw(BadParameterException);


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~DaemonMonitor()
                throw();


    protected:


        /**
        * Purpose:
        * <p> return the pullMonitorable object
        * 
        */ 
        virtual
        const CdmwPlatformMngt::PullMonitorable_ptr get_pullMonitorable()
            throw();
        
        
        
    private:

        //
        // Copy ctor
        //
        DaemonMonitor (const DaemonMonitor& rhs)
                throw();




        //
        // Assignement operator
        //
        DaemonMonitor& operator=(const DaemonMonitor& rhs)
                 throw();



        /**
        * The system responsible for the entity.
        */
        System_impl* m_system;
        
        
        /**
        * The Host name
        */
        CdmwPlatformMngt::HostName_var m_hostName;
        
        
        /**
        * The Host daemon persistent object reference
        */
        CdmwPlatformMngt::Daemon_var m_daemon;


}; // End class DaemonMonitor 

} // End namespace PlatformMngt
} // End namespace Cdmw


#endif // INCL_PLATFORMMNGT_DAEMON_MONITOR_HPP

