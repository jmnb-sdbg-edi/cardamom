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


#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/ossupport/ProcessCallback.hpp"
#include "SystemMngt/platformlibrary/ProcessLauncher.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include <sys/stat.h>
#include <errno.h>


namespace {

    static const char* CDMW_PID_FILE_SUFFIX = ".pid";
    static const char* CDMW_LAUNCH_PROCESS = "cdmw_platform_launch_process.sh";
    static const char* CDMW_KILL = "cdmw_platform_kill_pid.sh"; 
    static const char* CDMW_KILL_ALL_PIDS = "cdmw_platform_kill_all_pids.sh";

    /**
     * This EndingCallback is used to block a thread while a process is not finished.
     */
    class BlockingProcessCallback : public Cdmw::OsSupport::ProcessCallback {
        
        public:
            BlockingProcessCallback() : m_processEnded(false), m_condition(m_mutex) {};

            virtual ~BlockingProcessCallback() {};
            
            virtual void execute(Cdmw::OsSupport::OS::ProcessId processId) throw() {
                CDMW_MUTEX_GUARD(m_mutex);
                m_processEnded = true;
                try {
                    m_condition.broadcast();
                } catch (const Cdmw::InternalErrorException& ex) {
                    CDMW_ASSERT(false);
                }
            };
    
            void wait_callback() throw() {
                CDMW_MUTEX_GUARD(m_mutex);
                try {
                    while (!m_processEnded) m_condition.wait();
                } catch (const Cdmw::InternalErrorException& ex) {
                    CDMW_ASSERT(false);
                }
            };
    
        private:
            bool m_processEnded;
            Cdmw::OsSupport::Mutex m_mutex;
            Cdmw::OsSupport::Condition m_condition;
    };
    
    
    /**
     * This EndingCallback wraps the initial EndingCallback given when 
     * ProcessLauncher.create_process() is called.
     * When the process dies, this callback is called. First, it calls
     * the initial EndingCallback, then, it remove the pidFile of the process.
     */
    class PidFileCleaner: public Cdmw::OsSupport::ProcessCallback {
    
        public:
       
            PidFileCleaner(Cdmw::PlatformMngt::EndingCallback* initialEndingCallback,
                           const std::string& pidFile):
                m_initialEndingCallback(initialEndingCallback), m_pidFile(pidFile)
            {
            };
       
            virtual ~PidFileCleaner() {
            };
       
            virtual void execute(Cdmw::OsSupport::OS::ProcessId processId) throw() {
                
                // call initial EndingCallback
                if (m_initialEndingCallback != NULL) 
                {  
                    // if the callback is callable (active and counter > 0)                
		            if (m_initialEndingCallback->isCallable())
		            {
		                m_initialEndingCallback->execute(processId);
		            }
		          
		            // decrease the ending callback ref counter
		            // this ended process do not use it
		            bool is_deletable = m_initialEndingCallback->decreaseCounter();
		          
		            // if it can be deletable (inactive and counter = 0)
		            // we are the last user of callback
                    if (is_deletable)
                    {
                       m_initialEndingCallback = NULL;
                       delete m_initialEndingCallback;                       
                    }
                }

                try {
                    // remove pidFile
                    Cdmw::OsSupport::OS::unlink(m_pidFile.c_str());
                    std::string msg ("PidFile \"");
                    msg += m_pidFile;
                    msg += "\" cannot be removed : ";
                    msg += strerror(errno);
                    Cdmw::PlatformMngt::LogMngr::logMessage(Cdmw::PlatformMngt::WRN, msg.c_str());                    
                }
                catch (const Cdmw::BadParameterException &)
                { // ignore NOENT error: pid file could have been removed 
                  // by a script before unlink() is called.
                }                
                // delete this callback
                delete this;
            };
       
    
        private:
      //Cdmw::OsSupport::ProcessCallback* m_initialEndingCallback;
            Cdmw::PlatformMngt::EndingCallback* m_initialEndingCallback;
            std::string m_pidFile;
    };
    
};




namespace Cdmw {        // Begin namespace Cdmw
namespace PlatformMngt { // Begin namespace PlatformMngt

    
const char* ProcessLauncher::PIDS_DIRECTORY_OPTION = "--pidsDirectory";

    
ProcessLauncher::ProcessLauncher(const char* pidDirectory)
    throw (CannotCreatePidDirectory):
    m_pidDirectory(pidDirectory)
    
{
    try {
        // will not throw exception if EEXIST
        OsSupport::OS::mkdir(m_pidDirectory.c_str(), 0700); // mode 0700 = -rwx------
#ifdef CDMW_POSIX
        // if the directory is created, we change the owner
        std::cout<<"chown "<<m_pidDirectory.c_str()<<" "<<Cdmw::OsSupport::OS::get_uid()<<" "<<Cdmw::OsSupport::OS::get_gid()<<std::endl;
        Cdmw::OsSupport::OS::chown(m_pidDirectory.c_str(), 
                                   Cdmw::OsSupport::OS::get_uid(),
                                   Cdmw::OsSupport::OS::get_gid());
#endif
    }
    catch( const Cdmw::PermissionDeniedException &)
    {
        throw CannotCreatePidDirectory("PermissionDenied");
    }
    catch( const Cdmw::OsSupport::InterruptedException &)
    {
        throw CannotCreatePidDirectory("Interrupted");
    }
    catch( const Cdmw::BadParameterException &)
    {
        throw CannotCreatePidDirectory("BadParameter");
    }
    catch( const Cdmw::InternalErrorException &)
    {
        throw CannotCreatePidDirectory("InternalError");
    }
}


ProcessLauncher::~ProcessLauncher() throw()
{
}


OsSupport::OS::ProcessId
ProcessLauncher::create_process(const std::string& name, 
                                const std::string& path, 
                                const std::string& args, 
                                const std::string& home,
                                OsSupport::ProcessCallback* creationCallback,
                                //OsSupport::ProcessCallback* endingCallback)
                                EndingCallback* endingCallback)
    throw (OutOfMemoryException,
           InternalErrorException,
           BadParameterException)
{    
    // check validity of path and args
    OsSupport::OS::check_exec_validity(path, args);

    // pidFile into which the pid will be saved
    std::string pidFile = m_pidDirectory + "/" + name + CDMW_PID_FILE_SUFFIX;
    

    // final command to be launched
    std::string command = path + " " + args;
    
    // script to be launched: "CDMW_LAUNCH_PROCESS pidFile command"
    std::string launcherPath = CDMW_LAUNCH_PROCESS;
    std::string launcherArgs = pidFile + " " + command;
    
    // create PidFileCleaner wrapping the endingCallback
    PidFileCleaner* pidFileCleaner = new PidFileCleaner(endingCallback, pidFile);
    
    try {
        return OsSupport::OS::create_process(
                   launcherPath,
                   launcherArgs,
                   home,
                   creationCallback, 
                   pidFileCleaner);
    } catch (const BadParameterException& ex) {
        delete pidFileCleaner;
        std::string reason (CDMW_LAUNCH_PROCESS);
        reason += " cannot be found into PATH";
        CDMW_THROW1(InternalErrorException, reason);
    }
}



void
ProcessLauncher::kill_process(const std::string& name)
    throw (OutOfMemoryException,
           InternalErrorException)
{
    std::string killerPath = CDMW_KILL;
    std::string killerArgs = m_pidDirectory + "/" + name + CDMW_PID_FILE_SUFFIX;
    
    BlockingProcessCallback* endingCallback = new BlockingProcessCallback();
    
    try {
        OsSupport::OS::create_process(
            killerPath,
            killerArgs,
            "",
            NULL, 
            endingCallback);

        endingCallback->wait_callback();

    } catch (const BadParameterException& ex) {
        delete endingCallback;
        
        std::string reason (CDMW_KILL);
        reason += " cannot be found into PATH";
        CDMW_THROW1(InternalErrorException, reason);
    }
    
    delete endingCallback;
}

/*
void
ProcessLauncher::remove_pid_file(const std::string& name)
    throw(OutOfMemoryException,
          InternalErrorException)
{
}
*/


void
ProcessLauncher::kill_all_processes(const std::string& pidParentDirectory)
    throw (OutOfMemoryException,
           InternalErrorException)
{
    std::string killerPath = CDMW_KILL_ALL_PIDS;
    std::string killerArgs = m_pidDirectory + "/" + pidParentDirectory;
    
    try {
        OsSupport::OS::create_process(
            killerPath,
            killerArgs,
            "",
            NULL, 
            NULL);

    } catch (const BadParameterException& ex) {
        
        std::string reason (CDMW_KILL_ALL_PIDS);
        reason += " cannot be found into PATH";
        CDMW_THROW1(InternalErrorException, reason);
    }
    
}



}; // End namespace PlatformMngt
}; // End namespace Cdmw


