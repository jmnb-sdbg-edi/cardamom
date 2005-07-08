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


#ifndef INCL_PLATFORMADMIN_ADMIN_LOG_MESSAGE_ID_HPP 
#define INCL_PLATFORMADMIN_ADMIN_LOG_MESSAGE_ID_HPP


namespace Cdmw
{
namespace PlatformAdmin
{

class LoggedException : public Exception
{
public:
    LoggedException() : Exception( Exception::SAFE, "" )
    {};
};

enum AdminLogMessageIds
{
    /**
     * "Wrong command usage"
     */
    MSG_ID_ADMIN_COMMAND_USAGE,

    /**
     * "Log written in file \"%s\""
     */
    MSG_ID_ADMIN_LOG_FILE,

    /**
     * "An unknown error has occured"
     */
    MSG_ID_ADMIN_UNKNOWN_ERROR,

    /**
     * "Not allowed operation due to incompatible type"
     */
    MSG_ID_ADMIN_INCOMPATIBLE_TYPE,

    /**
     * "Not allowed operation due to incompatible status"
     */
    MSG_ID_ADMIN_INCOMPATIBLE_STATUS,

    
    /**
     * "Runset \"%s\" not found"
     */
    MSG_ID_ADMIN_RUNSET_NOT_FOUND,

    /**
     * "Host \"%s\" not found"
     */
    MSG_ID_ADMIN_HOST_NOT_FOUND,

    /**
     * "Host probe not found"
     */
    MSG_ID_ADMIN_HOST_PROBE_NOT_FOUND,

    /**
     * "Application \"%s\" not found"
     */
    MSG_ID_ADMIN_APPLICATION_NOT_FOUND,

    /**
     * "Application agent \"%s\" not found"
     */
    MSG_ID_ADMIN_APPLICATION_AGENT_NOT_FOUND,

    /**
     * "Process \"%s\" not found"
     */
    MSG_ID_ADMIN_PROCESS_NOT_FOUND,

    /**
     * "The host \"%s\" where the process \"%s\" must run has not been found"
     */
    MSG_ID_ADMIN_PROCESS_HOST_NOT_FOUND,

    /**
     * "The observer has not been found"
     */
    MSG_ID_ADMIN_OBSERVER_NOT_FOUND,

    /**
     * "The service \"%s\" has not been found"
     */
    MSG_ID_ADMIN_SERVICE_NOT_FOUND,

    /**
     * "The entity \"%s\" has not been found"
     */
    MSG_ID_ADMIN_ENTITY_NOT_FOUND,

    /**
     * "The monitoring point \"%s\" has not been found"
     */
    MSG_ID_ADMIN_MONITORING_POINT_NOT_FOUND,

    
    /**
     * "The host \"%s\" already exists"
     */
    MSG_ID_ADMIN_HOST_ALREADY_EXISTS,

    /**
     * "The host probe \"%s\" already exists"
     */
    MSG_ID_ADMIN_HOST_PROBE_ALREADY_EXISTS,

    /**
     * "The application \"%s\" already exists"
     */
    MSG_ID_ADMIN_APPLICATION_ALREADY_EXISTS,

    /**
     * "The application agent \"%s\" already exists"
     */
    MSG_ID_ADMIN_APPLICATION_AGENT_ALREADY_EXISTS,

    /**
     * "The process \"%s\" already exists"
     */
    MSG_ID_ADMIN_PROCESS_ALREADY_EXISTS,

    /**
     * "The entity \"%s\" already exists"
     */
    MSG_ID_ADMIN_ENTITY_ALREADY_EXISTS,

    /**
     * "The service \"%s\" already exists"
     */
    MSG_ID_ADMIN_SERVICE_ALREADY_EXISTS,


    /**
     * "The host \"%s\" is duplicated among the hosts"
     */
    MSG_ID_ADMIN_DUPLICATE_HOST,

    /**
     * "The application \"%s\" is duplicated among the applications"
     */
    MSG_ID_ADMIN_DUPLICATE_APPLICATION,

    /**
     * "The process \"%s\" is duplicated among the processes of the application\"%s\""
     */
    MSG_ID_ADMIN_DUPLICATE_PROCESS,

    /**
     * "The entity \"%s\" is duplicated among the entities of the system"
     */
    MSG_ID_ADMIN_DUPLICATE_ENTITY,

    /**
     * "The entity \"%s\" is duplicated among the entities of the application \"%s\""
     */
    MSG_ID_ADMIN_DUPLICATE_APPLICATION_ENTITY,

    /**
     * "The entity \"%s\" is duplicated among the entities of the process \"%s\""
     */
    MSG_ID_ADMIN_DUPLICATE_PROCESS_ENTITY,

    /**
     * "The service \"%s\" is duplicated among the services of the system"
     */
    MSG_ID_ADMIN_DUPLICATE_SERVICE,

    /**
     * "The service \"%s\" is duplicated among the services of the application \"%s\""
     */
    MSG_ID_ADMIN_DUPLICATE_APPLICATION_SERVICE,

    /**
     * "The service \"%s\" is duplicated among the services of the process \"%s\""
     */
    MSG_ID_ADMIN_DUPLICATE_PROCESS_SERVICE,

    /**
     * "The run set \"%s\" is duplicated among the runsets"
     */
    MSG_ID_ADMIN_DUPLICATE_RUNSET,

    /**
     * "The element \"%s\" already exists in the runset or among existing runsets"
     */
    MSG_ID_ADMIN_DUPLICATE_RUNSET_ELEMENT,

    /**
     * "The step label \"%s\" is duplicated among the steps"
     */
    MSG_ID_ADMIN_DUPLICATE_STEP,

    /**
     * "The step label \"%s\" is duplicated among the steps of the process \"%s\""
     */
    MSG_ID_ADMIN_DUPLICATE_PROCESS_STEP,

    /**
     * "The monitoring point \"%s\" is duplicated among the monitoring points"
     */
    MSG_ID_ADMIN_DUPLICATE_MONITORING_POINT,

    /**
     * "The monitoring point \"%s\" is duplicated among the monitoring points of the process \"%s\""
     */
    MSG_ID_ADMIN_DUPLICATE_PROCESS_MONITORING_POINT,

    
    /**
     * "The monitoring point \"%s\" is invalid"
     */
    MSG_ID_ADMIN_INVALID_MONITORING_POINT,

    /**
     * "The monitoring point \"%s\" of the process \"%s\" is invalid"
     */
    MSG_ID_ADMIN_INVALID_PROCESS_MONITORING_POINT,

    /**
     * "No other step to perform"
     */
    MSG_ID_ADMIN_INVALID_STEP,

    /**
     * "The process \"%s\" is not ready to run"
     */
    MSG_ID_ADMIN_NOT_READY_TO_RUN,

    /**
     * "The application \"%s\" is still active"
     */
    MSG_ID_ADMIN_APPLICATION_STILL_ACTIVE,

    /**
     * "The process \"%s\" is still active"
     */
    MSG_ID_ADMIN_PROCESS_STILL_ACTIVE,

    /**
     * "The entity name \"%s\" is invalid"
     */
    MSG_ID_ADMIN_INVALID_ENTITY_NAME,

    /**
     * "The service name \"%s\" is invalid"
     */
    MSG_ID_ADMIN_INVALID_SERVICE_NAME,

    /**
     * "The service \"%s\" is unavailable"
     */
    MSG_ID_ADMIN_SERVICE_UNAVAILABLE,



    /**
     * "The element \"%s\" doesn't refer to an existing process or application."
     */
    MSG_ID_ADMIN_REFERENCE_NOT_FOUND,

    /**
     * "The step %d of the process \"%s\" is out of bound."
     */
    MSG_ID_ADMIN_STEP_OUT_OF_BOUND,

    /**
     * "The graph contains a circular reference %s-%d."
     */
    MSG_ID_ADMIN_CIRCULAR_REFERENCE,

    /**
     * "The process \"%s\" jumps from the step %d to the step %d."
     */
    MSG_ID_ADMIN_STEP_JUMP,


    MSG_ID_ADMIN_END
};

} // End namespace PlatformAdmin
} // End namespace Cdmw


#endif // INCL_PLATFORMADMIN_ADMIN_LOG_MESSAGE_ID_HPP

