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


#include "testplatformlibrary/TestLogMngr.hpp"

#include "SystemMngt/platformlibrary/CommonLogMessageId.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

void TestLogMngr::initialise()
     throw (OutOfMemoryException)
{
    // if init not already done
    if (M_pSingleton == NULL)
    {
        try 
	    {
            // create the concrete Log Manager singleton
            M_pSingleton = new TestLogMngr ();
        }
        catch (const std::bad_alloc&)
	    {
          CDMW_THROW (OutOfMemoryException);
        }
    }
}


void TestLogMngr::initialise(const std::string& logFile)
     throw (OutOfMemoryException)
{
    // first initialises the logging manager with standard output
    initialise();
    
    // call the initialisation of basic logging manager for using a log file.
    LogMngr::open (logFile);
}

 
TestLogMngr::TestLogMngr()
        throw()
{
}
            
             
TestLogMngr::~TestLogMngr()
        throw()
{
}


const char* TestLogMngr::getService()
{
    return "PFMTST";
}

    
const char* TestLogMngr::getMessage(long messageId)
{
    if (messageId >= MSG_ID_COMMON_START && messageId < MSG_ID_COMMON_END)
    {
        return CommonLogMessages::M_commonMessages [messageId - MSG_ID_COMMON_START];
    }
    else
    {
        return NULL;
    }
}


} // End namespace PlatformMngt
} // End namespace Cdmw


