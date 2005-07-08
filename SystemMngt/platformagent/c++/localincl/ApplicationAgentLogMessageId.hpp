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


#ifndef INCL_PLATFORMMNGT_AGENTLOGMESSAGEID_HPP 
#define INCL_PLATFORMMNGT_AGENTLOGMESSAGEID_HPP


namespace Cdmw
{
namespace PlatformMngt
{


enum ApplicationAgentLogMessageIds
{

    /**
    * "Specified port %s is invalid"
    */
    MSG_ID_AGENT_INVALID_PORT,

    /**
    * "Port not specified"
    */
    MSG_ID_AGENT_PORT_NOT_SPECIFIED,

    /**
    * "Invalid options"
    */
    MSG_ID_AGENT_INVALID_OPTIONS,

    /**
    * "Log file not specified"
    */
    MSG_ID_AGENT_LOG_FILE_NOT_SPECIFIED,

    /**
    * "Acknowledgement callback not specified"
    */
    MSG_ID_AGENT_CALLBACK_NOT_SPECIFIED,

    /**
    * "Invalid acknowledgement callback"
    */
    MSG_ID_AGENT_INVALID_CALLBACK,

    /**
    * "Provided process observer is nil"
    */
    MSG_ID_AGENT_PROCESS_OBSERVER_NIL,

    /**
    * "Agent for application '%s' has started on an available port"
    */
    MSG_ID_AGENT_STARTUP,

    /**
    * "Agent for application '%s' has started on port %d"
    */
    MSG_ID_AGENT_STARTUP_WITH_PORT,

    /**
    * "Log written in file '%s'"
    */
    MSG_ID_AGENT_LOG_FILE,

    /**
    * "Cannot start : port %d is already used"
    */
    MSG_ID_AGENT_UNAVAILABLE_PORT,

    /**
    * "Application agent runs out of memory"
    */
    MSG_ID_AGENT_NO_MEMORY,

    /**
    * "Internal error occured"
    */
    MSG_ID_AGENT_INTERNAL,

    /**
    * "Unexpected CORBA exception : \"%s\""
    */
    MSG_ID_AGENT_UNEXPECTED_CORBA_EXCEPTION,

    /**
    * "Cannot report problem. Unable to reinitialise the ORB"
    */
    MSG_ID_AGENT_CANNOT_REPORT_PROBLEM,

    /**
    * "Invalid info for creating process '%s' : \"%s\""
    */
    MSG_ID_AGENT_INVALID_PROCESS_INFO,

    /**
    * "Creating process '%s'"
    */
    MSG_ID_AGENT_CREATE_PROCESS,

    /**
    * "Process '%s' created"
    */
    MSG_ID_AGENT_PROCESS_CREATED,

    /**
    * "Creating managed process '%s'"
    */
    MSG_ID_AGENT_CREATE_MANAGED_PROCESS,

    /**
    * "Managed process '%s' created"
    */
    MSG_ID_AGENT_MANAGED_PROCESS_CREATED,

    /**
    * "Error reported by the managed process '%s' during its creation :\n%s"
    */
    MSG_ID_AGENT_MANAGED_PROCESS_ERROR,

    /**
    * "Timeout for the creation of the managed process '%s' has expired"
    */
    MSG_ID_AGENT_MANAGED_PROCESS_TIMEOUT,

    /**
    * "Kills process '%s'"
    */
    MSG_ID_AGENT_KILL_PROCESS,

    /**
    * "Process '%s' ended"
    */
    MSG_ID_AGENT_PROCESS_ENDED,

    /**
    * "Exception '%s' raised by application"
    */
    MSG_ID_AGENT_EXCEPTION_FROM_APPLICATION,

    
    /**
    * "PIDs of processes launched by application '%s' will be saved into \"%s\" directory"
    */
    MSG_ID_AGENT_PID_DIR_CREATED,
    
    /**
    * "Cannot create ProcessesIdDirectory \"%s\" : (errno %s)"
    */
    MSG_ID_AGENT_CANNOT_CREATE_PID_DIR,

#ifdef CDMW_USE_FAULTTOLERANCE
    /**
     * "Provided fault notifier is nil"
    */
    MSG_ID_AGENT_FAULT_NOTIFIER_NIL,
#endif
    
    /**
    * "Exception raised during agent init!"
    */
    MSG_ID_AGENT_EXCEPTION_DURING_INIT,
    
    /**
    * "Unexpected statement reached after init!"
    */
    MSG_ID_AGENT_UNEXPECTED_STATEMENT,
    
    MSG_ID_AGENT_END

};


} // End namespace PlatformMngt
} // End namespace Cdmw


#endif // INCL_PLATFORMMNGT_AGENTLOGMESSAGEID_HPP


