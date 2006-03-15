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


#ifndef INCL_PLATFORMMNGT_PROCESS_KILL_MNGR_HPP 
#define INCL_PLATFORMMNGT_PROCESS_KILL_MNGR_HPP 

#include <string>
#include <map>

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "Foundation/idllib/FTPullMonitorable.stub.hpp"


namespace Cdmw
{

namespace PlatformMngt
{


class ProcessLauncher;

/**
*Purpose:
*<p>    This class is used as a singleton to kill processes created by daemon
*       or agents following a monitoring failure
*
*/
class ProcessKillMngr
{

    public:
	

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~ProcessKillMngr()
            throw();


	    
        /**
        * Purpose:
        * <p> Returns the ProcessKillMngr singleton
        * 
        *@return ProcessKillMngr singleton
	    *
        *@exception
        */ 
	    static
        ProcessKillMngr& Instance()
            throw ();


        /**
        * Purpose:
        * <p> Initialize the Process Kill Manager. This function must be called
        *   before any other call. The Process Kill Manager must be closed
        *   by a call to cleanup().
        * 
		*@param argc,argv the main parameter arguments
        *
        *@exception OutOfMemoryException
        *@exception BadParameterException
        */
        static
        void Init (int argc, char* argv[])
            throw (OutOfMemoryException,
                   BadParameterException);




        /**
        * Purpose:
        * <p> Free all allocated memory
        * 
        *@exception InternalErrorException
        */ 
        static
        void Cleanup()
            throw (InternalErrorException);
            
        /**
        * Purpose:
        * <p> register the process
        *
        *@param application_name the name of the application 
        *                        for processes started by agent
        *@param process_name     the name of the process
        *@param processId        the process id
        *
        *@exception OutOfMemoryException
        *@exception BadParameterException
        */    
        void register_process (const char* application_name,
                               const char* process_name,
                               OsSupport::OS::ProcessId processId)
            throw(OutOfMemoryException, 
                  InternalErrorException);
                  
        /**
        * Purpose:
        * <p> remove the process
        *
        *@param application_name the name of the application 
        *                        for processes started by agent
        *@param process_name     the name of the process
        *
        */    
        void remove_process (const char* application_name,
                             const char* process_name);
        
        
        /**
        * Purpose:
        * <p> remove the process
        *
        *@param processId the process id
        *
        */    
        void remove_process (OsSupport::OS::ProcessId processId);
        
        
        /**
        * Purpose:
        * <p> kill the process
        *
        *@param application_name the name of the application 
        *                        for processes started by agent
        *@param process_name     the name of the process 
        *
        */    
        void kill_process (const char* application_name,
                           const char* process_name);


    protected:
    

    private:
    
    
        /**
        * Purpose:
        * <p> Constructor
        * 
		*@param argc,argv the main parameter arguments
        *
        *@exception OutOfMemoryException
        *@exception BadParameterException      
        */ 
        ProcessKillMngr (int argc, char* argv[])
            throw (OutOfMemoryException,
				   BadParameterException);


    
    
    
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        ProcessKillMngr (const ProcessKillMngr& rhs)
                throw();


        /**
        * Purpose:
        * <p> Assignement operator
        * 
        */ 
        ProcessKillMngr& operator=(const ProcessKillMngr& rhs)
                throw();



        /**
        * This variable is set to true, since the init() function has been called.
        * The cleanup() function reset this variable to false.
        */
        static  
        bool M_init_done;

        /**
        * The singleton reference.
        */
        static
        ProcessKillMngr* M_pSingleton; 
         
        /**
        * The auto kill flag.
        */
        bool m_auto_kill;
        
        typedef std::map<std::string, OsSupport::OS::ProcessId> StartedProcesses;

        /**
        * The started processes.
        */
        StartedProcesses m_started_processes;

        /**
        * The mutex protecting concurrent access to m_started_processes.
        */
        OsSupport::Mutex m_started_processes_mutex;

        
}; // End class ProcessKillMngr 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PROCESS_KILL_MNGR_HPP

