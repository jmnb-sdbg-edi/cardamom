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


#include "ossupport/ProcessNotifierThread.hpp"

#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/Condition.hpp"

#include "ossupport/OS_P.hpp"

#if defined(CDMW_POSIX)
#   include <sys/types.h>
#   include <sys/wait.h>
#   include <signal.h>
#endif


namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Implementation notes: none 
        *<p>
        *Portability issues: none 
        *<p>
        */


        //
        // Ctor
        //
        ProcessNotifierThread::ProcessNotifierThread()
        throw()
                : m_processes_condition(m_processes_mutex), m_thread_must_exit(false)
        {
#   ifdef _WIN32



            m_event = ::CreateEvent( NULL,   // No security descriptor
                                     FALSE,  // Auto-reset
                                     FALSE,  // Creation as non-signalled
                                     NULL   // Event without name
                                   );
#   endif

        }




        //
        // Dtor
        //
        ProcessNotifierThread::~ProcessNotifierThread()
        throw()
        {
            // We unlock the thread
            m_thread_must_exit = true;

#if defined( CDMW_POSIX )

            /**
                FIXME: interupting the thread !!!
                        when blocked in the wait system call
                struct sigvec oldvec;
                struct sigvec newvec;
                newvec.sv_flags = SV_INTERRUPT;
             
                int ::sigvec(SIG_USR2, &newvec, &oldvec);
                    ::signal
             
            */
            m_processes_condition.broadcast();


#elif defined(_WIN32)

            ::SetEvent( m_event );

#   endif

            // And wait it's completion
            this->join();

        }



        //
        // Call in a new thread by the Thread sub-system
        //
        void
        ProcessNotifierThread::run()
        throw()
        {

#   ifdef _WIN32

            size_t nbHandle;

            HANDLE* pHandle;
            PROCESS_INFORMATION* pProcessInformation;


            while ( ! m_thread_must_exit )
            {


                // We built the list of handle of each process, we must waiting on
                //  NB: The handle 0 is the event
                build_handle_list( pHandle, pProcessInformation, nbHandle );
                size_t iSignalled;


                DWORD waitStatus =
                    ::WaitForMultipleObjects( nbHandle,
                                              pHandle,
                                              FALSE,         // If one of the object is signaled we exit
                                              INFINITE      // No timeout
                                            );

                // We compute the index of the signalled object
                iSignalled = waitStatus - WAIT_OBJECT_0;


                // If it is not only the Event has been signalled

                if ( iSignalled != 0)
                {

                    OS::ProcessId signalledPid
                    = pProcessInformation[iSignalled].dwProcessId;

                    // TODO: get exit code
                    ::CloseHandle( pProcessInformation[iSignalled].hProcess );
                    ::CloseHandle( pProcessInformation[iSignalled].hThread );

                    ProcessCallback *endingCallback = wait_creation(pProcessInformation[iSignalled]);

                    if (endingCallback != NULL)
                        endingCallback->execute(signalledPid);


                }

                free_handle_list( pHandle, pProcessInformation, nbHandle );

            }


#   elif defined (CDMW_POSIX)

            while ( ! m_thread_must_exit )
            {

                // We only need to wait
                // FIXME: wait may also return if the process has been stopped
                //  check status
                int status;
                int pid = ::wait( &status );

                // If we have not been interrupted, a process is died

                if ( pid != -1)
                {

                    ProcessCallback *endingCallback = wait_creation(pid);

                    if (endingCallback != NULL)
                        endingCallback->execute(pid);

                }

            }

#   else
#       error Cdmw: Not supported platform
#   endif
        }


#ifdef _WIN32 
        //
        // Add the specified ID to the list of processes
        //  to be monitored
        //
        void
        ProcessNotifierThread::add_process(PROCESS_INFORMATION processInformation,
                                           ProcessCallback* endingCallback)
        throw()

#elif defined (CDMW_POSIX)

        void
        ProcessNotifierThread::add_process(OS::ProcessId processInformation,
                                           ProcessCallback* endingCallback)
        throw()

#endif
        {

            {

                CDMW_MUTEX_GUARD(m_processes_mutex);

                m_processes[processInformation] = endingCallback;

                m_processes_condition.broadcast();

            }

#if defined( _WIN32 )

            // We notify the waiting thread there is a new process to monitor

            ::SetEvent( m_event );

#endif

        }


#ifdef _WIN32

        ProcessCallback*
        ProcessNotifierThread::wait_creation(PROCESS_INFORMATION processInformation)

#elif defined (CDMW_POSIX)

        ProcessCallback*
        ProcessNotifierThread::wait_creation(OS::ProcessId processInformation)

#endif
        {

            bool exited = false;

            ProcessCallback *endingCallback = NULL;

            CDMW_MUTEX_GUARD(m_processes_mutex);

            Processes::iterator it;

            while (!exited)
            {
                it = m_processes.find(processInformation);

                if (it != m_processes.end())
                    break;

                m_processes_condition.wait();

                if (m_thread_must_exit)
                    exited = true;
            }

            if (!exited)
            {
                endingCallback = it->second;
                m_processes.erase(processInformation);
            }

            return endingCallback;

        }


#if defined( _WIN32 )

        //
        // Return the list of handle to be monitored associated to the
        //  started process.
        //
        void
        ProcessNotifierThread::build_handle_list(
            HANDLE*& pHandle,
            PROCESS_INFORMATION*& pProcessInformation,
            size_t& nbHandle)
        throw()
        {

            CDMW_MUTEX_GUARD(m_processes_mutex);

            // We allocate enough memory for all process
            // +1 for the Event
            nbHandle = m_processes.size() + 1;
            pHandle = new HANDLE[ nbHandle ];
            pProcessInformation = new PROCESS_INFORMATION[ nbHandle ];

            pHandle[0] = m_event;

            size_t iProcess = 1;

            for ( ProcessesCtr::const_iterator itrProcess = m_processes.begin();
                    itrProcess != m_processes.end(); ++itrProcess )
            {

                pHandle[iProcess] = itrProcess->hProcess;
                pProcessInformation[iProcess] = *itrProcess;

                ++iProcess;

            }

        }

        void
        ProcessNotifierThread::free_handle_list(HANDLE* pHandle, PROCESS_INFORMATION* pProcessInformation, size_t nbHandle)
        throw()
        {

            delete [] pHandle;
            delete [] pProcessInformation;

        }


#   endif

    } // End namespace OsSupport
} // End namespace Cdmw


// To allow the insertion, of PROCESS_INFORMATION inside the list

#   ifdef _WIN32
bool
operator==(PROCESS_INFORMATION lhs, PROCESS_INFORMATION rhs)
{

    return lhs.hProcess == rhs.hProcess;
}

#   endif


