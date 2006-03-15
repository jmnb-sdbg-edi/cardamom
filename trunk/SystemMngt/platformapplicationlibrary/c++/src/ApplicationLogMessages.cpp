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


#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


static const char* messages[] =
{
    "The agent of the application '%s' has crashed on the host '%s'",
    "The agent of the application '%s' already exists on the host '%s'",
    "The process '%s' of the application'%s' already exists on the host '%s'",

    "The host '%s' has crashed",
    "The host '%s' couldn't be found",
    "The host '%s' is unreachable",

    "The number of init steps is invalid",
    "The process is not allowed to end by itself",
    "Exception '%s' raised",
    "The process has reached the timeout expiration %u ms",
    "The process went out of memory",
    "The process is not ready to run",
    "The process failed while monitoring",
    "An unknown error has occured while killing the process '%s' on the host '%s'",
    "Unexpected step %d during the initialisation of the process '%s'",

    "The application entered in the degraded mode",
    "The application entered in the normal mode",

    "Defines the application '%s'",
    "Sets the init graph of the application '%s'",
    "Sets the stop graph of the application '%s'",
    "Initialises the application '%s'",
    "Resumes the initialisation of the application '%s'",
    "Acknowledges the application '%s'",
    "Runs the application '%s'",
    "Stops the application '%s'",
    "Starts the init sequencer of the application '%s'",
    "Resumes the init sequencer of the application '%s'",
    "Starts the run starter of the application '%s'",
    "Starts the stop sequencer of the application '%s'",

    "Adds the process '%s' to the application '%s'",
    "Removes the process '%s' from the application '%s'",
    "Initialises the process '%s' of the application '%s'",
    "Runs the process '%s' of the application '%s'",
    "Stops the process '%s' of the application '%s'",

    "Initialisation by sequencer of process '%s' of the application '%s'",
    "Next step processing by sequencer of process '%s' of the application '%s'",
    "Status OK for initialising process '%s' of the application '%s'",
    "Incompatible status for initialising process '%s' of the application '%s'",
    "Invalid step for initialising process '%s' of the application '%s'",
    "Incompatible type for initialising process '%s' of the application '%s'",
    "System exception '%s' \n   during initialisation of process '%s' of the application '%s'",
    "Run processing by starter of process '%s' of the application '%s'",
    "Status OK for running process '%s' of the application '%s'",
    "Incompatible status for running process '%s' of the application '%s'",
    "Process  '%s' of the application '%s' not ready to run",
    "Incompatible type for running process '%s' of the application '%s'",
    "System exception '%s' \n   during starting of process '%s' of the application '%s'",
    "Stop processing by sequencer of process '%s' of the application '%s'",
    "Status OK for stopping process '%s' of the application '%s'",
    "System exception '%s' \n   during stopping of process '%s' of the application '%s'",
    "Application Unexpected Exception '%s' for process '%s' of application '%s'",
    
    "Start to kill all agents of the application '%s'",
    "Start to kill agent of the application '%s' on the host '%s'",
    "Successfull killing of agent of the application '%s' on the host '%s'",
    "Cannot kill agent of the application '%s' on the host '%s'",
    "The Pull Monitorable of the process '%s' is nil",
    "The Pull Monitoring values of the process '%s' are incompatible",
    "Unexpected exception in the starting of Pull Monitoring for the process '%s'",
    
    "Add Entity \"%s\" for application \"%s\"",
    "Remove Entity \"%s\" for application \"%s\"",
    "Add Entity \"%s\" for process \"%s\",\"%s\",\"%s\"",
    "Remove Entity \"%s\" for process \"%s\",\"%s\",\"%s\""    
};


const char** ApplicationLogMessages::M_applicationMessages = messages;

} // End namespace PlatformMngt
} // End namespace Cdmw

