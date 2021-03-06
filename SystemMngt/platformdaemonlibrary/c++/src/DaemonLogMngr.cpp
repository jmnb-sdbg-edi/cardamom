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


#include "platformdaemonlibrary/DaemonLogMngr.hpp"
#include "SystemMngt/platformlibrary/CommonLogMessageId.hpp"
#include "platformdaemonlibrary/DaemonLogMessageId.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

    
const char* messages[] = {
            "Specified port %s is invalid",
            "Port not specified",
            "Invalid options",

            "Configuration file '%s' not found",
            "Configuration file not specified",
            "Invalid configuration file '%s':\nAt line %d :\n%s",

            "Daemon started on port %s with a timeout for notification call of %d ms",
            "Reference published with 'corbaloc::%s:%s/%s'",
            "Log written in file '%s'",
            "IOR written in file '%s'",

            "Cannot start daemon : port %s is already used",
            "Platform Daemon runs out of memory",
            "Internal error occured : %s",

            "Creating process '%s'",
            "Invalid info for creating process '%s' : \"%s\"",
            "Process '%s' created",
            "Kills process '%s'",
            "Process '%s' ended",

            "Creating platform application '%s'",
            "Invalid info for creating platform application '%s' : \"%s\"",
            "Platform application '%s' created",
            "Error reported by the platform application '%s' during its creation :\n%s",
            "Timeout for platform application '%s' has expired",
            "Kills platform application '%s'",
            "Platform application '%s' ended",

            "Creating agent for application '%s'",
            "Invalid info for creating agent for application '%s' : \"%s\"",
            "Agent for application '%s' created",
            "Error reported by the agent of the application '%s' during its creation :\n%s",
            "Timeout for agent of application '%s' has expired",
            "Kills agent of application '%s'",
            "Agent for application '%s' ended",

            "PIDs of launched processes will be saved into \"%s\" directory",
            "Cannot create ProcessesIdDirectory \"%s\" : %s",

            "Shutdowns",
            "Reboots",

            "Specified port %s is invalid",
            "Port not specified",
            "Invalid options",

            "Log file not specified",

            "Acknowledgement callback not specified",
            "Invalid acknowledgement callback",

            "Provided process observer is nil",

            "Agent for application '%s' has started on an available port (process id %d)",
            "Agent for application '%s' has started on port %d (process id %d)",
            "Log written in file '%s'",

            "Cannot start : port %d is already used",
            "Application agent runs out of memory",
            "Internal error occured",
            "Unexpected CORBA exception : \"%s\"",
            "Cannot report problem. Unable to reinitialise the ORB",

            "Invalid info for creating process '%s' : \"%s\"",

            "Creating process '%s'",
            "Process '%s' created",

            "Creating managed process '%s'",
            "Managed process '%s' created",
            "Error reported by the managed process '%s' during its creation :\n%s",
            "Timeout for the creation of the managed process '%s' has expired",

            "Exception reported when trying to pilot the created managed process '%s'\n%s",
            "Timeout when trying to pilot the created managed process '%s' has expired",
            "The created managed process '%s' is not ready to run",
            
            "Kills process '%s'",
            "Process '%s' ended",

            "Exception '%s' raised by application",
            
            "PIDs of processes launched by application '%s' will be saved into \"%s\" directory",

            "Cannot create ProcessesIdDirectory \"%s\" : (errno %s)",

#ifdef CDMW_USE_FAULTTOLERANCE
            "Provided fault notifier is nil",

            "Fault Manager corbaloc not specified",

            "Cannot create SystemMngtProxy : %s",
#endif

            "Exception raised during agent init!",

            "Unexpected statement reached after init!",


};

void DaemonLogMngr::initialise()
     throw (OutOfMemoryException)
{
    // if init not already done
    if (M_pSingleton == NULL)
    {
        try 
	    {
            // create the concrete Log Manager singleton
            M_pSingleton = new DaemonLogMngr ();
        }
        catch (const std::bad_alloc&)
	    {
          CDMW_THROW (OutOfMemoryException);
        }
    }
}


void DaemonLogMngr::initialise(const std::string& logFile)
     throw (OutOfMemoryException)
{
    // first initialises the logging manager with standard output
    initialise();
    
    // call the initialisation of basic logging manager for using a log file.
    LogMngr::open (logFile);
}

 
DaemonLogMngr::DaemonLogMngr()
        throw()
{
}
            
             
DaemonLogMngr::~DaemonLogMngr()
        throw()
{
}


const char* DaemonLogMngr::getService()
{
    return "CDMW_PFMDMN";
}

    
const char* DaemonLogMngr::getMessage(long messageId)
{
    if (messageId >= MSG_ID_COMMON_START && messageId < MSG_ID_COMMON_END)
    {
        return CommonLogMessages::M_commonMessages[messageId - MSG_ID_COMMON_START];
    }
    else if (messageId >= 0 && messageId < MSG_ID_DAEMON_END)
    {
        return messages[messageId];
    }
    else
    {
        return NULL;
    }
}


} // End namespace PlatformMngt
} // End namespace Cdmw


