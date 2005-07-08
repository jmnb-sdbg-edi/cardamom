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


#ifndef INCL_PLATFORMMNGT_PROCESS_LAUNCHER_HPP
#define INCL_PLATFORMMNGT_PROCESS_LAUNCHER_HPP

#include <string>

#include "Foundation/common/Exception.hpp"
#include "Foundation/ossupport/OS.hpp"


namespace Cdmw {        // Begin namespace Cdmw
namespace PlatformMngt { // Begin namespace PlatformMngt
 

   
/**
* Purpose:
* <p>Thrown the Processes ID Directory cannot be created.
*/
class CannotCreatePidDirectory  : public Exception {
    
    public:
        CannotCreatePidDirectory(const char* reason)
            : Exception(Exception::FATAL, reason) { }
};




/**
* Purpose:
* <p> This class launch processes and save their PID into a file 
* which is saved into the specified pidDirectory.
*/
class ProcessLauncher {

public:

    /**
    * Purpose:
    * <p> Option to be used with the Platform Agent to specify where the pids 
    * of lauched processes have to be saved.
    */
    static const char* PIDS_DIRECTORY_OPTION;

    

    /**
    * Purpose:
    * <p> Constructor
    */
    ProcessLauncher(const char* pidDirectory) 
        throw (CannotCreatePidDirectory);

    /**
    * Purpose:
    * <p> Destructor
    */
    virtual
    ~ProcessLauncher() 
        throw();
    
    
    /**
    * Purpose:
    * <p> Return the directory into which the PIDs of launched processes are saved.
    */
    const char* getPidDirectory() const throw()
    {
        return m_pidDirectory.c_str();
    }
    
    
    /**
    * Purpose:
    * <p> Launch a command and save the process ID into a <name>.pid file.
    * The path of the .pid file is returned into the pidFile argument.
    */
    OsSupport::OS::ProcessId
    create_process(const std::string& name, 
                   const std::string& path, 
                   const std::string& args, 
                   const std::string& home="",
                   OsSupport::ProcessCallback* creationCallback = NULL,
                   OsSupport::ProcessCallback* endingCallback = NULL)
        throw (OutOfMemoryException,
               InternalErrorException,
               BadParameterException);
    
    
    
    /**
    * Purpose:
    * <p> Kill the process which have been created with the specified name,
    * and delete its pid file.
    */
    void
    ProcessLauncher::kill_process(const std::string& name)
        throw (OutOfMemoryException,
               InternalErrorException);
      

   /**
    * Purpose:
    * <p> Remove the pid file created for specified process.
    * This operation doesn't kill the process.
    */
/*    void 
    remove_pid_file(const std::string& name)
        throw(OutOfMemoryException,
               InternalErrorException);
*/
           
   /**
    * Purpose:
    * <p> Kill all processes for which the pid has been saved 
    * into a file which is into the specified directory.
    */
    void
    kill_all_processes(const std::string& pidParentDirectory)
        throw (OutOfMemoryException,
               InternalErrorException);

private:

    /**
    * The directory into which the PIDs of launched processes are saved.
    */
    std::string m_pidDirectory;    
    
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_PROCESS_LAUNCHER_HPP

