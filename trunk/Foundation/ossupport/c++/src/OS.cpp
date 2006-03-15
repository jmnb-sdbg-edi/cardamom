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


// To get ::realpath() on sparc environment
#ifdef sun
#    define __EXTENSIONS__
#endif


#include "Foundation/ossupport/OS.hpp"
#include "ossupport/ProcessMonitor.hpp"
#include "ossupport/Which.hpp"


#include <time.h>
#include <cerrno>
#include <cstdio>
#include <iostream>
#include <sstream>


#if defined (CDMW_POSIX)
#    include <sys/types.h>
#    include <sys/utsname.h>
#    include <sys/wait.h>

// FIXME: gettimeofday not available under posix
#    undef _POSIX_C_SOURCE
#    include <stdlib.h>
#    include <unistd.h>
#    include <sys/time.h>
#    include <signal.h>

#elif defined(_WIN32)

#   include <windows.h>
#   include <io.h>
#   include <winsock2.h>
#   include <direct.h>
#endif

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/Barrier.hpp"
#include "Foundation/ossupport/ProcessCallback.hpp"
#include "ossupport/ProcessNotifierThread.hpp"
#include "Foundation/ossupport/SharedObjectHandle.hpp"
#include "Foundation/osthreads/Thread.hpp"


#ifdef linux
extern "C" char *realpath (const char * __name,
                                 char * __resolved);
#endif

namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Implementation notes:  none 
        *<p>
        *Portability issues:  none 
        *<p>
        */

#ifdef sun
        typedef DefaultSharedObject EffectiveSharedObject;
#elif linux
        //typedef DefaultSharedObject EffectiveSharedObject;
        typedef LinuxSharedObject EffectiveSharedObject;
#elif AIX
        typedef AIXSharedObject EffectiveSharedObject;
#elif _WIN32
        typedef Win32DllObject EffectiveSharedObject;
#endif


        ProcessNotifierThread* OS::M_pProcessNotifierThread = NULL;
        Mutex OS::M_processNotifierThread_mutex;

        MonitorGarbageCollector* OS::M_garbageCollector = NULL;
        Mutex OS::M_garbageCollector_mutex;

        int OS::M_uid = -1;
        int OS::M_gid = -1;

#if defined(linux)
        OS::CreateProcessPolicy OS::M_createProcessPolicy = OS::CREATE_BY_THE_CALLER;
//        OS::CreateProcessPolicy OS::M_createProcessPolicy = OS::CREATE_BY_MONITOR_THREADS;
#else
        OS::CreateProcessPolicy OS::M_createProcessPolicy = OS::CREATE_BY_THE_CALLER;
#endif

#if defined(_WIN32)


        //
        // Retreive from a Process ID, a win32 handle
        //
        HANDLE
        get_win32_process_handle(OS::ProcessId pid)
        throw ( PermissionDeniedException,
                BadParameterException,
                InternalErrorException
              )
        {

            // We need to get the handle to the process
            // TODO: maybe we can only require right we need ?
            HANDLE process_handle = ::OpenProcess (PROCESS_ALL_ACCESS,
                                                   FALSE,
                                                   pid);

            if (process_handle == NULL)
            {

                switch (::GetLastError())
                {

                        case ERROR_ACCESS_DENIED:
                        CDMW_THROW( PermissionDeniedException );
                        break;


                        // FIXME
                        // I don't know the error code returned, when a pid is not
                        // valid, so now I suppose it is the current problem

                        default:
                        CDMW_THROW2(BadParameterException, "processId", "No process with this id");
                        break;

                }

            }

            return process_handle;

        }

#endif



        //
        // Return the of the option specified in the option parameter
        //    if the option is not on the command line "no" is returned
        //
        std::string
        OS::get_option_value(int argc, char** argv, const std::string& option)
        throw ( OutOfMemoryException,
                InternalErrorException )
        {
            std::string option_value = "no";

            try
            {
                int iArg(1);


                // We scan all arguments to find the matching with option

                while (iArg < argc)
                {

                    std::string arg (argv[iArg]);

                    // We check if in the current argument we have, the option

                    if (arg.find(option) != std::string::npos)
                    {

                        std::string::size_type pos_sep = arg.find('=');

                        // We have no separator the option is only here so
                        //    we return yes, or if the equal is not followed
                        //    by something

                        if ( ( pos_sep == std::string::npos ) || (pos_sep + 1 == (arg.size()) ))
                        {

                            option_value = "yes";

                            // Else we return the value
                        }
                        else
                        {

                            option_value = arg.substr(pos_sep + 1);
                        }

                    }

                    iArg++;
                }

            }
            catch ( const std::bad_alloc& )
            {

                CDMW_THROW( OutOfMemoryException );

            }
            catch ( ... )
            {

                CDMW_THROW( InternalErrorException );

            }

            return option_value;
        }




        //
        // Make sleep the calling thread during X milliseconds
        //
        void
        OS::sleep(unsigned int milliseconds)
        throw( InterruptedException,
               InternalErrorException)
        {
            Thread::sleep(milliseconds);
        }

        // 
        // Extended version
        //
        void
        OS::sleep(unsigned int seconds,
              unsigned int nanoseconds)
            throw (std::range_error)
        {
            timespec ts;
            ts.tv_sec  = seconds;
            ts.tv_nsec = nanoseconds;

            if (::nanosleep(&ts, 0) != 0)
                throw std::range_error(strerror(errno));
        }

        //
        // Return true if the specified file exists
        //
        bool
        OS::file_exists(const std::string& path)
        throw()
        {
            bool exists(false);
            
#ifdef _WIN32

            if ( 0 == ::_access (path.c_str(), 00))
            {
                exists = true;
            }

#else
            if ( 0 == ::access (path.c_str(), F_OK))
            {
                exists = true;
            }

#endif

            return exists;
        }




        //
        // Delete the specified file
        //
        void
        OS::unlink(const std::string& path)
        throw( PermissionDeniedException,
               InterruptedException,
               BadParameterException,
               InternalErrorException)
        {

            if ( 0 != ::unlink( path.c_str()) )
            {

#       if defined(CDMW_POSIX)

                switch (errno)
                {

                        // The file is a mount point

                        case EBUSY:
                        // No break;

                        // The file is a directory

                        case EPERM:
                        // No break;

                        // The file is being executed

                        case ETXTBSY:
                        // No break;

                        // The file system of the file is read-only

                        case EROFS:
                        // No break;

                        // The user is not allowed

                        case EACCES:

                        CDMW_THROW(PermissionDeniedException);
                        break;

                        case ENOENT:
                        CDMW_THROW2(BadParameterException, "path", "file not exists");
                        break;

                        // Interrupted by a signal

                        case EINTR:
                        CDMW_THROW(InterruptedException);
                        break;

                        // Too many symbolic link

                        case ELOOP:
                        CDMW_THROW2(BadParameterException, "path", "too many symbolic link");
                        break;

                        // Path exceeds MAX_PATH

                        case ENAMETOOLONG:
                        CDMW_THROW2(BadParameterException, "path", "path too long");
                        break;


                        case ENOLINK:
                        CDMW_THROW2(BadParameterException, "path", "file on a remote machine down");
                        break;

                        case ENOTDIR:
                        CDMW_THROW2(BadParameterException, "path", "a path component is not a directory");
                        break;

                        // The path point on an illegal adress

                        case EFAULT:
                        // No break;

                        default:
                        CDMW_THROW(InternalErrorException);
                        break;
                }

#       elif defined(_WIN32)

                switch (errno)
                {

                        // The user is not allowed

                        case EACCES:

                        CDMW_THROW(PermissionDeniedException);
                        break;

                        case ENOENT:
                        CDMW_THROW2(BadParameterException, "path", "file not exists");
                        break;

                        default:
                        CDMW_THROW(InternalErrorException);
                        break;
                }

#       endif

            }
        }


        void
        OS::chdir(const std::string& path)
        throw( BadParameterException,
               PermissionDeniedException,
               InterruptedException,
               InternalErrorException)
        {

#if defined(CDMW_POSIX)

            int status = ::chdir (path.c_str());

            if (status != 0)
            {

                switch (errno)
                {

                        case EACCES:
                        CDMW_THROW(PermissionDeniedException);
                        break;

                        case EINTR:
                        CDMW_THROW(InterruptedException);
                        break;

                        case ENAMETOOLONG:
                        // No break

                        case ELOOP:
                        CDMW_THROW2(BadParameterException, "path", "path too long");
                        break;

                        case ENOTDIR:
                        // No break

                        case ENOLINK:
                        // No break

                        case ENOENT:
                        CDMW_THROW2(BadParameterException, "path", "invalid path");
                        break;


                        case EIO:
                        // No break

                        case EFAULT:
                        // No break

                        default:
                        CDMW_THROW(InternalErrorException);
                        break;
                }

            }

#elif defined(_WIN32)
            int status = ::_chdir (path.c_str());

            if (status != 0)
            {
                CDMW_THROW2(BadParameterException, "path", "invalid path");
            }

#endif

        }

        void
        OS::mkdir(const std::string& path, unsigned short mode )
        throw( BadParameterException,
               PermissionDeniedException,
               InterruptedException,
               InternalErrorException)
        {

#if defined(CDMW_POSIX)

            int status = ::mkdir (path.c_str(), mode);

            if (status != 0)
            {

                switch (errno)
                {
                        case EROFS:
                        // No break

                        case ENOSPC:
                        // No break

                        case EACCES:
                        CDMW_THROW(PermissionDeniedException);
                        break;

                        case EEXIST:
                        // ignore
                        break;

                        case EINTR:
                        CDMW_THROW(InterruptedException);
                        break;

                        case ENAMETOOLONG:
                        // No break

                        case ELOOP:
                        CDMW_THROW2(BadParameterException, "path", "path too long");
                        break;

                        case ENOTDIR:
                        // No break

                        case ENOENT:
                        CDMW_THROW2(BadParameterException, "path", "invalid path");
                        break;

                        case EFAULT:
                        // No break

                        default:
                        CDMW_THROW(InternalErrorException);
                        break;
                }

            }

#elif defined(_WIN32)
            int status = ::_mkdir (path.c_str(), mode);

            if (status != 0)
            {
                CDMW_THROW2(BadParameterException, "path", "invalid path");
            }

#endif

        }


        std::string
        OS::get_absolute_path (const std::string& path)
        throw( BadParameterException,
               PermissionDeniedException,
               NotExistException,
               InternalErrorException)
        {
            
#if defined(CDMW_POSIX)

            char buffer[PATH_MAX + 1];

            // The System call doesn't guarantee the NULL terminator, so
            //  we must insert it

            if (::realpath( path.c_str(), buffer) == NULL)
            {

                switch (errno)
                {

                        case EACCES:
                        CDMW_THROW(PermissionDeniedException);
                        break;

                        case ENAMETOOLONG:
                        // No break

                        case ELOOP:
                        CDMW_THROW2(BadParameterException, "path", "path too long");
                        break;

                        case ENOTDIR:
                        // No break

                        case ENOLINK:
                        // No break

                        case ENOENT:
                        CDMW_THROW(NotExistException);
                        break;


                        case EIO:
                        // No break

                        case EFAULT:
                        // No break

                        default:
                        CDMW_THROW(InternalErrorException);
                        break;
                }


            }

            return buffer;


#elif defined(_WIN32)

            char buffer[_MAX_PATH];

            if (_fullpath( buffer, path.c_str(), _MAX_PATH) != NULL)
            {

                return buffer;

            }
            else
            {

                CDMW_THROW2(BadParameterException, "path", "invalid path");

            }

#else
#   error Cdmw: not supported platform
#endif


        }


        //
        // Wait until the completion of the specified process
        //
        OS::StatusProcess
        OS::wait_process (OS::ProcessId processId)
        throw ( BadParameterException,
                InterruptedException,
                InternalErrorException)
        {

            return 0;

            /*
            #if defined(CDMW_POSIX)
             
                int status(0);
                
                pid_t status_waitpid = ::waitpid(    processId, 
                                                     &status, 
                                                     0);
                if ( static_cast<pid_t>(-1) ==  status_waitpid ) {
                    
                    switch ( errno ) {
             
                     case ECHILD:
                      CDMW_THROW2(BadParameterException,"processId", "Process does not exist");
                   break;
              
                  case EINTR:
                      CDMW_THROW(InterruptedException);
                      break;
                  
                  // Option was not valid, impossible we get corrupted
                  case EINVAL:
                      // No break;
               
                  default:
                      CDMW_THROW(InternalErrorException);
                   break;
                  
                    };
             }
             
             return static_cast<OS::StatusProcess>(status);
                
             
             
            #elif defined(_WIN32)      
             
             
                HANDLE process_handle = get_win32_process_handle( processId );
                
                DWORD status_wait = ::WaitForSingleObject( process_handle,INFINITE );
             
                if ( status_wait != WAIT_OBJECT_0 ) {
             
                    CDMW_THROW(InternalErrorException);
             
                }
             
                DWORD exitCode;
                if ( ::GetExitCodeProcess( process_handle, &exitCode) == false ) {
             
                    CDMW_THROW(InternalErrorException);
             
                }
             
                return static_cast<OS::StatusProcess>(exitCode);
                
             
            #endif
            */
        }


#if defined(CDMW_POSIX)

        void OS::set_process_attributes(uid_t uid, gid_t gid)
        {
            M_uid = uid;
            M_gid = gid;
        }



        uid_t OS::get_uid()
        {
            return M_uid;
        }
        
        gid_t OS::get_gid()
        {
            return M_gid;
        }
        
        
        void OS::chown(const std::string& path, uid_t uid, gid_t gid)
        throw (PermissionDeniedException,
               BadParameterException,
               OutOfMemoryException,
               InternalErrorException)
        {
            int res = ::chown(path.c_str(), uid, gid);
            if (res != 0)
            {
                switch (errno)
                {
                    
                    // The system ressource are exhausted
                    
                    case EPERM:
                        // No break

                    case EACCES:
                        // No break
                        
                    case EROFS:
                        
                        CDMW_THROW(PermissionDeniedException);
                        break;

                    case ENOTDIR:
                        CDMW_THROW2(BadParameterException, "path", "File not found");
                        break;

                    default:
                        CDMW_THROW(InternalErrorException);
                        break;
                }
            }
        }
        
        

#endif


        //
        // Create a new process from the specified executable file
        //
        OS::ProcessId
        OS::create_process (const std::string& path, const std::string& args,
                            const std::string& home,
                            ProcessCallback* creationCallback,
                            ProcessCallback* endingCallback)
        throw (BadParameterException,
               OutOfMemoryException,
               InternalErrorException)
        {

	  ProcessId childPid = 0;



#if defined(CDMW_POSIX)

            // check argument validity
            check_exec_validity (path, args);


            if (M_createProcessPolicy == CREATE_BY_THE_CALLER)
            {
                // create if necessary, the thread in charge of monitoring child
                create_processNotifierThread();


                // create the process
                childPid = _create_process (path, args, home,
                                            creationCallback, endingCallback);

                // call creation callback

                if (creationCallback != NULL)
                {
                    creationCallback->execute(childPid);
                }

                // Add the process to the list of to be monitored process
                M_pProcessNotifierThread->add_process(childPid, endingCallback);

            }

            else if (M_createProcessPolicy == CREATE_BY_MONITOR_THREADS)
            {
                // create the garbage collector if necessary
                create_MonGarbageCollectorThread();


                // create the ProcessMonitor associated to the new process
                std::auto_ptr <ProcessMonitorStarter> pProcessMonitorStarter (
                    new ProcessMonitorStarter (
                        path, args, home,
                        creationCallback,
                        endingCallback));

                // create the new process
                childPid = (pProcessMonitorStarter.get())->createProcess();
            }


#elif defined(_WIN32)


            // create the process
            childPid = _create_process (path, args, home,
                                        creationCallback, endingCallback);


#else
#    error "Cdmw: implementation not defined"
#endif


            return childPid;
        }



        //
        // Actually create a new process from the specified executable file
        //
        OS::ProcessId
        OS::_create_process(const std::string& path, const std::string& args,
                            const std::string& home,
                            ProcessCallback* creationCallback,
                            ProcessCallback* endingCallback)
        throw (BadParameterException,
               OutOfMemoryException,
               InternalErrorException)
        {

            ProcessId childPid;


#if defined(CDMW_POSIX)

            pid_t fork_return = fork();

            switch (fork_return)
            {

                    // We are in the child

                    case 0:
                    {

                        if (M_uid > 0 && M_gid > 0)
                        {

                            if (::setgid(M_gid) != 0)
                            {
                                ::perror("setgid error");
                                ::_exit(EXIT_FAILURE);
                            }

                            if (::setuid(M_uid) != 0)
                            {
                                ::perror("setuid error");
                                ::_exit(EXIT_FAILURE);
                            }

                        }

                        std::string path_to_start = path;

                        // If a home directory has been specified, we must go into

                        if (home != "")
                        {
                            try
                            {
                                try
                                {
                                    // We must get the absolute path to the process
                                    // from its relative path
                                    path_to_start = OS::get_absolute_path (path);
                                }
                                catch (const NotExistException &ex)
                                {
                                    // if path is not found by get_absolute_path
                                    // it will be found by exec_process using the PATH environment
                                    // path has been checked by check_exec_validity at the beginning
                                    // of create_process
                                    // so we can continue
                                }
                                
                                // change the working directory
                                OS::chdir (home);
                            }
                            
                            
                            catch (const Exception &ex)
                            {
                                {
                                    std::ostringstream msg;
                                
                                    msg << "exception in child process : " << std::endl
                                        << ex.what() << std::endl;
                                    ::perror(msg.str().c_str());
                                }
                                ::_exit(EXIT_FAILURE);
                            }
                        }

                        // exec the new process
                        exec_process (path_to_start, args);
                    }

                    break;

                    // Error
                    case static_cast<pid_t>( -1):
                    {

                        switch (errno)
                        {

                                // The system ressource are exhausted

                                case EAGAIN:
                                // No break

                                // There is not enough memory

                                case ENOMEM:

                                CDMW_THROW(OutOfMemoryException);
                                break;

                                default:
                                CDMW_THROW(InternalErrorException);
                                break;
                        }

                    }


                    // The process is created, we are in the father

                    default:
                    break;

            }

            childPid = fork_return;



#elif defined(_WIN32)

            // create if necessary, the thread in charge of monitoring child
            create_processNotifierThread();


            STARTUPINFO si;
            PROCESS_INFORMATION pi;

            memset(&si, 0, sizeof(STARTUPINFO));
            si.cb = sizeof(STARTUPINFO);

            char* cmdLine = new char[path.length() + args.length() + 2];
            strcpy (cmdLine, path.c_str());
            strcat (cmdLine, " ");
            strcat (cmdLine, args.c_str());

            // We specify a current directory if required
            const char* home_dir;

            if (home != "")
            {
                home_dir = home.c_str();
            }
            else
            {
                home_dir = NULL;
            }

            BOOL status_create = ::CreateProcess (NULL,              // We ask to use PATH variable
                                                  cmdLine,
                                                  NULL, NULL,        // No security parameters
                                                  false,             // We don't inherit current handle
                                                  CREATE_NEW_CONSOLE,
                                                  NULL,              // Same environment as us
                                                  home_dir,          // Same current directory
                                                  &si,               // Info to design the new window
                                                  &pi                // info about the created process
                                                );

            if (status_create == FALSE)
            {

                DWORD lastError = ::GetLastError();

                switch (lastError)
                {


                        // This is not an executable file

                        case ERROR_BAD_EXE_FORMAT:
                        CDMW_THROW2(BadParameterException, "path", "execution permission denied");
                        break;


                        case ERROR_FILE_NOT_FOUND:

                        case ERROR_PATH_NOT_FOUND:
                        CDMW_THROW2(BadParameterException, "path", "File not found");
                        break;


                        case ERROR_NO_PROC_SLOTS:
                        CDMW_THROW(OutOfMemoryException);
                        break;

                        case ERROR_PROCESS_ABORTED:
                        // No break;

                        default:
                        CDMW_THROW(InternalErrorException);

                }
            }

            delete[] cmdLine;



            childPid = pi.dwProcessId;

            if (creationCallback != NULL)
            {
                creationCallback->execute(childPid);
            }

            M_pProcessNotifierThread->add_process (pi, endingCallback);


#else
#    error "Cdmw: implementation not defined"
#endif


            return childPid;
        }



        //
        // Kill a process
        //
        void
        OS::kill_process (OS::ProcessId processId)
        throw (BadParameterException,
               PermissionDeniedException,
               InternalErrorException)
        {
            
#if defined(CDMW_POSIX)

#ifdef CDMW_USE_GCOV
            int status_kill = ::kill (processId, SIGTERM);
#else
            int status_kill = ::kill (processId, SIGKILL);
#endif

            if ( status_kill == -1 )
            {

                switch ( errno )
                {

                        // The user has not enough right

                        case EPERM:
                        CDMW_THROW(PermissionDeniedException);
                        break;

                        // The process id does not exist

                        case ESRCH:
                        CDMW_THROW2(BadParameterException, "processId", "No process with this id");
                        break;


                        // The signale value is not valid: impossible

                        case EINVAL:
                        // No break;

                        default:
                        CDMW_THROW(InternalErrorException);
                        break;

                };
            }

#elif defined(_WIN32)

            HANDLE process_handle = get_win32_process_handle (processId);

            BOOL status_terminate = ::TerminateProcess (process_handle, -1);

            if (status_terminate == FALSE)
            {

                CDMW_THROW(InternalErrorException);

            }

#else
#    error "Cdmw: implementation not defined"
#endif


        }




        //
        // Return the id of the process
        //
        OS::ProcessId
        OS::get_processId()
        {
#if defined(CDMW_POSIX)

            pid_t pid = ::getpid();

#elif defined(_WIN32)

            DWORD pid = ::GetCurrentProcessId();


#else
#    error "Cdmw: implementation not defined"
#endif

            return pid;

        }





        //
        // This function is in charge to create the ProcessNotifier
        //
        void OS::create_processNotifierThread()
        throw()
        {
            try
            {
                CDMW_MUTEX_GUARD(M_processNotifierThread_mutex);

                if (M_pProcessNotifierThread == NULL)
                {

                    M_pProcessNotifierThread = new ProcessNotifierThread();
                    M_pProcessNotifierThread->start();

                    // We register the cleanup function
                    ::atexit (cleanup_processNotifierThread);

                }
            }
            catch (...)
            {
                CDMW_ASSERT(false);
            }
        }


        void OS::cleanup_processNotifierThread()
        throw()
        {
            CDMW_MUTEX_GUARD(M_processNotifierThread_mutex);
            if (M_pProcessNotifierThread != NULL) {
// PCR-0562 workaround: don't delete M_pProcessNotifierThread
// In ProcessNotifierThread destructor, a join() operation could block
// indefinitely, as the thread could be blocked on ::wait() call (if the 
// process doesn't have child process anymore.
// But the ProcessNotifierThread cannot be deleted without a join(), else,
// an assert occurs in Thread destructor if it's still running.
// Memory leak to be corrected with PCR-0xxx
//                delete M_pProcessNotifierThread;
                M_pProcessNotifierThread = NULL;
            }
        }



        //
        // This function is in charge to create the Monitor Garbage Collector
        //
        void OS::create_MonGarbageCollectorThread()
        throw()
        {

            try
            {
                CDMW_MUTEX_GUARD(M_garbageCollector_mutex);
                if (M_garbageCollector == NULL)
                {
                    M_garbageCollector = new MonitorGarbageCollector();
                    M_garbageCollector->start();

                    // We register the cleanup function
                    ::atexit(cleanup_MonGarbageCollectorThread);
                }

            }
            catch (...)
            {
                CDMW_ASSERT(false);
            }
        }


        void OS::cleanup_MonGarbageCollectorThread()
        throw()
        {
            if (M_garbageCollector != NULL)
            {
                MonitorGarbageCollector* garbageCollector = M_garbageCollector;

                {
                    CDMW_MUTEX_GUARD(M_garbageCollector_mutex);

                    M_garbageCollector = NULL;
                }

                garbageCollector->stop();
                delete garbageCollector;
            }
        }


        void OS::addStartedProcessMonitor(ProcessMonitor* p_processMonitor)
        throw(OutOfMemoryException)
        {
            CDMW_MUTEX_GUARD(M_garbageCollector_mutex);

            if (M_garbageCollector != NULL)
            {
                M_garbageCollector->addStartedProcessMonitor(p_processMonitor);
            }
        }


        void OS::addTerminatedProcessMonitor(ProcessMonitor* p_processMonitor)
        throw(OutOfMemoryException)
        {
            CDMW_MUTEX_GUARD(M_garbageCollector_mutex);

            if (M_garbageCollector != NULL)
            {
                M_garbageCollector->addTerminatedProcessMonitor(p_processMonitor);
            }
        }


        //
        // Set the policy of process creation
        //
        void
        OS::setUpCreateProcessPolicy (CreateProcessPolicy policy)
        {
            M_createProcessPolicy = policy;
        }



        //
        // Check the validity of the arguments used to create a new process
        //
        void
        OS::check_exec_validity( const std::string& path, const std::string args)
        throw ( BadParameterException )
        {

#ifdef CDMW_POSIX

            // Argument list too long

            if ( args.size() > ARG_MAX )
            {
                CDMW_THROW2(BadParameterException, "args", "argument list too long");
            }

            // File name too long
            if ( path.size() > PATH_MAX )
            {
                CDMW_THROW2(BadParameterException, "path", "file path too long");
            }


            if ( path_search(path) == 0)
            {
                CDMW_THROW2(BadParameterException, "path", "file not found, not executable or permission denied ");
            }


            // Check the existence of the file path
            if ( -1 == ::access( path.c_str(), F_OK ) )
            {
                //CDMW_THROW2(BadParameterException,"path","file not found");
            }

            // Check if we have execution rights
            if ( -1 == ::access( path.c_str(), X_OK ) )
            {
                //CDMW_THROW2(BadParameterException,"path","file is not executable or permission denied");
            }

#endif

        }




#if defined(CDMW_POSIX) 
        //
        // Used after a fork() under unix to replace the process image
        //    with a new image
        //
        void
        OS::exec_process (const std::string& path, const std::string& args)
        throw()
        {

            using std::vector;
            using std::string;
            using Cdmw::Common::String;

            // The first argument (argv[0]), must be the process path, or at least
            //    the basename of the path, so we concat arguments with the path
            //    to get it.
            vector<string> parsed_args = String::to_strings( path + " " + args);

            char** ppArgs = String::to_char_array(parsed_args);

            if ( -1 == ::execvp (path.c_str(), ppArgs))
            {

                switch (errno)
                {

                        case E2BIG:

                        case EACCES:

                        case EAGAIN:

                        case EFAULT:

                        case EINTR:

                        case ELOOP:

                        case ENAMETOOLONG:

                        case ENOENT:

                        case ENOEXEC:

                        case ENOLINK:

                        case ENOMEM:

                        case ENOTDIR:

                        default:
                        // Sorry we can not propogate the error to the caller, because
                        // we are already in a new process.
                        // We only display the error
                        ::perror ("Cdmw::OsSupport::OS::create_process()");

                        // Be careful not exit() but _exit()
                        // See in documentation the difference
                        ::_exit (errno);
                        break;

                }
            }



        }

#elif defined(_WIN32)

        // Nothing

#else
#    error "Cdmw: implementation not defined"
#endif



        //
        // Return the name of the current host in short notation
        //
        std::string
        OS::get_hostname()
        throw( InternalErrorException )
        {
            
#if defined(CDMW_POSIX)

            struct utsname uts_info;
            int status = ::uname (&uts_info);

            if (status == -1)
            {
                CDMW_THROW( InternalErrorException );
            }
#if defined(CDMW_USE_CANONICAL_HOSTNAME)
            std:: string res(uts_info.nodename);
#ifdef _GNU_SOURCE
            std::string domainname(uts_info.domainname);
            if (!domainname.empty()) {
                res +='.';
                res += uts_info.domainname;
            }
#endif
            return res;
#else
            return uts_info.nodename;
#endif

#elif defined(_WIN32)

            // Request a winsock version library
            // in fact the version is not important for us
            WORD wVersionRequested;
            WSADATA wsaData;
            wVersionRequested = MAKEWORD (1, 1);
            int status_startup = ::WSAStartup (wVersionRequested, &wsaData);

            if (status_startup != 0)
            {
                CDMW_THROW( InternalErrorException );
            }

            // No defined constant
#   define MAX_HOSTNAME 1024

            char hostName[MAX_HOSTNAME];

            int status_hostname = ::gethostname (hostName, MAX_HOSTNAME);

            if (status_hostname != 0)
            {

                CDMW_THROW( InternalErrorException );

            }

            ::WSACleanup();

            return hostName;
#else
#    error "Cdmw: implementation not defined"
#endif
        }

        //
        // Return the name of the user logged in on the controlling
        // terminal of the process
        //
        std::string
        OS::get_logname()
        throw( InternalErrorException )
        {
            
#if defined(CDMW_POSIX)
           char * logname = ::getenv("LOGNAME");

           if (!logname)
           {
              CDMW_THROW( InternalErrorException );
           }

           return logname;
#else
#    error "Cdmw: implementation not defined"
#endif
        }

            

        //
        // Return the current time in sec/microsec from 1st Jan 1970
        //
        OS::Timeval
        OS::get_time()
        throw( InternalErrorException )
        {

            OS::Timeval os_timeval;

#if defined(CDMW_POSIX)


            struct timeval the_timeval;
            int status = ::gettimeofday (&the_timeval, NULL);

            if (status != 0)
            {
                CDMW_THROW(InternalErrorException);
            }

            os_timeval.seconds = the_timeval.tv_sec;
            os_timeval.microseconds = the_timeval.tv_usec;



#elif defined(_WIN32)

            // This implementation comes from the ACE Framework
            //  see ACE_Time_Value object

            FILETIME systemTime;
            ::GetSystemTimeAsFileTime (&systemTime);

            DWORDLONG unix_win_time_skew = 0x19db1ded53e8000L;

            ULARGE_INTEGER _100ns = { systemTime.dwLowDateTime,
                                      systemTime.dwHighDateTime
                                    };

            _100ns.QuadPart -= unix_win_time_skew;

            // Convert 100ns units to seconds;
            os_timeval.seconds = static_cast<long>(_100ns.QuadPart / (10000 * 1000));

            // Convert remainder to microseconds;
            os_timeval.microseconds = static_cast<long> ((_100ns.LowPart % (DWORD) (10000 * 1000)) / 10);


#else
#    error "Cdmw: implementation not defined"
#endif

            return os_timeval;
        }



        void
        OS::get_fmt_time (char *p_timeBuffer, size_t timeBufferSize,
                          unsigned long timeSec, char *p_timeFormat,
                          TimeMode mode)
        throw (InternalErrorException)
        {

            struct tm *p_today;

            struct tm today;
            time_t timer_value = timeSec;

            // according to time mode display select gmt or local

            if (mode == GMT_TIME)
            {
                p_today = ::gmtime (&timer_value);
            }
            else
            {
                p_today = ::localtime (&timer_value);
            }

            today = *p_today;

            // format time
            int status = ::strftime (p_timeBuffer, timeBufferSize, p_timeFormat, &today);

            if (status == 0)
            {
                CDMW_THROW(InternalErrorException);
            }

        }


        //
        // return a handler on the dynamic library
        //


        OS::SharedObjectHandle*
        OS::dlopen(const char *libname, int flag)
        throw()
        {
            return EffectiveSharedObject::dlopen(libname, flag);
        };


        //
        // close the dynamic library
        //

        int
        OS::dlclose(OS::SharedObjectHandle *handle)
        throw()
        {
            return handle->dlclose();
        };


        //
        // return the error title produce by dlopen, dlclose and dlsym function
        //

        std::string
        OS::dlerror(void)
        throw()
        {
            return EffectiveSharedObject::dlerror();
        };


        //
        // retrun a handler on the function
        //

        OS::FunctionHandle
        OS::dlsym(OS::SharedObjectHandle* handle, const char * symname)
        throw()
        {
            return handle->dlsym(symname);
        };


    } // End namespace ossupport

} // End namespace Cdmw

