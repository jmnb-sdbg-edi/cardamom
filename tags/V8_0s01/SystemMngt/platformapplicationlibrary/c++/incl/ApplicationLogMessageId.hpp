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


#ifndef INCL_PLATFORMMNGT_APPLICATIONLOGMESSAGEID_HPP 
#define INCL_PLATFORMMNGT_APPLICATIONLOGMESSAGEID_HPP

namespace Cdmw
{
namespace PlatformMngt
{

enum ApplicationLogMessageIds
{
    /**
     * "The agent of the application '%s' has crashed on the host '%s'"
     */
    MSG_ID_AGENT_CRASHED,

    /**
     * "The agent of the application '%s' already exists on the host '%s'"
     */
    MSG_ID_AGENT_EXISTS,

    /**
     * "the process '%s' of the application '%s' already exists on the host '%s'"
     */
    MSG_ID_AGENT_PROCESS_EXISTS,


    /**
     * "The host '%s' has crashed"
     */
    MSG_ID_HOST_CRASHED,

    /**
     * "The host '%s' couldn't be found"
     */
    MSG_ID_HOST_NOT_FOUND,

    /**
     * "The host '%s' is unreachable"
     */
    MSG_ID_HOST_UNREACHABLE,



    /**
     * "The number of init steps is invalid"
     */
    MSG_ID_PROCESS_INVALID_STEP_NUMBER,

    /**
     * "The process is not allowed to end by itself"
     */
    MSG_ID_PROCESS_ENDING_ERROR,

    /**
     * "The process received the exception '%s'"
     */
    MSG_ID_PROCESS_EXCEPTION,

    /**
     * "The process has reached the timeout expiration %lu ms"
     */
    MSG_ID_PROCESS_TIMEOUT,

    /**
     * "The process went out of memory"
     */
    MSG_ID_PROCESS_MEMORY,

    /**
     * "The process is not ready to run"
     */
    MSG_ID_PROCESS_NOT_READY_TO_RUN,

    /**
     * "The process failed while monitoring"
     */
    MSG_ID_PROCESS_MONITORING_FAILURE,

    /**
     * "An unknown error has occured while killing the process '%s' on the host '%s'"
     */
    MSG_ID_PROCESS_KILL_ERROR,

    /**
     * "Unexpected step %d during the initialisation of the process '%s'"
     */
    MSG_ID_PROCESS_UNEXPECTED_STEP,



    /**
     * "The application entered in the degraded mode"
     */
    MSG_ID_APPLICATION_DEGRADED_MODE,

    /**
     * "The application entered in the normal mode"
     */
    MSG_ID_APPLICATION_NORMAL_MODE,



    /**
     * "Defines the application '%s'"
     */
    MSG_ID_APPLICATION_DEFINE,

    /**
     * "Sets the init graph of the application '%s'"
     */
    MSG_ID_APPLICATION_SET_INIT_GRAPH,

    /**
     * "Sets the stop graph of the application '%s'"
     */
    MSG_ID_APPLICATION_SET_STOP_GRAPH,

    /**
     * "Initialises the application '%s'"
     */
    MSG_ID_APPLICATION_INITIALISE,

    /**
     * "Resumes the initialisation of the application '%s'"
     */
    MSG_ID_APPLICATION_RESUME,

    /**
     * "Acknowledges the application '%s'"
     */
    MSG_ID_APPLICATION_ACKNOWLEDGE,

    /**
     * "Runs the application '%s'"
     */
    MSG_ID_APPLICATION_RUN,

    /**
     * "Stops the application '%s'"
     */
    MSG_ID_APPLICATION_STOP,

    /**
     * "Starts the init sequencer of the application '%s'"
     */
    MSG_ID_APPLICATION_START_INIT_SEQUENCER,

    /**
     * "Resumes the init sequencer of the application '%s'"
     */
    MSG_ID_APPLICATION_RESUME_INIT_SEQUENCER,

    /**
     * "Starts the run starter of the application '%s'"
     */
    MSG_ID_APPLICATION_START_RUN_STARTER,

    /**
     * "Starts the stop sequencer of the application '%s'"
     */
    MSG_ID_APPLICATION_START_STOP_SEQUENCER,



    /**
     * "Adds the process '%s' to the application '%s'"
     */
    MSG_ID_PROCESS_ADD,

    /**
     * "Removes the process '%s' from the application '%s'"
     */
    MSG_ID_PROCESS_REMOVE,

    /**
     * "Initialises the process '%s' of the application '%s'"
     */
    MSG_ID_PROCESS_INITALISE,

    /**
     * "Runs the process '%s' of the application '%s'"
     */
    MSG_ID_PROCESS_RUN,

    /**
     * "Stops the process '%s' of the application '%s'"
     */
    MSG_ID_PROCESS_STOP,
    
    
    /**
     * "Initialisation of process by sequencer"
     */
    MSG_ID_APP_PROC_INIT,
    
    /**
     * "Next step processing of process by sequencer"
     */
    MSG_ID_APP_PROC_NEXTSTEP,
    
    /**
     * "Status ok for initialising process"
     */
    MSG_ID_APP_PROC_INIT_STATUS_OK,
    
    /**
     * "Incompatible status for initialising process"
     */
    MSG_ID_APP_PROC_INIT_STATUS_ERR,


    /**
     * "Invalid step for initialising process"
     */
    MSG_ID_APP_PROC_INIT_STEP_ERR,
    
    
    /**
     * "Incompatible type for initialising process"
     */
    MSG_ID_APP_PROC_INIT_TYPE_ERR,
    
    
    /**
     * "System exception during initialisation of process"
     */
    MSG_ID_APP_PROC_INIT_SYSTEM_ERR,


    /**
     * "Run processing of process by starter"
     */
    MSG_ID_APP_PROC_RUN,
    
    /**
     * "Status ok for running process"
     */
    MSG_ID_APP_PROC_RUN_STATUS_OK,
    
    /**
     * "Incompatible status for running process"
     */
    MSG_ID_APP_PROC_RUN_STATUS_ERR,


    /**
     * "Process Not Ready to run"
     */
    MSG_ID_APP_PROC_RUN_NOT_READY_ERR,
    
    
    /**
     * "Incompatible type for running process"
     */
    MSG_ID_APP_PROC_RUN_TYPE_ERR,
    
    
    /**
     * "System exception during starting of process"
     */
    MSG_ID_APP_PROC_RUN_SYSTEM_ERR,


    /**
     * "Stop processing of process by sequencer"
     */
    MSG_ID_APP_PROC_STOP,
    
    /**
     * "Status ok for stopping process"
     */
    MSG_ID_APP_PROC_STOP_STATUS_OK,
    
    /**
     * "System exception during stopping of process"
     */
    MSG_ID_APP_PROC_STOP_SYSTEM_ERR,


    /**
     * "Unexpected Exception"
     */
    MSG_ID_APP_UNEXPECTED_EXCEPTION,
    
    /**
     * "Start to kill all agents of the application"
     */
    MSG_ID_APPLICATION_START_KILL_AGENTS,
    
    /**
     * "Start to kill agent of the application on the host"
     */
    MSG_ID_APP_START_KILL_AGENT_HOST,
    
    /**
     * "Successfull killing of agent of the application on the host"
     */
    MSG_ID_APP_KILL_AGENT_OK,
    
    /**
     * "Cannot kill agent of the application on the host"
     */
    MSG_ID_APP_KILL_AGENT_FAIL,
    
    /**
     * "The Pull Monitorable of the process '%s' is nil"
     */
    MSG_ID_PULL_MONITORING_NIL_ERROR,
    
    /**
     * "The Pull Monitoring values of the process '%s' are incompatible"
     */
    MSG_ID_PULL_MONITORING_VALUE_ERROR,
    
    /**
     * "Unexpected exception in the starting of Pull Monitoring for the process '%s'"
     */
    MSG_ID_PULL_MONITORING_UNEXPECTED_ERROR,
    
    /**
     * "Add Entity \"%s\" for application \"%s\""
     */
    MSG_ID_ADD_APPLICATION_ENTITY,
     
    /**
     * "Remove Entity \"%s\" for application \"%s\""
     */
    MSG_ID_REMOVE_APPLICATION_ENTITY,
     
    /**
     * "Add Entity \"%s\" for process \"%s\",\"%s\",\"%s\""
     */
    MSG_ID_ADD_PROCESS_ENTITY,
     
    /**
     * "Remove Entity \"%s\" for process \"%s\",\"%s\",\"%s\""
     */
    MSG_ID_REMOVE_PROCESS_ENTITY,
    

    MSG_ID_APPLICATION_END
};


class ApplicationLogMessages
{
public:
    static const char** M_applicationMessages;
};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_APPLICATIONLOGMESSAGEID_HPP

