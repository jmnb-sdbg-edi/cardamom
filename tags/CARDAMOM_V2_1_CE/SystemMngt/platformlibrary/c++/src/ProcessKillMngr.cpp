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


#include <iostream>
#include <sstream>


#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/System.hpp"

#include "Foundation/osthreads/MutexGuard.hpp"

#include "SystemMngt/platformlibrary/ProcessKillMngr.hpp"

namespace
{
    const std::string NO_KILL_BY_MONITORING_OPTION = "--noKillByMonitoring";
    
}; // End anonymous namespace


namespace Cdmw
{

namespace PlatformMngt
{

 
/**
* This variable is set to true, when the ProcessKillMngr::Init() function
* has been called.
*/
bool ProcessKillMngr::M_init_done = false;

/**
* Contains the ProcessKillMngr singleton initialised by the 
* ProcessKillMngr::Init() function.
*/
ProcessKillMngr* ProcessKillMngr::M_pSingleton = NULL;



//
// Ctor
//
//
ProcessKillMngr::ProcessKillMngr (int argc, char* argv[])
    throw (OutOfMemoryException,
		   BadParameterException)  
{
    m_auto_kill = true;
    
    // checks the no kill option by monitoring
    std::string no_kill_option =
        OsSupport::OS::get_option_value(argc, argv, NO_KILL_BY_MONITORING_OPTION);
  
    if (no_kill_option == "yes")
    {
        m_auto_kill = false;
    }
}



//
// Initialise the ProcessKillerManager
// 
void ProcessKillMngr::Init (int argc, char* argv[])
    throw (OutOfMemoryException,
           BadParameterException)
{

    // The user try to initialise twice the collector
    CDMW_ASSERT (M_init_done==false);

    
    try 
	{
        M_pSingleton = new ProcessKillMngr(argc, argv);
    } 
	catch (const std::bad_alloc&)
	{
      CDMW_THROW (OutOfMemoryException);
    } 

    M_init_done = true;
}




//
// Free memory and terminate the Trace service
//
void ProcessKillMngr::Cleanup()
    throw (InternalErrorException)
{
    // The library is not initialised
    CDMW_ASSERT (M_init_done==true);

    M_init_done = false;
    delete M_pSingleton;
    
    M_pSingleton = NULL;
}



//
// Dtor
// 
ProcessKillMngr::~ProcessKillMngr()
    throw()
{
    // The user has not called the Cleanup function
    CDMW_ASSERT (M_init_done == false);
}




ProcessKillMngr& ProcessKillMngr::Instance()
    throw ()
{
    // The user has not called the Init() function
    CDMW_ASSERT (M_init_done == true);
    
    return *M_pSingleton ;
}

void ProcessKillMngr::register_process (const char* application_name,
                                          const char* process_name,
                                          OsSupport::OS::ProcessId processId)
    throw(OutOfMemoryException, 
          InternalErrorException)
{
    CDMW_MUTEX_GUARD(m_started_processes_mutex);
    
    std::string process_key = application_name;
    process_key += "/";
    process_key += process_name;
    
    try
    {
        m_started_processes[process_key] = processId;
    }
    catch(const std::bad_alloc&)
    {
        CDMW_THROW(OutOfMemoryException);
    }
}

   
void ProcessKillMngr::remove_process (const char* application_name,
                                        const char* process_name)
{
    CDMW_MUTEX_GUARD(m_started_processes_mutex);
    
    std::string process_key = application_name;
    process_key += "/";
    process_key += process_name;

    m_started_processes.erase(process_key);

}
  
void ProcessKillMngr::remove_process (OsSupport::OS::ProcessId processId)
{
    CDMW_MUTEX_GUARD(m_started_processes_mutex);
    
    StartedProcesses::iterator it;
    bool found = false;

    for (it = m_started_processes.begin(); it != m_started_processes.end(); ++it)
    {
        if (processId == it->second)
        {
            found = true;
            break;
        }
    }
    
    if (found)
    {
        m_started_processes.erase(it);
    }
}

    
void ProcessKillMngr::kill_process (const char* application_name,
                                      const char* process_name)
{
    CDMW_MUTEX_GUARD(m_started_processes_mutex);
    
    std::string process_key = application_name;
    process_key += "/";
    process_key += process_name;

    StartedProcesses::iterator it = m_started_processes.find(process_key);

    if (it != m_started_processes.end())
    {
        
        if (m_auto_kill)
        {
            std::cout << "<<ProcessKillMngr::kill_process ('" 
                      << application_name << "','" << process_name
                      << "'>>"
                      << std::endl;

            try
            {
                OsSupport::OS::kill_process(it->second);
            }
            catch(...){}
            
        }
        
        m_started_processes.erase(it);
    }
}




}; // End namespace PlatformMngt
}; // End namespace Cdmw

