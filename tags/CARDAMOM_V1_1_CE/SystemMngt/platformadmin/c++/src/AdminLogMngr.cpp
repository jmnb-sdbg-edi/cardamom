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


#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformadmin/AdminLogMngr.hpp"
#include "platformadmin/AdminLogMessageId.hpp"

namespace Cdmw
{
namespace PlatformAdmin
{

const char* messages[] =
{
    "Wrong command usage",
    "Log written into file \"%s\"",
    "An unknown error has occured",

    "Not allowed operation due to incompatible type",
    "Not allowed operation due to incompatible status",

    "Runset \"%s\" not found",
    "Host \"%s\" not found",
    "Host probe not found",
    "Application \"%s\" not found",
    "Application agent \"%s\" not found",
    "Process \"%s\" not found",
    "The host \"%s\" where the process \"%s\" must run has not been found",
    "The observer has not been found",
    "The service \"%s\" has not been found",
    "The entity \"%s\" has not been found",
    "The monitoring point \"%s\" has not been found",

    "The host \"%s\" already exists",
    "The host probe \"%s\" already exists",
    "The application \"%s\" already exists",
    "The application agent \"%s\" already exists",
    "The process \"%s\" already exists",
    "The entity \"%s\" already exists",
    "The service \"%s\" already exists",

    "The host \"%s\" is duplicated among the hosts",
    "The application \"%s\" is duplicated among the applications",
    "The process \"%s\" is duplicated among the processes of the application \"%s\"",
    "The entity \"%s\" is duplicated among the entities of the system",
    "The entity \"%s\" is duplicated among the entities of the application \"%s\"",
    "The entity \"%s\" is duplicated among the entities of the process \"%s\"",
    "The service \"%s\" is duplicated among the services of the system",
    "The service \"%s\" is duplicated among the services of the application \"%s\"",
    "The service \"%s\" is duplicated among the services of the process \"%s\"",
    "The run set \"%s\" is duplicated among the runsets",
    "The element \"%s\" already exists in the runset or among existing runsets",
    "The step label \"%s\" is duplicated among the steps",
    "The step label \"%s\" is duplicated among the steps of the process \"%s\"",
    "The monitoring point \"%s\" is duplicated among the monitoring points",
    "The monitoring point \"%s\" is duplicated among the monitoring points of the process \"%s\"",
    
    "The monitoring point \"%s\" is invalid",
    "The monitoring point \"%s\" of the process \"%s\" is invalid",
    "No other step to perform",
    "The process \"%s\" is not ready to run",
    "The application \"%s\" is still active",
    "The process \"%s\" is still active",
    "The entity name \"%s\" is invalid",
    "The service name \"%s\" is invalid",
    "The service \"%s\" is unavailable",

    "The element \"%s\" doesn't refer to an existing process or application.",
    "The step %d of the process \"%s\" is out of bound.",
    "The graph contains a circular reference %s-%d.",
    "The process \"%s\" jumps from the step %d to the step %d."
};


void AdminLogMngr::initialise()
     throw (OutOfMemoryException)
{
    // if init not already done
    if (M_pSingleton == NULL)
    {
        try 
	    {
            // create the concrete Log Manager singleton
            M_pSingleton = new AdminLogMngr ();
        }
        catch (const std::bad_alloc&)
	    {
          CDMW_THROW (OutOfMemoryException);
        }
    }
}


void AdminLogMngr::initialise(const std::string& logFile)
     throw (OutOfMemoryException)
{
    // first initialises the logging manager with standard output
    initialise();
    
    // call the initialisation of basic logging manager for using a log file.
    LogMngr::open (logFile);
}

 
AdminLogMngr::AdminLogMngr()
        throw()
{
}
            
             
AdminLogMngr::~AdminLogMngr()
        throw()
{
}


const char* AdminLogMngr::getService()
{
    return "CDMW_PFMADM";
}

    
const char* AdminLogMngr::getMessage(long messageId)
{
    if( (messageId >= 0) && (messageId < PlatformAdmin::MSG_ID_ADMIN_END) )
    {
        return PlatformAdmin::messages[ messageId ];
    }

    return NULL;
}

} // End namespace PlatformAdmin
} // End namespace Cdmw


