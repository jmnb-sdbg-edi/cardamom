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


#ifndef INCL_PLATFORMMNGT_DAEMONLOGMESSAGEID_HPP 
#define INCL_PLATFORMMNGT_DAEMONLOGMESSAGEID_HPP


namespace Cdmw
{
namespace PlatformMngt
{


enum DaemonLogMessageIds
{

    /**
    * "Specified port %s is invalid"
    */
    MSG_ID_DAEMON_INVALID_PORT,

    /**
    * "Port not specified"
    */
    MSG_ID_DAEMON_PORT_NOT_SPECIFIED,

    /**
    * "Invalid options"
    */
    MSG_ID_DAEMON_INVALID_OPTIONS,

    /**
    * "Configuration file '%s' not found"
    */
    MSG_ID_DAEMON_CONF_FILE_NOT_FOUND,

    /**
    * "Configuration file not specified"
    */
    MSG_ID_DAEMON_CONF_FILE_NOT_SPECIFIED,

    /**
    * "Invalid configuration file '%s':\nAt line : %d:\n%s"
    */
    MSG_ID_DAEMON_INVALID_CONF_FILE,

    /**
    * "Daemon started on port %s with a timeout for notification call of %d ms"
    */
    MSG_ID_DAEMON_STARTUP,

    /**
    * "Reference published with 'corbaloc::%s:%s/%s'"
    */
    MSG_ID_DAEMON_CORBALOC,

    /**
    * "Log written in file '%s'"
    */
    MSG_ID_DAEMON_LOG_FILE,

    /**
    * "IOR written in file '%s'"
    */
    MSG_ID_DAEMON_IOR_FILE,

    /**
    * "Port %s is already used"
    */
    MSG_ID_DAEMON_UNAVAILABLE_PORT,

    /**
    * "Platform Daemon runs out of memory"
    */
    MSG_ID_DAEMON_NO_MEMORY,

    /**
    * "Internal error occured : %s"
    */
    MSG_ID_DAEMON_INTERNAL,

    /**
    * "Creating process '%s'"
    */
    MSG_ID_DAEMON_CREATE_PROCESS,

    /**
    * "Invalid info for creating process '%s' : \"%s\""
    */
    MSG_ID_DAEMON_INVALID_PROCESS_INFO,

    /**
    * "Process '%s' created"
    */
    MSG_ID_DAEMON_PROCESS_CREATED,

    /**
    * "Kills process '%s'"
    */
    MSG_ID_DAEMON_KILL_PROCESS,

    /**
    * "Process '%s' ended"
    */
    MSG_ID_DAEMON_PROCESS_ENDED,

    /**
    * "Creating platform application '%s'"
    */
    MSG_ID_DAEMON_CREATE_PLATFORM_APPLICATION,

    /**
    * "Invalid info for creating platform application '%s' : \"%s\""
    */
    MSG_ID_DAEMON_INVALID_PLATFORM_APPLICATION_INFO,

    /**
    * "Platform application '%s' created"
    */
    MSG_ID_DAEMON_PLATFORM_APPLICATION_CREATED,

    /**
    * "Error reported by the platform application '%s' during its creation :\n%s"
    */
    MSG_ID_DAEMON_PLATFORM_APPLICATION_ERROR,

    /**
    * "Timeout for platform application '%s' has expired"
    */
    MSG_ID_DAEMON_PLATFORM_APPLICATION_TIMEOUT,

    /**
    * "Kills platform application '%s'"
    */
    MSG_ID_DAEMON_KILL_PLATFORM_APPLICATION,

    /**
    * "Platform application '%s' ended"
    */
    MSG_ID_DAEMON_PLATFORM_APPLICATION_ENDED,

    /**
    * "Creating agent for application '%s'"
    */
    MSG_ID_DAEMON_CREATE_APPLICATION_AGENT,

    /**
    * "Invalid info for creating agent for application '%s' : \"%s\""
    */
    MSG_ID_DAEMON_INVALID_APPLICATION_AGENT_INFO,

    /**
    * "Agent for application '%s' created"
    */
    MSG_ID_DAEMON_APPLICATION_AGENT_CREATED,

    /**
    * "Error reported by the agent of the application '%s' during its creation :\n%s"
    */
    MSG_ID_DAEMON_APPLICATION_AGENT_ERROR,

    /**
    * "Timeout for agent of application '%s' has expired"
    */
    MSG_ID_DAEMON_APPLICATION_AGENT_TIMEOUT,

    /**
    * "Kills agent of application '%s'"
    */
    MSG_ID_DAEMON_KILL_APPLICATION_AGENT,

    /**
    * "Agent for application '%s' ended"
    */
    MSG_ID_DAEMON_APPLICATION_AGENT_ENDED,

    /**
    * "Creating host probe"
    */
    MSG_ID_DAEMON_CREATE_HOST_PROBE,

    /**
    * "Invalid info for creating host probe : \"%s\""
    */
    MSG_ID_DAEMON_INVALID_HOST_PROBE_INFO,

    /**
    * "Host probe created"
    */
    MSG_ID_DAEMON_HOST_PROBE_CREATED,

    /**
    * "Error reported by the host probe during its creation :\n%s"
    */
    MSG_ID_DAEMON_HOST_PROBE_ERROR,

    /**
    * "Timeout for host probe has expired"
    */
    MSG_ID_DAEMON_HOST_PROBE_TIMEOUT,

    /**
    * "Kills host probe"
    */
    MSG_ID_DAEMON_KILL_HOST_PROBE,

    /**
    * "Host probe ended"
    */
    MSG_ID_DAEMON_HOST_PROBE_ENDED,

    /**
    * "PIDs of launched processes will be saved into \"%s\" directory"
    */
    MSG_ID_DAEMON_PID_DIR_CREATED,

    /**
    * "Cannot create ProcessesIdDirectory \"%s\" : (errno %s)"
    */
    MSG_ID_DAEMON_CANNOT_CREATE_DIR,

    
    /**
    * "Shutdowns"
    */
    MSG_ID_DAEMON_SHUTDOWN,

    /**
    * "Reboots"
    */
    MSG_ID_DAEMON_REBOOT,

    MSG_ID_DAEMON_END

    
};


} // End namespace PlatformMngt
} // End namespace Cdmw


#endif // INCL_PLATFORMMNGT_DAEMONLOGMESSAGEID_HPP


