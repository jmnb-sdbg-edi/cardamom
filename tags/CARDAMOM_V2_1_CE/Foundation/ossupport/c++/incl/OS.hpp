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


#ifndef INCL_OSSUPPORT_OS_HPP
#define INCL_OSSUPPORT_OS_HPP

#include <string>
#include <list>
#include <map>
#include <stdexcept>
#include "Foundation/common/MacroDefs.h"

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Thread.hpp"

#if defined(CDMW_POSIX)

#include <sys/types.h>
#include <unistd.h>

#endif


#ifdef AIX
#   include <Foundation/ossupport/AIXSharedObject.hpp>
#elif linux
#   include <Foundation/ossupport/LinuxSharedObject.hpp>
#elif _WIN32
#   include <Foundation/ossupport/Win32DllObject.hpp>
#   define RTLD_LAZY 1
#else
#   include <Foundation/ossupport/DefaultSharedObject.hpp>
#endif



namespace Cdmw
{

    namespace OsSupport
    {

        // Forward of interface to be used to be notified about the process life

        class ProcessCallback;

        class ProcessNotifierThread;

        class MonitorGarbageCollector;

        class ProcessMonitor;

        /**
        *Purpose:
        *<p>    This class provides several helper tool
        *       to execute some system level operation.
        *       All functions of this class are static.
        *
        *Features:
        *<p> Thread safe, exception safe
        *
        */

        class OS
        {

            public:

                /**
                * Purpose:
                *<p> Identify a process on the host.
                *
                */
#       if defined(CDMW_POSIX)

                typedef int ProcessId;
#       elif defined( _WIN32)

                typedef unsigned long ProcessId;
#       endif


                /**
                * Purpose:
                *<p> Value used by dl functions (dlopen, dlclose, dlsym and dlerror)
                *
                */



#ifdef AIX
                typedef SharedObjectHandle_T<AIXSharedObjectHandle> SharedObjectHandle;
                typedef AIXSharedObject::FunctionHandle FunctionHandle;
#elif _WIN32
                typedef SharedObjectHandle_T<Win32DllObjectHandle> SharedObjectHandle;
                typedef Win32DllObject::FunctionHandle FunctionHandle;
#elif linux
                typedef SharedObjectHandle_T<LinuxSharedObjectHandle> SharedObjectHandle;
                typedef LinuxSharedObject::FunctionHandle FunctionHandle;
#else
                typedef SharedObjectHandle_T<DefaultSharedObjectHandle> SharedObjectHandle;
                typedef DefaultSharedObjectHandle::FunctionHandle FunctionHandle;
#endif





                /**
                * Purpose:
                * <p> Value returned by a process after completion.
                *
                */
                typedef int StatusProcess;

                /**
                * Contains time mode display : gmt or local
                */
                enum TimeMode
                {
                    /**
                    * Time is displayed as gmt
                    */
                    GMT_TIME,

                    /**
                    * Time is displayed as local
                    */
                    LOCAL_TIME
                };

                /**
                * Purpose:
                * <p> This structure is used to express the current time.
                * The contained values represent the seconds and 
                * microseconds elapsed since 01/01/1970.
                *
                */

                struct Timeval
                {
                    /**
                    * second part of current time
                    */
                    long seconds;

                    /**
                    * microsecond part of current time
                    */
                    long microseconds;
                    
                    Timeval()
                    :seconds(0)
                    ,microseconds(0)
                    {
                    }
                   
                    Timeval(long pSec, long pmSec)
                    :seconds(pSec)
                    ,microseconds(pmSec)
                    {
                    }
 
                    Timeval(const Timeval& tv)
                    :seconds(tv.seconds)
                    ,microseconds(tv.microseconds)
                    {
                    }
                    
                    Timeval& operator=(const Timeval& tv)
                    {
                        seconds = tv.seconds;
                        microseconds = tv.microseconds;
                        return *this;                        
                    }
                    
#ifndef         _WIN32
                    Timeval(const timeval& tv )
                    :seconds(tv.tv_sec)
                    ,microseconds(tv.tv_usec)
                    {
                    }
#else
#error  Please add implementation
#endif

                    bool operator==(const Timeval& tv) const
                    {
                        if (( tv.seconds == this->seconds )&& ( tv.microseconds == this->microseconds ) )
                        {
                            return true;
                        }
                        return false;
                    }

                    bool operator>(const Timeval& tv) const
                    {
                        if ( tv.seconds < this->seconds )
                        {
                            return true;
                        }
                        else if ( tv.seconds > this->seconds )
                        {
                            return false;
                        }
                        else if ( tv.microseconds < this->microseconds )
                        {
                            return true;
                        }
                        return false;
                    }

                    bool operator<(const Timeval& tv) const
                    {
                        return ((tv > (*this)) && (!(tv == (*this))));
                    }
                };


                /**
                * Purpose:
                * <p> This functions returned the value of an option passed
                * on the command line with the format: --option=value, or
                * --option. If no value are specified after an option "yes"
                * is returned if the option is present, "no" elsewhere.
                * 
                * @param argc argc argument as received in the main function
                * @param argv argv argument as received in the main function
                * @param option option to test if it is present on the command line
                *
                * @return the value of the option, "yes" if the option is present
                *         without value, "no" if the option is not present.
                * 
                * @exception OutOfMemoryException
                * @exception InternalErrorException        
                */
                static
                std::string
                get_option_value (int argc, char** argv, const std::string& option)
                throw (OutOfMemoryException,
                       InternalErrorException);

                /**
                * Purpose:
                * <p> The calling thread is blocked during the specified
                * time expressed in milliseconds.
                *
                * @param milliseconds time to sleep expressed in 
                *        milliseconds
                *
                * @exception InterruptedException
                * @exception InternalErrorException        
                */
                static
                void
                sleep (unsigned int milliseconds)
                throw (InterruptedException,
                       InternalErrorException);


                /**
                * Purpose:
                * <p> The calling is blocked during the specified
                * time expressed in seconds and nanoseconds.
                *
                * @param seconds time to sleep expressed in
                *        seconds
                * @param nanoseconds time to sleep expressed in 
                *        nanoseconds
                * @exception range error if seconds or nanoseconds 
                *           are out of range (i.e. seconds is negative or 
                *           nanoseconds is not in [0,999 999 999].
                */
                static
                void
                sleep (unsigned int seconds,
                       unsigned int nanoseconds)
		throw (std::range_error);

                /**
                * Purpose:
                * <p> Test if a file exists 
                *
                * @param path path to the file to test for existence
                *
                * @return true if the file exists, false elsewhere
                */
                static
                bool
                file_exists (const std::string& path)
                throw();


                /**
                * Purpose:
                * <p> Delete the specified file.
                *
                * @param path path to the file to be deleted
                * 
                * @exception PermissionDeniedException the caller has not enough
                *            right to delete the file.
                * @exception InterruptedException a signal has been received and
                *            has interrupted the operation
                * @exception BadParameterException the specified path is not valid
                * @exception InternalErrorException
                */
                static
                void
                unlink(const std::string& path)
                throw (PermissionDeniedException,
                       InterruptedException,
                       BadParameterException,
                       InternalErrorException);
                /**
                * Purpose:
                * <p> Change the current working directory
                *
                * @param path path to the directory
                * 
                * @exception BadParameterException the specified path is not valid
                * @exception PermissionDeniedException the caller has not enough
                *            right to access the directory
                * @exception InterruptedException a signal has been received and
                *            has interrupted the operation
                * @exception InternalErrorException
                */
                static
                void
                chdir (const std::string& path)
                throw (BadParameterException,
                       PermissionDeniedException,
                       InterruptedException,
                       InternalErrorException);

                /**
                * Purpose:
                * <p> create a new directory
                *
                * @param path name of the directory
                * 
                * @exception BadParameterException the specified path is not valid
                * @exception PermissionDeniedException the caller has not enough
                *            right to access the directory
                * @exception InterruptedException a signal has been received and
                *            has interrupted the operation
                * @exception InternalErrorException
                */
                static
                void
                mkdir (const std::string& path, unsigned short mode)
                throw (BadParameterException,
                       PermissionDeniedException,
                       InterruptedException,
                       InternalErrorException);

                /**
                * Purpose:
                * <p> Return the absolute path associated to the current
                *   working directory and specified path
                *
                * @param path path to the file
                * 
                * @exception BadParameterException     the specified path is not valid
                * @exception PermissionDeniedException the caller has not enough
                *                                      right to access the directory
                * @exception NotExistException         the specified path does not exist
                * @exception InternalErrorException
                */
                static
                std::string
                get_absolute_path (const std::string& path)
                throw (BadParameterException,
                       PermissionDeniedException,
                       NotExistException,
                       InternalErrorException);


#       if defined(CDMW_POSIX)

                /**
                * Purpose:
                * <p> Sets the attributes for created processes.
                *
                * @param uid the user id
                * @param gid the group id
                */
                static void set_process_attributes(uid_t uid, gid_t gid);


                /**
                 * Purpose:
                 * <p> Get the uid attribute
                 *
                 * @return  the uid
                 */
                 static uid_t get_uid();


                /**
                 * Purpose:
                 * <p> Get the gid attribute
                 *
                 * @return  the uid
                 */
                 static gid_t get_gid();

                /**
                 * Purpose:
                 * <p> change the owner of a file
                 *
                 * @exception BadParameterException
                 * @exception OutOfMemoryException
                 * @exception InternalErrorException
                 * 
                 */
                 static void chown(const std::string& path, uid_t uid, gid_t gid)
                     throw (PermissionDeniedException,
                            BadParameterException,
                            OutOfMemoryException,
                            InternalErrorException);
            
#       endif


                /**
                * Purpose:
                * <p> Create a new process from the specified executable
                *
                * @param path path to the executable
                * @param args arguments to pass to the process
                * @param home directory where the process must be started
                * @param creationCallback the callback that will be executed just after
                * the process creation.
                * @param endingCallback the callback that will be executed just after
                * the process ending.
                *
                * @return the process id of the newly created process
                *
                * @exception BadParameterException
                * @exception OutOfMemoryException
                * @exception InternalErrorException
                */
                static
                ProcessId
                create_process (const std::string& path, const std::string& args,
                                const std::string& home = "",
                                ProcessCallback* creationCallback = NULL,
                                ProcessCallback* endingCallback = NULL)
                throw (BadParameterException,
                       OutOfMemoryException,
                       InternalErrorException);


                /**
                * Purpose:
                * <p> Actually create a new process from the specified executable
                *
                * @param path path to the executable
                * @param args arguments to pass to the process
                * @param home directory where the process must be started
                * @param creationCallback the callback that will be executed just after
                * the process creation.
                * @param endingCallback the callback that will be executed just after
                * the process ending.
                *
                * @return the process id of the newly created process
                *
                * @exception BadParameterException
                * @exception OutOfMemoryException
                * @exception InternalErrorException
                */
                static
                ProcessId
                _create_process (const std::string& path, const std::string& args,
                                 const std::string& home = "",
                                 ProcessCallback* creationCallback = NULL,
                                 ProcessCallback* endingCallback = NULL)
                throw (BadParameterException,
                       OutOfMemoryException,
                       InternalErrorException);

                /**
                * Purpose:
                * <p> Kill a process. The processId is the value
                *    return by OS::create_process()
                *
                  * @param processId id of the process to be killed
                   * @param args arguments to pass to the process
                *
                * @exception BadParameterException the process id is invalid
                * @exception PermissionDeniedException the user has not enough
                *    right to kill this process
                * @exception InternalErrorException
                */
                static
                void
                kill_process (OS::ProcessId processId)
                throw (BadParameterException,
                       PermissionDeniedException,
                       InternalErrorException);

                /**
                * Purpose:
                * <p> Blocks the calling thread until the completion of
                *    the process whose process id is specified
                *
                * @param processId id of the process to be waited for
                *
                * @return exit value of the process
                *
                * @exception BadParameterException the process id is not valid
                * @exception OutOfMemoryException
                * @exception InternalErrorException
                */
                static
                StatusProcess
                wait_process (OS::ProcessId processId)
                throw (BadParameterException,
                       InterruptedException,
                       InternalErrorException);

                /**
                * Purpose:
                * <p> Returns the number of sec/microsec since 1th of Jan 1970
                * 
                * @return  the current date in sec/microsec
                *
                * @exception InternalErrorException
                */
                static
                Timeval
                get_time()
                throw (InternalErrorException);

                /**
                * Purpose:
                * <p> Returns the current formatted date
                * 
                * @param  p_timeBuffer pointer to formatted time buffer
                *         where the string formatted time is returned
                * @param  timeBufferSize size of formatted time buffer
                * @param  timeSec time in seconds since 1th of Jan 1970
                * @param  format to display data
                * @param  mode : selection of gmt or local time
                *
                * @exception InternalErrorException
                */
                static
                void
                get_fmt_time(char *p_timeBuffer, size_t timeBufferSize,
                             unsigned long timeSec, char *p_timeFormat,
                             TimeMode mode)
                throw (InternalErrorException);


                /**
                * Purpose:
                * <p> Returns the host name
                * 
                * @return  the host name
                *
                * @exception InternalErrorException
                */
                static
                std::string
                get_hostname()
                throw (InternalErrorException);

                /**
                * Purpose:
                * <p> Returns the name of the user logged in on the controlling
                * terminal of the process
                * 
                * @return  the user name
                *
                * @exception InternalErrorException
                */
                static
                std::string
                get_logname()
                throw (InternalErrorException);


                /**
                * Purpose:
                * <p> Returns the id of the process
                * 
                * !!!WARNING!!! : on Linux OS the return pid corresponds to the 
                *                 thread id of the calling thread. In this case,
                *                 threads are managed as processes by OS
                *                 
                * @return  the process id
                *
                */
                static
                ProcessId
                get_processId();



                /**
                * Policy to create process
                */
                enum CreateProcessPolicy {

                    /**
                    * The process is created by thread requesting the creation
                    * An unique ProcessNotifier thread is in charge to
                    * wait termination of created processes.
                    */
                    CREATE_BY_THE_CALLER,

                    /**
                    * The process is created by an associated created thread, the
                    * ProcessMonitor which is charge after the requested process
                    * creation to wait its termination.
                    * There are ProcessMonitor threads as much as created processes.
                    */
                    CREATE_BY_MONITOR_THREADS,

                    /**
                    * The process is created by the unique process monitor thread, 
                    * which is charge after the creation of processes 
                    * to wait their termination.
                    */
                    CREATE_BY_THE_MONITOR
                };


                /**
                * Purpose:
                * <p> Set the creation process policy
                * 
                *
                */
                static
                void
                setUpCreateProcessPolicy (CreateProcessPolicy policy);


                /**
                * Purpose:
                * <p> This function creates, if necessary, the thread in
                *   charge of destruction of ProcessMonitor
                */
                static
                void
                create_MonGarbageCollectorThread()
                throw();



                /**
                * Purpose:
                * <p> Destroying the garbageCollector thread, when the process is
                *   near to exit.
                */
                static
                void
                cleanup_MonGarbageCollectorThread()
                throw();


                /**
                * Purpose:
                * <p> Insert started ProcessMonitor into garbageCollector
                */
                static
                void OS::addStartedProcessMonitor(ProcessMonitor* p_processMonitor)
                throw(OutOfMemoryException);


                /**
                * Purpose:
                * <p> Insert terminated ProcessMonitor into garbageCollector
                */
                static
                void OS::addTerminatedProcessMonitor(ProcessMonitor* p_processMonitor)
                throw(OutOfMemoryException);


                /**
                * Purpose:
                * <p> Open a dynamic library
                * @param name of the dynamic library
                * return a handler on the dynamic library, if not NULL
                */
                static
                SharedObjectHandle*
                dlopen(const char* libname, int flag = RTLD_LAZY)
                throw();

                /**
                * Purpose:
                * <p> Close a dynamic library
                * @param handle of the dynamic library
                * return 0 if the library is closed without problem, if not 1 
                */
                static
                int
                dlclose(SharedObjectHandle* handle)
                throw();

                /**
                * Purpose:
                * <p> Search a function in the dynamic library specified
                * @param handle of the dynamic library and the name of the function
                * return a handler on this function, if not NULL
                */
                static
                FunctionHandle
                dlsym(SharedObjectHandle* handle, const char* symname)
                throw();

                /**
                * Purpose:
                * <p> return a description of the error generated by dlopen,
                * dlclose and dlsym function
                * 
                * return a string with a description of the error
                */
                static
                std::string
                dlerror(void)
                throw();


                /**
                * Purpose:
                * <p> Test the validity of the argument to be used to
                *    create a new process
                *
                * @param path path to the executable file
                * @param args arguments to be passed to the process
                * 
                */
                static
                void
                check_exec_validity (const std::string& path,
                                     const std::string args)
                throw (BadParameterException);



            protected:

                /**
                * Purpose:
                * <p> replace the process image with a new image
                *
                * @param path path to the executable file
                * @param args arguments to be passed to the process
                * 
                */
                static
                void
                exec_process(const std::string& path, const std::string& args)
                throw ();



                /**
                * Purpose:
                * <p> This function creates, if necessary, the thread in
                *   charge of notifying subscribing of the process created 
                *   life.
                */
                static
                void
                create_processNotifierThread()
                throw();


           // Workaround for PCR-0562:
           // Daemon needs to explicitly call cleanup_processNotifierThread
           // before its ORB shutdown.
           public:
           
                /**
                * Purpose:
                * <p> Destroying the monitoring thread, when the process is
                *   near to exit.
                */
                static
                void
                cleanup_processNotifierThread()
                throw();



            private:


                //
                // Ctor
                //
                OS();



                //
                // Copy ctor
                //
                OS(const OS& rhs);

                //
                // Assignement operator
                //
                OS&
                operator=(const OS& rhs);


                //
                // Dtor
                //
                virtual
                ~OS();




                /**
                * process notifier reference
                */
                static ProcessNotifierThread* M_pProcessNotifierThread;

                /**
                * Purpose:
                * The mutex protecting M_pProcessNotifierThread.
                */
                static Mutex M_processNotifierThread_mutex;

                /**
                * Purpose:
                * The user id used for created processes.
                */
                static int M_uid;

                /**
                * Purpose:
                * The group id used for created processes.
                */
                static int M_gid;


                /**
                * Purpose:
                * policy used to create processes.
                */
                static CreateProcessPolicy M_createProcessPolicy;


                /**
                * garbage collector reference
                */
                static MonitorGarbageCollector* M_garbageCollector;

                /**
                * Purpose:
                * The mutex protecting M_garbageCollector.
                */
                static Mutex M_garbageCollector_mutex;



        }

        ; // End class OS

    } // End namespace ossupport
} // End namespace Cdmw

#endif // INCL_OSSUPPORT_OS_HPP

