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


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "platformsystem/SystemLogMngr.hpp"
#include "SystemMngt/platformlibrary/CommonLogMessageId.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "platformsystem/SystemLogMessageId.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

static const char* messages[] =
{    
    "Invalid options",
    "Specified port %s is invalid",
    "Port not specified",
    "Cannot start supervision : port %s is already used",
    "Specified event notifier timeout %s is invalid",
    "Event notifier timeout not specified",
    "Specified creation timeout %s is invalid",
    "Creation timeout not specified",
    "Log file not specified",   
    "Supervision started on port %s",
    "Reference published with \"corbaloc::%s:%s/%s\"",
    "Log written in file \"%s\"",
    "RunSetManager runs out of memory",
    "Define System \"%s\"",
    "Add Application \"%s\" in system \"%s\"",
    "Remove Application \"%s\" in system \"%s\"",
    "Set Init Graph for system \"%s\"",
    "Set Stop Graph for system \"%s\"",
    "Set Run Set for system \"%s\"",
    "Remove Run Set for system \"%s\"",
    "Remove All Run Set for system \"%s\"",
    "Add Host for system \"%s\"",
    "Remove Host for system \"%s\"",
    "Start system \"%s\"",
    "Resume system \"%s\"",
    "Acknowledge system \"%s\"",
    "Stop system \"%s\"",
    "Start Application Init Sequencer",
    "Resume Application Init Sequencer",
    "Start Application Stop Sequencer",
    "Start RunSet \"%s\"",
    "Register Supervision Observer \"%s\" for system \"%s\"", 
    "Unregister Supervision Observer \"%s\" for system \"%s\"",
    "Initialisation by sequencer of application \"%s\"",
    "Status OK for initialising application \"%s\"",
    "Incompatible status for initialising application \"%s\"",
    "System exception '%s' \n   during initialisation of application \"%s\"",
    "Start processing by starter of application \"%s\"",
    "Status OK for starting application \"%s\"",
    "Incompatible status for starting application \"%s\"",
    "System exception '%s' \n   during starting of application \"%s\"",
    "Stop processing by sequencer of application \"%s\"",
    "Status OK for stopping application \"%s\"",
    "Incompatible status for stopping application \"%s\"",
    "System exception '%s' \n   during stopping of application \"%s\"",
    "System Unexpected Exception \"%s\" for application \"%s\""
};

const char** SystemLogMessages::M_systemMessages = messages;



void SystemLogMngr::initialise()
     throw (OutOfMemoryException)
{
    // if init not already done
    if (M_pSingleton == NULL)
    {
        try 
	    {
            // create the concrete Log Manager singleton
            M_pSingleton = new SystemLogMngr ();
        }
        catch (const std::bad_alloc&)
	    {
          CDMW_THROW (OutOfMemoryException);
        }
    }
}


void SystemLogMngr::initialise(const std::string& logFile)
     throw (OutOfMemoryException)
{
    // first initialises the logging manager with standard output
    initialise();
    
    // call the initialisation of basic logging manager for using a log file.
    LogMngr::open (logFile);
}

 
SystemLogMngr::SystemLogMngr()
        throw()
{
}
            
             
SystemLogMngr::~SystemLogMngr()
        throw()
{
}


const char* SystemLogMngr::getService()
{
    return CdmwPlatformMngtBase::PFMMNG;
}


//
// Get the specified text message associated to message ident
// This method is static and belongs to the logMngr class
// It is defined specifically for this system process
// It includes the common, application and system messages
//
//  @param messageId    the ident of message.
//
//  @return  the text of message.
//    
const char* SystemLogMngr::getMessage(long messageId)
{
    // if application message
    if ((messageId >= 0)  && (messageId < MSG_ID_APPLICATION_END))
    {
        return ApplicationLogMessages::M_applicationMessages [messageId];
    }
    // if system message
    else if ((messageId >= MSG_ID_SYSTEM_START) &&
             (messageId < MSG_ID_SYSTEM_END))
    {
        return SystemLogMessages::M_systemMessages [messageId - MSG_ID_SYSTEM_START];
    }
    // if common message
    else if ((messageId >= MSG_ID_COMMON_START) &&
             (messageId < MSG_ID_COMMON_END))
    {
        return CommonLogMessages::M_commonMessages [messageId - MSG_ID_COMMON_START];
    }
    // message id not found
    else
    {
        return NULL;
    }
}


} // End namespace PlatformMngt
} // End namespace Cdmw

